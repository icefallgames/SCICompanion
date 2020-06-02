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

#if (!defined __EXT_PAINT_MANAGER_H)
#define __EXT_PAINT_MANAGER_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif // __EXT_CMD_ICON_H

#if (!defined __AFXTEMPL_H__)
	#include <AfxTempl.h>
#endif

#if (!defined __EXT_MFC_NO_TAB_CTRL)
	#if (!defined __EXT_TABWND_H)
		#include <ExtTabWnd.h>
	#endif
#endif // (!defined __EXT_MFC_NO_TAB_CTRL)

#if (!defined __EXT_PMSYNCHRONIZESINK_H)
	#include "../Src/ExtPmSynchronizeSink.h"
#endif

#define __ExtMfc_CXTEXTMARGIN (-1) // 1
#define __ExtMfc_CYTEXTMARGIN (0)  // 1

#define __EXT_PM_BACK_COMPATIBILITY_COLOR_API
#define __EXT_PM_BACK_COMPATIBILITY_BITMAP_API

class CExtScrollBar;
class CExtBitmap;
class CExtCmdIcon;
class CExtBarButton;
class CExtBarNcAreaButton;
class CExtMiniDockFrameWnd;
class CExtControlBar;
class CExtToolControlBar;
class CExtBarContentExpandButton;
class CExtPopupMenuWnd;
class CExtAnimationParameters;
class CExtAnimationClient;

#ifndef __EXT_MFC_NO_GRIDWND
	class CExtGridCell;
#endif // #ifndef __EXT_MFC_NO_GRIDWND

#ifndef __EXT_MFC_NO_STATUSBAR
	class CExtStatusControlBar;
#endif

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	class CExtDynDockMarkerWnd;
#endif

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	class CExtRibbonButton;
	class CExtRibbonButtonGroup;
	class CExtRibbonButtonToolGroup;
	class CExtRibbonButtonFile;
	class CExtRibbonPage;
	class CExtNcFrameImpl;
	class CExtRibbonButtonQuickAccessContentExpand;
	class CExtRibbonBar;
	class CExtRibbonButtonDialogLauncher;
	class CExtRibbonGalleryInplaceScrollBar;
	class CExtRibbonGalleryWnd;
	class CExtRibbonGalleryPopupMenuWnd;
	class CExtRibbonNodeGallery;
	class CExtRibbonButtonGallery;
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	class CExtCustomizeCmdScreenTip;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __DEFAULT_PAINT_MANAGER_CLASS)
	#define __DEFAULT_PAINT_MANAGER_CLASS CExtPaintManagerXP
#endif

#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
	class CExtPageContainerWnd;
#endif // (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)

#define __ExtMfc_WM_THEMECHANGED 0x031A

// AnimateWindow() Flags

#define __EXT_AW_HOR_POSITIVE    0x00000001
#define __EXT_AW_HOR_NEGATIVE    0x00000002
#define __EXT_AW_VER_POSITIVE    0x00000004
#define __EXT_AW_VER_NEGATIVE    0x00000008
#define __EXT_AW_CENTER          0x00000010
#define __EXT_AW_HIDE            0x00010000
#define __EXT_AW_ACTIVATE        0x00020000
#define __EXT_AW_SLIDE           0x00040000
#define __EXT_AW_BLEND           0x00080000

// for layered API

#define __EXT_MFC_LWA_COLORKEY  0x00000001
#define __EXT_MFC_LWA_ALPHA     0x00000002

#define __EXT_MFC_WS_EX_LAYERED 0x00080000

#define __EXT_MFC_ULW_COLORKEY  0x00000001
#define __EXT_MFC_ULW_ALPHA     0x00000002
#define __EXT_MFC_ULW_OPAQUE    0x00000004

#define __EXT_MFC_AC_SRC_OVER   0x00
#define __EXT_MFC_AC_SRC_ALPHA  0x01

// Prof-UIS helper color constants

#define __ExtMfc_MIN_SYS_COLOR_VAL	0L
#define __ExtMfc_MAX_SYS_COLOR_VAL	49L

#define __ExtMfc_COLOR_MAP_BASE				1000L
#define __ExtMfc_XP_COLOR_SHIFT				(__ExtMfc_COLOR_MAP_BASE + 1000L)
#define __ExtMfc_2003_COLOR_SHIFT			(__ExtMfc_XP_COLOR_SHIFT + 1000L)
#define __ExtMfc_STUDIO_2005_COLOR_SHIFT	(__ExtMfc_2003_COLOR_SHIFT + 1000L)
#define __ExtMfc_STUDIO_2008_COLOR_SHIFT	(__ExtMfc_STUDIO_2005_COLOR_SHIFT + 1000L)
#define __ExtMfc_STUDIO_2010_COLOR_SHIFT	(__ExtMfc_STUDIO_2008_COLOR_SHIFT + 1000L)

//#define __ExtMfc_MENU_GAP__ 4

#define __EXT_MENU_ICON_CX 16
#define __EXT_MENU_ICON_CY 16
#define __EXT_MENU_GAP 3
#define __EXT_MENU_MIN_HEIGHT (__EXT_MENU_ICON_CY+__EXT_MENU_GAP*2)
#define __EXT_MENU_POPUP_ARROW_AREA_DX 16
#define __EXT_MENU_IMG_RECT_OUT_GAP 2
#define __EXT_MENU_TOOLBTN_GAP 2

#define __DROPDOWN_ARROW_GAP 3

// PrintWindow() API flags
#define __EXT_MFC_PW_CLIENTONLY 0x00000001

//////////////////////////////////////////////////////////////////////////
// paint managers

