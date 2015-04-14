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

#if (!defined __EXT_POPUP_MENU_WND_H)
#define __EXT_POPUP_MENU_WND_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_HOOK_H)
	#include <../Src/ExtHook.h>
#endif // __EXT_HOOK_H

#if (!defined __AFXTEMPL_H__)
	#include <AfxTempl.h>
#endif

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define __BaseClassOfCExtPopupBaseWnd CWnd 
#define __EXT_MENU_DEF_INPLACE_EDIT_WIDTH 100

class CExtBarButton;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
class CExtPopupScreenTipWnd;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtShadowCtrl

class __PROF_UIS_API CExtShadowCtrl
	: public CWnd
	//, public CExtHookSink
{
public:
	DECLARE_DYNCREATE( CExtShadowCtrl );
	mutable CExtBitmap m_bmpLayerSource;
	CBitmap m_bmpLayerData;
	mutable CRect m_rcOuterPadding, m_rcRenderingPaddingData;
	CExtShadowCtrl();
	virtual ~CExtShadowCtrl();
	virtual bool IsAvailable() const;
	virtual bool Create(
		HWND hWndParent,
		INT nShadowSize
		);
	bool UpdateWindowLayer(
		BYTE nConstantAlpha
		);
	virtual bool UpdateWindowLayer(
		CBitmap & _bmpLayer,
		BYTE nConstantAlpha
		);
protected:
	HWND m_hWndParent;
	virtual LRESULT WindowProc(
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);
//	virtual bool OnHookWndMsg(
//		LRESULT & lResult,
//		HWND hWndHooked,
//		UINT nMessage,
//		WPARAM & wParam,
//		LPARAM & lParam
//		);
}; // class CExtShadowCtrl

/////////////////////////////////////////////////////////////////////////////
// CExtSoundPlayer

class __PROF_UIS_API CExtSoundPlayer : public CObject
{
	struct __PROF_UIS_API InternalSoundEventInitDone_t : public CEvent
	{
		InternalSoundEventInitDone_t();
		void SetInitDone();
		void WaitInitDone();
	}; // struct InternalSoundEventInitDone_t
	struct __PROF_UIS_API InternalSoundThredadParms_t
	{
		InternalSoundEventInitDone_t * m_pEventInitDone;
		TCHAR m_sSoundSpecBuffer[1024];
		HANDLE m_hModule;
		DWORD m_dwPlayerFlags;
		HANDLE m_hThread;
		DWORD m_dwThreadID;
		static CEvent g_EventPlaySnd;
		static CCriticalSection g_CsPlaySnd;
		InternalSoundThredadParms_t(
			__EXT_MFC_SAFE_LPCTSTR sSoundSpecBuffer,
			HANDLE hModule,
			DWORD dwPlayerFlags
			);

		static DWORD WINAPI stat_ParallelPlayerProc(
			InternalSoundThredadParms_t * pParms
			);
		operator LPTHREAD_START_ROUTINE() const
		{
			return (LPTHREAD_START_ROUTINE)stat_ParallelPlayerProc;
		}
	
		void PlaySound();
		void PlaySoundParallel();
	}; // struct InternalSoundThredadParms_t

public:

	DECLARE_DYNCREATE( CExtSoundPlayer );

	class __PROF_UIS_API CExtSoundPlayerAutoPtr
	{
		CExtSoundPlayer * m_pPlayer;
	public:
		CExtSoundPlayerAutoPtr();
		virtual ~CExtSoundPlayerAutoPtr();
		void InstallSoundPlayer(
			CExtSoundPlayer * pPlayer
			);
		CExtSoundPlayer * operator -> ()
		{
			ASSERT( m_pPlayer != NULL );
			ASSERT_VALID( m_pPlayer );
			return m_pPlayer;
		}
	}; // class CExtSoundPlayerAutoPtr
	
	CExtSoundPlayer();
	virtual ~CExtSoundPlayer();

	enum e_ui_sounds_t
	{
	__NO_SOUND					= 0,
	__ON_MENU_EXPAND_CLICKED	= 1,
	__ON_MENU_POPUP_DISPLAYED	= 2,
	__ON_MENU_CMD_CLICKED		= 3,
	}; // enum e_ui_sounds_t

	virtual void PlaySound(
		CExtSoundPlayer::e_ui_sounds_t eSoundID
		);
}; // class CExtSoundPlayer

extern __PROF_UIS_API CExtSoundPlayer::CExtSoundPlayerAutoPtr g_SoundPlayer;

/////////////////////////////////////////////////////////////////////////////
// CExtWndShadow

class __PROF_UIS_API CExtWndShadow
{
	CBitmap m_bmp0mak, m_bmp1mak;		// photo of right & bottom shadow parts to save/restore
	CBitmap m_bmp0org, m_bmp1org;		// original shadow source
	COLORREF * m_pHelperDibSurface;
	CRect m_rcWndArea;					// window are not including shadow
	CRect m_rc1stArea;					// basic area
	CRect m_rc2ndArea;					// combined area
	UINT m_nShadowSize;					// shadow size (in pixels)
	UINT m_nBr0, m_nBr1;				// min/max brightness percents [0..100]
	bool m_bEnablePhotos:1;				// use photo bitmaps
	bool m_bEnablePaintManagerColor:1;	// use paint manager's shadow color
	CRgn m_rgnSafe;
	void _FreeWinObjects();
	void _DoPixelOvershadow(
		int nMakeSpec,
		int nPosX,
		int nPosY,
		COLORREF clrShadowAdjust
		);
	bool _MakePhotos( CDC & dc, CDC & dcmm );
	bool _MakeOriginals( CDC & dc );
	bool _PaintLo(
		CExtPaintManager * pPM,
		CDC & dc,
		HWND hWndStatic,
		HWND hWndFloat,
		LPCRECT pRectCustomFloatPos = NULL
		);
	bool _PaintHi(
		CExtPaintManager * pPM,
		CDC & dc,
		HWND hWndStatic,
		HWND hWndFloat,
		LPCRECT pRectCustomFloatPos = NULL
		);
	void _Assign( const CExtWndShadow & other );
	bool _RestoreOriginals( CDC & dc );
	bool _HaveOriginals() const;
public:
	enum eShadowOptions
	{
		DEF_BRIGHTNESS_MIN =  75UL, // default min brightness percent
		DEF_BRIGHTNESS_MAX = 100UL, // default max brightness percent
		DEF_SHADOW_SIZE    =   3UL, // default shadow size (in pixels)
	};

	CExtWndShadow();
	CExtWndShadow( const CExtWndShadow & other );
	virtual ~CExtWndShadow();

	CExtWndShadow & operator = ( const CExtWndShadow & other );

	void ApplyFloatingImage(
		HWND hWndStatic,
		HWND hWndFloat,
		LPCRECT pRectCustomFloatPos = NULL
		);
	
	bool Restore( CDC & dc );
	bool Paint(
		CExtPaintManager * pPM,
		CDC & dc,
		bool m_bMakePhotosOnly = false,
		HWND hWndStatic = NULL,
		HWND hWndFloat = NULL,
		LPCRECT pRectCustomFloatPos = NULL
		);
	bool Paint(
		CExtPaintManager * pPM,
		CDC & dc,
		const CRect & rcWndArea,
		const CRect & rc1stArea = CRect(0,0,0,0),
		const CRect & rc2ndArea = CRect(0,0,0,0),
		UINT nShadowSize = DEF_SHADOW_SIZE,
		UINT nBr0 = DEF_BRIGHTNESS_MIN,
		UINT nBr1 = DEF_BRIGHTNESS_MAX,
		bool bEnablePhotos = true,
		bool bEnablePaintManagerColor = true,
		bool m_bMakePhotosOnly = false,
		HWND hWndStatic = NULL,
		HWND hWndFloat = NULL,
		LPCRECT pRectCustomFloatPos = NULL
		);
	bool HavePhotos() const;
	void Destroy();
	void PostReShadow();
}; // class CExtWndShadow


/////////////////////////////////////////////////////////////////////////////
// CExtPopupBaseWnd

