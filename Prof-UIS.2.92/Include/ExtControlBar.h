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

#if (!defined __EXT_CONTROLBAR_H)
#define __EXT_CONTROLBAR_H

//
// Many thanks to Paul DiLascia & Cristi Posea, their works
// were a powerful incentive to learn more about pretty
// complicated MFC docking windows mechanism
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#if (!defined __AFXMT_H__)
//	#include <AfxMT.h>
//#endif

#if (!defined __AFXTEMPL_H__)
	#include <AfxTempl.h>
#endif

#if (!defined __EXT_MOUSECAPTURESINK_H)
	#include <../Src/ExtMouseCaptureSink.h>
#endif

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

	#if (!defined __EXT_HOOK_H)
		#include <../Src/ExtHook.h>
	#endif

#endif // (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __EXT_HOOK_H)
	#include <../Src/ExtHook.h>
#endif

#define __EXT_TB_BUTTON_SEPARATOR_WIDTH 6
#define __EXT_TB_BUTTON_SEPARATOR_HEIGHT 6
#define __EXT_TB_BUTTON_TEXT_MARGIN 3 // 5
#define __EXT_TB_BUTTON_INNER_MARGIN 0 // 1
#define __EXT_TB_BUTTON_OUTER_MARGIN 3
#define __EXT_TB_ROW_DISTANCE 6

class CDockBar;
class CExtDockBar;
class CExtDockOuterBar;
class CExtMiniDockFrameWnd;
class CExtPopupMenuWnd;
class CExtPopupMenuTipWnd;
class CExtControlBar;
class CExtToolControlBar;
class CExtMenuControlBar;
class CExtBarButton;
class CExtBarContentExpandButton;
class CExtBarMdiDocButton;
class CExtControlBar;
class CExtDynControlBar;
class CExtDockDynBar;
class CExtDynDockMarkerWnd;
class CExtDynDockHighlightWnd;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	class CExtDockDynTabBar;
	class CExtDynTabWnd;
	class CExtDynTabControlBar;
	class CExtDynAutoHideArea;
	class CExtDynAutoHideSlider;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

typedef
	CArray < CExtControlBar*, CExtControlBar* >
	ExtControlBarVector_t;

typedef
	CArray < CControlBar*, CControlBar* >
	MfcControlBarVector_t;

class CExtAnimationClient;
class CExtAnimationSite;

/////////////////////////////////////////////////////////////////////////////
// CExtAnimationParameters

#define __EXT_MFC_DEF_ANIMATION_STEP_COUNT				   5
#define __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX			 255
#define __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE			  40
#define __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID		   0
#define __EXT_MFC_DEF_ANIMATION_TIMER_BASE_TIMER_ID		8700
#define __EXT_MFC_DEF_ANIMATION_TIMER_MAX_TIMER_ID		( __EXT_MFC_DEF_ANIMATION_TIMER_BASE_TIMER_ID + 1000 - 1 )

#define __EAPT_BY_HOVERED_STATE_TURNED_ON		0
#define __EAPT_BY_HOVERED_STATE_TURNED_OFF		1
#define __EAPT_BY_PRESSED_STATE_TURNED_ON		2
#define __EAPT_BY_PRESSED_STATE_TURNED_OFF		3
#define __EAPT_BY_SELECTED_STATE_TURNED_ON		4
#define __EAPT_BY_SELECTED_STATE_TURNED_OFF		5

class CExtAnimationParameters
{
protected:
	INT m_nAcStepCount;
	UINT m_nAcTimerElapse;
public:
	CExtAnimationParameters(
		INT nAcStepCount = __EXT_MFC_DEF_ANIMATION_STEP_COUNT,
		UINT nAcTimerElapse = __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE
		);
	virtual ~CExtAnimationParameters();
	virtual INT AnimationClient_StepCountGet() const;
	virtual void AnimationClient_StepCountSet( INT nAcStepCount );
	virtual UINT AnimationClient_TimerElapseGet() const;
	virtual void AnimationClient_TimerElapseSet( UINT nAcTimerElapse );
};

/////////////////////////////////////////////////////////////////////////////
// CExtAnimationClient

class __PROF_UIS_API CExtAnimationClient
{
protected:
	CExtAnimationSite * m_pAcAS;
	CExtBitmap m_bmpAcStatePrev, m_bmpAcStateNext;
	INT m_nAcStepNumber, m_nAcStepCount;
	UINT m_nAcTimerElapse;
	bool m_bAcEnabled:1, m_bAcNonClientMode:1;
	CRect m_rcAcTarget;
	INT m_nAcCacheGeneratorLock;
	INT m_eAPT_Last;
public:
	CExtAnimationClient(
		CExtAnimationSite * pAcAS = NULL,
		bool bAcEnabled = true,
		bool bAcNonClientMode = false,
		INT nAcStepNumber = 0,
		INT nAcStepCount = __EXT_MFC_DEF_ANIMATION_STEP_COUNT,
		UINT nAcTimerElapse = __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE,
		const RECT * pAcRcAnimationTarget = NULL
		);
	virtual ~CExtAnimationClient();
	virtual void AnimationClient_CacheGeneratorLock();
	virtual void AnimationClient_CacheGeneratorUnlock();
	virtual bool AnimationClient_CacheGeneratorIsLocked() const;
	virtual CExtAnimationSite * AnimationClient_SiteGet();
	const CExtAnimationSite * AnimationClient_SiteGet() const;
	virtual void AnimationClient_SiteSet(
		CExtAnimationSite * pAcAS = NULL
		);
	virtual CExtBitmap & AnimationClient_StateGet( bool bAcNext );
	const CExtBitmap & AnimationClient_StateGet( bool bAcNext ) const;
	virtual bool AnimationClient_IsEmpty() const;
	virtual void AnimationClient_Empty();
	virtual INT AnimationClient_StepNumberGet() const;
	virtual void AnimationClient_StepNumberSet( INT nAcStepNumber );
	virtual INT AnimationClient_StepCountGet() const;
	virtual void AnimationClient_StepCountSet( INT nAcStepCount );
	virtual UINT AnimationClient_TimerElapseGet() const;
	virtual void AnimationClient_TimerElapseSet( UINT nAcTimerElapse );
	virtual bool AnimationClient_NonClientModeGet() const;
	virtual void AnimationClient_NonClientModeSet(
		bool bAcNonClientMode = false
		);
	virtual bool AnimationClient_EnabledGet() const;
	virtual void AnimationClient_EnabledSet(
		bool bAcEnabled = true
		);
	virtual bool AnimationClient_OnQueryEnabledState(
		INT eAPT // __EAPT_*** animation type
		) const;
	virtual CRect AnimationClient_TargetRectGet() const;
	virtual void AnimationClient_TargetRectSet(
		const RECT & rcAcAnimationTarget
		);
	virtual bool AnimationClient_CacheNextState(
		CDC & dc,
		const RECT & rcAcAnimationTarget,
		bool bAnimate,
		INT eAPT // __EAPT_*** animation type
		);
	bool AnimationClient_CacheNextState(
		HWND hWnd,
		const RECT & rcAcAnimationTarget,
		bool bAnimate,
		INT eAPT // __EAPT_*** animation type
		);
	bool AnimationClient_CacheNextStateMinInfo(
		bool bAnimate,
		INT eAPT // __EAPT_*** animation type
		);
	virtual void AnimationClient_NextStatePrepare(
		CDC & dc,
		const RECT & rcAcAnimationTarget,
		bool bAnimate,
		INT eAPT // __EAPT_*** animation type
		);
	virtual void AnimationClient_NextStateAdjust(
		CExtBitmap & _bmp
		);
	virtual void AnimationClient_StateDoStep();
	virtual bool AnimationClient_StatePaint(
		CDC & dc
		);
	bool AnimationClient_StatePaint(
		HWND hWnd
		);
	bool AnimationClient_StatePaint();
	virtual void AnimationClient_OnProgressStart(
		INT eAPT // __EAPT_*** animation type
		);
	virtual void AnimationClient_OnProgressStop(
		INT eAPT // __EAPT_*** animation type
		);
	virtual INT AnimationClient_OnStepCountGet(
		INT eAPT // __EAPT_*** animation type
		) const;
	virtual UINT AnimationClient_OnTimerElapseGet(
		INT eAPT // __EAPT_*** animation type
		) const;
	virtual const CExtAnimationParameters *
		AnimationClient_OnQueryAnimationParameters(
			INT eAPT // __EAPT_*** animation type
			) const;
	virtual INT AnimationClient_LastAnimationTypeGet() const;
	virtual void AnimationClient_LastAnimationTypeSet(
		INT eAPT // __EAPT_*** animation type
		);
}; // class CExtAnimationClient

/////////////////////////////////////////////////////////////////////////////
// CExtAnimationSite

class __PROF_UIS_API CExtAnimationSite
	: public CExtPmBridge
{
protected:
	CTypedPtrArray < CPtrArray, CExtAnimationClient * > m_arrAsAnimationClients;
	CTypedPtrMap < CMapPtrToWord, CExtAnimationClient *, WORD > m_mapAsAnimationClients;
	CMap < WORD, WORD, WORD, WORD > m_mapAsAnimationTimers; // key = nElapse, value = timer ID
	bool m_bAsEnabled:1;
public:
	DECLARE_CExtPmBridge_MEMBERS_GENERIC( CExtAnimationSite );
	CExtAnimationSite(
		bool bAsEnabled = true
		);
	virtual ~CExtAnimationSite();
	virtual bool AnimationSite_EnabledGet() const;
	virtual void AnimationSite_EnabledSet(
		bool bAsEnabled = true
		);
	virtual bool AnimationSite_OnQueryEnabledState() const;
	virtual INT AnimationSite_ClientGetCount(
		bool bAsIncludeClientArea = true,
		bool bAsIncludeNonClientArea = true
		) const;
	virtual CExtAnimationClient * AnimationSite_ClientGetAt( INT nAcIndex );
	const CExtAnimationClient * AnimationSite_ClientGetAt( INT nAcIndex ) const;
	virtual bool AnimationSite_ClientInsertAt( INT nAcIndex, CExtAnimationClient * pAC );
	bool AnimationSite_ClientAdd( CExtAnimationClient * pAC );
	virtual INT AnimationSite_ClientGetIndexOf( const CExtAnimationClient * pAC ) const;
	virtual INT AnimationSite_ClientRemove(
		INT nAcIndex = 0,
		INT nAcCount = -1,
		bool bDelete = false
		);
	bool AnimationSite_ClientRemove(
		CExtAnimationClient * pAC,
		bool bDelete = false
		);
	virtual HWND AnimationSite_GetSafeHWND() const = 0;
	virtual WORD AnimationSite_TimerGetBaseID();
	virtual WORD AnimationSite_TimerGetMaxID();
	virtual bool AnimationSite_ClientProgressStart(
		CExtAnimationClient * pAC,
		INT eAPT // __EAPT_*** animation type
		);
	virtual void AnimationSite_ClientProgressStop(
		CExtAnimationClient * pAC
		);
protected:
	class __PROF_UIS_API AnimationHookSink : public CExtHookSink
	{
		bool m_bHookEnabled:1;
		CExtAnimationSite * m_pAS;
	public:
		AnimationHookSink(
			CExtAnimationSite * pAS = NULL
			);
		virtual ~AnimationHookSink();
		CExtAnimationSite * AnimationSiteGet();
		const CExtAnimationSite * AnimationSiteGet() const;
		void AnimationSiteSet( CExtAnimationSite * pAS, bool bHookEnabled );
		bool AnimationInit( bool bHookEnabled );
		void AnimationDone();
	protected:
		virtual bool OnHookWndMsg(
			LRESULT & lResult,
			HWND hWndHooked,
			UINT nMessage,
			WPARAM & wParam,
			LPARAM & lParam
			);
		friend class CExtAnimationSite;
	}; // class AnimationHookSink
	friend class AnimationHookSink;
	AnimationHookSink m_AcHS;
	virtual bool AnimationSite_OnHookWndMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		);
public:
	virtual bool AnimationSite_OnHookTimer(
		UINT nTimerID
		);
	virtual void AnimationSite_OnAnimationHookInit();
	virtual void AnimationSite_OnAnimationHookDone();
	virtual bool AnimationSite_Init( bool bHookEnabled );
	virtual void AnimationSite_Done();
	virtual void AnimationSite_OnClientStateChanged(
		CExtAnimationClient * pAC,
		INT eAPT // __EAPT_*** animation type
		);
	virtual void AnimationSite_PassAnimationEvents();
	virtual void AnimationSite_OnProgressShutdownTimer( UINT nTimerID );

	virtual HWND PmBridge_GetSafeHwnd() const;
	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);
}; // class CExtAnimationSite

/////////////////////////////////////////////////////////////////////////////
// CExtAnimationSingleton

class __PROF_UIS_API CExtAnimationSingleton
	: public CExtAnimationSite
	, public CExtAnimationClient
{
public:
	CExtAnimationSingleton(
		bool bAsEnabled = true,
		INT nAcStepNumber = 0,
		INT nAcStepCount = __EXT_MFC_DEF_ANIMATION_STEP_COUNT,
		UINT nAcTimerElapse = __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE,
		const RECT * pAcRcAnimationTarget = NULL
		);
	virtual ~CExtAnimationSingleton();
}; // class CExtAnimationSingleton

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButton

class __PROF_UIS_API CExtBarNcAreaButton : public CObject
{
protected:
	CExtControlBar * m_pBar;
	CRect m_rc;
	bool m_bHover:1, m_bPushed:1, m_bDisabled:1,
		m_bPaintingCombinedArea:1;

public:

	DECLARE_DYNCREATE( CExtBarNcAreaButton );

	CExtBarNcAreaButton(
		CExtControlBar * pBar = NULL
		);
	virtual ~CExtBarNcAreaButton();

	virtual void OnNcDestroy();

