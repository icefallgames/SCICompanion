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

#if (!defined __EXT_NC_FRAME_H)
#define __EXT_NC_FRAME_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_CONTROLBAR_H)
	#include <ExtControlBar.h>
#endif

#if (!defined __EXT_HOOK_H)
	#include "../Src/ExtHook.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif

#if (!defined __EXT_MFC_NO_NC_FRAME )

class CExtPmBridgeNC;
class CExtNcFrameImpl;
class CExtPaintManager;

class __PROF_UIS_API CExtPmBridgeNC
	: public CExtPmBridge
{
	CExtNcFrameImpl * m_pNcFrameImpl;
public:
	DECLARE_CExtPmBridge_MEMBERS( CExtPmBridgeNC );
	CExtPmBridgeNC(
		CExtNcFrameImpl * pNcFrameImpl = NULL
		);
	virtual ~CExtPmBridgeNC();
	const CExtNcFrameImpl * NcFrameImpl_Get() const;
	CExtNcFrameImpl * NcFrameImpl_Get();
	void NcFrameImpl_Set(
		CExtNcFrameImpl * pNcFrameImpl
		);
	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);
	virtual void PmBridge_OnDisplayChange(
		CExtPaintManager * pGlobalPM,
		CWnd * pWndNotifySrc,
		INT nDepthBPP,
		CPoint ptSizes
		);
	virtual void PmBridge_OnThemeChanged(
		CExtPaintManager * pGlobalPM,
		CWnd * pWndNotifySrc,
		WPARAM wParam,
		LPARAM lParam
		);
	virtual void _AdjustThemeSettings();
}; // class CExtPmBridgeNC

class __PROF_UIS_API CExtNcFrameImpl
{
protected:
	mutable CMapPtrToPtr m_mapNcFrameImpl_HtRects;
	mutable bool m_bNcFrameImpl_HelperInsideNcHitTest:1;
	MINMAXINFO m_NcFrameImpl_MinMaxInfo_Cache;
public:
	CExtPmBridgeNC m_BridgeNC;
	bool m_bNcFrameImpl_PivotPmSyncMode:1, m_bNcFrameImpl_HelperLastThemeSupportedNC:1;
protected:
	bool m_bNcFrameImpl_IsActive:1, m_bNcFrameImpl_RgnSet:1,
		m_bNcFrameImpl_RestoreBorder:1, m_bNcFrameImpl_DelatayedFrameRecalc:1,
		m_bNcFrameImpl_QuickWindowPlacement:1,
		m_bNcFrameImpl_AssigningRGN:1,
		m_bNcFrameImpl_InAdjustAdjustThemeSettings:1,
		m_bNcFrameImpl_InAdjustAdjustThemeSettings_NoRgnSetup:1;
	__EXT_MFC_LONG_PTR m_dwNcFrameImpl_StyleInitial, m_dwNcFrameImpl_StyleExInitial;
	mutable INT m_nNcFrameImpl_Lock;
	UINT m_nNcFrameImpl_LastShowCmd;
	LPARAM m_nNcFrameImpl_LastCheckCursorHT;
	CPoint m_ptNcFrameImpl_LastCheckCursor;
	mutable CExtCmdIcon m_iconNcFrameImpl_QueryCache;
	void NcFrameImpl_MapHtRects_Clean();
	void NcFrameImpl_MapHtRects_SetAt( LONG nHT, const RECT & rc ) const;
	bool NcFrameImpl_MapHtRects_GetAt( LONG nHT, RECT & rc ) const;
	bool m_bNcFrameImpl_NcInitPassed:1, m_bNcFrameImpl_CreatePassed:1, m_bNcFrameImpl_DestroyPassed:1;
public:
	bool m_bNcFrameImpl_RestoreEnabledState:1, m_bNcFrameImpl_DwmSurfaceInitializationComplete:1;
	static const UINT g_nMsgFindExtNcFrameImpl;
	UINT m_nNcFrameImpl_LastExternalNcHT;
	CRect m_rcNcFrameImpl_LastExternalNcHT;