class __PROF_UIS_API CExtPopupBaseWnd
	: public __BaseClassOfCExtPopupBaseWnd
	, public CExtPmBridge
{
public:

	DECLARE_DYNCREATE( CExtPopupBaseWnd );
	DECLARE_CExtPmBridge_MEMBERS( CExtPopupBaseWnd );

	CExtPopupBaseWnd();

	virtual bool _IsFadeOutAnimation() const;
	virtual BOOL DestroyWindow();
	virtual bool OnQueryLayoutRTL() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtPopupBaseWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtPopupBaseWnd();

	static CObject * g_pCombinedHelper;
	static const UINT g_nMsgPrepareMenu; // prepare popup-menu window message (entire tree)
	static const UINT g_nMsgPrepareOneMenuLevel; // prepare popup-menu window message (one tree level)
	static const UINT g_nMsgPrepareDesktopBk;
	static const UINT g_nMsgPopupNext;
	static const UINT g_nMsgPopupPrev;
	static const UINT g_nMsgNotifyMenuExpanded;
	static const UINT g_nMsgNotifyMenuClosed;
	static const UINT g_nMsgPopupDrawItem;
	static const UINT g_nMsgPopupDrawLeftArea;
	static const UINT g_nMsgPopupDrawBackground;
	static const UINT g_nMsgCreateInplaceEdit;
	static const UINT g_nMsgTranslateKeyboardEvent;
	static const UINT g_nMsgTranslateMouseClickEvent;
	static const UINT g_nMsgTranslateMouseMoveEvent;
	static const UINT g_nMsgTranslateMouseWheelEvent;

	struct __PROF_UIS_API TranslateKeyboardEventData_t
	{
		CExtPopupBaseWnd * m_pEventSourcePopup;
		bool m_bMessageHandled:1, m_bNoEat:1;
		UINT m_nChar, m_nRepCnt, m_nFlags;
		TranslateKeyboardEventData_t(
			CExtPopupBaseWnd * pEventSourcePopup,
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags,
			bool bNoEat
			);
		operator WPARAM ();
		static TranslateKeyboardEventData_t & FromWPARAM( WPARAM wParam );
		bool Notify( HWND hWnd );
		bool Notify();
	};

	struct __PROF_UIS_API TranslateMouseClickEventData_t
	{
		CExtPopupBaseWnd * m_pEventSourcePopup;
		bool m_bMessageHandled:1, m_bNoEat:1;
		UINT m_nFlags;
		CPoint m_point;
		TranslateMouseClickEventData_t(
			CExtPopupBaseWnd * pEventSourcePopup,
			UINT nFlags,
			CPoint point,
			bool bNoEat
			);
		operator WPARAM ();
		static TranslateMouseClickEventData_t & FromWPARAM( WPARAM wParam );
		bool Notify( HWND hWnd );
		bool Notify();
	};

	struct __PROF_UIS_API TranslateMouseMoveEventData_t
	{
		CExtPopupBaseWnd * m_pEventSourcePopup;
		bool m_bMessageHandled:1, m_bNoEat:1;
		UINT m_nFlags;
		CPoint m_point;
		TranslateMouseMoveEventData_t(
			CExtPopupBaseWnd * pEventSourcePopup,
			UINT nFlags,
			CPoint point,
			bool bNoEat
			);
		operator WPARAM ();
		static TranslateMouseMoveEventData_t & FromWPARAM( WPARAM wParam );
		bool Notify( HWND hWnd );
		bool Notify();
	};

	struct __PROF_UIS_API TranslateMouseWheelEventData_t
	{
		CExtPopupBaseWnd * m_pEventSourcePopup;
		bool m_bMessageHandled:1, m_bNoEat:1;
		WPARAM m_wParam;
		LPARAM m_lParam;
		TranslateMouseWheelEventData_t(
			CExtPopupBaseWnd * pEventSourcePopup,
			WPARAM wParam,
			LPARAM lParam,
			bool bNoEat
			);
		operator WPARAM ();
		static TranslateMouseWheelEventData_t & FromWPARAM( WPARAM wParam );
		bool Notify( HWND hWnd );
		bool Notify();
	};

	enum e_combine_align_t // relatively to menu window rect
	{
		__CMBA_NONE = 0,
		__CMBA_TOP = 1,
		__CMBA_BOTTOM = 2,
		__CMBA_RIGHT = 3,
		__CMBA_LEFT = 4,
	};

	typedef
		void (*pCbPaintCombinedContent)(
			LPVOID pCookie,
			CDC & dc,
			const CWnd & refWndMenu,
			const CRect & rcExcludeArea, // in screen coords
			int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
			);

	enum e_animation_type_t
	{
		__AT_NONE = 0,
		__AT_RANDOM = 1,
		__AT_ROLL = 2,
		__AT_SLIDE = 3,
		__AT_FADE = 4,
		__AT_ROLL_AND_STRETCH = 5,
		__AT_SLIDE_AND_STRETCH = 6,
		__AT_NOISE = 7,
		__AT_BOXES = 8,
		__AT_CIRCLES = 9,
		__AT_HOLES = 10,
		
		__AT_MAX_VALID_VALUE = 10, // used for random effect selection
		__AT_CONTENT_DISPLAY = -1, // internal helper - delayed display
		__AT_CONTENT_EXPAND = -2 // internal helper - funny expand/stretch rarely used items (office XP like)
	};
	static e_animation_type_t g_DefAnimationType;
	static CExtSafeString GetAnimaptionName( e_animation_type_t eAT );

	static bool g_bUseDesktopWorkArea; // align to desktop work area (false - to screen area)
	static bool g_bEnableOnIdleCalls; // enable idle time processing

protected:
	
	CRect m_rcExcludeArea;
	CPoint m_ptTrack, m_ptTrackOriginal, m_ptTrackInvoked, m_ptTrackWatched;
	bool m_bExcludeAreaSpec:1,m_bCombineWithEA:1;

	LPVOID m_pCbPaintCombinedCookie;
	pCbPaintCombinedContent m_pCbPaintCombinedContent;
	bool m_bCookieIsObject;

public:
	bool _IsExcludeAreaSpecified() const
	{
		ASSERT_VALID( this );
		return m_bExcludeAreaSpec;
	}
	bool _IsCombineWithEA() const
	{
		ASSERT_VALID( this );
		return m_bCombineWithEA;
	}

	CRect GetExcludeArea() const
	{
//		ASSERT_VALID( this );
		return m_rcExcludeArea;
	}
	virtual CObject * GetCookieAsObject();
	virtual bool IsToolBtnCombinedArea() const;
	pCbPaintCombinedContent GetCbPaintCombinedContent() const
	{
		return (pCbPaintCombinedContent) m_pCbPaintCombinedContent;
	}
	LPVOID GetCbPaintCombinedCookie() const
	{
		return (LPVOID)m_pCbPaintCombinedCookie;
	}

protected:
	e_animation_type_t m_AnimationType;
	CExtSoundPlayer::e_ui_sounds_t m_ePlaySoundOnAnimationFinished;

	COLORREF * m_clrFadeBitsSrc;
	COLORREF * m_clrFadeBitsDst;
	COLORREF * m_clrFadeBitsTmp;
	int m_nAnimPercent,m_nAnimPercentOld;
	bool m_bAnimFinished:1;

	CSize m_sizeFullItems;

public:
	e_combine_align_t m_eCombineAlign;
	CExtWndShadow m_ShadowMain, m_ShadowCMBA;
	CExtShadowCtrl m_ctrlShadow;
protected:
	CBitmap
		m_bmpScreenSrc, m_bmpScreenSrcAlt,
		m_bmpScreenDst, m_bmpScreenTmp;
		//m_bmpSaveBits;

public:
	virtual bool _FindCustomizeMode() const;
protected:
	virtual void _InitAnimation();
	virtual void _StartAnimation();
	virtual void _AdjustAnimation( e_animation_type_t & eAT );

	void _SurfacesDelete()
	{
		ASSERT_VALID( this );

		if( m_bmpScreenDst.GetSafeHandle() != NULL )
		{
			VERIFY(
				m_bmpScreenDst.DeleteObject()
				);
		}
		if( m_bmpScreenSrc.GetSafeHandle() != NULL )
		{
			VERIFY(
				m_bmpScreenSrc.DeleteObject()
				);
		}
		if( m_bmpScreenSrcAlt.GetSafeHandle() != NULL )
		{
			VERIFY(
				m_bmpScreenSrcAlt.DeleteObject()
				);
		}
		if( m_bmpScreenTmp.GetSafeHandle() != NULL )
		{
			VERIFY(
				m_bmpScreenTmp.DeleteObject()
				);
		}
		m_clrFadeBitsSrc
			= m_clrFadeBitsDst
			= m_clrFadeBitsTmp
			= NULL;
	}
	
	virtual void _EndAnimation();

	virtual bool _IsPopupWithShadows() const;
	virtual bool _IsPopupWithShadowsDynamic() const;

	virtual void _DrawAnimatedState( CDC & dc );
public:
	virtual int OnQueryMenuShadowSize() const;
	virtual void _DoPaint( CDC & dcPaint, bool bUseBackBuffer = true );
	virtual void _DelayPaint();
	virtual void _GetClientRect(RECT * pRectClient)
	{
		ASSERT_VALID( this );

		ASSERT( GetSafeHwnd() != NULL );
		ASSERT( pRectClient != NULL );
		GetClientRect(pRectClient);
	}
	virtual bool _FindHelpMode() const;

protected:
	HFONT m_hFont;

protected:

	// Generated message map functions
	//{{AFX_MSG(CExtPopupBaseWnd)
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	afx_msg LRESULT OnSetFont( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFont( WPARAM, LPARAM );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool m_bPrintState:1;
	virtual LRESULT WindowProc(
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);

}; // class CExtPopupBaseWnd

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuTipWnd

class __PROF_UIS_API CExtPopupMenuTipWnd
	: public CExtPopupBaseWnd
{
public:
	static bool g_bEnableAdvTips;
	enum e_tip_style_t
	{
		__ETS_NONE = 0,
		__ETS_BALLOON = 1,
		__ETS_BALLOON_NO_ICON = 2,
		__ETS_RECTANGLE = 3,
		__ETS_RECTANGLE_NO_ICON = 4,
		__ETS_INV_RECTANGLE = 5,
		__ETS_INV_RECTANGLE_NO_ICON = 6,
	};
	CRect m_rcAlignment;
	UINT m_nPeriodDelayShowQuick, m_nPeriodDelayShowNormal;
	bool m_bNoHideDetection:1;
	CFont m_fontAdvTip;
protected:
	CExtCmdIcon m_icon;
	CSize m_sizeRenderingIcon, m_sizeClassicMargins,
		m_sizeRounders, m_sizeLeader;
	int m_nIconMarginDX, m_nBalloonDistance, m_nClassicDistance, m_nSizeShadow;
	CExtSafeString m_sText;
	bool m_bFlipHorz:1, m_bFlipVert:1;
	e_tip_style_t m_eTS;
	CExtWndShadow m_shadow;
	long m_ttLA;
	bool m_bPassiveMode:1, m_bDelayedLayeredBehavior:1;
	CPoint m_ptGuideLines;
	BYTE m_nTransparencyKey;
	mutable HWND m_hWndLastParent;
public:
	DECLARE_DYNCREATE( CExtPopupMenuTipWnd );

	CExtPopupMenuTipWnd();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtPopupMenuTipWnd)
	//}}AFX_VIRTUAL

public:
	virtual ~CExtPopupMenuTipWnd();

	void SetIcon(
		HICON hIcon,
		bool bCloneIcon = false
		);
	void SetIcon(
		CExtCmdIcon & _icon
		);
	virtual __EXT_MFC_SAFE_LPCTSTR GetText() const;
	virtual void SetText(
		__EXT_MFC_SAFE_LPCTSTR lpszText
		);
	virtual e_tip_style_t GetTipStyle() const;
	virtual void SetTipStyle( e_tip_style_t eTS );
	virtual int GetShadowSize() const;
	virtual void SetShadowSize( int nSizeShadow );

	static CTypedPtrList < CPtrList, CExtPopupMenuTipWnd * > g_listAllTips;
	static void UpdateDelayedLayeredBehaviorAll(
		CRuntimeClass * pRTC = NULL
		);
	void UpdateDelayedLayeredBehavior();
	HWND GetLastParentHWND() const
	{
		return m_hWndLastParent;
	}
	virtual bool Show(
		CWnd * pWndParent,
		const RECT & rcExcludeArea,
		bool bNoDelay,
		HDWP & hPassiveModeDWP
		);
	bool Show(
		CWnd * pWndParent,
		const RECT & rcExcludeArea,
		bool bNoDelay = false
		)
	{
		HDWP hPassiveModeDWP = NULL;
		return Show( pWndParent, rcExcludeArea, bNoDelay, hPassiveModeDWP );
	}
	virtual void Hide();

protected:
	virtual int CalcShadowSize() const;
	virtual HRGN CalcRegion(
		CDC & dc,
		CSize * pSize = NULL
		);
public:
	virtual void _DoPaint(
		CDC & dcPaint,
		bool bUseBackBuffer = true
		);
protected:
	virtual void _EndAnimation();

	// Generated message map functions
	//{{AFX_MSG(CExtPopupMenuTipWnd)
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	//}}AFX_MSG
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif
	DECLARE_MESSAGE_MAP()

}; // class CExtPopupMenuTipWnd

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupKeyTipWnd

class __PROF_UIS_API CExtPopupKeyTipWnd : public CExtPopupMenuTipWnd
{
// Construction
public:
	DECLARE_DYNCREATE( CExtPopupKeyTipWnd );
	CExtPopupKeyTipWnd();
	virtual ~CExtPopupKeyTipWnd();

// Attributes
public:

// Operations
public:
	virtual bool Create(
		CWnd * pWndKeyTipParent,
		CPoint ptGuideLines,
		__EXT_MFC_SAFE_LPCTSTR strKeyTipText,
		bool bEnabled,
		HDWP & hPassiveModeDWP
		);

// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CExtPopupKeyTipWnd)
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
    //{{AFX_MSG(CExtPopupKeyTipWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtPopupKeyTipWnd

/////////////////////////////////////////////////////////////////////////////
// CExtPopupScreenTipWnd

class __PROF_UIS_API CExtPopupScreenTipWnd : public CExtPopupMenuTipWnd
{
// Construction
public:
	DECLARE_DYNCREATE( CExtPopupScreenTipWnd );
	CExtPopupScreenTipWnd();
	virtual ~CExtPopupScreenTipWnd();

// Attributes
public:
	CExtCustomizeCmdScreenTip m_cmdScreenTip;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CExtPopupScreenTipWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	HRGN CalcRegion(
		CDC & dc,
		CSize * pSize = NULL
		);
public:
	virtual void _DoPaint(
		CDC & dcPaint,
		bool bUseBackBuffer = true
		);

protected:
    //{{AFX_MSG(CExtPopupScreenTipWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtPopupScreenTipWnd

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuSite

class CExtPopupMenuWnd;
class CExtPopupColorMenuWnd;
class CExtPopupInplaceListBox;
class CExtDatePickerWnd;

class __PROF_UIS_API CExtPopupMenuSite
{
	bool m_bShutdownMode:1;
	CExtPopupMenuTipWnd * m_pWndToolTip;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtPopupScreenTipWnd * m_pWndScreenTip;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	CExtPopupMenuWnd
		* m_pTopMenu,
		* m_pWndCapture,
		* m_pWndAnimation;
	HHOOK m_hMouseHook;
	HHOOK m_hKeyboardHook;
	UINT * m_lpnResultCmdID;

	void _Done();

	// hooks
	static LRESULT CALLBACK _HookMouseProc(
		int nCode,      // hook code
		WPARAM wParam,  // message identifier
		LPARAM lParam   // mouse coordinates
		);
	static LRESULT CALLBACK _HookKeyboardProc(
		int nCode,      // hook code
		WPARAM wParam,  // virtual-key code
		LPARAM lParam   // keystroke-message information
		);
	void _Hook( bool bHook = true );

	CMap < CExtPopupMenuWnd *, CExtPopupMenuWnd *, bool, bool > m_mapFadeOutInsances;
public:
	
	CExtPopupMenuSite();
	virtual ~CExtPopupMenuSite();

	bool IsEmpty() const;
	bool IsShutdownMode() const;
	CExtPopupMenuTipWnd & GetTip();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtPopupScreenTipWnd & GetScreenTip();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	void SetTargetCmdIdPtr( UINT * lpnResultCmdID = NULL );
	UINT * GetTargetCmdIdPtr();

	CExtPopupMenuWnd * GetCapture();
	void SetCapture( CExtPopupMenuWnd * pNewPopup = NULL );

	CExtPopupMenuWnd * GetAnimated();
	void SetAnimated( CExtPopupMenuWnd * pNewPopup = NULL );

	void SetInstance( CExtPopupMenuWnd * pTopMenu = NULL );
	CExtPopupMenuWnd * GetInstance();
	void DoneInstance();
	void FadeOutInstanceAdd( CExtPopupMenuWnd * pPopup );
	void FadeOutInstanceRemove( CExtPopupMenuWnd * pPopup );
	
	void operator = ( CExtPopupMenuWnd * pTopMenu );
	CExtPopupMenuWnd * operator -> ();

	bool IsTopPopup(
		CExtPopupMenuWnd * pTopMenu
		) const;

	static CExtPopupMenuSite g_DefPopupMenuSite;