	const CRect & RectGet() const
	{
		ASSERT_VALID( this );
		return m_rc;
	}
	CRect & RectGet()
	{
		ASSERT_VALID( this );
		return m_rc;
	}
	void RectSet( const RECT & rc )
	{
		ASSERT_VALID( this );
		m_rc = rc;
	}

	operator CSize() const
	{
		ASSERT_VALID( this );
		return m_rc.Size();
	}
	operator const CRect & () const
	{
		ASSERT_VALID( this );
		return m_rc;
	}
	operator CRect & ()
	{
		ASSERT_VALID( this );
		return m_rc;
	}

	bool HoverGet() const
	{
		ASSERT_VALID( this );
		return m_bHover;
	}
	void HoverSet( bool bSet )
	{
		ASSERT_VALID( this );
		m_bHover = bSet;
	}
	bool PushedGet() const
	{
		ASSERT_VALID( this );
		return m_bPushed;
	}
	void PushedSet( bool bSet )
	{
		ASSERT_VALID( this );
		m_bPushed = bSet;
	}
	bool DisabledGet() const
	{
		ASSERT_VALID( this );
		return m_bDisabled;
	}
	void DisabledSet( bool bSet )
	{
		ASSERT_VALID( this );
		m_bDisabled = bSet;
	}
	bool PaintingCombinedAreaGet() const
	{
		ASSERT_VALID( this );
		return m_bPaintingCombinedArea;
	}
	void PaintingCombinedAreaSet( bool bSet )
	{
		ASSERT_VALID( this );
		m_bPaintingCombinedArea = bSet;
	}

	bool IsBarFixedMode() const;
	bool IsBarVisible() const;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	virtual bool OnQueryDeactivateAutohideSlider() const;
	bool IsBarAutoHideMode(
		bool * p_bDisplayed = NULL,
		bool * p_bActive = NULL
		) const;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	bool IsBarFloated() const;
	bool IsBarDocked() const;
	bool IsBarDockedHorizontally() const;
	bool IsBarDockedVertically() const;
	bool IsBarDockedAtLeft() const;
	bool IsBarDockedAtRight() const;
	bool IsBarDockedAtTop() const;
	bool IsBarDockedAtBottom() const;
	bool GetBarNcAreaInfo( // returns false if no nc area on bar
		LPRECT pRectGripper = NULL, // in window coords
		LPRECT pRectWnd = NULL, // in screen coords
		bool * pbGripperAtTop = NULL,
		bool * pbTextOnGripper = NULL
		) const;
	bool IsBarWndActive() const;
	bool IsBarSingleOnRow() const;
	bool IsBarMinimizedOnRow() const;
	bool IsBarMaximizedOnRow() const;
	bool IsBarFirstOnRow( bool bExtBar ) const;
	bool IsBarLastOnRow( bool bExtBar ) const;
	CFrameWnd * GetDockingFrame();

	CExtControlBar * GetBar()
	{
		ASSERT_VALID( this );
		return m_pBar;
	}
	const CExtControlBar * GetBar() const
	{
		ASSERT_VALID( this );
		return m_pBar;
	}

	virtual void OnNcAreaReposition(
		CExtBarNcAreaButton * pPrevBtn
		);

	virtual bool OnQueryVisibility() const;
	
	void NcDrawDefault(
		CDC & dc,
		int nDockBtnGlyptT
		);

	virtual void OnNcAreaDraw( CDC & dc );

	virtual UINT OnNcAreaHitTest( CPoint point );
	virtual HCURSOR OnNcAreaQueryCursor( CPoint point );

	virtual bool OnNcAreaClicked( CPoint point );

	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);

	__EXT_MFC_INT_PTR DoDefaultReflectForToolHitTest(
		TOOLINFO * pTI,
		UINT nIdResourceTooltipText,
		__EXT_MFC_SAFE_LPCTSTR sTooltipTextDefault
		);
	__EXT_MFC_INT_PTR DoDefaultReflectForToolHitTest(
		TOOLINFO * pTI,
		__EXT_MFC_SAFE_LPCTSTR sTooltipText
		);

#ifdef _DEBUG
	virtual void AssertValid() const
	{
		ASSERT( m_pBar != NULL );
		CObject::AssertValid();
	}
	
	virtual void Dump( CDumpContext & dc ) const
	{
		CObject::Dump( dc );
	}
#endif

	friend class CExtControlBar;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	friend class CExtDynAutoHideSlider;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}; // class CExtBarNcAreaButton

/////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonFloatingMaximizeRestore
//
//class __PROF_UIS_API CExtBarNcAreaButtonFloatingMaximizeRestore : public CExtBarNcAreaButton
//{
//public:
//
//	DECLARE_DYNCREATE( CExtBarNcAreaButtonFloatingMaximizeRestore );
//
//	CExtBarNcAreaButtonFloatingMaximizeRestore(
//		CExtControlBar * pBar = NULL
//		);
//
//	virtual bool OnQueryVisibility() const;
//	virtual void OnNcAreaDraw( CDC & dc );
//	virtual UINT OnNcAreaHitTest( CPoint point );
//	virtual bool OnNcAreaClicked( CPoint point );
//
//	virtual __EXT_MFC_INT_PTR OnToolHitTest(
//		CPoint point,
//		TOOLINFO * pTI
//		);
//
//	static bool stat_IsBarInFloatingMaximizedState(
//		const CExtControlBar * pBar
//		);
//	virtual bool IsBarInFloatingMaximizedState() const;
//
//}; // class CExtBarNcAreaButtonFloatingMaximizeRestore

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonExpand

class __PROF_UIS_API CExtBarNcAreaButtonExpand : public CExtBarNcAreaButton
{
public:

	DECLARE_DYNCREATE( CExtBarNcAreaButtonExpand );

	CExtBarNcAreaButtonExpand(
		CExtControlBar * pBar = NULL
		);

	virtual bool OnQueryVisibility() const;
	virtual void OnNcAreaDraw( CDC & dc );
	virtual UINT OnNcAreaHitTest( CPoint point );
	virtual bool OnNcAreaClicked( CPoint point );

	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);

}; // class CExtBarNcAreaButtonExpand

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonMenu

class __PROF_UIS_API CExtBarNcAreaButtonMenu : public CExtBarNcAreaButton
{
public:

	DECLARE_DYNCREATE( CExtBarNcAreaButtonMenu );

	CExtBarNcAreaButtonMenu(
		CExtControlBar * pBar = NULL
		);

	static void _CbPaintCombinedContent(
		LPVOID pCookie,
		CDC & dc,
		const CWnd & refWndMenu,
		const CRect & rcExcludeArea, // in screen coords
		int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
		);

	virtual bool OnQueryVisibility() const;
	virtual void OnNcAreaDraw( CDC & dc );
	virtual UINT OnNcAreaHitTest( CPoint point );
	virtual bool OnNcAreaClicked( CPoint point );

	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);
}; // class CExtBarNcAreaButtonMenu

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonClose

class __PROF_UIS_API CExtBarNcAreaButtonClose : public CExtBarNcAreaButton
{
public:

	DECLARE_DYNCREATE( CExtBarNcAreaButtonClose );

	CExtBarNcAreaButtonClose(
		CExtControlBar * pBar = NULL
		);

	virtual bool OnQueryVisibility() const;
	virtual void OnNcAreaDraw( CDC & dc );
	virtual UINT OnNcAreaHitTest( CPoint point );
	virtual bool OnNcAreaClicked( CPoint point );

	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);

}; // class CExtBarNcAreaButtonClose

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonAutoHide

class __PROF_UIS_API CExtBarNcAreaButtonAutoHide : public CExtBarNcAreaButton
{
public:

	DECLARE_DYNCREATE( CExtBarNcAreaButtonAutoHide );

	CExtBarNcAreaButtonAutoHide(
		CExtControlBar * pBar = NULL
		);

	virtual bool OnQueryVisibility() const;
	virtual void OnNcAreaDraw( CDC & dc );
	virtual UINT OnNcAreaHitTest( CPoint point );
	virtual bool OnNcAreaClicked( CPoint point );

	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);

	virtual void OnNcAreaReposition(
		CExtBarNcAreaButton * pPrevBtn
		);
}; // class CExtBarNcAreaButtonAutoHide

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)


class CExtCustomizeToolbarsListBox;

/////////////////////////////////////////////////////////////////////////////
// CExtControlBar window