	struct __PROF_UIS_API IExtNcFrameImplBridge
	{
		virtual HWND NcFrameImplBridge_GetSafeHwnd() const = 0;
		virtual bool NcFrameImplBridge_OnQueryCaptionMergeMode() const = 0;
		virtual void NcFrameImplBridge_OnOverPaint(
			CDC & dc,
			CRect rcWnd,
			CRect rcClient
			) const = 0;
		virtual bool NcFrameImplBridge_OnOverPaint_IsEnabled() const = 0;
		virtual void NcFrameImplBridge_OnDrawCaptionText(
			CDC & dc,
			__EXT_MFC_SAFE_LPCTSTR strCaption,
			CRect rcDrawText
			) const = 0;
		virtual HRGN NcFrameImplBridge_GetNcExcludeHRGN() const = 0;
		virtual HRGN NcFrameImplBridge_GetNcResizingHRGN() const = 0;
		virtual bool NcFrameImplBridge_GetNcScRect( UINT nSC, CRect & rc ) const = 0;
		virtual bool NcFrameImpl_GetNcHtRect(
			CRect & rc,
			UINT nHT,
			bool bScreenMapping = true,
			bool bLayoutBordersH = false,
			bool bLayoutBordersV = false,
			LPMINMAXINFO pMinMaxInfo = NULL,
			LPCRECT pRectWnd = NULL
			) const = 0;
		virtual bool NcFrameImplBridge_OnQueryDrawIcon(
			CRect & rcAdjustLocation
			) const = 0;
		virtual bool NcFrameImplBridge_OnQueryDrawCaptionText(
			CRect & rcAdjustLocation,
			UINT & nAdjustDrawTextFlags
			) const = 0;
		virtual void NcFrameImpl_PreSetWindowPlacement( const WINDOWPLACEMENT & _wp ) = 0;
		virtual void NcFrameImpl_PostSetWindowPlacement( const WINDOWPLACEMENT & _wp ) = 0;
		virtual void NcFrameImpl_AdjustDwmArea( CExtUxTheme::__EXT_UX_MARGINS & _margins ) = 0;
	}; // struct IExtNcFrameImplBridge

	IExtNcFrameImplBridge * m_pNcFrameImplBridge;
public:

