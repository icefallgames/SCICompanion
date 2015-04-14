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

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXTDOCKBAR_H)
	#include "ExtDockBar.h"
#endif

#if( !defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

//#if (!defined __EXT_MFC_NO_RIBBON_BAR)
//	#if (!defined __EXT_RIBBON_BAR_H)
//		#include <ExtRibbonBar.h>
//	#endif // (!defined __EXT_RIBBON_BAR_H)
//#endif

#include <Resources/Resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define __MENUBAR_RIGHT_ADJUST_GAP 1

bool CExtMenuControlBar::g_bHideMenuBarExpandContentButton = false;
bool CExtMenuControlBar::g_bCancelNearestAltUp = false;
bool CExtMenuControlBar::g_bKeyCodeTracking = false;
const UINT CExtMenuControlBar::g_nMsgPrepareSystemMenu =
	::RegisterWindowMessage(
		_T("CExtMenuControlBar::g_nMsgPrepareSystemMenu")
		);
bool CExtMenuControlBar::g_bTranslateContextHelpCmd = false;
ACCEL CExtMenuControlBar::g_accelContextHelp = { FVIRTKEY, VK_F1, ID_HELP };

/////////////////////////////////////////////////////////////////////////////
// CExtMenuControlBar hook sink

bool CExtMenuControlBar::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
__PROF_UIS_MANAGE_STATE;
	if(		GetSafeHwnd() == NULL
		||	CWnd::FromHandlePermanent(GetSafeHwnd()) == NULL
		||	_DraggingGetBar() != NULL
		)
		return 
			CExtHookSink::OnHookWndMsg(
				lResult,
				hWndHooked,
				nMessage,
				wParam,
				lParam
				);