class __PROF_UIS_API CExtControlBar
	: public CControlBar
	, public CExtMouseCaptureSink
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, public CExtCustomizeSite::ICustomizeDropTarget
	, public CExtCustomizeSite::ICustomizeDropSource
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	, public CExtAnimationSite
{
	struct __PROF_UIS_API InternalDockBarInfo_t
	{
		CMap < UINT, UINT, DWORD, DWORD > m_mapDockBarInfo;
		InternalDockBarInfo_t();
		DWORD operator [] ( UINT nDockBarID );
		bool ReplaceFrameDockBar(
			UINT nDockBarID,
			DWORD dwDockStyle,
			CFrameWnd * pFrame
			);
	}; // struct InternalDockBarInfo_t

	static InternalDockBarInfo_t g_DockBarInfo;

public:

	class __PROF_UIS_API InternalAffixmentData
	{
		void _AssignFromOther(
			const InternalAffixmentData & other
			);
	public:
		UINT m_nAffixmentWeight;
		CRect m_rcAffixment;
		CSize m_sizeCompressed;
		DWORD m_dwAffixmentFlags;
		enum e_affixment_flags
		{
			__EAFF_FIXED_MODE_INITIAL_RECALC = 0x00000001L
		};
		InternalAffixmentData();
		InternalAffixmentData(
			const InternalAffixmentData & other
			);
		~InternalAffixmentData();
		bool IsEmpty() const;
		void Empty();
		bool operator ! ()
		{
			return (! IsEmpty() );
		}
		InternalAffixmentData & operator = (
			const InternalAffixmentData & other
			)
		{
			_AssignFromOther( other );
			return *this;
		}
		int CompareAffixment(
			const InternalAffixmentData & other
			) const;
		void Serialize( CArchive & ar );
	}; // class InternalAffixmentData

	class __PROF_UIS_API InternalDockStateBar : public CObject
	{
	public:
		UINT m_nBarID, m_nDockBarID, m_nCircleNo, m_nMRUWidth;
		DWORD m_dwBarFlags, m_dwDockStateFlags, m_dwMRUFloatStyle,
			m_dwFloatingFrameCreateStyle;
		LONG m_nDelayedRowUpdateMetric;
		CPoint m_ptFloatPos, m_ptMRUFloatPos;
		CRect m_rcMRUDockPos, m_rcFloatingFrame, m_rcBar;
		CSize m_sizeDockedH, m_sizeDockedV, m_sizeFloated;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		INT m_nAutoHideWidth, m_nAutoHideHeight;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		CArray <
			InternalDockStateBar *,
			InternalDockStateBar *
			> m_arrChildStates;

		LONG m_nHelperAciveTabChildIndex; 
		CExtControlBar * m_pHelperBar;

		// m_dwDockStateFlags values
		enum
		{
			__DOCK_AT_NOWHERE				= 0x00000000,
			__DOCK_AT_SIDE					= 0x00000001,
			__DOCK_DYNAMIC_HALF_SPLIT		= 0x00000002,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			__DOCK_DYNAMIC_TABBED			= 0x00000003,
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			__DOCK_AT_MASK					= 0x0000000F,
			__STATE_VISIBLE					= 0x00000010,
			__STATE_FLOATING				= 0x00000020,
			__STATE_NEW_ROW					= 0x00000040,
			__STATE_DYNAMIC_CHILD			= 0x00000080,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			__STATE_TAB_CHILD				= 0x00000100,
			__STATE_TAB_ACTIVE				= 0x00000200,
			__STATE_AUTOHIDDEN				= 0x00000400,
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		};

		InternalDockStateBar();
		~InternalDockStateBar();
		void InitMembers();
		virtual void Serialize( CArchive & ar );
		void Clear();
		void StateGet(
			CExtControlBar * pBar,
			InternalDockStateBar * pParentState
			);
		void StateSet(
			CFrameWnd * pDockSite,
			InternalDockStateBar * pParentState
			);
		void PreSyncAttributes(
			CFrameWnd * pDockSite,
			InternalDockStateBar * pParentState
			);
		void PostSyncAttributes(
			CFrameWnd * pDockSite,
			InternalDockStateBar * pParentState
			);
	}; // class InternalDockStateBar

	struct __PROF_UIS_API OuterItemData_t
	{
		UINT m_nBarID;
		CPoint m_ptFloatHelper;
		CSize m_sizeDockedH, m_sizeDockedV, m_sizeFloated;
		InternalAffixmentData m_AffixmentData;
		bool m_bFloating:1, m_bVisible:1;
		OuterItemData_t();
		OuterItemData_t(
			const OuterItemData_t & other
			);
		OuterItemData_t & operator = (
			const OuterItemData_t & other
			);
		void Serialize( CArchive & ar );
	}; // struct OuterItemData_t

	typedef
		CArray < OuterItemData_t, OuterItemData_t & >
		OuterPropsArr_t;

	class __PROF_UIS_API InternalDockStateSite : public CObject
	{
	public:
		
		OuterPropsArr_t m_arrOuterProps;

		CFrameWnd * m_pDockSite;
		WINDOWPLACEMENT m_wp;
		CDockState * m_pOuterState;
		bool m_bSerializeFixedBarsState:1,
			m_bSerializeResizableBarsState:1;

		CArray <
			InternalDockStateBar *,
			InternalDockStateBar *
			> m_arrBarStates;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		CArray < UINT, UINT > m_arrAutoHideOrder;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		InternalDockStateSite(
			CFrameWnd * pDockSite,
			bool bSerializeFixedBarsState = true,
			bool bSerializeResizableBarsState = true
			);
		~InternalDockStateSite();

		virtual void Serialize( CArchive & ar );
		void Clear();
		void StateGet();
		void StateSet( bool bPresetWP ) const;

		InternalDockStateBar * AppendFloatingLayout(
			CExtMiniDockFrameWnd * pMiniFrame
			);
		InternalDockStateBar * AppendDockBarAtSideLayout(
			CExtDockBar * pDockBar
			);
	}; // class InternalDockStateSite

protected:
	class __PROF_UIS_API InternalFriendlyFrameWnd : public CFrameWnd
	{
		void SetOuterDockState(
			const CDockState & state,
			const OuterPropsArr_t & arrOuterProps,
			bool bSerializeFixedBarsState,
			bool bSerializeResizableBarsState
			);
		void GetOuterDockState(
			CDockState & state,
			OuterPropsArr_t & arrOuterProps
			) const;
	public:
		bool SetupSmilyFrameWnd(void);

		void SetDockState(
			const InternalDockStateSite & state,
			bool bPresetWP
			);
		void GetDockState(
			InternalDockStateSite & state
			) const;

		bool IsRecalcLayoutDelayed()
		{
			return
				( (m_nIdleFlags & CFrameWnd::idleLayout) != NULL )
					? true : false;
		}
		bool IsInRecalcLayout()
		{
			return m_bInRecalcLayout ? true : false;
		}
		BOOL ProcessHelpMsg( MSG & msg, DWORD * pContext )
		{
			return CFrameWnd::ProcessHelpMsg( msg, pContext );
		}
		HWND SetHelpCapture( POINT point, BOOL * pbDescendant )
		{
			return CFrameWnd::SetHelpCapture( point, pbDescendant );
		}

		friend class InternalDockStateSite;
		friend class CExtControlBar;
	}; // class InternalFriendlyFrameWnd

	struct __PROF_UIS_API InternalBarRepositionData_t
	{
		CControlBar * m_pBar;
		CRect m_rcSrc, m_rcDst;
		INT	m_nSpaceAvailByBar,
			m_nSpaceAvailBySideBefore,
			m_nSpaceAvailBySideAfter;
		InternalBarRepositionData_t(
			CControlBar * pBar = NULL,
			bool bSetAdvData = false
			);
		InternalBarRepositionData_t(
			const InternalBarRepositionData_t & other
			);
		void _AssignFromOther(
			const InternalBarRepositionData_t & other
			);
		void SetBar(
			CControlBar * pBar,
			bool bSetAdvData = false
			);
		InternalBarRepositionData_t & operator = (
			const InternalBarRepositionData_t & other
			)
		{
			_AssignFromOther( other );
			return *this;
		}
		bool IsRectsEqual() const
		{
			return (m_rcSrc == m_rcDst) ? true : false;
		}
		CExtControlBar * GetExtBar();
		CExtControlBar * GetExtBarFixed();
	}; // struct InternalBarRepositionData_t

// Construction
public:

	struct __PROF_UIS_API QUERY_REPOSITION_CALC_EFFECT_DATA
	{
		enum // Prof-UIS reposition effect flags
		{
			__PRCF_WND_REPOS_DEFAULT	= 0x0000,
			__PRCF_WND_REPOS_QUERY		= 0x0001,
			__PRCF_WND_REPOS_EXTRA		= 0x0002,
			__PRCF_WND_REPOS_MASK		= 0x000F,
			__PRCF_EXCLUDE_FROM_CENTER	= 0x0010,
		};
		SHORT m_nFlags; 
		CWnd * m_pWndToReposChilds;
		UINT m_nIDFirst;
		UINT m_nIDLast;
		UINT m_nIDLeftOver;
		LPRECT m_lpRectParam;
		LPCRECT m_lpcRectClient;
		bool m_bStretch:1;
		QUERY_REPOSITION_CALC_EFFECT_DATA(
			SHORT nFlags,
			CWnd * pWndToReposChilds,
			UINT nIDFirst,
			UINT nIDLast,
			UINT nIDLeftOver,
			LPRECT lpRectParam,
			LPCRECT lpcRectClient,
			bool bStretch
			);
		~QUERY_REPOSITION_CALC_EFFECT_DATA();
		operator WPARAM() const;
		void QueryTypeSet( SHORT nQueryType );
		SHORT QueryTypeGet() const;
		bool IsQueryReposDefault() const;
		bool IsQueryReposQuery() const;
		bool IsQueryReposExtra() const;
		void ExcludeFromCenterSet(
			bool bExclude = true
			);
		bool ExcludeFromCenterGet() const;
	}; // struct QUERY_REPOSITION_CALC_EFFECT_DATA

	struct __PROF_UIS_API POPUP_MENU_EVENT_DATA
	{
		enum // Prof-UIS notification types
		{
			__PMED_DOCKBAR_CTX						=  0, // dockbar context menu
			__PMED_CONTROLBAR_CTX					=  1, // any control bar context menu (client area)
			__PMED_CONTROLBAR_NC_CTX				=  2, // any control bar context menu (non-client area)
			__PMED_STATUSBAR_CTX					=  3, // statusbar context menu
			__PMED_AUTOHIDESLIDER_CTX				=  4, // autohide slider window context menu
			__PMED_MINIFRAME_NC_CTX					=  5, // miniframe context menu (non-client area)
			__PMED_MDICLIAREA_CTX					=  6, // MDI client area context menu
			__PMED_MDITABS_CTX						=  7, // MDI-tabs window
			__PMED_AUTOHIDETABS_CTX					=  8, // autohide-tabs window
			__PMED_DYNCBCTABS_CTX					=  9, // dynamic control bar container tabs window
			__PMED_CONTROLBAR_NCBTNMENU_TOP			= 10, // control bar nc-area-menu-button - top level
			__PMED_CONTROLBAR_NCBTNMENU_BARS		= 11, // control bar nc-area-menu-button - control bars list 
			__PMED_CTXEXPBTN_TOP					= 12, // content expand button - top level
			__PMED_CTXEXPBTN_APPEND					= 13, // content expand button - append to buttons list
			__PMED_CTXEXPBTN_BARS					= 14, // content expand button - control bars list
			__PMED_CONTROLBAR_NCBTNMENU_DYNSTATE	= 15, // dynamic resizable control bar nc-area-menu-button
			__PMED_TAB_PAGE_CONTAINER_TABS_CTX		= 16, // page-container-as-main-view-tabs window
		};

		SHORT m_nHelperNotificationType; // notification type
		CExtPopupMenuWnd * m_pPopupMenuWnd; // popup menu to initialize
		CWnd * m_pWndEventSrc; // any control bar or mdi-tabs or ahtohide-tabs
		UINT m_nMsgID; // message handler from where notification sent
		CObject * m_pObjectSrc; // NULL or CExtBarNcAreaButton
		CWnd * m_pWndInitialTarget; // window to notify first
		bool m_bPostNotification:1; // is it post notifictaion?
		CPoint m_ptScreen; // cursor position in screen coords
		
		POPUP_MENU_EVENT_DATA(
			SHORT nHelperNotificationType,
			CExtPopupMenuWnd * pPopupMenuWnd,
			CWnd * pWndEventSrc,
			const POINT & ptScreen,
			UINT nMsgID = WM_NULL,
			CObject * pObjectSrc = NULL
			);
		virtual ~POPUP_MENU_EVENT_DATA();

		virtual bool IsControlBarMenuListNotification() const;

		operator WPARAM() const;
		static POPUP_MENU_EVENT_DATA * FromWParam( WPARAM wParam );
		static POPUP_MENU_EVENT_DATA * FromMSG( MSG * pMSG );
		
		virtual bool NotifyTarget(
			bool bPostNotification
			);
	}; // struct POPUP_MENU_EVENT_DATA

	DECLARE_DYNCREATE( CExtControlBar );

	CExtControlBar();

	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);
	virtual bool OnConstructDockSiteControlBarPopupMenu(
		CExtPopupMenuWnd * pPopup
		) const;

// Attributes
public:
	bool m_bGripperStaticallyAtTop:1;
	//bool m_bAllowContractExpand:1; // (- v.2.24)
	bool m_bDoNotEraseClientBackground:1;
	bool m_bPresubclassDialogMode:1;
	bool m_bReflectParentSizing:1;
	bool m_bInCustomModeUpdateCmdUI:1;
	bool m_bNoForcedRecalcMetrics:1;
	bool m_bAppearInDockSiteControlBarPopupMenu:1;
	static const UINT g_nMsgQueryFocusChangingEnabled;
	static const UINT g_nMsgQueryRepositionCalcEffect;
	static const UINT g_nMsgQueryIcon;
	static const UINT g_nMsgPutToPopupMenu;
	static const UINT g_nMsgConstructPopupMenu;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	static int g_nTabDrarDropSelectionDelay;
	static const UINT g_nMsgCreateTabbedBar;
	static bool g_bDisableAutoHideAnimationExpanding;
	static bool g_bDisableAutoHideAnimationCollapsing;
	static INT g_nAutoHideAnimationStepCount;
	static INT g_nAutoHideAnimationStepTime;
	static const int g_nTabShapeMaxHeight;
	static const int g_nTabShapeBottomLeftAreaWidth;
	static const int g_nTabShapeBottomMiddleAreaWidth;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	static bool g_bEnableOnIdleCalls;

	struct __PROF_UIS_API QueryFocusChangingEnabled_t
	{
		bool m_bFocusChangingEnabled;
		CWnd * m_pWndQueryOwner;
		HWND m_hWndNewFocusOwner;
		QueryFocusChangingEnabled_t(
			CWnd * pWndQueryOwner,
			HWND hWndNewFocusOwner
			);
		operator WPARAM()
		{
			return
				reinterpret_cast < WPARAM > ( this );
		}
	};
	static bool stat_QueryFocusChangingEnabled(
		HWND hWndNotifyTarget,
		CWnd * pWndQueryOwner,
		HWND hWndNewFocusOwner
		);
	static bool stat_QueryFocusChangingEnabled(
		CExtControlBar * pWndQueryOwner,
		HWND hWndNewFocusOwner
		);
	static bool stat_DragDetect_ImplStep(
		HWND hWnd,
		POINT pt,
		UINT nUpMessage = WM_LBUTTONUP
		);
	static bool stat_DoDragDetect(
		HWND hWnd,
		const POINT & ptWndClient,
		UINT nUpMessage = WM_LBUTTONUP
		);

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	static bool g_bUseMouseClickOnlyInAutoHideTabs; // (+ v.2.82) 
	static bool g_bUseMouseClickOnOffStyleAutoHideTabs; // (+ v.2.83) 
	static bool g_bAllowAutohideTabsOleDropTarget; // (+ v.2.23)
	static bool g_bAllowDynamicTabsOleDropTarget; // (+ v.2.23)
	static bool g_bAllowKeyboardTabSwitch; // (+ v.2.24)
	static bool g_bAllowKeyboardAutohideSwitch; // (+ v.2.24)
	static bool g_bTabsAtTop; // (+ v.2.43)
	static DWORD g_dwTabContainerTabsStyle;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	enum eResizablePanelDockingType_t
	{
		__RESIZABLE_DOCKING_TYPE_BY_THEME				= -1,
		__RESIZABLE_DOCKING_TYPE_STUDIO_2003			=  0,
		__RESIZABLE_DOCKING_TYPE_STUDIO_2005			=  1,
		__RESIZABLE_DOCKING_TYPE_STUDIO_2008_XP			=  2,
		__RESIZABLE_DOCKING_TYPE_STUDIO_2008_VISTA		=  3,
		__RESIZABLE_DOCKING_TYPE_STUDIO_2008_AUTO		=  4,
		__RESIZABLE_DOCKING_TYPE_STUDIO_2010			=  5,
		__RESIZABLE_DOCKING_TYPE_MIN_VALUE				=  0,
		__RESIZABLE_DOCKING_TYPE_MAX_VALUE				=  5,
	};
	static eResizablePanelDockingType_t g_eResizablePanelDockingType;
	eResizablePanelDockingType_t OnQueryDockingType() const;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	enum eAutoHideTabsType_t
	{
		__AUTOHIDE_TABS_TYPE_BY_THEME = -1,
		__AUTOHIDE_TABS_TYPE_STUDIO_2003 = 0,
		__AUTOHIDE_TABS_TYPE_STUDIO_2005 = 1,
		__AUTOHIDE_TABS_TYPE_MIN_VALUE = 0,
		__AUTOHIDE_TABS_TYPE_MAX_VALUE = 1,
	};
	static eAutoHideTabsType_t g_eAutoHideTabsType;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

protected:
	bool m_bDelelayRepaintNcButtons:1;
	bool m_bFixedMode:1;
	INT m_nGripHeightAtTop, m_nGripWidthAtLeft;
	INT m_nSeparatorHeight, m_nSeparatorWidth;
	INT m_nMinHW, m_nMinVH;
	INT m_nTrackerOffset, m_nMetricOffset;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	INT m_nAutoHideWidth, m_nAutoHideHeight;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
public:
	virtual INT  _GripperWidthAtLeftGet() const;
	virtual void _GripperWidthAtLeftSet( INT nGripWidthAtLeft );
	virtual INT  _GripperHeightAtTopGet() const;
	virtual void _GripperHeightAtTopSet( INT nGripHeightAtTop );
	virtual INT  _SeparatorWidthGet() const;
	virtual void _SeparatorWidthSet( INT nSeparatorWidth );
	virtual INT  _SeparatorHeightGet() const;
	virtual void _SeparatorHeightSet( INT nSeparatorHeight );
	virtual bool _IsCustomNcArea() const;
	static bool g_bUseCustomNcArea;
	static const UINT g_nMsgCustomNcAreaQuery;
	struct __PROF_UIS_API CUSTOM_NC_AREA_QUERY_DATA
	{
		bool m_bQueryHandled:1;
		HDC m_hDcDraw;
		CExtControlBar * m_pBar;
		CUSTOM_NC_AREA_QUERY_DATA(
			CExtControlBar * pBar,
			HDC hDcDraw = NULL
			);
		bool Notify();
		operator WPARAM() const;
		static CUSTOM_NC_AREA_QUERY_DATA * FromWPARAM( WPARAM wParam );
	}; // struct CUSTOM_NC_AREA_QUERY_DATA