	friend class CExtPopupBaseWnd;
	friend class CExtPopupMenuWnd;
}; // class CExtPopupMenuSite

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuWnd

// track flags (menu area align)
#define TPMX_LEFTALIGN							0x00000001L
#define TPMX_CENTERALIGN						0x00000002L
#define TPMX_RIGHTALIGN							0x00000003L
#define TPMX_TOPALIGN							0x00000004L
#define TPMX_VCENTERALIGN						0x00000005L
#define TPMX_BOTTOMALIGN						0x00000006L
#define TPMX_ALIGN_MASK							0x0000000FL
// track flags (menu area combine type)
#define TPMX_COMBINE_NONE						0x00000000L // do not combine menu area with lpRect
#define TPMX_COMBINE_ANY_SUITABLE				0x00000010L // try combine menu area with lpRect
#define TPMX_COMBINE_DEFAULT					0x00000020L // do like PaintManager said
#define TPMX_COMBINE_MASK						0x00000030L
// select first enabled menu item
#define TPMX_SELECT_ANY							0x00000040L
// perform message loop
#define TPMX_DO_MESSAGE_LOOP					0x00000080L
// initially expand rarely used items
#define TPMX_NO_HIDE_RARELY						0x00000100L
// do not post WM_COMMAND message
// (result could be stored in lpnResultCmdID)
#define TPMX_NO_WM_COMMAND						0x00000200L
// do not update cmdui
#define TPMX_NO_CMD_UI							0x00000400L
// ownerdraw items
// (variable sizes are not supported)
#define TPMX_OWNERDRAW_FIXED					0x00000800L
// force no shadows
#define TPMX_NO_SHADOWS							0x00001000L
// help context mode
#define TPMX_HELP_CTX_MODE						0x00002000L
// customize mode / no menu site control
#define TPMX_CUSTOMIZE_MODE						0x00004000L
// force no animation (for quick displaying)
#define TPMX_FORCE_NO_ANIMATION					0x00008000L
// toolbar/palette mode
#define TPMX_PALETTE							0x00010000L
// palette with floating toolbar background
#define TPMX_PALETTE_TB_BKGND					0x00020000L
// tear-off caption
#define TPMX_TEAROFF							0x00040000L
// do not connect to global menu site
#define TPMX_NO_SITE							0x00080000L
// ribbon mode, if supported by paint manager
#define TPMX_RIBBON_MODE						0x00100000L
// ribbon mode, file menu
#define TPMX_RIBBON_FILE_MENU					0x00200000L
// ribbon mode, file menu
#define TPMX_RIBBON_OPTIONS_BUTTON				0x00400000L
// ribbon mode, file menu
#define TPMX_RIBBON_EXIT_BUTTON					0x00800000L
// disable fade-out animation
#define TPMX_NO_FADE_OUT_ANIMATION				0x01000000L
// disable dynamic shadows
#define TPMX_NO_DYNAMIC_SHADOWS					0x02000000L
// ribbon-like resizing
#define TPMX_RIBBON_RESIZING					0x04000000L
// ribbon-like resizing vertical only
#define TPMX_RIBBON_RESIZING_VERTICAL_ONLY		0x08000000L
// ribbon key tips
#define TPMX_RIBBON_KEYTIPS						0x10000000L
// hide keyboard accelerators
#define TPMX_HIDE_KEYBOARD_ACCELERATORS			0x20000000L
// system menu
#define TPMX_SYSMENU							0x40000000L

class __PROF_UIS_API CExtPopupMenuWnd
	: public CExtPopupBaseWnd
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, public CExtCustomizeSite::ICustomizeDropTarget
	, public CExtCustomizeSite::ICustomizeDropSource
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
{
	friend class CExtControlBar;
	friend class CExtToolControlBar;
	friend class CExtMenuControlBar;

	class __PROF_UIS_API InternalFriendlyWinApp : CWinApp
	{
	public:
		inline static InternalFriendlyWinApp * _GetFriendlyApp()
		{
			CWinApp * pApp = ::AfxGetApp();
			ASSERT( pApp != NULL );
			return (InternalFriendlyWinApp *)pApp;
		}
		CRecentFileList * _GetRecentFileList()
		{
			//ASSERT( m_pRecentFileList != NULL );
			return m_pRecentFileList;
		}
	}; // class InternalFriendlyWinApp

public:
	virtual void _ForceCancelCallbacs();

	static bool g_bShowStatusTipsOverDisabledItems;
	static bool g_bShowCoolTipsOverDisabledItems;

	static CExtSafeString g_strSpecSymbols;
	static bool g_bUseAcceleratedMenuScrolling;
	static INT g_nAcceleratedIncreaseStep;
	static INT g_nAcceleratedIncreasePercent;
	static INT g_nAcceleratedPixelsPerStepMax;
protected:
	INT m_nAcceleratedPixelsPerStepCurrent;
	INT m_nAcceleratedMilliSecondsPassed;

public:
	static bool g_bTranslateContextHelpCmd;
	static ACCEL g_accelContextHelp;

	virtual const ACCEL * _GetTranslatedContextHelpAccel() const;

	// is allowed menu item positioning without
	// using only & - marked text
	static bool g_bAllowNonAccelPositioning;

	friend class CExtPopupMenuSite;

public:
	DECLARE_DYNCREATE( CExtPopupMenuWnd );

	CExtPopupMenuWnd();

// Attributes
public:

	virtual int _GetCurIndex() const;
	void _DoShowChain( int nCmdShow = SW_SHOWNA );
	void _DoResetChainPositions();
	virtual bool OnQueryLayoutRTL() const;
protected:
	void _DoResetChainPositionsImpl(
		CExtPopupMenuWnd * pPrev
		);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
public:
	int m_nHelperDropIndexBefore;
	virtual void _SetDropIndexBefore(
		int nIndex = -1 // reset
		);
protected:
	bool m_bHelperNoHideChild:1, m_bHelperNoRemoveSrc:1, m_bHelperDragOverPassed:1;
	CPoint m_ptHelperDragStart;
	int m_nDragSrcIdx;
	CExtCustomizeCmdTreeNode * m_pNode, * m_pDragSrcNode;
	CExtCustomizeSite * m_pCustomizeSite;
public:
	CExtCustomizeCmdTreeNode * _CmdNodeGet()
	{
		ASSERT_VALID( this );
#ifdef _DEBUG
		if( m_pNode != NULL )
		{
			ASSERT_VALID( m_pNode );
		}
#endif // _DEBUG
		return m_pNode;
	}
	const CExtCustomizeCmdTreeNode * _CmdNodeGet() const
	{
		ASSERT_VALID( this );
//		if( _IsFadeOutAnimation() )
//			return NULL;
		const CExtPopupMenuWnd * pTLP = GetTopLevelMenuWnd();
		if( pTLP != NULL && pTLP->m_bEndSequenceInvoked )
			return NULL;
#ifdef _DEBUG
		if( m_pNode != NULL )
		{
			ASSERT_VALID( m_pNode );
		}
#endif // _DEBUG
		return m_pNode;
	}
	void _CmdNodeSet( CExtCustomizeCmdTreeNode * pNode )
	{
		ASSERT_VALID( this );
#ifdef _DEBUG
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
		}
#endif // _DEBUG
		m_pNode = pNode;
	}
	virtual CExtCustomizeSite * _FindCustomizeSite() const;
protected:
	// CExtCustomizeSite::ICustomizeDropTarget
	virtual DROPEFFECT OnCustomizeTargetOver(
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		CPoint point,
		DWORD dwKeyState
		);
	virtual void OnCustomizeTargetLeave();
	virtual bool OnCustomizeTargetDrop(
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		CPoint point,
		DROPEFFECT de
		);
	// CExtCustomizeSite::ICustomizeDropSource
	virtual void OnCustomizeSourceDragComplete(
		DROPEFFECT de,
		bool bCanceled,
		bool * p_bNoResetActiveItem
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
public:
	virtual INT _FindChildPopup( const CExtPopupMenuWnd * pPopup ) const;
	virtual bool _FindCustomizeMode() const;
protected:

	int m_nCurIndex, m_nDelayedFocusItemIndex,
		m_nLastMousePick;
	bool
		m_bTopLevel:1,
		m_bScrollingAvailable:1,
		m_bExpandAvailable:1,
		m_bExpandWasPressed:1,
		m_bSuspendTips:1,
		m_bDelayedHiding:1;
	UINT m_nWaitingExpandTickCount;
	DWORD m_dwTrackFlags;

	HWND m_hWndCmdReceiver;

	int m_nDyScrollOffset;
	CExtPopupMenuWnd * m_pWndParentMenu;

	CRect m_rcScrollTop, m_rcScrollBottom,
		m_rcExpandBtn, m_rcTearOffBtn,
		m_rcRibbonBtnOptions, m_rcRibbonBtnExit;
	int m_nScrollingDirection;

	UINT m_nLeftAreaWidth;

	bool m_bHelperMouseBtnDownOnStart:1;
	CPoint m_ptStartMousePos;
public:
	HWND m_hWndNotifyMenuClosed;
	bool m_bEndSequenceInvoked:1;

	bool _IsTopLevelPopup() const
	{
		ASSERT_VALID( this );
		return m_bTopLevel;
	}
	bool _IsScrollingAvailable() const
	{
		ASSERT_VALID( this );
		return m_bScrollingAvailable;
	}

	CExtPopupMenuWnd * GetParentMenuWnd()
	{
		ASSERT_VALID( this );
		return m_pWndParentMenu;
	}
	const CExtPopupMenuWnd * GetParentMenuWnd() const
	{
		ASSERT_VALID( this );
		return ( const_cast < CExtPopupMenuWnd * > ( this ) ) -> GetParentMenuWnd();
	}
	CExtPopupMenuWnd * GetTopLevelMenuWnd()
	{
		ASSERT_VALID( this );
		CExtPopupMenuWnd * pPopup = this;
		for( ; pPopup != NULL; )
		{
			ASSERT_VALID( pPopup );
			if( pPopup->_IsTopLevelPopup() )
				break;
			pPopup = pPopup->GetParentMenuWnd();
		}
		return pPopup;
	}
	const CExtPopupMenuWnd * GetTopLevelMenuWnd() const
	{
		ASSERT_VALID( this );
		return ( const_cast < CExtPopupMenuWnd * > ( this ) ) -> GetTopLevelMenuWnd();
	}
	virtual HWND GetCmdReceiverHWND();
	HWND GetCmdReceiverHWND() const;
	virtual bool IsNoHighlightIconArea();
	virtual bool IsSyncFullRowItems();
	virtual DWORD TrackFlagsGet() const;
	virtual void TrackFlagsSet(
		DWORD dwTrackFlags = 0
		);
	enum spec_buttons_index_t
	{
		IDX_NOTHING = -1,
		IDX_SCROLL_TOP = -2,
		IDX_SCROLL_BOTTOM = -3,
		IDX_EXPAND = -4,
		IDX_TEAROFF = -5,
		IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON = -6,
		IDX_RIBBON_FILE_MENU_EXIT_BUTTON = -7,
	};
// Operations
public:
	virtual BOOL LoadMenu( 
		HWND hWndCmdRecv,
		UINT nIDResource,
		bool bPopupMenu = true,
		bool bNoRefToCmdMngr = false
		);
	virtual HMENU ExportToMenu(
		BOOL bDeep = TRUE
		) const;
	virtual BOOL UpdateFromMenu( 
		HWND hWndCmdRecv,
		CMenu *pBuildMenu,
		bool bPopupMenu = true,
		bool bTopLevel = true,
		bool bNoRefToCmdMngr = false
		);
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	bool UpdateFromCmdTree(
		HWND hWndCmdRecv,
		CExtCustomizeCmdTreeNode * pNode,
		bool bTopLevel = true
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	static bool g_bMdiWindowsMenuUsesCheckInsteadOfRadio;
	virtual BOOL UpdateMdiWindowsMenu(
		CWnd * pWndStartSearchMdiFrameWnd = NULL
		);
	virtual void SetCmdTargetToAllItems(
		HWND hWndSpecCmdReceiver = NULL,
		bool bOnlyThisLevel = false
		);

	enum menu_item_type_t
	{
		TYPE_POPUP		= 0xFFFFFFFF,
		TYPE_SEPARATOR	= 0x00000000 // same as ID_SEPARATOR
	};

	virtual INT ItemGetCount() const;
	virtual UINT ItemGetCmdID( // menu_item_type_t values can be returned
		INT nPos
		) const;
	virtual CExtCmdIcon & ItemGetIcon( INT nPos );
	const CExtCmdIcon & ItemGetIcon( INT nPos ) const;
	virtual CExtSafeString ItemGetText( INT nPos ) const;
	virtual CExtSafeString ItemGetAccelText( INT nPos ) const;
	virtual INT ItemFindPosForCmdID(
		UINT nCmdID,
		INT nPosStart = -1
		) const;
	bool ItemSetPopupIcon(
		INT nPos,
		HICON hIcon = NULL // no icon by default
		);
	virtual bool ItemSetPopupIcon(
		INT nPos,
		const CExtCmdIcon & _icon
		);
	virtual bool ItemSetPopupText(
		INT nPos,
		__EXT_MFC_SAFE_LPCTSTR sText // NULL if empty
		);
	virtual bool ItemSetPopupAccelText(
		INT nPos,
		__EXT_MFC_SAFE_LPCTSTR sText // NULL if empty
		);
	virtual CExtPopupMenuWnd * ItemGetPopup(
		INT nPos
		);
	virtual const CExtPopupMenuWnd * ItemGetPopup(
		INT nPos
		) const;
	virtual bool ItemIsDisplayed(
		INT nPos
		) const;
	virtual void ItemSetDisplayed(
		INT nPos,
		bool bForceDisplayed = true
		);
	virtual BOOL ItemRemove(
		INT nPos = -1 // remove all
		);
	virtual INT ItemRemoveAllSeparators();
	virtual bool ItemPaletteRowWrapGet(
		INT nPos
		) const;
	virtual void ItemPaletteRowWrapSet(
		INT nPos,
		bool bWrap = true
		);
	virtual bool ItemToolLayoutGet(
		INT nPos
		) const;
	virtual void ItemToolLayoutSet(
		INT nPos,
		bool bToolButton = true
		);
	virtual bool ItemEnabledGet(
		INT nPos
		) const;
	virtual void ItemEnabledSet(
		INT nPos,
		bool bEnabled = true
		);
	virtual bool ItemBoldGet(
		INT nPos
		) const;
	virtual void ItemBoldSet(
		INT nPos,
		bool bBold = true
		);
	virtual bool ItemDefaultGet(
		INT nPos
		) const;
	virtual void ItemDefaultSet(
		INT nPos,
		bool bDefault = true
		);
	INT ItemDefaultFind() const;
	void ItemDefaultUnset();
	virtual bool ItemEnabledAppearanceGet(
		INT nPos
		) const;
	virtual void ItemEnabledAppearanceSet(
		INT nPos,
		bool bEnabledAppearance = true
		);
	BOOL ItemInsert(
		UINT nCmdID = TYPE_SEPARATOR,
		INT nPos = -1, // append
		__EXT_MFC_SAFE_LPCTSTR sText = NULL, // for TYPE_POPUP only
		HICON hIcon = NULL, // for TYPE_POPUP only
		HWND hWndSpecCmdReceiver = NULL // specific command receiver
		);
	virtual BOOL ItemInsert(
		UINT nCmdID,
		INT nPos, // -1 - append
		__EXT_MFC_SAFE_LPCTSTR sText, // for TYPE_POPUP only
		const CExtCmdIcon & icon, // for TYPE_POPUP only
		HWND hWndSpecCmdReceiver = NULL // specific command receiver
		);
	virtual BOOL ItemInsertCommand( // no references to command manager
		UINT nCmdID,
		INT nPos, // -1 append
		__EXT_MFC_SAFE_LPCTSTR sItemText, // may be NULL
		__EXT_MFC_SAFE_LPCTSTR sAccelText, // may be NULL
		const CExtCmdIcon & icon,
		int nCheck = 0, // 0 - no check, 1 - checked, 2 - indeterminate, 3 - radio
		HWND hWndSpecCmdReceiver = NULL
		);
	BOOL ItemInsertCommand( // no references to command manager
		UINT nCmdID = TYPE_SEPARATOR,
		INT nPos = -1, // append
		__EXT_MFC_SAFE_LPCTSTR sItemText = NULL,
		__EXT_MFC_SAFE_LPCTSTR sAccelText = NULL,
		HICON hIcon = NULL,
		bool bCopyIcon = true,
		int nCheck = 0, // 0 - no check, 1 - checked, 2 - indeterminate, 3 - radio
		HWND hWndSpecCmdReceiver = NULL
		);
	BOOL ItemInsertSpecPopup( // insert specific popup implementation
		CExtPopupMenuWnd * pSpecPopup,
		INT nPos = -1, // append
		__EXT_MFC_SAFE_LPCTSTR sText = NULL,
		HICON hIcon = NULL
		);
	virtual BOOL ItemInsertSpecPopup( // insert specific popup implementation
		CExtPopupMenuWnd * pSpecPopup,
		INT nPos, // -1 - append
		__EXT_MFC_SAFE_LPCTSTR sText,
		const CExtCmdIcon & icon
		);
	virtual INT ItemFindByAccessChar(
		__EXT_MFC_SAFE_TCHAR chrAccess,
		INT nStartIdx = -1,
		BOOL bRestartAt0 = TRUE,
		BOOL bSearchDisplayedOnly = TRUE
		) const;
	virtual INT ItemFindByText(
		__EXT_MFC_SAFE_LPCTSTR sText,
		INT nStartIdx = -1,
		BOOL bRestartAt0 = TRUE
		) const;

	virtual bool IsAllItemsRarelyUsed() const;
	virtual BOOL CreatePopupMenu( HWND hWndCmdRecv ); // alternative to LoadMenu()

	void SetLeftAreaWidth( UINT nWidth = 0 );
	UINT GetLeftAreaWidth() const;

	static CExtPopupMenuWnd * GetTrackingMenu();
	static void CancelMenuTracking();
	inline static BOOL IsKeyPressed(
		int nVirtKey,
		bool bAsync = false
		)
	{
		SHORT nKeyState = bAsync ? ( ::GetAsyncKeyState( nVirtKey ) ) : ( ::GetKeyState( nVirtKey ) );
		if( (nKeyState & (1 << (sizeof(SHORT)*8-1))) != 0 )
			return TRUE;
		return FALSE;
	}
	static BOOL IsCmdKeyActivation(
		LPARAM lParam = 0
		);
	static BOOL IsMenuTracking();

	virtual BOOL TrackPopupMenu(
		DWORD dwTrackFlags,
		int x,
		int y,
		LPCRECT lpRect = NULL,
		LPVOID pCbPaintCombinedCookie = NULL,
		pCbPaintCombinedContent pCbPCC = NULL,
		UINT * lpnResultCmdID = NULL,
		bool bCookieIsObject = false
		);

	struct __PROF_UIS_API MsgPrepareMenuData_t
	{
		bool m_bMenuChanged:1;
		bool m_bMenuCanceled:1;
		bool m_bOneLevelCall:1;
		CExtPopupMenuWnd * m_pPopup;

		MsgPrepareMenuData_t(
			CExtPopupMenuWnd * pPopup
			)
			: m_bMenuChanged( false )
			, m_bMenuCanceled( false )
			, m_bOneLevelCall( false )
			, m_pPopup( pPopup )
		{
			ASSERT( m_pPopup != NULL );
		}
		~MsgPrepareMenuData_t()
		{
		}

		operator WPARAM() const
		{
			return WPARAM( this );
		}

		LRESULT SendMessage(
			HWND hWnd,
			bool bOneLevelCall
			)
		{
			ASSERT( m_pPopup != NULL );
			m_bOneLevelCall = bOneLevelCall;
			ASSERT( hWnd != NULL );
			ASSERT( ::IsWindow(hWnd) );
			LRESULT lResult =
				::SendMessage(
					hWnd,
					m_bOneLevelCall
						? CExtPopupMenuWnd::g_nMsgPrepareOneMenuLevel
						: CExtPopupMenuWnd::g_nMsgPrepareMenu
						,
					WPARAM( *this ),
					LPARAM( m_pPopup )
					);
			
			return lResult;
		}
		LRESULT SendMessage(
			CWnd * pWnd,
			bool bOneLevelCall
			)
		{
			return
				SendMessage(
					pWnd->GetSafeHwnd(),
					bOneLevelCall
					);
		}
	}; // struct MsgPrepareMenuData_t

	static bool g_bFullScreenMode;
	static bool g_bMenuWithShadows; // allow shadows
	static bool g_bMenuExpanding; // allow hide rarely used
	static bool g_bMenuDelayExpanding; // show full menu after short delay
	static bool g_bMenuHighlightRarely; // display rarely used in different style
	static bool g_bMenuShowCoolTips; // allow display cool tooltips
	static bool g_bMenuExpandAnimation; // allow animation when show rarely used
	static bool g_bUseStretchOnExpandAnimation; // use StretchBlt() instead of BitBlt() for painting menu expand amimation
	static bool g_bMenuLargeIcons; // use large icons
	static bool g_bEatNcAreaClicks;
	static bool g_bMRU_UseFullPathsInMenu;
	static bool g_bMRU_UseFullPathsInTipTool;
	static CExtPopupMenuTipWnd::e_tip_style_t g_eTtsClassicMenu;
	static CExtPopupMenuTipWnd::e_tip_style_t g_eTtsPaletteMenu;
	static CExtPopupMenuTipWnd::e_tip_style_t g_eTtsColorMenu;
	static CExtPopupMenuTipWnd::e_tip_style_t g_eTtsTearOffCaption;

public:
	static void stat_paint_combined_menu_toolbtn(
		LPVOID pCookie,
		CDC & dc,
		const CWnd & refWndMenu,
		const CRect & rcExcludeArea, // in screen coords
		int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
		);

protected:
	CExtPopupMenuTipWnd m_wndToolTip;
	virtual CExtPopupMenuTipWnd & GetTip();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtPopupScreenTipWnd m_wndScreenTip;
	virtual CExtPopupScreenTipWnd & GetScreenTip();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
protected:
	virtual bool _CreateHelper(
		CWnd * pWndCmdReceiver
		);
	virtual bool _BuildItems(
		CMenu * pBuildMenu,
		bool bTopLevel,
		bool bNoRefToCmdMngr = false
		);
	virtual void _UpdateCmdUI();
	virtual void _Init();
	virtual void _RecalcLayoutImpl();
	static int _GetSpecBtnHeight();
public:
	virtual void _RecalcTrackParms(
		bool bMoveWindow = true
		);
	void _CachedRectsEmpty();
	virtual CRect OnQueryMenuBorderMetrics() const;
	virtual int _HitTest(
		const CPoint & point,
		bool * p_bInplaceControlArea = NULL,
		bool * p_bInplaceDropDownArea = NULL
		);
	virtual int _GetTearOffCaptionHeight() const;
	virtual bool _IsTearOff() const;
	virtual bool _IsPalette() const;
public:
	virtual void _DoPaint( CDC & dcPaint, bool bUseBackBuffer = true );
	virtual void _DoPaintMenuItems( CDC & dc );
	virtual void _DoPaintMenuItemsPalette( CDC & dc );
	virtual void _GetClientRect(RECT * pRectClient);
	static void PassMsgLoop(
		bool bEnableOnIdleCalls
		);

	struct __PROF_UIS_API ExpandEffectRects_t
	{
		CRect m_rcSrc,m_rcDst;
		ExpandEffectRects_t()
		{
			m_rcSrc.SetRectEmpty();
			m_rcDst.SetRectEmpty();
		}
		ExpandEffectRects_t(
			const RECT & rcSrc,
			const RECT & rcDst
			)
		{
			m_rcSrc = rcSrc;
			m_rcDst = rcDst;
		}
		ExpandEffectRects_t(
			const ExpandEffectRects_t & other
			)
		{
			m_rcSrc = other.m_rcSrc;
			m_rcDst = other.m_rcDst;
		}
		ExpandEffectRects_t &
			operator = (
				const ExpandEffectRects_t & other
				)
		{
			m_rcSrc = other.m_rcSrc;
			m_rcDst = other.m_rcDst;
			return * this;
		}
	}; // struct ExpandEffectRects_t

	typedef
		CArray < ExpandEffectRects_t, ExpandEffectRects_t & >
		expand_effect_rects_container_t;

	struct __PROF_UIS_API VisibleItemDefinition_t
	{
		INT m_nIndex;
		bool m_bRarelyUsed:1;
		bool m_bRarelyUsedPrev:1;
		bool m_bRarelyUsedNext:1;
		bool m_bHelperIsSeparator:1;
		bool m_bHelperIsPopup:1;
		UINT m_nHelperCmdID;
		CRect m_rcItem;
		CRect m_rcItemWithIndents;
		VisibleItemDefinition_t()
		{
			m_nIndex = -1;
			m_bRarelyUsed
				= m_bRarelyUsedPrev
				= m_bRarelyUsedNext
				= m_bHelperIsSeparator
				= m_bHelperIsPopup
				= false;
			m_nHelperCmdID = ID_SEPARATOR;
			m_rcItem.SetRectEmpty();
			m_rcItemWithIndents.SetRectEmpty();
		}
		VisibleItemDefinition_t(
			const VisibleItemDefinition_t & other
			)
		{
			AssignFromOther(other);
		}
		~VisibleItemDefinition_t()
		{
		}
		VisibleItemDefinition_t & operator = (
			const VisibleItemDefinition_t & other
			)
		{
			AssignFromOther(other);
			return * this;
		}
		void AssignFromOther(
			const VisibleItemDefinition_t & other
			)
		{
			m_nIndex = other.m_nIndex;
			m_bRarelyUsed = other.m_bRarelyUsed;
			m_bRarelyUsedPrev = other.m_bRarelyUsedPrev;
			m_bRarelyUsedNext = other.m_bRarelyUsedNext;
			m_bHelperIsSeparator = other.m_bHelperIsSeparator;
			m_bHelperIsPopup = other.m_bHelperIsPopup;
			m_nHelperCmdID = other.m_nHelperCmdID;
			m_rcItem = other.m_rcItem;
			m_rcItemWithIndents = other.m_rcItemWithIndents;
		}
		operator int() const
		{
			return m_nIndex;
		}
	}; // struct VisibleItemDefinition_t

	typedef
		CArray < VisibleItemDefinition_t, VisibleItemDefinition_t & >
		visible_items_t;
	virtual void _GetVisibleItems(
		HDC hDcTest, // may be NULL
		visible_items_t & v
		);
	virtual void _GetVisibleItemsPalette(
		HDC hDcTest, // may be NULL
		visible_items_t & v
		);
	virtual bool _IsPaletteItemWrap(
		int nIndex
		) const;

	static INT g_nTimeDelayedDisplaying;
	static INT g_nTimeDelayedHiding;
protected:
	virtual BOOL _TrackPopupMenu(
		DWORD dwTrackFlags,
		int x,
		int y,
		LPCRECT lpRect,
		LPVOID pCbPaintCombinedCookie = NULL,
		pCbPaintCombinedContent pCbPCC = NULL,
		bool bCookieIsObject = false
		);
public:

	static const UINT g_nMsgRibbonFileMenuButtonInvocation;
	struct __PROF_UIS_API RIBBONFILEMENUBUTTONINVOCATION
	{
		DWORD m_dwButtonType; // TPMX_RIBBON_EXIT_BUTTON or TPMX_RIBBON_OPTIONS_BUTTON
		HWND m_hWndTarget1, m_hWndTarget2;
		bool m_bQueryHandled:1;
		RIBBONFILEMENUBUTTONINVOCATION(
			DWORD dwButtonType,
			CExtPopupMenuWnd * pPopup
			);
		bool Notify();
		operator WPARAM() const;
		static RIBBONFILEMENUBUTTONINVOCATION * FromWPARAM( WPARAM wParam );
	};

	static const UINT g_nMsgRibbonFileMenuButtonQuery;
	struct __PROF_UIS_API RIBBONFILEMENUBUTTONQUERY
	{
		CExtPopupMenuWnd * m_pPopup;
		DWORD m_dwButtonType; // TPMX_RIBBON_EXIT_BUTTON or TPMX_RIBBON_OPTIONS_BUTTON
		CDC * m_pDC; // painting query if not NULL
		INT m_nMeasuredWidth;
		CRect m_rcButton;
		bool m_bHover:1, m_bPressed:1, m_bQueryHandled:1;
		CExtCmdIcon m_icon;
		CExtSafeString m_sText;
		RIBBONFILEMENUBUTTONQUERY(
			CExtPopupMenuWnd * pPopup,
			DWORD dwButtonType,
			CDC * pDC = NULL
			);
		bool Notify();
		operator WPARAM() const;
		static RIBBONFILEMENUBUTTONQUERY * FromWPARAM( WPARAM wParam );
	}; // struct RIBBONFILEMENUBUTTONQUERY

	static const UINT g_nMsgItemCoveringNotification;
	struct __PROF_UIS_API ITEMCOVERINGNOTIFICATON
	{
		// only one of the following is not NULL
		CExtPopupMenuWnd * m_pPopup;
		CExtBarButton * m_pTBB;
		// item index of pPopup or -1 for toolbar
		INT m_nPopupItemIndex;
		// event type
		enum eICN_t
		{
			__EICN_CANCEL	= 0, // unhover in toolbar, unfocus in menu
			__EICN_SET		= 1, // hover in toolbar, focus in menu
			__EICN_DELAY	= 2, // delay focus (in menu only)
		};
		eICN_t m_eICN;
		// pop-up menu specific for po-pup sub item only
		bool m_bEnableDropChild:1, m_bSelectAnyInChild:1;
		// flag that allows to cancel any conflicting tool tip windows
		bool m_bSuspendTips:1;
		// constructor, methods, operators
		ITEMCOVERINGNOTIFICATON(
			CExtPopupMenuWnd * pPopup,
			CExtBarButton * pTBB,
			eICN_t eICN,
			INT nPopupItemIndex = -1,
			bool bEnableDropChild = false,
			bool bSelectAnyInChild = false
			);
		bool Notify();
		operator WPARAM() const;
		static ITEMCOVERINGNOTIFICATON * FromWPARAM( WPARAM wParam );
	}; // struct ITEMCOVERINGNOTIFICATON

protected:
	bool m_bCanceling:1;
public:
	virtual void _CancelingSet();
	virtual bool _CancelingGet() const;

	struct __PROF_UIS_API ItemFocusTracker_t
	{
	protected:
		static CMap < CExtPopupMenuWnd *, CExtPopupMenuWnd *, INT, INT > g_map;
		const CExtPopupMenuWnd & m_Popup;
	public:
		ItemFocusTracker_t( const CExtPopupMenuWnd & _Popup );
		ItemFocusTracker_t( const CExtPopupMenuWnd * pPopup );
		virtual ~ItemFocusTracker_t();
		virtual void FocusChangingRegister();
		virtual void FocusChangingUnRegister();
		virtual bool FocusChangingIsRegistered();
		static bool FocusChangingIsInProgress( const CExtPopupMenuWnd * pPopup );
	}; // struct ItemFocusTracker_t
	virtual bool _ItemFocusChangingIsInProgress() const;
	virtual void _ItemFocusCancel(
		BOOL bRepaint,
		BOOL bCancelTip = TRUE,
		BOOL bDelayHiding = FALSE
		);
	virtual void _ItemFocusDelay(
		int nItemIndex = IDX_NOTHING,
		bool * p_bFocusAreadySet = NULL
		);
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
protected:
	CExtCustomizeCmdKeyTip m_keyTipChain;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
protected:
	CExtPopupKeyTipWnd m_wndKeyTipOptionsButton, m_wndKeyTipExitButton;
public:
	CExtCustomizeCmdKeyTip m_keyTipOptionsButton, m_keyTipExitButton;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
public:
	bool _KeyTipsEnabledGet() const;
	void _KeyTipsShow( bool bShow );
	bool _KeyTipsTranslate(
		DWORD dwKeyCode,
		INT * p_nIndex = NULL
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
public:
	virtual void _ItemFocusSet(
		int nCurIndex,
		BOOL bEnableDropChild,
		BOOL bRepaint,
		BOOL bSelectAnyInChild = TRUE,
		BOOL bDelayHiding = FALSE // delay hiding previous
		);
	virtual void _GetItemRect(
		int nIndex,
		RECT & rectItem,
		bool bIncludeIndents = true
		);
	virtual void _GetItemRectPalette(
		int nIndex,
		RECT & rectItem,
		bool bIncludeIndents = true
		);
public:
	virtual bool _OnMouseWheel(
		WPARAM wParam,
		LPARAM lParam,
		bool & bNoEat
		);
	virtual bool _OnMouseMove(
		UINT nFlags,
		CPoint point,
		bool & bNoEat
		);
	virtual bool _OnMouseClick(
		UINT nFlags,
		CPoint point,
		bool & bNoEat
		);
	virtual bool _OnKeyDown(
		UINT nChar,
		UINT nRepCnt,
		UINT nFlags,
		bool & bNoEat
		);

	virtual bool _CanStartLevelTracking();

protected:
	virtual void _FreeWinObjects();
	virtual void _OnCancelMode(
		bool bNcDestroy = false
		);
	virtual bool _PtInWndArea( CPoint ptClient );


public:
	virtual void _SetCapture();
	virtual CExtPopupMenuWnd * _GetCapture();
	virtual void _ReleaseCapture();
protected:

	virtual int _GetMaxScrollPos();
	virtual bool _StartScrolling(int nButtonIndex);

public:
	virtual void _SyncItems();
	virtual void _SyncItemsPalette();
	virtual void ResetPaletteWrapping(
		bool bCreateToolBarWrapping = false,
		int nIndexFirst = 0,
		int nIndexLast = -1
		);
	static const UINT g_nMsgTearOff;
	static const UINT g_nMsgCustomFloatPos;
	enum TEAROFFNOTIFICATONTYPE
	{
	__ETONT_BEGIN  = 0,
	__ETONT_UPDATE = 1,
	__ETONT_FINISH = 2,
	};
	class __PROF_UIS_API TEAROFFNOTIFICATON : public CExtHookSink
	{
	public:
		HWND m_hWndTearOff, m_hWndHelperPopup, m_hWndHelperFloat;
		CExtPopupMenuWnd * m_pPopup;
		CExtControlBar * m_pHelperTearOffBar;
		CPoint m_ptScreenStartPos;
		CSize m_sizeTearOffArea;
		CRect m_rcHelperScreenStartArea, m_rcCustomFloatPos;
		bool m_bHelperMenuAreasUpdated:1,
			m_bHelperForceFloatInvisible:1,
			m_bCanceled:1, m_bFinished:1;
		static TEAROFFNOTIFICATON * g_pCurrentTEAROFFNOTIFICATON;
		TEAROFFNOTIFICATONTYPE m_eTONT;
		TEAROFFNOTIFICATON(
			CExtPopupMenuWnd * pPopup,
			const POINT & ptScreenStartPos
			);
		~TEAROFFNOTIFICATON();
		virtual bool Notify();
		virtual bool DoDefaultTearOff();
		virtual void DoStep(
			bool bFinalStep = false
			);
		virtual bool PtInTearOffArea( const POINT & ptScreen ) const;
		operator WPARAM() const;
		static TEAROFFNOTIFICATON * FromWPARAM( WPARAM wParam );
		virtual bool _OnMouseWheel(
			WPARAM wParam,
			LPARAM lParam
			);
		virtual bool _OnMouseMove(
			UINT nFlags,
			CPoint point
			);
		virtual bool _OnMouseClick(
			UINT nFlags,
			CPoint point
			);
		virtual bool _OnKeyDown(
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags
			);
		virtual bool OnHookWndMsg(
			LRESULT & lResult,
			HWND hWndHooked,
			UINT nMessage,
			WPARAM & wParam,
			LPARAM & lParam
			);
	};
	virtual void _DoReCreateShadows();
	virtual bool _IsFloatingAreaInstersectsWithMenu(
		const RECT & rcFloatingArea,
		bool bAnalyzeParents = true
		) const;
protected:
	virtual void _DoTearOff();
	virtual void _DoExpand();
	virtual bool _CoolTipIsVisible();
	virtual void _CoolTipHide(
		bool bAdvOperation = true
		);
	virtual CExtPopupMenuTipWnd::e_tip_style_t _CoolTipGetStyle() const;

	CRgn m_rgnWnd;
	CRect m_rcClient;

public:
	virtual CRect _CalcTrackRect();
	virtual CSize _CalcTrackSize();

	virtual bool _IsPopupWithShadows() const;
	virtual bool _IsPopupWithShadowsDynamic() const;
	virtual bool _IsRibbonMode() const;
	virtual void _AdjustAnimation( e_animation_type_t & eAT );
	virtual bool _IsFadeOutAnimation() const;

	enum next_item_t
	{
		__NI_NOTHING	= -100,
		__NI_ANY		= -101,
		__NI_NEXT		= -102,
		__NI_PREV		= -103,
		__NI_ROW_NEXT	= -104,
		__NI_ROW_PREV	= -105,
		__NI_PAGE_UP	= -106,
		__NI_PAGE_DOWN	= -107,
	};
	
	virtual int _GetNextItem(
		next_item_t nit
		);

public:
	
	/////////////////////////////////////////////////////////////////////////////
	// CExtPopupMenuWnd::CInPlaceEditWnd

	class CInPlaceEditWnd;

	typedef
		bool (*pCbInplaceEditWndProc)(
			LRESULT & lResult,
			UINT message,
			WPARAM wParam,
			LPARAM lParam,
			CInPlaceEditWnd & wndEdit,
			LPVOID pCookie
			);
	typedef
		bool (*pCbVerifyTextInput)(
			CInPlaceEditWnd & wndEdit,
			LPVOID pCookie,
			__EXT_MFC_SAFE_LPCTSTR sTextOld,
			__EXT_MFC_SAFE_LPCTSTR sTextNew
			);
	typedef
		void (*pCbPutTextInputResult)(
			CInPlaceEditWnd & wndEdit,
			LPVOID pCookie,
			__EXT_MFC_SAFE_LPCTSTR sTextNew
			);

	class __PROF_UIS_API CInPlaceEditWnd : public CEdit
	{
		pCbVerifyTextInput m_pCbVerifyTextInput;
		pCbPutTextInputResult m_pCbPutTextInputResult;
		pCbInplaceEditWndProc m_pCbWndProc;
		LPVOID m_pCbCookie;
		CExtSafeString * m_pStr;
		CExtSafeString m_sTextSaved;
		bool m_bNoCancelAtKillFocus:1;
	public:
		static const UINT g_nMsgDeleteSelection;
		static const UINT g_nMsgSelectAll;
		CInPlaceEditWnd(
			CExtSafeString * pStr,
			pCbVerifyTextInput pCbVerify = NULL,
			pCbPutTextInputResult pCbResult = NULL,
			pCbInplaceEditWndProc pCbWndProc = NULL,
			LPVOID pCbCookie = NULL
			);
		~CInPlaceEditWnd();
#ifdef _DEBUG
		virtual void AssertValid() const;
#endif // _DEBUG
		virtual bool Create(
			CExtPopupMenuWnd * pPopup,
			CRect rc,
			UINT nDlgCtrlID
			);
		virtual LRESULT WindowProc(
			UINT message,
			WPARAM wParam,
			LPARAM lParam
			);
	protected:
		virtual void PostNcDestroy();
		CExtPopupMenuWnd * GetPopupMenu();
	}; // class CInPlaceEditWnd


	/////////////////////////////////////////////////////////////////////////////
	// CExtPopupMenuWnd::MENUITEMDATA
	
	class MENUITEMDATA;
	friend class MENUITEMDATA;
	class CExtPopupMenuCmdUI;

	struct __PROF_UIS_API INPLACEEDITCREATEINFO
	{
		CWnd ** m_ppInplaceEdit;
		CExtPopupMenuWnd * m_pPopup;
		MENUITEMDATA * m_pMenuItemData;
		CExtSafeString * m_pInplaceEditStr;
		pCbVerifyTextInput m_pCbVerifyTextInput;
		pCbPutTextInputResult m_pCbPutTextInputResult;
		pCbInplaceEditWndProc m_pInplaceEditCbWndProc;
		LPVOID m_pInplaceEditCbCookie;
		CRect m_rc;
		INPLACEEDITCREATEINFO(
			CWnd ** ppInplaceEdit,
			CExtPopupMenuWnd * pPopup,
			MENUITEMDATA * pMenuItemData,
			CExtSafeString * pInplaceEditStr,
			pCbVerifyTextInput pCbVerify,
			pCbPutTextInputResult pCbResult,
			pCbInplaceEditWndProc pInplaceEditCbWndProc,
			LPVOID pInplaceEditCbCookie,
			const RECT & rc
			)
			: m_ppInplaceEdit( ppInplaceEdit )
			, m_pPopup( pPopup )
			, m_pMenuItemData( pMenuItemData )
			, m_pInplaceEditStr( pInplaceEditStr )
			, m_pCbVerifyTextInput( pCbVerify )
			, m_pCbPutTextInputResult( pCbResult )
			, m_pInplaceEditCbWndProc( pInplaceEditCbWndProc )
			, m_pInplaceEditCbCookie( pInplaceEditCbCookie )
			, m_rc( rc )
		{
		}
	}; // struct INPLACEEDITCREATEINFO

	class __PROF_UIS_API MENUITEMDATA
	{
	public:
		typedef bool (*pCbItemClick)(
			CExtPopupMenuWnd * pPopup,
			MENUITEMDATA * pItemData
			);
	protected:
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeCmdTreeNode * m_pCmdNode;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtPopupMenuWnd * m_pWndChild;
		int m_nItemIndex;
		
		UINT m_nCmdID; // can be ID_SEPARATOR
		CExtSafeString m_sItemText, m_sAccelText,
			m_sCustomTipText, m_sExtendedText;
		CExtCmdIcon m_iconPopup;

		CSize m_sizeItem;
		CRect m_rcCacheBasic, m_rcCacheIndents;
		INT m_nIconAreaWidth;
		INT m_nMarginLeft, m_nMarginTop, m_nMarginRight, m_nMarginBottom;
		bool m_bSelected:1, m_bDisplayed:1, m_bForceDisplayed:1,
			m_bChildCombine:1, m_bToolButton:1, m_bToolWrap:1, m_bBold:1,
			m_bForcePopupDisabled:1, m_bForceEnabled:1, m_bForceNoLeftGradient:1,
			m_bEnabledAppearance:1, m_bBigAccent:1,
			m_bCheck:1, m_bRadio:1, m_bEnabled: 1,m_bIndeterminate:1,
			m_bXtraChecked:1, m_bNoCmdUI:1, m_bIconMode:1, m_bDefault:1;
		pCbItemClick m_pCbCmdDeliver;
		pCbItemClick m_pCbXtraMarkState;
		/*__EXT_MFC_SAFE_TCHAR*/ INT m_cAccelChar;

		HWND m_hWndSpecCmdReceiver;
		
		CWnd * m_pHelperInplaceEditWnd;
		CExtSafeString * m_pInplaceEditStr;
		pCbVerifyTextInput m_pCbVerifyTextInput;
		pCbPutTextInputResult m_pCbPutTextInputResult;
		pCbInplaceEditWndProc m_pInplaceEditCbWndProc;
		LPVOID m_pInplaceEditCbCookie;
		int m_nInplaceEditWidth;
		bool m_bAllowInplaceEditActivation:1;

		bool m_bTempSelTextValid:1;
		CExtSafeString m_sTempSelText;

		LPARAM m_nLParam;

		CExtPopupMenuWnd * m_pOwner;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtPopupKeyTipWnd * m_pWndKeyTipBasic, * m_pWndKeyTipDD;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	public:
		MENUITEMDATA(
			CExtPopupMenuWnd * pOwner = NULL
			);
		MENUITEMDATA( const MENUITEMDATA & other );
		~MENUITEMDATA();
		MENUITEMDATA & operator = ( const MENUITEMDATA & other );
	private:
		void AssignFromOther( const MENUITEMDATA & other );
	public:
		CExtPopupMenuWnd * GetOwner();
		const CExtPopupMenuWnd * GetOwner() const;
		void SetOwner( CExtPopupMenuWnd * pOwner );
		bool UpdateFromMenu(
			HWND hWndCmdRecv,
			CMenu * pTrackMenu,
			int nItemIndex,
			bool bNoRefToCmdMngr = false
			);
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		bool UpdateFromCmdTree(
			HWND hWndCmdRecv,
			CExtCustomizeCmdTreeNode * pNode,
			int nItemIndex,
			CExtPopupMenuWnd * pPopupParent = NULL
			);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	public:
		void AccelCharInit();
		__EXT_MFC_SAFE_TCHAR AccelCharGet() const;
		bool AccelCharIsSet() const;

	protected:
		bool UpdateCmdManagerCommand(
			CExtCmdItem * pCmdItem,
			int nItemIndex
			);
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	protected:
		CExtCustomizeCmdKeyTip * m_pCustomKeyTipInfoBasic, * m_pCustomKeyTipInfoDD;
	public:
		CExtCustomizeCmdKeyTip * KeyTipGetInfo(
			bool bBasic
			);
		CExtSafeString KeyTipGetText(
			INT nIndent,
			bool bBasic
			);
		CWnd * KeyTipGetParentWnd();
		CPoint KeyTipGetGuideLines(
			bool bBasic
			);
 		CExtPopupKeyTipWnd * KeyTipGetWnd(
			bool bBasic
			);
		void KeyTipTrackingQuery(
			bool bShow,
			CExtCustomizeCmdKeyTip * pKeyTipChain,
			HDWP & hPassiveModeDWP
			);
		bool KeyTipTranslate(
			DWORD dwKeyCode,
			CExtCustomizeCmdKeyTip * pKeyTipChain
			);
		void KeyTipDisplay(
			CExtCustomizeCmdKeyTip & keyTipChain
			);
		bool KeyTipInvokeAction(
			bool bBasic
			);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	public:
		void MeasureItem(
			CExtPaintManager * pPM
			);

		CExtCmdItem * GetCmd() const;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeCmdTreeNode * GetCmdNode();
		const CExtCustomizeCmdTreeNode * GetCmdNode() const
		{
			return
				( const_cast < MENUITEMDATA * > ( this ) )
					-> GetCmdNode();
		}
		void SetCmdNode( CExtCustomizeCmdTreeNode * pNode );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

		HWND GetCmdReceiver() const
		{
			ASSERT( m_hWndSpecCmdReceiver != NULL );
			ASSERT( ::IsWindow(m_hWndSpecCmdReceiver) );
			return m_hWndSpecCmdReceiver;
		}
		void SetCmdReceiver( HWND hWndSpecCmdReceiver )
		{
			ASSERT( hWndSpecCmdReceiver != NULL );
			ASSERT( ::IsWindow(hWndSpecCmdReceiver) );
			m_hWndSpecCmdReceiver = hWndSpecCmdReceiver;
		}

		void SetDisplayed( bool bDisplayed )
		{
			m_bDisplayed = bDisplayed;
		}
		void SetForceDisplayed( bool bForceDisplayed )
		{
			m_bForceDisplayed = bForceDisplayed;
		}
	public:
		void SetOuterMargins(
			INT nMarginLeft = -1,
			INT nMarginTop = -1,
			INT nMarginRight = -1,
			INT nMarginBottom = -1
			);
		void SetOuterMargins(
			const RECT & rcMargins
			);
		void GetOuterMargins(
			INT * p_nMarginLeft = NULL,
			INT * p_nMarginTop = NULL,
			INT * p_nMarginRight = NULL,
			INT * p_nMarginBottom = NULL
			) const;
		void GetOuterMargins(
			RECT & rcMargins
			) const;
		CRect GetOuterMargins() const;

		bool IsDisplayed() const
		{
			return m_bDisplayed || m_bForceDisplayed;
		}
		bool IsForceDisplayed()
		{
			return m_bForceDisplayed;
		}
		bool IsAllItemsRarelyUsed() const
		{
			ASSERT( IsPopup() );
			ASSERT( m_pWndChild != NULL );
			if( IsNoCmdUI() )
				return false;
			return m_pWndChild->IsAllItemsRarelyUsed();
		}

		bool IsForcePopupDisabled() const
		{
			return m_bForcePopupDisabled;
		}
		void SetForcePopupDisabled( bool bForcePopupDisabled = true )
		{
			if(		(m_bForcePopupDisabled && bForcePopupDisabled)
				||	((!m_bForcePopupDisabled) && (!bForcePopupDisabled))
				)
				return;
			m_bForcePopupDisabled = bForcePopupDisabled;
		}

		bool IsEnabled() const
		{
			//if( IsPopup() && GetCmd() == NULL )
			//	return true;
			if( IsPopup() )
			{
				if( IsForcePopupDisabled() )
					return false;
				if( GetCmd() == NULL )
					return true;
			}
			if( IsSeparator() )
				return false;
			if( IsForceEnabled() )
				return true;
			return m_bEnabled;
		}
		bool Enable( bool bOn )
		{
//			if( IsPopup() )
//				return true;
			if( IsSeparator() )
				return false;
			bool bRetVal = m_bEnabled;
			m_bEnabled = bOn;
			return bRetVal;
		}
		bool IsIndeterminated() const
		{
			if( IsPopup() || IsSeparator() )
				return false;
			return m_bIndeterminate;
		}
		bool Indeterminate( bool bOn )
		{
			bool bRetVal = m_bIndeterminate;
			m_bIndeterminate = bOn;
			return bRetVal;
		}

		bool IsSelected() const
		{
			if( /*IsPopup() ||*/ IsSeparator() )
				return false;
			return m_bSelected;
		}
		void SetSelected( bool bOn = true )
		{
			m_bSelected = bOn;
		}
	public:
		bool GetRadio() const
		{
			if( /*IsPopup() ||*/ IsSeparator() )
				return false;
			return m_bRadio;
		}
		bool Radio( bool bOn )
		{
			bool bRetVal = m_bRadio;
			m_bRadio = bOn;
			return bRetVal;
		}

		bool GetCheck() const
		{
			if( /*IsPopup() ||*/ IsSeparator() )
				return false;
			return m_bCheck;
		}
		bool Check( bool bOn )
		{
			bool bRetVal = m_bCheck;
			m_bCheck = bOn;
			return bRetVal;
		}

		pCbItemClick GetCmdDeliverCb() const
		{
			if( IsSeparator() )
				return NULL;
			return (pCbItemClick)m_pCbCmdDeliver;
		}
		void SetCmdDeliverCb( pCbItemClick pCb )
		{
			if( IsSeparator() )
				return;
			m_pCbCmdDeliver = pCb;
		}

		bool IsExtraMark() const
		{
			if( IsSeparator() )
				return false;
			return (m_pCbXtraMarkState != NULL) ? true : false;
		}
		pCbItemClick GetExtraMarkCallback() const
		{
			return (pCbItemClick)m_pCbXtraMarkState;
		}
		void SetExtraMark( pCbItemClick pCb )
		{
			if( IsSeparator() )
				return;
			m_pCbXtraMarkState = pCb;
		}
		bool IsExtraChecked() const
		{
			if( !IsExtraMark() )
				return false;
			return m_bXtraChecked;
		}
		void SetExtraChecked( bool bOn )
		{
			if( !IsExtraMark() )
				return;
			m_bXtraChecked = bOn;
		}
		
		bool IsNoCmdUI() const
		{
			return m_bNoCmdUI;
		}
		void SetNoCmdUI( bool bOn )
		{
			m_bNoCmdUI = bOn;
		}

		bool IsIconMode() const
		{
			return m_bIconMode;
		}
		void SetIconMode( bool bOn )
		{
			m_bIconMode = bOn;
		}

		LPARAM LParamGet() const
		{
			return m_nLParam;
		}
		void LParamSet( LPARAM nLParam )
		{
			m_nLParam = nLParam;
		}

		__EXT_MFC_SAFE_LPCTSTR GetText() const;
		void SetText( __EXT_MFC_SAFE_LPCTSTR strText );
		__EXT_MFC_SAFE_LPCTSTR GetAccelText() const;
		void SetAccelText( __EXT_MFC_SAFE_LPCTSTR strText );
		
		CExtCmdIcon * GetIconPtr();
		const CExtCmdIcon * GetIconPtr() const;
		CExtCmdIcon & GetIcon();
		const CExtCmdIcon & GetIcon() const;

		void SetCustomTip( __EXT_MFC_SAFE_LPCTSTR sTip );
		__EXT_MFC_SAFE_LPCTSTR GetCustomTip() const;
		void GetTip( CExtSafeString & sTip ) const;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		bool InitSceenTip( CExtPopupScreenTipWnd & _wndScreenTip );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

		void SetExtendedText( __EXT_MFC_SAFE_LPCTSTR sExtendedText );
		__EXT_MFC_SAFE_LPCTSTR GetExtendedText() const;

		bool IsSeparator() const
		{
			bool bSeparator =
				(m_nCmdID==TYPE_SEPARATOR) ? true : false;
			return bSeparator;
		}

		bool IsChildCombine() const
		{
			return m_bChildCombine;
		}
		void SetChildCombine( bool bChildCombine = true )
		{
			m_bChildCombine = bChildCombine;
		}

		bool IsToolButton() const
		{
			return m_bToolButton;
		}
		void SetToolButton( bool bToolButton = true )
		{
			if(		(m_bToolButton && bToolButton)
				||	((!m_bToolButton) && (!bToolButton))
				)
				return;
			m_bToolButton = bToolButton;
			MeasureItem( NULL );
		}

		bool IsToolWrap() const
		{
			return m_bToolWrap;
		}
		void SetToolWrap( bool bToolWrap = true )
		{
			m_bToolWrap = bToolWrap;
		}

		bool IsBold() const
		{
			return m_bBold;
		}
		void SetBold( bool bBold = true )
		{
			if( m_bBold == bBold )
				return;
			m_bBold = bBold;
			MeasureItem( NULL );
		}

		bool IsDefault() const
		{
			return m_bDefault;
		}
		void SetDefault( bool bDefault = true )
		{
			m_bDefault = bDefault;
		}

		bool IsEnabledAppearance() const
		{
			return m_bEnabledAppearance;
		}
		void SetEnabledAppearance( bool bEnabledAppearance = true )
		{
			m_bEnabledAppearance = bEnabledAppearance;
		}

		bool IsBigAccent() const
		{
			return m_bBigAccent;
		}
		void SetBigAccent( bool bBigAccent = true )
		{
			m_bBigAccent = bBigAccent;
		}

		bool IsForceEnabled() const
		{
			return m_bForceEnabled;
		}
		void SetForceEnabled( bool bForceEnabled = true )
		{
			if(		(m_bForceEnabled && bForceEnabled)
				||	((!m_bForceEnabled) && (!bForceEnabled))
				)
				return;
			m_bForceEnabled = bForceEnabled;
		}

		bool IsForceNoLeftGradient() const
		{
			return m_bForceNoLeftGradient;
		}
		void SetForceNoLeftGradient( bool bForceNoLeftGradient = true )
		{
			if(		(m_bForceNoLeftGradient && bForceNoLeftGradient)
				||	((!m_bForceNoLeftGradient) && (!bForceNoLeftGradient))
				)
				return;
			m_bForceNoLeftGradient = bForceNoLeftGradient;
			MeasureItem( NULL );
		}

		bool IsInplaceEdit() const
		{
			return (m_pInplaceEditStr != NULL) ? true : false;
		}
		void SetInplaceEdit(
			CExtSafeString * pInplaceEditStr,
			pCbVerifyTextInput pCbVerify = NULL,
			pCbPutTextInputResult pCbResult = NULL,
			pCbInplaceEditWndProc pInplaceEditCbWndProc = NULL,
			LPVOID pInplaceEditCbCookie = NULL,
			int nInplaceEditWidth = __EXT_MENU_DEF_INPLACE_EDIT_WIDTH
			)
		{
			ASSERT( pInplaceEditStr != NULL );
			ASSERT( nInplaceEditWidth > 0 );
			m_pInplaceEditStr = pInplaceEditStr;
			m_pCbVerifyTextInput = pCbVerify;
			m_pCbPutTextInputResult = pCbResult;
			m_pInplaceEditCbWndProc = pInplaceEditCbWndProc;
			m_pInplaceEditCbCookie = pInplaceEditCbCookie;
			m_nInplaceEditWidth = nInplaceEditWidth;
			MeasureItem( NULL );
		}
		void ResetActiveInplaceEditPtr()
		{
			m_pHelperInplaceEditWnd = NULL;
		}
		CWnd * GetInplaceEditPtr()
		{
			if( !IsInplaceEdit() )
				return NULL;
			return m_pHelperInplaceEditWnd;
		}
		int GetInplaceEditWidth() const
		{
			if( !IsInplaceEdit() )
				return 0;
			ASSERT( m_nInplaceEditWidth > 0 );
			return m_nInplaceEditWidth;
		}
		bool IsAllowInplaceEditActivation() const
		{
			if( !IsInplaceEdit() )
				return false;
			return m_bAllowInplaceEditActivation;
		}
		void AllowInplaceEditActivation(
			bool bAllowInplaceEditActivation
			)
		{
			m_bAllowInplaceEditActivation = bAllowInplaceEditActivation;
		}
		CExtSafeString GetInplaceEditText() const
		{
			CExtSafeString s( _T("") );
			if(		IsInplaceEdit()
				&&	m_pInplaceEditStr != NULL
				)
			{
				if( m_bTempSelTextValid )
					s = m_sTempSelText;
				else
					s = *m_pInplaceEditStr;
			}
			return s;
		}
		void SetTempSelectedInplaceEditText(
			__EXT_MFC_SAFE_LPCTSTR sTempSelText = NULL // NULL - reset
			)
		{
			m_bTempSelTextValid = false;
			m_sTempSelText.Empty();
			if(		(!IsInplaceEdit())
				||	sTempSelText == NULL
				)
				return;
			m_bTempSelTextValid = true;
			m_sTempSelText = sTempSelText;
		}
		bool CreateInplaceEdit(
			CExtPopupMenuWnd * pPopup,
			CRect rc
			)
		{
			if( m_pInplaceEditStr == NULL )
			{
				ASSERT( FALSE );
				return false;
			}
			m_pHelperInplaceEditWnd = NULL;
			HWND hWndCmdReceiver = GetCmdReceiver();
			if(		hWndCmdReceiver != NULL
				&&	::IsWindow( hWndCmdReceiver )
				)
			{
				CExtPopupMenuWnd::INPLACEEDITCREATEINFO _ieci(
					&m_pHelperInplaceEditWnd,
					pPopup,
					this,
					m_pInplaceEditStr,
					m_pCbVerifyTextInput,
					m_pCbPutTextInputResult,
					m_pInplaceEditCbWndProc,
					m_pInplaceEditCbCookie,
					rc
					);
				::SendMessage(
					hWndCmdReceiver,
					CExtPopupBaseWnd::g_nMsgCreateInplaceEdit,
					(WPARAM)(&_ieci),
					(LPARAM)0L
					);
			}
			
			if( m_pHelperInplaceEditWnd == NULL )
			{
				m_pHelperInplaceEditWnd =
					new CInPlaceEditWnd(
						m_pInplaceEditStr,
						m_pCbVerifyTextInput,
						m_pCbPutTextInputResult,
						m_pInplaceEditCbWndProc,
						m_pInplaceEditCbCookie
						);
				if( !((CInPlaceEditWnd*)m_pHelperInplaceEditWnd)->
						Create( pPopup, rc, GetCmdID() ) )
				{
					ASSERT( FALSE );
					delete m_pHelperInplaceEditWnd;
					m_pHelperInplaceEditWnd = NULL;
					return false;
				}
			} // if( m_pHelperInplaceEditWnd == NULL )
#ifdef _DEBUG
			else
			{
				ASSERT( m_pHelperInplaceEditWnd->GetSafeHwnd() != NULL );
			} // else from if( m_pHelperInplaceEditWnd == NULL )
#endif // _DEBUG

			return true;
		}

		CRect AdjustInplaceEditRect(
			const RECT & rcItem,
			bool bRTL
			) const;
		
	protected:
		void SetSeparator(
			int nItemIndex
			)
		{
			if( !IsSeparator() )
			{
				if( m_pWndChild != NULL )
					DestroyPopup();
				ASSERT( m_pWndChild == NULL );
				m_sItemText.Empty();
				m_nCmdID = TYPE_SEPARATOR;
				m_nItemIndex = nItemIndex;
			}
			MeasureItem( NULL );
		}

		void SetPopup(
			int nItemIndex,
			__EXT_MFC_SAFE_LPCTSTR sText = NULL,
			HICON hIcon = NULL,
			CExtPopupMenuWnd * pSpecPopup = NULL
			)
		{
			CExtCmdIcon icon;
			if( hIcon != NULL )
				icon.AssignFromHICON( hIcon, false );
			SetPopup(
				nItemIndex,
				sText,
				icon,
				pSpecPopup
				);
		}
		void SetPopup(
			int nItemIndex,
			__EXT_MFC_SAFE_LPCTSTR sText,
			const CExtCmdIcon & icon,
			CExtPopupMenuWnd * pSpecPopup = NULL
			)
		{
			ASSERT( m_hWndSpecCmdReceiver != NULL );
			ASSERT( ::IsWindow(m_hWndSpecCmdReceiver) );

			if( m_pWndChild == NULL )
			{
				if( pSpecPopup == NULL )
				{
					ConstructPopup();
				} // if( pSpecPopup == NULL )
				else
				{
					m_pWndChild = pSpecPopup;
					ASSERT( m_pWndChild != NULL );
					m_pWndChild->m_hWndCmdReceiver = m_hWndSpecCmdReceiver;
				} // else from if( pSpecPopup == NULL )
			}
			
			m_nCmdID = (UINT)TYPE_POPUP;
			m_sItemText = _T("");
			m_sAccelText = _T("");
			m_nItemIndex = nItemIndex;
			ASSERT( m_iconPopup.IsEmpty() );
			VERIFY( SetPopupIcon( icon ) );
			VERIFY( SetPopupText(sText) );
			MeasureItem( NULL );
		}
	public:
		void SetSpecPopup(
			CExtPopupMenuWnd * pSpecPopup
			)
		{
			ASSERT_VALID( pSpecPopup );
			m_nCmdID = (UINT)TYPE_POPUP;
			m_pWndChild = pSpecPopup;
			m_pWndChild->m_hWndCmdReceiver = m_hWndSpecCmdReceiver;
		}
		bool SetPopupIcon( HICON hIcon, bool bCopyIcon = true )
		{
			if( ! ( IsPopup() || IsNoCmdUI() ) )
			{
				ASSERT( FALSE );
				return false;
			}
			if( hIcon == NULL )
			{
				m_iconPopup.Empty();
				return true;
			}
			m_iconPopup.AssignFromHICON( hIcon, bCopyIcon );
			return true;
		}
		bool SetPopupIcon( const CExtCmdIcon & icon )
		{
			if( ! ( IsPopup() || IsNoCmdUI() ) )
			{
				ASSERT( FALSE );
				return false;
			}
			if( icon.IsEmpty() )
			{
				m_iconPopup.Empty();
				return true;
			}
			m_iconPopup = icon;
			return true;
		}
		CExtCmdIcon & GetPopupIcon()
		{
			return m_iconPopup;
		}
		const CExtCmdIcon & GetPopupIcon() const
		{
			return m_iconPopup;
		}
		bool SetPopupText( __EXT_MFC_SAFE_LPCTSTR strText );
		bool SetPopupAccelText( __EXT_MFC_SAFE_LPCTSTR strText ); // NULL if empty
	protected:
		bool ConstructPopup();
		void DestroyPopup();
	public:
		CExtPopupMenuWnd * GetPopup()
		{
			ASSERT( this != NULL );
			return m_pWndChild;
		}
		const CExtPopupMenuWnd * GetPopup() const
		{
			ASSERT( this != NULL );
			return m_pWndChild;
		}
		bool IsPopup() const
		{
			return ( m_nCmdID == TYPE_POPUP ) ? true : false;
		}

		UINT GetCmdID() const;
		UINT SetCmdID( UINT nCmdID );

		bool IsExecutableCmdID() const;

		int GetMeasuredHeight();
		int GetMeasuredWidth();
		
		int GetIconAreaWidth() const
		{
			return m_nIconAreaWidth;
		}
		void UpdateIconAreaWidth(
			INT nIconAreaWidthLager
			)
		{
			if( IsToolButton() )
				return;
			if( nIconAreaWidthLager <= m_nIconAreaWidth )
				return;
			if( !IsSeparator() )
				m_sizeItem.cx +=
					nIconAreaWidthLager - m_nIconAreaWidth;
			m_nIconAreaWidth = nIconAreaWidthLager;
		}

		CRect GetBaseRect() const
		{
			CRect rcItem(
				CPoint(0,0),
				m_sizeItem
				);
			return rcItem;
		}
		CRect GetCachedRect(
			bool bIncludeIndents
			) const
		{
			return bIncludeIndents ? m_rcCacheBasic : m_rcCacheIndents;
		}
		void CacheRect(
			const RECT & rcCache,
			bool bIncludeIndents
			)
		{
			if( bIncludeIndents )
				m_rcCacheBasic = rcCache;
			else
				m_rcCacheIndents = rcCache;
		}
		void CacheRectsEmpty()
		{
			m_rcCacheBasic.SetRect( 0, 0, 0, 0 );
			m_rcCacheIndents.SetRect( 0, 0, 0, 0 );
		}
		
		int GetIndex() const
		{
			return m_nItemIndex;
		}
		void SetIndex( int nItemIndex )
		{
			m_nItemIndex = nItemIndex;
		}
	protected:
		friend class CExtPopupMenuWnd;
		friend class CExtPopupMenuCmdUI;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		static bool stat_CbVerifyTextInput(
			CInPlaceEditWnd & wndEdit,
			CExtCustomizeCmdTreeNode * pNode,
			__EXT_MFC_SAFE_LPCTSTR sTextOld,
			__EXT_MFC_SAFE_LPCTSTR sTextNew
			);
		static void stat_CbPutTextInputResult(
			CInPlaceEditWnd & wndEdit,
			CExtCustomizeCmdTreeNode * pNode,
			__EXT_MFC_SAFE_LPCTSTR sTextNew
			);
		static bool stat_CbInplaceEditWndProc(
			LRESULT & lResult,
			UINT message,
			WPARAM wParam,
			LPARAM lParam,
			CEdit & wndEdit,
			CExtCustomizeCmdTreeNode * pNode
			);
		static bool stat_CbInitListBoxContent(
			CExtPopupInplaceListBox & wndListBox,
			CExtCustomizeCmdTreeNode * pNode
			);
		static bool stat_CbListBoxSelection(
			CExtPopupInplaceListBox & wndListBox,
			CExtCustomizeCmdTreeNode * pNode,
			int eSAT // CExtPopupInplaceListBox::e_sel_action_t
			);
		static bool stat_CbListBoxItemDraw(
			CExtPopupInplaceListBox & wndListBox,
			CExtCustomizeCmdTreeNode * pNode,
			LPDRAWITEMSTRUCT pDIS
			);
		static bool stat_CbListBoxItemMeasure(
			CExtPopupInplaceListBox & wndListBox,
			CExtCustomizeCmdTreeNode * pNode,
			LPMEASUREITEMSTRUCT pMIS
			);

	#ifndef __EXT_MFC_NO_BUILTIN_DATEFIELD
		static bool stat_CbInitDatePickerContent(
			CExtDatePickerWnd & wndDatePicker,
			CExtCustomizeCmdTreeNode * pNode
			);
		static bool stat_CbDatePickerSelection(
			LPVOID pSelectionNotification,
			CExtCustomizeCmdTreeNode * pNode
			);
	#endif // __EXT_MFC_NO_BUILTIN_DATEFIELD
	
	#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP
		static bool stat_CbFormatCaption(
			CExtSafeString & strCaption,
			CExtPopupUndoRedoMenuWnd * pUndoRedoPopupMenuWnd,
			CExtCustomizeCmdTreeNode * pNode
			);
	#endif // __EXT_MFC_NO_UNDO_REDO_POPUP

	#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	}; // class MENUITEMDATA
	
	CExtPopupMenuWnd::MENUITEMDATA & ItemGetInfo( INT nPos );
	const CExtPopupMenuWnd::MENUITEMDATA & ItemGetInfo( INT nPos ) const;

	INT ItemGetIndexOf(
		const CExtPopupMenuWnd::MENUITEMDATA * pMII
		) const;

	/////////////////////////////////////////////////////////////////////////////
	// CExtPopupMenuWnd::DRAWITEMDATA
	
	struct DRAWITEMDATA;
	friend struct DRAWITEMDATA;
	
	struct __PROF_UIS_API DRAWITEMDATA
	{
		CDC * m_pDC;
		LPCRECT m_pRectItem;
		LPCRECT m_pRectItemWithIndents;
		const MENUITEMDATA * m_pItemData;
		CExtPopupMenuWnd * m_pPopup;
		const bool m_bMenuOwnerDrawFixed:1;
		const bool m_bPopupItem:1;
		const bool m_bRarelyUsed:1;
		const bool m_bRarelyUsedPrevious:1;
		const bool m_bRarelyUsedNext:1;
	public:
		DRAWITEMDATA(
			CDC * pDC,
			LPCRECT pRectItem,
			LPCRECT pRectItemWithIndents,
			const MENUITEMDATA * pItemData,
			CExtPopupMenuWnd * pPopup,
			bool bMenuOwnerDrawFixed,
			bool bPopupItem,
			bool bRarelyUsed,
			bool bRarelyUsedPrevious,
			bool bRarelyUsedNext
			)
			: m_pDC( pDC )
			, m_pRectItem( pRectItem )
			, m_pRectItemWithIndents( pRectItemWithIndents )
			, m_pItemData( pItemData )
			, m_pPopup( pPopup )
			, m_bMenuOwnerDrawFixed( bMenuOwnerDrawFixed )
			, m_bPopupItem( bPopupItem )
			, m_bRarelyUsed( bRarelyUsed )
			, m_bRarelyUsedPrevious( bRarelyUsedPrevious )
			, m_bRarelyUsedNext( bRarelyUsedNext )
		{
			ASSERT( m_pDC != NULL );
			ASSERT( m_pDC->GetSafeHdc() != NULL );
			ASSERT( m_pRectItem != NULL );
			ASSERT( m_pRectItemWithIndents != NULL );
			ASSERT( m_pItemData != NULL );
			ASSERT( m_pPopup != NULL );
		}
		operator CDC * ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pDC != NULL );
			ASSERT( m_pDC->GetSafeHdc() != NULL );
			return m_pDC;
		}
		operator LPCRECT()
		{
			ASSERT( this != NULL );
			ASSERT( m_pRectItem != NULL );
			return m_pRectItem;
		}
		operator const MENUITEMDATA * ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pItemData != NULL );
			return m_pItemData;
		}
		operator __EXT_MFC_SAFE_LPCTSTR()
		{
			ASSERT( this != NULL );
			ASSERT( m_pItemData != NULL );
			return m_pItemData->GetText();
		}
		operator CSize()
		{
			ASSERT( this != NULL );
			ASSERT( m_pRectItem != NULL );
			return
				CSize(
					m_pRectItem->right - m_pRectItem->left,
					m_pRectItem->bottom - m_pRectItem->top
					);
		}
		operator CExtPopupMenuWnd * ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pPopup != NULL );
			return m_pPopup;
		}
		CExtPopupMenuWnd * operator -> ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pPopup != NULL );
			return m_pPopup;
		}
		operator LPARAM()
		{
			return reinterpret_cast < LPARAM > ( this );
		}
		UINT GetCmdID()
		{
			ASSERT( this != NULL );
			ASSERT( m_pItemData != NULL );
			return m_pItemData->GetCmdID();
		}
		void PaintDefault(
			bool bForceNoIcon = false,
			bool bForceNoText = false,
			bool bForceNoCheck = false,
			bool bForceEnabled = false,
			bool bForceUnselected = false
			);
		void PaintDefaultBk()
		{
			PaintDefault(
				true,
				true,
				true,
				true,
				false
				);
		}
		bool DoOwnerDrawPainting();
		friend class CExtPopupMenuWnd;
	}; // struct DRAWITEMDATA

	/////////////////////////////////////////////////////////////////////////////
	// CExtPopupMenuWnd::DRAWBACKGROUNDDATA
	struct __PROF_UIS_API DRAWBACKGROUNDDATA
	{
		CDC & m_DC;
		CRect m_rcBackground;
		CExtPopupMenuWnd * m_pPopup;
		bool m_bCustomBackgroundDrawn:1, m_bCustomBorderDrawn:1;
		DRAWBACKGROUNDDATA(
			CDC & dc,
			const RECT & rcBackground,
			CExtPopupMenuWnd * pPopup
			)
			: m_DC( dc )
			, m_rcBackground( rcBackground )
			, m_pPopup( pPopup )
			, m_bCustomBackgroundDrawn( false )
			, m_bCustomBorderDrawn( false )
		{
			ASSERT( m_DC.GetSafeHdc() != NULL );
			ASSERT( m_pPopup != NULL );
		}
	public:
		operator CDC & ()
		{
			ASSERT( this != NULL );
			ASSERT( m_DC.GetSafeHdc() != NULL );
			return m_DC;
		}
		operator LPCRECT()
		{
			ASSERT( this != NULL );
			return (&m_rcBackground);
		}
		operator CSize()
		{
			ASSERT( this != NULL );
			return m_rcBackground.Size();
		}
		operator CExtPopupMenuWnd * ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pPopup != NULL );
			return m_pPopup;
		}
		CExtPopupMenuWnd * operator -> ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pPopup != NULL );
			return m_pPopup;
		}
		operator LPARAM()
		{
			return reinterpret_cast < LPARAM > ( this );
		}
		void Notify();
	};

	/////////////////////////////////////////////////////////////////////////////
	// CExtPopupMenuWnd::DRAWLEFTAREADATA
	
	struct DRAWLEFTAREADATA;
	friend struct DRAWLEFTAREADATA;
	
	struct __PROF_UIS_API DRAWLEFTAREADATA
	{
		CDC * m_pDC;
		LPCRECT m_pRectLeftArea;
		CExtPopupMenuWnd * m_pPopup;
	protected:
		DRAWLEFTAREADATA(
			CDC * pDC,
			LPCRECT pRectLeftArea,
			CExtPopupMenuWnd * pPopup
			)
			: m_pDC( pDC )
			, m_pRectLeftArea( pRectLeftArea )
			, m_pPopup( pPopup )
		{
			ASSERT( m_pDC != NULL );
			ASSERT( m_pDC->GetSafeHdc() != NULL );
			ASSERT( m_pRectLeftArea != NULL );
			ASSERT( m_pPopup != NULL );
		}
	public:
		operator CDC * ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pDC != NULL );
			ASSERT( m_pDC->GetSafeHdc() != NULL );
			return m_pDC;
		}
		operator LPCRECT()
		{
			ASSERT( this != NULL );
			ASSERT( m_pRectLeftArea != NULL );
			return m_pRectLeftArea;
		}
		operator CSize()
		{
			ASSERT( this != NULL );
			ASSERT( m_pRectLeftArea != NULL );
			return
				CSize(
					m_pRectLeftArea->right - m_pRectLeftArea->left,
					m_pRectLeftArea->bottom - m_pRectLeftArea->top
					);
		}
		operator CExtPopupMenuWnd * ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pPopup != NULL );
			return m_pPopup;
		}
		CExtPopupMenuWnd * operator -> ()
		{
			ASSERT( this != NULL );
			ASSERT( m_pPopup != NULL );
			return m_pPopup;
		}
		operator LPARAM()
		{
			return reinterpret_cast < LPARAM > ( this );
		}
	protected:
		bool DoOwnerDrawPainting();
		friend class CExtPopupMenuWnd;
		friend class CExtPopupColorMenuWnd;
	}; // struct DRAWLEFTAREADATA