	class __PROF_UIS_API NcLock
	{
		const CExtNcFrameImpl & m_NcFrameImpl;
		bool m_bLocked:1;
	public:
		NcLock( const CExtNcFrameImpl & _NcFrameImpl );
		~NcLock();
		void UnlockNow();
	}; // class NcLock
	bool m_bNcFrameImpl_IsEnabled:1, m_bNcFrameImpl_IsDwmBased:1,
		m_bNcFrameImpl_Resizing:1, m_bNcFrameImpl_Moving:1;
protected:
	bool m_bNcFrameImpl_ResizingLoopTracking:1, m_bNcFrameImpl_MovingLoopTracking:1;
public:
	mutable CRect
		m_rcNcFrameImpl_Icon,
		m_rcNcFrameImpl_Text,
		m_rcNcFrameImpl_ScHelp,
		m_rcNcFrameImpl_ScClose,
		m_rcNcFrameImpl_ScMaximize,
		m_rcNcFrameImpl_ScMinimize;
	enum e_StateIndex_t
	{
		__ESI_NORMAL   = 0,
		__ESI_HOVER    = 1,
		__ESI_PRESSED  = 2,
	};
	UINT m_nNcFrameImpl_ScTrackedButtonHover,
		m_nNcFrameImpl_ScTrackedButtonPressed;
	CExtPopupMenuTipWnd m_wndNcFrameImpl_Tip;
	mutable CExtSafeString
		m_strNcFrameImpl_TipMinimize,
		m_strNcFrameImpl_TipMaximize,
		m_strNcFrameImpl_TipRestore,
		m_strNcFrameImpl_TipClose,
		m_strNcFrameImpl_TipHelp;
	CExtNcFrameImpl();
	virtual ~CExtNcFrameImpl();
	virtual bool NcFrameImpl_IsNcLocked() const;
	virtual void NcFrameImpl_NcLock( bool bLock ) const;
	virtual DWORD NcFrameImpl_GetInitialStyle() const;
	virtual DWORD NcFrameImpl_GetInitialStyleEx() const;
	virtual HWND NcFrameImpl_OnQueryHWND() = 0;
	HWND NcFrameImpl_OnQueryHWND() const;
	virtual CWnd * NcFrameImpl_GetFrameWindow();
	const CWnd * NcFrameImpl_GetFrameWindow() const;
	CRect NcFrameImpl_GetNcHtRect(
		UINT nHT,
		LPMINMAXINFO pMinMaxInfo,
		LPCRECT pRectWnd = NULL
		) const;
	virtual CRect NcFrameImpl_GetNcHtRect(
		UINT nHT,
		bool bScreenMapping = true,
		bool bLayoutBordersH = false,
		bool bLayoutBordersV = false,
		LPMINMAXINFO pMinMaxInfo = NULL,
		LPCRECT pRectWnd = NULL
		) const;
	virtual CRect NcFrameImpl_GetNcScRect( UINT nSC ) const;
	virtual void NcFrameImpl_GetIcon( CExtCmdIcon & _icon ) const;
	virtual void NcFrameImpl_ReCacheScButtonRects() const;
	virtual void NcFrameImpl_DelayRgnAdjustment();
	virtual void NcFrameImpl_SetupRgn(
		WINDOWPOS * pWndPos = NULL
		);
	virtual bool NcFrameImpl_GetMinMaxInfo(
		LPMINMAXINFO pMMI
		) const;
	static void NcFrameImpl_ExtractScTipText( const CWnd * pWnd, UINT nSC, CExtSafeString & strTipText );
	virtual __EXT_MFC_SAFE_LPCTSTR NcFrameImpl_GetScTipText( UINT nSC ) const;
	virtual bool NcFrameImpl_IsSupported() const;
	virtual bool NcFrameImpl_IsDwmBased() const;
	virtual bool NcFrameImpl_IsDwmCaptionReplacement() const;
	virtual bool NcFrameImpl_IsForceEmpty() const;
	virtual CRect NcFrameImpl_GetForceEmptyNcBorder() const;
	bool NcFrameImpl_IsForceEmptyNcBorderEmpty() const;
	virtual bool NcFrameImpl_IsActive();
	virtual bool NcFrameImpl_FilterWindowProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam );
	virtual bool NcFrameImpl_PreWindowProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam );
	virtual void NcFrameImpl_PostWindowProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam );
	virtual bool NcFrameImpl_OnQueryQuickWindowPlacement() const;
	virtual bool NcFrameImpl_RecalcNcFrame();
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	virtual bool NcFrameImpl_OnQuerySystemCommandEnabled(
		UINT nSystemCommandID
		);
	virtual void NcFrameImpl_CheckCursor(
		CPoint pointScreen,
		LPARAM nHT,
		bool bCheckWindowFromPoint = true
		);
	static CExtNcFrameImpl * NcFrameImpl_FindInstance(
		HWND hWnd,
		IExtNcFrameImplBridge * pNcFrameImplBridge = NULL
		);
	virtual CExtPaintManager * NcFrameImpl_GetPM();
	CExtPaintManager * NcFrameImpl_GetPM() const;
	virtual void NcFrameImpl_OnNcPaint(
		CDC & dcPaint,
		bool bOuterEmulationMode = false
		);
	virtual void NcFrameImpl_AdjustVistaDwmCompatibilityIssues();

	virtual bool NcFrameImpl_GetWindowPlacement( WINDOWPLACEMENT & _wp ) const;
	virtual bool NcFrameImpl_SetWindowPlacement( const WINDOWPLACEMENT & _wp );
	BOOL GetWindowPlacement( WINDOWPLACEMENT * pWP ) const;
	BOOL GetWindowPlacement( WINDOWPLACEMENT & _wp ) const;
	BOOL SetWindowPlacement( const WINDOWPLACEMENT * pWP );
	BOOL SetWindowPlacement( const WINDOWPLACEMENT & _wp );
	static bool stat_GetWindowPlacement( HWND hWnd, WINDOWPLACEMENT & _wp );
	static bool stat_SetWindowPlacement( HWND hWnd, const WINDOWPLACEMENT & _wp );
	static bool stat_SaveWindowRect(
		HWND hWnd,
		__EXT_MFC_SAFE_LPCTSTR strSection,
		__EXT_MFC_SAFE_LPCTSTR strEntry
		);
	static bool stat_LoadWindowRect(
		HWND hWnd,
		__EXT_MFC_SAFE_LPCTSTR strSection,
		__EXT_MFC_SAFE_LPCTSTR strEntry,
		bool bForceHideWindow = false
		);

	virtual void NcFrameImpl_CalcWindowRect(
		LPRECT lpClientRect,
		UINT nAdjustType = CWnd::adjustBorder
		);

	static void stat_MDI_GetWindowList(
		HWND hWndMdiClient,
		CList < HWND, HWND > & _list,
		CList < HWND, HWND > & _listWindowsToSkip,
		bool bAddActiveToTail,
		bool bIncludeEnabled,
		bool bIncludeDisabled,
		bool bIncludeIconic,
		bool bIncludeNonIconic,
		bool bIncludeWithOwner,
		bool bIncludeWithoutOwner,
		bool bIncludeVisible = true,
		bool bIncludeHidden = false
		);
	static INT stat_MDI_ArrangeIconic(
		HWND hWndMdiClient,
		CList < HWND, HWND > & _listWindowsToSkip
		);
	static INT stat_MDI_ArrangeIconicEWL( // explicit window list based version
		HWND hWndMdiClient,
		CList < HWND, HWND > & _list
		);
	static void stat_MDI_Cascade(
		HWND hWndMdiClient,
		CList < HWND, HWND > & _listWindowsToSkip,
		WPARAM nActionFlags,
		INT nStairWindowCount = 0 // zero or negative - compute automatically
		);
	static void stat_MDI_CascadeEWL( // explicit window list based version
		HWND hWndMdiClient,
		CList < HWND, HWND > & _list,
		INT nStairWindowCount = 0 // zero or negative - compute automatically
		);
	static void stat_MDI_Tile(
		HWND hWndMdiClient,
		CList < HWND, HWND > & _listWindowsToSkip,
		WPARAM nActionFlags
		);
	static void stat_MDI_TileEWL( // explicit window list based version
		HWND hWndMdiClient,
		CList < HWND, HWND > & _list,
		bool bHorizontal
		);
	virtual void NcFrameImpl_SyncThemeSettings( CExtPaintManager * pPM, bool bPostNotification );

	friend class CExtPmBridgeNC;
}; // class CExtNcFrameImpl