class __PROF_UIS_API CExtImageList : public CImageList
{
public:
	CExtImageList()
	{
	}
	CExtImageList(
		UINT nResourceID,
		COLORREF clrTransparencyMask = RGB(255,0,255),
		int nButtonWidth = 16,
		HINSTANCE hInstResource = NULL,
		UINT nColorFlag = ILC_COLOR24,
		UINT nColorAddionalFlag = ILC_MASK
		)
	{
		VERIFY( Load( nResourceID, clrTransparencyMask, nButtonWidth, hInstResource, nColorFlag, nColorAddionalFlag ) );
	}
	BOOL Load(
		UINT nResourceID,
		COLORREF clrTransparencyMask = RGB(255,0,255),
		int nButtonWidth = 16,
		HINSTANCE hInstResource = NULL,
		UINT nColorFlag = ILC_COLOR24,
		UINT nColorAddionalFlag = ILC_MASK
		)
	{
		ASSERT(
				nColorFlag == ILC_COLOR4
			||	nColorFlag == ILC_COLOR8
			||	nColorFlag == ILC_COLOR16
			||	nColorFlag == ILC_COLOR24
			||	nColorFlag == ILC_COLOR32
			);
		if( hInstResource == NULL )
		{
			hInstResource = ::AfxGetResourceHandle();
			if( hInstResource == NULL )
			{
				hInstResource = ::AfxGetInstanceHandle();
				if( hInstResource == NULL )
				{
					ASSERT( FALSE );
					return FALSE;
				}
			}
		}
		CBitmap objBitmap;
		if( ! objBitmap.Attach( LoadImage( hInstResource, MAKEINTRESOURCE( nResourceID ), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_CREATEDIBSECTION ) ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		BITMAP infoBitmap;
		::memset( &infoBitmap, 0, sizeof(BITMAP) );
		if( ! objBitmap.GetBitmap( &infoBitmap ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		CSize sizeBitmap( infoBitmap.bmWidth, infoBitmap.bmHeight ); 
		int nCountOfButtons = sizeBitmap.cx / nButtonWidth;
		if( ! Create( nButtonWidth, sizeBitmap.cy, nColorFlag|nColorAddionalFlag, nCountOfButtons, 0 ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		if( Add( &objBitmap, clrTransparencyMask ) == -1 )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		return TRUE;
	}
}; // class CExtImageList

//	class __PROF_UIS_API CExtSWMR
//	{
//	protected:
//		static __EXT_MFC_SAFE_LPCTSTR stat_GenerateObjectName(
//			__EXT_MFC_SAFE_LPCTSTR strPrefix,
//			__EXT_MFC_SAFE_LPCTSTR strSuffix,
//			__EXT_MFC_SAFE_LPTSTR strOutNameBuffer,
//			size_t nBufferSize,
//			bool & bConstructed
//			);
//		mutable bool m_bConstructed;
//		HANDLE m_hProtectWriter, m_hProtectReaders, m_hEnumReaders;
//	public:
//		class __PROF_UIS_API LockSection
//		{
//			CExtSWMR & m_SWMR;
//			bool m_bRead:1, m_bLocked:1;
//		public:
//			LockSection(
//				CExtSWMR & _SWMR,
//				bool bRead,
//				DWORD dwTimeout = INFINITE
//				);
//			~LockSection();
//			bool IsLocked() const;
//		}; // class LockSection
//		CExtSWMR(
//			__EXT_MFC_SAFE_LPCTSTR strName = NULL
//			);
//		virtual ~CExtSWMR();
//		bool Create(
//			__EXT_MFC_SAFE_LPCTSTR strName = NULL
//			);
//		void Destroy();
//		bool IsConstructed() const;
//		DWORD WaitToWrite(
//			DWORD dwTimeout = INFINITE
//			);
//		DWORD WaitToRead(
//			DWORD dwTimeout = INFINITE
//			);
//		void DoneReading();
//		void DoneWriting();
//	}; // class CExtSWMR

class __PROF_UIS_API CExtPaintManager : public CObject
{
public:
	DECLARE_SERIAL( CExtPaintManager );
	enum e_ui_scaling_type_t
	{
		__EUIST_X           = 0,
		__EUIST_Y           = 1,
		__EUIST_Z           = 2,
		__EUIST_MIN_VALUE   = 0,
		__EUIST_MAX_VALUE   = 2,
	};
	virtual bool UiScalingIsEnabled(
		e_ui_scaling_type_t eUIST
		) const;
	virtual INT UiScalingDo(
		INT nValue,
		e_ui_scaling_type_t eUIST
		) const;

protected:
	// color values container
	typedef CArray < COLORREF, COLORREF & >
		sys_colors_container;
	sys_colors_container m_colors;

	// system brushes container
	typedef CArray < HBRUSH, HBRUSH & >
		sys_brashes_container;

	sys_brashes_container m_brushes;

	// translated colors map
	typedef
		CMap < int, int, int, int >
		translate_color_map;

	translate_color_map m_mapColorTranslate;

public:
	CPalette m_PaletteWide;
	COLORREF m_clrForceSplitterBk;

	CBrush m_brushLightDefault, m_brushLightSystem,
		m_brushDarkDefault, m_brushDarkSystem,
		m_brushLighterDefault, m_brushLighterSystem,
		m_brushDarkerDefault, m_brushDarkerSystem,
		m_brushLightestDefault, m_brushLightestSystem,
		m_brushDarkestDefault, m_brushDarkestSystem;

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	// ribbon
	CFont m_FontRibbonNormal,
		m_FontRibbonGroupCaption,
		m_FontRibbonTabItem;
	virtual void Ribbon_SyncFonts();
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	// normal
	CFont m_FontNormal;
	CFont m_FontBold;
	// generic caption
	CFont m_FontCaption;
	CFont m_FontCaptionBold;
	// bar caption
	CFont m_FontNormalBC;
	CFont m_FontBoldBC;
	// marlett symbols
	CFont m_FontMarlett;

	int m_DSS_DISABLED_style;

	enum e_paint_manager_name_t
	{
		NativeXP				= 29756,
	};

	enum e_system_theme_t
	{
		ThemeUnknown			= 0,
		ThemeLunaBlue			= 1,
		ThemeLunaOlive			= 2,
		ThemeLunaSilver			= 3,
		ThemeLunaRoyale			= 4,
		ThemeVistaOrLaterUX		= 5,
		ThemeVistaOrLaterDWM	= 6,
	};

	virtual e_paint_manager_name_t OnQueryPaintManagerName()
	{
		ASSERT_VALID( this );
		return NativeXP;
	}

	virtual INT GetSeparatorHeight()
	{
		ASSERT_VALID( this );
		return 6;
	}

#ifndef __EXT_MFC_NO_TAB_ONENOTE_CTRL
	virtual void TabOneNoteWnd_DrawClientArea(
		CDC & dc,
		CRect & rcClient,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual void GetTabOneNoteItemColors(
		bool bSelected,
		bool bHover,
		bool bEnabled,
		COLORREF &clrBorderLight,
		COLORREF &clrBorderDark,
		COLORREF &clrBkLight,
		COLORREF &clrBkDark,
		COLORREF &clrText
		);
#endif // __EXT_MFC_NO_TAB_ONENOTE_CTRL

#ifndef __EXT_MFC_NO_TAB_WHIDBEY_CTRL
	virtual void GetTabWhidbeyItemColors(
		bool bSelected,
		bool bHover,
		bool bEnabled,
		COLORREF &clrBorder,
		COLORREF &clrBkLight,
		COLORREF &clrBkDark,
		COLORREF &clrText
		);
#endif // __EXT_MFC_NO_TAB_WHIDBEY_CTRL

	virtual bool OnQueryUseThemeColors() const;

	virtual e_system_theme_t OnQuerySystemTheme() const;

	virtual bool IsHighContrast() const;

	// global auto-pointer class for CExtPaintManager
	class __PROF_UIS_API CExtPaintManagerAutoPtr
		: public CExtPmSynchronizeSink
	{
		CExtPaintManager * m_pPaintManager;

		CTypedPtrList
			< CPtrList, CExtPmBridge * >
			m_listBridges;
		
		bool m_bHaveScrollLines:1;
		UINT m_uCachedScrollLines;
		bool InitPaintManagerInstance();
	
	public:

		CExtSafeString m_strOsVer;
		OSVERSIONINFO m_osVerData;
		// quick boolean helpers
		bool
			  m_bIsWin32s:1				// old Win32s
			, m_bIsWin9x:1				// any Windows 9x
			, m_bIsWin95:1				// exactly Windows 95
			, m_bIsWin98:1				// exactly Windows 98
			, m_bIsWin98orLater:1		// Windows 98 or later
			, m_bIsWinNT:1				// any NT
			, m_bIsWinNT4:1				// exactly Windows NT 4
			, m_bIsWin2000:1			// exactly Windows 2000
			, m_bIsWinXP:1				// exactly Windows XP
			, m_bIsWinNT4orLater:1		// Windows NT 4 or later
			, m_bIsWin2000orLater:1		// Windows 2000 or later
			, m_bIsWinXPorLater:1		// Windows XP or later
			, m_bIsWinVista:1			// Windows Vista
			, m_bIsWinVistaOrLater:1	// Windows Vista or later
			, m_bIsWin7:1				// Windows 7
			, m_bIsWin7OrLater:1		// Windows 7 or later
			;
		INT m_nLPX, m_nLPY, m_nLPZ;
		bool
			  m_bEnableUiScalingX:1
			, m_bEnableUiScalingY:1
			, m_bEnableUiScalingZ:1
			;
		bool m_bAutoSkinScrollBars:1;

		CExtPaintManagerAutoPtr();
		virtual ~CExtPaintManagerAutoPtr();

		void Advise(
			CExtPmBridge * pPmBridge,
			bool bAddTail = true
			);
		void Unadvise( CExtPmBridge * pPmBridge );
		void UnadviseAll();

		CExtPaintManager * GetPM();
		CExtPaintManager * operator -> ();

		INT m_nInstallPaintManagerCounter;
		const UINT m_nMsgPaintManagerChanged;
		bool InstallPaintManager(
			CExtPaintManager * pPaintManager
			);
		bool InstallPaintManager(
			CRuntimeClass * pRtcPaintManager
			);

		e_paint_manager_name_t GetPaintManagerName()
		{
			ASSERT_VALID( m_pPaintManager );
			return m_pPaintManager->OnQueryPaintManagerName();
		}

		bool PaintManagerStateLoad(
			__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
			__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
			__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
			HKEY hKeyRoot = HKEY_CURRENT_USER,
			bool bEnableThrowExceptions = false
			);
		bool PaintManagerStateSave(
			__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
			__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
			__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%\%sSectionNameProfile%
			HKEY hKeyRoot = HKEY_CURRENT_USER,
			bool bEnableThrowExceptions = false
			);
		virtual bool PaintManagerStateSerialize(
			CArchive & ar,
			bool bEnableThrowExceptions = false
			);

		UINT GetMouseWheelScrollLines();

		void InitUserExApi();
		bool m_bUxValidColorsExtracted:1;
		COLORREF
			m_clrUxTaskBandEdgeLightColor,
			m_clrUxTaskBandEdgeHighLightColor,
			m_clrUxTaskBandEdgeShadowColor,
			m_clrUxTaskBandEdgeDkShadowColor,
			m_clrUxTaskBandEdgeFillColor,
			m_clrUxTaskBandFillColorHint,
			m_clrUxTaskBandAccentColorHint;

		CExtUxTheme m_UxTheme;
		CExtDWM m_DWM;

		e_system_theme_t m_eCurrentTheme;

		bool m_bAllowWndUpdateSourceDetection:1;
		bool IsWndUpdateSource(
			CWnd * pWndNotifySrc
			);
	
		void OnSysColorChange(
			CWnd * pWndNotifySrc
			);
		void OnSettingChange(
			CWnd * pWndNotifySrc,
			UINT uFlags,
			__EXT_MFC_SAFE_LPCTSTR lpszSection
			);
		void OnDisplayChange(
			CWnd * pWndNotifySrc,
			INT nDepthBPP,
			CPoint ptSizes
			);
		void OnThemeChanged(
			CWnd * pWndNotifySrc,
			WPARAM wParam,
			LPARAM lParam
			);

		// layered API
		struct __EXT_MFC_BLENDFUNCTION
		{
			BYTE BlendOp;
			BYTE BlendFlags;
			BYTE SourceConstantAlpha;
			BYTE AlphaFormat;
		};

		typedef BOOL (WINAPI *PFNGRADIENTFILL)(HDC,PTRIVERTEX,ULONG,PVOID,ULONG,ULONG);
		typedef BOOL (WINAPI *PFNALPHABLEND)(HDC,INT,INT,INT,INT,HDC,INT,INT,INT,INT,__EXT_MFC_BLENDFUNCTION);
		typedef BOOL (WINAPI *PFNALPHADIBBLEND)(HDC,INT,INT,INT,INT,CONST VOID *,CONST BITMAPINFO *,UINT,INT,INT,INT,INT,__EXT_MFC_BLENDFUNCTION);
		typedef BOOL (WINAPI *PFNTRANSPARENTBLT)(HDC,INT,INT,INT,INT,HDC,INT,INT,INT,INT,UINT);
		typedef BOOL (WINAPI *PFNTRANSPARENTDIBITS)(HDC,INT,INT,INT,INT,CONST VOID *,CONST BITMAPINFO *,UINT,INT,INT,INT,INT,UINT);

		HMODULE m_hDllMsImg;
		PFNGRADIENTFILL			m_pfnMsImgGradientFill;
		PFNALPHABLEND			m_pfnMsImgAlphaBlend;
		PFNALPHADIBBLEND		m_pfnMsImgAlphaDIBBlend;
		PFNTRANSPARENTBLT		m_pfnMsImgTransparentBlt;
		PFNTRANSPARENTDIBITS	m_pfnMsImgTransparentDIBits;

		typedef BOOL (WINAPI *PFNANIMATEWINDOW)( HWND,DWORD,DWORD );

		HMODULE m_hDllUser32;
		PFNANIMATEWINDOW		m_pfnAnimateWindow;

		typedef
			BOOL ( __stdcall * PFNSETLAYEREDWINDOWATTRIBUTES)(
				HWND             hwnd,    // handle to the layered window
				COLORREF         crKey,   // specifies the color key
				BYTE             bAlpha,  // value for the blend function
				DWORD            dwFlags  // action
				);
		typedef
			BOOL ( __stdcall * PFNUPDATELAYEREDWINDOW )(
				HWND             hwnd,    // handle to layered window
				HDC              hdcDst,  // handle to screen DC
				POINT          * pptDst,  // new screen position
				SIZE           * psize,   // new size of the layered window
				HDC              hdcSrc,  // handle to surface DC
				POINT          * pptSrc,  // layer position
				COLORREF         crKey,   // color key
				__EXT_MFC_BLENDFUNCTION  * pblend,  // blend function
				DWORD            dwFlags  // options
				);
		PFNSETLAYEREDWINDOWATTRIBUTES m_pfnSetLayeredWindowAttributes;
		PFNUPDATELAYEREDWINDOW m_pfnUpdateLayeredWindow;

		typedef
			BOOL ( __stdcall * PFNPRINTWINDOW )(
				HWND hwnd,               // Window to copy
				HDC  hdcBlt,             // HDC to print into
				UINT nFlags              // Optional flags
				);
		PFNPRINTWINDOW m_pfnPrintWindow;

		HMODULE m_hDllSHLWAPI;

		typedef HRESULT ( WINAPI * PFNSHAUTOCOMPLETE )( HWND hWnd, DWORD dwFlags );
		PFNSHAUTOCOMPLETE m_pfnSHAutoComplete;

		
	public:
		// CExtPmSynchronizeSink methods
		virtual bool PmSynchronizeSink_PerformSynchronizationWith(
			CExtPmSynchronizeSink * pPmSynchronizeSink,
			bool bRegister = true,
			bool bSynchronizeItNow = true
			);
		virtual LPSTREAM PmSynchronizeSink_GetData(
			HGLOBAL hGlobal = NULL
			);
		virtual void PmSynchronizeSink_SynchronizeFrom(
			LPSTREAM pStream
			);

		bool m_bLayeredHighlighting2005:1;
	}; // class CExtPaintManagerAutoPtr

	// construction/destruction
	CExtPaintManager();
	virtual ~CExtPaintManager();

	virtual void SerializeSynchronizationData( CArchive & ar );

	static CExtAnimationParameters g_DefAnimationParametersEmpty;
	static CExtAnimationParameters g_DefAnimationParametersNormal;
	static CExtAnimationParameters g_DefAnimationParametersSlow;
	static CExtAnimationParameters g_DefAnimationParametersVerySlow;
	static CExtAnimationParameters g_DefAnimationParametersVerySlowAndSmooth;
	virtual const CExtAnimationParameters *
		Animation_GetParameters(
			INT eAPT, // __EAPT_*** animation type
			CObject * pHelperSrc,
			const CExtAnimationClient * pAC,
			LPARAM lParam = 0L
			) const;

	// paint session complete event - free all cached objects
	virtual void OnPaintSessionComplete(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	virtual bool NcFrame_IsSupported(
		const CWnd * pWnd,
		LPARAM lParam = 0L
		) const;
	virtual HRGN NcFrame_GenerateSkinFrameRGN(
		const RECT & rcWnd,
		const CWnd * pWnd,
		LPARAM lParam = 0L
		) const;
	virtual INT NcFrame_GetCaptionHeight(
		bool bActive,
		const CWnd * pWnd,
		LPARAM lParam = 0L
		) const;
	virtual void NcFrame_GetMetrics(
		RECT & rcNcBorders,
		RECT & rcThemePadding,
		const CWnd * pWnd,
		LPARAM lParam = 0L
		) const;
	enum e_nc_button_state_t
	{
		__ENCBS_DISABLED = 0,
		__ENCBS_NORMAL = 1,
		__ENCBS_HOVER = 2,
		__ENCBS_PRESSED = 3,
	};
	virtual void NcFrame_Paint(
		CDC & dc,
		const CExtCmdIcon * pIcon,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		UINT nDrawTextAlignFlags,
		const RECT & rcFrame,
		const RECT & rcClient,
		const RECT & rcIcon,
		const RECT & rcText,
		const RECT & rcHelp,
		const RECT & rcMinimize,
		const RECT & rcMaximizeRestore,
		const RECT & rcClose,
		bool bFrameActive,
		bool bFrameEnabled,
		bool bFrameMaximized,
		e_nc_button_state_t eStateButtonHelp,
		e_nc_button_state_t eStateButtonMinimize,
		e_nc_button_state_t eStateButtonMaximizeRestore,
		e_nc_button_state_t eStateButtonClose,
		const CWnd * pWnd,
		LPARAM lParam = 0L
		) const;
	virtual CRect NcFrame_GetHtRect(
		UINT nHT,
		bool bScreenMapping,
		bool bLayoutBordersH,
		bool bLayoutBordersV,
		const CExtCmdIcon * pIcon,
		const CWnd * pWnd,
		LPMINMAXINFO pMinMaxInfo = NULL,
		LPARAM lParam = 0L
		) const;
	virtual void NcFrame_GetRects(
		CExtCmdIcon * pIcon,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		UINT nDrawTextAlignFlags,
		RECT & rcIcon,
		RECT & rcText,
		RECT & rcHelp,
		RECT & rcMinimize,
		RECT & rcMaximizeRestore,
		RECT & rcClose,
		const CWnd * pWnd,
		LPMINMAXINFO pMinMaxInfo = NULL,
		LPARAM lParam = 0L
		) const;

	virtual bool Bar_MiniDockFrameGetNcMetrics(
		INT & nResizingFrameDX,
		INT & nResizingFrameDY,
		INT & nCaptionDY,
		CExtMiniDockFrameWnd * pMiniFrameWnd,
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual bool Bar_MiniDockFrameNcCalcSize(
		RECT & rcAdjust,
		CExtMiniDockFrameWnd * pMiniFrameWnd,
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual bool Bar_MiniDockFrameCaptionCalcRect(
		RECT & rcCaption,
		CExtMiniDockFrameWnd * pMiniFrameWnd,
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual void PaintResizableBarSeparator(
		CDC & dc,
		const RECT & rc,
		bool bHorz,
		CExtControlBar * pBar,
		LPARAM lParam = 0
		);
	virtual bool Bar_LayoutNcAreaButton(
		RECT & rcButton,
		const RECT & rcCaption,
		CExtBarNcAreaButton * pLayoutBtn,
		CExtBarNcAreaButton * pPrevBtn,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual CRect Bar_GetCaptionBordersForText(
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual INT Bar_SeparatorWidthGet(
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual INT Bar_SeparatorHeightGet(
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual INT Bar_GripperWidthAtLeftGet(
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual INT Bar_GripperHeightAtTopGet(
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual HFONT Bar_GetCaptionFont(
		bool bHorz,
		CExtToolControlBar * pBar,
		LPARAM lParam = 0
		);
	virtual CSize Toolbar_GetMinButtonSize(
		CObject * pHelperSrc,
		LPARAM lParam = 0
		);
	virtual void Toolbar_AdjustButtonSize(
		CExtBarButton * pTBB,
		CSize & sizePreCalc
		);
	virtual HFONT Toolbar_GetFont(
		bool bHorz,
		CExtControlBar * pBar,
		LPARAM lParam = 0
		);
	virtual bool Toolbar_GetBGInfo(
		RECT & rcBorders,
		INT & nGripWidthAtLeft,
		INT & nGripHeightAtTop,
		CExtToolControlBar * pBar,
		LPARAM lParam = 0
		) const;
	virtual bool Toolbar_GetSizeOfCEB(
		SIZE & sizeReal,
		const SIZE & sizePreCalc,
		CDC & dc,
		BOOL bHorz,
		CExtBarContentExpandButton * pTBB,
		LPARAM lParam = 0
		);

#ifndef __EXT_MFC_NO_STATUSBAR
	virtual bool StatusBar_QuerySkinSupport(
		const CExtStatusControlBar * pStatusBar,
		LPARAM lParam = 0
		) const;
	virtual bool StatusBar_EraseBackground(
		CDC & dc,
		const RECT & rcClient,
		const CExtStatusControlBar * pStatusBar,
		LPARAM lParam = 0
		) const;
	virtual bool StatusBar_ErasePaneBackground(
		CDC & dc,
		int nPaneIdx,
		const RECT & rcPane,
		const CExtStatusControlBar * pStatusBar,
		LPARAM lParam = 0
		) const;
	virtual bool StatusBar_PaintPane(
		CDC & dc,
		int nPaneIdx,
		const RECT & rcPane,
		DWORD dwPaneStyle,
		HICON hIcon,
		__EXT_MFC_SAFE_LPCTSTR sPaneText,
		UINT nDrawTextFlags,
		COLORREF clrText,
		const CExtStatusControlBar * pStatusBar,
		LPARAM lParam = 0
		) const;
	virtual bool StatusBar_PaintSeparator(
		CDC & dc,
		int nPaneIdxAfterSep,
		const RECT & rcPaneBefore,
		const RECT & rcPaneAfter,
		const CExtStatusControlBar * pStatusBar,
		LPARAM lParam = 0
		) const;
#endif // __EXT_MFC_NO_STATUSBAR

	virtual bool AdjustControlFont(
		HFONT & hFont,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual DLGTEMPLATE * AdjustDialogTemplate(
		const DLGTEMPLATE * lpDialogTemplate,
		CObject * pHelperSrc,
		bool bWizard = false,
		LPARAM lParam = 0L
		);
	virtual HFONT CreateDefaultFont();
	virtual HFONT CreateCaptionFont();

	// init part of color table and system brushes table
	bool m_bUseTahomaFont:1; // uses in SyncSysColors() to re-init font
	virtual void SyncSysColors();

	// get any system color based brush
	virtual HBRUSH GetBrush( int nColorIndex );

	// get any system or translated color value
	virtual COLORREF GetColor(
		int nColorIndex,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	COLORREF GetColor(
		int nColorIndex,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		) const;
	virtual COLORREF GetMenuFrameFillColor(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual COLORREF GetShadowAdjustColor();
	virtual void FixedBar_AdjustClientRect(
		const CExtControlBar * pBar,
		CRect & rcPreCalc
		);
	virtual INT FixedBar_GetRowDistance(
		const CExtToolControlBar * pBar
		);
	virtual bool FixedBar_IsPaintRowBkMode(
		const CExtToolControlBar * pBar
		);

	// install new color
	int InstallColor(
		COLORREF clr,
		int nColorIndex = -1
		);
	
	// install required translated color as
	// described in e_translated_colors_t enumeration
	virtual void InitTranslatedColors();

	// init lighter/darker 3d-face-like brushes
	virtual void InitHelperBrushes();


	// used always by default used translated colors
	enum e_translated_colors_t
	{
		CLR_3DFACE_OUT			= __ExtMfc_COLOR_MAP_BASE + 0,
		CLR_3DFACE_IN			= __ExtMfc_COLOR_MAP_BASE + 1,
		CLR_3DFACE_DISABLED		= __ExtMfc_COLOR_MAP_BASE + 2,

		CLR_3DLIGHT_OUT			= __ExtMfc_COLOR_MAP_BASE + 3,
		CLR_3DLIGHT_IN			= __ExtMfc_COLOR_MAP_BASE + 4,
		CLR_3DLIGHT_DISABLED	= __ExtMfc_COLOR_MAP_BASE + 5,

		CLR_3DHILIGHT_OUT		= __ExtMfc_COLOR_MAP_BASE + 6,
		CLR_3DHILIGHT_IN		= __ExtMfc_COLOR_MAP_BASE + 7,
		CLR_3DHILIGHT_DISABLED	= __ExtMfc_COLOR_MAP_BASE + 8,

		CLR_3DSHADOW_OUT		= __ExtMfc_COLOR_MAP_BASE + 9,
		CLR_3DSHADOW_IN			= __ExtMfc_COLOR_MAP_BASE + 10,
		CLR_3DSHADOW_DISABLED	= __ExtMfc_COLOR_MAP_BASE + 11,

		CLR_3DDKSHADOW_OUT		= __ExtMfc_COLOR_MAP_BASE + 12,
		CLR_3DDKSHADOW_IN		= __ExtMfc_COLOR_MAP_BASE + 13,
		CLR_3DDKSHADOW_DISABLED = __ExtMfc_COLOR_MAP_BASE + 14,
		
		CLR_TEXT_OUT			= __ExtMfc_COLOR_MAP_BASE + 15,
		CLR_TEXT_IN				= __ExtMfc_COLOR_MAP_BASE + 16,
		CLR_TEXT_DISABLED		= __ExtMfc_COLOR_MAP_BASE + 17,

		// Menu text colors
		CLR_MENUTEXT_OUT		= __ExtMfc_COLOR_MAP_BASE + 18,
		CLR_MENUTEXT_IN			= __ExtMfc_COLOR_MAP_BASE + 19,
		CLR_MENUTEXT_DISABLED	= __ExtMfc_COLOR_MAP_BASE + 20,

		// Task Pane colors
		CLR_TASK_PANE_BK_TOP	= __ExtMfc_COLOR_MAP_BASE + 21,
		CLR_TASK_PANE_BK_BOTTOM	= __ExtMfc_COLOR_MAP_BASE + 22,

		CLR_TASK_PANE_GROUP_CAPTION_TEXT_NORMAL				= __ExtMfc_COLOR_MAP_BASE + 23,
		CLR_TASK_PANE_GROUP_CAPTION_TEXT_NORMAL_HOVERED		= __ExtMfc_COLOR_MAP_BASE + 24,
		CLR_TASK_PANE_GROUP_CAPTION_TEXT_HIGHLIGHTED		= __ExtMfc_COLOR_MAP_BASE + 25,
		CLR_TASK_PANE_GROUP_CAPTION_TEXT_HIGHLIGHTED_HOVERED= __ExtMfc_COLOR_MAP_BASE + 26,
	
		CLR_TASK_PANE_GROUP_CAPTION_BK_LEFT_NORMAL			= __ExtMfc_COLOR_MAP_BASE + 27,
		CLR_TASK_PANE_GROUP_CAPTION_BK_LEFT_HIGHLIGHTED		= __ExtMfc_COLOR_MAP_BASE + 28,
		CLR_TASK_PANE_GROUP_CAPTION_BK_RIGHT_NORMAL			= __ExtMfc_COLOR_MAP_BASE + 29,
		CLR_TASK_PANE_GROUP_CAPTION_BK_RIGHT_HIGHLIGHTED	= __ExtMfc_COLOR_MAP_BASE + 30,

		// Page Navigator colors
		CLR_PN_CAPTION_TEXT		= __ExtMfc_COLOR_MAP_BASE + 31,

		// Group Box colors
		CLR_GROUP_BOX_FRAME		= __ExtMfc_COLOR_MAP_BASE + 32,

		// Resizable Bar non client area color
		CLR_WRB_FRAME = __ExtMfc_COLOR_MAP_BASE + 33,
	};

	// align types
	enum e_align_t
	{
		__ALIGN_HORIZ_LEFT		= 0x00000000L,
		__ALIGN_HORIZ_CENTER	= 0x00000001L,
		__ALIGN_HORIZ_RIGHT		= 0x00000002L,
		__ALIGN_HORIZ_MASK		= 0x00000003L,
		__ALIGN_VERT_TOP		= 0x00000004L,
		__ALIGN_VERT_CENTER		= 0x00000008L,
		__ALIGN_VERT_BOTTOM		= 0x00000010L,
		__ALIGN_VERT_MASK		= 0x0000001CL,
	};

	enum e_ButtonFocusMode_t
	{
		__EBFM_CAPTION_AREA		= 0x00000000L,
		__EBFM_CLIENT_AREA		= 0x00000001L,
	};

	// gets the pushed metic offset
	virtual CSize GetPushedOffset();

	class __PROF_UIS_API glyph_t
	{
		CSize m_size; // size in pixels
		UINT m_nColors; // count of used indexes (including 0)
			// index 0 - transparent
		const UINT * m_pColorIndexes;
		bool m_bDynamicData;
	public:
		glyph_t();
		glyph_t(
			const SIZE & _size,
			UINT nColors,
			const UINT * pColorIndexes
			);
		glyph_t(
			int cx,
			int cy,
			UINT nColors,
			const UINT * pColorIndexes
			);
		glyph_t(
			const glyph_t & other,
			UINT nRotateAngle = 0
				);
		~glyph_t();

		const glyph_t & operator = ( const glyph_t & other )
		{
			_CopyFromOther(other);
		#ifdef _DEBUG
			if( other.m_pColorIndexes != NULL )
			{
				ASSERT( m_pColorIndexes != NULL );
				ASSERT( m_pColorIndexes != other.m_pColorIndexes );
				ASSERT( m_bDynamicData );
			}
		#endif // _DEBUG
			return *this;
		}

		UINT GetColorPixel(
			int x,
			int y
			) const;
		void Rotate(UINT nRotateAngle);
		CSize Size() const;
		operator CSize() const
		{
			return Size();
		}
		const UINT * GetSurface() const;
		UINT GetColorsCount() const;

	private:
		void _Init();
		void _Done();
		bool _CopyFromOther( const glyph_t & other );
		void _SetColorPixel(
			int x,
			int y,
			UINT nValue
			);
	public:
	
		static bool stat_DefGlyphRgnTransparencyFunc(
			const glyph_t & _glyph,
			int x,
			int y,
			UINT nColor,
			LPVOID lpCookie
			);
		CRgn & GenerateRgn(
			CRgn & rgn,
			bool (*pGlyphRgnTransparencyFunc)(
				const glyph_t & _glyph,
				int x,
				int y,
				UINT nColor,
				LPVOID lpCookie
				) = stat_DefGlyphRgnTransparencyFunc,
			LPVOID lpCallbackCookie = NULL,
			LPLONG pMinNonTransparentX = NULL,
			LPLONG pMinNonTransparentY = NULL,
			LPLONG pMaxNonTransparentX = NULL,
			LPLONG pMaxNonTransparentY = NULL,
			LPLONG pMinTransparentX = NULL,
			LPLONG pMinTransparentY = NULL,
			LPLONG pMaxTransparentX = NULL,
			LPLONG pMaxTransparentY = NULL
			) const;
	}; // class glyph_t

	static const glyph_t g_glyph_btn_expand_left;
	static const glyph_t g_glyph_btn_expand_right;
	static const glyph_t g_glyph_btn_expand_right2;
	static const glyph_t g_glyph_btn_expand_bottom;
	static const glyph_t g_glyph_btn_expand_bottom2;

	static const glyph_t g_glyph_btn_expand_left_2003;
	static const glyph_t g_glyph_btn_expand_right_2003;
	static const glyph_t g_glyph_btn_expand_right2_2003;
	static const glyph_t g_glyph_btn_expand_bottom_2003;
	static const glyph_t g_glyph_btn_expand_bottom2_2003;

	static const glyph_t g_glyph_customize_popup_src;
	static const glyph_t g_glyph_customize_dropdown_src;

	static const glyph_t g_glyph_task_pane_btn_expand_up;
	static const glyph_t g_glyph_task_pane_btn_expand_down;

	static const BYTE g_arrWinXpTreeBoxCollapsedBmpData_small[];
	static const BYTE g_arrWinXpTreeBoxExpandedBmpData_small[];
	static const BYTE g_arrWinXpTreeBoxCollapsedBmpData_big[];
	static const BYTE g_arrWinXpTreeBoxExpandedBmpData_big[];

	virtual void LoadWinXpTreeBox(
		CExtCmdIcon & _icon,
		bool bExpanded,
		bool bBig = false
		);

	virtual void PaintGlyph(
		CDC & dc,
		POINT ptDest,
		const glyph_t & glyph,
		COLORREF * pColorValues
		);
	virtual void PaintGlyphCentered(
		CDC & dc,
		const RECT & rect,
		const glyph_t & glyph,
		COLORREF * pColorValues
		);

	virtual void PaintFloatToolbarRowBk(
		CDC & dc,
		const CExtToolControlBar * pBar,
		int nLastReviewBtnIdx,
		CRect & rcRowBk
		);

	virtual void PaintToolbarExpandButton(
		CDC & dc,
		const RECT & rcButtonArea,
		bool bHorz, // if false - down
		bool bBarIsCompletelyVisible,
		bool bEnabled,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		bool bTransparentBackground = false
		);
	virtual void PaintMenuExpandButton(
		CDC & dc,
		const RECT & rectButton,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintMenuScrollButton(
		CDC & dc,
		bool bHover,
		bool bPushed,
		bool bTopButton,
		const RECT & rcScrollButton,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintMenuTearOffButton(
		CDC & dc,
		const RECT & rectButton,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual int GetMenuExpandButtonHeight();

	// statically implemented paint/helper algorithms

	static DWORD stat_DllGetVersion( __EXT_MFC_SAFE_LPCTSTR lpszDllName );

	static HBITMAP stat_GetScreenSurfacePart(
		const RECT & rcScreenSurfacePart,
		COLORREF ** ppClrSurface = NULL
		);

	static HBITMAP stat_PrintWnd(
		HWND hWnd,
		UINT nMessage = WM_PRINTCLIENT,
		LPARAM lParamPrint = PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN,
		HDC hSrcDC = NULL,
		const RECT * pRectFillSrc = NULL
		);
	static void stat_PrintChildren(
		HWND hWndStart,
		UINT nMessage,
		HDC hDC,
		LPARAM lParamPrint,
		bool bDeepTreeWalk = true,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	static LONG stat_PassPaintMessages(
		HWND hWnd,
		bool bPassNcPaint = true,
		bool bPassEraseBkgnd = true,
		bool bPassPaint = true
		);
	static LONG stat_PassPaintMessages(
		bool bPassNcPaint = true,
		bool bPassEraseBkgnd = true,
		bool bPassPaint = true
		);

#if (defined __EXT_PM_BACK_COMPATIBILITY_BITMAP_API)
	static bool stat_ResizeBitmap(
		CBitmap & bmpSrc,
		CSize _size,
		CBrush & brushFill
		);
	static HICON stat_HBITMAPtoHICON(
		HBITMAP hBmpSrc,
		COLORREF clrTransparent,
		const RECT * pRcPart = NULL,
		bool bReplaceSysColors = true
		);
	static bool stat_CloneBitmap(
		CBitmap & bmpDst,
		const CBitmap & bmpSrc,
		const RECT * pRcPart = NULL,
		COLORREF ** ppSurface = NULL // if not NULL - create 32 bit surface
		);
	static HBITMAP stat_CloneBitmap(
		HBITMAP hBmpSrc,
		const RECT * pRcPart = NULL,
		COLORREF ** ppSurface = NULL // if not NULL - create 32 bit surface
		);
	static INT stat_GetBitmapColorMap( // returns bitmap's color bit count or zero if error
		HBITMAP hBmpSrc,
		CMap < COLORREF, COLORREF, DWORD, DWORD > & _map // key is color, value is count of color entries in bitmap
		);
	static INT stat_GetBitmapColorArray( // returns bitmap's color bit count or zero if error
		HBITMAP hBmpSrc,
		CArray < COLORREF, COLORREF > & arr,
		bool bMakeUniqueColors = true // used only if bit count <= 8
		);
	static HGLOBAL stat_HBITMAPtoHGLOBAL(
		HBITMAP hBmpSrc,
		HPALETTE hPalDst = NULL
		);
	static HBITMAP stat_LoadBitmapFromMemoryData(
		const BYTE * pDataBmp,
		DWORD dwSizeDataBmp
		);
	static bool stat_SerializeBitmap(
		CArchive & ar,
		HBITMAP * p_hBmp,
		DWORD dwResourceSize = 0L // not 0 only if loaded from resources
		);
	static bool stat_SerializeBitmap(
		__EXT_MFC_SAFE_LPCTSTR strFileName,
		bool bRead,
		HBITMAP * p_hBmp
		);
	static HBITMAP stat_LoadBitmapResource(
		HINSTANCE hInst,
		HRSRC hRsrc
		);
	static HBITMAP stat_LoadBitmapResource(
		__EXT_MFC_SAFE_LPCTSTR sResourceID,
		__EXT_MFC_SAFE_LPCTSTR sResourceType = RT_BITMAP, // default bitmap section
		HINSTANCE hInst = NULL // find it automatically
		);
#endif // (defined __EXT_PM_BACK_COMPATIBILITY_BITMAP_API)

	struct monitor_parms_t
	{
		CRect m_rcMonitor, m_rcWorkArea;
		bool m_bPrimaryMonitor:1;
		monitor_parms_t()
			: m_rcMonitor( 0, 0, 0, 0 )
			, m_rcWorkArea( 0, 0, 0, 0 )
			, m_bPrimaryMonitor( true )
		{
		}
	}; // struct monitor_parms_t
	static void stat_GetMonitorParms(
		monitor_parms_t & _mp
		);
	static void stat_GetMonitorParms(
		monitor_parms_t & _mp,
		const POINT & _ptScreenFrom
		);
	static void stat_GetMonitorParms(
		monitor_parms_t & _mp,
		const RECT & _rcScreenFrom
		);
	static void stat_GetMonitorParms(
		monitor_parms_t & _mp,
		HWND hWndFrom
		);
	static void stat_GetMonitorParms(
		monitor_parms_t & _mp,
		CWnd * pWndFrom
		);
	static CRect stat_AlignWndRectToMonitor(
		CRect rcInitial,
		bool bUseWorkArea = true, // if false - use all monitor area
		bool bNoPartialVisibility = false
		);

	static void stat_DrawDotLineH(
		CDC & dc,
		int x1,
		int x2,
		int y,
		COLORREF clrDots
		);
	static void stat_DrawDotLineV(
		CDC & dc,
		int x,
		int y1,
		int y2,
		COLORREF clrDots
		);

#if (defined __EXT_PM_BACK_COMPATIBILITY_COLOR_API)

	static COLORREF stat_RGBtoCMY( COLORREF clr );
	static COLORREF stat_CMYtoRGB( COLORREF clr );

	//----------------------------------------------------------------------
	// Conversion between the HSL (Hue, Saturation, and Luminosity) 
	// and RBG color model.
	//----------------------------------------------------------------------
	// The conversion algorithms presented here come from the book by 
	// Fundamentals of Interactive Computer Graphics by Foley and van Dam. 
	// In the example code, HSL values are represented as floating point 
	// number in the range 0 to 1. RGB tridrants use the Windows convention 
	// of 0 to 255 of each element. 
	//----------------------------------------------------------------------
	static double stat_HuetoRGB(double m1, double m2, double h );
	static COLORREF stat_HLStoRGB( double H, double L, double S );
	static void stat_RGBtoHSL( COLORREF rgb, double *H, double *S, double *L );

	static BYTE stat_GetRGBFromHue(float rm1, float rm2, float rh);
	static COLORREF stat_GetRGBFromHLSExtend( double H, double L, double S );

	static COLORREF stat_HLS_Adjust(
		COLORREF clr,
		double percentH = 0.0,
		double percentL = 0.0,
		double percentS = 0.0
		);

	static COLORREF stat_RGB_Enlight( 
		COLORREF clrLight, 
		COLORREF clrDark, 
		INT nEnlighten
		);

	static COLORREF stat_RGB_Blend( 
		COLORREF clrFrom, 
		COLORREF clrTo, 
		INT nAlpha
		);

#endif // (defined __EXT_PM_BACK_COMPATIBILITY_COLOR_API)

	static BOOL stat_PaintParentBk(
		HWND hWnd,
		HDC hDC,
		LPCRECT rectClip = NULL
		);

	typedef bool (*pfnExcludeChildAreaCallback)(
		HDC hDC,
		HWND hWnd,
		HWND hWndChild,
		LPVOID pCookie
		);

	static void stat_ExcludeChildAreas(
		HDC hDC,
		HWND hWnd,
		pfnExcludeChildAreaCallback pCallback = NULL,
		LPVOID pCallbackCookie = NULL
		);

	static void stat_TransparentBlt(
		HDC hdcDest,        // handle to destination DC
		int nXOriginDest,   // x-coord of destination upper-left corner
		int nYOriginDest,   // y-coord of destination upper-left corner
		int nWidthDest,     // width of destination rectangle
		int hHeightDest,    // height of destination rectangle
		HDC hdcSrc,         // handle to source DC
		int nXOriginSrc,    // x-coord of source upper-left corner
		int nYOriginSrc,    // y-coord of source upper-left corner
		int nWidthSrc,      // width of source rectangle
		int nHeightSrc,     // height of source rectangle
		UINT crTransparent  // color to make transparent
		);

	// generate DIB for UI light brush (when BPP <=8)
	static HBITMAP stat_GenLBDIB(
		COLORREF clrDarker = COLORREF(-1), // by default COLORREF(-1) is changed to ::GetSysColor(COLOR_BTNFACE)
		COLORREF clrLighter = COLORREF(-1), // by default COLORREF(-1) is changed to ::GetSysColor(COLOR_BTNHIGHLIGHT)
		int nColorMaskHorz = 1,
		int nColorMaskVert = 1,
		int nSkipMaskHorz = 0,
		int nSkipMaskVert = 0,
		COLORREF clrSkip = COLORREF(-1L) // // default COLORREF(-1) is changed to clrDarker
		);

	static void stat_PaintGradientRect(
		CDC & dc,
		const CRect & rcPaintGradient,
		COLORREF clrLeft,
		COLORREF clrRight,
		bool bHorz = false,
		UINT nCountOfSteps = 256
		);
	static void stat_PaintGradientRect2steps(
		CDC & dc,
		const CRect & rcPaintGradient,
		COLORREF clrLeft,
		COLORREF clrMiddle,
		COLORREF clrRight,
		bool bHorz = false,
		INT nNumerator = 2,
		INT nDenominator = 5,
		UINT nCountOfSteps = 256
		);

	static void stat_PaintDotNet3dRect(
		CDC & dc,
		LPCRECT lpRect,
		COLORREF clrTopLeft,
		COLORREF clrBottomRight
		);
	static void stat_PaintDotNet3dRect(
		CDC & dc,
		int x,
		int y,
		int cx,
		int cy,
		COLORREF clrTopLeft,
		COLORREF clrBottomRight
		);

	static void stat_PaintRadioDot( CDC & dc, int x, int y, COLORREF color );
	static void stat_PaintIndeterminatedSquare( CDC & dc, int x, int y, COLORREF color );
	static void stat_PaintCheckMark( CDC & dc, int x, int y, COLORREF color );

#if (!defined __EXT_MFC_NO_TAB_CTRL)

	static void stat_PaintTabItemImpl(
		CDC & dc,
		CRect & rcTabItemsArea,
		bool bTopLeft,
		bool bHorz,
		bool bSelected,
		bool bEnabled,
		bool bCenteredText,
		bool bGroupedMode,
		bool bInGroupActive,
		bool bInvertedVerticalMode,
		bool bDrawIcon,
		const CRect & rcEntireItem,
		CSize sizeTextMeasured,
		CFont * pFont,
		__EXT_MFC_SAFE_LPCTSTR sText,
		bool bNoPrefix,
		CExtCmdIcon * pIcon,
		CExtCmdIcon * pIconTabItemCloseButton,
		INT nPaintStateITICB,
		CRect & rcTabItemCloseButton,
		COLORREF clrText,
		COLORREF clrTabBk,
		COLORREF clrTabBorderLT,
		COLORREF clrTabBorderRB,
		COLORREF clrTabSeparator,
		bool bEnableEndEllipsis = true,
		CObject * pHelperSrc = NULL,
		bool bDwmMode = false
		);

	static void stat_PaintTabButtonImpl(
		CDC & dc,
		CRect & rcButton,
		LONG nHitTest,
		bool bTopLeft,
		bool bHorz,
		bool bEnabled,
		bool bHover,
		bool bPushed,
		bool bGroupedMode,
		COLORREF clrGlyph,
		COLORREF clrTL,
		COLORREF clrBR,
		bool bFlat = false
		);

	static void stat_PaintTabClientAreaImpl(
		CDC & dc,
		CRect & rcClient,
		CRect & rcTabItemsArea,
		CRect & rcTabNearBorderArea,
		DWORD dwOrientation,
		bool bGroupedMode,
		COLORREF clrTabItemsAreaBk,
		COLORREF clrTabNearBorderAreaBk,
		COLORREF clrTabNearMargin
		);

	virtual void GetThemeAccentTabColors(
		COLORREF * pClrSetectedTabItemText,
		COLORREF * pClrSetectedTabItemFace = NULL,
		COLORREF * pClrSetectedTabItemBorderLight = NULL,
		COLORREF * pClrSetectedTabItemBorderDark = NULL,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual void PaintTabItem(
		CDC & dc,
		CRect & rcTabItemsArea,
		bool bTopLeft,
		bool bHorz,
		bool bSelected,
		bool bCenteredText,
		bool bGroupedMode,
		bool bInGroupActive,
		bool bInvertedVerticalMode,
		const CRect & rcEntireItem,
		CSize sizeTextMeasured,
		CFont * pFont,
		__EXT_MFC_SAFE_LPCTSTR sText,
		CExtCmdIcon * pIcon,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		COLORREF clrForceText = COLORREF(-1L),
		COLORREF clrForceTabBk = COLORREF(-1L),
		COLORREF clrForceTabBorderLT = COLORREF(-1L),
		COLORREF clrForceTabBorderRB = COLORREF(-1L),
		COLORREF clrForceTabSeparator = COLORREF(-1L),
		bool bDwmMode = false
		);

	virtual bool QueryTabWndHoverChangingRedraw(
		const CExtTabWnd * pWndTab,
		LPARAM lParam = 0L
		);

	virtual void PaintTabClientArea(
		CDC & dc,
		CRect & rcClient,
		CRect & rcTabItemsArea,
		CRect & rcTabNearBorderArea,
		DWORD dwOrientation,
		bool bGroupedMode,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	
	virtual void PaintTabbedTabClientArea(
		CDC & dc,
		CRect & rcClient,
		CRect & rcTabItemsArea,
		CRect & rcTabNearBorderArea,
		DWORD dwOrientation,
		bool bGroupedMode,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	
	virtual void PaintTabButton(
		CDC & dc,
		CRect & rcButton,
		LONG nHitTest,
		bool bTopLeft,
		bool bHorz,
		bool bEnabled,
		bool bHover,
		bool bPushed,
		bool bGroupedMode,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		bool bFlat = false
		);

	virtual void PaintTabNcAreaRect(
		CDC & dc,
		const RECT & rc,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool PaintTabMdiOuterBorder(
		CDC & dc,
		const CRect & rcOuterBorder,
		const CRect & rcMdiAreaClient,
		const CRect & rcMdiAreaWnd,
		HWND hWndHooked,
		CExtTabWnd * pTabWnd,
		LPARAM lParam = 0L
		);

	virtual void TabWnd_AdjustTabAreaMetric( CExtTabWnd * pTabWnd, DWORD dwOrientation, LONG & nTabAreaMetric, LPARAM lParam = 0L );
	virtual void TabWnd_MeasureItemAreaMargins(
		CExtTabWnd * pTabWnd,
		LONG & nSpaceBefore,
		LONG & nSpaceAfter,
		LONG & nSpaceOver
		);	
	
	virtual void TabWnd_UpdateItemMeasure(
		CExtTabWnd * pTabWnd,
		CExtTabWnd::TAB_ITEM_INFO * pTii,
		CDC & dcMeasure,
		CSize & sizePreCalc
		);	
	virtual bool TabWnd_GetParentSizingMargin(
		INT & nMargin,
		DWORD dwOrientation,
		CExtTabWnd * pTabWnd
		) const;
	virtual void TabWnd_AdjustItemCloseButtonRect(
		CRect & rcCloseButton,
		CExtTabWnd * pTabWnd
		);	
#endif // #if (!defined __EXT_MFC_NO_TAB_CTRL)

	virtual void PaintResizableBarChildNcAreaRect(
		CDC & dc,
		const RECT & rc,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	static CRect stat_CalcTextDimension(
		CExtRichContentLayout::e_layout_format_t eLFMT,
		CExtRichContentLayout::e_layout_orientation_t eLO,
		CDC & dc,
		CFont & font,
		const CExtSafeString & strText,
		UINT uFlags = DT_SINGLELINE|DT_LEFT|DT_CALCRECT,
		UINT nExtDTF = __EXT_DT_CALC_EFFECTIVE_RECT
		);
	static CRect stat_CalcTextDimension(
		CDC & dc,
		CFont & font,
		const CExtSafeString & strText,
		UINT uFlags = DT_SINGLELINE|DT_LEFT|DT_CALCRECT,
		UINT nExtDTF = __EXT_DT_CALC_EFFECTIVE_RECT
		);
	static int stat_CalcTextWidth(
		CDC & dc,
		CFont & font,
		const CExtSafeString & strText
		);

	// paint methods

	virtual void PaintIcon(
		CDC & dc,
		bool bHorz,
		CExtCmdIcon * pIcon,
		const CRect & rcClient,
		bool bPushed,
		bool bEnabled,
		bool bHover = false,
		INT nIconAlignment = 0,
		bool bRTL = false,
		CRect * prcIcon = NULL,
		CRect * prcText = NULL,
		CRect * prcIconMargins = NULL,
		COLORREF clrCustomAccentEffectForIcon = COLORREF(-1L)
		);

	mutable CExtBitmap m_bmpCacheFilterApplied, m_bmpCacheFilterUnApplied;
	CExtBitmap & _GetBmpCacheFilter( bool bApplied ) const;

	enum e_docking_caption_button_t
	{
		__DCBT_EMPTY					= 0,
		__DCBT_CLOSE					= 1,
		__DCBT_ARROW_UP					= 2,
		__DCBT_ARROW_DOWN				= 3,
		__DCBT_ARROW_LEFT				= 4,
		__DCBT_ARROW_RIGHT				= 5,
		__DCBT_WND_MINIMIZE				= 6,
		__DCBT_WND_RESTORE				= 7,
		__DCBT_WND_MAXIMIZE				= 8,
		__DCBT_WND_CONTEXTHELP			= 9,
		__DCBT_WND_KEEP					= 10,
		__DCBT_WND_UNKEEP				= 11,
		__DCBT_ARROW_UP_L				= 12,
		__DCBT_ARROW_DOWN_L				= 13,
		__DCBT_ARROW_LEFT_L				= 14,
		__DCBT_ARROW_RIGHT_L			= 15,
		__DCBT_XP_THIN_CLOSE			= 16,
		__DCBT_HOLLOW_UP				= 17,
		__DCBT_HOLLOW_DOWN				= 18,
		__DCBT_HOLLOW_LEFT				= 19,
		__DCBT_HOLLOW_RIGHT				= 20,
		__DCBT_CLOSE_DC2K				= 21,
		__DCBT_HOLLOW_PLUS				= 22,
		__DCBT_HOLLOW_MINUS				= 23,
		__DCBT_TREE_PLUS				= 24,
		__DCBT_TREE_MINUS				= 25,
		__DCBT_CIRCLE_PLUS				= 26,
		__DCBT_CIRCLE_MINUS				= 27,
		__DCBT_ARROW_PT_UP				= 28,
		__DCBT_ARROW_PT_DOWN			= 29,
		__DCBT_ARROW_PT_LEFT			= 30,
		__DCBT_ARROW_PT_RIGHT			= 31,
		__DCBT_ARROW_PT_LT				= 32,
		__DCBT_ARROW_PT_RT				= 33,
		__DCBT_ARROW_PT_RB				= 34,
		__DCBT_ARROW_PT_LB				= 35,
		__DCBT_ARROW_SMALL_UP			= 36,
		__DCBT_ARROW_SMALL_DOWN			= 37,
		__DCBT_ARROW_SMALL_LEFT			= 38,
		__DCBT_ARROW_SMALL_RIGHT		= 39,
		__DCBT_ELLIPSIS					= 40,
		__DCBT_ARROW_SORT_UP			= 41,
		__DCBT_ARROW_SORT_DOWN			= 42,
		__DCBT_ARROW_SORT_LEFT			= 43,
		__DCBT_ARROW_SORT_RIGHT			= 44,
		__DCBT_TABS_LIST_ARROW			= 45,
		__DCBT_PN_CONFIG_BUTTON			= 46,
		__DCBT_PN_CONFIG_BUTTON_2007	= 47,
		__DCBT_PN_CAPTION_EXPANDED		= 48,
		__DCBT_PN_CAPTION_COLLAPSED		= 49,
		__DCBT_TREE_PLUS_FILLED			= 50,
		__DCBT_TREE_MINUS_FILLED		= 51,
		__DCBT_SCROLL_TOP				= 52,
		__DCBT_SCROLL_LEFT				= 53,
		__DCBT_SCROLL_BOTTOM			= 54,
		__DCBT_SCROLL_RIGHT				= 55,
		__DCBT_2007_ARROW_SORT_UP		= 56,
		__DCBT_2007_ARROW_SORT_DOWN		= 57,
		__DCBT_2007_ARROW_SORT_LEFT		= 58,
		__DCBT_2007_ARROW_SORT_RIGHT	= 59,

		__DCBT_VALUE_MIN				= 0,
		__DCBT_VALUE_MAX				= 59,
	};
	
	static glyph_t * g_DockingCaptionGlyphs[__DCBT_VALUE_MAX+1];

	struct __PROF_UIS_API PAINTDOCKINGCAPTIONBUTTONDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		CRect m_rcClient;
		int m_eType; // CExtPaintManager::e_docking_caption_button_t
		bool m_bHover:1;
		bool m_bPushed:1;
		bool m_bEnabled:1;
		bool m_bSideBar:1;
		bool m_bBarWndActive:1;
		bool m_bFloating:1;

		PAINTDOCKINGCAPTIONBUTTONDATA();
		PAINTDOCKINGCAPTIONBUTTONDATA(
			CObject * pHelperSrc,
			const RECT & rcClient,
			int eType, // CExtPaintManager::e_docking_caption_button_t
			bool bHover,
			bool bPushed,
			bool bEnabled,
			bool bSideBar,
			bool bBarWndActive,
			bool bFloating
			);
	}; // struct PAINTDOCKINGCAPTIONBUTTONDATA

	virtual void PaintDockingCaptionButton(
		CDC & dc,
		CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA & _pdcbd
		);

	virtual void PaintSysCmdGlyph(
		CDC & dc,
		UINT nHelperSysCmdID,
		const RECT & rcItem,
		bool bHover,
		bool bPushed,
		bool bEnabled,
		COLORREF clrForceGlyphColor = COLORREF(-1L)
		);

	struct __PROF_UIS_API PAINTPUSHBUTTONDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		CRect m_rcClient;
		__EXT_MFC_SAFE_LPCTSTR m_sText;
		CExtCmdIcon * m_pIcon;
		HFONT m_hFont;
		UINT m_nHelperSysCmdID;

		INT m_nTextAlignment;
		INT m_nIconAlignment;

		CRect m_rcBorderSizes;
		CRect m_rcIconMargins;
		CRect m_rcTextMargins;

		INT m_nFocusMode;

		bool m_bHorz:1;
		bool m_bFlat:1;
		bool m_bHover:1;
		bool m_bPushed:1;
		bool m_bChecked:1;
		bool m_bIndeterminate:1;
		bool m_bEnabled:1;
		bool m_bDrawBorder:1;
		bool m_bDrawFocusRect:1;
		bool m_bDefaultPushButton:1;
		bool m_bDropDown:1;
		bool m_bTransparentBackground:1;
		bool m_bSeparatedDropDown:1;
		bool m_bNoDropDownArrow:1;
		bool m_bPushedDropDown:1;
		bool m_bHoverDropDown:1;
		bool m_bHelperColorCellButton:1;

		// text properties
		bool m_bWordBreak:1;

		// custom colors
		COLORREF m_clrForceTextDisabled, m_clrForceTextNormal,
			m_clrForceTextHover, m_clrForceTextPressed,
			m_clrCustomAccentEffectForIcon;
		COLORREF m_clrForceBk;
		UINT m_nDrawTextFlagsH;
		bool m_bForceUseDrawTextApiH:1;
		bool m_bForceRtlLayout:1;
		bool m_bForceRtlText:1;

		PAINTPUSHBUTTONDATA();
		PAINTPUSHBUTTONDATA(
			CObject * pHelperSrc,
			bool bHorz,
			const RECT & rcClient,
			__EXT_MFC_SAFE_LPCTSTR sText,
			CExtCmdIcon * pIcon,
			bool bFlat,
			bool bHover,
			bool bPushed,
			bool bIndeterminate,
			bool bEnabled,
			bool bDrawBorder,
			bool bDrawFocusRect,
			bool bDefaultPushButton,
			int eAlign,
			HFONT hFont = NULL,
			bool bDropDown = false,
			UINT nHelperSysCmdID = 0,
			bool bTransparentBackground = false
			);

		bool IsRTL();
	}; // struct PAINTPUSHBUTTONDATA

	virtual LONG _CalcSplitButtonExtentImpl( const CObject * pObjButton, bool bHorz ) const;
	virtual LONG _CalcSplitButtonExtent( const CObject * pObjButton, bool bHorz ) const;

	virtual void PaintPushButton(
		CDC & dc,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);
	virtual void PaintPushButtonMdiRight(
		CDC & dc,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);
	virtual void PaintPushButtonText(
		CDC & dc,
		CRect & rcCaption,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);
	virtual void PaintPushButtonIcon(
		CDC & dc,
		CRect & rcClient,
		CRect & rcCaption,
		CRect & rcFocus,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);

	enum eBoxState_t
	{
		BOX_UNCHECKED					= 0, // unchecked state (check, radio)
		BOX_CHECKED						= 1, // checked state (check, radio)
		BOX_INDETERMINATE				= 2, // indeterminate state (check)
		
		BOX_LDOWN_UNCHECKED				= 3, // mouse down in unchecked state (check, radio)
		BOX_LDOWN_CHECKED				= 4, // mouse down in checked state (check, radio)
		BOX_LDOWN_INDETERMINATE			= 5, // mouse down in indeterminate state (check)
		
		BOX_MOUSE_HOVER_UNCHECKED		= 6, // mouse hover in unchecked state (check, radio)
		BOX_MOUSE_HOVER_CHECKED			= 7, // mouse hover in checked state (check, radio)
		BOX_MOUSE_HOVER_INDETERMINATE	= 8, // mouse hover in indeterminate state (check)
		
		BOX_DISABLED_UNCHECKED			= 9, // disabled in unchecked state (check, radio)
		BOX_DISABLED_CHECKED			= 10,// disabled in checked state (check, radio)
		BOX_DISABLED_INDETERMINATE		= 11 // disabled in indeterminate state (check)

	}; // enum eBoxState_t

	struct __PROF_UIS_API PAINTCHECKRADIOBUTTONDATA
	{
		eBoxState_t m_eState;

		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		CRect m_rcClient;
		__EXT_MFC_SAFE_LPCTSTR m_sText;
		HFONT m_hFont;
		
		INT m_nTextAlignment;
		CRect m_rcTextMargins;

		bool m_bLeftText:1;
		bool m_bMultiline:1;
		bool m_bHover:1;
		bool m_bPushed:1;
		bool m_bEnabled:1;
		bool m_bDrawFocusRect:1;
		bool m_bTransparentBackground:1;

		COLORREF m_clrForceTextDisabled;
		COLORREF m_clrForceTextNormal;
		COLORREF m_clrForceTextHover;
		COLORREF m_clrForceTextPressed;
		COLORREF m_clrForceBk;

		CRect m_rcBox;

		PAINTCHECKRADIOBUTTONDATA();
		PAINTCHECKRADIOBUTTONDATA(
			CObject * pHelperSrc,
			eBoxState_t eState,
			bool bLeftText,
			bool bMultiline,
			const RECT & rcClient,
			__EXT_MFC_SAFE_LPCTSTR sText,
			bool bHover,
			bool bPushed,
			bool bEnabled,
			bool bDrawFocusRect,
			INT eAlign,
			HFONT hFont = NULL,
			bool bTransparentBackground = false
			);
	}; // struct PAINTCHECKRADIOBUTTONDATA

	virtual CSize GetCheckButtonBoxSize(
		CDC & dc,
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd
		) const;
	virtual CSize GetRadioButtonBoxSize(
		CDC & dc,
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd
		) const;

	virtual void PaintCheckOrRadioButtonText(
		CDC & dc,
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd,
		CSize _sizeBox,
		CRect & rcBoxLocation
		);

	virtual void PaintCheckButtonBox(
		CDC & dc,
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd
		);
	virtual void PaintRadioButtonBox(
		CDC & dc,
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd
		);

	virtual void PaintCheckButton(
		CDC & dc,
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd
		);
	virtual void PaintRadioButton(
		CDC & dc,
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd
		);

	virtual CSize GetDropDividerMerics();
	virtual void PaintDropDivider(
		CDC & dc,
		const RECT & rcItem,
		bool bBefore,
		bool bHorz
		);
	virtual void PaintDragSourceRect(
		CDC & dc,
		const RECT & rcItem
		);

	struct __PROF_UIS_API PAINTMENUITEMDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		CRect m_rcItem;
		CRect m_rcItemWithIndents;
		INT m_nIconAreaWidth;
		__EXT_MFC_SAFE_LPCTSTR m_sText, m_sAccelText, m_sExtendedText;
		CExtCmdIcon * m_pIcon;
		UINT m_nHelperSysCmdID; // for painting system menu glyphs
		HFONT m_hCustomFont;

		bool m_bPopup:1;
		bool m_bSelected:1;
		bool m_bPushedCheck:1;
		bool m_bPushedRadio:1;
		bool m_bIndeterminate:1;
		bool m_bEnabled:1;
		bool m_bRarelyUsed:1;
		bool m_bRarelyUsedPrev:1;
		bool m_bRarelyUsedNext:1;
		bool m_bForceNoLeftGradient:1;
		bool m_bCustomizeListMode:1;
		bool m_bSeparatedDropDown:1;
		bool m_bXtraMark:1;
		bool m_bXtraChecked:1;
		bool m_bBigAccent:1;
		CRect m_rcInPlaceWnd;
		CExtSafeString m_strInPlaceText;
		bool m_bHelperPopupDropped:1;
		COLORREF m_clrCustomAccentEffectForIcon;
		COLORREF m_clrForceMenuText;

		PAINTMENUITEMDATA();
		PAINTMENUITEMDATA(
			CObject * pHelperSrc,
			const RECT & rcItem,
			const RECT & rcItemWithIndents,
			INT nIconAreaWidth,
			__EXT_MFC_SAFE_LPCTSTR sText,
			__EXT_MFC_SAFE_LPCTSTR sAccelText,
			CExtCmdIcon * pIcon,
			bool bPopup,
			bool bSelected,
			bool bPushedCheck,
			bool bPushedRadio,
			bool bIndeterminate,
			bool bEnabled,
			bool bRarelyUsed,
			bool bRarelyUsedPrev,
			bool bRarelyUsedNext,
			UINT nHelperSysCmdID, // for painting system menu glyphs
			HFONT hCustomFont
			);
	}; // struct PAINTMENUITEMDATA

	virtual int TearOff_GetCaptionHeight() const;
	virtual void TearOff_GetCaptionPaint(
		CDC & dc,
		const RECT & rectItem,
		bool bHover,
		bool bPressed,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	virtual void PaintMenuItem(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuItemIcon(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuItemInPlaceWnd(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuSeparator(
		CDC & dc,
		const RECT & rectItem,
		int nIconAreaWidth,
		bool bRarelyUsed,
		bool bIsForceNoLeftGradient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintSeparator(
		CDC & dc,
		const RECT & rectItem,
		bool bHorz,
		bool bTransparentBk,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	struct __PROF_UIS_API PAINTGRIPPERDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		const CRect m_rcGripper, m_rcText;
		bool m_bActive:1;
		bool m_bFloating:1;
		bool m_bHorz:1;
		bool m_bSideBar:1;
		__EXT_MFC_SAFE_LPCTSTR m_sCaption;
		bool m_bHelperNoRect:1;
		bool m_bHelperNoFill:1;
		bool m_bForceRTL:1;
		bool m_bFlashCaptionHighlightedState:1;
		COLORREF m_clrFlashCaptionText, m_clrFlashCaptionBackground;

		PAINTGRIPPERDATA();
		PAINTGRIPPERDATA(
			CObject * pHelperSrc,
			const RECT & rcGripper,
			const RECT & rcText,
			bool bActive,
			bool bFloating,
			bool bHorz,
			bool bSideBar = false,
			__EXT_MFC_SAFE_LPCTSTR sCaption = NULL,
			bool bForceRTL = false
			);
	}; // struct PAINTGRIPPERDATA

	virtual void PaintGripper(
		CDC & dc,
		CExtPaintManager::PAINTGRIPPERDATA & _pgd
		);

	struct __PROF_UIS_API PAINTDOCKINGFRAMEDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		CRect m_rcWindow;
		CRect m_rcClient;
		bool m_bActive:1;
		bool m_bFloating:1;
		bool m_bExtBar:1;
		bool m_bSideBar:1;

		PAINTDOCKINGFRAMEDATA();
		PAINTDOCKINGFRAMEDATA(
			CObject * pHelperSrc,
			const RECT & rcWindow,
			const RECT & rcClient,
			bool bActive,
			bool bFloating,
			bool bExtBar,
			bool bSideBar = false
			);
	}; // struct PAINTDOCKINGFRAMEDATA

	virtual void PaintDockingFrame(
		CDC & dc,
		CExtPaintManager::PAINTDOCKINGFRAMEDATA & _pdfd
		);

	struct __PROF_UIS_API PAINTCONTROLFRAMEDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		CRect m_rcClient;
		bool m_bFlat:1;
		bool m_bHover:1;
		bool m_bEnabled:1;
		bool m_bFocus:1;
		bool m_bReadOnly:1;

		PAINTCONTROLFRAMEDATA();
		PAINTCONTROLFRAMEDATA(
			CObject * pHelperSrc,
			const RECT & rcClient,
			bool bFlat,
			bool bHover,
			bool bEnabled,
			bool bFocus,
			bool bReadOnly
			);
	}; // struct PAINTCONTROLFRAMEDATA

	virtual void PaintControlFrame(
		CDC & dc,
		CExtPaintManager::PAINTCONTROLFRAMEDATA & _pcfd
		);

	struct __PROF_UIS_API PAINTCOMBOFRAMEDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		CRect m_rcClient;
		bool m_bHover:1;
		bool m_bPushed:1;
		bool m_bEnabled:1;
		COLORREF
			m_clrForceNABorder,
			m_clrForceNAContent,
			m_clrForceActiveOuterBorder
			;
		bool m_bNoTopOuterBorder:1;
		bool m_bRightText:1;

		PAINTCOMBOFRAMEDATA();
		PAINTCOMBOFRAMEDATA(
			CObject * pHelperSrc,
			const RECT & rcClient,
			bool bHover,
			bool bPushed,
			bool bEnabled,
			bool bRightText = false
			);
	}; // struct PAINTCOMBOFRAMEDATA

	virtual void PaintComboFrame(
		CDC & dc,
		CExtPaintManager::PAINTCOMBOFRAMEDATA & _pcfd
		);

	static HICON stat_GenerateColorIconBox(
		COLORREF clrIcon
		);
	static HICON stat_GenerateColorIconChar(
		COLORREF clrIcon,
		HFONT hFontForLetter,
		__EXT_MFC_SAFE_TCHAR tChr
		);
	static HICON stat_GenerateColorIconChar(
		COLORREF clrIcon,
		HFONT hFontForLetter
		);
	static HICON stat_GenerateColorIconFrame(
		COLORREF clrIcon
		);

	static bool stat_DefIsHwndNeedsDirectRepaint(
		HWND hWndChild
		);
	static bool stat_DefExcludeChildAreaCallback(
		HDC hDC,
		HWND hWnd,
		HWND hWndChild,
		LPVOID pCookie
		);

	static int stat_GetBPP();

	virtual INT GetDropDownButtonWidth(
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		) const;
	virtual INT GetSpinButtonWidth(
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		) const;
	virtual CRect GetSpinButtonMargins(
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		) const;

	CSize GetTextSizes(bool bBold = false);
	virtual int GetTextHeight(bool bHorz);

	virtual int GetTipShadowSize() const;
	virtual int GetMenuShadowSize() const;
	virtual int GetMenuIconAreaWidth() const;
	virtual CRect GetMenuBorderMetrics(
		CWnd * pWnd,
		LPARAM lParam = 0
		) const;
	virtual void MenuBottomResizingMarginGetInfo(
		INT & nMarginHeight,
		INT & nResizingCornerPartWidth,
		CExtPopupMenuWnd * pPopup,
		LPARAM lParam = 0L
		);
	virtual void MenuBottomResizingMarginPaint(
		CDC & dc,
		const RECT & rcResizingArea,
		CExtPopupMenuWnd * pPopup,
		LPARAM lParam = 0L
		);
	virtual CFont * MenuCaptionGetFont(
		CExtPopupMenuWnd * pPopup,
		LPARAM lParam = 0L
		) const;
	virtual CSize MenuCaption_Measure(
		CDC & dc,
		__EXT_MFC_SAFE_LPCTSTR sMenuCaptionText,
		CExtPopupMenuWnd * pPopup,
		LPARAM lParam = 0L
		) const;
	virtual void MenuCaption_Paint(
		CDC & dc,
		const RECT & rcMenuCaption,
		__EXT_MFC_SAFE_LPCTSTR sMenuCaptionText,
		CExtPopupMenuWnd * pPopup,
		LPARAM lParam = 0L
		) const;
	virtual void PaintMenuBorder(
		CDC & dc,
		const RECT & rectClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void AdjustMenuDC(
		CExtPopupMenuWnd * pPopup,
		CDC & dc,
		bool bMemoryDC,
		LPARAM lParam = 0L
		);

	virtual bool IsMenuMustCombineExcludeArea(
		const CObject * pMenuTrackerObject = NULL
		) const;
	virtual bool IsToolBarChevronMenuCombined();
	virtual void PaintMenuCombinedArea(
		CDC & dc,
		const RECT & rcArea,
		const RECT & rcClient,
		int eCombineAlign, // CExtPopupMenuWnd::e_combine_align_t values
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	enum e_control_bar_border_type_t
	{
		__CB_OUTER_DOCKBAR = 0,
		__CB_OUTER_STATUSBAR = 1,
		__CB_INNER_STATUSBAR_ITEM = 2,
	};

	struct __PROF_UIS_API PAINTCONTROLBARBORDERSDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;

		int m_eCBBT; // CExtPaintManager::e_control_bar_border_type_t
		DWORD m_dwStyle;
		CRect m_rc;

		PAINTCONTROLBARBORDERSDATA();
		PAINTCONTROLBARBORDERSDATA(
			CObject * pHelperSrc,
			int eCBBT, // CExtPaintManager::e_control_bar_border_type_t
			DWORD dwStyle,
			const RECT & rc
			);
	}; // struct PAINTCONTROLBARBORDERSDATA
	
	void PaintControlBarBordersDefImpl(
		CDC & dc,
		CExtPaintManager::PAINTCONTROLBARBORDERSDATA & _pcbbd,
		COLORREF clrHi,
		COLORREF clrLo
		);

	virtual void PaintControlBarBorders(
		CDC & dc,
		CExtPaintManager::PAINTCONTROLBARBORDERSDATA & _pcbbd
		);

	virtual int GetThemeDockingType() const;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	virtual int GetAutoHideTabsType() const;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	struct __PROF_UIS_API PAINTINHERITEDBACKGROUNDDATA
	{
		bool m_bClientMapping:1,
			m_bBackgroundDrawn:1,
			m_bSequenceCanceled:1;
		HWND m_hWndBackgroundRequestPainter;
		CDC & m_dc;
		CWnd * m_pWnd;
		CObject * m_pObjHelperSrc;
		LPARAM m_lParam;
		PAINTINHERITEDBACKGROUNDDATA(
			bool bClientMapping,
			CDC & dc,
			CWnd * pWnd,
			CObject * pObjHelperSrc = NULL,
			LPARAM lParam = 0L
			);
		PAINTINHERITEDBACKGROUNDDATA(
			bool bClientMapping,
			CDC & dc,
			CObject * pObjHelperSrc,
			LPARAM lParam = 0L
			);
		bool Notify();
		operator WPARAM() const;
		static PAINTINHERITEDBACKGROUNDDATA * FromWPARAM( WPARAM wParam );
		CRect GetRenderingRect() const;
	}; // struct PAINTINHERITEDBACKGROUNDDATA

	virtual bool RenderCustomBackground(
		bool bClientMapping,
		CDC & dc,
		CObject * pObjHelperSrc,
		LPARAM lParam = 0L
		) const;
	static UINT g_nMsgPaintInheritedBackground;
	bool m_bCustomBackgroundInheritanceEnabled:1;
	virtual bool IsCustomBackgroundInheritanceEnabled() const;
	virtual bool GetCustomBackgroundInheritanceMode(
		CObject * pObjHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual bool GetCb2DbTransparentMode(
		CObject * pObjHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool PaintDockerBkgnd(
		bool bClientMapping,
		CDC & dc,
		CWnd * pWnd,
		LPARAM lParam = NULL
		);
	virtual bool PaintDockerBkgnd(
		bool bClientMapping,
		CDC & dc,
		const CRect &rcDst,
		const CRect &rcWnd,
		LPARAM lParam = NULL
		);
	virtual bool PaintDocumentClientAreaBkgnd(
		CDC & dc,
		CWnd * pWnd,
		LPARAM lParam = NULL
		);

	virtual void PaintControlBarClientArea(
		CDC & dc,
		const RECT & rcClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	virtual void PaintDockBarClientArea(
		CDC & dc,
		const RECT & rcClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
	bool m_bShortcutListUseButtonStyleInScrollButtons:1;
	bool m_bShortcutListUseDockerBkgnd:1;
	bool m_bShortcutListItemUseButtonStyle:1;
	COLORREF m_clrShortcutListItemAlternativeText;
	virtual void PaintShortcutListBkgnd(
		CDC & dc,
		const RECT & rcClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintShortcutListScrollButton(
		CDC & dc,
		bool bHorzLayout,
		const RECT & rcButton,
		bool bUpOrLeftBtn,
		bool bHover,
		bool bPressed,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	struct __PROF_UIS_API PAINTSHORTCUTLISTITEMSDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;

		CExtCmdIcon * m_pIconBig, * m_pIconSmall;
		LPARAM m_lParamItem;
		CRect m_rcBoundsAll, m_rcBoundsIcon, m_rcBoundsText,
			m_rcCtrlClient;
		CExtSafeString m_sText;
		bool m_bHover:1, m_bPressed:1, m_bVisible:1,
			m_bChecked:1, m_bExpandMode:1, m_bHorzLayout:1,
			m_bBigIconView:1, m_bEraseQuery:1;
		DWORD m_dwShortcutListStyle;
		HFONT m_hItemTextFont;

		PAINTSHORTCUTLISTITEMSDATA();
		PAINTSHORTCUTLISTITEMSDATA(
			CObject * pHelperSrc,
			DWORD dwShortcutListStyle,
			HFONT hItemTextFont,
			CExtCmdIcon * pIconBig,
			CExtCmdIcon * pIconSmall,
			LPARAM lParamItem,
			const RECT & rcBoundsAll,
			const RECT & rcBoundsIcon,
			const RECT & rcBoundsText,
			const RECT & rcCtrlClient,
			__EXT_MFC_SAFE_LPCTSTR sText,
			bool bHover,
			bool bPressed,
			bool bVisible,
			bool bChecked,
			bool bExpandMode,
			bool bHorzLayout,
			bool BigIconView,
			bool bEraseQuery
			);
	}; // struct PAINTSHORTCUTLISTITEMSDATA

	virtual void PaintShortcutListItem(
		CDC & dc,
		CExtPaintManager::PAINTSHORTCUTLISTITEMSDATA & _pslid
		);
#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)

	virtual void OnSysColorChange();
	virtual void OnSettingChange(
		UINT uFlags,
		__EXT_MFC_SAFE_LPCTSTR lpszSection
		);
	virtual void OnDisplayChange(
		INT nDepthBPP,
		CPoint ptSizes
		);
	virtual void OnThemeChanged(
		WPARAM wParam,
		LPARAM lParam
		);
	virtual bool OnStateSerialize(
		CArchive & ar,
		bool bEnableThrowExceptions = false
		);

	virtual CSize GetToolBarButtonOffset( BOOL bHorz ) const;

	struct __PROF_UIS_API PAINTTOOLBARTEXTFIELDDATA
	{
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		UINT m_nDrawTextFlags;
		CExtSafeString m_sText;
		CRect m_rcButton, m_rcTextField;
		bool m_bComboField:1;
		bool m_bHover:1;
		bool m_bPushed:1;
		bool m_bEnabled:1;

		PAINTTOOLBARTEXTFIELDDATA();
		PAINTTOOLBARTEXTFIELDDATA(
			CObject * pHelperSrc,
			__EXT_MFC_SAFE_LPCTSTR sText,
			const RECT & rcButton,
			const RECT & rcTextField,
			bool bComboField,
			bool bHover,
			bool bPushed,
			bool bEnabled
			);
	}; // struct PAINTTOOLBARTEXTFIELDDATA

	virtual void PaintToolbarTextField(
		CDC & dc,
		PAINTTOOLBARTEXTFIELDDATA & _ptbtfd
		);

	virtual COLORREF GetIconAlphaColor() const;
	virtual double GetIconAlphaAdjustPercent() const;

	virtual bool AdjustIcon( 
		CExtCmdIcon * pIcon
		);

	virtual CSize FixedBar_GetRoundedAreaMerics() const;

	virtual CSize GetResizingGriperSize(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;

	virtual void PaintResizingGripper(
		CDC & dc,
		const RECT & rcGrip,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

#if (!defined __EXT_MFC_NO_PAGE_NAVIGATOR )
	virtual INT PageNavigator_GetSplitterHeight() const;
	virtual INT PageNavigator_GetItemCaptionHeight() const;
	virtual INT PageNavigator_GetItemPaneCaptionHeight() const;
	virtual void PaintPageNavigatorClientArea(
		CDC & dc,
		const RECT & rcClient,
		CObject * pHelperSrc
		);
	virtual void PaintPageNavigatorBorder(
		CDC & dc,
		const RECT & rcControl
		);
	virtual void PaintPageNavigatorItem(
		CDC & dc,
		const CRect & rcItem,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		CExtCmdIcon * pIcon,
		bool bExpanded,
		bool bSelected,
		bool bPushed,
		bool bHover
		);	
	virtual void PaintPageNavigatorItemCaptionText(
		CDC & dc,
		const CRect & rcCaption,
		__EXT_MFC_SAFE_LPCTSTR strCaption
		);	
	virtual void PaintPageNavigatorItemCaption(
		CDC & dc,
		const CRect & rcCaption,
		__EXT_MFC_SAFE_LPCTSTR strCaption
		);
	virtual void PaintPageNavigatorCaption(
		CDC & dc,
		const CRect & rcItem,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		CExtCmdIcon * pIcon,
		bool bExpanded,
		bool bSelected,
		bool bPushed,
		bool bHover,
		bool bAllowInvertTextColor = true,
		COLORREF clrForceText = COLORREF(-1L)
		);	
	virtual void PaintPageNavigatorCaptionTextImpl(
		CDC & dc,
		const CRect & rcItem,
		__EXT_MFC_SAFE_LPCTSTR strCaption
		);	
	virtual void PaintPageNavigatorCaptionText(
		CDC & dc,
		const CRect & rcItem,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		bool bSelected,
		bool bPushed,
		bool bHover,
		bool bAllowInvertTextColor = true,
		COLORREF clrForceText = COLORREF(-1L)
		);	
	virtual void PaintPageNavigatorItemPaneCaptionTextImpl(
		CDC & dc,
		const CRect & rcCaption,
		__EXT_MFC_SAFE_LPCTSTR strCaption
		);
	virtual void PaintPageNavigatorItemPaneCaptionText(
		CDC & dc,
		const CRect & rcCaption,
		__EXT_MFC_SAFE_LPCTSTR strCaption
		);
	virtual void PaintPageNavigatorItemPaneCaption(
		CDC & dc,
		const CRect & rcCaption,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		bool bHover,
		bool bPressed,
		bool bExpandable,
		bool bExpanded,
		bool bDrawTopLine = true
		);
	virtual void PaintPageNavigatorConfigButton(
		CDC & dc,
		const CRect & rcItem,
		bool bPushed,
		bool bHover
		);	
	virtual void PaintPageNavigatorSplitter(
		CDC & dc,
		const CRect & rcSplitter,
		bool bDrawDots = true
		);
	virtual void PaintPageNavigatorSplitterDots(
		CDC & dc,
		const CRect & rcSplitter,
		INT nOffsetV = 0
		);
	virtual void PaintPageNavigatorSplitterDotsImpl(
		CDC & dc,
		const CRect & rcSplitter,
		COLORREF clrDark,
		COLORREF clrLight,
		INT nOffsetV = 0,
		INT nDotCount = 9
		);
#endif // (!defined __EXT_MFC_NO_PAGE_NAVIGATOR )

#if (!defined __EXT_MFC_NO_DATE_PICKER)
	virtual void PaintDatePickerPushButton(
		CDC & dc,
		const CRect & rcButton,
		LONG nButtonType, // __EDPWH_BTN_NONE or __EDPWH_BTN_TODAY
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		bool bFlat,
		bool bDrawBorder,
		bool bPushed,
		bool bHover,
		HFONT hFont,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintDatePickerButtonsSeparator(
		CDC & dc,
		const RECT & rcSeparator,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintDatePickerClientArea(
		CDC & dc,
		const RECT & rcTotalClient,
		const RECT & rcUsedClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintDatePickerBorder(
		CDC & dc,
		const RECT & rcControl,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintDatePickerHeaderBackground(
		INT nCol,
		INT nRow,
		CDC & dc,
		INT nYear,
		INT nMonth,
		const RECT & rcHeader,
		const RECT & rcHeaderWithoutBorders,
		const RECT & rcHeaderDate,
		const RECT & rcScrollBtnBackward,
		const RECT & rcScrollBtnForward,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
#endif // (!defined __EXT_MFC_NO_DATE_PICKER)

#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP
	virtual void PaintUndoRedoCaption(
		CDC & dc,
		const CRect & rcCaption,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // __EXT_MFC_NO_UNDO_REDO_POPUP

#if( !defined __EXT_MFC_NO_SPIN || !defined __EXT_MFC_NO_DURATIONWND )
	virtual CRect Spin_QueryNcAreaMargins(
		bool bSpinAlignRight,
		bool bSpinAlignLeft,
		bool bHasScrolBars
		) const;	
	virtual void PaintSpinButtonArrow(
		CDC & dc,
		const CRect & rcButton,
		bool bUp,		
		bool bHorz,
		bool bEnabled,
		bool bPushed,
		bool bHover,
		bool bHotTrack,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
	virtual void PaintSpinButton(
		CDC & dc,
		const CRect & rcButton,
		bool bUp,		
		bool bHorz,
		bool bEnabled,
		bool bFlat,
		bool bDrawBorder,
		bool bPushed,
		bool bHover,
		bool bHotTrack,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // #if( !defined __EXT_MFC_NO_SPIN || !defined __EXT_MFC_NO_DURATIONWND )

#ifndef __EXT_MFC_NO_SLIDER
	virtual bool PaintSliderChannel(
		CDC & dc,
		const CRect & rcChannel,
		bool bHorz,
		bool bEnabled,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
	virtual bool PaintSliderThumb(
		CDC & dc,
		const CRect & rcThumb,
		bool bHorz,
		bool bLeft,
		bool bRight,
		bool bEnabled,
		bool bPushed,
		bool bFocused,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
	virtual bool PaintSliderTics(
		CDC & dc,
		const CRect & rcClient,
		bool bHorz,
		bool bEnabled,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // __EXT_MFC_NO_SLIDER
	
#ifndef __EXT_MFC_NO_DURATIONWND
	virtual INT Duration_GetSpinButtonWidth(
		const CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual INT Duration_GetDropDownButtonWidth(
		const CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual CRect Duration_GetSpinButtonExtraSpace(
		const CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual CRect Duration_GetDropDownButtonExtraSpace(
		const CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual void Duration_PaintSpinButton(
		CDC & dc,
		const CRect & rcButton,
		bool bUp,		
		bool bEnabled,
		bool bPushed,
		bool bHovered,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
	virtual void Duration_PaintDropDownButton(
		CDC & dc,
		const CRect & rcButton,
		bool bEnabled,
		bool bPushed,
		bool bHovered,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // __EXT_MFC_NO_DURATIONWND

#if (defined __EXT_PM_BACK_COMPATIBILITY_BITMAP_API)
	static HBITMAP stat_ColorizeMonochromeBitmap(
		HBITMAP hBitmapSrc,
		COLORREF clrTransparent,
		COLORREF clrTarget
		);
	static HBITMAP stat_HLS_Adjust_Bitmap(
		HBITMAP hBitmapSrc,
		COLORREF clrTransparent = RGB(0,0,0),
		COLORREF clrTolerance = RGB(0,0,0),
		double percentH = 0.0,
		double percentL = 0.0,
		double percentS = 0.0
		);
	static bool stat_HLS_Adjust_Bitmap(
		CBitmap & _bmp,
		COLORREF clrTransparent = RGB(0,0,0),
		COLORREF clrTolerance = RGB(0,0,0),
		double percentH = 0.0,
		double percentL = 0.0,
		double percentS = 0.0
		);
	static HRGN stat_HBITMAPtoHRGN(
		HBITMAP hBitmap,
		COLORREF clrTransparent,
		COLORREF clrTolerance = RGB(0,0,0)
		);
#endif // __EXT_PM_BACK_COMPATIBILITY_BITMAP_API

	enum eStyle2005_t
	{
		__ES2005_BETA1	= 1,
		__ES2005_BETA2	= 2,
		__ES2005_RC		= 3,
	};

	eStyle2005_t m_eStyle2005;

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	enum eDockMarkerType_t
	{
		__EDMT_2005_TOP				= 0,
		__EDMT_2005_BOTTOM			= 1,
		__EDMT_2005_LEFT			= 2,
		__EDMT_2005_RIGHT			= 3,
		__EDMT_2005_CENTER			= 4,
		__EDMT_2005_GROUP_SPARSED	= 5,
		__EDMT_2005_GROUP_CENTRAL	= 6,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		__EDMT_2005_GROUP_TAB		= 7,
		__EDMT_2005_TAB				= 8,
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		__EDMT_MIN_VALUE			= 0,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		__EDMT_MAX_VALUE			= 8,
#else // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		__EDMT_MAX_VALUE			= 6,
#endif // else from (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		__EDMT_EMPTY				= -1
	};

	class __PROF_UIS_API DockMarkerBase
	{
	public:
		eDockMarkerType_t m_eDockMarkerType;
		CExtBitmapCache m_bmpIn, m_bmpOut, m_bmpOuterBk;
		CPoint m_ptOffsetInner;
		CRgn m_rgn;
		CSize m_size;
		CTypedPtrArray < CPtrArray, DockMarkerBase * > m_arrChildren;
		CRect m_rcScreen;
		bool m_bSparsedMode:1;
	protected:
		CExtPaintManager * m_pPM;
		int m_eResizablePanelDockingType;
		virtual void _EmptyChildren();
		virtual void _DetachHandles();
	public:
		DockMarkerBase(
			CExtPaintManager * pPM,
			int eResizablePanelDockingType,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual ~DockMarkerBase();
		virtual bool IsEmpty() const;
		virtual void Empty();
		bool CreateFromBitmapResources(
			CRect rcScreen,
			eDockMarkerType_t eDockMarkerType,
			COLORREF clrTransparentIn,
			UINT nResourceBmpIn,
			UINT nResourceBmpOut,
			__EXT_MFC_SAFE_LPCTSTR strResourceTypeBmpIn = RT_BITMAP, // default bitmap section
			__EXT_MFC_SAFE_LPCTSTR strResourceTypeBmpOut = RT_BITMAP, // default bitmap section
			HINSTANCE hInstBmpIn = NULL, // find it automatically
			HINSTANCE hInstBmpOut = NULL // find it automatically
			);
		virtual bool CreateFromBitmapResources(
			CRect rcScreen,
			eDockMarkerType_t eDockMarkerType,
			COLORREF clrTransparentIn,
			__EXT_MFC_SAFE_LPCTSTR strResourceBmpIn,
			__EXT_MFC_SAFE_LPCTSTR strResourceBmpOut = NULL,
			__EXT_MFC_SAFE_LPCTSTR strResourceTypeBmpIn = RT_BITMAP, // default bitmap section
			__EXT_MFC_SAFE_LPCTSTR strResourceTypeBmpOut = RT_BITMAP, // default bitmap section
			HINSTANCE hInstBmpIn = NULL, // find it automatically
			HINSTANCE hInstBmpOut = NULL // find it automatically
			);
		bool CreateFromBitmapData(
			CRect rcScreen,
			eDockMarkerType_t eDockMarkerType,
			COLORREF clrTransparentIn,
			const BYTE * pDataBmpIn,
			DWORD dwSizeDataBmpIn,
			const BYTE * pDataBmpOut = NULL,
			DWORD dwSizeDataBmpOut = 0
			);
		virtual bool CreateFromBitmaps(
			CRect rcScreen,
			eDockMarkerType_t eDockMarkerType,
			COLORREF clrTransparentIn,
			const CExtBitmap & _bmpIn,
			const CExtBitmap & _bmpOut
			);
		virtual bool Create(
			bool bCentralMode,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual bool DockMarker_CreateWnds(
			HWND hWndLastDynamicTarget,
			CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * > & arrDockMarkerWnds,
			CExtDynDockMarkerWnd * pWndParentMarker
			);
		virtual void OnCalcScreenRect(
			eDockMarkerType_t eDockMarkerType,
			CRect rcScreen,
			DockMarkerBase * pCenterMarker = NULL
			);
	}; // class DockMarkerBase

	class __PROF_UIS_API DockMarkerBeta2 : public DockMarkerBase
	{
	public:
		DockMarkerBeta2(
			CExtPaintManager * pPM,
			int eResizablePanelDockingType,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual ~DockMarkerBeta2();
		virtual bool Create(
			bool bCentralMode,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual void OnCalcScreenRect(
			eDockMarkerType_t eDockMarkerType,
			CRect rcScreen,
			DockMarkerBase * pCenterMarker = NULL
			);
	}; // class DockMarkerBeta2

	class __PROF_UIS_API DockMarker2008XP : public DockMarkerBase
	{
	public:
		DockMarker2008XP(
			CExtPaintManager * pPM,
			int eResizablePanelDockingType,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual ~DockMarker2008XP();
		virtual bool Create(
			bool bCentralMode,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual void OnCalcScreenRect(
			eDockMarkerType_t eDockMarkerType,
			CRect rcScreen,
			DockMarkerBase * pCenterMarker = NULL
			);
	}; // class DockMarker2008XP

	class __PROF_UIS_API DockMarker2008Vista : public DockMarkerBase
	{
	public:
		DockMarker2008Vista(
			CExtPaintManager * pPM,
			int eResizablePanelDockingType,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual ~DockMarker2008Vista();
		virtual bool Create(
			bool bCentralMode,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual void OnCalcScreenRect(
			eDockMarkerType_t eDockMarkerType,
			CRect rcScreen,
			DockMarkerBase * pCenterMarker = NULL
			);
	}; // class DockMarker2008Vista

	class __PROF_UIS_API DockMarker2010 : public DockMarkerBase
	{
	public:
		DockMarker2010(
			CExtPaintManager * pPM,
			int eResizablePanelDockingType,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual ~DockMarker2010();
		virtual bool Create(
			bool bCentralMode,
			eDockMarkerType_t eDockMarkerType = __EDMT_EMPTY,
			CRect rcScreen = CRect( 0, 0, 0, 0 )
			);
		virtual void OnCalcScreenRect(
			eDockMarkerType_t eDockMarkerType,
			CRect rcScreen,
			DockMarkerBase * pCenterMarker = NULL
			);
	}; // class DockMarker2010

	virtual DockMarkerBase * DockMarker_CreateObject(
		int eResizablePanelDockingType
		);
	virtual bool DockMarker_CreateWnds(
		HWND hWndLastDynamicTarget,
		eDockMarkerType_t eDockMarkerType,
		CRect rcScreen,
		CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * > & arrDockMarkerWnds,
		int eResizablePanelDockingType,
		CExtDynDockMarkerWnd * pWndParentMarker
		);
	virtual void DockMarker_PostRender(
		CDC & dc,
		CRect rcItem,
		eDockMarkerType_t eMarkerType,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual CRect DockMarker_GetOuterDistance();
	virtual BYTE DockMarker_GetAlpha(
		bool bHighlight,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool DockMarker_IsAdjustBmpsRequired(
		eDockMarkerType_t eDockMarkerType
		) const;
	virtual void DockMarker_AdjustBmps(
		eDockMarkerType_t eDockMarkerType,
		CExtBitmap & bmpIn,
		COLORREF clrTransparentIn,
		CExtBitmap & bmpOut,
		COLORREF clrTransparentOut
		);
	virtual void DockMarker_AdjustHighlightedArea(
		COLORREF * pClrSurface,
		int nClrSurfaceDX,
		int nClrSurfaceDY,
		bool bTabShape,
		bool bTabsAtTop,
		const CRect & rcTabMainArea,
		const CRect & rcTabBottomMiddleArea,
		COLORREF clrAdjustMain = COLORREF(-1L),
		COLORREF clrAdjustBorder = COLORREF(-1L),
		int nBorderMetric = 2
		);
	virtual void DockMarker_AdjustHighlightedLayer(
		HWND hWnd,
		int nDX,
		int nDY,
		bool bTabShape,
		bool bTabsAtTop,
		const CRect & rcTabMainArea,
		const CRect & rcTabBottomMiddleArea,
		COLORREF clrAdjustMain = COLORREF(-1L),
		COLORREF clrAdjustBorder = COLORREF(-1L),
		int nBorderMetric = 2
		);
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

	virtual void PaintBarButtonCombinedArea(
		CDC & dc,
		CExtBarButton * pTBB,
		LPARAM lParam = 0L
		);

	virtual COLORREF QueryObjectTextColor(
		CDC & dc,
		bool bEnabled,
		bool bFocused,
		bool bHovered,
		bool bPressed,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	virtual bool PaintGroupBoxFrame(
		CDC & dc,
		const RECT & rcBorder,
		COLORREF clrFrame = COLORREF(-1L),
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual bool PaintGroupBoxLabel(
		CDC & dc,
		__EXT_MFC_SAFE_LPCTSTR strText,
		COLORREF clrText,
		HFONT hFont,
		DWORD dwDrawTextFlags,
		bool bEnabled,
		const RECT & rcText,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	static COLORREF stat_PaintShadedEllipse_ComputeColorValue(
		double lfA,
		COLORREF clrShadeAccentLight,
		COLORREF clrShadeAccentDark
		);
	static void stat_PaintShadedEllipse(
		HDC hDC,
		const POINT & ptEllipseCenter,
		LONG nRadiusH,
		LONG nRadiusV,
		COLORREF clrShadeAccentLight,
		COLORREF clrShadeAccentDark
		);

	virtual bool ScrollBar_GetMetrics(
		INT * p_nHorzBarHeight = NULL,
		INT * p_nVertBarWidth = NULL,
		INT * p_nHorzButtonWidth = NULL,
		INT * p_nVertButtonHeight = NULL,
		INT * p_nHorzThumbMinWidth = NULL,
		INT * p_nVertThumbMinHeight = NULL,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		) const;
	enum e_scroll_bar_mouse_hover_type_t
	{
		__ESBMHT_NOWHERE                  = -1,
		__ESBMHT_SOLID_SCROLLABLE_AREA    = 0, // basically used for disabled state
		__ESBMHT_BUTTON_UP                = 1,
		__ESBMHT_BUTTON_DOWN              = 2,
		__ESBMHT_THUMB                    = 3,
		__ESBMHT_PAGE_UP                  = 4,
		__ESBMHT_PAGE_DOWN                = 5,
		__ESBMHT_GLYPH_ARROW_UP           = 6, // for skinning purposes only
		__ESBMHT_GLYPH_ARROW_DOWN         = 7, // for skinning purposes only
		__ESBMHT_GLYPH_THUMB_GRIPPER      = 8, // for skinning purposes only
		__ESBMHT_PART_COUNT               = 9, // for using as array dimension
	}; // enum e_scroll_bar_mouse_hover_type_t
	enum e_scroll_bar_skin_part_t
	{
		__ESBSPT_DISABLED                 = 0,
		__ESBSPT_COLD                     = 1,
		__ESBSPT_NORMAL                   = 2,
		__ESBSPT_HOT                      = 3,
		__ESBSPT_PRESSED                  = 4,
		__ESBSPT_PART_COUNT               = 5, // for using as array dimension
	}; // enum e_scroll_bar_skin_part_t
	struct __PROF_UIS_API SCROLLBARSKINDATA
	{
		CExtBitmapCache m_arrPartBmp[__ESBSPT_PART_COUNT][__ESBMHT_PART_COUNT];
		CRect m_arrPartPadding[__ESBSPT_PART_COUNT][__ESBMHT_PART_COUNT];
		COLORREF m_arrClrSolidBackground[__ESBSPT_PART_COUNT][__ESBMHT_PART_COUNT];
		COLORREF m_clrZsLineLightEnabled, m_clrZsLineDarkEnabled, m_clrZsLineLightDisabled, m_clrZsLineDarkDisabled;
		CExtBitmapCache m_bmpEntireArea;
		CRect m_rcEntirePaddingR, m_rcEntirePaddingC;
		SCROLLBARSKINDATA();
		virtual ~SCROLLBARSKINDATA();
		virtual void Empty();
		virtual CExtBitmap & GetPart(
			e_scroll_bar_skin_part_t ePartState,
			e_scroll_bar_mouse_hover_type_t eSBMHT, // part type
			RECT * pRectPadding = NULL,
			COLORREF * pClrSolidBackground = NULL
			);
		const CExtBitmap & GetPart(
			e_scroll_bar_skin_part_t ePartState,
			e_scroll_bar_mouse_hover_type_t eSBMHT, // part type
			RECT * pRectPadding = NULL,
			COLORREF * pClrSolidBackground = NULL
			) const;
		virtual void Load2007_Zoom(
			UINT nResourceID_ButtonPlus,
			UINT nResourceID_ButtonMinus,
			UINT nResourceID_ButtonThumb,
			COLORREF clrZsLineLightEnabled,
			COLORREF clrZsLineDarkEnabled,
			COLORREF clrZsLineLightDisabled,
			COLORREF clrZsLineDarkDisabled,
			UINT nDockID,
			bool bResourceColorChannelsPreMultiplied = true
			);
		virtual void Load2010_Zoom_R1(
			UINT nResourceID_ButtonPlus,
			UINT nResourceID_ButtonMinus,
			UINT nResourceID_ButtonThumb,
			COLORREF clrZsLineLightEnabled,
			COLORREF clrZsLineDarkEnabled,
			COLORREF clrZsLineLightDisabled,
			COLORREF clrZsLineDarkDisabled,
			UINT nDockID,
			bool bResourceColorChannelsPreMultiplied = true
			);
		virtual void Load2010_Zoom_R2(
			UINT nResourceID_ButtonPlus,
			UINT nResourceID_ButtonMinus,
			UINT nResourceID_ButtonThumb,
			COLORREF clrZsLineLightEnabled,
			COLORREF clrZsLineDarkEnabled,
			COLORREF clrZsLineLightDisabled,
			COLORREF clrZsLineDarkDisabled,
			UINT nDockID,
			bool bResourceColorChannelsPreMultiplied = true
			);
		virtual void Load2007_R1(
			UINT nResourceID_BumpyGlyphStack,
			INT nBumpyGlyphStackItemHeight,
			UINT nResourceID_Gripper,
			const RECT & rcBumpyGlyphStackItemPadding,
			COLORREF clrSolidBackground,
			COLORREF clrPressedPageAreas,
			UINT nResourceID_ArrowUp_Normal,
			UINT nResourceID_ArrowUp_Disabled,
			UINT nResourceID_ArrowDown_Normal,
			UINT nResourceID_ArrowDown_Disabled,
			bool bRotateArrowsAndBk270,
			UINT nResourceID_BkEntire,
			UINT nResourceID_BkPage,
			const RECT & rcEntirePaddingR,
			const RECT & rcEntirePaddingC
			);
		virtual void Load2007_R2(
			UINT nResourceID_BumpyGlyphStack,
			INT nBumpyGlyphStackItemHeight,
			UINT nResourceID_GripperStack,
			INT nGripperStackItemHeight,
			const RECT & rcBumpyGlyphStackItemPadding,
			COLORREF clrSolidBackground,
			COLORREF clrPressedPageAreas,
			UINT nResourceID_ArrowUp_Normal,
			UINT nResourceID_ArrowUp_Disabled,
			UINT nResourceID_ArrowDown_Normal,
			UINT nResourceID_ArrowDown_Disabled,
			bool bRotateArrowsAndBk270,
			UINT nResourceID_BkEntire,
			UINT nResourceID_BkPage,
			const RECT & rcEntirePaddingR,
			const RECT & rcEntirePaddingC
			);
	}; // struct SCROLLBARSKINDATA
	struct __PROF_UIS_API PAINTSCROLLBARDATA
	{
		bool m_bHorzBar:1, m_bEnabled:1, m_bHoveredAreaIsPressed:1, m_bHelperLightAccent:1;
		e_scroll_bar_mouse_hover_type_t m_eSBMHT;
		CRect m_rcBar, m_rcPageUp, m_rcPageDown,
			m_rcButtonUp, m_rcButtonDown, m_rcThumb,
			m_rcScrollableArea;
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		SCROLLINFO m_DSI;
		PAINTSCROLLBARDATA();
		PAINTSCROLLBARDATA(
			bool bHorzBar,
			bool bEnabled,
			bool bHoveredAreaIsPressed,
			e_scroll_bar_mouse_hover_type_t eSBMHT,
			const RECT & rcBar,
			const RECT & rcButtonUp,
			const RECT & rcButtonDown,
			const RECT & rcThumb,
			const RECT & rcPageUp,
			const RECT & rcPageDown,
			CObject * pHelperSrc = NULL,
			LPARAM lParam = 0L
			);
		PAINTSCROLLBARDATA(
			CExtScrollBar * pExtScrollBar,
			bool bHoveredAreaIsPressed = false,
			e_scroll_bar_mouse_hover_type_t eSBMHT = CExtPaintManager::__ESBMHT_NOWHERE,
			LPARAM lParam = 0L
			);
		virtual ~PAINTSCROLLBARDATA();
		virtual void PaintDefault(
			CDC & dc,
			CExtPaintManager * pPM
			);
		virtual void PaintSkinned(
			CDC & dc,
			CExtPaintManager * pPM,
			const SCROLLBARSKINDATA & _sbsd,
			BYTE nConstantAlpha = 0x0FF
			);
		virtual e_scroll_bar_mouse_hover_type_t GetHT(
			const POINT & pt
			) const;
		virtual e_scroll_bar_mouse_hover_type_t AdjustHT(
			const POINT & pt
			);
		virtual CRect GetAreaRect(
			e_scroll_bar_mouse_hover_type_t eSBMHT
			) const;
		virtual CRect GetAreaRectHT() const;
	}; // struct PAINTSCROLLBARDATA
	bool m_bScrollBarsHaveHoverEffectIn2kStyle:1;
	virtual void ScrollBar_Paint(
		CDC & dc,
		PAINTSCROLLBARDATA & _psbd
		) const;
	virtual SCROLLBARSKINDATA * ScrollBar_GetSkinData(
		bool bHorzBar,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		bool bLightAccent = true
		);

#ifndef __EXT_MFC_NO_GRIDWND
	virtual bool Grid_PaintButton(
		CDC & dc,
		const RECT & rcButton,
		INT nButtonType,
		bool bPressed,
		bool bHovered,
		bool bFocused,
		bool bEnabled,
		COLORREF clrFace,
		COLORREF clrLight,
		COLORREF clrShadow,
		COLORREF clrWindow,
		COLORREF clrText,
		DWORD dwHelperPaintFlags,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool Grid_PaintHeaderSortArrow(
		CDC & dc,
		const RECT & rcSortArrow,
		bool bHorz,
		bool bAscending,
		COLORREF * pColorValues,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool Grid_PaintHeaderBackground(
		CDC & dc,
		const RECT & rcArea,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		DWORD dwAreaFlags,
		DWORD dwHelperPaintFlags,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual COLORREF Grid_OnQueryTextColor(
		const CExtGridCell & _cell,
		CDC & dc,
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		DWORD dwAreaFlags,
		DWORD dwHelperPaintFlags
		);
#endif // #ifndef __EXT_MFC_NO_GRIDWND

#if (!defined __EXT_MFC_NO_REPORTGRIDWND)
	virtual bool ReportGrid_PaintHeaderRowBackground(
		CDC & dc,
		const RECT & rcArea,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool ReportGrid_PaintHeaderSortArrow(
		CDC & dc,
		const RECT & rcSortArrow,
		bool bAscending,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool ReportGrid_PaintGroupAreaBackground(
		CDC & dc,
		const RECT & rcArea,
		__EXT_MFC_SAFE_LPCTSTR strEmptyAreaCaption, // not NULL if area is empty
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual COLORREF ReportGrid_GetHeaderTextColor(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual COLORREF ReportGrid_GetGroupConnectorColor(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool ReportGrid_PaintGroupAreaItemBackground(
		CDC & dc,
		const RECT & rcItem,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
#endif // (!defined __EXT_MFC_NO_REPORTGRIDWND)

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	virtual bool Ribbon_OptionsPageBackgroundIsDefault() const;
	virtual void Ribbon_DwmAreaAdjust(
		CExtRibbonBar * pRibbonBar,
		CExtUxTheme::__EXT_UX_MARGINS & _margins,
		LPARAM lParam = 0L
		);
	virtual bool Ribbon_DwmAreaCoversTabs() const;
	virtual INT Ribbon_GetContractedMarginHeight(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_PaintContractedMargin(
		CDC & dc,
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GetGroupHeight(
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GetToolGroupExtraOffset(
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GetToolGroupExtraLineDistance(
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GetOuterGroupDistance(
		const CExtBarButton * pTBB,
		bool bDistanceBefore,
		LPARAM lParam = 0L
		);
	virtual CRect Ribbon_GetContentPadding(
		const CExtBarButton * pTBB,
		LPARAM lParam = 0L
		);
	virtual CSize Ribbon_GetIconSize(
		const CObject * pHelperSrc,
		INT nILV,
		LPARAM lParam = 0L
		);
	virtual CSize Ribbon_GetDropDownArrowSize(
		CDC & dc,
		INT nILV,
		const CExtBarButton * pTBB,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GetTextToIconDistance(
		CDC & dc,
		INT nILV,
		const CExtBarButton * pTBB,
		LPARAM lParam = 0L
		);
	virtual CSize Ribbon_CalcButtonSize(
		CDC & dc,
		INT nILV,
		const CExtBarButton * pTBB,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GroupCaptionGetHeight(
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		) const;
	virtual bool Ribbon_GroupCaptionIsAtTop(
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		) const;
	virtual CExtCmdIcon * Ribbon_GetGroupCollapsedIcon(
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_PaintText(
		CDC & dc,
		__EXT_MFC_SAFE_LPCTSTR strText,
		const RECT & rcText,
		UINT nDrawTextFlags,
		bool bDrawDropDownArrow,
		CExtBitmap * pBmpAlternativeDropDownArrow,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_PaintGroupBk(
		CDC & dc,
		const RECT & rcGroupBk,
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		);
	virtual bool Ribbon_PaintQuickAccessGroupButton(
		CDC & dc,
		const RECT & rcGroupBk,
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		);
	virtual CRect Ribbon_GetPageOuterContentPadding(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_PaintPageBk(
		CDC & dc,
		const RECT & rcPageBk,
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_PaintSeparator(
		CDC & dc,
		const RECT & rcTBB,
		const CExtBarButton * pSepTBB, // can be NULL
		LPARAM lParam = 0L
		);
	virtual bool Ribbon_HaveSeparatorsBetweenToolGroups() const;
	virtual CRect Ribbon_CalcLargeDropDownRect(
		const CExtBarButton * pTBB,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_PaintPushButton(
		CDC & dc,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);
	virtual CRect Ribbon_GetPopupGroupBorderMetrics(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		);
	virtual bool Ribbon_IsPopupGroupWithCaption(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual INT RibbonLayout_CalcQatbAboveRibbonHorzStart(
		CExtRibbonBar * pRibbonBar,
		LPARAM lParam = 0L
		);
	virtual INT RibbonQuickAccessBar_GetBottomHeight(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		) const;
	virtual INT RibbonQuickAccessBar_GetButtonsAlignment(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		) const;
	virtual void RibbonQuickAccessBar_PaintBkAtTheBottom(
		CDC & dc,
		CRect rcQATB,
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		) const;
	virtual void RibbonQuickAccessBar_AdjustButtonLocation(
		CExtBarButton * pTBB,
		CRect & rcTBB,
		LPARAM lParam = 0L
		);
	virtual INT RibbonQuickAccessBar_GetAdditionalSpaceAtRight(
		bool bDwmMode,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GetTabIntersectionHeight(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		) const;
	virtual void Ribbon_PaintTabItem(
		CDC & dc,
		CRect & rcTabItemsArea,
		bool bSelected,
		const CRect & rcEntireItem,
		CFont * pFont,
		__EXT_MFC_SAFE_LPCTSTR sText,
		CExtCmdIcon * pIcon,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		COLORREF clrForceText = COLORREF(-1L),
		COLORREF clrForceTabBk = COLORREF(-1L),
		COLORREF clrForceTabBorderLT = COLORREF(-1L),
		COLORREF clrForceTabBorderRB = COLORREF(-1L),
		COLORREF clrForceTabSeparator = COLORREF(-1L)
		);
	virtual void Ribbon_PaintTabClientArea(
		CDC & dc,
		CRect & rcClient,
		CRect & rcTabItemsArea,
		CRect & rcTabNearBorderArea,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GetTabLineHeight(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		) const;

	virtual HRGN Ribbon_GetRgnCornerExclude(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual HRGN Ribbon_GetRgnCornerResizing(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_NcOverPaint(
		CDC & dc,
		bool bFrameActive,
		INT nWidthFromLeft,
		bool bDwmMode,
		LPCRECT pRectDwmText,
		__EXT_MFC_SAFE_LPCTSTR strDwmCaptionTitle,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_EmbeddedCaptionPaintText(
		CDC & dc,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		CRect rcDrawText,
		__EXT_MFC_SAFE_LPCTSTR strCaptionDelimiter,
		const CExtRibbonBar * pRibbonBar,
		LPARAM lParam = 0L
		);
	virtual UINT Ribbon_EmbeddedCaptionGetTextAlignmentFlags(
		const CExtRibbonBar * pRibbonBar,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_EmbeddedCaptionAdjustTextRect(
		CRect & rcAdjust,
		const CExtRibbonBar * pRibbonBar,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_FileMenuButtonQuery(
		LPVOID pData,
		CExtCmdIcon * pIcon = NULL,
		__EXT_MFC_SAFE_LPCTSTR sText = NULL,
		LPARAM lParam = 0L
		);
	virtual bool Ribbon_FileButtonIsItegrationSupported() const;
	virtual bool Ribbon_IsSystemMenuIconPresent() const;
	virtual INT Ribbon_FileButtonGetMenuIntersectionHeight() const;
	virtual bool Ribbon_FileButtonCalcLayout(
		CDC & dc,
		CExtNcFrameImpl * pExtNcFrameImpl,
		CExtRibbonButtonFile * pFileTBB,
		LPARAM lParam = 0L
		);
	virtual bool Ribbon_FileButtonPaint(
		CDC & dc,
		CExtRibbonButtonFile * pFileTBB,
		LPARAM lParam = 0L
		);
	virtual bool Ribbon_QuickAccessContentExpandButtonCalcLayout(
		CDC & dc,
		CExtRibbonButtonQuickAccessContentExpand * pQACEB,
		LPARAM lParam = 0L
		);
	virtual bool Ribbon_QuickAccessContentExpandButtonPaint(
		CDC & dc,
		CExtRibbonButtonQuickAccessContentExpand * pQACEB,
		LPARAM lParam = 0L
		);
	virtual CSize Ribbon_QuickAccessContentExpandGetShift(
		CDC & dc,
		CExtRibbonButtonQuickAccessContentExpand * pQACEB,
		LPARAM lParam = 0L
		);

	virtual bool Ribbon_DLB_IsVisible(
		CExtRibbonButtonDialogLauncher * pTBB,
		LPARAM lParam = 0L
		) const;
	virtual CSize Ribbon_DLB_CalcSize(
		CDC & dc,
		CExtRibbonButtonDialogLauncher * pTBB,
		LPARAM lParam = 0L
		);
	virtual void Ribbon_DLB_Paint(
		CDC & dc,
		CExtRibbonButtonDialogLauncher * pTBB,
		LPARAM lParam = 0L
		);
	virtual CFont * RibbonGallery_GetItemFont(
		CExtRibbonGalleryWnd * pGalleryWnd,
		LPVOID pGalleryItemData,
		LPARAM lParam = 0L
		);
	virtual CSize RibbonGallery_MeasureItem(
		CDC & dc,
		CExtRibbonGalleryWnd * pGalleryWnd,
		LPVOID pGalleryItemData,
		LPARAM lParam = 0L
		);
	virtual bool RibbonGallery_DrawItem(
		CDC & dc,
		CExtRibbonGalleryWnd * pGalleryWnd,
		const RECT & rcItem,
		LPVOID pGalleryItemData,
		LPARAM lParam = 0L
		);
	virtual bool RibbonGallery_EraseEntireBk(
		CDC & dc,
		const CRect & rcClient,
		const CRect & rcActiveChildArea,
		CRgn & rgnActiveChildArea,
		const CRect & rcActiveChildArea2,
		CRgn & rgnActiveChildArea2,
		CExtRibbonGalleryWnd * pGalleryWnd,
		LPARAM lParam = 0L
		);
	virtual void RibbonGallery_AdjustClientRect(
		CRect & rcClient,
		const CExtRibbonGalleryWnd * pGalleryWnd,
		LPARAM lParam = 0L
		);
	virtual void RibbonGallery_PaintScrollArea(
		CDC & dc,
		CExtRibbonGalleryInplaceScrollBar * pSB,
		LPARAM lParam = 0L
		);
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	virtual void AdvTip_CalcRgn(
		HRGN hRgnPrecalc,
		const RECT & rcRectRgn,
		bool bDynamicShadowApplied,
		INT nSizeInnerShadowIncluded,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool AdvTip_PaintBackground(
		CDC & dc,
		const RECT & rcClient,
		bool bAdvTipWithShadow,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual CFont * AdvTip_GetFont(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual COLORREF AdvTip_GetTextColor(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	struct __PROF_UIS_API PAINTPROGRESSDATA
	{
		int m_nPos, m_nRange;
		CRect m_rcClient;
		bool m_bHorz:1, m_bSmooth:1, m_bBorder:1;
		CObject * m_pHelperSrc;
		LPARAM m_lParam;
		bool m_bEndlessMode:1;
		LONG m_nEndlessNominator, m_nEndlessDenominator;
		PAINTPROGRESSDATA(
			int nPos,
			int nRange,
			const RECT & rcClient,
			bool bHorz = true,
			bool bSmooth = true,
			bool bBorder = true,
			CObject * pHelperSrc = NULL,
			LPARAM lParam = 0L
			)
			: m_nPos( nPos )
			, m_nRange( nRange )
			, m_rcClient( rcClient )
			, m_bHorz( bHorz )
			, m_bSmooth( bSmooth )
			, m_bBorder( bBorder )
			, m_pHelperSrc( pHelperSrc )
			, m_lParam( lParam )
			, m_bEndlessMode( false )
			, m_nEndlessNominator( 1 )
			, m_nEndlessDenominator( 3 )
		{
		}
	}; // struct PAINTPROGRESSDATA
	virtual bool PaintProgress(
		CDC & dc,
		PAINTPROGRESSDATA & _ppd
		);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual CFont & ScreenTip_GetFont(
		bool bCaption,
		bool bMainPart,
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual CRect ScreenTip_GetPaddingOuter(
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual CRect ScreenTip_GetPaddingInnerBmp(
		bool bMainPart,
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual CRect ScreenTip_GetPaddingInnerText(
		bool bCaption,
		bool bMainPart,
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual INT ScreenTip_GetSeparatorHeight(
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual COLORREF ScreenTip_GetTextColor(
		bool bCaption,
		bool bMainPart,
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual CSize ScreenTip_Measure(
		CDC & dc,
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual void ScreenTip_PaintBackground(
		CDC & dc,
		const RECT & rcScreenTip,
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
	virtual void ScreenTip_Paint(
		CDC & dc,
		const RECT & rcScreenTip,
		CExtCustomizeCmdScreenTip & _CCST,
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
	bool m_bPageContainerUseButtonStyle:1;
	bool m_bPageContainerUseGroupBoxStyle:1;
	bool m_bPageContainerNoEmbossDisabledText:1;
	virtual void PageContainer_EraseEntire(
		CDC & dc,
		CRect rcClient,
		CExtPageContainerWnd * pWndPageContainer,
		LPARAM lParam = 0L
		);
	virtual void PageContainer_EraseScrollPosArea(
		CDC & dc,
		CRect rcScrollPosArea,
		LONG nScrollPos,
		LONG nScrollSize,
		CExtPageContainerWnd * pWndPageContainer,
		LPARAM lParam = 0L
		);
	virtual void PageContainer_DrawScrollPosArea(
		CDC & dc,
		CRect rcScrollPosArea,
		LONG nScrollPos,
		LONG nScrollSize,
		CExtPageContainerWnd * pWndPageContainer,
		LPARAM lParam = 0L
		);
	virtual void PageContainer_EraseItem(
		CDC & dc,
		LPVOID pPageInfo, // CExtPageContainerWnd::PAGE_ITEM_INFO
		CRect rcPage,
		CRect rcCaption,
		CRect rcWnd,
		CExtPageContainerWnd * pWndPageContainer,
		LPARAM lParam = 0L
		);
	virtual void PageContainer_DrawItem(
		CDC & dc,
		LPVOID pPageInfo, // CExtPageContainerWnd::PAGE_ITEM_INFO
		CRect rcPage,
		CRect rcCaption,
		CRect rcWnd,
		CExtPageContainerWnd * pWndPageContainer,
		LPARAM lParam = 0L
		);
	virtual void PageContainer_EraseCaption(
		CDC & dc,
		LPVOID pPageInfo, // CExtPageContainerWnd::PAGE_ITEM_INFO
		CRect rcCaption,
		CExtPageContainerWnd * pWndPageContainer,
		LPARAM lParam = 0L
		);
	virtual void PageContainer_DrawCaption(
		CDC & dc,
		LPVOID pPageInfo, // CExtPageContainerWnd::PAGE_ITEM_INFO
		CRect rcCaption,
		CExtPageContainerWnd * pWndPageContainer,
		LPARAM lParam = 0L
		);
#endif // (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)

	// header control
	virtual void Header_PaintBackground(
		CDC & dc,
		CRect rc,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual CRect Header_GetItemPadding(
		INT nColNo,
		INT nColCount,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual void Header_PaintItem(
		CDC & dc,
		CRect rcItemEntire,
		CRect rcItemData,
		CRect rcIcon,
		CRect rcText,
		CRect rcSortArrow,
		CRect rcButton,
		CRect rcButtonIcon,
		bool bSorted,
		bool bSortedAscending,
		INT nColNo,
		INT nColCount,
		const CExtCmdIcon & iconItem,
		const CExtCmdIcon & iconButton,
		bool bHover,
		bool bPressed,
		bool bButtonEvent,
		__EXT_MFC_SAFE_LPCTSTR strItemText,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void Header_GetButtonPadding(
		INT & nButtonPaddingBefore,
		INT & nButtonPaddingAfter,
		INT nColNo,
		INT nColCount,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual INT Header_GetDistanceBetweenIconAndSortArrow(
		INT nColNo,
		INT nColCount,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual INT Header_GetDistanceBetweenIconAndText(
		INT nColNo,
		INT nColCount,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual INT Header_GetDistanceBetweenTextAndButton(
		INT nColNo,
		INT nColCount,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual INT Header_GetSortArrowWidth(
		INT nColNo,
		INT nColCount,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
}; // class CExtPaintManager

class __PROF_UIS_API CExtPaintManagerXP : public CExtPaintManager
{
protected:
	bool m_bEnabledHoverIconShadows:1;
	bool m_bHelperXpStyle8BitBarRect:1;
	bool m_bExpBtnSwapVGlyphs:1;
	const glyph_t * m_pGlyphTbEpBtnH0;
	const glyph_t * m_pGlyphTbEpBtnH1;
	const glyph_t * m_pGlyphTbEpBtnV0;
	const glyph_t * m_pGlyphTbEpBtnV1;
	int m_nIdxClrMlaNormLeft, m_nIdxClrMlaNormRight, m_nIdxClrMlaNormMiddle,
		m_nIdxClrMlaRarelyLeft, m_nIdxClrMlaRarelyRight, m_nIdxClrMlaRarelyMiddle,
		m_nIdxClrBtnNormLeft, m_nIdxClrBtnNormRight,
		m_nIdxClrBtnHoverLeft, m_nIdxClrBtnHoverRight,
		m_nIdxClrBtnPressedLeft, m_nIdxClrBtnPressedRight,
		m_nIdxClrBtnHovPresLeft, m_nIdxClrBtnHovPresRight;

public:
	COLORREF m_clrCustomDropDownGlyphEnabled, m_clrCustomDropDownGlyphDisabled;

	DECLARE_SERIAL( CExtPaintManagerXP );

	// construction/destruction
	CExtPaintManagerXP();
	virtual ~CExtPaintManagerXP();

	virtual e_paint_manager_name_t OnQueryPaintManagerName()
	{
		ASSERT_VALID( this );
		return NativeXP;
	}

	virtual INT GetSeparatorHeight()
	{
		ASSERT_VALID( this );
		return 1;
	}

	virtual bool OnQueryUseThemeColors() const;

	virtual COLORREF GetShadowAdjustColor();
	virtual void FixedBar_AdjustClientRect(
		const CExtControlBar * pBar,
		CRect & rcPreCalc
		);
	virtual INT FixedBar_GetRowDistance(
		const CExtToolControlBar * pBar
		);
	virtual bool FixedBar_IsPaintRowBkMode(
		const CExtToolControlBar * pBar
		);

	// install required translated color as
	// described in e_translated_colors_t enumeration
	virtual void InitTranslatedColors();

	// gets the pushed metic offset
	virtual CSize GetPushedOffset();

	// paint methods

	virtual void PaintPushButton(
		CDC & dc,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);
	virtual void PaintPushButtonText(
		CDC & dc,
		CRect & rcCaption,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);

	virtual void PaintGripper(
		CDC & dc,
		CExtPaintManager::PAINTGRIPPERDATA & _pgd
		);

	virtual void PaintSeparator(
		CDC & dc,
		const RECT & rectItem,
		bool bHorz,
		bool bTransparentBk,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	virtual void PaintIcon(
		CDC & dc,
		bool bHorz,
		CExtCmdIcon * pIcon,
		const CRect & rcClient,
		bool bPushed,
		bool bEnabled,
		bool bHover = false,
		INT nIconAlignment = 0,
		bool bRTL = false,
		CRect * prcIcon = NULL,
		CRect * prcText = NULL,
		CRect * prcIconMargins = NULL,
		COLORREF clrCustomAccentEffectForIcon = COLORREF(-1L)
		);

	virtual void PaintMenuItem(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuItemIcon(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuItemInPlaceWnd(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuItemLeftAreaMarginXP(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid,
		const RECT & rcFillLeftPart,
		const RECT & rcFillRightPart
		);
	virtual bool PaintMenuItemBackgroundXP(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid,
		const RECT & rcSelectionArea
		);
	virtual bool PaintMenuItemSelectionAreaXP(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid,
		const RECT & rcSelectionArea
		);
	enum e_CheckMarkAreaTypeXP
	{
		__ECMATXP_EXTRA_MARK_CHECKED = 0,
		__ECMATXP_ICON_OR_CHECK_OR_RADIO_FRAME = 1,
	};
	virtual bool PaintMenuItemCheckMarkAreaXP(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid,
		const RECT & rcCheckMarkArea,
		e_CheckMarkAreaTypeXP eCMATXP
		);
	virtual void PaintMenuSeparator(
		CDC & dc,
		const RECT & rectItem,
		int nIconAreaWidth,
		bool bRarelyUsed,
		bool bIsForceNoLeftGradient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	virtual void PaintControlFrame(
		CDC & dc,
		CExtPaintManager::PAINTCONTROLFRAMEDATA & _pcfd
		);

	virtual void PaintComboFrame(
		CDC & dc,
		CExtPaintManager::PAINTCOMBOFRAMEDATA & _pcfd
		);

	virtual void PaintSysCmdGlyph(
		CDC & dc,
		UINT nHelperSysCmdID,
		const RECT & rcItem,
		bool bHover,
		bool bPushed,
		bool bEnabled,
		COLORREF clrForceGlyphColor = COLORREF(-1L)
		);

	virtual void PaintFloatToolbarRowBk(
		CDC & dc,
		const CExtToolControlBar * pBar,
		int nLastReviewBtnIdx,
		CRect & rcRowBk
		);

	virtual void PaintToolbarExpandButton(
		CDC & dc,
		const RECT & rcButtonArea,
		bool bHorz, // if false - down
		bool bBarIsCompletelyVisible,
		bool bEnabled,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		bool bTransparentBackground = false
		);
	virtual void PaintMenuExpandButton(
		CDC & dc,
		const RECT & rectButton,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintMenuScrollButton(
		CDC & dc,
		bool bHover,
		bool bPushed,
		bool bTopButton,
		const RECT & rcScrollButton,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintMenuTearOffButton(
		CDC & dc,
		const RECT & rectButton,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual int GetMenuExpandButtonHeight();

	virtual void PaintDockingFrame(
		CDC & dc,
		CExtPaintManager::PAINTDOCKINGFRAMEDATA & _pdfd
		);

	virtual int GetMenuShadowSize() const;
	virtual CRect GetMenuBorderMetrics(
		CWnd * pWnd,
		LPARAM lParam = 0
		) const;

	virtual void PaintMenuBorder(
		CDC & dc,
		const RECT & rectClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	enum e_xp_colors_t
	{
		XPCLR_3DFACE_DARK				= __ExtMfc_XP_COLOR_SHIFT+0,
		XPCLR_3DFACE_NORMAL				= __ExtMfc_XP_COLOR_SHIFT+1,
		XPCLR_SEPARATOR					= __ExtMfc_XP_COLOR_SHIFT+2,
		XPCLR_HILIGHT					= __ExtMfc_XP_COLOR_SHIFT+3,
		XPCLR_HILIGHT_BORDER			= __ExtMfc_XP_COLOR_SHIFT+4,
		XPCLR_HILIGHT_BORDER_SELECTED	= __ExtMfc_XP_COLOR_SHIFT+5,
		XPCLR_RARELY_BORDER				= __ExtMfc_XP_COLOR_SHIFT+6,
		XPCLR_PUSHEDHOVERTEXT			= __ExtMfc_XP_COLOR_SHIFT+7,
		XPCLR_MENU_BORDER				= __ExtMfc_XP_COLOR_SHIFT+8,
		XPCLR_3DFACE_FLOAT_R			= __ExtMfc_XP_COLOR_SHIFT+9,
		XPCLR_3DFACE_FLOAT_F			= __ExtMfc_XP_COLOR_SHIFT+10,
		XPCLR_TEXT_FIELD_BORDER_NORMAL	= __ExtMfc_XP_COLOR_SHIFT+11,
		XPCLR_TEXT_FIELD_BORDER_DISABLED= __ExtMfc_XP_COLOR_SHIFT+12,
		XPCLR_GRIPPER					= __ExtMfc_XP_COLOR_SHIFT+13,
	};

	virtual bool IsMenuMustCombineExcludeArea(
		const CObject * pMenuTrackerObject = NULL
		) const;
	virtual bool IsToolBarChevronMenuCombined();
	virtual void PaintMenuCombinedArea(
		CDC & dc,
		const RECT & rcArea,
		const RECT & rcClient,
		int eCombineAlign, // CExtPopupMenuWnd::e_combine_align_t values
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	virtual void PaintDockingCaptionButton(
		CDC & dc,
		CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA & _pdcbd
		);
	virtual COLORREF GetMenuFrameFillColor(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintControlBarBorders(
		CDC & dc,
		CExtPaintManager::PAINTCONTROLBARBORDERSDATA & _pcbbd
		);

#if (!defined __EXT_MFC_NO_TAB_CTRL)
	virtual void PaintTabItem(
		CDC & dc,
		CRect & rcTabItemsArea,
		bool bTopLeft,
		bool bHorz,
		bool bSelected,
		bool bCenteredText,
		bool bGroupedMode,
		bool bInGroupActive,
		bool bInvertedVerticalMode,
		const CRect & rcEntireItem,
		CSize sizeTextMeasured,
		CFont * pFont,
		__EXT_MFC_SAFE_LPCTSTR sText,
		CExtCmdIcon * pIcon,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		COLORREF clrForceText = COLORREF(-1L),
		COLORREF clrForceTabBk = COLORREF(-1L),
		COLORREF clrForceTabBorderLT = COLORREF(-1L),
		COLORREF clrForceTabBorderRB = COLORREF(-1L),
		COLORREF clrForceTabSeparator = COLORREF(-1L),
		bool bDwmMode = false
		);
	virtual void PaintTabClientArea(
		CDC & dc,
		CRect & rcClient,
		CRect & rcTabItemsArea,
		CRect & rcTabNearBorderArea,
		DWORD dwOrientation,
		bool bGroupedMode,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintTabButton(
		CDC & dc,
		CRect & rcButton,
		LONG nHitTest,
		bool bTopLeft,
		bool bHorz,
		bool bEnabled,
		bool bHover,
		bool bPushed,
		bool bGroupedMode,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		bool bFlat = false
		);
	virtual void PaintTabNcAreaRect(
		CDC & dc,
		const RECT & rc,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
#endif // #if (!defined __EXT_MFC_NO_TAB_CTRL)

	virtual void PaintResizableBarChildNcAreaRect(
		CDC & dc,
		const RECT & rc,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
	virtual void PaintShortcutListBkgnd(
		CDC & dc,
		const RECT & rcClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintShortcutListItem(
		CDC & dc,
		CExtPaintManager::PAINTSHORTCUTLISTITEMSDATA & _pslid
		);
#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)

	virtual void PaintToolbarTextField(
		CDC & dc,
		PAINTTOOLBARTEXTFIELDDATA & _ptbtfd
		);

	virtual COLORREF GetIconAlphaColor() const;
	virtual double GetIconAlphaAdjustPercent() const;

	virtual bool AdjustIcon( 
		CExtCmdIcon * pIcon
		);

	virtual CSize FixedBar_GetRoundedAreaMerics() const;
 
#if (!defined __EXT_MFC_NO_PAGE_NAVIGATOR )
	virtual void PaintPageNavigatorBorder(
		CDC & dc,
		const RECT & rcControl
		);
	virtual void PaintPageNavigatorItem(
		CDC & dc,
		const CRect & rcItem,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		CExtCmdIcon * pIcon,
		bool bExpanded,
		bool bSelected,
		bool bPushed,
		bool bHover
		);	
	virtual void PaintPageNavigatorSplitter(
		CDC & dc,
		const CRect & rcSplitter,
		bool bDrawDots = true
		);
#endif // (!defined __EXT_MFC_NO_PAGE_NAVIGATOR )

#if (!defined __EXT_MFC_NO_DATE_PICKER)
	virtual void PaintDatePickerBorder(
		CDC & dc,
		const RECT & rcControl,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintDatePickerHeaderBackground(
		INT nCol,
		INT nRow,
		CDC & dc,
		INT nYear,
		INT nMonth,
		const RECT & rcHeader,
		const RECT & rcHeaderWithoutBorders,
		const RECT & rcHeaderDate,
		const RECT & rcScrollBtnBackward,
		const RECT & rcScrollBtnForward,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
#endif // (!defined __EXT_MFC_NO_DATE_PICKER)

#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP
	virtual void PaintUndoRedoCaption(
		CDC & dc,
		const CRect & rcCaption,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // __EXT_MFC_NO_UNDO_REDO_POPUP

#if( !defined __EXT_MFC_NO_SPIN || !defined __EXT_MFC_NO_DURATIONWND )
	virtual void PaintSpinButton(
		CDC & dc,
		const CRect & rcButton,
		bool bUp,		
		bool bHorz,
		bool bEnabled,
		bool bFlat,
		bool bDrawBorder,
		bool bPushed,
		bool bHover,
		bool bHotTrack,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // #if( !defined __EXT_MFC_NO_SPIN || !defined __EXT_MFC_NO_DURATIONWND )

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	virtual void DockMarker_AdjustBmps(
		eDockMarkerType_t eDockMarkerType,
		CExtBitmap & bmpIn,
		COLORREF clrTransparentIn,
		CExtBitmap & bmpOut,
		COLORREF clrTransparentOut
		);
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	virtual CRect Ribbon_GetPopupGroupBorderMetrics(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		);
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	// header control
	virtual void Header_PaintBackground(
		CDC & dc,
		CRect rc,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void Header_PaintItem(
		CDC & dc,
		CRect rcItemEntire,
		CRect rcItemData,
		CRect rcIcon,
		CRect rcText,
		CRect rcSortArrow,
		CRect rcButton,
		CRect rcButtonIcon,
		bool bSorted,
		bool bSortedAscending,
		INT nColNo,
		INT nColCount,
		const CExtCmdIcon & iconItem,
		const CExtCmdIcon & iconButton,
		bool bHover,
		bool bPressed,
		bool bButtonEvent,
		__EXT_MFC_SAFE_LPCTSTR strItemText,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
}; // class CExtPaintManagerXP

//////////////////////////////////////////////////////////////////////////
// CExtPaintManagerNativeXP
//////////////////////////////////////////////////////////////////////////

class __PROF_UIS_API CExtPaintManagerNativeXP : public CExtPaintManager
{
public:
	DECLARE_SERIAL( CExtPaintManagerNativeXP );

	// construction/destruction
	CExtPaintManagerNativeXP();
	virtual ~CExtPaintManagerNativeXP();

	virtual e_paint_manager_name_t OnQueryPaintManagerName()
	{
		ASSERT_VALID( this );
		return NativeXP;
	}
	
	virtual bool OnQueryUseThemeColors() const;

	virtual void InitTranslatedColors();

	virtual bool PaintDockerBkgnd(
		bool bClientMapping,
		CDC & dc,
		CWnd * pWnd,
		LPARAM lParam = NULL
		);
	virtual bool GetCb2DbTransparentMode(
		CObject * pObjHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintControlFrame(
		CDC & dc,
		CExtPaintManager::PAINTCONTROLFRAMEDATA & _pcfd
		);
	virtual void PaintComboFrame(
		CDC & dc,
		CExtPaintManager::PAINTCOMBOFRAMEDATA & _pcfd
		);
	virtual void PaintPushButton(
		CDC & dc,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);
	virtual void PaintPushButtonText(
		CDC & dc,
		CRect & rcCaption,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);
	virtual void PaintPushButtonIcon(
		CDC & dc,
		CRect & rcClient,
		CRect & rcCaption,
		CRect & rcFocus,
		CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
		);
	virtual INT GetDropDownButtonWidth(
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		) const;
	virtual INT GetSpinButtonWidth(
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		) const;
	virtual CRect GetSpinButtonMargins(
		CObject * pHelperSrc = NULL,
		LPARAM lParam = 0L
		) const;

	virtual void PaintToolbarExpandButton(
		CDC & dc,
		const RECT & rcButtonArea,
		bool bHorz, // if false - down
		bool bBarIsCompletelyVisible,
		bool bEnabled,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		bool bTransparentBackground = false
		);
	virtual bool Toolbar_GetSizeOfCEB(
		SIZE & sizeReal,
		const SIZE & sizePreCalc,
		CDC & dc,
		BOOL bHorz,
		CExtBarContentExpandButton * pTBB,
		LPARAM lParam = 0
		);
	virtual void PaintMenuExpandButton(
		CDC & dc,
		const RECT & rectButton,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual CRect GetMenuBorderMetrics(
		CWnd * pWnd,
		LPARAM lParam = 0
		) const;
	virtual void PaintMenuBorder(
		CDC & dc,
		const RECT & rectClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool PaintMenuItem_UX(
		HWND hWnd,
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuItem(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuItemIcon(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuTearOffButton(
		CDC & dc,
		const RECT & rectButton,
		bool bPushed,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool PaintSysCmdGlyph_UX(
		HWND hWnd,
		CDC & dc,
		UINT nHelperSysCmdID,
		const RECT & rcItem,
		bool bHover,
		bool bPushed,
		bool bEnabled,
		COLORREF clrForceGlyphColor = COLORREF(-1L)
		);
	virtual bool PaintMenuItemIcon_UX(
		HWND hWnd,
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual void PaintMenuItemInPlaceWnd(
		CDC & dc,
		CExtPaintManager::PAINTMENUITEMDATA & _pmid
		);
	virtual bool PaintMenuSeparator_UX(
		HWND hWnd,
		CDC & dc,
		const RECT & rectItem,
		int nIconAreaWidth,
		bool bRarelyUsed,
		bool bIsForceNoLeftGradient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintMenuSeparator(
		CDC & dc,
		const RECT & rectItem,
		int nIconAreaWidth,
		bool bRarelyUsed,
		bool bIsForceNoLeftGradient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintSeparator(
		CDC & dc,
		const RECT & rectItem,
		bool bHorz,
		bool bTransparentBk,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintResizingGripper(
		CDC & dc,
		const RECT & rcGrip,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual COLORREF GetMenuFrameFillColor(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintControlBarClientArea(
		CDC & dc,
		const RECT & rcClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintDockBarClientArea(
		CDC & dc,
		const RECT & rcClient,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintControlBarBorders(
		CDC & dc,
		CExtPaintManager::PAINTCONTROLBARBORDERSDATA & _pcbbd
		);
	virtual void PaintGripper(
		CDC & dc,
		CExtPaintManager::PAINTGRIPPERDATA & _pgd
		);
	virtual void PaintDockingFrame(
		CDC & dc,
		CExtPaintManager::PAINTDOCKINGFRAMEDATA & _pdfd
		);
	virtual void FixedBar_AdjustClientRect(
		const CExtControlBar * pBar,
		CRect & rcPreCalc
		);
	virtual void PaintDockingCaptionButton(
		CDC & dc,
		CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA & _pdcbd
		);
	virtual bool Bar_LayoutNcAreaButton(
		RECT & rcButton,
		const RECT & rcCaption,
		CExtBarNcAreaButton * pLayoutBtn,
		CExtBarNcAreaButton * pPrevBtn,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual bool Bar_MiniDockFrameGetNcMetrics(
		INT & nResizingFrameDX,
		INT & nResizingFrameDY,
		INT & nCaptionDY,
		CExtMiniDockFrameWnd * pMiniFrameWnd,
		CExtControlBar * pBar,
		LPARAM lParam = 0
		) const;

#if (!defined __EXT_MFC_NO_TAB_CTRL)
	virtual bool QueryTabWndHoverChangingRedraw(
		const CExtTabWnd * pWndTab,
		LPARAM lParam = 0L
		);
	virtual void TabWnd_AdjustTabAreaMetric( CExtTabWnd * pTabWnd, DWORD dwOrientation, LONG & nTabAreaMetric, LPARAM lParam = 0L );
	virtual void TabWnd_MeasureItemAreaMargins(
		CExtTabWnd * pTabWnd,
		LONG & nSpaceBefore,
		LONG & nSpaceAfter,
		LONG & nSpaceOver
		);	
	virtual void TabWnd_UpdateItemMeasure(
		CExtTabWnd * pTabWnd,
		CExtTabWnd::TAB_ITEM_INFO * pTii,
		CDC & dcMeasure,
		CSize & sizePreCalc
		);	
	virtual bool TabWnd_GetParentSizingMargin(
		INT & nMargin,
		DWORD dwOrientation,
		CExtTabWnd * pTabWnd
		) const;
	virtual void PaintTabNcAreaRect(
		CDC & dc,
		const RECT & rc,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintTabClientArea(
		CDC & dc,
		CRect & rcClient,
		CRect & rcTabItemsArea,
		CRect & rcTabNearBorderArea,
		DWORD dwOrientation,
		bool bGroupedMode,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void PaintTabItem(
		CDC & dc,
		CRect & rcTabItemsArea,
		bool bTopLeft,
		bool bHorz,
		bool bSelected,
		bool bCenteredText,
		bool bGroupedMode,
		bool bInGroupActive,
		bool bInvertedVerticalMode,
		const CRect & rcEntireItem,
		CSize sizeTextMeasured,
		CFont * pFont,
		__EXT_MFC_SAFE_LPCTSTR sText,
		CExtCmdIcon * pIcon,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		COLORREF clrForceText = COLORREF(-1L),
		COLORREF clrForceTabBk = COLORREF(-1L),
		COLORREF clrForceTabBorderLT = COLORREF(-1L),
		COLORREF clrForceTabBorderRB = COLORREF(-1L),
		COLORREF clrForceTabSeparator = COLORREF(-1L),
		bool bDwmMode = false
		);
#endif // (!defined __EXT_MFC_NO_TAB_CTRL)

#ifndef __EXT_MFC_NO_STATUSBAR
	virtual bool StatusBar_EraseBackground(
		CDC & dc,
		const RECT & rcClient,
		const CExtStatusControlBar * pStatusBar,
		LPARAM lParam = 0
		) const;
	virtual bool StatusBar_ErasePaneBackground(
		CDC & dc,
		int nPaneIdx,
		const RECT & rcPane,
		const CExtStatusControlBar * pStatusBar,
		LPARAM lParam = 0
		) const;
	virtual bool StatusBar_PaintSeparator(
		CDC & dc,
		int nPaneIdxAfterSep,
		const RECT & rcPaneBefore,
		const RECT & rcPaneAfter,
		const CExtStatusControlBar * pStatusBar,
		LPARAM lParam = 0
		) const;
#endif // __EXT_MFC_NO_STATUSBAR

#if( !defined __EXT_MFC_NO_SPIN || !defined __EXT_MFC_NO_DURATIONWND )
	virtual CRect Spin_QueryNcAreaMargins(
		bool bSpinAlignRight,
		bool bSpinAlignLeft,
		bool bHasScrolBars
		) const;	
	virtual void PaintSpinButton(
		CDC & dc,
		const CRect & rcButton,
		bool bUp,		
		bool bHorz,
		bool bEnabled,
		bool bFlat,
		bool bDrawBorder,
		bool bPushed,
		bool bHover,
		bool bHotTrack,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // #if( !defined __EXT_MFC_NO_SPIN || !defined __EXT_MFC_NO_DURATIONWND )

#ifndef __EXT_MFC_NO_SLIDER
	virtual bool PaintSliderChannel(
		CDC & dc,
		const CRect & rcChannel,
		bool bHorz,
		bool bEnabled,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
	virtual bool PaintSliderThumb(
		CDC & dc,
		const CRect & rcThumb,
		bool bHorz,
		bool bLeft,
		bool bRight,
		bool bEnabled,
		bool bPushed,
		bool bFocused,
		bool bHover,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // __EXT_MFC_NO_SLIDER

#ifndef __EXT_MFC_NO_DURATIONWND
	virtual INT Duration_GetSpinButtonWidth(
		const CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual INT Duration_GetDropDownButtonWidth(
		const CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual CRect Duration_GetSpinButtonExtraSpace(
		const CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual CRect Duration_GetDropDownButtonExtraSpace(
		const CObject * pHelperSrc,
		LPARAM lParam = 0L
		) const;
	virtual void Duration_PaintSpinButton(
		CDC & dc,
		const CRect & rcButton,
		bool bUp,		
		bool bEnabled,
		bool bPushed,
		bool bHovered,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
	virtual void Duration_PaintDropDownButton(
		CDC & dc,
		const CRect & rcButton,
		bool bEnabled,
		bool bPushed,
		bool bHovered,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);	
#endif // __EXT_MFC_NO_DURATIONWND

#ifndef __EXT_MFC_NO_GRIDWND
	virtual bool Grid_PaintHeaderSortArrow(
		CDC & dc,
		const RECT & rcSortArrow,
		bool bHorz,
		bool bAscending,
		COLORREF * pColorValues,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool Grid_PaintHeaderBackground(
		CDC & dc,
		const RECT & rcArea,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		DWORD dwAreaFlags,
		DWORD dwHelperPaintFlags,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
#endif // #ifndef __EXT_MFC_NO_GRIDWND

#if (!defined __EXT_MFC_NO_REPORTGRIDWND)
	virtual bool ReportGrid_PaintHeaderRowBackground(
		CDC & dc,
		const RECT & rcArea,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool ReportGrid_PaintHeaderSortArrow(
		CDC & dc,
		const RECT & rcSortArrow,
		bool bAscending,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool ReportGrid_PaintGroupAreaBackground(
		CDC & dc,
		const RECT & rcArea,
		__EXT_MFC_SAFE_LPCTSTR strEmptyAreaCaption, // not NULL if area is empty
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual COLORREF ReportGrid_GetHeaderTextColor(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
#endif // (!defined __EXT_MFC_NO_REPORTGRIDWND)

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	virtual INT Ribbon_GetTabIntersectionHeight(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		) const;
	virtual void Ribbon_PaintTabItem(
		CDC & dc,
		CRect & rcTabItemsArea,
		bool bSelected,
		const CRect & rcEntireItem,
		CFont * pFont,
		__EXT_MFC_SAFE_LPCTSTR sText,
		CExtCmdIcon * pIcon,
		CObject * pHelperSrc,
		LPARAM lParam = 0L,
		COLORREF clrForceText = COLORREF(-1L),
		COLORREF clrForceTabBk = COLORREF(-1L),
		COLORREF clrForceTabBorderLT = COLORREF(-1L),
		COLORREF clrForceTabBorderRB = COLORREF(-1L),
		COLORREF clrForceTabSeparator = COLORREF(-1L)
		);
	virtual void Ribbon_PaintPageBk(
		CDC & dc,
		const RECT & rcPageBk,
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		);
	virtual CRect Ribbon_GetPageOuterContentPadding(
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		);
	virtual INT Ribbon_GetGroupHeight(
		const CExtRibbonButtonGroup * pGroupTBB,
		LPARAM lParam = 0L
		);
	virtual void RibbonQuickAccessBar_PaintBkAtTheBottom(
		CDC & dc,
		CRect rcQATB,
		const CExtRibbonPage * pRibbonPage,
		LPARAM lParam = 0L
		) const;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	virtual void AdvTip_CalcRgn(
		HRGN hRgnPrecalc,
		const RECT & rcRectRgn,
		bool bDynamicShadowApplied,
		INT nSizeInnerShadowIncluded,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual bool AdvTip_PaintBackground(
		CDC & dc,
		const RECT & rcClient,
		bool bAdvTipWithShadow,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual CFont * AdvTip_GetFont(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual COLORREF AdvTip_GetTextColor(
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

	virtual CSize GetPushedOffset();
	virtual const CExtAnimationParameters *
		Animation_GetParameters(
			INT eAPT, // __EAPT_*** animation type
			CObject * pHelperSrc,
			const CExtAnimationClient * pAC,
			LPARAM lParam = 0L
			) const;

	virtual bool PaintProgress(
		CDC & dc,
		PAINTPROGRESSDATA & _ppd
		);

	// header control
	virtual void Header_PaintBackground(
		CDC & dc,
		CRect rc,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	virtual void Header_PaintItem(
		CDC & dc,
		CRect rcItemEntire,
		CRect rcItemData,
		CRect rcIcon,
		CRect rcText,
		CRect rcSortArrow,
		CRect rcButton,
		CRect rcButtonIcon,
		bool bSorted,
		bool bSortedAscending,
		INT nColNo,
		INT nColCount,
		const CExtCmdIcon & iconItem,
		const CExtCmdIcon & iconButton,
		bool bHover,
		bool bPressed,
		bool bButtonEvent,
		__EXT_MFC_SAFE_LPCTSTR strItemText,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	static bool stat_PaintHeaderPartUsingUxTheme(
		CDC & dc,
		CRect rc,
		CObject * pHelperSrc,
		LPARAM lParam,
		bool bDrawItemLikeBackground = false,
		bool bHover = false,
		bool bPressed = false
		);
}; // class CExtPaintManagerNativeXP

extern __PROF_UIS_API CExtPaintManager::CExtPaintManagerAutoPtr g_PaintManager;

#endif // __EXT_PAINT_MANAGER_H