protected:
	virtual void _EndSequenceEx(
		MENUITEMDATA & mi
		);
	virtual void _EndSequence(
		UINT nCmdID = 0,
		HWND hWndDeliver = NULL
		);

	typedef
		CArray < MENUITEMDATA, MENUITEMDATA & >
		items_container_t;
	typedef
		const items_container_t
		const_items_container_t;
	items_container_t m_items_all;
//	items_container_t & _GetItemsContaner();
//	const_items_container_t & _GetItemsContaner() const;
	MENUITEMDATA & _GetItemRef(int nIndex);
	const MENUITEMDATA & _GetItemRef(int nIndex) const;
	virtual void _InsertItem(
		int nInsertBefore,
		MENUITEMDATA & mi,
		bool bNoRefToCmdMngr = false
		);

public:

	static bool TestHoverEnabledFromActiveHWND(
		HWND hWndTestFrom,
		HWND hWndActive = ::GetActiveWindow(),
		bool bCheckEnabled = true,
		bool bCheckVisible = true,
		bool bCheckExtPopupMenuTracking = true
		);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtPopupMenuWnd)
	protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtPopupMenuWnd();
	virtual CExtPopupMenuSite & GetSite() const;

protected:
	virtual void _StartAnimation();
	bool m_bHelperReCreateDynamicShadowAtTheEndOfNearestAnimation:1;
	virtual void _EndAnimation();
	