template < class _BTNCW >
class CExtNCW
	: public _BTNCW
	, public CExtNcFrameImpl
{
public:
	CExtNCW()
	{
	}
	virtual ~CExtNCW()
	{
	}
	virtual HWND NcFrameImpl_OnQueryHWND()
	{
		return GetSafeHwnd();
	}
protected:
	virtual void PreSubclassWindow()
	{
		_BTNCW::PreSubclassWindow();
		CExtNcFrameImpl::PreSubclassWindow();
	}
	virtual void PostNcDestroy()
	{
		CExtNcFrameImpl::PostNcDestroy();
		_BTNCW::PostNcDestroy();
	}
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		HWND hWndOwn = m_hWnd;
		LRESULT lResult = 0;
		if( NcFrameImpl_FilterWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( ! NcFrameImpl_IsSupported() )
			return _BTNCW::WindowProc( message, wParam, lParam );
		if( NcFrameImpl_PreWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		lResult = _BTNCW::WindowProc( message, wParam, lParam );
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( CWnd::FromHandlePermanent(hWndOwn) == NULL )
			return lResult;
		NcFrameImpl_PostWindowProc( lResult, message, wParam, lParam );
		return lResult;
	}
public:
	BOOL GetWindowPlacement( WINDOWPLACEMENT * pWP ) const
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::GetWindowPlacement( pWP );
	}
	BOOL SetWindowPlacement( const WINDOWPLACEMENT * pWP )
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::SetWindowPlacement( pWP );
	}
	virtual void CalcWindowRect(
		LPRECT lpClientRect,
		UINT nAdjustType = CWnd::adjustBorder
		)
	{
		NcFrameImpl_CalcWindowRect( lpClientRect, nAdjustType );
	}
}; // class CExtNCW