public:
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	virtual INT AutoHideWidthGet() const;
	virtual void AutoHideWidthSet( INT nAutoHideWidth );
	virtual INT AutoHideHeightGet() const;
	virtual void AutoHideHeightSet( INT nAutoHideHeight );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

protected:
//	static CCriticalSection g_csCB;
	static ExtControlBarVector_t g_AllBars;
	// sizes in docked/floated state
	CSize m_sizeDockedH, m_sizeDockedV, m_sizeFloated;
	CRect m_rcGrip, m_rcRowResize, m_rcRowRecalc, m_rcRowRecalcUp;
	// flags
	bool m_bRowResizing:1;
	bool m_bRowRecalcing:1;
	bool m_bDragging:1;
	bool m_bDragDetecting:1;
	bool m_bReposSingleChildMode:1;
public:
	virtual const CRect & _RectGripGet() const;
	virtual void _RectGripSet( const RECT & rcGrip );
	virtual const CRect & _RectRowResizeGet() const;
	virtual void _RectRowResizeSet( const RECT & rcRowResize );
	virtual const CRect & _RectRowRecalcGet() const;
	virtual void _RectRowRecalcSet( const RECT & rcRowRecalc );
	virtual const CRect & _RectRowRecalcUpGet() const;
	virtual void _RectRowRecalcUpSet( const RECT & rcRowRecalcUp );
protected:
	bool m_bUpdatingChain:1;
	bool m_bSuspendTips; // not a bit field (required for referencing)
	LONG m_nDelayedRowUpdateMetric;
	// floating/dragging helpers
	CPoint m_ptFloatHelper;
	CPoint m_ptHelperDragOffsetFloated;
	CPoint m_ptHelperDragOffsetDockedH;
	CPoint m_ptHelperDragOffsetDockedV;
	bool m_bWindowActive:1;
	bool m_bTopRecalcing:1;
	bool m_bNcAreaBtnTracking:1;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
public:
	bool m_bAutoHideDeactivateOnEsc:1;
protected:
	bool m_bAutoHideMode:1;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	CPoint m_ptDraggingLast;
public:
	virtual bool IsBarWindowActive() const;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtControlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bHelperSuppressDestruction:1;
public:
	virtual BOOL DestroyWindow();
	virtual ~CExtControlBar();

	void SetInitDesiredSizeVertical(CSize size_data)
	{
		m_sizeDockedV = size_data;
	}
	void SetInitDesiredSizeHorizontal(CSize size_data)
	{
		m_sizeDockedH = size_data;
	}
	void SetInitDesiredSizeFloating(CSize size_data)
	{
		m_sizeFloated = size_data;
	}
	void SetInitDesiredPosFloating(CPoint ptFloat)
	{
		m_ptFloatHelper = ptFloat;
	}
	CSize GetSizeDockedVertical() const
	{
		return m_sizeDockedV;
	}
	CSize GetSizeDockedHorizontal() const
	{
		return m_sizeDockedH;
	}
	CSize GetSizeFloating() const
	{
		return m_sizeFloated;
	}
	CPoint GetPosFloating() const
	{
		return m_ptFloatHelper;
	}

	static bool ProfileBarStateLoad(
		CFrameWnd * pFrame,
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
		LPWINDOWPLACEMENT pFrameWp = NULL, // need serialize frame's WP
		bool bSerializeFixedBarsState = true,
		bool bSerializeResizableBarsState = true,
		HKEY hKeyRoot = HKEY_CURRENT_USER,
		bool bEnableThrowExceptions = false
		);
	static bool ProfileBarStateSave(
		CFrameWnd * pFrame,
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%\%sSectionNameProfile%
		LPWINDOWPLACEMENT pFrameWp = NULL, // need serialize frame's WP
		bool bSerializeFixedBarsState = true,
		bool bSerializeResizableBarsState = true,
		HKEY hKeyRoot = HKEY_CURRENT_USER,
		bool bEnableThrowExceptions = false
		);
public:
	virtual void OnExtBarStateSerializationComplete(
		bool bStore
		);
protected:
	static CTypedPtrMap < CMapPtrToWord, CFrameWnd *, WORD > g_mapSF;
	static bool ProfileBarStateFramePush(
		CFrameWnd * pFrame,
		bool bIsLoading
		);
	static bool ProfileBarStateFramePop(
		CFrameWnd * pFrame
		);

public:
	static bool ProfileBarStateIsSerializing(
		CFrameWnd * pFrame,
		bool * p_bIsLoading = NULL
		);
	static bool ProfileBarStateSerialize(
		CArchive & ar,
		CFrameWnd * pFrame,
		LPWINDOWPLACEMENT pFrameWp = NULL, // need serialize frame's WP
		bool bSerializeFixedBarsState = true,
		bool bSerializeResizableBarsState = true,
		bool bEnableThrowExceptions = false
		);

    virtual BOOL Create(
		__EXT_MFC_SAFE_LPCTSTR lpszWindowName,
		CWnd * pParentWnd,
        UINT nID = AFX_IDW_DIALOGBAR,
		DWORD dwStyle =
			WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS
			|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS
			|CBRS_FLYBY|CBRS_SIZE_DYNAMIC
			|CBRS_HIDE_INPLACE
		);

	static bool FindPrintPreviewMode(
		CFrameWnd * pFrame
		);
	virtual bool IsDockSiteInPrintPreviewMode();
	static bool IsOleIpObjActive(
		CFrameWnd * pFrameSearch
		);
	bool IsOleIpObjActive() const;
	virtual bool SafeDisplayBar();
	static bool FindHelpMode(
		CWnd * pWndStartSearch
		);

	static bool stat_FindSingleVisbleBarNested(
		CDockBar * pDockBar,
		CExtControlBar ** ppFoundBar
		);

protected:
		INT m_nFlashCaptionCounter, m_nFlashCaptionStepMilliseconds, m_nFlashCaptionTimerID;
public:
		COLORREF m_clrFlashCaptionText, m_clrFlashCaptionBackground;
protected:
		bool m_bFlashCaptionHighlightedState:1;
public:
	bool FlashCaption_DoPaintNC(
		CDC & dc,
		LPVOID pPGD
		);
	virtual void FlashCaption_OnRepaint();
	virtual void FlashCaption_OnTimer();
	virtual bool FlashCaptionIsInProgress(
		bool * p_bFlashCaptionHighlightedState = NULL
		) const;
	virtual bool FlashCaption(
		INT nFlashCount = -1, // negative value cancels current flashing
		INT nStepMilliseconds = 250,
		COLORREF clrFlashText = RGB(255,255,255),
		COLORREF clrFlashBackground = RGB(255,0,0)
		);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual void OnCustomizeModeEnter();
	virtual void OnCustomizeModeLeave();
	bool m_bVisibleInCustomizeListBox:1;
	virtual void OnCustomizeListBoxUpdate( CExtCustomizeToolbarsListBox * pWndList );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

protected:
	static bool _FrameEnableDockingImpl(
		CFrameWnd * pFrame,
		DWORD dwDockStyle = CBRS_ALIGN_ANY,
		bool bReplaceFloatingWnd = true
		);
	static bool _SetupFloatingFrameWndImpl(
		CFrameWnd * pFrame
		);
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	static bool _FrameInjectAutoHideAreasImpl(
		CFrameWnd * pFrame
		);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
public:
	static bool FrameEnableDocking(
		CFrameWnd * pFrame,
		DWORD dwDockStyle = CBRS_ALIGN_ANY,
		bool bReplaceFloatingWnd = true
		);
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	static bool FrameInjectAutoHideAreas(
		CFrameWnd * pFrame
		);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	bool IsSingleOnRow(
		bool bOnlyFixed = false,
		bool bOnlyNonFixed = false
		) const;
	void MakeSingleOnRow();

	bool IsMinimizedOnRow() const;
	bool IsMaximizedOnRow() const;
	void MaximizeOnRow();
	void MinimizeOnRow();

	virtual bool IsFixedMode() const;
	virtual bool IsFixedDockStyle() const;

	UINT GetSafeDockBarDlgCtrlID() const;
	bool IsDockedAtTop() const;
	bool IsDockedAtBottom() const;
	bool IsDockedAtLeft() const;
	bool IsDockedAtRight() const;
	bool IsDockedHorizontally() const;
	bool IsDockedVertically() const;
	bool IsDocked() const;
	bool IsFloating() const;
	
	virtual bool IsBarWithGripper(
		bool * pbGripperAtTop = NULL,
		bool * pbTextOnGripper = NULL
		) const;
	virtual void OnShowControlBarChain(
		bool bShow,
		bool bDelay,
		bool bOnlyOneBar = false
		);
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	static bool g_bCloseOnlyOneBarInTabGroup;
	virtual bool IsCloseOnlyOneBarInTabGroup() const;
	virtual int OnReflectTabOleDragDrop(
		COleDropTarget * pOleDropTarget
		);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	
	virtual CSize CalcFixedLayout(
		BOOL bStretch,
		BOOL bHorz
		);
	virtual CSize CalcDynamicLayout(
		int nLength,
		DWORD nMode
		);

	virtual void DoPaintNC( CDC * pDC );
	virtual void DoPaint( CDC * pDC );
	virtual void DoEraseBk( CDC * pDC );

	bool m_bUseInvisibleCmdUI:1;
	virtual void OnUpdateCmdUI(
		CFrameWnd * pTarget,
		BOOL bDisableIfNoHndler
		);
	LRESULT OnIdleUpdateCmdUI( WPARAM, LPARAM );
	void OnInitialUpdate();

	static BOOL DoCustomModePreTranslateMessage(
		CWnd * pWndPretranslateSrc,
		MSG * pMsg
		);
	static void DoCustomModeUpdateControlBars(
		CWnd * pWndParentTarget,
		BOOL bDisableIfNoHndler = TRUE
		);
	bool DoCustomModeUpdateCmdUI(
		BOOL bDisableIfNoHndler = TRUE
		);

protected:

	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;
	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet(
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0
		) const;
	virtual void OnAdvancedPopupMenuTipWndDisplay(
		CExtPopupMenuTipWnd & _ATTW,
		const RECT & rcExcludeArea,
		__EXT_MFC_SAFE_LPCTSTR strTipText,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0
		) const;
	
public:
	static bool g_bUseAdvancedToolTips;
	virtual bool _GetFullRowMode() const;
	virtual INT _CalcDesiredMinHW() const;
	virtual INT _CalcDesiredMinVH() const;
	virtual CSize _CalcDesiredMinFloatedSize() const;
	virtual CSize _CalcDesiredMinOuterSize( BOOL bHorz ) const;
	virtual CExtControlBar::InternalAffixmentData *
		_AffixmentGetOuter();
	virtual void _AffixmentSetOuter(
		CExtControlBar::InternalAffixmentData * pAffixmentData
		);
	static CExtControlBar::InternalAffixmentData *
		_AffixmentGetOuter(
			CControlBar * pBar
			);
	static int _AffixmentCompareOuter(
		CControlBar * pBar1,
		CControlBar * pBar2
		);
	virtual void _AffixmentRecalcOuter();
	virtual void _AffixmentSafeClearOuter();
	virtual bool _AffixmentIsAlignedHW() const;