HWND hWndMainFrame = NULL;
CWnd * pWndForPlacement = NULL;
	if( ! m_bPresubclassDialogMode )
	{
		hWndMainFrame = _GetHwndMainFrame();
		if( hWndMainFrame != NULL )
			pWndForPlacement = stat_GetWndForPlacement( CWnd::FromHandle(hWndMainFrame) );
	} // if( ! m_bPresubclassDialogMode )
	if(		(		(!m_bPresubclassDialogMode)
				&&	(	(	hWndMainFrame != NULL
						&&	hWndHooked == hWndMainFrame
						)
					||	(	pWndForPlacement != NULL
						&&	hWndHooked == pWndForPlacement->m_hWnd
						)
					)	
				)
		||	(		m_bPresubclassDialogMode
				&&	hWndHooked == _GetHwndPlacement()
				)
		)
	{ // if window is a main frame or parent dialog-like
		if( nMessage == WM_PARENTNOTIFY
			&&
			CExtPopupMenuWnd::IsMenuTracking()
			)
		{
			lResult = 0;
			return true;
		}

//		if( nMessage == WM_INITMENU )
//		{
//			CFrameWnd * pFrame = _GetDockingFrameImpl();
//			if( pFrame != NULL )
//			{
//				HMENU hSysMenu = ::GetSystemMenu( hWndHooked, FALSE );
//				HMENU hCurMenu = GetSubMenu( (HMENU)wParam, 0 );
//				CPoint ptCursor;
//				if(		hCurMenu == hSysMenu
//					&&	hCurMenu != NULL
//					&&	::GetCursorPos(&ptCursor)
//					)
//				{
//					::SetForegroundWindow( hWndHooked ); 
//					if( TrackMainFrameSystemMenu( &ptCursor, FALSE ) )
//						return true;
//				}	
//			} // if( pFrame != NULL )
//		} // if( nMessage == WM_INITMENU )

//		if( nMessage == WM_INITMENU )
//		{
//			CFrameWnd * pFrame = _GetDockingFrameImpl();
//			if( pFrame != NULL )
//			{
//				HMENU hSysMenu = ::GetSystemMenu( hWndHooked, FALSE );
//				HMENU hCurMenu = GetSubMenu( (HMENU)wParam, 0 );
//				if(		hCurMenu == hSysMenu
//					&&	hCurMenu != NULL
//					)
//				{
//					CMenu * pSysMenu = CMenu::FromHandle( hSysMenu );
//					UINT nEnable = 
//						CExtPopupMenuWnd::g_bFullScreenMode
//							? MF_GRAYED
//							: MF_ENABLED
//							;
//					pSysMenu->EnableMenuItem( 
//						SC_MOVE,
//						nEnable|MF_BYCOMMAND
//						); 
//					pSysMenu->EnableMenuItem( 
//						SC_SIZE,
//						nEnable|MF_BYCOMMAND
//						); 
//					pSysMenu->EnableMenuItem( 
//						SC_MAXIMIZE,
//						nEnable|MF_BYCOMMAND
//						); 
//				}	
//			} // if( pFrame != NULL )
//		} // if( nMessage == WM_INITMENU )

		if(		nMessage == WM_SIZE
			||	nMessage == WM_ACTIVATEAPP
			||	(	nMessage == WM_COMMAND
				&&	(	((HIWORD(wParam))==0)
					||	((HIWORD(wParam))==1)
						)
				)
			||	nMessage == WM_SYSCOMMAND
			)
		{
			m_bHelperTrackingCharCode = false;
			if( _IsNonCommandButtonMode() )
				return
					CExtHookSink::OnHookWndMsg(
						lResult,
						hWndHooked,
						nMessage,
						wParam,
						lParam
						);
			HWND hWndCapture = CExtMouseCaptureSink::GetCapture();
			if( hWndCapture == GetSafeHwnd() )
				CExtMouseCaptureSink::ReleaseCapture();
			else if( hWndCapture != NULL )
			{
				CWnd * pWnd = FromHandlePermanent(hWndCapture);
				if( pWnd != NULL
					&& pWnd->GetSafeHwnd() != NULL
					&& pWnd->IsKindOf(RUNTIME_CLASS(CExtControlBar))
					)
					CExtMouseCaptureSink::ReleaseCapture();
			}

			_CancelFlatTracking();
			if( CExtMenuControlBar::g_bMenuTracking )
				CExtMenuControlBar::_CloseTrackingMenus();
			//if(	CExtPopupMenuWnd::IsMenuTracking() )
			//	CExtPopupMenuWnd::CancelMenuTracking();
			return CExtHookSink::OnHookWndMsg( lResult, hWndHooked, nMessage, wParam, lParam );
		}

		if( ! m_bPresubclassDialogMode )
		{
//			if( nMessage == WM_NCLBUTTONDBLCLK && wParam == HTSYSMENU )
//			{
//				if( hWndHooked == _GetHwndMainFrame() )
//				{
//					::SendMessage( hWndHooked, WM_SYSCOMMAND, SC_CLOSE, lParam );
//					if( ::IsWindow( hWndHooked ) )
//						::SendMessage( hWndHooked, WM_NCPAINT, 0, 0L );
//					lResult = 0L;
//					return true;
//				}
//			}
//			if( nMessage == WM_NCLBUTTONDOWN && wParam == HTSYSMENU )
//			{
//				if( hWndHooked == _GetHwndMainFrame() )
//				{
//					CExtPopupMenuWnd::CancelMenuTracking();
//					bool bDblClick = false;
//					static clock_t g_nLastClock = 0;
//					static HWND g_hWndLastClick = NULL;
//					clock_t nCurrClock = ::clock();
//					if( g_hWndLastClick == hWndHooked )
//					{
//						clock_t nDiff = (clock_t)::abs( nCurrClock - g_nLastClock );
//						clock_t nDoubleClickTime = (clock_t)::GetDoubleClickTime();
//						if( nDiff <= nDoubleClickTime )
//							bDblClick = true;
//					}
//					g_hWndLastClick = hWndHooked;
//					g_nLastClock = nCurrClock;
//					if( bDblClick )
//					{
//						::SendMessage( hWndHooked, WM_NCLBUTTONDBLCLK, wParam, lParam );
//						lResult = 0;
//						return true;
//					}
//					if( ! m_bAllowProfMainFrameSysmenu )
//						return false;
//				}
//			}
			if(		( nMessage == WM_NCLBUTTONUP && wParam == HTSYSMENU )
				||	( nMessage == WM_NCRBUTTONUP && ( wParam == HTCAPTION || wParam == HTSYSMENU ) )
				)
			{
				CExtToolControlBar::g_bMenuTrackingExpanded = false;
				CPoint * pPoint = NULL, ptCursor;
				if( nMessage == WM_NCRBUTTONUP )
				{
					if( ! ::GetCursorPos(&ptCursor) )
					{
						lResult = 0;
						return true;
					}
					pPoint = &ptCursor;
				}
				if( !_IsDockSiteCustomizeMode() )
				{
					if( ! TrackMainFrameSystemMenu( pPoint, FALSE ) )
						return false;
				} // if( !_IsDockSiteCustomizeMode() )
				lResult = 0L;
				return true;
			}
			if(		( nMessage == WM_NCLBUTTONDOWN || nMessage == WM_NCRBUTTONDOWN )
				&&	wParam == HTSYSMENU
				)
			{
				lResult = 0;
				if(		( ! m_bAllowProfMainFrameSysmenu )
					&&	hWndHooked == _GetHwndMainFrame()
					)
					return false;
				if(		(! m_bAllowProfChildFrameSysmenu )
					&&	_IsMdiApp()
					&&	hWndHooked == _GetHwndChildFrame()
					)
					return false;
				return true;
			}
			if( nMessage == WM_NCRBUTTONDOWN && wParam == HTCAPTION )
			{
				if( !_IsDockSiteCustomizeMode() )
				{
					CExtToolControlBar::g_bMenuTrackingExpanded = false;
					CPoint ptCursor;
					if( ::GetCursorPos(&ptCursor) )
						if( ! TrackMainFrameSystemMenu( &ptCursor, FALSE ) )
							return false;
				} // if( !_IsDockSiteCustomizeMode() )
				lResult = 0;
				return true;
			}
		
			if( nMessage == CExtPopupMenuWnd::g_nMsgNotifyMenuClosed )
			{
				m_bSysMenuTracking = false;
				return false;
			}
		} // if( !m_bPresubclassDialogMode )

		bool bPrintPreviewMode = false;
		if(		(!m_bPresubclassDialogMode)
			&&	m_pDockSite->GetSafeHwnd() != NULL
			&&	FindPrintPreviewMode(m_pDockSite)
			)
			bPrintPreviewMode = true;

		if(		nMessage == CExtPopupMenuWnd::g_nMsgPopupNext
			||	nMessage == CExtPopupMenuWnd::g_nMsgPopupPrev
			)
		{ // if menu next/prev popup activation messages
			BOOL bDummyMax = FALSE;
			HWND hWndMdiChild = _GetActiveMdiChildWnd(bDummyMax);

			int iTrackingIndex =
				GetMenuTrackingButton();
			if( (!bPrintPreviewMode) && iTrackingIndex < 0 ) // menu tracked on other bar
			{
				if( m_bSysMenuTracking )
				{
					if(		nMessage == CExtPopupMenuWnd::g_nMsgPopupNext
						&&	_IsMdiApp()
						&&	hWndMdiChild != NULL
						&&	IsDisplayMdiDocumentButtons()
						)
					{
						ASSERT( !m_bPresubclassDialogMode );
						if( ! TrackChildFrameSystemMenu( NULL, TRUE ) )
						{
							if( m_bAllowProfChildFrameSysmenu )
								return false;
							int iNewTrackIndex =
								GetVisibleButton(
									iTrackingIndex,
									(nMessage == CExtPopupMenuWnd::g_nMsgPopupNext) ?
										TRUE : FALSE
									);
							if( iNewTrackIndex == iTrackingIndex )
							{
								lResult = 0; // same as tracking now
								return true;
							}
							TrackButtonMenu(iNewTrackIndex);
						}
					} // if( _IsMdiApp() && hWndMdiChild != NULL )
					else
					{
						int iNewTrackIndex =
							GetVisibleButton(
								iTrackingIndex,
								(nMessage == CExtPopupMenuWnd::g_nMsgPopupNext) ?
									TRUE : FALSE
								);
						if( iNewTrackIndex == iTrackingIndex )
						{
							lResult = 0; // same as tracking now
							return true;
						}
						TrackButtonMenu(iNewTrackIndex);
					} // else from if( _IsMdiApp() && hWndMdiChild != NULL )
				} // if( m_bSysMenuTracking )
				lResult = 0;
				return true;
			}

			if(		nMessage == CExtPopupMenuWnd::g_nMsgPopupNext
				&&	( !m_bPresubclassDialogMode )
				&&	iTrackingIndex == GetVisibleButton(-1,FALSE)
				)
			{
				if( ! _IsDockSiteCustomizeMode() )
				{
					if( (! TrackMainFrameSystemMenu( NULL, TRUE ) ) )
					{
						if( m_bAllowProfMainFrameSysmenu )
							return false;
						//int iNewTrackIndex = 0;
						int iNewTrackIndex =
							GetVisibleButton(
								iTrackingIndex,
								(nMessage == CExtPopupMenuWnd::g_nMsgPopupNext) ?
									TRUE : FALSE
								);
						if(		bPrintPreviewMode
							||	iNewTrackIndex == iTrackingIndex
							)
						{
							lResult = 0; // same as tracking now
							return true;
						}
						TrackButtonMenu(iNewTrackIndex);
						lResult = 0;
						return true;
					}
				} // if( ! _IsDockSiteCustomizeMode() )
				lResult = 0;
				return true;
			}

			if(		nMessage == CExtPopupMenuWnd::g_nMsgPopupPrev
				&&	( !m_bPresubclassDialogMode )
				&&	iTrackingIndex == 0
				&&	IsDisplayMdiDocumentButtons()
				)
			{
				if( ! _IsDockSiteCustomizeMode() )
				{
					if(		(! (_IsMdiApp() && hWndMdiChild != NULL ) )
						||	bPrintPreviewMode
						||	(! TrackChildFrameSystemMenu( NULL, TRUE ) )
						)
					{
						if( ! TrackMainFrameSystemMenu( NULL, TRUE ) )
						{
							if( m_bAllowProfMainFrameSysmenu )
								return false;
							int iNewTrackIndex =
								GetVisibleButton(
									iTrackingIndex,
									(nMessage == CExtPopupMenuWnd::g_nMsgPopupNext) ?
										TRUE : FALSE
									);
							if(		bPrintPreviewMode
								||	iNewTrackIndex == iTrackingIndex
								)
							{
								lResult = 0; // same as tracking now
								return true;
							}
							TrackButtonMenu(iNewTrackIndex);
							lResult = 0;
							return true;
						}
					}
				} // if( ! _IsDockSiteCustomizeMode() )
				lResult = 0;
				return true;
			}

			if(		nMessage == CExtPopupMenuWnd::g_nMsgPopupNext
				&&	(!m_bPresubclassDialogMode)
				&&	_IsMdiApp()
				&&	m_bSysMenuTracking
				&&	iTrackingIndex < 0
				&&	hWndMdiChild != NULL
				&&	IsDisplayMdiDocumentButtons()
				&&	(!bPrintPreviewMode)
				)
			{
				if( !_IsDockSiteCustomizeMode() )
					if( ! TrackChildFrameSystemMenu( NULL, TRUE ) )
						return false;
				lResult = 0;
				return true;
			}

			int iNewTrackIndex =
				GetVisibleButton(
					iTrackingIndex,
					(nMessage == CExtPopupMenuWnd::g_nMsgPopupNext) ?
						TRUE : FALSE
					);
			if(		(! IsDisplayMdiDocumentButtons() )
				&&	nMessage == CExtPopupMenuWnd::g_nMsgPopupPrev
				&&	iTrackingIndex == 0
				&&	iNewTrackIndex > 0
				)
			{
				if( !_IsDockSiteCustomizeMode() )
					if( ! TrackMainFrameSystemMenu( NULL, TRUE ) )
						return false;
				lResult = 0;
				return true;
			}

			if(		bPrintPreviewMode
				||	iNewTrackIndex == iTrackingIndex
				)
			{
				lResult = 0; // same as tracking now
				return true;
			}
			TrackButtonMenu(iNewTrackIndex);
			lResult = 0;
			return true;
		} // if menu next/prev popup activation messages

		if( nMessage == CExtPopupMenuWnd::g_nMsgNotifyMenuExpanded )
		{ // if menu rarely used items was expanded
			CExtToolControlBar::g_bMenuTrackingExpanded = true;
			lResult = 0;
			return true;
		} // if menu rarely used items was expanded

		if( ! m_bPresubclassDialogMode )
		{
			if(		nMessage == WM_STYLECHANGING
				||	nMessage == WM_STYLECHANGED
				)
				return false;
			_KillFrameMenu();
		}
	} // if window is a main frame or parent dialog-like
	else if( (!m_bPresubclassDialogMode) && hWndHooked == _GetHwndChildFrame() )
	{

		if(		nMessage == WM_PARENTNOTIFY
			&&	CExtPopupMenuWnd::IsMenuTracking()
			)
		{
			lResult = 0;
			return true;
		}

		if(		nMessage == WM_SIZE
			||	nMessage == WM_WINDOWPOSCHANGED
			||	nMessage == WM_ACTIVATEAPP
			||	( nMessage == WM_COMMAND && (((HIWORD(wParam))==0)||((HIWORD(wParam))==1)) )
			||	nMessage == WM_SYSCOMMAND
			)
		{
			m_bHelperTrackingCharCode = false;
			bool bSizePos = false;
			if( nMessage == WM_SIZE )
				bSizePos = true;
			else if( nMessage == WM_WINDOWPOSCHANGED )
			{
				LPWINDOWPOS lpWP = (LPWINDOWPOS)lParam;
				ASSERT( lpWP != NULL );
				if( (lpWP->flags & SWP_FRAMECHANGED) == 0 )
					bSizePos = true;
			} // else if( nMessage == WM_WINDOWPOSCHANGED )
			
			HWND hWndCapture = CExtMouseCaptureSink::GetCapture();
			if( hWndCapture == GetSafeHwnd() )
				CExtMouseCaptureSink::ReleaseCapture();
			else if( hWndCapture != NULL )
			{
				CWnd * pWnd = FromHandlePermanent(hWndCapture);
				if(		pWnd != NULL
					&&	pWnd->GetSafeHwnd() != NULL
					&&	pWnd->IsKindOf(RUNTIME_CLASS(CExtControlBar))
					&&	(! (((CExtControlBar*)pWnd)->_RowResizingGet() ) )
					&&	(! (((CExtControlBar*)pWnd)->_RowRecalcingGet() ) )
					)
					CExtMouseCaptureSink::ReleaseCapture();
			}

			_CancelFlatTracking();
			if( CExtMenuControlBar::g_bMenuTracking )
				CExtMenuControlBar::_CloseTrackingMenus();
			if(	CExtPopupMenuWnd::IsMenuTracking() )
				CExtPopupMenuWnd::CancelMenuTracking();
			
			if( !bSizePos )
			{
				_KillFrameMenu();

				if(		(	nMessage == WM_SIZE
						||	nMessage == WM_WINDOWPOSCHANGED
						)
					&&	(! _IsDockSiteCustomizeMode() )
					)
				{
					WINDOWPLACEMENT _wp;
					::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
					_wp.length = sizeof(WINDOWPLACEMENT);
					::GetWindowPlacement( _GetHwndChildFrame(), &_wp );
					if(		_wp.showCmd != SW_SHOWMAXIMIZED
						||	(	_GetButtonsCountImpl() > 0
							&&	(! _GetButtonPtr(0)->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton)) )
							&&	( IsDisplayMdiDocumentButtons() )
							) // MDI doc buttons not installed, but should be displayed
						)
						_DelayUpdateMenuBar();
				}
			} // if( !bSizePos )
			
			return 
				CExtHookSink::OnHookWndMsg(
					lResult,
					hWndHooked,
					nMessage,
					wParam,
					lParam
					);
		}

		bool bPrintPreviewMode = false;
		if(		(!m_bPresubclassDialogMode)
			&&	m_pDockSite->GetSafeHwnd() != NULL
			&&	FindPrintPreviewMode(m_pDockSite)
			)
			bPrintPreviewMode = true;

		if( bPrintPreviewMode )
		{
			lResult = 0;
			return true;
		}

		bool bMinFrameNcLbDown = false;
		bool bMinFrameNcLbUp = false;
		if(		nMessage == WM_NCLBUTTONDOWN
			||	nMessage == WM_NCLBUTTONUP
			)
		{
			WINDOWPLACEMENT _wp;
			::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
			_wp.length = sizeof(WINDOWPLACEMENT);
			::GetWindowPlacement( hWndHooked, &_wp );
			if( _wp.showCmd == SW_SHOWMINIMIZED )
			{
				if( nMessage == WM_NCLBUTTONDOWN )
					bMinFrameNcLbDown = true;
				else if( nMessage == WM_NCLBUTTONUP )
					bMinFrameNcLbUp = true;
			}
		}

		if( bMinFrameNcLbUp && wParam == HTCAPTION )
			return true;

		if(		( bMinFrameNcLbDown && wParam == HTCAPTION )
			||	( nMessage == WM_NCLBUTTONUP && wParam == HTSYSMENU )
			||	( nMessage == WM_NCRBUTTONUP && ( wParam == HTCAPTION || wParam == HTSYSMENU ) )
			)
		{
			if(		wParam == HTCAPTION
				&&	::IsIconic( hWndHooked )
				)
				return false;
			CExtToolControlBar::g_bMenuTrackingExpanded = false;
			CPoint * pPoint = NULL, ptCursor;
			if( nMessage == WM_NCRBUTTONUP )
			{
				if( ! ::GetCursorPos(&ptCursor) )
				{
					lResult = 0;
					return true;
				}
				pPoint = &ptCursor;
			}
			if( !_IsDockSiteCustomizeMode() )
				if( ! TrackChildFrameSystemMenu(
						pPoint,
						FALSE
						)
					)
					return false;
			lResult = 0;
			return true;
		}
		if(		( nMessage == WM_NCLBUTTONDOWN || nMessage == WM_NCRBUTTONDOWN )
			&&	wParam == HTSYSMENU
			)
		{
			if( ! m_bAllowProfChildFrameSysmenu )
				return false;
			lResult = 0;
			return true;
		}
		if( nMessage == WM_NCRBUTTONDOWN && wParam == HTCAPTION )
		{
			if( !_IsDockSiteCustomizeMode() )
			{
				CExtToolControlBar::g_bMenuTrackingExpanded = false;
				CPoint ptCursor;
				if( ::GetCursorPos(&ptCursor) )
					if( ! TrackChildFrameSystemMenu(
							&ptCursor,
							FALSE
							)
						)
						return false;
			} // if( !_IsDockSiteCustomizeMode() )
			lResult = 0;
			return true;
		}

		if( nMessage == CExtPopupMenuWnd::g_nMsgNotifyMenuClosed )
		{
			m_bSysMenuTracking = false;
			return true;
		}

		if( nMessage == CExtPopupMenuWnd::g_nMsgPopupNext
			|| nMessage == CExtPopupMenuWnd::g_nMsgPopupPrev
			)
		{ // if menu next/prev popup activation messages
			int iTrackingIndex =
				GetMenuTrackingButton();
			if( nMessage == CExtPopupMenuWnd::g_nMsgPopupPrev )
			{
				if( ! _IsDockSiteCustomizeMode() )
				{
					if( ! TrackMainFrameSystemMenu(
							NULL,
							TRUE
							)
						)
					{
						if( m_bAllowProfMainFrameSysmenu )
							return false;
						int iNewTrackIndex =
							GetVisibleButton(
								iTrackingIndex,
								(nMessage == CExtPopupMenuWnd::g_nMsgPopupNext) ?
									TRUE : FALSE
								);
						if( iNewTrackIndex == iTrackingIndex )
						{
							lResult = 0; // same as tracking now
							return true;
						}
						TrackButtonMenu(iNewTrackIndex);
					}
				}
				lResult = 0;
				return true;
			}
			if( iTrackingIndex < 0 ) // menu tracked on other bar
			{
				if( m_bSysMenuTracking )
				{
					int iNewTrackIndex =
						GetVisibleButton(
							iTrackingIndex,
							(nMessage == CExtPopupMenuWnd::g_nMsgPopupNext) ?
								TRUE : FALSE
							);
					if( iNewTrackIndex == iTrackingIndex )
					{
						lResult = 0; // same as tracking now
						return true;
					}
					TrackButtonMenu(iNewTrackIndex);
				} // if( m_bSysMenuTracking )
				lResult = 0;
				return true;
			}

			int iNewTrackIndex =
				GetVisibleButton(
					iTrackingIndex,
					(nMessage == CExtPopupMenuWnd::g_nMsgPopupNext) ?
						TRUE : FALSE
					);
			if( iNewTrackIndex == iTrackingIndex )
			{
				lResult = 0; // same as tracking now
				return true;
			}
			TrackButtonMenu(iNewTrackIndex);
			lResult = 0;
			return true;
		} // if menu next/prev popup activation messages

	} // else else if( (!m_bPresubclassDialogMode) && hWndHooked == _GetHwndChildFrame() )
	else if( (!m_bPresubclassDialogMode) && hWndHooked == _GetHwndMdiArea() )
	{
		if( nMessage == WM_PARENTNOTIFY
			&&
			CExtPopupMenuWnd::IsMenuTracking()
			)
		{
			lResult = 0;
			return true;
		}

		if( nMessage == WM_MDISETMENU )
		{
			CFrameWnd * pFrame = _GetDockingFrameImpl();
			ASSERT_VALID( pFrame );
			HWND hWndFrame = pFrame->GetSafeHwnd();
			ASSERT( hWndFrame != NULL );
			ASSERT( ::IsWindow(hWndFrame) );

			bool bOle = IsOleIpObjActive();
			
			HMENU hOldMdiMenu = m_menuDoc.Detach();
			HMENU hNewMdiMenu = (HMENU)wParam;
			m_menuDoc.Attach( hNewMdiMenu );

			if( bOle )
			{
				::SetMenu( hWndFrame, hNewMdiMenu );
			} // if( bOle )
			else
			{
				HMENU hFrameRealMenu = ::GetMenu( hWndFrame );
				if( hFrameRealMenu != NULL )
					::SetMenu( hWndFrame, NULL );
				_DelayUpdateMenuBar();
			} // else from if( bOle )
			CExtHookSink::OnHookWndMsgNextProcCurrent( 0L, 0L );
			lResult = (LRESULT)hOldMdiMenu;
			//_DelaySyncActiveMdiChild(); // (- v.2.23)
			_SyncActiveMdiChild(); // (+ v.2.23)
			m_bDelayedSyncActiveMdiChild = false; // (+ v.2.23)
			return true;
		}
		if( nMessage == WM_MDIREFRESHMENU )
		{
			CExtHookSink::OnHookWndMsgNextProcCurrent( 0L, 0L );
			//_DelaySyncActiveMdiChild(); // (- v.2.23)
			_SyncActiveMdiChild(); // (+ v.2.23)
			m_bDelayedSyncActiveMdiChild = false; // (+ v.2.23)
			lResult = 0;
			return true;
		}

		if( nMessage == WM_CONTEXTMENU )
		{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			CExtCustomizeSite * pSite =
				CExtCustomizeSite::GetCustomizeSite( m_hWnd );
			if(		pSite != NULL
				&&	pSite->IsCustomizeMode()
				)
			{
				lResult = 0;
				return true;
			}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			if( !CExtControlBar::FindHelpMode(this) )
			{
				CFrameWnd * pFrame = _GetDockingFrameImpl();
				ASSERT_VALID( pFrame );

				CPoint ptCursor;
				if( ! ::GetCursorPos(&ptCursor) )
				{
					lResult = 0;
					return true;
				}
				HWND hWndTrack = GetOwner()->GetSafeHwnd();
				ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );
				CExtPopupMenuWnd * pPopup =
					CExtPopupMenuWnd::InstantiatePopupMenu(
						GetSafeHwnd(),
						RUNTIME_CLASS(CExtPopupMenuWnd),
						this
						);
				VERIFY( pPopup->CreatePopupMenu(hWndTrack) );
				if(	! CExtDockBar::_ConstructDockSiteControlBarPopupMenu(
						pFrame,
						pPopup,
						CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MDICLIAREA_CTX,
						this,
						WM_CONTEXTMENU,
						NULL
						)
					)
				{
					delete pPopup;
				}
				else
				{
					if( pPopup->ItemGetCount() == 0 )
					{
						delete pPopup;
					}
					else
					{
						::SetFocus( hWndTrack );
						pPopup->m_hWndNotifyMenuClosed = GetSafeHwnd();
						if( ! pPopup->TrackPopupMenu(
								TPMX_OWNERDRAW_FIXED,
								ptCursor.x,
								ptCursor.y,
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
				}
			} // if( !CExtControlBar::FindHelpMode(this) )
			lResult = 0;
			return true;
		} // if( nMessage == WM_CONTEXTMENU )

		if(		nMessage == WM_MDIACTIVATE
			||	nMessage == WM_MDIDESTROY
			||	nMessage == WM_MDINEXT
			)
		{
			if(		nMessage == WM_MDINEXT
				&&	IsOleIpObjActive()
				)
			{
				lResult = 0;
				return true;
			}
			_DelaySyncActiveMdiChild();
			_DelayUpdateMenuBar();
		}

	
		if( g_PaintManager.m_bIsWinXPorLater )
		{
			// this implements Windows XP or later MDI flicker fix
			// suggested by Mr. Raffaele Cappelli
			HWND hWndMdiClient = _GetHwndMdiArea();
			if( nMessage == WM_MDIACTIVATE )
			{
				HWND hWndNew = (HWND)wParam;
				BOOL bOldWasMaximized = FALSE;
				HWND hWndOld = (HWND)
					::SendMessage( hWndMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bOldWasMaximized );
//				hWndOld;
//				if( bOldWasMaximized )
//				{
//					if( ! ::IsZoomed( hWndOld ) )
//						::SendMessage( hWndNew, WM_SYSCOMMAND, SC_MAXIMIZE, 0L );
//					::SendMessage( hWndMdiClient, WM_SETREDRAW, FALSE, 0 );
//				}
//				if( ::IsIconic( hWndNew ) )
//					::SendMessage(
//						hWndMdiClient,
//						bOldWasMaximized ? WM_MDIMAXIMIZE : WM_MDIRESTORE,
//						(WPARAM)hWndNew,
//						0L
//						);
				LRESULT _lResult = CExtHookSink::OnHookWndMsgDefault();
				_lResult;
//				if( bOldWasMaximized )
//				{
//					::SendMessage( hWndMdiClient, WM_SETREDRAW, TRUE, 0 );
//					::RedrawWindow( hWndMdiClient, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN );
//				}
				if( hWndOld != NULL && bOldWasMaximized )
				{
					::SetWindowPos(
						hWndNew, NULL, 0, 0, 0, 0,
						SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE
							|SWP_NOZORDER|SWP_NOOWNERZORDER
							|SWP_FRAMECHANGED
						);
					//CRect rcClient;
					//::GetClientRect( hWndMdiClient, &rcClient );
					//::PostMessage( hWndMdiClient, WM_SIZE, SIZE_RESTORED, MAKELPARAM( rcClient.Width(), rcClient.Height() ) );
				}
				return true;
			}
			return 
				CExtHookSink::OnHookWndMsg(
					lResult,
					hWndHooked,
					nMessage,
					wParam,
					lParam
					);
		} // if( g_PaintManager.m_bIsWinXPorLater )

	} // else if( (!m_bPresubclassDialogMode) && hWndHooked == _GetHwndMdiArea() )
	else
	{
		ASSERT( FALSE );
	}

	MSG msg;
	::memset( &msg, 0, sizeof(MSG) );
	msg.hwnd = hWndHooked;
	msg.message = nMessage;
	msg.wParam = wParam;
	msg.lParam = lParam;
	if( TranslateMainFrameMessage(&msg) )
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

bool CExtMenuControlBar::_IsNonCommandButtonMode() const
{
	ASSERT_VALID( this );
	return m_bNonCommandButtonMode;
}


/////////////////////////////////////////////////////////////////////////////
// CExtMenuControlBar

IMPLEMENT_DYNCREATE(CExtMenuControlBar, CExtToolControlBar)

CExtMenuControlBar::CExtMenuControlBar()
	: m_bUseFastKeyboardMenuTracking( true )
	, m_bAllowProfMainFrameSysmenu( true )
	, m_bAllowProfChildFrameSysmenu( true )
	, m_bHandleDblClickAsCloseCmd( true )
	, m_bFlatTracking( false )
	, m_bSysMenuTracking( false )
	, m_bMdiApp( false )
	, m_bMultiRowLayout( true )
	, m_nFlatTrackingIndex( -1 )
	, m_nOldTrackingIndex( -1 )
	, m_sMdiWindowPopupName( _T("") )
	, m_nMdiDocButtonCmdID( 0 )
	, m_hWndHelper( NULL )
	, m_bDelayedSyncActiveMdiChild( false )
	, m_bDelayedUpdateMenuBar( false )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_pMenuInfo( NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_bNonCommandButtonMode( false )
	, m_bMenuBarUsesDelayedButtonMenuTracking( true )
	, m_bAdaptiveCmdAllocMode( false )
	, m_bHelperTrackingCharCode( false )
{
}

CExtMenuControlBar::~CExtMenuControlBar()
{
	_RemoveAllButtonsImpl();
//CSingleLock _slCsCB( &g_csCB );
//	_slCsCB.Lock();
int nCountOfBars = (int)g_AllBars.GetSize();
	for( int i=0; i<nCountOfBars; i++ )
	{
		CExtControlBar * pBar = g_AllBars[i];
		ASSERT( pBar != NULL );
		if( pBar == this )
		{
			g_AllBars.RemoveAt( i );
			break;
		}
	}
//	_slCsCB.Unlock();
}

BEGIN_MESSAGE_MAP(CExtMenuControlBar, CExtToolControlBar)
    //{{AFX_MSG_MAP(CExtMenuControlBar)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(CExtToolControlBar::g_nMsgTrackButtonMenu, OnTrackButtonMenu)
END_MESSAGE_MAP()

// LoadMenuBar() required only for CFrameWnd
// based windows principally without menu
BOOL CExtMenuControlBar::LoadMenuBar(
	UINT nResourceID
	)
{
	if( m_menuDoc.GetSafeHmenu() != NULL )
	{
		VERIFY( m_menuDoc.DestroyMenu() );
	}
	if( m_menuFrame.GetSafeHmenu() != NULL )
	{
		VERIFY( m_menuFrame.DestroyMenu() );
	}
	
	if( m_pDockSite != NULL )
	{
		if(		GetSafeHwnd() == NULL
			||	m_menuFrame.GetSafeHmenu() != NULL
			)
		{
			ASSERT( FALSE );
			return FALSE;
		}
	}
	
	if( ! g_ResourceManager->LoadMenu( m_menuFrame, nResourceID ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT( ::IsMenu(m_menuFrame.GetSafeHmenu()) );
	if( !g_CmdManager->UpdateFromMenu(
			g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
			m_menuFrame.GetSafeHmenu()
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	return _UpdateMenuBar();
}

CMenu * CExtMenuControlBar::_GetMenu()
{
	if( m_menuDoc.GetSafeHmenu() != NULL )
	{
		ASSERT( ::IsMenu(m_menuDoc.GetSafeHmenu()) );
		return &m_menuDoc;
	}
	if( m_menuFrame.GetSafeHmenu() != NULL )
	{
		ASSERT( ::IsMenu(m_menuFrame.GetSafeHmenu()) );
		return &m_menuFrame;
	}
	return NULL;
}

CMenu * CExtMenuControlBar::GetMenu()
{
	return _GetMenu();
}

BOOL CExtMenuControlBar::UpdateMenuBar( // update after menu changed
	BOOL bDoRecalcLayout // = TRUE
	)
{
	return _UpdateMenuBar( bDoRecalcLayout );
}

BOOL CExtMenuControlBar::_UpdateMenuBar(
	BOOL bDoRecalcLayout // = TRUE
	)
{
	SetButtons(); // remove all buttons

	VERIFY(
		g_CmdManager->CmdRemoveByMask(
			g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
			(DWORD)CExtCmdItem::STATE_MENUBAR_TMP
			)
		);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	MenuInfoUpdate();
CExtCustomizeSite::CCmdMenuInfo * pMenuInfo = MenuInfoGet();
	if( pMenuInfo != NULL )
	{
		ASSERT_VALID( pMenuInfo->GetNode() );
		SetButtons( pMenuInfo->GetNode() );
	} // if( pMenuInfo != NULL )
	else
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	{
		CMenu * pMenu = GetMenu();
		if( pMenu != NULL && pMenu->GetSafeHmenu() != NULL )
		{
			ASSERT( ::IsMenu(pMenu->GetSafeHmenu()) );
			bool bRevertRTL = OnQueryRevertRTL();
			UINT nMenuItemCount = pMenu->GetMenuItemCount();
			for( UINT nMenuItemIndex = 0; nMenuItemIndex < nMenuItemCount; nMenuItemIndex++ )
			{
				UINT nInsertButtonLocation =
					bRevertRTL
						? 0
						: nMenuItemIndex
						;
				MENUITEMINFO mii;
				::memset( &mii, 0, sizeof(MENUITEMINFO) );
				mii.cbSize = sizeof(MENUITEMINFO);
				mii.fMask =
					MIIM_CHECKMARKS
					|MIIM_DATA
					|MIIM_ID
					|MIIM_STATE
					|MIIM_SUBMENU
					|MIIM_TYPE
					;
				mii.cch = __MAX_UI_ITEM_TEXT;
				CExtSafeString sText;
				mii.dwTypeData =
					sText.GetBuffer( __MAX_UI_ITEM_TEXT );
				ASSERT( mii.dwTypeData != NULL );
				if( mii.dwTypeData == NULL )
				{
					ASSERT( FALSE );
					return FALSE;
				}
				if( ! pMenu->GetMenuItemInfo(
						nMenuItemIndex,
						&mii,
						TRUE
						)
					)
				{
					sText.ReleaseBuffer();
					ASSERT( FALSE );
					return false;
				}
				sText.ReleaseBuffer();

				BOOL bAppendMdiWindowsMenu = FALSE;
				UINT nCmdID = 0;
				CExtCmdItem * pCmdItem = NULL;
				bool bAllocCmd = false;
				bool bStateEnable = false;
				if( mii.hSubMenu == NULL )
				{
					nCmdID = mii.wID;
					if( nCmdID == ID_SEPARATOR )
					{
						if( ! InsertButton(
								nInsertButtonLocation,
								nCmdID,
								FALSE
								)
							)
						{
							ASSERT( FALSE );
							return FALSE;
						}
						continue;
					} // if( nCmdID == ID_SEPARATOR )
					ASSERT( CExtCmdManager::IsCommand(nCmdID) );
					pCmdItem =
						g_CmdManager->CmdGetPtr(
							g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
							nCmdID
							);
					if( m_bAdaptiveCmdAllocMode )
					{
						if( pCmdItem == NULL )
						{
							bAllocCmd = true;
							bStateEnable = true;
						} 
					}
#ifdef _DEBUG
					else
					{
						ASSERT( pCmdItem != NULL );
					}
#endif
				} // if( mii.hSubMenu == NULL )
				else
				{
					bAllocCmd = true;
				} // else from if( mii.hSubMenu == NULL )
				if( bAllocCmd )
				{
					pCmdItem =
						g_CmdManager->CmdAllocPtr(
							g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() )
							);
					if( pCmdItem == NULL )
					{
						ASSERT( FALSE );
						return FALSE;
					} // if( pCmdItem == NULL )
					nCmdID = pCmdItem->m_nCmdID;
					ASSERT( CExtCmdManager::IsCommand(nCmdID) );
					pCmdItem->StateSetMenubarTemp();
					pCmdItem->StateSetBasic();
					if( bStateEnable )
						pCmdItem->StateEnable(TRUE);

					if( _IsMdiApp() && (! m_sMdiWindowPopupName.IsEmpty() ) )
					{
						CExtSafeString _sText(sText);
						_sText.TrimLeft();
						_sText.TrimRight();
						while( _sText.Replace(_T("&"),_T("")) > 0 )
						{
							_sText.TrimLeft();
							_sText.TrimRight();
						} // while( _sText.Replace(_T("&"),_T("")) > 0 )
						if( _sText == m_sMdiWindowPopupName )
							bAppendMdiWindowsMenu = TRUE;
					} // if( _IsMdiApp() && (! m_sMdiWindowPopupName.IsEmpty() ) )
				} // else from if( mii.hSubMenu == NULL )
				ASSERT( pCmdItem != NULL );
				if( pCmdItem->m_sToolbarText.IsEmpty() )
					pCmdItem->m_sToolbarText = sText;
				if( pCmdItem->m_sMenuText.IsEmpty() )
					pCmdItem->m_sMenuText = sText;

				if( ! InsertButton(
						nInsertButtonLocation,
						nCmdID,
						FALSE
						)
					)
				{
					ASSERT( FALSE );
					return FALSE;
				}

				if( mii.hSubMenu != NULL )
				{
					ASSERT( ::IsMenu(mii.hSubMenu) );
					SetButtonMenu(
						nInsertButtonLocation,
						mii.hSubMenu,
						FALSE,
						FALSE,
						FALSE
						);
				} // if( mii.hSubMenu != NULL )
				if( bAppendMdiWindowsMenu )
				{
					VERIFY(
						MarkButtonAsMdiWindowsMenu(
							nInsertButtonLocation,
							TRUE
							)
						);
				} // if( bAppendMdiWindowsMenu )
			} // for( UINT nMenuItemIndex = 0; nMenuItemIndex < nMenuItemCount; nMenuItemIndex++ )

			ASSERT( m_pRightBtn == NULL );
			m_pRightBtn = OnCreateBarRightBtn();
			if( m_pRightBtn != NULL )
			{
				ASSERT_VALID( m_pRightBtn );
				ASSERT_KINDOF( CExtBarContentExpandButton, m_pRightBtn );
				m_buttons.Add( m_pRightBtn );
			} // if( m_pRightBtn != NULL )
		} // if( pMenu != NULL && pMenu->GetSafeHmenu() != NULL )
	} // else from if( pMenuInfo != NULL )

	if( _IsMdiApp() )
	{
		if( ! IsOleIpObjActive() )
			if( _InstallMdiDocButtons( FALSE ) )
				bDoRecalcLayout = TRUE;
		VERIFY( _SyncActiveMdiChild() );
	}
	
	if( bDoRecalcLayout && m_pDockSite != NULL )
	{
		CFrameWnd * pFrame = GetParentFrame();
		ASSERT_VALID( pFrame );
		pFrame->RecalcLayout();
		if( pFrame->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd)) )
			pFrame->SetWindowPos(
				NULL, 0, 0, 0, 0,
				SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE
					|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_FRAMECHANGED
				);
		Invalidate();
		_RecalcLayoutImpl();
		UpdateWindow();
	} // if( bDoRecalcLayout && m_pDockSite != NULL )
	return TRUE;
}

HWND CExtMenuControlBar::_GetActiveMdiChildWnd(
	BOOL & bMaximized
	)
{
	bMaximized = FALSE;
	if( ! _IsMdiApp() )
		return NULL;

	 // FrameFeatures does not use CMDIFrameWnd
CMDIFrameWnd * pMdiFrame = DYNAMIC_DOWNCAST( CMDIFrameWnd, _GetDockingFrameImpl() );
HWND hWndMdiClient = NULL;
	if( pMdiFrame == NULL )
		hWndMdiClient = _GetHwndMdiArea();
	else
	{
		ASSERT_VALID( pMdiFrame );
		hWndMdiClient = pMdiFrame->m_hWndMDIClient;
	} // else from if( pMdiFrame == NULL )
	if( hWndMdiClient == NULL )
		return NULL;
	ASSERT( ::IsWindow( hWndMdiClient ) );
BOOL bMax = FALSE;
HWND hWndActiveChildFrame = (HWND) ::SendMessage( hWndMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax );
	if( hWndActiveChildFrame == NULL )
		bMax = FALSE;
	else
	{
		if( ! ::IsWindow( hWndActiveChildFrame ) )
			return NULL;
		__EXT_MFC_LONG_PTR dwActiveChildFrameStyle = :: __EXT_MFC_GetWindowLong( hWndActiveChildFrame, GWL_STYLE );
		if( pMdiFrame == NULL )
		{
			if( (dwActiveChildFrameStyle&WS_VISIBLE) == 0 )
			{
				hWndActiveChildFrame = NULL;
				bMax = FALSE;
			} // if( (dwActiveChildFrameStyle&WS_VISIBLE) == 0 )
		} // if( pMdiFrame == NULL )
		else
		{
			CWnd * pWndTempAnalyze =
				CWnd::FromHandlePermanent( hWndActiveChildFrame );
			if( pWndTempAnalyze != NULL )
			{
				ASSERT_VALID( pWndTempAnalyze );
				CMDIChildWnd * pWndTempMDIChild = DYNAMIC_DOWNCAST( CMDIChildWnd, pWndTempAnalyze );
				if(		pWndTempMDIChild != NULL
					&&	pWndTempMDIChild->m_bPseudoInactive
					&&	(dwActiveChildFrameStyle&WS_VISIBLE) == 0
					)
				{
					hWndActiveChildFrame = NULL;
					bMax = FALSE;
				}
				else
				{
					if( ! bMax )
					{
						if( (dwActiveChildFrameStyle&WS_MAXIMIZE) != 0 )
							bMax = true;
					}
				}
			} // if( pWndTempAnalyze != NULL )
		} // else from if( pMdiFrame == NULL )
	} // else from if( hWndActiveChildFrame == NULL )
	bMaximized = bMax;
	return hWndActiveChildFrame;
}

void CExtMenuControlBar::OnUpdateCmdUI(
	CFrameWnd * pTarget,
	BOOL bDisableIfNoHndler
	)
{
	CExtToolControlBar::OnUpdateCmdUI(
		pTarget,
		bDisableIfNoHndler
		);
}

BOOL CExtMenuControlBar::_SyncActiveMdiChild()
{
	if( m_bPresubclassDialogMode )
		return TRUE;
	if( _DraggingGetBar() != NULL )
		return TRUE;

//	if( (GetStyle() & WS_VISIBLE) == 0 )
//		return TRUE;

BOOL bMax = FALSE;
HWND hWndActiveChild =
		_GetActiveMdiChildWnd( bMax );
	if( m_hWndHelper == hWndActiveChild )
		return TRUE;
	if( m_hWndHelper != NULL )
		SetupHookWndSink( m_hWndHelper, true );
	m_hWndHelper = hWndActiveChild;
	if( m_hWndHelper != NULL )
		SetupHookWndSink( m_hWndHelper );
	_RecalcLayoutImpl();
	return TRUE;
}

LRESULT CExtMenuControlBar::DoHelpHitTest( CPoint ptHelpHitTest )
{
	ASSERT_VALID( this );
	return CExtToolControlBar::DoHelpHitTest( ptHelpHitTest );
}

bool CExtMenuControlBar::_IsMdiApp() const
{
	ASSERT_VALID( this );
	return m_bMdiApp;
}

bool CExtMenuControlBar::IsDisplayMdiDocumentButtons() const
{
	ASSERT_VALID( this );
	return true;
}

BOOL CExtMenuControlBar::_InstallMdiDocButtons(
	BOOL bDoRecalcLayout // = TRUE
	)
{
	ASSERT( _IsMdiApp() );
	if( ! IsDisplayMdiDocumentButtons() )
	{
		if( bDoRecalcLayout )
			_RecalcLayoutImpl();
		return TRUE;
	}
	if( _DraggingGetBar() != NULL )
		return FALSE;
BOOL bMax = FALSE;
HWND hWndActiveChild =
		_GetActiveMdiChildWnd( bMax );
int nCountOfButtons = GetButtonsCount();
	if(		nCountOfButtons == 0
		&&	hWndActiveChild == NULL
		)
		return FALSE;
CExtBarMdiDocButton * pDocTBB = NULL;
	if( nCountOfButtons > 0 )
		pDocTBB = 
			DYNAMIC_DOWNCAST(
				CExtBarMdiDocButton,
				_GetButtonPtr(0)
				);
int nRightIndex = nCountOfButtons - 1;
CExtBarMdiRightButton * pRightTBB = NULL;
	if( nCountOfButtons > 0 )
		pRightTBB =
			DYNAMIC_DOWNCAST(
				CExtBarMdiRightButton,
				_GetButtonPtr(nRightIndex)
				);
	if( pRightTBB == NULL
		&& nCountOfButtons > 1
		)
	{
		if( !m_bMultiRowLayout )
			nRightIndex--;
		pRightTBB =
			DYNAMIC_DOWNCAST(
				CExtBarMdiRightButton,
				_GetButtonPtr(nRightIndex)
				);
	}
BOOL bRetVal = FALSE;
	if( hWndActiveChild == NULL || (!bMax) )
	{ // if no active MDI doc or not maximized
		if( pRightTBB != NULL )
		{
			ASSERT_VALID( pRightTBB );
			VERIFY( RemoveButton(nRightIndex,FALSE) );
			bRetVal = TRUE;
			pRightTBB = NULL;
		}
		if( pDocTBB != NULL )
		{
			ASSERT_VALID( pDocTBB );
			VERIFY( RemoveButton(0,FALSE) );
			bRetVal = TRUE;
			pDocTBB = NULL;
		}
	} // if no active MDI doc or not maximized
	else
	{ // if active MDI doc is maximized
		ASSERT( hWndActiveChild != NULL );
		ASSERT( ::IsWindow(hWndActiveChild) );
		if( m_nMdiDocButtonCmdID == 0 )
		{
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdAllocPtr(
					g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() )
					);
			ASSERT( pCmdItem != NULL );
			if( pCmdItem == NULL )
				return FALSE;
			m_nMdiDocButtonCmdID = pCmdItem->m_nCmdID;
			ASSERT(
				CExtCmdManager::IsCommand(
					m_nMdiDocButtonCmdID
					)
				);
			//pCmdItem->StateSetMenubarTemp();
			pCmdItem->StateSetBasic();
		} // if( m_nMdiDocButtonCmdID == 0 )
		if( pRightTBB == NULL )
		{
			pRightTBB =
				new CExtBarMdiRightButton(
					this,
					m_nMdiDocButtonCmdID
					);
			ASSERT_VALID( pRightTBB );
			m_buttons.InsertAt( nRightIndex + 1, pRightTBB );
			bRetVal = TRUE;
		} // if( pRightTBB == NULL )
		if( pDocTBB == NULL )
		{
			HMENU hDocMenu =
				::GetSystemMenu(hWndActiveChild,FALSE);
			if(		hDocMenu != NULL
				&&	::IsMenu(hDocMenu)
				)
			{
				pDocTBB =
					new CExtBarMdiDocButton(
						this,
						m_nMdiDocButtonCmdID
						);
				ASSERT_VALID( pDocTBB );
				pDocTBB->m_bHandleDblClickAsCloseCmd =
					m_bHandleDblClickAsCloseCmd;
				pDocTBB->SetMenu( hDocMenu, FALSE, FALSE );
				m_buttons.InsertAt( 0, pDocTBB );
				bRetVal = TRUE;
			}
		} // if( pDocTBB == NULL )
	} // if active MDI doc is maximized

	if( pRightTBB != NULL )
	{
		ASSERT_VALID( pRightTBB );
		pRightTBB->_ActiveMdiChildWndAttach( hWndActiveChild );
	}
	if( pDocTBB != NULL )
	{
		ASSERT_VALID( pDocTBB );
		pDocTBB->_ActiveMdiChildWndAttach( hWndActiveChild );
	}
	if( bDoRecalcLayout )
	{
		_RecalcLayoutImpl();
		bRetVal = FALSE;
	}
	return bRetVal;
}

void CExtMenuControlBar::OnDestroy() 
{
	if( m_nMdiDocButtonCmdID != 0 )
	{
		VERIFY(
			g_CmdManager->CmdRemove(
				g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
				m_nMdiDocButtonCmdID
				)
			);
		m_nMdiDocButtonCmdID = 0;
	}
	RemoveAllWndHooks();
	CExtToolControlBar::OnDestroy();
}

CSize CExtMenuControlBar::_GetDefButtonSize() const
{
	ASSERT_VALID( this );
	return 
		IsDockedHorizontally()
			? CSize( 22, 19 )
			: CSize( 19, 22 );
}

const ACCEL * CExtMenuControlBar::_GetTranslatedContextHelpAccel() const
{
	if( ! g_bTranslateContextHelpCmd )
		return NULL;
	return &g_accelContextHelp;
}

BOOL CExtMenuControlBar::TranslateMainFrameMessage(MSG* pMsg) 
{
	__PROF_UIS_MANAGE_STATE;
	if( GetSafeHwnd() == NULL )
		return  FALSE;
	ASSERT( pMsg != NULL );

	if(		pMsg->message == WM_TIMER
		&&	pMsg->hwnd == m_hWndHelper
		&&	AnimationSite_OnHookTimer( UINT(pMsg->wParam) )
		)
		return TRUE;

	if( pMsg->message == WM_QUERYOPEN )
	{
		if( m_pDockSite != NULL )
		{
			_DelayUpdateMenuBar();
//			m_bDelayedUpdateMenuBar = true;
//			SetWindowPos(
//				NULL, 0, 0, 0, 0,
//				SWP_FRAMECHANGED
//					|SWP_NOMOVE|SWP_NOSIZE|SWP_NOCOPYBITS
//					|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
//				);
//			SetTimer( 14, 100, NULL );
		}
		return FALSE;
	} // if( pMsg->message == WM_QUERYOPEN )

// BEGIN: fix for the bug reported by Vitaly Murashev
HWND hWndMainFrame = _GetHwndMainFrame();
	if(		(! m_bPresubclassDialogMode )
		&&	(	hWndMainFrame == NULL
			||	(! ::IsWindow( hWndMainFrame ) )
			||	(! ::IsWindowEnabled( hWndMainFrame ) )
			)
		)
		return FALSE;
// END: fix for the bug reported by Vitaly Murashev

	if(		pMsg->message == WM_ACTIVATE
		||	pMsg->message == WM_ACTIVATEAPP
		||	pMsg->message == WM_SETFOCUS
		||	pMsg->message == WM_KILLFOCUS
		)
	{
		m_bHelperTrackingCharCode = false;
		g_bCancelNearestAltUp = true;
		return FALSE;
	} // if( pMsg->message == WM_ACTIVATE ...

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = GetCustomizeSite();
	if( pSite != NULL )
	{
		if( pSite->GetCustomizeDropSource() != NULL )
			return FALSE;
		if(		WM_KEYFIRST <= pMsg->message
			&&	pMsg->message <= WM_KEYLAST
			&&	pSite->IsCustomizeMode()
			&&	(! CExtPopupMenuWnd::IsMenuTracking() )
			)
			return TRUE;
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool bDockSiteCustomizeMode = _IsDockSiteCustomizeMode();

	if( _IsMdiApp() )
	{
		if(		(	pMsg->message == WM_COMMAND
				&&	(	(LOWORD(pMsg->wParam)) == ID_NEXT_PANE
					||	(LOWORD(pMsg->wParam)) == ID_PREV_PANE
					||	(LOWORD(pMsg->wParam)) == ID_WINDOW_NEW
					||	(LOWORD(pMsg->wParam)) == ID_WINDOW_ARRANGE
					||	(LOWORD(pMsg->wParam)) == ID_WINDOW_CASCADE
					||	(LOWORD(pMsg->wParam)) == ID_WINDOW_TILE_HORZ
					||	(LOWORD(pMsg->wParam)) == ID_WINDOW_TILE_VERT
					||	(LOWORD(pMsg->wParam)) == ID_WINDOW_SPLIT
					)
				)
			||	pMsg->message == WM_MDIACTIVATE
			||	pMsg->message == WM_MDINEXT
			||	pMsg->message == WM_MDIRESTORE
			||	pMsg->message == WM_MDIMAXIMIZE
			||	pMsg->message == WM_MDITILE
			||	pMsg->message == WM_MDICASCADE
			||	pMsg->message == WM_MDIICONARRANGE
			)
		{
			if( bDockSiteCustomizeMode )
				return TRUE;
			if( IsOleIpObjActive() )
				return TRUE;
		}
	} // if( _IsMdiApp() )

bool bPrintPreviewMode = false;
	if(		(!m_bPresubclassDialogMode)
		&&	(!bDockSiteCustomizeMode)
		&&	m_pDockSite->GetSafeHwnd() != NULL
		&&	FindPrintPreviewMode(m_pDockSite)
		)
		bPrintPreviewMode = true;
bool bComboDropDownTracking = false;
CWnd * pWndCurrFocus = CWnd::GetFocus();
	if( pWndCurrFocus != NULL )
	{
		if( pWndCurrFocus->IsKindOf(RUNTIME_CLASS(CComboBox)) )
		{
			if(		(pWndCurrFocus->GetStyle() & (CBS_DROPDOWN|CBS_DROPDOWNLIST)) != 0
				&&	((CComboBox *)pWndCurrFocus)->GetDroppedState()
				)
				bComboDropDownTracking = true;
		}
		else
		{
			pWndCurrFocus = pWndCurrFocus->GetParent();
			if( pWndCurrFocus != NULL )
			{
				if( pWndCurrFocus->IsKindOf(RUNTIME_CLASS(CComboBox)) )
				{
					if(		(pWndCurrFocus->GetStyle() & (CBS_DROPDOWN|CBS_DROPDOWNLIST)) != 0
						&&	((CComboBox *)pWndCurrFocus)->GetDroppedState()
						)
						bComboDropDownTracking = true;
				}
			}
		}
	}

UINT nMsg = pMsg->message;

	if(		nMsg == WM_COMMAND
		&&	HIWORD( pMsg->wParam ) == 0
		) // (+ v.2.24)
	{
		bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
		g_bCancelNearestAltUp = bAlt;
	}

	if(	   nMsg == WM_SYSKEYDOWN
		|| nMsg == WM_SYSKEYUP
		|| nMsg == WM_KEYDOWN
		|| nMsg == WM_KEYUP
		)
	{
		BOOL bKeyUp = ( nMsg == WM_KEYUP || nMsg == WM_SYSKEYUP) ? TRUE : FALSE;
		BOOL bCtrl = ::GetKeyState(VK_CONTROL) & 0x80000000;
		BOOL bShift = ::GetKeyState(VK_SHIFT) & 0x80000000;
		BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;
		//BOOL bExtended = HIWORD(pMsg->lParam) & KF_EXTENDED;
		bool bRightAltGr = ( (::GetAsyncKeyState(VK_RMENU)&0x8000) != 0 ) ? true : false;
		TCHAR vkTCHAR = (TCHAR)pMsg->wParam;

		if( bRightAltGr && vkTCHAR != VK_MENU )
		{
			HWND hWndFocus = ::GetFocus();
			if( hWndFocus != NULL )
			{
				CExtSafeString sClassName;
				::GetClassName( hWndFocus, LPTSTR( sClassName.GetBuffer( _MAX_PATH+1 ) ), _MAX_PATH );
				sClassName.ReleaseBuffer();
				sClassName.MakeLower();
				if( sClassName == "edit" )
					return FALSE;
			}
		}

		if( pMsg->wParam == VK_MENU && m_bHelperTrackingCharCode && bKeyUp )
		{
			m_bHelperTrackingCharCode = false;
			return FALSE;
		}
		if( bAlt && pMsg->wParam != VK_MENU )
		{
			if(		( VK_NUMPAD0 <= pMsg->wParam && pMsg->wParam <= VK_NUMPAD9 )
				||	pMsg->wParam == VK_INSERT // 0
				||	pMsg->wParam == VK_END    // 1
				||	pMsg->wParam == VK_DOWN   // 2
				||	pMsg->wParam == VK_NEXT   // 3
				||	pMsg->wParam == VK_LEFT   // 4
				||	pMsg->wParam == 0x0C      // 5, VK_CLEAR
				||	pMsg->wParam == VK_RIGHT  // 8
				||	pMsg->wParam == VK_HOME   // 7
				||	pMsg->wParam == VK_UP     // 8
				||	pMsg->wParam == VK_PRIOR  // 9
				)
				m_bHelperTrackingCharCode = g_bCancelNearestAltUp = true;
			else
				m_bHelperTrackingCharCode = false;
		}
		if( m_bHelperTrackingCharCode )
			return FALSE;

		if( ( ! bDockSiteCustomizeMode ) && GetSafeHwnd() != NULL )
		{ // help key processing
			const ACCEL * pAccel = _GetTranslatedContextHelpAccel();
			CPoint ptCursor( LONG_MAX, LONG_MAX );
			HWND hWndFromPoint = NULL;
			CWnd * pWndFromPointPermanent = NULL;
			CExtToolControlBar * pToolBarFromPoint = NULL;
			int nHitTestButtonIndex = -1;
			CExtBarButton * pHitTestTBB = NULL;
			UINT nHitTestCmdID = ID_SEPARATOR;
			LPCTSTR strBarCmdProfileName = NULL;
			CExtCmdItem * pHitTestCmdItem = NULL;
			if(		pAccel != NULL
				&&	pMsg->wParam == WPARAM(pAccel->key)
				&&	(pAccel->fVirt&(FSHIFT|FCONTROL|FALT)) ==
						(	0
						|	( bShift ? (FSHIFT) : 0 )
						|	( bCtrl ? (FCONTROL) : 0 )
						|	( bAlt ? (FALT) : 0 )
						)
				&&	::GetCursorPos( &ptCursor )
				&&	( hWndFromPoint = ::WindowFromPoint( ptCursor ) ) != NULL
				&&	( pWndFromPointPermanent = CWnd::FromHandlePermanent( hWndFromPoint ) ) != NULL
				&&	( pToolBarFromPoint = DYNAMIC_DOWNCAST( CExtToolControlBar, pWndFromPointPermanent ) ) != NULL
				&&	(	( m_pDockSite != NULL )
							? ( m_pDockSite == pToolBarFromPoint->m_pDockSite )
							: ( GetParent() == pToolBarFromPoint->GetParent() )
						)
				&&	( strBarCmdProfileName = g_CmdManager->ProfileNameFromWnd( hWndFromPoint ) ) != NULL
				//&&	_tcscmp( g_CmdManager->ProfileNameFromWnd( hWndFromPoint ), g_CmdManager->ProfileNameFromWnd( m_hWnd ) ) == NULL
				&&	::ScreenToClient( hWndFromPoint, &ptCursor )
				&&	( nHitTestButtonIndex = pToolBarFromPoint->HitTest( ptCursor ) ) >= 0
				&&	( pHitTestTBB = pToolBarFromPoint->GetButton( nHitTestButtonIndex ) ) != NULL
				&&	( ! pHitTestTBB->IsKindOf( RUNTIME_CLASS( CExtBarMdiDocButton ) ) )
				//&&	( ! pHitTestTBB->IsKindOf( RUNTIME_CLASS( CExtBarContentExpandButton ) ) )
				&&	( nHitTestCmdID = pHitTestTBB->GetCmdID( false ) ) != ID_SEPARATOR
				&&	( pHitTestCmdItem = g_CmdManager->CmdGetPtr( strBarCmdProfileName, nHitTestCmdID ) ) != NULL
				&&	( ! pHitTestCmdItem->StateIsMenubarTemp() )
				)
			{
				if( ! bKeyUp )
					::AfxGetApp()->WinHelp( nHitTestCmdID, HELP_CONTEXT );
				return TRUE;
			} // if( pAccel != NULL ...
		} // help key processing

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if(		m_bFlatTracking
			&&	KeyTipsDisplayedGet()
			&&	(	( _T('A') <= vkTCHAR && vkTCHAR <= _T('Z') )
				||	( _T('0') <= vkTCHAR && vkTCHAR <= _T('9') )
				)
			)
		{
			if( bKeyUp )
				return TRUE;
			bool bActionInvoked = false;
			bool bContinueKeyTipMode = false;
			CExtCustomizeCmdKeyTip & keyTipChain = KeyTipChainGet();
			if(	KeyTipTranslate(
					(DWORD)pMsg->wParam,
					&keyTipChain,
					true,
					bActionInvoked,
					bContinueKeyTipMode
					) >= 0
				)
			{
				if(		bActionInvoked
					&&	( ! bContinueKeyTipMode )
					//&&	( ! CExtPopupMenuWnd::IsMenuTracking() )
					)
				{
					if( m_bFlatTracking )
					{
						m_bFlatTracking = false;
						OnFlatTrackingStop();
					} // if( m_bFlatTracking )
					_UpdateFlatTracking( FALSE );
				}
				else
					KeyTipDisplay( keyTipChain );
				//return TRUE;
			}
			return TRUE;
		}
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)

		if(		bAlt
			&&	(! m_bFlatTracking )
			&&	(	nMsg != WM_SYSKEYDOWN
				&&	nMsg != WM_SYSKEYUP
				)
			&&	(	vkTCHAR == VK_NUMPAD0
				||	vkTCHAR == VK_NUMPAD1
				||	vkTCHAR == VK_NUMPAD2
				||	vkTCHAR == VK_NUMPAD3
				||	vkTCHAR == VK_NUMPAD4
				||	vkTCHAR == VK_NUMPAD5
				||	vkTCHAR == VK_NUMPAD6
				||	vkTCHAR == VK_NUMPAD7
				||	vkTCHAR == VK_NUMPAD8
				||	vkTCHAR == VK_NUMPAD9
				||	vkTCHAR == 0x2D
				||	vkTCHAR == 0x22
				||	vkTCHAR == 0x28
				||	vkTCHAR == 0x22
				||	vkTCHAR == 0x25
				||	vkTCHAR == 0x0C
				||	vkTCHAR == 0x27
				||	vkTCHAR == 0x24
				||	vkTCHAR == 0x26
				||	vkTCHAR == 0x21
				)
			)
		{
			g_bKeyCodeTracking = true;
			g_bCancelNearestAltUp = false;
			return FALSE;
		}
		if( g_bKeyCodeTracking )
		{
			g_bKeyCodeTracking = false;
			return FALSE;
		}

		if( vkTCHAR == VK_MENU && (!bCtrl) && (!bShift) )
		{ // only VK_MENU pressed
			if( bDockSiteCustomizeMode )
				return TRUE;
			if(		bPrintPreviewMode
				||	bComboDropDownTracking
				)
				return FALSE;

			if( m_bFlatTracking ) // (+ v.2.23)
			{
				if( ! bKeyUp )
					return TRUE;
				_CancelFlatTracking();
				return TRUE;
			} // if( m_bFlatTracking )
			
			if( !bKeyUp )
			{
				g_bCancelNearestAltUp = false;
				return FALSE; // (+ v.2.23)
			}
			if(		g_bCancelNearestAltUp
				||	g_bMenuTracking
				||	GetButtonsCount() == 0
				)
			{
				if( g_bCancelNearestAltUp ) // (+ v.2.23)
					return FALSE;
				return TRUE;
			}
			if( ! CExtPopupMenuWnd::IsMenuTracking() )
			{
				HWND hWndFocus = ::GetFocus();
				if( hWndFocus != NULL )
					::SendMessage( hWndFocus, WM_CANCELMODE, 0L, 0L );
			} // if( ! CExtPopupMenuWnd::IsMenuTracking() )
			CExtToolControlBar::g_bMenuTrackingExpanded = false;
			
			if( hWndMainFrame != NULL )
			{
				CWnd * pWndForPlacement = stat_GetWndForPlacement( CWnd::FromHandle(hWndMainFrame) );
				HWND hWndForeground = ::GetForegroundWindow();
				bool bCancelButtonTracking = true;
				if(		hWndMainFrame == hWndForeground
					||	hWndMainFrame == pWndForPlacement->m_hWnd
					)
					bCancelButtonTracking = false;
				else if(
						(::__EXT_MFC_GetWindowLong(hWndMainFrame,GWL_STYLE)&WS_CHILD) != 0
					&&	::IsChild( hWndForeground, hWndMainFrame )
					)
					bCancelButtonTracking = false;
				else
				{
					for(	HWND hWnd = ::GetParent( hWndForeground );
							hWnd != NULL;
							hWnd = ::GetParent( hWnd )
							)
					{
						if(		hWnd == hWndMainFrame
							||	hWnd == pWndForPlacement->m_hWnd
							)
						{
							bCancelButtonTracking = false;
							break;
						} // if( hWnd == hWndMainFrame ...
					}
				}
				if( bCancelButtonTracking )
					return TRUE;
			} // if( hWndMainFrame != NULL )

			INT nStartFlatIndex = OnCalcFlatTrackingIndex( vkTCHAR, -1 );
			if( nStartFlatIndex < 0 )
			{
				INT nIndex, nCount = GetButtonsCount();
				for( nIndex = 0; nIndex < nCount; nIndex ++ )
				{
					CExtBarButton * pTBB = GetButton( nIndex );
					ASSERT_VALID( pTBB );
					if( pTBB->OnQueryFlatTrackingEnabled() )
					{
						nStartFlatIndex = nIndex;
						break;
					}
				}
			} // if( nStartFlatIndex < 0 )
			if( nStartFlatIndex >= 0 )
			{
				TrackButtonMenu( nStartFlatIndex );
				return TRUE;
			} // if( nStartFlatIndex >= 0 )
			else
				return FALSE;
		} // only VK_MENU pressed

		if(		(! m_bPresubclassDialogMode )
			&&	bAlt
			&&	(	 vkTCHAR == VK_SPACE
				||	(	_IsMdiApp() // (+ v.2.23)
					&&	(	vkTCHAR == VK_SUBTRACT
// disable warning C4310: cast truncates constant value
#pragma warning( push )
#pragma warning( disable : 4310 )
						||	vkTCHAR == ((TCHAR)0xbd) // '-' near '0'
// rollback warning C4310: cast truncates constant value
#pragma warning( pop )
						)
					)
				)
			)
		{
			if( bDockSiteCustomizeMode )
				return TRUE;
			if( bKeyUp )
				return TRUE;
			// allow child popup track it's system menu
			CWnd * pWnd = CWnd::GetActiveWindow();
			if( pWnd->GetSafeHwnd() == NULL )
				return TRUE;
			if( pWnd->GetSafeHwnd() != _GetDockingFrameImpl()->GetSafeHwnd()
				&& (pWnd->GetStyle() & WS_POPUP) != 0
				)
				return TRUE;

			// track frame system menu
			CExtToolControlBar::g_bMenuTrackingExpanded = false;
			if( !_IsDockSiteCustomizeMode() )
			{
				_CancelFlatTracking();
				if( vkTCHAR == VK_SPACE )
				{
					if( ! TrackMainFrameSystemMenu( NULL, TRUE ) )
						return FALSE;
				} // if( vkTCHAR == VK_SPACE )
				else
				{
					ASSERT( _IsMdiApp() ); // (+ v.2.23)
					if(	   nMsg == WM_KEYDOWN
						|| nMsg == WM_KEYUP
						)
						return TRUE;
					BOOL bMax = FALSE;
					HWND hWndActiveChild = _GetActiveMdiChildWnd( bMax );
					if( hWndActiveChild == NULL )
					{
						if( !bKeyUp )
							g_bCancelNearestAltUp = true;
						return TRUE;
					}
					bool bExplicitTrackChildFrameSystemMenu = true;
					if( GetButtonsCount() > 0 )
					{
						CExtBarButton * pTBB = _GetButtonPtr( 0 );
						ASSERT_VALID( pTBB );
						if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton)) )
						{
							if( ! CExtPopupMenuWnd::IsMenuTracking() )
							{
								HWND hWndFocus = ::GetFocus();
								if( hWndFocus != NULL )
									::SendMessage( hWndFocus, WM_CANCELMODE, 0L, 0L );
							} // if( ! CExtPopupMenuWnd::IsMenuTracking() )
							VERIFY( CExtToolControlBar::TrackButtonMenu( 0 ) );
							bExplicitTrackChildFrameSystemMenu = false;
						} // if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton)) )
					} // if( GetButtonsCount() > 0 )
					if( bExplicitTrackChildFrameSystemMenu )
						if( ! TrackChildFrameSystemMenu( NULL, TRUE ) )
							return FALSE;
				} // else from if( vkTCHAR == VK_SPACE )
			}
			return TRUE;
		} // if( ( !m_bPresubclassDialogMode ) && bAlt && vkTCHAR == VK_SPACE )

		if(		( bPrintPreviewMode || bComboDropDownTracking )
			&&	(! bDockSiteCustomizeMode )
			)
			return FALSE;

		if(		(	bAlt
				||	(	m_bFlatTracking
					&&	vkTCHAR != VK_LEFT
					&&	vkTCHAR != VK_RIGHT
					&&	vkTCHAR != VK_UP
					&&	vkTCHAR != VK_DOWN
					&&	vkTCHAR != VK_F4
					&&	vkTCHAR != VK_RETURN
					&&	vkTCHAR != VK_SPACE
					&&	vkTCHAR != VK_ESCAPE
					&&	vkTCHAR != VK_TAB
					)
				)
			&&	(!bDockSiteCustomizeMode)
			)
		{ // if VK_ALT + VK_... pressed
//			if( g_bCancelNearestAltUp ) // (+ v.2.23)
//				return FALSE;
			if( bKeyUp )
			{
				if( m_bHelperTrackingCharCode )
					return FALSE;
				return TRUE;
			}
			BYTE lpKeyState[256];
			::GetKeyboardState( lpKeyState );
			UINT wScanCode = ::MapVirtualKey( vkTCHAR, 0 );
			HKL hKeyboardLayout =
				::GetKeyboardLayout(
					( ::AfxGetThread() ) -> m_nThreadID
					);
#if (defined _UNICODE)
			TCHAR szChar[2] = { _T('\0'), _T('\0') };
			::ToUnicodeEx( vkTCHAR, wScanCode, lpKeyState, szChar, 1, 1, hKeyboardLayout );
#else
			WORD nMapped = 0;
			::ToAsciiEx( vkTCHAR, wScanCode, lpKeyState, &nMapped, 1, hKeyboardLayout );
			TCHAR szChar[2] = { (TCHAR)nMapped, _T('\0') };
#endif
			::CharUpper( szChar );

			int nBtnIdx = -1;
			if( szChar[0] != 0 )
			{
				nBtnIdx = GetButtonByAccessKey( szChar[0] );
				if( nBtnIdx < 0 )
				{
					CPtrList _listVisibleToolBars;
					CFrameWnd * pDockSite = m_pDockSite;
					if( pDockSite == NULL )
						pDockSite =
							DYNAMIC_DOWNCAST(
								CFrameWnd,
								GetParent()
								);
					if( pDockSite != NULL )
					{
						ExtControlBarVector_t vBars;
						((const CExtControlBar *)this)->_GetFrameControlBars(
							false,
							vBars,
							__GFECB_FIXED|__GFECB_NONFIXED|__GFECB_DOCKED|__GFECB_FLOATED|__GFECB_VIS_ALL
							);
						for( int i = 0; i < vBars.GetSize(); i++ )
						{
							CExtToolControlBar * pToolBar =
								DYNAMIC_DOWNCAST(
									CExtToolControlBar,
									vBars[i]
									);
							if(		pToolBar->GetSafeHwnd() == NULL
								||	pToolBar == this
								)
								continue;
							ASSERT_VALID( pToolBar );
							ASSERT( pToolBar->IsVisible() );
							_listVisibleToolBars.AddTail( pToolBar );
						}
					} // if( pDockSite != NULL )
					else
					{
						for(	CWnd * pWnd = GetParent()->GetWindow( GW_CHILD );
								pWnd != NULL;
								pWnd = pWnd->GetWindow( GW_HWNDNEXT )
								)
						{
							CExtToolControlBar * pToolBar =
								DYNAMIC_DOWNCAST(
									CExtToolControlBar,
									pWnd
									);
							if(		pToolBar->GetSafeHwnd() == NULL
								||	pToolBar == this
								)
								continue;
							ASSERT_VALID( pToolBar );
							if( ! pToolBar->IsVisible() )
								continue;
							_listVisibleToolBars.AddTail( pToolBar );
						}
					} // else from if( pDockSite != NULL )
					for(	POSITION pos = _listVisibleToolBars.GetHeadPosition();
							pos != NULL;
						)
					{
						CExtToolControlBar * pToolBar = (CExtToolControlBar *)
							_listVisibleToolBars.GetNext( pos );
						int nBtnIdx = pToolBar->GetButtonByAccessKey( szChar[0] );
						if( nBtnIdx < 0 )
							continue;
						CExtBarButton * pTBB = pToolBar->GetButton( nBtnIdx );
						ASSERT_VALID( pTBB );
						ASSERT( !pTBB->IsSeparator() );
						if( ! CExtPopupMenuWnd::IsMenuTracking() )
						{
							HWND hWndFocus = ::GetFocus();
							if( hWndFocus != NULL )
								::SendMessage( hWndFocus, WM_CANCELMODE, 0L, 0L );
						} // if( ! CExtPopupMenuWnd::IsMenuTracking() )
						_CancelFlatTracking();
						if( pTBB->IsAbleToTrackMenu() )
						{
							CExtToolControlBar::g_bMenuTrackingExpanded = false;
							pToolBar->TrackButtonMenu(
								nBtnIdx,
								true,
								false,
								m_bUseFastKeyboardMenuTracking
								);
						} // if( pTBB->IsAbleToTrackMenu() )
						else
						{
							g_bCancelNearestAltUp =
								CExtPopupMenuWnd::IsKeyPressed( VK_MENU )
									? true : false;
							pTBB->OnDeliverCmd();
						} // else from if( pTBB->IsAbleToTrackMenu() )
						return TRUE;
					}
					g_bCancelNearestAltUp = true; // (+ v.2.24)
					return FALSE;
				} // if( nBtnIdx < 0 )
				else
				{
					CExtBarButton * pTBB = CExtToolControlBar::GetButton( nBtnIdx );
					ASSERT_VALID( pTBB );
					ASSERT( !pTBB->IsSeparator() );
					if( ! CExtPopupMenuWnd::IsMenuTracking() )
					{
						HWND hWndFocus = ::GetFocus();
						if( hWndFocus != NULL )
							::SendMessage( hWndFocus, WM_CANCELMODE, 0L, 0L );
					} // if( ! CExtPopupMenuWnd::IsMenuTracking() )
					_CancelFlatTracking();
					if( pTBB->IsAbleToTrackMenu() )
					{
						CExtToolControlBar::g_bMenuTrackingExpanded = false;
						CExtToolControlBar::TrackButtonMenu(
							nBtnIdx,
							true,
							false,
							m_bUseFastKeyboardMenuTracking
							);
					} // if( pTBB->IsAbleToTrackMenu() )
					else
					{
						g_bCancelNearestAltUp =
							CExtPopupMenuWnd::IsKeyPressed( VK_MENU )
								? true : false;
						pTBB->OnDeliverCmd();
					} // else from if( pTBB->IsAbleToTrackMenu() )
					return TRUE;
				} // else from if( nBtnIdx < 0 )
			} // if( szChar[0] != 0 )
			g_bCancelNearestAltUp = true; // (+ v.2.23)
			if( m_bFlatTracking ) // (+ v.2.25)
			{
				if( bAlt && m_bFlatTracking && ( vkTCHAR == VK_DOWN || vkTCHAR == VK_UP ) )
				{
				}
				else
					return TRUE;
			}
			else
				return FALSE;
		} // if( (bAlt || m_bFlatTracking) && (!bDockSiteCustomizeMode) )

		if( m_bFlatTracking && (!bDockSiteCustomizeMode) )
		{
			if( bKeyUp )
				return TRUE;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			if(		vkTCHAR == VK_LEFT
				||	vkTCHAR == VK_RIGHT
				||	vkTCHAR == VK_UP
				||	vkTCHAR == VK_DOWN
				||	vkTCHAR == VK_TAB
				||	vkTCHAR == VK_SPACE
				||	vkTCHAR == VK_ESCAPE
				||	vkTCHAR == VK_RETURN
				||	( VK_F1 <= vkTCHAR && vkTCHAR <= VK_F24 )
				)
			{
				HDWP hPassiveModeDWP = NULL;
				int nBtnIdx, nCountOfButtons = _GetButtonsCountImpl();
				for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
				{
					CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
					ASSERT_VALID( pTBB );
					pTBB->OnKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
				}
			}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			bool bUpdateIndex = false;
			TCHAR _vkTCHAR = vkTCHAR;
			if(		( vkTCHAR == VK_DOWN || vkTCHAR == VK_UP )
				&&	bAlt
				)
				_vkTCHAR = VK_F4;
			int nNewCustomFlatTrackingIndex =
				OnCalcFlatTrackingIndex(
					INT(_vkTCHAR ),
					INT( TCHAR( m_nFlatTrackingIndex ) )
					);
			if( nNewCustomFlatTrackingIndex >= 0 )
			{
				m_nFlatTrackingIndex = nNewCustomFlatTrackingIndex;
				bUpdateIndex = true;
			} // if( nNewCustomFlatTrackingIndex >= 0 )
			else
			{
				if( _vkTCHAR == VK_TAB )
				{
					if( bShift )
						_vkTCHAR = VK_LEFT;
					else
						_vkTCHAR = VK_RIGHT;
				} // if( _vkTCHAR == VK_TAB )
				switch( _vkTCHAR )
				{
				case VK_LEFT:
					bUpdateIndex = true;
					m_nFlatTrackingIndex =
						GetVisibleButton(
							m_nFlatTrackingIndex,
							FALSE
							);
				break;
				case VK_RIGHT:
					bUpdateIndex = true;
					m_nFlatTrackingIndex =
						GetVisibleButton(
							m_nFlatTrackingIndex,
							TRUE
							);
				break;
				case VK_SPACE:
				case VK_RETURN:
				case VK_F4:
				case VK_UP:
				case VK_DOWN:
				{
					if(		CExtPopupMenuWnd::IsMenuTracking()
						||	m_nFlatTrackingIndex < 0
						)
						return TRUE;
					ASSERT( m_nFlatTrackingIndex < GetButtonsCount() );
					CExtBarButton * pTBB =
						_GetButtonPtr(m_nFlatTrackingIndex);
					ASSERT_VALID( pTBB );
					if( pTBB->IsAbleToTrackMenu() )
					{
						if( ! CExtPopupMenuWnd::IsMenuTracking() )
						{
							HWND hWndFocus = ::GetFocus();
							if( hWndFocus != NULL )
								::SendMessage( hWndFocus, WM_CANCELMODE, 0L, 0L );
						} // if( ! CExtPopupMenuWnd::IsMenuTracking() )
						int nIndex = m_nFlatTrackingIndex;
						if( m_bFlatTracking )
						{
							m_bFlatTracking = false;
							OnFlatTrackingStop();
						} // if( m_bFlatTracking )
						_UpdateFlatTracking( FALSE );
						CExtToolControlBar::TrackButtonMenu(
							nIndex,
							true,
							false,
							m_bUseFastKeyboardMenuTracking
							);
					}
					else
					{
						if(		( vkTCHAR == VK_RETURN || vkTCHAR == VK_SPACE )
						//	&&	CExtCmdManager::IsCommand(
						//			pTBB->GetCmdID(true)
						//			)
							)
						{
							pTBB->OnClick( CPoint(0,0), false );
						}
						if( m_bFlatTracking )
						{
							m_bFlatTracking = false;
							OnFlatTrackingStop();
						} // if( m_bFlatTracking )
						_UpdateFlatTracking();
					}
					return TRUE;
				}
				break;
				case VK_ESCAPE:
					if( m_bFlatTracking )
					{
						m_bFlatTracking = false;
						OnFlatTrackingStop();
					} // if( m_bFlatTracking )
					_UpdateFlatTracking();
					return TRUE;
				} // switch( _vkTCHAR )
			} // else from if( nNewCustomFlatTrackingIndex >= 0 )
			if( bUpdateIndex )
			{
				int nCountOfButtons = GetButtonsCount();
				ASSERT( nCountOfButtons > 0 );
				if( m_nFlatTrackingIndex < 0 )
					m_nFlatTrackingIndex = nCountOfButtons - 1;
				else if(m_nFlatTrackingIndex >= nCountOfButtons)
					m_nFlatTrackingIndex = 0;
			}
			_UpdateFlatTracking();
			return TRUE;
		} // if( m_bFlatTracking && (!bDockSiteCustomizeMode) )

	}

	if( nMsg == WM_CONTEXTMENU )
	{
		if( g_bMenuTracking || m_bFlatTracking || bDockSiteCustomizeMode )
			return TRUE;
	}

	if( m_bFlatTracking && (!bDockSiteCustomizeMode) )
	{
		if( (nMsg >= __EXT_MFC_WM_MOUSEFIRST && nMsg <= __EXT_MFC_WM_MOUSELAST)
				&& nMsg != WM_MOUSEMOVE
			)
			_CancelFlatTracking();
	} // if( m_bFlatTracking && (!bDockSiteCustomizeMode) )
	
	return FALSE;
}