class __PROF_UIS_API CExtNcFrameWatchMDIMF
	: public CExtHookSink
	, public CExtPmBridge
{
public:
	DECLARE_CExtPmBridge_MEMBERS( CExtNcFrameWatchMDIMF );
	static UINT g_nMsgSurface;
	HWND m_hWndSurface;
	CExtNcFrameImpl * m_pNcFrameImpl;
	CExtNcFrameWatchMDIMF();
	virtual ~CExtNcFrameWatchMDIMF();
	void _Surface( bool bForward );
	virtual bool OnHookWndMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		);
	virtual void OnPostHookWndMsg(
		LRESULT lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM wParam,
		LPARAM lParam
		);
}; // class CExtNcFrameWatchMDIMF

template < >
class CExtNCW < CMDIFrameWnd >
	: public CMDIFrameWnd
	, public CExtNcFrameImpl
{
protected:
	CExtNcFrameWatchMDIMF m_hookWatchMDI;
public:
	CExtNCW()
	{
		m_hookWatchMDI.m_pNcFrameImpl = this;
	}
	virtual ~CExtNCW()
	{
	}
	virtual HWND NcFrameImpl_OnQueryHWND()
	{
		return GetSafeHwnd();
	}
protected:
	virtual void PreSubclassWindow()
	{
		CMDIFrameWnd::PreSubclassWindow();
		CExtNcFrameImpl::PreSubclassWindow();
	}
	virtual void PostNcDestroy()
	{
		CExtNcFrameImpl::PostNcDestroy();
		CMDIFrameWnd::PostNcDestroy();
	}
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		HWND hWndOwn = m_hWnd;
		LRESULT lResult = 0;
		if( NcFrameImpl_FilterWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( ! NcFrameImpl_IsSupported() )
			return CMDIFrameWnd::WindowProc( message, wParam, lParam );
		if( message == WM_DESTROY )
			m_hookWatchMDI.RemoveAllWndHooks();
		if( NcFrameImpl_PreWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		lResult = CMDIFrameWnd::WindowProc( message, wParam, lParam );
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( CWnd::FromHandlePermanent(hWndOwn) == NULL )
			return lResult;
		NcFrameImpl_PostWindowProc( lResult, message, wParam, lParam );
		if( message == WM_CREATE )
		{
			HWND hWndMdiClient = CExtControlBar::stat_FindMdiClientHWND( m_hWnd );
			ASSERT( hWndMdiClient != NULL );
			m_hookWatchMDI.SetupHookWndSink( hWndMdiClient );
		}
		return lResult;
	} 
public:
	virtual void OnUpdateFrameTitle( BOOL bAddToTitle )
	{
		HWND hWndOwn = GetSafeHwnd();
		CMDIFrameWnd::OnUpdateFrameTitle( bAddToTitle );
		if( hWndOwn != NULL && ::IsWindow( hWndOwn ) )
			::SendMessage( hWndOwn, WM_NCPAINT, 0L, 0L );
	}
	virtual void OnUpdateFrameMenu( HMENU hMenuAlt )
	{
		HWND hWndOwn = GetSafeHwnd();
		CMDIFrameWnd::OnUpdateFrameMenu( hMenuAlt );
		if( hWndOwn != NULL && ::IsWindow( hWndOwn ) )
			::SendMessage( hWndOwn, WM_NCPAINT, 0L, 0L );
	}
	virtual BOOL DestroyWindow()
	{
		m_hookWatchMDI.RemoveAllWndHooks();
		return CMDIFrameWnd::DestroyWindow();
	}
	BOOL GetWindowPlacement( WINDOWPLACEMENT * pWP ) const
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::GetWindowPlacement( pWP );
	}
	BOOL SetWindowPlacement( const WINDOWPLACEMENT * pWP )
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::SetWindowPlacement( pWP );
	}
	virtual void CalcWindowRect(
		LPRECT lpClientRect,
		UINT nAdjustType = CWnd::adjustBorder
		)
	{
		NcFrameImpl_CalcWindowRect( lpClientRect, nAdjustType );
	}