protected:
	
	class FriendlyDockBarHack;

	CSize _CalcDesiredLayout(
		BOOL bHorz,
		BOOL bForceFullRow = FALSE
		);

	enum eGFECB_t
	{
		// filters by control bar window properties
		__GFECB_FIXED				= 0x00000001L, // fixed sized
		__GFECB_NONFIXED_MIN		= 0x00000002L, // resizable and minimized on row
		__GFECB_NONFIXED_NONMIN		= 0x00000004L, // resizable and not minimized on row
		__GFECB_NONFIXED			= (__GFECB_NONFIXED_MIN|__GFECB_NONFIXED_NONMIN),

		__GFECB_DOCKED_TOP			= 0x00000008L,
		__GFECB_DOCKED_BOTTOM		= 0x00000010L,
		__GFECB_DOCKED_LEFT			= 0x00000020L,
		__GFECB_DOCKED_RIGHT		= 0x00000040L,

		__GFECB_DOCKED_HORZ			= (__GFECB_DOCKED_TOP|__GFECB_DOCKED_BOTTOM),
		__GFECB_DOCKED_VERT			= (__GFECB_DOCKED_LEFT|__GFECB_DOCKED_RIGHT),
		__GFECB_DOCKED				= (__GFECB_DOCKED_HORZ|__GFECB_DOCKED_VERT),
		
		__GFECB_FLOATED				= 0x00000080L,
		
		__GFECB_BAR_VISIBLE			= 0x00000100L,
		__GFECB_BAR_INVISIBLE		= 0x00000200L,
		__GFECB_WND_VISIBLE			= 0x00000400L,
		__GFECB_WND_INVISIBLE		= 0x00000800L,
		
		__GFECB_VIS_BAR_ALL			= (__GFECB_BAR_VISIBLE|__GFECB_BAR_INVISIBLE),
		__GFECB_VIS_WND_ALL			= (__GFECB_WND_VISIBLE|__GFECB_WND_INVISIBLE),
		
		__GFECB_VIS_ALL				= (__GFECB_BAR_VISIBLE|__GFECB_WND_VISIBLE),
		__GFECB_INVIS_ALL			= (__GFECB_BAR_INVISIBLE|__GFECB_WND_INVISIBLE),
//		__GFECB_VIS_ALL				= (__GFECB_VIS_BAR_ALL|__GFECB_VIS_WND_ALL),
		
		__GFECB_ALL_NONRESTRICT		= (__GFECB_FIXED|__GFECB_NONFIXED|__GFECB_DOCKED|__GFECB_FLOATED|__GFECB_VIS_ALL|__GFECB_INVIS_ALL),

		// restriction by point
		__GFECB_BYPT_ONLY_WND		= 0x00008000L, // abondones all other __GFECB_BYPT_ flags
		__GFECB_BYPT_CLIENT			= 0x00010000L,
		__GFECB_BYPT_NCGRIP			= 0x00020000L,

		__GFECB_BYPT_HTLEFT			= 0x00040000L,
		__GFECB_BYPT_HTRIGHT		= 0x00080000L,
		__GFECB_BYPT_HTTOP			= 0x00100000L,
		__GFECB_BYPT_HTBOTTOM		= 0x00200000L,
		__GFECB_BYPT_HTTOPLEFT		= 0x00400000L,
		__GFECB_BYPT_HTTOPRIGHT		= 0x00800000L,
		__GFECB_BYPT_HTBOTTOMLEFT	= 0x01000000L,
		__GFECB_BYPT_HTBOTTOMRIGHT	= 0x02000000L,

		__GFECB_BYPT_HT_CORNER		= (__GFECB_BYPT_HTLEFT|__GFECB_BYPT_HTRIGHT|__GFECB_BYPT_HTTOP|__GFECB_BYPT_HTBOTTOM),
		__GFECB_BYPT_HT_NONCORNER	= (__GFECB_BYPT_HTTOPLEFT|__GFECB_BYPT_HTTOPRIGHT|__GFECB_BYPT_HTBOTTOMLEFT|__GFECB_BYPT_HTBOTTOMRIGHT),

		__GFECB_BYPT_HTNOWHERE		= 0x04000000L,
		__GFECB_BYPT_HTSYSMENU		= 0x06000000L,

		__GFECB_BYPT_HTGROWBOX		= 0x10000000L,
		__GFECB_BYPT_HTMINBUTTON	= 0x20000000L,
		__GFECB_BYPT_HTMAXBUTTON	= 0x40000000L,
		__GFECB_BYPT_HT_BUTTONS		= (__GFECB_BYPT_HTGROWBOX|__GFECB_BYPT_HTMINBUTTON|__GFECB_BYPT_HTMAXBUTTON),

		__GFECB_BYPT_HT_BORDER		= (__GFECB_BYPT_HT_CORNER|__GFECB_BYPT_HT_NONCORNER),
		__GFECB_BYPT_HT_NONBORDER	= (__GFECB_BYPT_HT_BUTTONS|__GFECB_BYPT_HTNOWHERE|__GFECB_BYPT_HTSYSMENU),
		__GFECB_BYPT_HTNONCLIENT	= (__GFECB_BYPT_HT_BORDER|__GFECB_BYPT_HT_NONBORDER),

		__GFECB_BYPT_HTCLIENT		= 0x80000000L,

		__GFECB_BYPT_NC				= (__GFECB_BYPT_NCGRIP|__GFECB_BYPT_HTNONCLIENT|__GFECB_BYPT_HTCLIENT),
		__GFECB_BYPT_ALL			= (__GFECB_BYPT_ONLY_WND|__GFECB_BYPT_NC|__GFECB_BYPT_CLIENT),
	}; // enum eGFECB_t
    
    static void _GetFrameControlBars(
		CControlBar * pBarExcludeSubTree,
		CFrameWnd * pFrame,
		ExtControlBarVector_t & vBars,
		DWORD dwCallOpt = ((DWORD)__GFECB_ALL_NONRESTRICT),
		CPoint ptRestrict = CPoint( -1000, -1000 )
		);
    void _GetFrameControlBars(
		bool bExcludeOwnSubtree,
		ExtControlBarVector_t & vBars,
		DWORD dwCallOpt = ((DWORD)__GFECB_ALL_NONRESTRICT),
		CPoint ptRestrict = CPoint( -1000, -1000 )
		) const;

	static CControlBar * _GetNextControlBarInRow(
		CControlBar * pBar,
		bool bNext = true, // next or previous
		bool bExtBar = false,
		bool bNonFixed = false
		);
	static CControlBar * _GetFirstControlBarInRow(
		CControlBar * pBar,
		bool bNext = false, // last or first
		bool bExtBar = false,
		bool bNonFixed = false
		);

    void _GetRowBars(
		MfcControlBarVector_t & vBars,
		CRect * pRcAllBars = NULL,
		INT * p_nOwnPos = NULL,
		bool * p_bRowMaxSized = NULL, // row with resizable bars?
		bool * p_bRowFullSized = NULL // row with menu-like bar?
		) const;
	void _GetRowExtBars(
		ExtControlBarVector_t & vBars,
		bool bNonFixedOnly = true,
		CRect * pRcAllBars = NULL,
		INT * p_nOwnPos = NULL,
		bool * p_bRowMaxSized = NULL, // row with resizable bars?
		bool * p_bRowFullSized = NULL // row with menu-like bar?
		) const;
    void _GetNextRowBars(
		bool bNextRow,
		MfcControlBarVector_t & vBars,
		CRect * pRcAllBars = NULL,
		INT * p_nOwnPos = NULL,
		bool * p_bRowMaxSized = NULL, // row with resizable bars?
		bool * p_bRowFullSized = NULL // row with menu-like bar?
		) const;

	bool _IsFirstBarInRow( bool bExtBar ) const;
	bool _IsLastBarInRow( bool bExtBar ) const;

    static CFrameWnd * _GetDockingFrameImpl(
		CControlBar * pBar
		);
    CFrameWnd * _GetDockingFrameImpl() const;

	virtual void _RecalcLayoutImpl();
	virtual void _RecalcNcArea();

protected:
	virtual bool _IsShowContentWhenDragging() const;
	virtual bool _IsShowContentWhenRowResizing() const;
	virtual bool _IsShowContentWhenRowRecalcing() const;
public:
	virtual void OnRepositionSingleChild(
		int cx = -1, // if negative - get from client area
		int cy = -1,
		bool bRedraw = true
		);

public:
	static CRect g_rcHelperRowResizingLimits;
	virtual bool _RowResizingIsEnabled(
		const CExtControlBar * pBarQuerySrc
		) const;
	virtual bool _RowResizingGet() const;
	virtual void _RowResizingStart();
protected:
	virtual void _RowResizingStop( bool bCancel );
	virtual void _RowResizingUpdateState();

public:
	virtual bool _RowRecalcingIsEnabled(
		const CExtControlBar * pBarQuerySrc
		) const;
	virtual bool _RowRecalcingGet() const;
	virtual void _RowRecalcingStart();
protected:
	virtual void _RowRecalcingStop( bool bCancel );
	virtual void _RowRecalcingUpdateState();
	
	virtual bool _RowRecalcing_IsBottomEnabled();
	virtual bool _RowRecalcing_IsTopEnabled();
	
	virtual void _DrawResizingTracker( bool bErasingPrevious );
	virtual void _DrawRecalcingTracker( bool bErasingPrevious );
public:
	static const UINT g_nMsgDrawTrackedArea;
	enum eDrawTrackedAreaType_t
	{
		__EDTAT_RESIZING_ENTIRE_ROW		= 0,
		__EDTAT_RESIZING_PART_OF_ROW	= 1,
		__EDTAT_RECTANGULAR_FRAME		= 2,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		__EDTAT_TAB_FRAME				= 3,
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	}; // enum eDrawTrackedAreaType_t
	struct __PROF_UIS_API MsgDrawTrackedAreaData_t
	{
		CExtControlBar & m_wndBar;
		CDC & m_dcDraw;
		eDrawTrackedAreaType_t m_eDTAT;
		CRect m_rcDrawArea;
		bool m_bErasingPrevious:1;
		bool m_bMessageHandled:1;
		MsgDrawTrackedAreaData_t(
			CExtControlBar & wndBar,
			CDC & dcDraw,
			eDrawTrackedAreaType_t eDTAT,
			const RECT & rcDrawArea,
			bool bErasingPrevious
			)
			: m_wndBar( wndBar )
			, m_dcDraw( dcDraw )
			, m_eDTAT( eDTAT )
			, m_rcDrawArea( rcDrawArea )
			, m_bErasingPrevious( bErasingPrevious )
			, m_bMessageHandled( false )
		{
			ASSERT_VALID( (&m_wndBar) );
			ASSERT( m_wndBar.GetSafeHwnd() != NULL );
			ASSERT( m_dcDraw.GetSafeHdc() != NULL );
		}
		void NotifyOrDrawDefault();
		bool Notify();
		void DrawDefault();
		void Draw(
			HBRUSH hBrush,
			DWORD dwPatBltROP // raster operation code for invoking PatBlt()
			);
	}; // struct MsgDrawTrackedAreaData_t

public:
	enum e_calc_dock_type_t
	{
		__ECDT_FLOATED				=  0,
		__ECDT_INNER_TOP			=  1,
		__ECDT_INNER_BOTTOM			=  2,
		__ECDT_INNER_LEFT			=  3,
		__ECDT_INNER_RIGHT			=  4,
		__ECDT_DOCKBAR_INNER_TOP	=  5,
		__ECDT_DOCKBAR_INNER_BOTTOM	=  6,
		__ECDT_DOCKBAR_INNER_LEFT	=  7,
		__ECDT_DOCKBAR_INNER_RIGHT	=  8,
		__ECDT_CIRCLE1_OUTER_TOP	=  9,
		__ECDT_CIRCLE1_OUTER_BOTTOM	= 10,
		__ECDT_CIRCLE1_OUTER_LEFT	= 11,
		__ECDT_CIRCLE1_OUTER_RIGHT	= 12,
		__ECDT_OWN_AREA				= 13,
		__ECDT_2005_OUTER_TOP		= 14,
		__ECDT_2005_OUTER_BOTTOM	= 15,
		__ECDT_2005_OUTER_LEFT		= 16,
		__ECDT_2005_OUTER_RIGHT		= 17,
		__ECDT_2005_INNER_TOP		= 18,
		__ECDT_2005_INNER_BOTTOM	= 19,
		__ECDT_2005_INNER_LEFT		= 20,
		__ECDT_2005_INNER_RIGHT		= 21,
		__ECDT_2005_BAR_TOP			= 22,
		__ECDT_2005_BAR_BOTTOM		= 23,
		__ECDT_2005_BAR_LEFT		= 24,
		__ECDT_2005_BAR_RIGHT		= 25,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		__ECDT_2005_BAR_NEW_TAB		= 26,
		__ECDT_2005_MAX_VALUE		= 26,
		__ECDT_TABBED_NEW			= 27,
		__ECDT_TABBED_UPDATE		= 28,
		__ECDT_TABBED_REINDEX		= 29,
		__ECDT_TABBED_REINDEX_NHM	= 30, // the "no hide 2005 markers" version of the __ECDT_TABBED_REINDEX mode
#else // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		__ECDT_2005_MAX_VALUE		= 25,
#endif // else from (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		__ECDT_2005_MIN_VALUE		= 14
	};