CRect CExtMenuControlBar::_GetChildFrameSysIconRect()
{
CRect rc;
	rc.SetRectEmpty();
BOOL bMax = FALSE;
HWND hWndActiveChild =
		_GetActiveMdiChildWnd( bMax );
	if( hWndActiveChild == NULL )
		return rc;
	ASSERT( ::IsWindow(hWndActiveChild) );
	if( bMax )
		return rc;
CWnd * pWnd = CWnd::FromHandlePermanent(hWndActiveChild);
	if( pWnd == NULL )
	{
		ASSERT( FALSE );
		return rc;
	}
	ASSERT_VALID( pWnd );

	// FrameFeatures does not use CMDIChildWnd
//	ASSERT( pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) );
//CFrameWnd * pFrame = 
//		DYNAMIC_DOWNCAST(
//			CFrameWnd,
//			pWnd
//			);
//	ASSERT( pFrame != NULL );
//	if( pFrame == NULL )
//	{
//		ASSERT( FALSE );
//		return rc;
//	}
//	ASSERT_VALID( pFrame );
		
CRect rcWnd,rcClient,rcHelper;
	pWnd->GetWindowRect( &rcWnd );
	pWnd->GetClientRect( &rcClient );
	rcHelper = rcWnd;
	pWnd->ScreenToClient( &rcHelper );
	ASSERT( rcHelper.top <= rcClient.top );
	int yDiff = rcClient.top - rcHelper.top;
	rc.SetRect(
		rcWnd.left + GetSystemMetrics(SM_CYFRAME),
		rcWnd.top + GetSystemMetrics(SM_CXFRAME),
		rcWnd.left + yDiff,
		rcWnd.top + yDiff
		);
	return rc;
}