protected:
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam )
	{
//		BOOL bRetVal = CMDIFrameWnd :: OnCommand( wParam, lParam );
//		return bRetVal;

		CMDIChildWnd * pActiveChild = MDIGetActive();
		if (pActiveChild != NULL && AfxCallWndProc( pActiveChild, pActiveChild->m_hWnd, WM_COMMAND, wParam, lParam) != 0)
			return TRUE;

		if( CFrameWnd::OnCommand( wParam, lParam ) )
			return TRUE;

		HWND hWndCtrl = (HWND)lParam;

		//ASSERT( AFX_IDM_FIRST_MDICHILD == 0xFF00 );
		if (hWndCtrl == NULL && (LOWORD(wParam) & 0xf000) == 0xf000)
		{
			m_hookWatchMDI._Surface( true );
			::SendMessage( m_hWndMDIClient, WM_SETREDRAW, 0, 0 );
			DefWindowProc( WM_COMMAND, wParam, lParam );
			::SendMessage( m_hWndMDIClient, WM_SETREDRAW, 1, 0 );
			m_hookWatchMDI._Surface( false );
			return TRUE;
		}
		return FALSE;
	}
}; // class CExtNCW

template < >
class CExtNCW < CExtResizableDialog >
	: public CExtResizableDialog
	, public CExtNcFrameImpl
{
public:
	CExtNCW()
	{
	}
	CExtNCW(
		UINT nIDTemplate,
		CWnd * pParentWnd = NULL
		)
		: CExtResizableDialog( nIDTemplate, pParentWnd )
	{
	}
	CExtNCW(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		CWnd * pParentWnd = NULL
		)
		: CExtResizableDialog( lpszTemplateName, pParentWnd )
	{
	}
	virtual ~CExtNCW()
	{
	}
	virtual HWND NcFrameImpl_OnQueryHWND()
	{
		return GetSafeHwnd();
	}
protected:
	virtual void PreSubclassWindow()
	{
		CExtResizableDialog::PreSubclassWindow();
		CExtNcFrameImpl::PreSubclassWindow();
	}
	virtual void PostNcDestroy()
	{
		CExtNcFrameImpl::PostNcDestroy();
		CExtResizableDialog::PostNcDestroy();
	}
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		HWND hWndOwn = m_hWnd;
		LRESULT lResult = 0;
		if( NcFrameImpl_FilterWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( ! NcFrameImpl_IsSupported() )
			return CExtResizableDialog::WindowProc( message, wParam, lParam );
		if( NcFrameImpl_PreWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		lResult = CExtResizableDialog::WindowProc( message, wParam, lParam );
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( CWnd::FromHandlePermanent(hWndOwn) == NULL )
			return lResult;
		NcFrameImpl_PostWindowProc( lResult, message, wParam, lParam );
		return lResult;
	}
public:
	BOOL GetWindowPlacement( WINDOWPLACEMENT * pWP ) const
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::GetWindowPlacement( pWP );
	}
	BOOL SetWindowPlacement( const WINDOWPLACEMENT * pWP )
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::SetWindowPlacement( pWP );
	}
	virtual void CalcWindowRect(
		LPRECT lpClientRect,
		UINT nAdjustType = CWnd::adjustBorder
		)
	{
		NcFrameImpl_CalcWindowRect( lpClientRect, nAdjustType );
	}

	virtual bool NcFrameImpl_GetMinMaxInfo(
		LPMINMAXINFO pMMI
		) const
	{
		CExtNcFrameImpl::NcFrameImpl_GetMinMaxInfo( pMMI );
		return CExtResizableDialog::NcFrameImpl_GetMinMaxInfo( pMMI );
	}
}; // class CExtNCW