protected:

	enum e_calc_fixed_resizing_t
	{
		__ECFR_NO_RESIZING = 0,
		__ECFR_HORZ = 1,
		__ECFR_VERT = 2,
	};

	class __PROF_UIS_API InternalDraggingState_t : public CObject
	{
		bool m_bEmpty:1, m_bDeepHalfSplit:1;
		e_calc_dock_type_t m_eCDT;
		e_calc_fixed_resizing_t m_eCFR;
		UINT m_nCircleNo;
		CRect m_rcTrack;
		CExtControlBar * m_pExtBarSrc;
		CExtControlBar * m_pExtBarDst;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		LONG m_nTabReindex;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		void _AssignFromOther(
			const InternalDraggingState_t & other
			);
		bool _IsEqualToOther(
			const InternalDraggingState_t & other
			) const
		{
			ASSERT_VALID( this );
			ASSERT_VALID( (&other) );
			if( m_bEmpty != other.m_bEmpty )
				return false;
			if( m_eCDT != other.m_eCDT )
				return false;
//			if( m_eCFR != other.m_eCFR )
//				return false;
			if( m_nCircleNo != other.m_nCircleNo )
				return false;
			if( m_rcTrack != other.m_rcTrack )
				return false;
			if( m_pExtBarSrc != other.m_pExtBarSrc )
				return false;
			if( m_pExtBarDst != other.m_pExtBarDst )
				return false;
			if( m_bDeepHalfSplit != other.m_bDeepHalfSplit )
				return false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			if( m_nTabReindex != other.m_nTabReindex )
				return false;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			return true;
		}
	public:
		InternalDraggingState_t(
			CExtControlBar * pExtBarSrc = NULL
			)
			: m_bEmpty( true )
			, m_bDeepHalfSplit( false )
			, m_eCDT( __ECDT_FLOATED )
			, m_eCFR( __ECFR_NO_RESIZING )
			, m_nCircleNo( 0 )
			, m_rcTrack( 0, 0, 0, 0 )
			, m_pExtBarSrc( pExtBarSrc )
			, m_pExtBarDst( NULL )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			, m_nTabReindex( 0 )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		{
		}
		InternalDraggingState_t(
			const InternalDraggingState_t & other
			)
		{
			_AssignFromOther( other );
		}

		InternalDraggingState_t & operator = (
			const InternalDraggingState_t & other
			)
		{
			_AssignFromOther( other );
			return *this;
		}
		bool operator == (
			const InternalDraggingState_t & other
			) const
		{
			return _IsEqualToOther( other );
		}
		bool operator != (
			const InternalDraggingState_t & other
			) const
		{
			return ( ! _IsEqualToOther( other ) );
		}

		e_calc_dock_type_t GetCDT() const
		{
			ASSERT_VALID( this );
			return m_eCDT;
		}
		e_calc_dock_type_t SetCDT( e_calc_dock_type_t eCDT )
		{
			//ASSERT_VALID( this );
			e_calc_dock_type_t eOldCDT = m_eCDT;
			m_eCDT = eCDT;
			return eOldCDT;
		}

		e_calc_fixed_resizing_t GetCFR() const
		{
			ASSERT_VALID( this );
			return m_eCFR;
		}
		e_calc_fixed_resizing_t SetCFR( e_calc_fixed_resizing_t eCFR )
		{
			//ASSERT_VALID( this );
			e_calc_fixed_resizing_t eOldCFR = m_eCFR;
			m_eCFR = eCFR;
			return eOldCFR;
		}

		UINT GetCircleNo() const
		{
			return m_nCircleNo;
		}
		UINT SetCircleNo( UINT nCircleNo )
		{
			UINT nCircleNoOld = m_nCircleNo;
			m_nCircleNo = nCircleNo;
			return nCircleNoOld;
		}

		CRect & TrackRectGet()
		{
			ASSERT_VALID( this );
			return m_rcTrack;
		}
		const CRect & TrackRectGet() const
		{
			ASSERT_VALID( this );
			return m_rcTrack;
		}
		void TrackRectGet( CRect & rcTrack ) const
		{
			ASSERT_VALID( this );
			rcTrack = m_rcTrack;
		}
		void TrackRectSet( const CRect & rcTrack )
		{
			ASSERT_VALID( this );
			m_rcTrack = rcTrack;
		}
		void TrackRectSetEmpty()
		{
			ASSERT_VALID( this );
			m_rcTrack.SetRectEmpty();
		}

		bool IsEmpty() const
		{
			//ASSERT_VALID( this );
			ASSERT( this != NULL );
			return m_bEmpty;
		}
		void SetEmpty(
			bool bEmpty = true
			);
		bool IsDeepHalfSplit() const
		{
			ASSERT_VALID( this );
			return m_bDeepHalfSplit;
		}
		void SetDeepHalfSplit( bool bDeepHalfSplit = true )
		{
			ASSERT_VALID( this );
			m_bDeepHalfSplit = bDeepHalfSplit;
		}

		CExtControlBar * ExtBarSrcGet()
		{
			//ASSERT_VALID( this );
			ASSERT( this != NULL );
			return m_pExtBarSrc;
		}
		CExtControlBar * ExtBarDstGet()
		{
			ASSERT_VALID( this );
			return m_pExtBarDst;
		}
		CExtControlBar * ExtBarSrcSet(
			CExtControlBar * pExtBarSrc
			)
		{
			ASSERT_VALID( this );
			ASSERT( pExtBarSrc != NULL );
			ASSERT_VALID( pExtBarSrc );
			CExtControlBar * pExtBarSrcOld = m_pExtBarSrc;
			m_pExtBarSrc = pExtBarSrc;
			ASSERT_VALID( this );
			return pExtBarSrcOld;
		}
		CExtControlBar * ExtBarDstSet(
			CExtControlBar * pExtBarDst = NULL,
			bool bNoCheckOnShutdown = false
			);

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		LONG ReindexPosGet() const
		{
			ASSERT_VALID( this );
			if(		m_eCDT != __ECDT_TABBED_REINDEX
				&&	m_eCDT != __ECDT_TABBED_REINDEX_NHM
				)
				return (-1L);
			return m_nTabReindex;
		}

		void ReindexPosCalc( CPoint ptCursor );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		void DrawState( bool bErasingPrevious );
	private:
		void _DoFixed_BasicAdjustment(
			CPoint ptTrack, // in screen coords
			bool bForceFloatMode
			);
		void _DoFixed_FloatingAdjustment(
			CPoint ptTrack // in screen coords
			);
		bool _DoFixed_OuterDockerAdjustment(
			CPoint ptTrack, // in screen coords
			CExtDockOuterBar * pOuterDocker
			);
		static void _DoFixed_StaticOuterRowSwap(
			CControlBar * pBar1,
			CControlBar * pBar2
			);
		static void _DoFixed_StaticMinimalDockSiteRecalc(
			CFrameWnd * pDockSite,
			CExtDockOuterBar * pOuterDocker
			);
	public:
		void CalcStateDynamic(
			CPoint ptTrack, // in screen coords
			bool bForceFloatMode
			);

#ifdef _DEBUG
		virtual void AssertValid() const
		{
			ASSERT( this != NULL );
			if( !m_bEmpty )
			{
				ASSERT( m_pExtBarSrc != NULL );
				ASSERT_VALID( m_pExtBarSrc );
				
				CFrameWnd * pFrameSrc =
					m_pExtBarSrc->_GetDockingFrameImpl();
				ASSERT_VALID( pFrameSrc );
				pFrameSrc;
				
				if( m_pExtBarDst != NULL )
				{
					ASSERT_VALID( m_pExtBarDst );
					
					CFrameWnd * pFrameDst =
						m_pExtBarDst->_GetDockingFrameImpl();
					ASSERT_VALID( pFrameDst );
					ASSERT( pFrameSrc == pFrameDst );
				}
			} // if( !m_bEmpty )
			else
			{
				ASSERT( m_pExtBarDst == NULL );
			} // if( !m_bEmpty )

			CObject::AssertValid();
		}
		
		virtual void Dump( CDumpContext & dc ) const
		{
			CObject::Dump( dc );
		}
#endif

	}; // class InternalDraggingState_t
	
	friend class InternalDraggingState_t;

	static InternalDraggingState_t g_DragStateOld;
	static InternalDraggingState_t g_DragStateNew;

	static bool g_bUpdatingDragState;

	virtual void _DraggingDoDeepHalfSplit(
		CExtControlBar * pExtBar,
		UINT nHalfSplitID, // one of AFX_IDW_DOCKBAR_... docked value
		int nPercentToOccupy = 50 // greater than zero and less than one hundred
		);
	virtual bool _DraggingApplyState(
		CExtControlBar::InternalDraggingState_t & _ds,
		bool bFinalApply
		);
public:
	static CExtControlBar * _DraggingGetBar(); // return currently dragging control bar
	static bool g_bLockAllBars;
	virtual bool _DraggingIsEnabled(
		const CExtControlBar * pBarQuerySrc
		) const;

	enum e_control_bar_position_changing_t
	{
		__ECBPC_EMPTY						= 0,
		__ECBPC_REDOCKING					= 1,
		__ECBPC_ROW_RESIZING				= 2,
		__ECBPC_IN_ROW_RESIZING				= 3,
		__ECBPC_FLOATING_FRAME_RESIZING		= 4,
		__ECBPC_MUTUAL_DEPENDENCY			= 5,
	};
	virtual e_control_bar_position_changing_t GetCurrentControlBarPositionChangingType() const;
	virtual void OnControlBarPositionChange(
		CExtControlBar * pBarEventSrc,
		e_control_bar_position_changing_t eCBPC,
		bool bInitial,
		bool bFinal
		);
protected:
	e_control_bar_position_changing_t m_eCBPC;
	virtual void _OnControlBarPositionChange_RedockingImpl(
		CExtControlBar * pBarEventSrc,
		e_control_bar_position_changing_t eCBPC,
		bool bInitial,
		bool bFinal
		);
	virtual void _OnControlBarPositionChange_InRowImpl(
		CExtControlBar * pBarEventSrc,
		e_control_bar_position_changing_t eCBPC,
		bool bInitial,
		bool bFinal
		);
public:
	static bool _DraggingCancel(); // returns true if dragging was really canceled
    virtual void _DraggingStart(
		const CPoint & point,
		const CPoint & pointOffset = CPoint( 0, 0 ),
		CSize sizeWaitMouseMove = CSize( 1, 1 )
		);
    virtual void _DraggingStop( bool bCancel );
    virtual void _DraggingUpdateState(
		const CPoint & point,
		bool bForceFloatMode
		);
	virtual void _Dragging_OnStart();
	virtual void _Dragging_OnStop(
		InternalDraggingState_t & _dsOld,
		InternalDraggingState_t & _dsNew,
		bool bCancel
		);

	virtual void _SetCursor(
		const CPoint & point
		);

	bool _IsSingleVisibleInFloatingPalette();

	virtual void _GetDeepNcModeMetrics(
		bool & bEnableResizerLeft,
		bool & bEnableResizerTop,
		bool & bEnableResizerRight,
		bool & bEnableResizerBottom
		);

	bool m_bDelayShowInvoked:1, m_bDelayShowFlag:1;
	virtual void DelayShow( BOOL bShow );
protected:
	virtual void _UpdateVisibilityInChain();
	virtual void _UpdateVisibilityInRow();
	virtual void _ActivateOnClick();
	virtual void _OnNcMiniFrameActivate( bool bActive );

	static CExtControlBar * g_pBarToggleFloatingState;
public:
	static CExtControlBar * _GetBarToggleFloatingState();

	static CFrameWnd * DockSiteFromWnd(
		CWnd * pWndStartSearch
		);
	static HWND FindHwndNotSafeForMutualPaint(
		CWnd * pWndStartSearch
		);

	virtual bool _IsDockSiteCustomizeMode() const;
	
	UINT GetDockBarCircleNo() const;

	virtual bool _CanDockToInnerCircles() const;
	virtual bool _CanDockToTabbedContainers(
		CExtControlBar * pDestBar
		) const;
	virtual bool _CanDockLTRB(
		CExtControlBar * pDestBar
		) const;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
public:
	virtual CExtDynTabControlBar * _GetNearestTabbedContainer();
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	virtual CExtDynControlBar * _GetNearestDynamicContainer();
protected:

	void _PreSyncRowMetrics();

	virtual void OnFrameBarCheckCmd(
		bool bResizableCheckmarks = false
		);
	virtual void OnFrameBarCheckUpdate(
		CCmdUI* pCmdUI,
		bool bResizableCheckmarks = false
		);

public:
	static bool stat_AdjustOccCompatibilityTree(
		CWnd * pWndRoot
		);
	static void stat_CompressFrameLayout(
		CFrameWnd * pDockSite,
		CSize sizeFreeSpaceAtCenterRequired = CSize(100,100),
		bool bCompressInnerFirst = true,
		bool bCompressProportionally = false,
		bool bRecalcLayout = true,
		bool bDelayRecalcLayout = true,
		UINT nCenterDlgCtrlID = AFX_IDW_PANE_FIRST
		);
	static void stat_RedrawFloatingFrames( CFrameWnd * pDockSite );
	static void stat_RecalcBarMetrics( CFrameWnd * pDockSite );
	static CWnd * stat_GetWndForPlacement( CWnd * pWnd );
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

	static bool g_bUseCmdManagerForSetMessageString;
	static void stat_SetMessageString(
		CWnd * pWnd,
		UINT nCmdID = AFX_IDS_IDLEMESSAGE
		);
	static HWND stat_FindMdiClientHWND(
		HWND hWndMdiFrame
		);

	static BOOL DoFrameBarCheckCmd(
		CFrameWnd * pFrame,
		UINT nBarID,
		bool bResizableCheckmarks = false
		);
	static void DoFrameBarCheckUpdate(
		CFrameWnd * pFrame,
		CCmdUI* pCmdUI,
		bool bResizableCheckmarks = false
		);

	virtual CExtMiniDockFrameWnd * OnCreateFloatingFrame(
		DWORD dwStyle
		);

protected:
	UINT m_nTearOffCmdID;
public:
	static CExtControlBar * FindTearOffBar(
		CFrameWnd * pDockSite,
		UINT nTearOffCmdID
		);
	virtual UINT TearOffCommandGet() const;
	virtual void TearOffCommandSet( UINT nCmdID );

	// toggle docking state
	virtual void ToggleDocking();

	// float control bar into recent floated position
	void FloatControlBar(
		DWORD dwStyle = CBRS_ALIGN_TOP
		);
	// float control bar
	virtual void FloatControlBar(
		CPoint ptFloat,
		DWORD dwStyle = CBRS_ALIGN_TOP
		);

	// dock control bar to new circle (most inner or outer)
	virtual bool DockControlBarInnerOuter(
		UINT nDockBarID,
		bool bInner,
		CFrameWnd * pDockSite = NULL, // can be NULL only if bar already was at once docked
		bool bRecalcLayout = true
		);

	// dock control bar relatively to other control bar (extended version)
	bool DockControlBarLTRB(
		CExtControlBar * pBarTarget,
		UINT nDockBarID = AFX_IDW_DOCKBAR_BOTTOM,
		bool bRecalcLayout = true
		);
	// dock control bar relatively to other control bar using
	// space percent specification (extended version)
	virtual bool DockControlBarLTRB(
		int nPercentToOccupy, // greater than zero and less than one hundred
		CExtControlBar * pBarTarget,
		UINT nDockBarID = AFX_IDW_DOCKBAR_BOTTOM,
		bool bRecalcLayout = true
		);

	// dock control bar to new row
	virtual bool DockControlBar(
		UINT nDockBarID,
		UINT nCircleNo,
		CFrameWnd * pDockSite = NULL, // can be NULL only if bar already was at once docked
		bool bRecalcLayout = true
		);

	// dock control bar relatively to other control bar
	virtual bool DockControlBar(
		CExtControlBar * pBar,
		bool bHorzBarHalfSplit,
		bool bInnerHalfSplit = false,
		CFrameWnd * pDockSite = NULL, // can be NULL only if bar already was at once docked
		bool bRecalcLayout = true
		);

	// dock control bar into tabbed container
	virtual bool DockControlBarIntoTabbedContainer(
		CExtControlBar * pBar,
		INT nIndex = -1, // append
		CFrameWnd * pDockSite = NULL, // can be NULL only if bar already was at once docked
		bool bRecalcLayout = true
		);

	virtual bool _FloatStateChangingIsEnabled(
		const CExtControlBar * pBarQuerySrc
		) const;
	virtual bool _NcMenuButtonIsEnabled(
		const CExtControlBar * pBarQuerySrc
		) const;
	virtual bool _ClosingIsEnabled(
		const CExtControlBar * pBarQuerySrc
		) const;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	virtual bool _AutoHidingIsEnabled(
		const CExtControlBar * pBarQuerySrc
		) const;
	// is bar in autohide mode
	virtual bool AutoHideModeGet() const;
	// switch to/restore from autohide mode
	virtual bool AutoHideModeSet(
		bool bAutoHideMode,
		bool bAnimate,
		bool bRecalcLayout,
		bool bForceNoShow
		);
	virtual bool OnQueryVisibilityInAutoHideTabs() const;
	virtual bool OnQueryVisibilityInContainerTabs() const;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