CRect CExtMenuControlBar::_GetMainFrameSysIconRect()
{
CRect rc;
CRect rcWnd,rcClient,rcHelper;
CFrameWnd * pFrame =
		_GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
	pFrame->GetWindowRect( &rcWnd );
	pFrame->GetClientRect( &rcClient );
	rcHelper = rcWnd;
	pFrame->ScreenToClient( &rcHelper );
	ASSERT( rcHelper.top <= rcClient.top );
	int yDiff = rcClient.top - rcHelper.top;
	rc.SetRect(
		rcWnd.left + GetSystemMetrics(SM_CYFRAME),
		rcWnd.top + GetSystemMetrics(SM_CXFRAME),
		rcWnd.left + yDiff,
		rcWnd.top + yDiff
		);
	return rc;
}

BOOL CExtMenuControlBar::_TrackFrameSystemMenu(
	CWnd * pFrame,
	CPoint * pPoint, // = NULL, // NULL means calc menu track area automatically
	BOOL bSelectAny, // = FALSE
	LPCRECT rcExcludeArea, // = NULL
	UINT nTrackFlags, // = (UINT)(-1)
	BOOL bCombinedMode // = FALSE
	)
{
	_CancelFlatTracking();

///	if( nTrackFlags == (UINT)(-1) )
///		nTrackFlags = TPMX_BOTTOMALIGN;
	ASSERT_VALID( pFrame );

bool bPrevTBMT = CExtToolControlBar::g_bMenuTracking;
int nTrackingIdxOld = m_nBtnIdxMenuTracking;

//	if( g_bMenuTracking )
		CExtToolControlBar::_CloseTrackingMenus();
//	if(	CExtPopupMenuWnd::IsMenuTracking() )
//		CExtPopupMenuWnd::CancelMenuTracking();
CMenu * pSysMenu = pFrame->GetSystemMenu( FALSE );
	if( pSysMenu == NULL )
		return FALSE;
	ASSERT( ::IsMenu(pSysMenu->GetSafeHmenu()) );

CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	if( ! pPopup->CreatePopupMenu(
			pFrame->GetSafeHwnd()
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	if( ! pPopup->UpdateFromMenu(
			pFrame->GetSafeHwnd(),
			pSysMenu,
			false,
			true,
			true
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( pFrame->GetSafeHwnd(), GWL_EXSTYLE );
bool bRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;

CPoint point;
	if( pPoint != NULL)
		point = *pPoint;
	else
	{
		CRect rcWnd,rcClient,rcHelper;
		pFrame->GetWindowRect( &rcWnd );
		pFrame->GetClientRect( &rcClient );
		rcHelper = rcWnd;
		pFrame->ScreenToClient( &rcHelper );
		//ASSERT( rcHelper.top <= rcClient.top );
		int yDiff = rcClient.top - rcHelper.top;
		int n = 0;

		WINDOWPLACEMENT _wp;
		::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
		_wp.length = sizeof(WINDOWPLACEMENT);
		VERIFY(
			::GetWindowPlacement(
				GetSafeHwnd(),
				&_wp
				)
			);
		if( _wp.showCmd != SW_SHOWMAXIMIZED ) 
			n = ::GetSystemMetrics( SM_CXFRAME );

		point = rcWnd.TopLeft();
		if( bRTL )
		{
			point = CPoint( rcWnd.right, rcWnd.top );
			n = -n;
		}
		point.x += n;
		point.y += yDiff;
	}
	if( nTrackFlags == (UINT)(-1) )
		nTrackFlags = 0;
	if(	(nTrackFlags & TPMX_ALIGN_MASK ) == 0 )
		nTrackFlags = bRTL ? TPMX_RIGHTALIGN : TPMX_TOPALIGN;

	if( bSelectAny )
		nTrackFlags |= TPMX_SELECT_ANY;
	if( bPrevTBMT )
		nTrackFlags |= TPMX_NO_HIDE_RARELY;
	pFrame->SetFocus();

	CExtToolControlBar::g_bMenuTracking =
		bSelectAny ? bPrevTBMT : false; // (+ v.2.23)
	if( nTrackingIdxOld >= 0 )
		_SwitchMenuTrackingIndex( nTrackingIdxOld );

CWnd * pWndNotify = m_pDockSite;
	if( pWndNotify == NULL )
		pWndNotify = GetParent();
	if( pWndNotify != NULL )
		pWndNotify->SendMessage(
			g_nMsgPrepareSystemMenu,
			WPARAM(pPopup),
			LPARAM(pFrame)
			);

	if(	bRTL &&	!bCombinedMode )
		point.x += pPopup->OnQueryMenuShadowSize();

	CRect rcExclude( rcExcludeArea );
	if( rcExcludeArea == NULL )
		rcExclude.SetRect( point, point );

	pPopup->m_hWndNotifyMenuClosed = GetSafeHwnd();
	if(	! pPopup->TrackPopupMenu(
			nTrackFlags 
				| ( bCombinedMode ? TPMX_COMBINE_DEFAULT : 0) 
				| TPMX_OWNERDRAW_FIXED
				| TPMX_SYSMENU,
			point.x,point.y,
			rcExclude,
			bCombinedMode ?
				this : NULL,
			bCombinedMode ?
				CExtToolControlBar::_CbPaintCombinedContent : NULL,
			NULL,
			bCombinedMode ? true : false
			)
		)
	{
		//delete pPopup;
		CExtToolControlBar::_CloseTrackingMenus();
		return FALSE;
	}

	g_bMenuTracking = true;

	return TRUE;
}

BOOL CExtMenuControlBar::TrackChildFrameSystemMenu(
	CPoint * pPoint, // = NULL, // NULL means calc any track area automatically
	BOOL bSelectAny // = FALSE
	)
{
	if( ! m_bAllowProfChildFrameSysmenu )
		return FALSE;
	if( ! _IsMdiApp() )
	{
		ASSERT( FALSE );
		return FALSE;
	} // if( ! _IsMdiApp() )

	if( GetButtonsCount() > 0 )
	{
		CExtBarButton * pTBB = _GetButtonPtr( 0 );
		ASSERT_VALID( pTBB );
		if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton)) )
		{
			if( ! CExtPopupMenuWnd::IsMenuTracking() )
			{
				HWND hWndFocus = ::GetFocus();
				if( hWndFocus != NULL )
					::SendMessage( hWndFocus, WM_CANCELMODE, 0L, 0L );
			} // if( ! CExtPopupMenuWnd::IsMenuTracking() )
			if( ! TrackButtonMenu( 0 ) )
			{
				ASSERT( FALSE );
				return FALSE;
			} // if( ! TrackButtonMenu( 0 ) )
			return TRUE;
		} // if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton)) )
	} // if( GetButtonsCount() > 0 )

BOOL bMax = FALSE;
HWND hWndActiveChild = _GetActiveMdiChildWnd( bMax );
	if( hWndActiveChild == NULL )
		return FALSE;
	ASSERT( ::IsWindow(hWndActiveChild) );
CWnd * pWnd = CWnd::FromHandlePermanent(hWndActiveChild);
	if( pWnd == NULL )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT_VALID( pWnd );

	// FrameFeatures does not use CMDIChildWnd
//	ASSERT( pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) );
//CFrameWnd * pFrame = 
//		DYNAMIC_DOWNCAST(
//			CFrameWnd,
//			pWnd
//			);
//	ASSERT( pFrame != NULL );
//	if( pFrame == NULL )
//	{
//		ASSERT( FALSE );
//		return FALSE;
//	}
//	ASSERT_VALID( pFrame );

	g_bMenuTracking = true;
	_SwitchMenuTrackingIndex();
	m_bSysMenuTracking =
		_TrackFrameSystemMenu(
			pWnd,
			pPoint,
			bSelectAny
			) ? true : false;
	return m_bSysMenuTracking ? TRUE : FALSE;
}

BOOL CExtMenuControlBar::TrackMainFrameSystemMenu(
	CPoint * pPoint, // = NULL // NULL means calc menu track area automatically
	BOOL bSelectAny // = FALSE
	)
{
	if( ! m_bAllowProfMainFrameSysmenu )
		return FALSE;
CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
CWnd * pWndForPlacement = stat_GetWndForPlacement( pFrame );
//	if( pWndForPlacement->m_hWnd != pFrame->m_hWnd )
//		return FALSE;
	g_bMenuTracking = true;
	_SwitchMenuTrackingIndex();
	m_bSysMenuTracking =
		_TrackFrameSystemMenu(
			pWndForPlacement, // pFrame
			pPoint,
			bSelectAny
			) ? true : false;
	return m_bSysMenuTracking ? TRUE : FALSE;
}

BOOL CExtMenuControlBar::TrackButtonMenu(
	int nIndex,
	BOOL bSelectAny, // = FALSE
	BOOL bDelayedTracking, // = FALSE
	BOOL bNoAnimation // = FALSE
	)
{
	bSelectAny;
	bDelayedTracking;

	if(		g_bMenuTracking
		&&	m_nBtnIdxMenuTracking == nIndex
		)
		return TRUE;

	if( ! SafeDisplayBar() )
		return FALSE;

	_CancelFlatTracking();

	if( m_bMenuBarUsesDelayedButtonMenuTracking )
		PostMessage(
			g_nMsgTrackButtonMenu,
			(WPARAM)nIndex,
			(LPARAM)bNoAnimation
			);
	else
		SendMessage(
			g_nMsgTrackButtonMenu,
			(WPARAM)nIndex,
			(LPARAM)bNoAnimation
			);
	return TRUE;
}

LRESULT CExtMenuControlBar::OnTrackButtonMenu(WPARAM wParam,LPARAM lParam)
{
	if( !(	::IsWindowEnabled(
				m_bPresubclassDialogMode
					? GetOwner()->GetSafeHwnd()
					: _GetDockingFrameImpl()->GetSafeHwnd()
				)
			)
		) 
		return 0;
int nCountOfButtons = GetButtonsCount();
	ASSERT( nCountOfButtons >= 0 );
	if( nCountOfButtons == 0 )
		return 0;
int nIndex = int(wParam);
	if( nIndex < 0 || nIndex>=nCountOfButtons )
	{
		_CancelFlatTracking();
		_CloseTrackingMenus();
		return 0;
	}
CExtBarButton * pTBB = _GetButtonPtr(nIndex);
	ASSERT_VALID( pTBB );
BOOL bDroppedMode =
		CExtPopupMenuWnd::IsMenuTracking();
	if( !pTBB->IsAbleToTrackMenu() )
		bDroppedMode = FALSE;
	if( bDroppedMode )
	{
		CExtToolControlBar::TrackButtonMenu(
			nIndex,
			true,
			false,
			(lParam != 0) ? true : false
			);
		return 0;
	}
	_CloseTrackingMenus();
	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();

	if( ! m_bFlatTracking )
	{
		HWND hWndFocus = ::GetFocus();
		if( hWndFocus != NULL )
			::SendMessage( hWndFocus, WM_CANCELMODE, 0L, 0L );
	} // if( ! m_bFlatTracking )

	m_nFlatTrackingIndex = nIndex;
	if( ! m_bFlatTracking )
	{
		m_bFlatTracking = true;
		HDWP hPassiveModeDWP = NULL;
		OnFlatTrackingStart( hPassiveModeDWP );
	} // if( ! m_bFlatTracking )
	_UpdateFlatTracking();

	return 0;
}

int CExtMenuControlBar::_FlatTrackingGet() const
{
	ASSERT_VALID( this );
	return m_bFlatTracking;
}

void CExtMenuControlBar::_FlatTrackingSet( bool bFlatTracking )
{
	ASSERT_VALID( this );
	m_bFlatTracking = bFlatTracking;
}

int CExtMenuControlBar::_FlatTrackingIndexGet() const
{
	ASSERT_VALID( this );
	return m_nFlatTrackingIndex;
}

void CExtMenuControlBar::_FlatTrackingIndexSet( int nFlatTrackingIndex )
{
	ASSERT_VALID( this );
	m_nFlatTrackingIndex = nFlatTrackingIndex;
}

void CExtMenuControlBar::_CancelFlatTracking(
	BOOL bRepaint // = TRUE
	)
{
	ASSERT_VALID( this );
	if( ! m_bFlatTracking )
		return;
	m_bFlatTracking = false;
	OnFlatTrackingStop();
	_UpdateFlatTracking( bRepaint );
}

void CExtMenuControlBar::OnFlatTrackingStart(
	HDWP & hPassiveModeDWP
	)
{
	ASSERT_VALID( this );
	CExtToolControlBar::OnFlatTrackingStart( hPassiveModeDWP );
}

void CExtMenuControlBar::OnFlatTrackingStop()
{
	ASSERT_VALID( this );
	CExtToolControlBar::OnFlatTrackingStop();
}

void CExtMenuControlBar::_UpdateFlatTracking(
	BOOL bRepaint // = TRUE
	)
{
	ASSERT_VALID( this );
	if(		m_nFlatTrackingIndex < 0
		&&	m_nOldTrackingIndex < 0
		)
	{
		if( m_bFlatTracking )
		{
			m_bFlatTracking = false;
			OnFlatTrackingStop();
		} // if( m_bFlatTracking )
		return;
	}

HWND hWndCurrentFocus = ::GetFocus();
	if( hWndCurrentFocus != NULL )
	{
		CWnd * pWndTest = CWnd::FromHandlePermanent( hWndCurrentFocus );
		if( pWndTest == NULL )
		{
			HWND hWnd = ::GetParent( hWndCurrentFocus );
			if( hWnd != NULL )
				pWndTest = CWnd::FromHandlePermanent( hWnd );
		} // if( pWndTest == NULL )
		if( pWndTest != NULL )
		{
			CComboBox * pComboTest =
				DYNAMIC_DOWNCAST(
					CComboBox,
					pWndTest
					);
			if( pComboTest == NULL )
			{
				pWndTest = pWndTest->GetParent();
				if( pWndTest != NULL )
				{
					pComboTest =
						DYNAMIC_DOWNCAST(
							CComboBox,
							pWndTest
							);
				}
			} // if( pComboTest == NULL )
			if(		pComboTest != NULL
				&&	(pComboTest->GetStyle() & (CBS_DROPDOWN|CBS_DROPDOWNLIST)) != 0
				&&	pComboTest->GetDroppedState()
				)
				pComboTest->ShowDropDown( FALSE );
		} // if( pWndTest != NULL )
	} // if( hWndCurrentFocus != NULL )

int nCountOfButtons = GetButtonsCount();
	if( !m_bFlatTracking )
	{
		if( m_nFlatTrackingIndex >= 0 )
		{
			ASSERT( m_nFlatTrackingIndex < nCountOfButtons );
			CExtBarButton * pTBB =
				_GetButtonPtr(m_nFlatTrackingIndex);
			ASSERT_VALID( pTBB );
			pTBB->ModifyStyle(0,TBBS_PRESSED);
			INT nIndexRepaint = m_nFlatTrackingIndex;
			m_nFlatTrackingIndex = m_nOldTrackingIndex = -1;
			if( bRepaint )
			{
				_InvalidateButton( nIndexRepaint );
				///UpdateWindow();
			}
		}
//		if( g_bMenuTracking )
//			_CloseTrackingMenus();
		return;
	} // if( !m_bFlatTracking )
	if( m_nFlatTrackingIndex != m_nOldTrackingIndex )
	{
		if( m_nOldTrackingIndex >= 0 && m_nOldTrackingIndex < nCountOfButtons )
		{
			CExtBarButton * pTBB =
				_GetButtonPtr(m_nOldTrackingIndex);
			ASSERT_VALID( pTBB );
			pTBB->ModifyStyle(0,TBBS_PRESSED|TBBS_CHECKED);
			if( bRepaint )
				_InvalidateButton(m_nOldTrackingIndex);
		} // if( m_nOldTrackingIndex >= 0 && m_nOldTrackingIndex < nCountOfButtons )
		if( m_nFlatTrackingIndex >= 0 )
		{
			ASSERT( m_nFlatTrackingIndex < nCountOfButtons );
			CExtBarButton * pTBB =
				_GetButtonPtr(m_nFlatTrackingIndex);
			ASSERT_VALID( pTBB );
			pTBB->ModifyStyle(TBBS_PRESSED);
			if( bRepaint )
				_InvalidateButton(m_nFlatTrackingIndex);
		}
		m_nOldTrackingIndex = m_nFlatTrackingIndex;
		AnimationSite_PassAnimationEvents();
		///UpdateWindow();
	} // if( m_nFlatTrackingIndex != m_nOldTrackingIndex )
}

void CExtMenuControlBar::_DelaySyncActiveMdiChild()
{
	ASSERT_VALID( this );
	if(		m_bDelayedSyncActiveMdiChild
		||	m_bDelayedUpdateMenuBar
		||	m_hWnd == NULL
		||	(! ::IsWindow(m_hWnd) )
		)
		return;
	m_bDelayedSyncActiveMdiChild = true;
	PostMessage( WM_TIMER, 13, 0 );
}

void CExtMenuControlBar::_DelayUpdateMenuBar()
{
	ASSERT_VALID( this );
	if(		m_bDelayedUpdateMenuBar
		||	m_hWnd == NULL
		||	(! ::IsWindow(m_hWnd) )
		)
		return;
	m_bDelayedSyncActiveMdiChild = false;
	m_bDelayedUpdateMenuBar = true;
	PostMessage( WM_TIMER, 14, 0 );
}

void CExtMenuControlBar::OnTimer(__EXT_MFC_UINT_PTR nIDEvent) 
{
	__PROF_UIS_MANAGE_STATE;

	if( nIDEvent == 13 )
	{
		KillTimer( nIDEvent );
		if( m_bDelayedSyncActiveMdiChild )
		{
			m_bDelayedSyncActiveMdiChild = false;
			_SyncActiveMdiChild();
		}
		return;
	} // if( nIDEvent == 13 )

	if( nIDEvent == 14 )
	{
		KillTimer( nIDEvent );
		if( m_bDelayedUpdateMenuBar )
		{
			m_bDelayedUpdateMenuBar = false;
			VERIFY( UpdateMenuBar( TRUE ) );
		} // if( m_bDelayedUpdateMenuBar )
		return;
	} // if( nIDEvent == 14 )

	if( nIDEvent == 15 )
	{
		KillTimer( nIDEvent );
		ASSERT( m_bMdiApp );
		HWND hWndMdiClient = _GetHwndMdiArea();
		if( hWndMdiClient != NULL )
		{
			KillTimer( nIDEvent );
			VERIFY( _InitMdiChildHook( hWndMdiClient ) );
		} // if( hWndMdiClient != NULL )
		return;
	} // if( nIDEvent == 15 )

	if( nIDEvent == 16 )
	{
		KillTimer( nIDEvent );
		_KillFrameMenu();
		return;
	} // if( nIDEvent == 16 )

	CExtToolControlBar::OnTimer(nIDEvent);
}

bool CExtMenuControlBar::_InitMdiChildHook( HWND hWndMdiClient )
{
	ASSERT( m_bMdiApp );
	ASSERT( ::IsWindow(hWndMdiClient) );
	if( ! SetupHookWndSink( hWndMdiClient ) )
	{
		ASSERT( FALSE );
		return false;
	} // if( ! SetupHookWndSink( hWndMdiClient ) )
	::SetWindowPos(
		hWndMdiClient, NULL, 0, 0, 0, 0,
		SWP_FRAMECHANGED
			|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOCOPYBITS
			|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE
		);
	VERIFY( _SyncActiveMdiChild() );
	return true;
}


bool CExtMenuControlBar::_IsSimplifiedDropDownButtons() const
{
	ASSERT_VALID( this );
	return true;
}

bool CExtMenuControlBar::_OnMouseMoveMsg(UINT nFlags, CPoint point) 
{
	if( ! m_bFlatTracking )
		return CExtToolControlBar::_OnMouseMoveMsg(nFlags, point);
int nIndex = HitTest( point );
	if(		nIndex == m_nFlatTrackingIndex
		||	nIndex < 0
		)
		return true;
	TrackButtonMenu( nIndex );
	return true;
}

void CExtMenuControlBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( !m_bFlatTracking )
	{
		int nIndex = HitTest(point);
		if( nIndex >= 0 )
		{
			int nMenuIndex = GetMenuTrackingButton();
			if( nIndex == nMenuIndex )
				return;
		}
	}

	CExtToolControlBar::OnLButtonDown(nFlags, point);
}

void CExtMenuControlBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
BOOL bRepaint = FALSE;
	if( m_bFlatTracking )
	{
		m_bFlatTracking = false;
		OnFlatTrackingStop();
		_UpdateFlatTracking( FALSE );
		bRepaint = TRUE;
	}
	CExtToolControlBar::OnLButtonUp(nFlags, point);
	if( bRepaint )
		Invalidate();
}

void CExtMenuControlBar::_RecalcLayoutImpl()
{
	CExtToolControlBar::_RecalcLayoutImpl();
	if(		m_pDockSite != NULL
		&&	IsFloating()
		&&	( ! ((InternalFriendlyFrameWnd*)m_pDockSite)->IsInRecalcLayout() )
		&&	(! CExtControlBar::ProfileBarStateIsSerializing( m_pDockSite ) )
		&&	(m_pDockSite->GetStyle()&WS_VISIBLE) != 0
		)
	{
		WINDOWPLACEMENT _wp;
		ZeroMemory( &_wp, sizeof(WINDOWPLACEMENT) );
		_wp.length = sizeof(WINDOWPLACEMENT);
		CWnd * pWndPlacement = stat_GetWndForPlacement( m_pDockSite );
		ASSERT_VALID( pWndPlacement );
		pWndPlacement->GetWindowPlacement( &_wp );
		if( _wp.showCmd == SW_SHOWMINIMIZED )
		{
			//m_pDockSite->DelayRecalcLayout();
			return;
		}
		m_pDockSite->RecalcLayout();
	}
}