template < >
class CExtNCW < CExtResizablePropertySheet >
	: public CExtResizablePropertySheet
	, public CExtNcFrameImpl
{
public:
	CExtNCW()
	{
	}
	CExtNCW(
		UINT nIDCaption,
		CWnd *pParentWnd = NULL,
		UINT iSelectPage = 0
		)
		: CExtResizablePropertySheet( nIDCaption, pParentWnd, iSelectPage )
	{
	}
	CExtNCW(
		__EXT_MFC_SAFE_LPCTSTR pszCaption,
		CWnd *pParentWnd = NULL,
		UINT iSelectPage = 0
		)
		: CExtResizablePropertySheet( pszCaption, pParentWnd, iSelectPage )
	{
	}
	virtual ~CExtNCW()
	{
	}
	virtual HWND NcFrameImpl_OnQueryHWND()
	{
		return GetSafeHwnd();
	}
protected:
	virtual void PreSubclassWindow()
	{
		CExtResizablePropertySheet::PreSubclassWindow();
		CExtNcFrameImpl::PreSubclassWindow();
	}
	virtual void PostNcDestroy()
	{
		CExtNcFrameImpl::PostNcDestroy();
		CExtResizablePropertySheet::PostNcDestroy();
	}
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		HWND hWndOwn = m_hWnd;
		LRESULT lResult = 0;
		if( NcFrameImpl_FilterWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( ! NcFrameImpl_IsSupported() )
			return CExtResizablePropertySheet::WindowProc( message, wParam, lParam );
		if( NcFrameImpl_PreWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		lResult = CExtResizablePropertySheet::WindowProc( message, wParam, lParam );
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( CWnd::FromHandlePermanent(hWndOwn) == NULL )
			return lResult;
		NcFrameImpl_PostWindowProc( lResult, message, wParam, lParam );
		return lResult;
	}
public:
	BOOL GetWindowPlacement( WINDOWPLACEMENT * pWP ) const
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::GetWindowPlacement( pWP );
	}
	BOOL SetWindowPlacement( const WINDOWPLACEMENT * pWP )
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::SetWindowPlacement( pWP );
	}
	virtual void CalcWindowRect(
		LPRECT lpClientRect,
		UINT nAdjustType = CWnd::adjustBorder
		)
	{
		NcFrameImpl_CalcWindowRect( lpClientRect, nAdjustType );
	}

	virtual bool NcFrameImpl_GetMinMaxInfo(
		LPMINMAXINFO pMMI
		) const
	{
		CExtNcFrameImpl::NcFrameImpl_GetMinMaxInfo( pMMI );
		return CExtResizablePropertySheet::NcFrameImpl_GetMinMaxInfo( pMMI );
	}
}; // class CExtNCW