public:
	virtual bool _FindHelpMode() const;

	virtual BOOL DestroyWindow();

protected:
	virtual void PostNcDestroy();
	virtual void _DeleteFadeOutMenu();

	// Generated message map functions
	//{{AFX_MSG(CExtPopupMenuWnd)
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	afx_msg UINT OnNcHitTest(CPoint point);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);

	friend class CExtPopupMenuCmdUI;

protected:
	CExtSafeString m_sMenuCaption;
	mutable CSize m_sizeCaptionMeasured;
public:
	virtual __EXT_MFC_SAFE_LPCTSTR MenuCaptionTextGet() const;
	virtual void MenuCaptionTextSet(
		__EXT_MFC_SAFE_LPCTSTR sMenuCaption = NULL
		);
	virtual CSize MenuCaptionMeasure() const;
	virtual void MenuCaptionPaint(
		CDC & dc,
		CRect rcMenuCaption
		) const;

public:
	bool m_bHelperResizingMode:1;
	CSize m_sizeResizingMin, m_sizeResizingMax;
	bool m_bHelperAnimationControllerDetected:1;
protected:
	INT m_nFadeOutAnimationStepIndex, m_nFadeOutAnimationStepCount, m_nFadeOutAnimationEllapse;
public:
	static INT g_nDefaultFadeOutAnimationStepCount;
	static INT g_nDefaultFadeOutAnimationEllapse;
	static INT g_nAutoExpandTime;

	virtual bool _IsResizingMode() const;
	virtual void _DoResizing(
		CPoint ptScreenClick,
		bool bVerticalOnlyResizing
		);
	virtual void _DoAdjustControlMetrics(
		CSize _sizeAdjust
		);

	virtual CSize ResizingMinSizeGet() const;
	virtual void ResizingMinSizeSet( CSize _size );
	virtual CSize ResizingMaxSizeGet() const;
	virtual void ResizingMaxSizeSet( CSize _size );

	static const UINT g_nMsgInstantiatePopupMenu;
	struct __PROF_UIS_API INSTANTIATEPOPUPMENUNOTIFICATION
	{
		CRuntimeClass * m_pMenuRTC;
		CObject * m_pQuerySrc;
		LPARAM m_lParam;
		CExtPopupMenuWnd * m_pCreatedPopupMenuInstance;
		INSTANTIATEPOPUPMENUNOTIFICATION(
			CRuntimeClass * pMenuRTC,
			CObject * pQuerySrc,
			LPARAM lParam
			);
		operator WPARAM ();
		static INSTANTIATEPOPUPMENUNOTIFICATION & FromWPARAM( WPARAM wParam );
		void Notify( HWND hWndNotifyTarget );
	}; // struct INSTANTIATEPOPUPMENUNOTIFICATION
	static CExtPopupMenuWnd * InstantiatePopupMenu(
		HWND hWndNotifyTarget,
		CRuntimeClass * pMenuRTC,
		CObject * pQuerySrc,
		LPARAM lParam = 0L
		);
}; // class CExtPopupMenuWnd