bool CExtMenuControlBar::OnQueryShiftRTL() const
{
	ASSERT_VALID( this );
	return ((g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0) ? true : false;
}

bool CExtMenuControlBar::OnQueryRevertRTL() const
{
	ASSERT_VALID( this );
	return ((g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0) ? true : false;
}

void CExtMenuControlBar::_RecalcPositionsImpl()
{
	CExtToolControlBar::_RecalcPositionsImpl();

	if(		g_bHideMenuBarExpandContentButton
		&&	m_pRightBtn != NULL
		)
	{
		ASSERT_VALID( m_pRightBtn );
		if( m_pRightBtn->GetButtons().GetSize() == 0 )
			m_pRightBtn->Show( FALSE );
	}

CWnd * pWndParent = GetParent();
	ASSERT_VALID( pWndParent );
BOOL bInDockBar = pWndParent->IsKindOf(RUNTIME_CLASS(CDockBar));
	if( IsFloating() && bInDockBar )
		return;
int nCountOfButtons = GetButtonsCount();
	if( nCountOfButtons <= 1 )
		return;
int nRightIndex = nCountOfButtons - 1;
CExtBarMdiRightButton * pRightTBB =
		DYNAMIC_DOWNCAST(
			CExtBarMdiRightButton,
			_GetButtonPtr( nRightIndex )
			);
	if( pRightTBB == NULL && nCountOfButtons > 1 )
	{
		nRightIndex--;
		pRightTBB =
			DYNAMIC_DOWNCAST(
				CExtBarMdiRightButton,
				_GetButtonPtr( nRightIndex )
				);
	} // if( pRightTBB == NULL && nCountOfButtons > 1 )
	if( pRightTBB == NULL )
		return;
	if( !pRightTBB->IsVisible() )
		return;
	ASSERT( nRightIndex >= 1 );

#ifdef _DEBUG
CExtBarButton * pPrevTBB =
		_GetButtonPtr( nRightIndex - 1 );
	ASSERT_VALID( pPrevTBB );
///	ASSERT( pPrevTBB->IsVisible() );
#endif // _DEBUG

CExtBarButton * pExpandTBB = NULL;
CRect rcBtnExpand(0,0,0,0);
int nAlignMargin = 0;
BOOL bHorz = 
	bInDockBar
		? IsDockedHorizontally()
		: (	(m_dwStyle & (CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM))
				? true
				: false
			)
		;
CRect rcClient;
	GetClientRect( &rcClient );
	
	if( nRightIndex < (nCountOfButtons-1) )
	{
		pExpandTBB = _GetButtonPtr( nRightIndex + 1 );
		ASSERT_VALID( pExpandTBB );
		if( ! pExpandTBB->IsKindOf( RUNTIME_CLASS(CExtBarContentExpandButton)) )
			pExpandTBB = NULL;
	} // if( nRightIndex < (nCountOfButtons-1) )
	
	if( pExpandTBB != NULL )
	{ // if content-expand button exists
		if( pExpandTBB->IsVisible() )
		{
			rcBtnExpand = pExpandTBB->Rect();
			nAlignMargin =
				bHorz ? rcBtnExpand.left : rcBtnExpand.top;
		} // if( pExpandTBB->IsVisible() )
		else
		{
			pExpandTBB = NULL;
			nAlignMargin =
				bHorz ? rcClient.right : rcClient.bottom;
		} // else from if( pExpandTBB->IsVisible() )
		nAlignMargin -= __MENUBAR_RIGHT_ADJUST_GAP;
	} // if content-expand button exists
	else
	{  // if content-expand button DOES NOT exists
		nAlignMargin =
			bHorz ? rcClient.right : rcClient.bottom;
		nAlignMargin -= __MENUBAR_RIGHT_ADJUST_GAP;
	}  // if content-expand button DOES NOT exists
CRect rcBtnRight = pRightTBB->Rect();
	if( bHorz && rcBtnRight.right >= nAlignMargin )
		return;
	if( (!bHorz) && rcBtnRight.bottom >= nAlignMargin )
		return;
	if( bHorz )
		rcBtnRight.OffsetRect(
			nAlignMargin - rcBtnRight.right,
			0
			);
	else
		rcBtnRight.OffsetRect(
			0,
			nAlignMargin - rcBtnRight.bottom
			);

	if( pExpandTBB == NULL )
	{
		pRightTBB->SetRect(rcBtnRight);
		return;
	}

	ASSERT( !rcBtnExpand.IsRectEmpty() );
CRect rcBtnExpand2(
		bHorz ? rcBtnRight.left : rcBtnExpand.left,
		bHorz ? rcBtnExpand.top : rcBtnRight.top,
		bHorz ?
			rcBtnRight.left + rcBtnExpand.Width()
			:
			rcBtnExpand.left + rcBtnExpand.Width()
			,
		bHorz ?
			rcBtnExpand.top + rcBtnExpand.Height()
			:
			rcBtnRight.top + rcBtnExpand.Height()
		);
	rcBtnRight.OffsetRect(
		bHorz ? rcBtnExpand.right - rcBtnRight.right : 0,
		bHorz ? 0 : rcBtnExpand.bottom - rcBtnRight.bottom
		);
	pRightTBB->SetRect(rcBtnRight);
	pExpandTBB->SetRect(rcBtnExpand2);

	if( g_bHideMenuBarExpandContentButton )
		pExpandTBB->Show( FALSE );
}

bool CExtMenuControlBar::_GetFullRowMode() const
{
	ASSERT_VALID( this );
	return true;
}

bool CExtMenuControlBar::OnQueryMultiRowLayout() const
{
	ASSERT_VALID( this );
	if(		m_bPresubclassDialogMode
//		||	m_pDockSite == NULL
		)
		return false;
	if( m_pDockBar != NULL )
	{
		if( IsFloating() )
			return false;
		if( _IsDockSiteCustomizeMode() )
			return true;
	} // if( m_pDockBar != NULL )
	return m_bMultiRowLayout;
}

CExtBarContentExpandButton * CExtMenuControlBar::OnCreateBarRightBtn()
{
	ASSERT_VALID( this );
	if( m_bMultiRowLayout )
		return NULL;
	return CExtToolControlBar::OnCreateBarRightBtn();
}

void CExtMenuControlBar::_Dragging_OnStart()
{
	_CancelFlatTracking();
	if( g_bMenuTracking )
		_CloseTrackingMenus();
	CExtToolControlBar::_Dragging_OnStart();
}

void CExtMenuControlBar::_Dragging_OnStop(
	CExtControlBar::InternalDraggingState_t & _dsOld,
	CExtControlBar::InternalDraggingState_t & _dsNew,
	bool bCancel
	)
{
	CExtToolControlBar::_Dragging_OnStop(
		_dsOld,
		_dsNew,
		bCancel
		);
	if( _IsMdiApp() )
	{
		_InstallMdiDocButtons();
		_RecalcPositionsImpl();
	} // if( _IsMdiApp() )
}

HWND CExtMenuControlBar::_GetHwndMainFrame()
{
	if( m_bPresubclassDialogMode )
		return NULL;

CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
	ASSERT( pFrame->GetSafeHwnd() != NULL );
	return pFrame->GetSafeHwnd();
}

HWND CExtMenuControlBar::_GetHwndPlacement()
{
	if( !m_bPresubclassDialogMode )
		return NULL;
	return m_hWndHelper;
}

HWND CExtMenuControlBar::_GetHwndChildFrame()
{
	if( m_bPresubclassDialogMode )
		return NULL;
	return m_hWndHelper;
}

HWND CExtMenuControlBar::_GetHwndMdiArea()
{
	if( m_bPresubclassDialogMode )
		return NULL;

	ASSERT( _IsMdiApp() );

CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
	ASSERT( pFrame->GetSafeHwnd() != NULL );

	// FrameFeatures does not use CMDIFrameWnd
	// ASSERT_KINDOF( CMDIFrameWnd, pFrame );
CMDIFrameWnd * pMdiFrame =
		DYNAMIC_DOWNCAST(
			CMDIFrameWnd,
			pFrame
			);
	if( pMdiFrame == NULL )
	{
		for(	HWND hWnd = ::GetWindow( pFrame->GetSafeHwnd(), GW_CHILD );
				hWnd != NULL;
				hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
			)
		{
			ASSERT( ::IsWindow( hWnd ) );
			TCHAR strClassName[ 512 ];
			::memset( strClassName, 0, sizeof(strClassName) );
			::GetClassName( hWnd, strClassName, 512 );
			if( _tcslen( strClassName ) == 0 )
				continue;
			__EXT_MFC_STRLWR( strClassName, 512 );
			//if( _tcscmp( strClassName, _T("mdiclient") ) == 0 )
			if( _tcsstr( strClassName, _T("mdiclient") ) != NULL )
				return hWnd;
		}
		return NULL;
	} // if( pMdiFrame == NULL )
	//ASSERT_VALID( pMdiFrame );

	ASSERT( pMdiFrame->m_hWndMDIClient != NULL );
	ASSERT( ::IsWindow( pMdiFrame->m_hWndMDIClient ) );
	return pMdiFrame->m_hWndMDIClient;
}

int CExtMenuControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CExtToolControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

CFrameWnd * pFrame = _GetDockingFrameImpl();
	if( pFrame == NULL )
	{
		m_bPresubclassDialogMode = true;
		if( m_hWndHelper == NULL )
		{
			m_hWndHelper = ::GetParent( m_hWnd );
			ASSERT( m_hWndHelper != NULL );
			ASSERT( ::IsWindow( m_hWndHelper ) );
			if( !IsHookedWindow( m_hWndHelper ) )
				SetupHookWndSink( m_hWndHelper );
		} // if( m_hWndHelper == NULL )
		return 0;
	}

	ASSERT_VALID( pFrame );
	ASSERT( pFrame->GetSafeHwnd() != NULL );

	ASSERT( m_menuFrame.GetSafeHmenu() == NULL );
	ASSERT( m_menuDoc.GetSafeHmenu() == NULL );

	if( (pFrame->GetStyle()&WS_CHILD) == 0 )
	{
		HMENU hFrameMenu = pFrame->GetMenu()->GetSafeHmenu();
		if( hFrameMenu != NULL )
		{
			m_menuFrame.Attach( hFrameMenu );
			_UpdateMenuBar( FALSE );
			pFrame->SetMenu( NULL );
		}
	} // if( (pFrame->GetStyle()&WS_CHILD) == 0 )

HWND hWndMainFrame = _GetHwndMainFrame();
	if( hWndMainFrame != NULL )
	{
		CWnd * pWndForPlacement = stat_GetWndForPlacement( CWnd::FromHandle(hWndMainFrame) );
		if( ! SetupHookWndSink( pWndForPlacement->m_hWnd ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		if( pWndForPlacement->m_hWnd != hWndMainFrame )
		{
			if( ! SetupHookWndSink( hWndMainFrame ) )
			{
				ASSERT( FALSE );
				return FALSE;
			}
		}

		// FrameFeatures does not use CMDIFrameWnd
		if(		pFrame->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))
			||	_IsMdiApp()
			)
		{ // if MDI App
			m_bMdiApp = true;
			HWND hWndMdiClient = _GetHwndMdiArea();
			if( hWndMdiClient != NULL )
			{
				VERIFY( _InitMdiChildHook( hWndMdiClient ) );
			}
			else
				SetTimer( 15, 10, NULL );

//			// pass message loop for smooth MDI client area update reasons
//			//CExtPopupMenuWnd::PassMsgLoop( CExtControlBar::g_bEnableOnIdleCalls );
//			MSG msg;
//			int i = 0;
//			for( ; PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) && i < 1000; i++ )
//			{
//				if( !AfxGetThread()->PumpMessage() )
//					break;
//				if( msg.message == WM_TIMER && msg.wParam == 15 )
//					break;
//			} // while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		} // if( pFrame->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) ...

	} // if( hWndMainFrame != NULL )


	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarMdiDocButton

IMPLEMENT_DYNCREATE(CExtBarMdiDocButton,CExtBarButton)

CExtBarMdiDocButton::CExtBarMdiDocButton(
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	)
	: CExtBarButton( pBar, nCmdID, nStyle )
	, m_hWndActiveMdiChild( NULL )
	, m_bHandleDblClickAsCloseCmd( true )
{
}

CExtBarMdiDocButton::~CExtBarMdiDocButton()
{
}

#if (!defined __EXT_MFC_NO_RIBBON_BAR)

bool CExtBarMdiDocButton::IsRibbonPaintingMode() const
{
	ASSERT_VALID( this );
	return false;
}

CSize CExtBarMdiDocButton::RibbonILV_CalcSize(
	CDC & dc,
	INT nILV // = -1 // -1 use current visual level
	) const
{
	ASSERT_VALID( this );
	nILV;
	return
		( const_cast < CExtBarMdiDocButton * > ( this ) )
		-> CalculateLayout( dc, CSize(0,0), IsHorzBarOrientation() );
}

#endif //  (!defined __EXT_MFC_NO_RIBBON_BAR)

CSize CExtBarMdiDocButton::CalculateLayout(
	CDC & dc,
	CSize sizePreCalc,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( GetBar()->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
	ASSERT_VALID( (&dc) );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
	dc;
	sizePreCalc;
	bHorz;
CSize _size( 16, 16 );
CExtCmdIcon * pIcon = GetIconPtr();
	if(		pIcon != NULL
		&&	(! pIcon->IsEmpty() )
		)
		_size = pIcon->GetSize();
	_size.cx += 3;
	_size.cy += 3;
	return _size;
}

bool CExtBarMdiDocButton::CanStartCommandDrag()
{
	ASSERT_VALID( this );
	return false;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * CExtBarMdiDocButton::GetCmdNode(
	bool bInitial // = false
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	bInitial;
//	ASSERT( m_pCmdNodeI == NULL );
//	ASSERT( m_pCmdNodeC == NULL );
//	return NULL;
	return CExtBarButton::GetCmdNode( bInitial );
}
void CExtBarMdiDocButton::SetBasicCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	ASSERT( m_pCmdNodeI == NULL );
//	ASSERT( m_pCmdNodeC == NULL );
//	ASSERT( FALSE ); // should be never called
//	pNode;
	CExtBarButton::SetBasicCmdNode( pNode );
}
void CExtBarMdiDocButton::SetCustomizedCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	ASSERT( m_pCmdNodeI == NULL );
//	ASSERT( m_pCmdNodeC == NULL );
//	ASSERT( FALSE ); // should be never called
//	pNode;
	CExtBarButton::SetCustomizedCmdNode( pNode );
}
CExtCustomizeCmdTreeNode * CExtBarMdiDocButton::OnCustomizeNodeInit(
	CExtCustomizeSite * pSite,
	CExtCustomizeCmdTreeNode * pParentNode // toolbar node
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	ASSERT( pSite != NULL );
//	ASSERT_VALID( pParentNode );
//	pSite;
//	pParentNode;
//	return NULL; // no customization support
	return CExtBarButton::OnCustomizeNodeInit( pSite, pParentNode );
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

BOOL CExtBarMdiDocButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	ASSERT( pPopup != NULL );
	ASSERT( pPopup->GetSafeHwnd() == NULL );

	ASSERT_VALID( GetBar() );
	ASSERT( GetBar()->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );

HWND hWndActiveChild = _ActiveMdiChildWndGet();
	ASSERT( ::IsWindow( hWndActiveChild ) );

//CFrameWnd * pFrame =
//		DYNAMIC_DOWNCAST(
//			CFrameWnd,
//			CWnd::FromHandle(hWndActiveChild)
//			);
CMenu * pSysMenu =
		CWnd::FromHandle(hWndActiveChild) ->
			GetSystemMenu( FALSE );
	if( pSysMenu == NULL )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT( ::IsMenu(pSysMenu->GetSafeHmenu()) );
CExtCmdIcon & _icon = GetIcon();

CExtSafeString sActiveDocument;
	if( ! g_ResourceManager->LoadString( sActiveDocument, IDS_ACTIVE_DOCUMENT ) )
		sActiveDocument = _T("Active document");

	if( ! pPopup->ItemInsert(
			(UINT)CExtPopupMenuWnd::TYPE_POPUP,
			-1,
			sActiveDocument,
			_icon
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
CExtPopupMenuWnd * pChildPopup =
		pPopup->ItemGetPopup(
			pPopup->ItemGetCount() - 1
			);
	ASSERT( pChildPopup != NULL );
	if(	! pChildPopup->UpdateFromMenu(
			GetBar()->GetSafeHwnd(),
			pSysMenu,
			false,
			false
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	pChildPopup->
		SetCmdTargetToAllItems(
			hWndActiveChild
			);
	if( (pPopup->TrackFlagsGet()&TPMX_PALETTE) != 0 )
		pPopup->ItemGetInfo(pPopup->ItemGetCount()-1).SetChildCombine();

	VERIFY(
		pPopup->ItemInsert(
			CExtPopupMenuWnd::TYPE_SEPARATOR,
			-1
			)
		);

	return TRUE;
}

HWND CExtBarMdiDocButton::_ActiveMdiChildWndGet()
{
	ASSERT_VALID( this );
	if(		m_hWndActiveMdiChild != NULL
		&&	::IsWindow( m_hWndActiveMdiChild )
		)
		return m_hWndActiveMdiChild;
///	if( p_bMaximized != NULL )
///		(*p_bMaximized) = false;
CMDIFrameWnd * pMdiFrame = DYNAMIC_DOWNCAST( CMDIFrameWnd, GetBar()->_GetDockingFrameImpl() );
HWND hWndMdiClient = NULL;
	if( pMdiFrame == NULL )
		hWndMdiClient = ((CExtMenuControlBar*)GetBar())->_GetHwndMdiArea();
	else
	{
		ASSERT_VALID( pMdiFrame );
		hWndMdiClient = pMdiFrame->m_hWndMDIClient;
	} // else from if( pMdiFrame == NULL )
	if( hWndMdiClient == NULL )
		return NULL;
	ASSERT( ::IsWindow( hWndMdiClient ) );
BOOL bMax = FALSE;
HWND hWndActiveChildFrame = (HWND) ::SendMessage( hWndMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax );
	if( hWndActiveChildFrame == NULL || ( ! ::IsWindow( hWndActiveChildFrame ) ) )
	{
///		bMax = FALSE;
	}
	else
	{
		__EXT_MFC_LONG_PTR dwActiveChildFrameStyle = ::__EXT_MFC_GetWindowLong( hWndActiveChildFrame, GWL_STYLE );
		if( pMdiFrame == NULL )
		{
			if( (dwActiveChildFrameStyle&WS_VISIBLE) == 0 )
			{
				hWndActiveChildFrame = NULL;
				///bMax = FALSE;
			} // if( (dwActiveChildFrameStyle&WS_VISIBLE) == 0 )
		} // if( pMdiFrame == NULL )
		else
		{
			CWnd * pWndTempAnalyze = CWnd::FromHandlePermanent( hWndActiveChildFrame );
			if( pWndTempAnalyze != NULL )
			{
				ASSERT_VALID( pWndTempAnalyze );
				CMDIChildWnd * pWndTempMDIChild = DYNAMIC_DOWNCAST( CMDIChildWnd, pWndTempAnalyze );
				if(		pWndTempMDIChild != NULL
					&&	pWndTempMDIChild->m_bPseudoInactive
					&&	(dwActiveChildFrameStyle&WS_VISIBLE) == 0
					)
				{
					hWndActiveChildFrame = NULL;
					///bMax = FALSE;
				}
			} // if( pWndTempAnalyze != NULL )
		} // else from if( pMdiFrame == NULL )
	} // else from if( hWndActiveChildFrame == NULL )
///	if( p_bMaximized != NULL )
///		(*p_bMaximized) = bMax ? true : false;
	return hWndActiveChildFrame;
}

HWND CExtBarMdiDocButton::_ActiveMdiChildWndAttach(
	HWND hWndActiveMdiChildNew
	)
{
	ASSERT_VALID( this );
	if( hWndActiveMdiChildNew == m_hWndActiveMdiChild )
		return m_hWndActiveMdiChild;
HWND hWndActiveMdiChildOld = m_hWndActiveMdiChild;
	m_hWndActiveMdiChild = hWndActiveMdiChildNew;
	m_iconCache.Empty();
	return hWndActiveMdiChildOld;
}

HWND CExtBarMdiDocButton::_ActiveMdiChildWndDetach()
{
	ASSERT_VALID( this );
	return _ActiveMdiChildWndAttach( NULL );
}

CExtCmdIcon * CExtBarMdiDocButton::GetIconPtr()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	if(		m_hWndActiveMdiChild != NULL
		&&	::IsWindow( m_hWndActiveMdiChild )
		&&	( ! m_iconCache.IsEmpty() )
		)
		return (&m_iconCache);
HWND hWndActiveChild = _ActiveMdiChildWndGet();
	if( hWndActiveChild == NULL )
		return NULL;
	if( m_hWndActiveMdiChild != hWndActiveChild )
		m_hWndActiveMdiChild = NULL;
	ASSERT( ::IsWindow(hWndActiveChild) );
	_ExtractWndIcon( hWndActiveChild, m_iconCache );
	if( m_iconCache.IsEmpty() )
		return NULL;
	return (&m_iconCache);
}

void CExtBarMdiDocButton::_ExtractWndIcon(
	HWND hWnd,
	CExtCmdIcon & _Icon
	)
{
	_Icon.Empty();
	if(		hWnd == NULL
		||	( ! ::IsWindow( hWnd ) )
		)
		return;
HICON hIcon = (HICON)(__EXT_MFC_DWORD_PTR) ::SendMessage( hWnd, CExtControlBar::g_nMsgQueryIcon, 0, 0 );
	if( hIcon == NULL )
	{
		hIcon = (HICON)(__EXT_MFC_DWORD_PTR) ::SendMessage( hWnd, WM_GETICON, ICON_SMALL, 0 );
		if( hIcon == NULL )
		{
			hIcon = (HICON)(__EXT_MFC_DWORD_PTR) ::SendMessage( hWnd, WM_GETICON, ICON_BIG, 0 );
			if( hIcon == NULL )
			{
				hIcon = (HICON)(__EXT_MFC_DWORD_PTR) ::__EXT_MFC_GetClassLong( hWnd, __EXT_MFC_GCL_HICONSM );
				if( hIcon == NULL )
				{
					hIcon = (HICON)(__EXT_MFC_DWORD_PTR) ::__EXT_MFC_GetClassLong( hWnd, __EXT_MFC_GCL_HICON );
					if( hIcon == NULL )
						return;
				} // if( hIcon == NULL )
			} // if( hIcon == NULL )
		} // if( hIcon == NULL )
	} // if( hIcon == NULL )
	ASSERT( hIcon != NULL );
//	_Icon.AssignFromHICON( hIcon, true );
CExtCmdIcon _iconBig;
	_iconBig.AssignFromHICON( hIcon, true );
	VERIFY(
		_Icon.CreateScaledCopy(
			_iconBig,
			CSize( 16, 16 )
			)
		);
}

void CExtBarMdiDocButton::OnUpdateCmdUI(
	CWnd * pTarget,
	BOOL bDisableIfNoHndler,
	int nIndex
	)
{
	ASSERT_VALID( this );
	pTarget;
	bDisableIfNoHndler;
	nIndex;
}

bool CExtBarMdiDocButton::OnDblClick(
	CPoint point
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonDblClick( this, point ) )
		return true;
	if( ! IsEnabled() )
		return false;
	if( ! m_bHandleDblClickAsCloseCmd )
		return false;
CWnd * pWndMdiChildFrame = GetCmdTargetWnd();
	if( pWndMdiChildFrame->GetSafeHwnd() == NULL )
		return true;
bool bAllowClose = false;
CMenu * pSysMenu = pWndMdiChildFrame->GetSystemMenu( FALSE );
	if( pSysMenu != NULL )
	{
		INT nCount = pSysMenu->GetMenuItemCount();
		for( INT nItem = 0; nItem < nCount; nItem++ )
		{
			UINT nID = pSysMenu->GetMenuItemID( nItem );
			if( nID != SC_CLOSE )
				continue;
			MENUITEMINFO mii;
			::memset(&mii,0,sizeof(MENUITEMINFO));
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STATE;
			if( !pSysMenu->GetMenuItemInfo(
					nItem,
					&mii,
					TRUE
					)
				)
				break;
			if( ( mii.fState & (MFS_DISABLED|MFS_GRAYED) ) == 0 )
				bAllowClose = true;
			break;
		} // for( INT nItem = 0; nItem < nCount; nItem++ )
	} // if( pSysMenu != NULL )
	if( ! bAllowClose )
		return true;
	GetBar()->ClientToScreen( &point );
	pWndMdiChildFrame->PostMessage(
		WM_SYSCOMMAND,
		SC_CLOSE,
		MAKELONG( point.x, point.y )
		);
	return true;
}

CWnd * CExtBarMdiDocButton::GetCmdTargetWnd()
{
	ASSERT_VALID( GetBar() );
HWND hWndActiveChild = _ActiveMdiChildWndGet();
	if( hWndActiveChild == NULL )
		return NULL;
	ASSERT( ::IsWindow(hWndActiveChild) );
CWnd * pWnd = CWnd::FromHandlePermanent(hWndActiveChild);
	if( pWnd == NULL )
	{
		pWnd = CWnd::FromHandle(hWndActiveChild);
		ASSERT_VALID( pWnd );
	} // if( pWnd == NULL )
#ifdef _DEBUG
	else
	{
		ASSERT_VALID( pWnd );
		// FrameFeatures specifics
		//ASSERT( pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) );
	} // else from if( pWnd == NULL )
#endif // _DEBUG
	return pWnd;
}

UINT CExtBarMdiDocButton::OnTrackPopup(
	CPoint point,
	bool bSelectAny,
	bool bForceNoAnimation
	)
{
	ASSERT_VALID( GetBar() );

bool bDockSiteCustomizeMode =
		GetBar()->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return UINT(-1L);

CExtMenuControlBar * pBar =
		DYNAMIC_DOWNCAST(
			CExtMenuControlBar,
			GetBar()
			);
CWnd * pWnd = CExtBarMdiDocButton::GetCmdTargetWnd();
//CFrameWnd * pFrame = 
//		DYNAMIC_DOWNCAST(
//			CFrameWnd,
//			pWnd
//			);
//	ASSERT( pFrame != NULL );
	if(		pBar == NULL
		//||	pFrame == NULL
		)
	{
		CExtBarButton::OnTrackPopup( point, bSelectAny, bForceNoAnimation );
		return UINT(-1L);
	}

bool bPrevTBMT = CExtToolControlBar::g_bMenuTracking;
	if( CExtToolControlBar::g_bMenuTracking
		//&& CExtPopupMenuWnd::IsMenuTracking()
		&& pBar->_GetIndexOf(this) ==
			pBar->m_nBtnIdxMenuTracking
		)
		return UINT(-1L);

	CExtToolControlBar::_CloseTrackingMenus();

	if( pBar->IsFloating() )
	{
		pBar->ActivateTopParent();
		CFrameWnd * pFrame =
			pBar->GetDockingFrame();
		ASSERT_VALID( pFrame );
		pFrame->BringWindowToTop();
	}

__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( pWnd->GetSafeHwnd(), GWL_EXSTYLE );
bool bRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;

UINT nTrackFlags = TPMX_TOPALIGN;
CRect rcBtn = Rect();
//	point.x = rcBtn.left;
//	point.y = rcBtn.bottom;
	switch( GetBar()->GetSafeDockBarDlgCtrlID() )
	{
	case AFX_IDW_DOCKBAR_BOTTOM:
		nTrackFlags = TPMX_BOTTOMALIGN;
//		point.x = rcBtn.left;
//		point.y = rcBtn.bottom;
	break;
	case AFX_IDW_DOCKBAR_LEFT:
		nTrackFlags = bRTL ? TPMX_RIGHTALIGN : TPMX_LEFTALIGN;
//		point.x = rcBtn.right;
//		point.y = rcBtn.top;
	break;
	case AFX_IDW_DOCKBAR_RIGHT:
		nTrackFlags = bRTL ? TPMX_LEFTALIGN : TPMX_RIGHTALIGN;
//		point.x = rcBtn.left;
//		point.y = rcBtn.top;
	break;
	} // switch( GetBar()->GetSafeDockBarDlgCtrlID() )
	pBar->ClientToScreen( &rcBtn );
	//	pBar->ClientToScreen( &point );
	point = rcBtn.CenterPoint();

	CExtToolControlBar::g_bMenuTracking = bPrevTBMT;

	GetBar()->_SwitchMenuTrackingIndex(
		GetBar()->_GetIndexOf( this )
		);

	if( bForceNoAnimation )
		nTrackFlags |= TPMX_FORCE_NO_ANIMATION;

	g_pTrackingMenuTBB = this;
	pBar->m_bSysMenuTracking =
		pBar->
		_TrackFrameSystemMenu(
			pWnd,
			&point, // NULL,
			TRUE,
			&rcBtn,
			nTrackFlags,
			m_bHandleDblClickAsCloseCmd ? FALSE : TRUE
			) ? true : false;

	CExtToolControlBar::g_bMenuTracking = true;

	return UINT(-1L);
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarMdiRightButton

IMPLEMENT_DYNCREATE(CExtBarMdiRightButton,CExtBarMdiDocButton)

CExtBarMdiRightButton::CExtBarMdiRightButton(
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	) :
	CExtBarMdiDocButton(
		pBar,
		nCmdID,
		nStyle
		)
{
}

CExtBarMdiRightButton::~CExtBarMdiRightButton()
{
}

BOOL CExtBarMdiRightButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	ASSERT( pPopup != NULL );
	ASSERT( pPopup->GetSafeHwnd() == NULL );

	ASSERT_VALID( GetBar() );
	ASSERT( GetBar()->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );

CExtMenuControlBar * pBar =
		DYNAMIC_DOWNCAST(
			CExtMenuControlBar,
			GetBar()
			);
	ASSERT_VALID( pBar );
BOOL bHorz = !pBar->IsDockedVertically();
MdiMenuBarRightButtonsInfo_t _info;
	pBar->_GetMdiMenuBarRightButtonsInfo( _info, this, bHorz );
	if( !_info.m_bInitialized )
		return FALSE;

HWND hWndActiveChild = _ActiveMdiChildWndGet();
	ASSERT( ::IsWindow( hWndActiveChild ) );

int nCount = pPopup->ItemGetCount();
	if( nCount > 0 )
	{
		if( pPopup->ItemGetCmdID(nCount-1) !=
				CExtPopupMenuWnd::TYPE_SEPARATOR
				)
		{
			VERIFY(
				pPopup->ItemInsert(
					CExtPopupMenuWnd::TYPE_SEPARATOR,
					-1
					)
				);
		}
	}

	if( _info.m_bBtnHelp )
	{
		VERIFY(
			pPopup->ItemInsert(
				SC_CONTEXTHELP,
				-1,NULL,NULL,hWndActiveChild
				)
			);
	}
	if( _info.m_bBtnMinimize )
	{
		VERIFY(
			pPopup->ItemInsert(
				SC_MINIMIZE,
				-1,NULL,NULL,hWndActiveChild
				)
			);
		VERIFY(
			pPopup->ItemInsert(
				SC_RESTORE,
				-1,NULL,NULL,hWndActiveChild
				)
			);
	}
//	if( _info.m_bBtnMaximize && !_info.m_bBtnMinimize )
//	{
//		VERIFY(
//			pPopup->ItemInsert(
//				SC_MAXIMIZE,
//				-1,NULL,NULL,hWndActiveChild
//				)
//			);
//	}
	if( _info.m_bBtnMaximize )
	{
		VERIFY(
			pPopup->ItemInsert(
				_info.m_bBtnMinimize ? SC_MAXIMIZE : SC_RESTORE,
				-1,NULL,NULL,hWndActiveChild
				)
			);
	}
	if( _info.m_bBtnClose )
	{
		VERIFY(
			pPopup->ItemInsert(
				SC_CLOSE,
				-1,NULL,NULL,hWndActiveChild
				)
			);
	}

	return TRUE;
}

CExtCmdIcon * CExtBarMdiRightButton::GetIconPtr()
{
	return NULL;
}

void CExtMenuControlBar::_GetMdiMenuBarRightButtonsInfo(
	CExtBarMdiRightButton::MdiMenuBarRightButtonsInfo_t & _info,
	CExtBarMdiRightButton * pTBB,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	_info.m_pWndMdiChildFrame = pTBB->GetCmdTargetWnd();
	if( _info.m_pWndMdiChildFrame == NULL )
		return;
	//ASSERT( _info.m_pWndMdiChildFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) );
	ASSERT_VALID( _info.m_pWndMdiChildFrame );
	_info.m_pBar = this;
DWORD dwFrameStyle = _info.m_pWndMdiChildFrame->GetStyle();
	_info.m_bBtnClose = false;
CMenu * pSysMenu = _info.m_pWndMdiChildFrame->GetSystemMenu( FALSE );
	if( pSysMenu != NULL )
	{
		INT nCount = pSysMenu->GetMenuItemCount();
		for( INT nItem = 0; nItem < nCount; nItem++ )
		{
			UINT nID = pSysMenu->GetMenuItemID( nItem );
			if( nID != SC_CLOSE )
				continue;
			MENUITEMINFO mii;
			::memset(&mii,0,sizeof(MENUITEMINFO));
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STATE;
			if( !pSysMenu->GetMenuItemInfo(
					nItem,
					&mii,
					TRUE
					)
				)
				break;
			if( ( mii.fState & (MFS_DISABLED|MFS_GRAYED) ) == 0 )
				_info.m_bBtnClose = true;
			break;
		} // for( INT nItem = 0; nItem < nCount; nItem++ )
	} // if( pSysMenu != NULL )
	_info.m_bBtnMaximize =
		(dwFrameStyle & WS_MAXIMIZEBOX) ?
			true : false;
	_info.m_bBtnMinimize =
		(dwFrameStyle & WS_MINIMIZEBOX) ?
			true : false;
	_info.m_bBtnHelp =
		(_info.m_bBtnMaximize && (_info.m_pWndMdiChildFrame->GetExStyle()&WS_EX_CONTEXTHELP)) ?
			true : false;
	if( bHorz )
	{
		_info.m_calcSize.cx = __GAP_BETWEEN_WND_RIGHT_BUTTONS;
		_info.m_calcSize.cy = _info.m_cyIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
		if( _info.m_bBtnMinimize )
			_info.m_calcSize.cx +=
				(_info.m_cxIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS)*2;
		if( _info.m_bBtnMaximize && !_info.m_bBtnMinimize )
			_info.m_calcSize.cx +=
				_info.m_cxIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
		if( _info.m_bBtnHelp )
			_info.m_calcSize.cx +=
				_info.m_cxIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
		if( _info.m_bBtnClose )
			_info.m_calcSize.cx +=
				_info.m_cxIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
	} // if( bHorz )
	else
	{
		_info.m_calcSize.cx = _info.m_cxIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
		_info.m_calcSize.cy = __GAP_BETWEEN_WND_RIGHT_BUTTONS;
		if( _info.m_bBtnMinimize )
			_info.m_calcSize.cy +=
				(_info.m_cyIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS)*2;
		if( _info.m_bBtnMaximize && !_info.m_bBtnMinimize )
			_info.m_calcSize.cy +=
				_info.m_cyIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
		if( _info.m_bBtnHelp )
			_info.m_calcSize.cy +=
				_info.m_cyIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
		if( _info.m_bBtnClose )
			_info.m_calcSize.cy +=
				_info.m_cyIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
	} // else from if( bHorz )

	_info.m_bInitialized = true;
}

CExtBarMdiRightButton::MdiMenuBarRightButtonsInfo_t::
	MdiMenuBarRightButtonsInfo_t()
{
	m_bInitialized
		= m_bBtnClose
		= m_bBtnMaximize
		= m_bBtnMinimize
		= m_bBtnHelp
		= false;
	m_pBar = NULL;
	m_pWndMdiChildFrame = NULL;
	if( g_PaintManager.m_bIsWinXPorLater )
	{
		CSize _sizeTmp( // (+ v.2.22)
				::GetSystemMetrics(SM_CXSMSIZE),
				::GetSystemMetrics(SM_CYSMSIZE)
				);
		int nMetric =
			max( _sizeTmp.cx, _sizeTmp.cy ) + 1;
		m_cxIcon = max( nMetric, 18 ); // 16->18 (+ v.2.23)
		m_cyIcon = max( nMetric, 18 ); // 16->18 (+ v.2.23)
	} // if( g_PaintManager.m_bIsWinXPorLater )
	else
	{
		m_cxIcon = ::GetSystemMetrics(SM_CXSIZE);
		m_cyIcon = ::GetSystemMetrics(SM_CYSIZE);
	} // else from if( g_PaintManager.m_bIsWinXPorLater )
	ASSERT(
		m_cxIcon > __GAP_BETWEEN_WND_RIGHT_BUTTONS
		&&
		m_cyIcon > __GAP_BETWEEN_WND_RIGHT_BUTTONS
		);
	m_cxIcon -= __GAP_BETWEEN_WND_RIGHT_BUTTONS;
	m_cyIcon -= __GAP_BETWEEN_WND_RIGHT_BUTTONS;
	m_calcSize.cx = m_calcSize.cy = 0;
	EmptyLocations();
}

void CExtBarMdiRightButton::MdiMenuBarRightButtonsInfo_t::
	EmptyLocations()
{
	m_rcBtnClose.SetRectEmpty();
	m_rcBtnMaximize.SetRectEmpty();
	m_rcBtnRestore.SetRectEmpty();
	m_rcBtnMinimize.SetRectEmpty();
	m_rcBtnHelp.SetRectEmpty();
}

void CExtBarMdiRightButton::MdiMenuBarRightButtonsInfo_t::
	InitLocations(
		const CRect & rcBtn,
		BOOL bHorz
		)
{
	ASSERT( m_bInitialized );
	EmptyLocations();
	if( rcBtn.IsRectEmpty() )
		return;
CRect rcStart(0,0,0,0);
CSize sizeOffset(0,0);
	if( bHorz )
	{
		int nOffset = (rcBtn.Height() - m_cyIcon) / 2;
		rcStart.SetRect(
			rcBtn.left + __GAP_BETWEEN_WND_RIGHT_BUTTONS,
			rcBtn.top + nOffset,
			rcBtn.left + __GAP_BETWEEN_WND_RIGHT_BUTTONS
				+ m_cxIcon,
			rcBtn.top + nOffset + m_cyIcon
			);
		sizeOffset.cx = m_cxIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
	} // if( bHorz )
	else
	{
		int nOffset = (rcBtn.Width() - m_cxIcon) / 2;
		rcStart.SetRect(
			rcBtn.left + nOffset,
			rcBtn.top + __GAP_BETWEEN_WND_RIGHT_BUTTONS,
			rcBtn.left + nOffset + m_cxIcon,
			rcBtn.top + __GAP_BETWEEN_WND_RIGHT_BUTTONS
				+ m_cxIcon
			);
		sizeOffset.cy = m_cyIcon+__GAP_BETWEEN_WND_RIGHT_BUTTONS;
	} // else from if( bHorz )
	if( m_bBtnHelp )
	{
		m_rcBtnHelp = rcStart;
		rcStart.OffsetRect(sizeOffset);
	}
	if( m_bBtnMinimize )
	{
		m_rcBtnMinimize = rcStart;
		rcStart.OffsetRect(sizeOffset);
		m_rcBtnRestore = rcStart;
		rcStart.OffsetRect(sizeOffset);
	}
	if( m_bBtnMaximize && (! m_bBtnMinimize ) )
	{
		m_rcBtnMaximize = rcStart;
		rcStart.OffsetRect(sizeOffset);
	}
	if( m_bBtnClose )
		m_rcBtnClose = rcStart;
}

CSize CExtBarMdiRightButton::CalculateLayout(
	CDC & dc,
	CSize sizePreCalc,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	dc;
	ASSERT_VALID( GetBar() );
	ASSERT( GetBar()->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
	ASSERT_VALID( (&dc) );
	ASSERT( CtrlGet() == NULL );
//	ASSERT( ! IsSeparator() );

	if(		( ! IsVisible() )
		||	( GetStyle() & TBBS_HIDDEN ) != 0
		)
	{
		m_ActiveSize.cx = m_ActiveSize.cy = 0;
		return m_ActiveSize;
	}

CExtMenuControlBar * pBar =
		STATIC_DOWNCAST(
			CExtMenuControlBar,
			GetBar()
			);
	ASSERT_VALID( pBar );
MdiMenuBarRightButtonsInfo_t _info;
	pBar->_GetMdiMenuBarRightButtonsInfo( _info, this, bHorz );

	//ASSERT( _info.m_bInitialized );
	if( !_info.m_bInitialized )
		return CSize( 1, 1 );

CSize _size( _info.m_calcSize );
	if( bHorz )
	{
		if( _size.cy < sizePreCalc.cy )
			_size.cy = sizePreCalc.cy;
	}
	else
	{
		if( _size.cx < sizePreCalc.cx )
			_size.cx = sizePreCalc.cx;
	}
	
	m_ActiveSize = _size;

	return _size;
}

void CExtBarMdiRightButton::PaintCompound(
	CDC & dc,
	bool bPaintParentChain,
	bool bPaintChildren,
	bool bPaintOneNearestChildrenLevelOnly
	)
{
	ASSERT_VALID( GetBar() );
	ASSERT( GetBar()->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
	ASSERT_VALID( (&dc) );
	ASSERT( CtrlGet() == NULL );
//	ASSERT( ! IsSeparator() );

	if( ! IsPaintAble( dc ) )
		return;
	if( AnimationClient_StatePaint( dc ) )
		return;
	if( bPaintParentChain )
		PaintParentChain( dc );

CExtMenuControlBar * pBar =
		DYNAMIC_DOWNCAST(
			CExtMenuControlBar,
			GetBar()
			);
	ASSERT_VALID( pBar );
MdiMenuBarRightButtonsInfo_t _info;
bool bHorz = IsHorzBarOrientation();
	pBar->_GetMdiMenuBarRightButtonsInfo( _info, this, bHorz );
	if( ! _info.m_bInitialized )
		return;
CRect rcTBB = Rect();
	_info.InitLocations( rcTBB, bHorz );

CPoint ptCursor( 0, 0 );
	::GetCursorPos(&ptCursor);
	pBar->ScreenToClient( &ptCursor );
bool bFlat = true;
bool bDrawBorder = true;
int eAlign =
		CExtPaintManager::__ALIGN_HORIZ_CENTER
			| CExtPaintManager::__ALIGN_VERT_CENTER;
bool bEnabled = IsEnabled() ? true : false;
bool bChecked =
	( (GetStyle()&TBBS_CHECKED) != 0 ) 
		? true 
		: false;
	if( _info.m_bBtnHelp )
	{
		bool bHover =
			(IsHover() && _info.m_rcBtnHelp.PtInRect(ptCursor))
				? true : false;
		bool bPushed =
			(IsPressed() && _info.m_rcBtnHelp.PtInRect(ptCursor))
				? true : false;
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbdCtxHelp(
			this,
			bHorz ? true : false,
			_info.m_rcBtnHelp,_T(""),NULL,bFlat,
			bHover, bPushed, false, bEnabled,
			bDrawBorder,false,false,eAlign,
			NULL, false, SC_CONTEXTHELP,
			(!bEnabled) || ( bEnabled && (!bHover) && (!bPushed) )
			);
		_ppbdCtxHelp.m_bChecked = bChecked;
		_ppbdCtxHelp.m_nIconAlignment = CExtPaintManager::__ALIGN_HORIZ_CENTER | CExtPaintManager::__ALIGN_VERT_CENTER;
		pBar->PmBridge_GetPM()->PaintPushButtonMdiRight( dc, _ppbdCtxHelp );
	}
	if( _info.m_bBtnMinimize )
	{
		bool bHover =
			(IsHover() && _info.m_rcBtnRestore.PtInRect(ptCursor))
				? true : false;
		bool bPushed =
			(IsPressed() && _info.m_rcBtnRestore.PtInRect(ptCursor))
				? true : false;
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbdRestore(
			this,
			bHorz ? true : false,
			_info.m_rcBtnRestore,_T(""),NULL,bFlat,
			bHover, bPushed, false, bEnabled,
			bDrawBorder,false,false,eAlign,
			NULL, false, SC_RESTORE,
			(!bEnabled) || ( bEnabled && (!bHover) && (!bPushed) )
			);
		_ppbdRestore.m_bChecked = bChecked;
		pBar->PmBridge_GetPM()->PaintPushButtonMdiRight( dc, _ppbdRestore );
		bHover =
			(IsHover() && _info.m_rcBtnMinimize.PtInRect(ptCursor))
				? true : false;
		bPushed =
			(IsPressed() && _info.m_rcBtnMinimize.PtInRect(ptCursor))
				? true : false;
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbdMinimize(
			this,
			bHorz ? true : false,
			_info.m_rcBtnMinimize,_T(""),NULL,bFlat,
			bHover, bPushed, false, bEnabled,
			bDrawBorder,false,false,eAlign,
			NULL, false, SC_MINIMIZE,
			(!bEnabled) || ( bEnabled && (!bHover) && (!bPushed) )
			);
		_ppbdMinimize.m_bChecked = bChecked;
		pBar->PmBridge_GetPM()->PaintPushButtonMdiRight( dc, _ppbdMinimize );
	}
	if( _info.m_bBtnMaximize && !_info.m_bBtnMinimize )
	{
		bool bHover =
			(IsHover() && _info.m_rcBtnMaximize.PtInRect(ptCursor))
				? true : false;
		bool bPushed =
			(IsPressed() && _info.m_rcBtnMaximize.PtInRect(ptCursor))
				? true : false;
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbdMaximize(
			this,
			bHorz ? true : false,
			_info.m_rcBtnMaximize,_T(""),NULL,bFlat,
			bHover, bPushed, false, bEnabled,
			bDrawBorder,false,false,eAlign,
			NULL, false, SC_MAXIMIZE,
			(!bEnabled) || ( bEnabled && (!bHover) && (!bPushed) )
			);
		_ppbdMaximize.m_bChecked = bChecked;
		pBar->PmBridge_GetPM()->PaintPushButtonMdiRight( dc, _ppbdMaximize );
	}
	if( _info.m_bBtnClose )
	{
		bool bHover =
			(IsHover() && _info.m_rcBtnClose.PtInRect(ptCursor))
				? true : false;
		bool bPushed =
			(IsPressed() && _info.m_rcBtnClose.PtInRect(ptCursor))
				? true : false;
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbdClose(
			this,
			bHorz ? true : false,
			_info.m_rcBtnClose,_T(""),NULL,bFlat,
			bHover, bPushed, false, bEnabled,
			bDrawBorder,false,false,eAlign,
			NULL, false, SC_CLOSE,
			(!bEnabled) || ( bEnabled && (!bHover) && (!bPushed) )
			);
		_ppbdClose.m_bChecked = bChecked;
		pBar->PmBridge_GetPM()->PaintPushButtonMdiRight( dc, _ppbdClose );
	}

	if( bPaintChildren )
		PaintChildren( dc, bPaintOneNearestChildrenLevelOnly );
}

UINT CExtBarMdiRightButton::OnTrackPopup(
	CPoint point,
	bool bSelectAny,
	bool bForceNoAnimation
	)
{
	point;
	bSelectAny;
	bForceNoAnimation;
	return UINT( -1L );
}

void CExtBarMdiRightButton::OnClick(
	CPoint point,
	bool bDown
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonClick( this, point, bDown ) )
		return;
bool bDockSiteCustomizeMode = pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;
	if( bDown )
		return;
//	CExtMouseCaptureSink::ReleaseCapture();
	ASSERT( pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
CExtMenuControlBar * pMenuBar = DYNAMIC_DOWNCAST( CExtMenuControlBar, pBar );
	ASSERT_VALID( pMenuBar );
BOOL bHorz = ! pMenuBar->IsDockedVertically();
MdiMenuBarRightButtonsInfo_t _info;
	pMenuBar->_GetMdiMenuBarRightButtonsInfo( _info, this, bHorz );
	if( !_info.m_bInitialized )
		return;
	_info.InitLocations( Rect(), bHorz );

bool bInstallHideFlag = false;
UINT nHelperSysCmdID = 0;
	if( _info.m_bBtnHelp
		&& _info.m_rcBtnHelp.PtInRect(point)
		)
		nHelperSysCmdID = SC_CONTEXTHELP;
	else if( _info.m_bBtnMinimize
		&& _info.m_rcBtnRestore.PtInRect(point)
		)
	{
		nHelperSysCmdID = SC_RESTORE;
		bInstallHideFlag = true;
	}
	else if( _info.m_bBtnMinimize
		&& _info.m_rcBtnMinimize.PtInRect(point)
		)
	{
		nHelperSysCmdID = SC_MINIMIZE;
		bInstallHideFlag = true;
	}
	else if( _info.m_bBtnMaximize && !_info.m_bBtnMinimize
		&& _info.m_rcBtnMaximize.PtInRect(point)
		)
		nHelperSysCmdID = SC_MAXIMIZE;
	else if( _info.m_bBtnClose
		&& _info.m_rcBtnClose.PtInRect(point)
		)
		nHelperSysCmdID = SC_CLOSE;

	if( nHelperSysCmdID == 0 )
		return;

HWND hWndActiveChild = _ActiveMdiChildWndGet();
	if( hWndActiveChild == NULL )
		return;
	ASSERT( ::IsWindow( hWndActiveChild ) );

	if( bInstallHideFlag )
	{
		Show( FALSE );
		ASSERT( GetBar()->GetButtonsCount() > 0 );
		CExtBarButton * pTBB = GetBar()->GetButton( 0 );
		ASSERT_VALID( pTBB );
		if(		pTBB->IsVisible()
			&&	pTBB->IsKindOf( RUNTIME_CLASS(CExtBarMdiDocButton) )
			)
		{
			pTBB->Show( FALSE );
		}
//		GetBar()->Invalidate();
//		GetBar()->UpdateWindow();
//		GetBar()->GetParentFrame()->DelayRecalcLayout();
	}

	if( nHelperSysCmdID == SC_MAXIMIZE )
	{
		HWND hWndMdiClientArea =
			::GetParent( hWndActiveChild );
		ASSERT( hWndMdiClientArea != NULL );
		ASSERT( ::IsWindow( hWndMdiClientArea ) );
		::PostMessage(
			hWndMdiClientArea,
			WM_MDIRESTORE,
			(WPARAM)hWndActiveChild,
			0
			);
	} // if( nHelperSysCmdID == SC_MAXIMIZE )
	else
	{
		pMenuBar->ClientToScreen( &point );
		pMenuBar->_DelayUpdateMenuBar();
		::SendMessage(
			hWndActiveChild,
			WM_SYSCOMMAND,
			nHelperSysCmdID,
			MAKELONG( point.x, point.y )
			);
	} // else
}

bool CExtBarMdiRightButton::OnDblClick(
	CPoint point
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonDblClick( this, point ) )
		return true;
	return false;
}

__EXT_MFC_INT_PTR CExtBarMdiRightButton::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( GetBar()->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );

CExtMenuControlBar * pBar =
		DYNAMIC_DOWNCAST(
			CExtMenuControlBar,
			GetBar()
			);
	ASSERT_VALID( pBar );
BOOL bHorz = !pBar->IsDockedVertically();
MdiMenuBarRightButtonsInfo_t _info;
	pBar->_GetMdiMenuBarRightButtonsInfo( _info, this, bHorz );
	if( !_info.m_bInitialized )
		return -1;
	_info.InitLocations( Rect(), bHorz );

	if(		_info.m_bBtnHelp
		&&	_info.m_rcBtnHelp.PtInRect(point)
		)
	{
		if( pTI != NULL )
		{
			::CopyRect(
				&(pTI->rect),
				&_info.m_rcBtnHelp
				);
			CExtSafeString str;
			if( g_ResourceManager->LoadString( str, ID_CONTEXT_HELP ) )
				pTI->lpszText = (LPTSTR)
					::calloc( (str.GetLength() + 1), sizeof(TCHAR) );
			if( pTI->lpszText != NULL )
				__EXT_MFC_STRCPY( pTI->lpszText, str.GetLength() + 1, str );
			else
				pTI->lpszText = LPSTR_TEXTCALLBACK;
		}
		return ((__EXT_MFC_INT_PTR)ID_CONTEXT_HELP);
	}
	if(		_info.m_bBtnMinimize
		&&	_info.m_rcBtnRestore.PtInRect(point)
		)
	{
		if( pTI != NULL )
		{
			::CopyRect(
				&(pTI->rect),
				&_info.m_rcBtnRestore
				);
			CExtSafeString str;
			if( g_ResourceManager->LoadString( str, AFX_IDS_SCRESTORE ) )
				pTI->lpszText = (LPTSTR)
					::calloc( (str.GetLength() + 1), sizeof(TCHAR) );
			if( pTI->lpszText != NULL )
				__EXT_MFC_STRCPY( pTI->lpszText, str.GetLength() + 1, str );
			else
				pTI->lpszText = LPSTR_TEXTCALLBACK;
		}
		return ((int)AFX_IDS_SCRESTORE);
	}
	if(		_info.m_bBtnMinimize
		&&	_info.m_rcBtnMinimize.PtInRect(point)
		)
	{
		if( pTI != NULL )
		{
			::CopyRect(
				&(pTI->rect),
				&_info.m_rcBtnMinimize
				);
			CExtSafeString str;
			if( g_ResourceManager->LoadString( str, AFX_IDS_SCMINIMIZE ) )
				pTI->lpszText = (LPTSTR)
					::calloc( (str.GetLength() + 1), sizeof(TCHAR) );
			if( pTI->lpszText != NULL )
				__EXT_MFC_STRCPY( pTI->lpszText, str.GetLength() + 1, str );
			else
				pTI->lpszText = LPSTR_TEXTCALLBACK;
		}
		return ((__EXT_MFC_INT_PTR)AFX_IDS_SCMINIMIZE);
	}
	if(		_info.m_bBtnMaximize && !_info.m_bBtnMinimize
		&&	_info.m_rcBtnMaximize.PtInRect(point)
		)
	{
		if( pTI != NULL )
		{
			::CopyRect(
				&(pTI->rect),
				&_info.m_rcBtnMaximize
				);
			CExtSafeString str;
			if( g_ResourceManager->LoadString( str, AFX_IDS_SCMAXIMIZE ) )
				pTI->lpszText = (LPTSTR)
					::calloc( (str.GetLength() + 1), sizeof(TCHAR) );
			if( pTI->lpszText != NULL )
				__EXT_MFC_STRCPY( pTI->lpszText, str.GetLength() + 1, str );
			else
				pTI->lpszText = LPSTR_TEXTCALLBACK;
		}
		return ((__EXT_MFC_INT_PTR)AFX_IDS_SCMAXIMIZE);
	}
	if(		_info.m_bBtnClose
		&&	_info.m_rcBtnClose.PtInRect(point)
		)
	{
		if( pTI != NULL )
		{
			::CopyRect(
				&(pTI->rect),
				&_info.m_rcBtnClose
				);
			CExtSafeString str;
			if( g_ResourceManager->LoadString( str, AFX_IDS_SCCLOSE ) )
				pTI->lpszText = (LPTSTR)
					::calloc( (str.GetLength() + 1), sizeof(TCHAR) );
			if( pTI->lpszText != NULL )
				__EXT_MFC_STRCPY( pTI->lpszText, str.GetLength() + 1, str );
			else
				pTI->lpszText = LPSTR_TEXTCALLBACK;
		}
		return ((__EXT_MFC_INT_PTR)AFX_IDS_SCCLOSE);
	}
	return -1;
}

bool CExtBarMdiRightButton::CanStartCommandDrag()
{
	ASSERT_VALID( this );
	return false;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * CExtBarMdiRightButton::GetCmdNode(
	bool bInitial // = false
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	bInitial;
//	ASSERT( m_pCmdNodeI == NULL );
//	ASSERT( m_pCmdNodeC == NULL );
//	return NULL;
	return CExtBarMdiDocButton::GetCmdNode( bInitial );
}
void CExtBarMdiRightButton::SetBasicCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	ASSERT( m_pCmdNodeI == NULL );
//	ASSERT( m_pCmdNodeC == NULL );
//	ASSERT( FALSE ); // should be never called
//	pNode;
	CExtBarMdiDocButton::SetBasicCmdNode( pNode );
}
void CExtBarMdiRightButton::SetCustomizedCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	ASSERT( m_pCmdNodeI == NULL );
//	ASSERT( m_pCmdNodeC == NULL );
//	ASSERT( FALSE ); // should be never called
//	pNode;
	CExtBarMdiDocButton::SetCustomizedCmdNode( pNode );
}

CExtCustomizeCmdTreeNode * CExtBarMdiRightButton::OnCustomizeNodeInit(
	CExtCustomizeSite * pSite,
	CExtCustomizeCmdTreeNode * pParentNode // toolbar node
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	ASSERT( pSite != NULL );
//	ASSERT_VALID( pParentNode );
//	pSite;
//	pParentNode;
//	return NULL; // no customization support
	return CExtBarMdiDocButton::OnCustomizeNodeInit( pSite, pParentNode );
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtBarMdiRightButton::OnQueryFlatTrackingEnabled() const
{
	ASSERT_VALID( this );
	return false;
}

LRESULT CExtBarMdiRightButton::OnHelpHitTest(
	CPoint point
	)
{
	ASSERT_VALID( this );
	return (LRESULT)OnToolHitTest(point,NULL);
}

void CExtMenuControlBar::_KillFrameMenu()
{
	if( m_bPresubclassDialogMode )
		return;
	if( IsOleIpObjActive() )
		return;
CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
HWND hWndFrame = pFrame->GetSafeHwnd();
	ASSERT( hWndFrame != NULL );
	ASSERT( ::IsWindow(hWndFrame) );
HMENU hFrameRealMenu = ::GetMenu( hWndFrame );
	if( hFrameRealMenu == NULL )
		return;
	::SetMenu( hWndFrame, NULL );
	_DelayUpdateMenuBar();
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
void CExtMenuControlBar::OnCustomizeModeEnter()
{
	ASSERT_VALID( this );
	CExtToolControlBar::OnCustomizeModeEnter();
}
void CExtMenuControlBar::OnCustomizeModeLeave()
{
	ASSERT_VALID( this );
	CExtToolControlBar::OnCustomizeModeLeave();
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtMenuControlBar::PreSubclassWindow() 
{
	CExtToolControlBar::PreSubclassWindow();
	m_bHelperTrackingCharCode = false;
	if( m_bPresubclassDialogMode && m_hWndHelper == NULL )
	{
		m_hWndHelper = ::GetParent( m_hWnd );
		if( m_hWndHelper != NULL )
		{
			ASSERT( ::IsWindow( m_hWndHelper ) );
			if( !IsHookedWindow( m_hWndHelper ) )
				SetupHookWndSink( m_hWndHelper );
		}
	} // if( m_bPresubclassDialogMode && m_hWndHelper == NULL )
}

BOOL CExtMenuControlBar::PreTranslateMessage(MSG* pMsg) 
{
	if( m_bPresubclassDialogMode )
	{
		if( TranslateMainFrameMessage(pMsg) )
			return TRUE;
	}
	
	return CExtToolControlBar::PreTranslateMessage(pMsg);
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtCustomizeSite::CCmdMenuInfo * CExtMenuControlBar::MenuInfoGet()
{
	ASSERT_VALID( this );
	return m_pMenuInfo;
}

CExtCustomizeSite::CCmdMenuInfo * CExtMenuControlBar::MenuInfoDetach()
{
	ASSERT_VALID( this );
CExtCustomizeSite::CCmdMenuInfo * pMenuInfo = m_pMenuInfo;
	m_pMenuInfo = NULL;
	return pMenuInfo;
}

CExtCustomizeSite::CCmdMenuInfo * CExtMenuControlBar::MenuInfoAttach(
	CExtCustomizeSite::CCmdMenuInfo * pMenuInfo
	)
{
	ASSERT_VALID( this );
CExtCustomizeSite::CCmdMenuInfo * pMenuInfoOld = m_pMenuInfo;
	m_pMenuInfo = pMenuInfo;
	return pMenuInfoOld;
}

void CExtMenuControlBar::MenuInfoUpdate()
{
	ASSERT_VALID( this );
CExtCustomizeSite * pSite =
		GetCustomizeSite();
	if( pSite == NULL )
		return;
CExtCustomizeSite::CCmdMenuInfo * pMenuInfo =
		pSite->MenuInfoFindForMenuBar();
	if( ((LPVOID)pMenuInfo) != ((LPVOID)MenuInfoGet()) )
		MenuInfoAttach( pMenuInfo );
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtMenuControlBar::SetMdiWindowPopupName(
	__EXT_MFC_SAFE_LPCTSTR sMdiWindowPopupName // = NULL
	)
{
	ASSERT_VALID( this );

	m_sMdiWindowPopupName =
		( sMdiWindowPopupName != NULL )
			? sMdiWindowPopupName
			: _T("")
			;
	m_sMdiWindowPopupName.TrimLeft();
	m_sMdiWindowPopupName.TrimRight();
	while( m_sMdiWindowPopupName.Replace( _T("&"), _T("") ) > 0 )
	{
		m_sMdiWindowPopupName.TrimLeft();
		m_sMdiWindowPopupName.TrimRight();
	}
}