template < >
class CExtNCW < CFileDialog >
	: public CFileDialog
	, public CExtNcFrameImpl
{
public:
	CExtNCW(
		BOOL bOpenFileDialog,
		__EXT_MFC_SAFE_LPCTSTR lpszDefExt = NULL,
		__EXT_MFC_SAFE_LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		__EXT_MFC_SAFE_LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL
		)
		: CFileDialog( 
			bOpenFileDialog, 
			lpszDefExt, 
			lpszFileName, 
			dwFlags, 
			lpszFilter, 
			pParentWnd 
			)
	{
	}
	virtual ~CExtNCW()
	{
	}
	virtual HWND NcFrameImpl_OnQueryHWND()
	{
		return GetSafeHwnd();
	}
protected:
	virtual void PreSubclassWindow()
	{
		CFileDialog::PreSubclassWindow();
		CExtNcFrameImpl::PreSubclassWindow();
	}
	virtual void PostNcDestroy()
	{
		CExtNcFrameImpl::PostNcDestroy();
		CFileDialog::PostNcDestroy();
	}
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		HWND hWndOwn = m_hWnd;
		LRESULT lResult = 0;
		if( NcFrameImpl_FilterWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( ! NcFrameImpl_IsSupported() )
			return CFileDialog::WindowProc( message, wParam, lParam );
		if( NcFrameImpl_PreWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		lResult = CFileDialog::WindowProc( message, wParam, lParam );
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( CWnd::FromHandlePermanent(hWndOwn) == NULL )
			return lResult;
		NcFrameImpl_PostWindowProc( lResult, message, wParam, lParam );
		return lResult;
	}
public:
	BOOL GetWindowPlacement( WINDOWPLACEMENT * pWP ) const
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::GetWindowPlacement( pWP );
	}
	BOOL SetWindowPlacement( const WINDOWPLACEMENT * pWP )
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::SetWindowPlacement( pWP );
	}
	virtual void CalcWindowRect(
		LPRECT lpClientRect,
		UINT nAdjustType = CWnd::adjustBorder
		)
	{
		NcFrameImpl_CalcWindowRect( lpClientRect, nAdjustType );
	}
}; // class CExtNCW

template < >
class CExtNCW < CPrintDialog >
	: public CPrintDialog
	, public CExtNcFrameImpl
{
public:
	CExtNCW(
		BOOL bPrintSetupOnly,
		DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION,
		CWnd* pParentWnd = NULL
		)
		: CPrintDialog(
			bPrintSetupOnly,
			dwFlags,
			pParentWnd
		)
	{
	}
	virtual ~CExtNCW()
	{
	}
	virtual HWND NcFrameImpl_OnQueryHWND()
	{
		return GetSafeHwnd();
	}
protected:
	virtual void PreSubclassWindow()
	{
		CPrintDialog::PreSubclassWindow();
		CExtNcFrameImpl::PreSubclassWindow();
	}
	virtual void PostNcDestroy()
	{
		CExtNcFrameImpl::PostNcDestroy();
		CPrintDialog::PostNcDestroy();
	}
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		HWND hWndOwn = m_hWnd;
		LRESULT lResult = 0;
		if( NcFrameImpl_FilterWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( ! NcFrameImpl_IsSupported() )
			return CPrintDialog::WindowProc( message, wParam, lParam );
		if( NcFrameImpl_PreWindowProc( lResult, message, wParam, lParam ) )
			return lResult;
		lResult = CPrintDialog::WindowProc( message, wParam, lParam );
		if( ! ::IsWindow( hWndOwn ) )
			return lResult;
		if( CWnd::FromHandlePermanent(hWndOwn) == NULL )
			return lResult;
		NcFrameImpl_PostWindowProc( lResult, message, wParam, lParam );
		return lResult;
	}
public:
	BOOL GetWindowPlacement( WINDOWPLACEMENT * pWP ) const
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::GetWindowPlacement( pWP );
	}
	BOOL SetWindowPlacement( const WINDOWPLACEMENT * pWP )
	{
		ASSERT_VALID( this );
		return CExtNcFrameImpl::SetWindowPlacement( pWP );
	}
	virtual void CalcWindowRect(
		LPRECT lpClientRect,
		UINT nAdjustType = CWnd::adjustBorder
		)
	{
		NcFrameImpl_CalcWindowRect( lpClientRect, nAdjustType );
	}
}; // class CExtNCW

#endif // (!defined __EXT_MFC_NO_NC_FRAME )

#endif // __EXT_NC_FRAME_H