class __PROF_UIS_API CExtPopupColorMenuWnd :
	public CExtPopupMenuWnd
{
public:
	DECLARE_DYNCREATE( CExtPopupColorMenuWnd );

	CExtPopupColorMenuWnd();
	virtual ~CExtPopupColorMenuWnd();

// Attributes
public:
	static const UINT g_nMsgNotifyColorChanged;
	static const UINT g_nMsgNotifyColorChangedFinally;
	static const UINT g_nMsgNotifyCustColor;

	HWND m_hWndNotifyColorChanged; // def is NULL - use command targed
	LPARAM m_lParamCookie;
	COLORREF * m_pClrValueFinal;
	enum e_color_selection_t
	{
		__ECST_NONE = COLORREF( -1L ),
		__ECST_BUTTON_CUSTOM = COLORREF( -2L ),
		__ECST_BUTTON_DEFAULT = COLORREF( -3L ),
	};

	COLORREF m_clrDefault,m_clrInitial;
	bool m_bEnableBtnColorDefault:1;
	bool m_bEnableBtnColorCustom:1;

	CExtSafeString m_sBtnTextColorDefault;
	CExtSafeString m_sBtnTextColorCustom;

protected:
	CRect m_rcDefColorText, m_rcCustColorText;

	enum
	{
		IDX_DEFAULT_COLOR_BTN = -101,
		IDX_CUSTOM_COLOR_BTN = -102
	};

// Implementation
protected:
	int m_nColorIdxCurr, m_nInitialColorIdx;

	HWND _GetWndNotifyColorChanged();

	int _FindCellByColorRef( COLORREF clr );

	virtual bool _CreateHelper(
		CWnd * pWndCmdReceiver
		);
	virtual bool IsAllItemsRarelyUsed() const;
	virtual CExtPopupMenuTipWnd::e_tip_style_t _CoolTipGetStyle() const;

public:
	class __PROF_UIS_API COLORREF_TABLE_ENTRY
	{
	protected:
		void _AssignFromOther (
			const COLORREF_TABLE_ENTRY & other
			);
	public:
		UINT             m_nStrTblResID;
		COLORREF         m_clrValue;
		CExtSafeString   m_strColorNameEnglish;
		COLORREF_TABLE_ENTRY();
		COLORREF_TABLE_ENTRY(
			UINT         nStrTblResID,
			COLORREF     clrValue,
			__EXT_MFC_SAFE_LPCTSTR
			             strColorNameEnglish
			);
		COLORREF_TABLE_ENTRY(
			const COLORREF_TABLE_ENTRY & other
			);
		virtual ~COLORREF_TABLE_ENTRY();
		COLORREF_TABLE_ENTRY & operator = (
			const COLORREF_TABLE_ENTRY & other
			);
		virtual CExtSafeString GetLocalColorName() const;
	}; // struct COLORREF_TABLE_ENTRY

	static const COLORREF_TABLE_ENTRY
		g_arrDefaultColors[ 40 ];
	static const COLORREF_TABLE_ENTRY
		g_arrDefaultColorsLess[ 16 ];

	typedef
		CTypedPtrArray < CPtrArray, COLORREF_TABLE_ENTRY * >
		color_array_t;
protected:
	int m_nColorsInRow;
	color_array_t m_arrColors;
public:
	CSize m_sizeColorItemCell, m_sizeColorItemSpaces;

	void AddColor( COLORREF_TABLE_ENTRY * pCTE );
	void RemoveAllColors();
	void SetColorsInRow( int nColorsInRow );
	void SetColors8x5();
	void SetColors8x2();

	virtual CSize _GetColorItemCellSize() const;
	virtual CSize _GetColorItemSpaces() const;
	virtual int _GetColorCellCount() const;
	virtual int _GetColorsInRow() const;
	virtual void _SetColorsInRow( int nColorsInRow );
	virtual CSize _GetColorDimension() const;
	virtual const COLORREF_TABLE_ENTRY * _GetColorEntry( int nIndex ) const;
	virtual COLORREF _GetColorValue( int nIndex ) const;
	virtual CExtSafeString _GetLocalColorName( int nIndex ) const;
	virtual int _GetNextKeyColor(
		int nColorIndex,
		UINT nChar // VK_LEFT, VK_RIGHT, VK_UP and VK_DOWN
		);
	virtual void _RemoveAllColorEntries();

	virtual int _HitTest(
		const CPoint & point,
		bool * p_bInplaceControlArea = NULL,
		bool * p_bInplaceDropDownArea = NULL
		)
	{
		ASSERT_VALID( this );
		if( p_bInplaceControlArea != NULL )
			*p_bInplaceControlArea = false;
		if( p_bInplaceDropDownArea != NULL )
			*p_bInplaceDropDownArea = false;
		point;
		return IDX_NOTHING;
	}
protected:
	bool m_bHelperInNotifyColorChanged:1;
	void _NotifyColorChanged(
		bool bFinal = false
		);

	CPoint _GetColorItemCoord(int nIdx);
	CRect _GetColorItemRect(int nIdx);
	int _ColorItemHitTest(const CPoint & point);

public:
	virtual CRect _CalcTrackRect();
	virtual CSize _CalcTrackSize();
	virtual void _DoPaint( CDC & dcPaint, bool bUseBackBuffer = true );
protected:

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	// CExtCustomizeSite::ICustomizeDropTarget
	virtual DROPEFFECT OnCustomizeTargetOver(
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		CPoint point,
		DWORD dwKeyState
		);
	virtual void OnCustomizeTargetLeave();
	virtual bool OnCustomizeTargetDrop(
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		CPoint point,
		DROPEFFECT de
		);
	// CExtCustomizeSite::ICustomizeDropSource
	virtual void OnCustomizeSourceDragComplete(
		DROPEFFECT de,
		bool bCanceled,
		bool * p_bNoResetActiveItem
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

public:
	virtual bool _OnMouseWheel(
		WPARAM wParam,
		LPARAM lParam,
		bool & bNoEat
		);
	virtual bool _OnMouseMove(
		UINT nFlags,
		CPoint point,
		bool & bNoEat
		);
	virtual bool _OnMouseClick(
		UINT nFlags,
		CPoint point,
		bool & bNoEat
		);
	virtual bool _OnKeyDown(
		UINT nChar,
		UINT nRepCnt,
		UINT nFlags,
		bool & bNoEat
		);

	virtual bool _CanStartLevelTracking();

protected:
	// Generated message map functions
	//{{AFX_MSG(CExtPopupColorMenuWnd)
	//}}AFX_MSG
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	DECLARE_MESSAGE_MAP()
}; // class CExtPopupColorMenuWnd

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_POPUP_MENU_WND_H