private:
	typedef
		CArray < CExtBarNcAreaButton *, CExtBarNcAreaButton * >
		NcButtonsArr_t;
	NcButtonsArr_t m_vNcButtons;

public:
	enum e_bar_caption_text_t
	{
		__EBCT_SINGLE_CAPTION_DOCKED = 0,
		__EBCT_SINGLE_CAPTION_FLOATING = 1,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		__EBCT_TAB_GROUP_CAPTION_DOCKED = 2,
		__EBCT_TAB_GROUP_CAPTION_FLOATING = 3,
		__EBCT_TAB_ITEM_CAPTION = 4,
		__EBCT_AUTOHIDE_ITEM_CAPTION = 5,
		__EBCT_AUTOHIDE_SLIDER_CAPTION = 6,
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
		__EBCT_DOCUMENT_CONTAINER_CAPTION = 7
#endif // (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
	};
	virtual void OnGetBarCaptionText(
		e_bar_caption_text_t eBCT,
		CExtSafeString & strCaptionText
		) const;
	virtual CRect & NcButtons_CalcGripperTextLocation(
		CRect & rcPreCalcText
		);
	UINT NcButtons_HitTest(
		CPoint point,
		CExtBarNcAreaButton ** ppBtn = NULL
		);
	void NcButtons_Paint( CDC & dc );

	INT NcButtons_GetCount() const
	{
		ASSERT_VALID( this );
		return (INT)m_vNcButtons.GetSize();
	}

	CExtBarNcAreaButton * NcButtons_GetAt( INT nIndex )
	{
		ASSERT_VALID( this );
		ASSERT( nIndex >= 0 && nIndex < m_vNcButtons.GetSize() );
		CExtBarNcAreaButton * pBtn = 
			m_vNcButtons.GetAt( nIndex );
		ASSERT_VALID( pBtn );
		return pBtn;
	}
	
	const CExtBarNcAreaButton * NcButtons_GetAt( INT nIndex ) const
	{
		ASSERT_VALID( this );
		ASSERT( nIndex >= 0 && nIndex < m_vNcButtons.GetSize() );
		const CExtBarNcAreaButton * pBtn = 
			m_vNcButtons.GetAt( nIndex );
		ASSERT_VALID( pBtn );
		return pBtn;
	}
	
	void NcButtons_SetAt(
		INT nIndex,
		CExtBarNcAreaButton * pBtn
		)
	{
		ASSERT_VALID( this );
		ASSERT( pBtn != NULL );
		if( nIndex < 0 )
		{
			m_vNcButtons.Add( pBtn );
			return;
		}
		ASSERT_VALID( pBtn );
		ASSERT( nIndex >= 0 && nIndex <= m_vNcButtons.GetSize() );
		if( nIndex != m_vNcButtons.GetSize() )
		{
			CExtBarNcAreaButton * pBtnOld =
				m_vNcButtons.GetAt( nIndex );
			ASSERT_VALID( pBtnOld );
			if( pBtnOld != pBtn )
			{
				pBtnOld->OnNcDestroy();
				m_vNcButtons.SetAt( nIndex, pBtn );
			}
			return;
		} // if( nIndex != m_vNcButtons.GetSize() )
		m_vNcButtons.Add( pBtn );
	}
	
	void NcButtons_Add( CExtBarNcAreaButton * pBtn )
	{
		ASSERT_VALID( this );
		NcButtons_SetAt( -1, pBtn );
	}

	void NcButtons_InsertAt( INT nIndex, CExtBarNcAreaButton * pBtn )
	{
		ASSERT_VALID( this );
		if( ( nIndex < 0 ) || ( nIndex >= INT(m_vNcButtons.GetSize()) ) )
		{
			NcButtons_Add( pBtn );
			return;
		}
		ASSERT( nIndex >= 0 && nIndex <= m_vNcButtons.GetSize() );
		m_vNcButtons.InsertAt( nIndex, pBtn );
	}

	void NcButtons_RemoveAt( INT nIndex )
	{
		ASSERT_VALID( this );
		ASSERT( nIndex >= 0 && nIndex < m_vNcButtons.GetSize() );
		CExtBarNcAreaButton * pBtnOld =
			m_vNcButtons.GetAt( nIndex );
		ASSERT_VALID( pBtnOld );
		pBtnOld->OnNcDestroy();
		m_vNcButtons.RemoveAt( nIndex );
	}
	
	void NcButtons_RemoveAll()
	{
		ASSERT_VALID( this );
		INT nCount = (INT)m_vNcButtons.GetSize();
		for( INT nIndex = 0; nIndex < nCount; nIndex++ )
		{
			CExtBarNcAreaButton * pBtnOld =
				m_vNcButtons.GetAt( nIndex );
			ASSERT_VALID( pBtnOld );
			pBtnOld->OnNcDestroy();
		}
		m_vNcButtons.RemoveAll();
		m_vNcButtons.FreeExtra();
	}

	virtual bool NcButtons_HandleClick(
		CExtBarNcAreaButton * pNcAreaButton,
		CPoint point,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual void NcButtons_PostClick(
		CExtBarNcAreaButton * pNcAreaButton,
		CPoint point,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleQueryVisibility(
		bool & bButtonIsVisible,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleDraw(
		CDC & dc,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleHitTest(
		UINT & nExternalHT,
		CPoint point,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleQueryCursor(
		HCURSOR & hExternalCursor,
		CPoint point,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleReposition(
		CExtBarNcAreaButton * pPrevBtn,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleToolHitTest(
		__EXT_MFC_INT_PTR & hExternalToolHT,
		CPoint point,
		TOOLINFO * pTI,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);

	virtual void OnNcAreaButtonsReinitialize();
	virtual void OnNcAreaButtonsReposition();

	virtual bool OnQueryBarHoverProcessingEnabled() const;

	static bool g_bControlBarClassRegistered;
	static bool RegisterControlBarClass();

	static void RepositionBarsEx(
		CWnd * pWndToReposChilds,
		UINT nIDFirst,
		UINT nIDLast,
		UINT nIDLeftOver,
		UINT nFlag = 0,
		LPRECT lpRectParam = NULL,
		LPCRECT lpRectClient = NULL,
		BOOL bStretch = TRUE
		);
	
	virtual LRESULT DoHelpHitTest( CPoint ptHelpHitTest );

	virtual	bool _OnMouseMoveMsg(UINT nFlags, CPoint point);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
protected:
	mutable CExtCustomizeSite * m_pNotifiedCustomizeSite;
public:
	virtual CExtCustomizeSite * NotifiedCustomizeSiteGet() const;
#endif

protected:
	bool m_bDisplaying:1, m_bVisCheckFlag:1;
public:
	virtual bool _DisplayingGet() const;
	virtual void _DisplayingSet( bool bDisplaying );

	virtual void AnimationSite_OnProgressShutdownTimer( UINT nTimerID );
	virtual HWND AnimationSite_GetSafeHWND() const;

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtControlBar)
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

	friend class CExtBarButton;
	friend class CExtBarContentExpandButton;
	friend class CExtBarMdiDocButton;
	friend class CExtDockBar;
	friend class CExtDockOuterBar;
	friend class CExtToolControlBar;
	friend class CExtMenuControlBar;
	friend class CExtMiniDockFrameWnd;
	friend class CExtBarNcAreaButton;
	friend struct InternalBarRepositionData_t;
	friend class CExtDynControlBar;
	friend class CExtDockDynBar;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	friend class CExtDockDynTabBar;
	friend class CExtDynTabWnd;
	friend class CExtDynTabControlBar;
	friend class CExtDynAutoHideArea;
	friend class CExtDynAutoHideSlider;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	friend class InternalDockStateBar;
	friend class InternalDockStateSite;
	friend class InternalFriendlyFrameWnd;
}; // class CExtControlBar

#define __EXT_CONTROLBAR_CLASS_NAME _T("ProfUIS-ControlBar")

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)

class __PROF_UIS_API CExtDynDockHighlightWnd
	: public CWnd
	, public CExtPmBridge
{
public:
	DECLARE_DYNAMIC( CExtDynDockHighlightWnd );
	DECLARE_CExtPmBridge_MEMBERS( CExtDynDockHighlightWnd );
	CExtDynDockHighlightWnd(
		CExtDynDockMarkerWnd * pWndDynDockMarker
		);

// Attributes
public:
	CExtDynDockMarkerWnd * m_pWndDynDockMarker;
	CExtBitmap m_bmpHighlightSurface;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtDynDockHighlightWnd)
	protected:
	virtual BOOL PreCreateWindow( CREATESTRUCT & cs );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtDynDockHighlightWnd();

private:
	static bool g_bDockHighlightClassRegistered;
	static bool RegisterDockHighlightClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtDynDockHighlightWnd)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

}; // class CExtDynDockHighlightWnd

#define __EXT_DYNDOCKHIGHLIGHTWND_CLASS_NAME _T("ProfUIS-DynDockHighlightWnd")

/////////////////////////////////////////////////////////////////////////////
// CExtDynDockMarkerWnd window

class __PROF_UIS_API CExtDynDockMarkerWnd
	: public CWnd
	, public CExtPmBridge
{
public:
	DECLARE_DYNAMIC( CExtDynDockMarkerWnd );
	DECLARE_CExtPmBridge_MEMBERS( CExtDynDockMarkerWnd );
	CExtDynDockMarkerWnd(
		CExtDynDockMarkerWnd * pWndParentMarker,
		HWND hWndMarkerTarget,
		int eMarkerType,
		const CExtBitmap & _bmpIn,
		const CExtBitmap & _bmpOut,
		HRGN hRgn,
		CRect rcScreen,
		const CExtBitmap * _pBmpOuterBk = NULL,
		int nOffserInnerX = 0,
		int nOffserInnerY = 0,
		bool bSparsedMode = false
		);

// Attributes
public:
	bool m_bSparsedMode;
	int m_eMarkerType;
	HWND m_hWndLastHoverDepenentMarker;
private:
	CExtDynDockHighlightWnd * m_pWndDockHighlight;
	HWND m_hWndMarkerTarget;
	CExtDynDockMarkerWnd * m_pWndParentMarker;
	static CWnd * g_pWndMarkersParent;
	static CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
		g_arrDockMarkerWndsOuter;
	static CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
		g_arrDockMarkerWndsInner;
	static HWND g_hWndLastDynamicTarget;

public:
	CExtControlBar::e_calc_dock_type_t GetCDT();
	int GetMarkerType() const;
	HWND GetMarkerTargetHWND() const;
	static void UnHighlightAllMarkers(
		bool bPassPaintMessages = true
		);
	static void ShowAllMarkers(
		bool bShow = true
		);
	static void DestroyAllMarkers();
	static void DestroyMarkers(
		CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
			& arrDockMarkerWnds,
		bool bAnimate = true
		);
	static void ShowMarkers(
		CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
			& arrDockMarkerWnds,
		bool bShow = true,
		bool bAnimate = true
		);
	static void CreateOuterMarkers(
		CFrameWnd * pFrame,
		bool bShow = true,
		CExtPaintManager * pPM = NULL
		);
//	static void CreateInnerMarkers(
//		CFrameWnd * pFrame,
//		bool bShow = true
//		);
	static void CreateDynamicMarkers(
		CFrameWnd * pFrame,
		bool bShow = true,
		CExtPaintManager * pPM = NULL
		);

	static CExtDynDockMarkerWnd * stat_FindMarker(
		HWND hWndMarkerTarget,
		int eMarkerType,
		bool bSparsedMode
		);
	static HWND stat_AnalyzeDynBar(
		CExtDynControlBar * pDynBar,
		CPoint ptCursor
		);

	CExtBitmap m_bmpIn, m_bmpOut, m_bmpOuterBk;
	CPoint m_ptOffsetInner;
	CRgn m_rgn;
	static bool g_bDockMarkerClassRegistered;
	static bool RegisterDockMarkerClass();
	enum eMouseTrackState_t
	{
		__EMTS_UNKNOWN	= 0,
		__EMTS_HOVER	= 1,
		__EMTS_OUT		= 2
	};

	eMouseTrackState_t m_eMTS;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtDynDockMarkerWnd)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow( CREATESTRUCT & cs );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtDynDockMarkerWnd();
	bool HighlightMarkerTarget(
		bool bHighlight = true
		);
	bool IsStateApplyable() const;

	bool ShowMarker( 
		bool bShow, 
		bool bAnimate = true 
		);
	void AnimationStart(
		bool bShow
		);
	void AnimationStop();
	bool IsAnimationInProcess() const;
	bool m_bDestroyOnAnimationStop:1;

protected:
	bool m_bAnimationInProcess:1;
	bool m_bAnimationShow:1;
	int m_nOpaquePercent;

	static void UnHighlightMarkersExcluding(
		CExtDynDockMarkerWnd * pMarkerExclude,
		bool bPassPaintMessages = true
		);
	static void UnHighlightMarkersExcluding(
		CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
			& arrDockMarkerWnds,
		CExtDynDockMarkerWnd * pMarkerExclude,
		bool bPassPaintMessages = true
		);

	virtual void _RenderImpl( CDC * pDC );

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtDynDockMarkerWnd)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnPrintClient( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

	friend class CExtDynDockHighlightWnd;
}; // class CExtDynDockMarkerWnd

#define __EXT_DYNDOCKMARKER_CLASS_NAME _T("ProfUIS-DynDockMarker")

#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

class CExtDynamicBarSite;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	class CExtDynamicTabbedControlBar;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	class CExtTabPageContainerWnd;
	class CExtTabWnd;
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarNcAreaButtonMenu

class __PROF_UIS_API CExtDynamicBarNcAreaButtonMenu : public CExtBarNcAreaButtonMenu
{
public:
	DECLARE_DYNCREATE( CExtDynamicBarNcAreaButtonMenu );
	CExtDynamicBarNcAreaButtonMenu(
		CExtControlBar * pBar = NULL
		);
	virtual bool OnQueryVisibility() const;
	virtual bool OnQueryDeactivateAutohideSlider() const;
	virtual bool OnNcAreaClicked( CPoint point );
}; // class CExtDynamicBarNcAreaButtonMenu

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarNcAreaButtonAutoHide

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

class __PROF_UIS_API CExtDynamicBarNcAreaButtonAutoHide : public CExtBarNcAreaButtonAutoHide
{
public:
	DECLARE_DYNCREATE( CExtDynamicBarNcAreaButtonAutoHide );
	CExtDynamicBarNcAreaButtonAutoHide(
		CExtControlBar * pBar = NULL
		);
	virtual bool OnNcAreaClicked( CPoint point );
}; // class CExtDynamicBarNcAreaButtonAutoHide

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarNcAreaButtonClose

class __PROF_UIS_API CExtDynamicBarNcAreaButtonClose : public CExtBarNcAreaButtonClose
{
public:
	DECLARE_DYNCREATE( CExtDynamicBarNcAreaButtonClose );
	CExtDynamicBarNcAreaButtonClose(
		CExtControlBar * pBar = NULL
		);
	virtual bool OnNcAreaClicked( CPoint point );
}; // class CExtDynamicBarNcAreaButtonClose

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicControlBar

class CExtDynamicMDIChildWnd;

class __PROF_UIS_API CExtDynamicControlBar : public CExtControlBar
{
	CExtDynamicBarSite * m_pDBS;
protected:
	bool m_bPersistentBar:1;
public:
	bool m_bInsertToHeadOfTPC:1;
	static bool g_bInsertToHeadOfTPC;
	DECLARE_SERIAL( CExtDynamicControlBar );
	CExtDynamicControlBar();
	virtual ~CExtDynamicControlBar();
	bool IsPersistentBar() const;
	CExtDynamicBarSite * GetBarSite();
	virtual void PostNcDestroy();
	virtual void OnNcAreaButtonsReinitialize();
#if (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)
	bool m_bEnableDndDocumentTabs:1;
	virtual bool OnStartDraggingDocumentTabItem(
		CExtTabWnd * pWndTab,
		LONG nTabItemIndex
		);
#endif // (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)
protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
public:
	enum eDynamicBarState_t
	{
		__EDBS_FLOATING				= 0,
		__EDBS_DOCKED				= 1,
		__EDBS_DOCUMENT				= 2,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		__EDBS_AUTO_HIDDEN			= 3,
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	};
	virtual eDynamicBarState_t BarStateGet(
		bool * p_bIsVisibleState = NULL
		) const;
	virtual bool BarStateSet(
		eDynamicBarState_t eDBS,
		bool bShow
		);
	UINT m_nMdiMenuResourceID, m_nMdiAccelTableResourceID;
	CExtSafeString m_strCaptionText;
	CExtCmdIcon m_icon;
	virtual void OnGetBarCaptionText(
		e_bar_caption_text_t eBCT,
		CExtSafeString & strCaptionText
		) const;
	virtual bool NcButtons_HandleClick(
		CExtBarNcAreaButton * pNcAreaButton,
		CPoint point,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual void NcButtons_PostClick(
		CExtBarNcAreaButton * pNcAreaButton,
		CPoint point,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
private:
	enum eAdditionalStateFlags_t
	{
		__EASF_INIT_VALUE	= 0,
		__EASF_INVISIBLE	= 0x00000001,
	}; // enum eAdditionalStateFlags_t
	mutable DWORD m_dwAdditionalStateFlags;
	mutable eDynamicBarState_t m_eDBS;
	HWND m_hWndChildCached;
public:
	mutable eDynamicBarState_t m_eHelperSwitchingDBS;
	mutable bool m_bHelperSwitchingIntoVisibleState:1;
	virtual bool OnQueryUseCheckMarkCommand();
	virtual bool OnInitDbsMenu(
		CExtPopupMenuWnd * pPopup,
		HWND hWndTrack,
		CObject * pHelperSrc,
		LPARAM lParamHelper = 0
		);
protected:
	virtual bool _CanDockToTabbedContainers(
		CExtControlBar * pDestBar
		) const;
	virtual bool _CanDockLTRB(
		CExtControlBar * pDestBar
		) const;
    virtual void _DraggingUpdateState(
		const CPoint & point,
		bool bForceFloatMode
		);
	virtual CExtDynamicMDIChildWnd * OnCreateDynamicMDIChildWndInstance();
	virtual void OnMoveChildToDocument();
	virtual void OnMoveChildToBar();
public:
	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);
	virtual HWND OnQueryChildHWND();
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	// switch to/restore from autohide mode
	virtual bool AutoHideModeSet(
		bool bAutoHideMode,
		bool bAnimate,
		bool bRecalcLayout,
		bool bForceNoShow
		);
	virtual bool OnQueryVisibilityInAutoHideTabs() const;
	virtual bool OnQueryVisibilityInContainerTabs() const;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	virtual void OnSerializeDynamicProps( CArchive & ar );
protected:
	virtual void OnExtBarStateSerializationComplete(
		bool bStore
		);
public:
	virtual bool OnInsertDynamicBarCommandIntoPopupMenu(
		CExtPopupMenuWnd * pPopup,
		INT nInsertPos
		);
	friend class CExtDynamicBarNcAreaButtonMenu;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	friend class CExtDynamicBarNcAreaButtonAutoHide;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	friend class CExtDynamicBarNcAreaButtonClose;
	friend class CExtDynamicBarSite;
}; // class CExtDynamicControlBar

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicMDIChildWnd

class __PROF_UIS_API CExtDynamicMDIChildWnd
	: public CMDIChildWnd
{
	CExtDynamicBarSite * m_pDBS;
	CExtDynamicControlBar * m_pBar;
	HWND m_hWndHelperBar;
public:
	DECLARE_DYNAMIC( CExtDynamicMDIChildWnd );
	CExtCmdIcon m_icon;
	HICON m_hIconExtracted;
	CExtDynamicMDIChildWnd(
		CExtDynamicControlBar * pBar = NULL
		);
	virtual ~CExtDynamicMDIChildWnd();
	virtual void PostNcDestroy();
	CExtDynamicControlBar * GetBar();
	void AttachBar( CExtDynamicControlBar * pBar );
	CExtDynamicBarSite * GetBarSite();
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(
		UINT nID,
		int nCode,
		void * pExtra,
		AFX_CMDHANDLERINFO * pHandlerInfo
		);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
public:
	virtual BOOL DestroyWindow();
}; // class CExtDynamicMDIChildWnd

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarHookSink

class __PROF_UIS_API CExtDynamicBarHookSink : public CExtHookSink
{
	CExtDynamicBarSite * m_pDBS;
public:
	CExtDynamicBarHookSink();
	virtual ~CExtDynamicBarHookSink();
	CFrameWnd * DockSiteGet();
	CExtDynamicBarSite * GetBarSite();
protected:
	virtual bool OnHookWndMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		);
	virtual bool OnPreparePopupMenu( CExtPopupMenuWnd * pPopup );
	friend class CExtDynamicBarSite;
}; // class CExtDynamicBarHookSink

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarSite

class __PROF_UIS_API CExtDynamicBarSite
{
	CFrameWnd * m_pDockSite;
	CTypedPtrMap < CMapPtrToPtr, CExtDynamicControlBar *, CWnd * > m_mapBars;
	CTypedPtrList < CPtrList, CExtDynamicControlBar * > m_listBars, m_listBarsInDocMode;
	static const UINT g_nMsgFindDynamicBarSite;
	CExtDynamicBarHookSink m_DbsHookSink;
public:
	UINT m_nBarListInMenusCmdID;
	CExtDynamicBarSite();
	virtual ~CExtDynamicBarSite();
	static CExtDynamicBarSite * FindBarSite(
		HWND hWnd
		);
	static CExtDynamicBarSite * FindBarSite(
		CWnd * pWnd
		);
	HWND GetActiveMdiChildFrameHWND(
		bool * p_bMaximized = NULL,
		CMDIChildWnd ** ppMDIChildWnd = NULL
		);
	CFrameWnd * DockSiteGet();
	const CFrameWnd * DockSiteGet() const;
	void Install( CFrameWnd * pDockSite );
	virtual BOOL OnCmdMsg(
		UINT nID,
		int nCode,
		void * pExtra,
		AFX_CMDHANDLERINFO * pHandlerInfo
		);
	virtual BOOL OnCmdMsgOneBar(
		CExtDynamicControlBar * pBar,
		int nCode,
		void * pExtra,
		AFX_CMDHANDLERINFO * pHandlerInfo
		);
	virtual CExtDynamicMDIChildWnd * OnDbsCreateDynamicMDIChildWndInstance(
		CExtDynamicControlBar * pBar
		);
	virtual bool OnDbsQueryUseCheckMarkCommand(
		CExtDynamicControlBar * pBar
		);
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	virtual bool OnTabPageContainerClickedButton(
		LONG nHitTest,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	virtual bool OnPreparePopupMenu( CExtPopupMenuWnd * pPopup );
	virtual INT OnPrepareBarsListInMenu(
		CExtPopupMenuWnd * pPopup,
		INT nInsertPos
		);
	virtual UINT OnDbsAllocNewBarCommandID(
		__EXT_MFC_SAFE_LPCTSTR strCaptionText,
		const CExtCmdIcon & icon,
		UINT nDesiredCmdID = 0
		) const;
	virtual void OnDbsFreeBarCommandID( UINT nCmdID ) const;
	virtual void OnDbsResetPageSequence();
protected:
	virtual CExtDynamicControlBar * OnDbsCreateNewBarInstance() const;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	virtual CExtDynamicTabbedControlBar * OnDbsCreateTabbedBarInstance() const;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//	CExtDynamicControlBar * BarFindAnyVisible();
public:
	CExtDynamicControlBar * BarFindByChildHWND( HWND hWndChildToFind );
	CExtDynamicControlBar * BarAlloc(
		__EXT_MFC_SAFE_LPCTSTR strCaptionText,
		const CExtCmdIcon & icon,
		UINT nDesiredCmdID = 0,
		CRuntimeClass * pDesiredRTC = NULL,
		bool bPersistentBar = false
		);
	void BarFree( CExtDynamicControlBar * pBar );
	void BarFreeAll(
		bool bFreePersistentBars = true,
		bool bFreeDynamicBars = true
		);
	CExtDynamicControlBar * BarGetByCmdID( UINT nLookupCmdID );
	void BarEnumGetArray(
		CTypedPtrArray < CPtrArray, CExtDynamicControlBar * > & arrAllDynamicBars,
		bool bIncludePersistentBars = true,
		bool bIncludeDynamicBars = true
		);
	INT BarEnumGetCount(
		bool bIncludePersistentBars = true,
		bool bIncludeDynamicBars = true
		) const;
	POSITION BarEnumGetStartPosition() const;
	CExtDynamicControlBar * BarEnumGetNext(
		POSITION & pos,
		bool bIncludePersistentBars = true,
		bool bIncludeDynamicBars = true
		);
	const CExtDynamicControlBar * BarEnumGetNext(
		POSITION & pos,
		bool bIncludePersistentBars = true,
		bool bIncludeDynamicBars = true
		) const;
public:
	virtual bool StateSerialize(
		CArchive & ar,
		bool bEnableThrowExceptions = false
		);
	bool StateLoad(
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
		HKEY hKeyRoot = HKEY_CURRENT_USER,
		bool bEnableThrowExceptions = false
		);
	bool StateSave(
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
		HKEY hKeyRoot = HKEY_CURRENT_USER,
		bool bEnableThrowExceptions = false
		);
protected:
	void _BarFreeAllImpl(
		bool bForceNoOptimizeMode,
		bool bFreePersistentBars = true,
		bool bFreeDynamicBars = true
		);
	void _BarFreeImpl(
		CExtDynamicControlBar * pBar,
		bool bForceFloatMode,
		bool bForceNoOptimizeMode
		);
public:
	enum eDetectedUiType_t
	{
		__EDUIT_UNSUPPORTED_ENVIRONMENT	= 0, // UI is not compatible with dynamic bar site
		__EDUIT_MDI_ORDINARY			= 1, // simple MDI
		__EDUIT_MDI_WITH_TABS			= 2, // MDI with tabs
		__EDUIT_SDI_TAB_PAGE_CONTAINER	= 3, // SDI with tab page container
		__EDUIT_NOT_DETECTED_YET		= 4, // for internal use only
		__EDUIT_VALID_VALUE_MIN			= 0, // for internal use only
		__EDUIT_VALID_VALUE_MAX			= 3, // for internal use only
		__EDUIT_DETECTED_VALUE_MIN		= 1, // for internal use only
		__EDUIT_DETECTED_VALUE_MAX		= 3, // for internal use only
	}; // enum eDetectedUiType_t
private:
	mutable eDetectedUiType_t m_eDUIT;
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	mutable CExtTabPageContainerWnd * m_pWndTabPageContainer;
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
	mutable CExtTabWnd * m_pWndMdiTabs;
#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)
#if (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)
public:
	bool m_bEnableDndDocumentTabs:1;
	virtual bool OnStartDraggingDocumentTabItem(
		CExtDynamicControlBar * pBar,
		CExtTabWnd * pWndTab,
		LONG nTabItemIndex
		);
#endif // (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)
private:
	mutable HWND m_hWndMdiClient;
public:
	virtual eDetectedUiType_t GetDetectedUiType() const;

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	CExtTabPageContainerWnd * GetTabPageContainer();
	const CExtTabPageContainerWnd * GetTabPageContainer() const;
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
	CExtTabWnd * GetMdiTabs();
	const CExtTabWnd * GetMdiTabs() const;
#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)
	HWND GetMdiClient() const;

	friend class CExtDynamicControlBar;
	friend class CExtDynamicBarHookSink;
}; // class CExtDynamicBarSite

#endif // (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_CONTROLBAR_H

