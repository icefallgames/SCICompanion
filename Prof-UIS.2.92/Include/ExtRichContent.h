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

#if (!defined __EXT_RICH_CONTENT_H)
#define __EXT_RICH_CONTENT_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExtBitmap;

#define __EXT_RICH_CONTENT_NEGATIVE_UNUSED							(-32761)
#define __EXT_RICH_CONTENT_BIG_EXTENT								32767

#define __EXT_RICH_CONTENT_TRIM_CHARS								_T(" \t\r\n")
#define __EXT_RICH_CONTENT_TRIM_CHARS_AND_QUOTES					_T(" \t\r\n\'\"")
#define __EXT_RICH_CONTENT_TRIM_CLASS_NAME							_T(" \t\r\n\'\".")
#define __EXT_RICH_CONTENT_TRIM_CHARS_WITHOUT_SPACES				_T("\t\r\n")
#define __EXT_RICH_CONTENT_TRIM_OBJECT_NAME							__EXT_RICH_CONTENT_TRIM_CLASS_NAME

#define __EXT_RICH_CONTENT_DEFAULT_LOCAL_OBJECT_CONTAINER_NAME		_T("$-object-container-local-$")
#define __EXT_RICH_CONTENT_DEFAULT_GLOBAL_OBJECT_CONTAINER_NAME		_T("$-object-container-global-$")
#define __EXT_RICH_CONTENT_DEFAULT_CSS_DOC_OBJECT_NAME				_T("$-default-css-$")

#define __EXT_DT_MULTI_LINE_VERT_ALIGNMENT							0x00000001
#define __EXT_DT_CALC_EFFECTIVE_RECT								0x00000002

class CExtRichFontDescription;
class CExtRichStyleDescription;
class CExtRichStyleRule;

class CExtRichTableLayout;
class CExtRichTableRow;
class CExtRichContentHitTestInfo;
class CExtRichContentItem;
class CExtRichContentEvents;
class CExtRichContentEventsBroadcast;
class CExtRichContentLayout;

class CExtRichDocObjectBase;
class CExtRichDocObjectContainer;
class CExtRichDocObjectImage;
class CExtRichDocObjectLayoutKeeper;

	#define __EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( __PTR__ )					;
	#define __EXT_DEBUG_RICH_CONTENT_ASSERT_KINDOF( __CTYPE__, __PTR__ )		;
	#define __EXT_DEBUG_RICH_CONTENT_ASSERT( __SOME_EXPR__ )					;
	#define __EXT_DEBUG_RICH_CONTENT_VERIFY( __SOME_EXPR__ )					__SOME_EXPR__ ;

//	#define __EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( __PTR__ )					ASSERT_VALID( __PTR__ ) ;
//	#define __EXT_DEBUG_RICH_CONTENT_ASSERT_KINDOF( __CTYPE__, __PTR__ )		ASSERT_KINDOF( __CTYPE__, __PTR__ ) ;
//	#define __EXT_DEBUG_RICH_CONTENT_ASSERT( __SOME_EXPR__ )					ASSERT( __SOME_EXPR__ ) ;
//	#define __EXT_DEBUG_RICH_CONTENT_VERIFY( __SOME_EXPR__ )					VERIFY( __SOME_EXPR__ ) ;

//////////////////////////////////////////////////////////////////////////
// UXTHEME defines
//////////////////////////////////////////////////////////////////////////

// Define API decoration for direct importing of DLL references.
#define __EXT_UX_THEMEAPI(_name_)			HRESULT ( STDAPICALLTYPE * _name_ )
#define __EXT_UX_THEMEAPI_(_type_,_name_)	_type_ ( STDAPICALLTYPE * _name_ )

#define __EXT_UX_DTT_GRAYED                 0x1
#define __EXT_UX_DTT_FLAGS2VALIDBITS        (__EXT_UX_DTT_GRAYED)

#define __EXT_UX_HTTB_BACKGROUNDSEG         0x0000  
#define __EXT_UX_HTTB_FIXEDBORDER           0x0002
#define __EXT_UX_HTTB_CAPTION               0x0004  
#define __EXT_UX_HTTB_RESIZINGBORDER_LEFT   0x0010
#define __EXT_UX_HTTB_RESIZINGBORDER_TOP    0x0020
#define __EXT_UX_HTTB_RESIZINGBORDER_RIGHT  0x0040
#define __EXT_UX_HTTB_RESIZINGBORDER_BOTTOM 0x0080
#define __EXT_UX_HTTB_RESIZINGBORDER \
	(	__EXT_UX_HTTB_RESIZINGBORDER_LEFT \
	|	__EXT_UX_HTTB_RESIZINGBORDER_TOP \
	|	__EXT_UX_HTTB_RESIZINGBORDER_RIGHT \
	|	__EXT_UX_HTTB_RESIZINGBORDER_BOTTOM \
	)
#define __EXT_UX_HTTB_SIZINGTEMPLATE        0x0100
#define __EXT_UX_HTTB_SYSTEMSIZINGMARGINS   0x0200
#define __EXT_UX_MAX_INTLIST_COUNT          10
#define __EXT_UX_ETDT_DISABLE               0x00000001
#define __EXT_UX_ETDT_ENABLE                0x00000002
#define __EXT_UX_ETDT_USETABTEXTURE         0x00000004
#define __EXT_UX_ETDT_ENABLETAB \
	(	__EXT_UX_ETDT_ENABLE \
	|	__EXT_UX_ETDT_USETABTEXTURE \
	)
#define __EXT_UX_STAP_ALLOW_NONCLIENT       (1 << 0)
#define __EXT_UX_STAP_ALLOW_CONTROLS        (1 << 1)
#define __EXT_UX_STAP_ALLOW_WEBCONTENT      (1 << 2)
#define __EXT_UX_SZ_THDOCPROP_DISPLAYNAME   L"DisplayName"
#define __EXT_UX_SZ_THDOCPROP_CANONICALNAME L"ThemeName"
#define __EXT_UX_SZ_THDOCPROP_TOOLTIP       L"ToolTip"
#define __EXT_UX_SZ_THDOCPROP_AUTHOR        L"author"
#define __EXT_UX_DTBG_CLIPRECT              0x00000001
#define __EXT_UX_DTBG_DRAWSOLID             0x00000002
#define __EXT_UX_DTBG_OMITBORDER            0x00000004
#define __EXT_UX_DTBG_OMITCONTENT           0x00000008
#define __EXT_UX_DTBG_COMPUTINGREGION       0x00000010
#define __EXT_UX_DTBG_MIRRORDC              0x00000020

#define __EXT_UX_OTD_FORCE_RECT_SIZING      0x00000001 // make all parts size to rect
#define __EXT_UX_OTD_NONCLIENT              0x00000002 // set if hTheme to be used for nonclient area
#define __EXT_UX_OTD_VALIDBITS              (__EXT_UX_OTD_FORCE_RECT_SIZING|__EXT_UX_OTD_NONCLIENT)

// bits used in dwFlags of DTBGOPTS ----
#define __EXT_UX_DTBG_CLIPRECT              0x00000001  // rcClip has been specified
#define __EXT_UX_DTBG_DRAWSOLID             0x00000002  // DEPRECATED: draw transparent/alpha images as solid
#define __EXT_UX_DTBG_OMITBORDER            0x00000004  // don't draw border of part
#define __EXT_UX_DTBG_OMITCONTENT           0x00000008  // don't draw content area of part
#define __EXT_UX_DTBG_COMPUTINGREGION       0x00000010  // TRUE if calling to compute region
#define __EXT_UX_DTBG_MIRRORDC              0x00000020  // assume the hdc is mirrorred and
														// flip images as appropriate (currently 
			                                            // only supported for bgtype=imagefile)
#define __EXT_UX_DTBG_NOMIRROR              0x00000040  // don't mirror the output, overrides everything else 
#define __EXT_UX_DTBG_VALIDBITS             (__EXT_UX_DTBG_CLIPRECT|__EXT_UX_DTBG_DRAWSOLID      | \
											__EXT_UX_DTBG_OMITBORDER|__EXT_UX_DTBG_OMITCONTENT   | \
											__EXT_UX_DTBG_COMPUTINGREGION|__EXT_UX_DTBG_MIRRORDC | \
											__EXT_UX_DTBG_NOMIRROR)

#define __EXT_UX_DTT_TEXTCOLOR       (1UL << 0)      // crText has been specified
#define __EXT_UX_DTT_BORDERCOLOR     (1UL << 1)      // crBorder has been specified
#define __EXT_UX_DTT_SHADOWCOLOR     (1UL << 2)      // crShadow has been specified
#define __EXT_UX_DTT_SHADOWTYPE      (1UL << 3)      // iTextShadowType has been specified
#define __EXT_UX_DTT_SHADOWOFFSET    (1UL << 4)      // ptShadowOffset has been specified
#define __EXT_UX_DTT_BORDERSIZE      (1UL << 5)      // iBorderSize has been specified
#define __EXT_UX_DTT_FONTPROP        (1UL << 6)      // iFontPropId has been specified
#define __EXT_UX_DTT_COLORPROP       (1UL << 7)      // iColorPropId has been specified
#define __EXT_UX_DTT_STATEID         (1UL << 8)      // IStateId has been specified
#define __EXT_UX_DTT_CALCRECT        (1UL << 9)      // Use pRect as and in/out parameter
#define __EXT_UX_DTT_APPLYOVERLAY    (1UL << 10)     // fApplyOverlay has been specified
#define __EXT_UX_DTT_GLOWSIZE        (1UL << 11)     // iGlowSize has been specified
#define __EXT_UX_DTT_CALLBACK        (1UL << 12)     // pfnDrawTextCallback has been specified
#define __EXT_UX_DTT_COMPOSITED      (1UL << 13)     // Draws text with antialiased alpha (needs a DIB section)
#define __EXT_UX_DTT_VALIDBITS       (__EXT_UX_DTT_TEXTCOLOR    | \
                                      __EXT_UX_DTT_BORDERCOLOR  | \
                                      __EXT_UX_DTT_SHADOWCOLOR  | \
                                      __EXT_UX_DTT_SHADOWTYPE   | \
                                      __EXT_UX_DTT_SHADOWOFFSET | \
                                      __EXT_UX_DTT_BORDERSIZE   | \
                                      __EXT_UX_DTT_FONTPROP     | \
                                      __EXT_UX_DTT_COLORPROP    | \
                                      __EXT_UX_DTT_STATEID      | \
                                      __EXT_UX_DTT_CALCRECT     | \
                                      __EXT_UX_DTT_APPLYOVERLAY | \
                                      __EXT_UX_DTT_GLOWSIZE     | \
                                      __EXT_UX_DTT_COMPOSITED)
#if (defined __EXT_MFC_TEMPLATED_CHARS_IMPLEMENTED)
	#if (defined __EXT_MFC_COMPILED_WITH_NATIVE_WCHAR_T )
		typedef wchar_t * __EXT_UX_LPWSTR;
		typedef const wchar_t * __EXT_UX_LPCWSTR;
	#else
		typedef unsigned short * __EXT_UX_LPWSTR;
		typedef const unsigned short * __EXT_UX_LPCWSTR;
	#endif
#else
		typedef LPWSTR __EXT_UX_LPWSTR;
		typedef LPCWSTR __EXT_UX_LPCWSTR;
#endif

//////////////////////////////////////////////////////////////////////////
// CExtUxTheme class
//////////////////////////////////////////////////////////////////////////

class __PROF_UIS_API CExtUxTheme
{
public:

	typedef enum __EXT_UX_THEMESIZE
	{
		__EXT_UX_TS_MIN,             // minimum size
		__EXT_UX_TS_TRUE,            // size without stretching
		__EXT_UX_TS_DRAW,            // size that theme mgr will use to draw part
	};

	typedef struct __PROF_UIS_API __EXT_UX__MARGINS
	{
		int cxLeftWidth;             // width of left border that retains its size
		int cxRightWidth;            // width of right border that retains its size
		int cyTopHeight;             // height of top border that retains its size
		int cyBottomHeight;          // height of bottom border that retains its size
	} __EXT_UX_MARGINS, *__EXT_UX_PMARGINS;

	typedef struct __PROF_UIS_API __EXT_UX__INTLIST
	{
		int iValueCount;      // number of values in iValues
		int iValues[__EXT_UX_MAX_INTLIST_COUNT];
	} __EXT_UX_INTLIST, *__EXT_UX_PINTLIST;

	typedef enum __EXT_UX_PROPERTYORIGIN
	{
		__EXT_UX_PO_STATE,           // property was found in the state section
		__EXT_UX_PO_PART,            // property was found in the part section
		__EXT_UX_PO_CLASS,           // property was found in the class section
		__EXT_UX_PO_GLOBAL,          // property was found in [globals] section
		__EXT_UX_PO_NOTFOUND         // property was not found
	};

	typedef struct __PROF_UIS_API __EXT_UX__DTBGOPTS
	{
		DWORD dwSize;                // size of the struct
		DWORD dwFlags;               // which options have been specified
		RECT rcClip;                 // clipping rectangle
	}
	__EXT_UX_DTBGOPTS, *__EXT_UX_PDTBGOPTS;

// Callback function used by DrawTextWithGlow instead of DrawTextW
	typedef int ( WINAPI * __EXT_UX_DTT_CALLBACK_PROC ) (
		HDC hdc,
		__EXT_UX_LPWSTR pszText,
		int cchText,
		LPRECT prc,
		UINT dwFlags,
		LPARAM lParam
		);
	typedef struct __EXT_UX__DTTOPTS
	{
		DWORD             dwSize;              // size of the struct
		DWORD             dwFlags;             // which options have been specified
		COLORREF          crText;              // color to use for text fill
		COLORREF          crBorder;            // color to use for text outline
		COLORREF          crShadow;            // color to use for text shadow
		int               iTextShadowType;     // TST_SINGLE or TST_CONTINUOUS
		POINT             ptShadowOffset;      // where shadow is drawn (relative to text)
		int               iBorderSize;         // Border radius around text
		int               iFontPropId;         // Font property to use for the text instead of TMT_FONT
		int               iColorPropId;        // Color property to use for the text instead of TMT_TEXTCOLOR
		int               iStateId;            // Alternate state id
		BOOL              fApplyOverlay;       // Overlay text on top of any text effect?
		int               iGlowSize;           // Glow radious around text
		__EXT_UX_DTT_CALLBACK_PROC
		                  pfnDrawTextCallback; // Callback for DrawText
		LPARAM            lParam;              // Parameter for callback
	} __EXT_UX_DTTOPTS, *__EXT_UX_PDTTOPTS;

	typedef HANDLE __EXT_UX_HTHEME;            // handle to a section of theme data for class

	CExtUxTheme();
	virtual ~CExtUxTheme();

	bool IsOpened() const;

	bool IsControlsThemed( 
		bool bCheckComCtlVersion = true 
		);
	bool IsNonClientThemed( 
		bool bCheckComCtlVersion = true 
		);
	bool IsWebContentThemed( 
		bool bCheckComCtlVersion = true 
		);

	//---------------------------------------------------------------------------
	//  OpenThemeData()     - Open the theme data for the specified HWND and 
	//                        semi-colon separated list of class names. 
	// 
	//                        OpenThemeData() will try each class name, one at 
	//                        a time, and use the first matching theme info
	//                        found.  If a match is found, a theme handle
	//                        to the data is returned.  If no match is found,
	//                        a "NULL" handle is returned. 
	//
	//                        When the window is destroyed or a WM_THEMECHANGED
	//                        msg is received, "CloseThemeData()" should be 
	//                        called to close the theme handle.
	//
	//  hWnd                - window handle of the control/window to be themed
	//  pszClassList        - class name (or list of names) to match to theme data
	//                        section.  if the list contains more than one name, 
	//                        the names are tested one at a time for a match.  
	//                        If a match is found, OpenThemeData() returns a 
	//                        theme handle associated with the matching class. 
	//                        This param is a list (instead of just a single 
	//                        class name) to provide the class an opportunity 
	//                        to get the "best" match between the class and 
	//                        the current theme.  For example, a button might
	//                        pass L"OkButton, Button" if its ID=ID_OK.  If 
	//                        the current theme has an entry for OkButton, 
	//                        that will be used.  Otherwise, we fall back on 
	//                        the normal Button entry.
	//---------------------------------------------------------------------------
	__EXT_UX_HTHEME OpenThemeData(
		HWND hWnd, 
		__EXT_UX_LPCWSTR pszClassList
		);

	//---------------------------------------------------------------------------
	//  OpenThemeDataEx     - Open the theme data for the specified HWND and 
	//                        semi-colon separated list of class names. 
	// 
	//                        OpenThemeData() will try each class name, one at 
	//                        a time, and use the first matching theme info
	//                        found.  If a match is found, a theme handle
	//                        to the data is returned.  If no match is found,
	//                        a "NULL" handle is returned. 
	//
	//                        When the window is destroyed or a WM_THEMECHANGED
	//                        msg is received, "CloseThemeData()" should be 
	//                        called to close the theme handle.
	//
	//  hwnd                - window handle of the control/window to be themed
	//
	//  pszClassList        - class name (or list of names) to match to theme data
	//                        section.  if the list contains more than one name, 
	//                        the names are tested one at a time for a match.  
	//                        If a match is found, OpenThemeData() returns a 
	//                        theme handle associated with the matching class. 
	//                        This param is a list (instead of just a single 
	//                        class name) to provide the class an opportunity 
	//                        to get the "best" match between the class and 
	//                        the current theme.  For example, a button might
	//                        pass L"OkButton, Button" if its ID=ID_OK.  If 
	//                        the current theme has an entry for OkButton, 
	//                        that will be used.  Otherwise, we fall back on 
	//                        the normal Button entry.
	//
	//  dwFlags             - allows certain overrides of std features
	//                        (see OTD_XXX defines above)
	//---------------------------------------------------------------------------
	__EXT_UX_HTHEME OpenThemeDataEx(
		HWND hwnd,
		__EXT_UX_LPCWSTR pszClassList,
		DWORD dwFlags
		);

	//---------------------------------------------------------------------------
	//  CloseTHemeData()    - closes the theme data handle.  This should be done 
	//                        when the window being themed is destroyed or
	//                        whenever a WM_THEMECHANGED msg is received 
	//                        (followed by an attempt to create a new Theme data 
	//                        handle).
	//---------------------------------------------------------------------------
	HRESULT CloseThemeData( 
		bool bAllowCaching = false
		);

	//---------------------------------------------------------------------------
	//    functions for basic drawing support 
	//---------------------------------------------------------------------------
	// The following methods are the theme-aware drawing services.
	// Controls/Windows are defined in drawable "parts" by their author: a 
	// parent part and 0 or more child parts.  Each of the parts can be 
	// described in "states" (ex: disabled, hot, pressed).  
	//---------------------------------------------------------------------------
	// For the list of all themed classes and the definition of all
	// parts and states, see the file "tmschmea.h".
	//---------------------------------------------------------------------------
	// Each of the below methods takes a "iPartId" param to specify the 
	// part and a "iStateId" to specify the state of the part.  
	// "iStateId=0" refers to the root part.  "iPartId" = "0" refers to 
	// the root class.  
	//-----------------------------------------------------------------------
	// Note: draw operations are always scaled to fit (and not to exceed)  
	// the specified "Rect".
	//-----------------------------------------------------------------------

	//------------------------------------------------------------------------
	//  DrawThemeBackground()   
	//                      - draws the theme-specified border and fill for 
	//                        the "iPartId" and "iStateId".  This could be 
	//                        based on a bitmap file, a border and fill, or 
	//                        other image description.  
	//
	//  hdc                 - HDC to draw into
	//  iPartId             - part number to draw
	//  iStateId            - state number (of the part) to draw
	//  pRect               - defines the size/location of the part
	//  pClipRect           - optional clipping rect (don't draw outside it)
	//------------------------------------------------------------------------
	HRESULT DrawThemeBackground(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		const RECT * pRect, 
		OPTIONAL const RECT * pClipRect
		);

	HRESULT DrawBackground(
		HWND hWnd,
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		const RECT * pRect, 
		OPTIONAL const RECT * pClipRect
		);

	//-------------------------------------------------------------------------
	//  DrawThemeText()     - draws the text using the theme-specified 
	//                        color and font for the "iPartId" and 
	//                        "iStateId".  
	//
	//  hdc                 - HDC to draw into
	//  iPartId             - part number to draw
	//  iStateId            - state number (of the part) to draw
	//  pszText             - actual text to draw
	//  dwCharCount         - number of chars to draw (-1 for all)
	//  dwTextFlags         - same as DrawText() "uFormat" param
	//  dwTextFlags2        - additional drawing options 
	//  pRect               - defines the size/location of the part
	//-------------------------------------------------------------------------
	HRESULT DrawThemeText(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		__EXT_UX_LPCWSTR pszText, 
		int iCharCount, 
		DWORD dwTextFlags, 
		DWORD dwTextFlags2, 
		const RECT * pRect
		);
	HRESULT DrawThemeText(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		const CExtSafeString & sText, 
		int iCharCount, 
		DWORD dwTextFlags, 
		DWORD dwTextFlags2, 
		const RECT * pRect
		);

	HRESULT DrawThemeTextEx(
		HDC hdc,
		int iPartId,
		int iStateId,
		__EXT_UX_LPCWSTR pszText,
		int iCharCount,
		DWORD dwFlags,
		LPRECT pRect,
		const __EXT_UX_DTTOPTS * pOptions
		);
	HRESULT DrawThemeTextEx(
		HDC hdc,
		int iPartId,
		int iStateId,
		const CExtSafeString & sText, 
		int iCharCount,
		DWORD dwFlags,
		LPRECT pRect,
		const __EXT_UX_DTTOPTS * pOptions
		);

	//-------------------------------------------------------------------------
	//  GetThemeBackgroundContentRect()
	//                      - gets the size of the content for the theme-defined 
	//                        background.  This is usually the area inside
	//                        the borders or Margins.  
	//
	//      hdc             - (optional) device content to be used for drawing
	//      iPartId         - part number to draw
	//      iStateId        - state number (of the part) to draw
	//      pBoundingRect   - the outer RECT of the part being drawn
	//      pContentRect    - RECT to receive the content area
	//-------------------------------------------------------------------------
	HRESULT GetThemeBackgroundContentRect(
		OPTIONAL HDC hdc, 
		int iPartId, 
		int iStateId,  
		const RECT * pBoundingRect, 
		OUT RECT * pContentRect
		);

	//-------------------------------------------------------------------------
	//  GetThemeBackgroundExtent() - calculates the size/location of the theme-
	//                               specified background based on the 
	//                               "pContentRect".
	//
	//      hdc             - (optional) device content to be used for drawing
	//      iPartId         - part number to draw
	//      iStateId        - state number (of the part) to draw
	//      pContentRect    - RECT that defines the content area
	//      pBoundingRect   - RECT to receive the overall size/location of part
	//-------------------------------------------------------------------------
	HRESULT GetThemeBackgroundExtent(
		OPTIONAL HDC hdc,
		int iPartId, 
		int iStateId, 
		const RECT * pContentRect, 
		OUT RECT * pExtentRect
		);

	//-------------------------------------------------------------------------
	//  GetThemePartSize() - returns the specified size of the theme part
	//
	//  hdc                 - HDC to select font into & measure against
	//  iPartId             - part number to retrieve size for
	//  iStateId            - state number (of the part) 
	//  prc                 - (optional) rect for part drawing destination
	//  eSize               - the type of size to be retreived
	//  psz                 - receives the specified size of the part 
	//-------------------------------------------------------------------------
	HRESULT GetThemePartSize(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		OPTIONAL RECT * prc, 
		enum __EXT_UX_THEMESIZE eSize, 
		OUT SIZE * psz
		);

	//-------------------------------------------------------------------------
	//  GetThemeTextExtent() - calculates the size/location of the specified 
	//                         text when rendered in the Theme Font. 
	//
	//  hdc                 - HDC to select font & measure into
	//  iPartId             - part number to draw
	//  iStateId            - state number (of the part) 
	//  pszText             - the text to be measured
	//  dwCharCount         - number of chars to draw (-1 for all)
	//  dwTextFlags         - same as DrawText() "uFormat" param
	//  pszBoundingRect     - optional: to control layout of text
	//  pszExtentRect       - receives the RECT for text size/location
	//-------------------------------------------------------------------------
	HRESULT GetThemeTextExtent(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		__EXT_UX_LPCWSTR pszText, 
		int iCharCount, 
		DWORD dwTextFlags, 
		OPTIONAL const RECT * pBoundingRect, 
		OUT RECT * pExtentRect
		);
	HRESULT GetThemeTextExtent(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		const CExtSafeString & sText, 
		int iCharCount, 
		DWORD dwTextFlags, 
		OPTIONAL const RECT * pBoundingRect, 
		OUT RECT * pExtentRect
		);

	//-------------------------------------------------------------------------
	//  GetThemeTextMetrics()
	//                      - returns info about the theme-specified font
	//                        for the part/state passed in.
	//
	//  hdc                 - optional: HDC for screen context
	//  iPartId             - part number to draw
	//  iStateId            - state number (of the part) 
	//  ptm                 - receives the font info
	//-------------------------------------------------------------------------
	HRESULT GetThemeTextMetrics(
		OPTIONAL HDC hdc, 
		int iPartId, 
		int iStateId, 
		OUT TEXTMETRIC * ptm
		);

	//-------------------------------------------------------------------------
	//  GetThemeBackgroundRegion()
	//                      - computes the region for a regular or partially 
	//                        transparent theme-specified background that is 
	//                        bound by the specified "pRect".
	//                        If the rectangle is empty, sets the HRGN to NULL 
	//                        and return S_FALSE.
	//
	//  hdc                 - optional HDC to draw into (DPI scaling)
	//  iPartId             - part number to draw
	//  iStateId            - state number (of the part) 
	//  pRect               - the RECT used to draw the part
	//  pRegion             - receives handle to calculated region
	//-------------------------------------------------------------------------
	HRESULT GetThemeBackgroundRegion(
		OPTIONAL HDC hdc,  
		int iPartId, 
		int iStateId, 
		const RECT * pRect, 
		OUT HRGN * pRegion
		);

	//-------------------------------------------------------------------------
	//  HitTestThemeBackground()
	//                      - returns a HitTestCode (a subset of the values 
	//                        returned by WM_NCHITTEST) for the point "ptTest" 
	//                        within the theme-specified background
	//                        (bound by pRect).  "pRect" and "ptTest" should 
	//                        both be in the same coordinate system 
	//                        (client, screen, etc).
	//
	//      hdc             - HDC to draw into
	//      iPartId         - part number to test against
	//      iStateId        - state number (of the part) 
	//      pRect           - the RECT used to draw the part
	//      hrgn            - optional region to use; must be in same coordinates as
	//                      -    pRect and pTest.
	//      ptTest          - the hit point to be tested
	//      dwOptions       - HTTB_xxx constants
	//      pwHitTestCode   - receives the returned hit test code - one of:
	//                        HTNOWHERE, HTLEFT, HTTOPLEFT, HTBOTTOMLEFT,
	//                        HTRIGHT, HTTOPRIGHT, HTBOTTOMRIGHT,
	//                        HTTOP, HTBOTTOM, HTCLIENT
	//-------------------------------------------------------------------------
	HRESULT HitTestThemeBackground(
		OPTIONAL HDC hdc, 
		int iPartId, 
		int iStateId, 
		DWORD dwOptions, 
		const RECT * pRect, 
		OPTIONAL HRGN hrgn, 
		POINT ptTest, 
		OUT WORD * pwHitTestCode
		);

	//------------------------------------------------------------------------
	//  DrawThemeEdge()     - Similar to the DrawEdge() API, but uses part colors
	//                        and is high-DPI aware
	//  hdc                 - HDC to draw into
	//  iPartId             - part number to draw
	//  iStateId            - state number of part
	//  pDestRect           - the RECT used to draw the line(s)
	//  uEdge               - Same as DrawEdge() API
	//  uFlags              - Same as DrawEdge() API
	//  pContentRect        - Receives the interior rect if (uFlags & BF_ADJUST)
	//------------------------------------------------------------------------
	HRESULT DrawThemeEdge(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		const RECT * pDestRect, 
		UINT uEdge, UINT uFlags, 
		OPTIONAL OUT RECT * pContentRect
		);

	//------------------------------------------------------------------------
	//  DrawThemeIcon()     - draws an image within an imagelist based on 
	//                        a (possible) theme-defined effect. 
	//
	//  hdc                 - HDC to draw into
	//  iPartId             - part number to draw
	//  iStateId            - state number of part
	//  pRect               - the RECT to draw the image within
	//  himl                - handle to IMAGELIST 
	//  iImageIndex         - index into IMAGELIST (which icon to draw)
	//------------------------------------------------------------------------
	HRESULT DrawThemeIcon(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		const RECT * pRect, 
		HIMAGELIST himl, 
		int iImageIndex
		);

	//---------------------------------------------------------------------------
	//  IsThemePartDefined() - returns TRUE if the theme has defined parameters
	//                         for the specified "iPartId" and "iStateId".
	//
	//  iPartId             - part number to find definition for
	//  iStateId            - state number of part
	//---------------------------------------------------------------------------
	BOOL IsThemePartDefined(
		int iPartId, 
		int iStateId
		);

	//---------------------------------------------------------------------------
	//  IsThemeBackgroundPartiallyTransparent()
	//                      - returns TRUE if the theme specified background for 
	//                        the part/state has transparent pieces or 
	//                        alpha-blended pieces.
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//---------------------------------------------------------------------------
	BOOL IsThemeBackgroundPartiallyTransparent(
		int iPartId, 
		int iStateId
		);

	//---------------------------------------------------------------------------
	//    lower-level theme information services
	//---------------------------------------------------------------------------
	// The following methods are getter routines for each of the Theme Data types.
	// Controls/Windows are defined in drawable "parts" by their author: a 
	// parent part and 0 or more child parts.  Each of the parts can be 
	// described in "states" (ex: disabled, hot, pressed).  
	//---------------------------------------------------------------------------
	// Each of the below methods takes a "iPartId" param to specify the 
	// part and a "iStateId" to specify the state of the part.  
	// "iStateId=0" refers to the root part.  "iPartId" = "0" refers to 
	// the root class.  
	//-----------------------------------------------------------------------
	// Each method also take a "iPropId" param because multiple instances of 
	// the same primitive type can be defined in the theme schema.
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	//  GetThemeColor()     - Get the value for the specified COLOR property
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  pColor              - receives the value of the property
	//-----------------------------------------------------------------------
	HRESULT GetThemeColor(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT COLORREF * pColor
		);

	//-----------------------------------------------------------------------
	//  GetThemeMetric()    - Get the value for the specified metric/size
	//                        property
	//
	//  hdc                 - (optional) hdc to be drawn into (DPI scaling)
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  piVal               - receives the value of the property
	//-----------------------------------------------------------------------
	HRESULT GetThemeMetric(
		OPTIONAL HDC hdc, 
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT int * piVal
		);

	//-----------------------------------------------------------------------
	//  GetThemeString()    - Get the value for the specified string property
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  pszBuff             - receives the string property value
	//  cchMaxBuffChars     - max. number of chars allowed in pszBuff
	//-----------------------------------------------------------------------
	HRESULT GetThemeString(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT __EXT_UX_LPWSTR pszBuff, 
		int cchMaxBuffChars
		);

	//-----------------------------------------------------------------------
	//  GetThemeBool()      - Get the value for the specified BOOL property
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  pfVal               - receives the value of the property
	//-----------------------------------------------------------------------
	HRESULT GetThemeBool(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT BOOL * pfVal
		);

	//-----------------------------------------------------------------------
	//  GetThemeInt()       - Get the value for the specified int property
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  piVal               - receives the value of the property
	//-----------------------------------------------------------------------
	HRESULT GetThemeInt(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT int * piVal
		);

	//-----------------------------------------------------------------------
	//  GetThemeEnumValue() - Get the value for the specified ENUM property
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  piVal               - receives the value of the enum (cast to int*)
	//-----------------------------------------------------------------------
	HRESULT GetThemeEnumValue(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT int * piVal
		);

	//-----------------------------------------------------------------------
	//  GetThemePosition()  - Get the value for the specified position
	//                        property
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  pPoint              - receives the value of the position property
	//-----------------------------------------------------------------------
	HRESULT GetThemePosition(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT POINT * pPoint
		);

	//-----------------------------------------------------------------------
	//  GetThemeFont()      - Get the value for the specified font property
	//
	//  hdc                 - (optional) hdc to be drawn to (DPI scaling)
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  pFont               - receives the value of the LOGFONT property
	//                        (scaled for the current logical screen dpi) 
	//-----------------------------------------------------------------------
	HRESULT GetThemeFont(
		OPTIONAL HDC hdc, 
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT LOGFONT * pFont
		);

	//-----------------------------------------------------------------------
	//  GetThemeRect()      - Get the value for the specified RECT property
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to get the value for
	//  pRect               - receives the value of the RECT property
	//-----------------------------------------------------------------------
	HRESULT GetThemeRect(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT RECT * pRect
		);

	//-----------------------------------------------------------------------
	//  GetThemeMargins()   - Get the value for the specified __EXT_UX_MARGINS property
	//
	//      hdc             - (optional) hdc to be used for drawing
	//      iPartId         - part number 
	//      iStateId        - state number of part
	//      iPropId         - the property number to get the value for
	//      prc             - RECT for area to be drawn into
	//      pMargins        - receives the value of the __EXT_UX_MARGINS property
	//-----------------------------------------------------------------------
	HRESULT GetThemeMargins(
		OPTIONAL HDC hdc, 
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OPTIONAL RECT * prc, 
		OUT __EXT_UX_MARGINS * pMargins
		);

	//-----------------------------------------------------------------------
	//  GetThemeIntList()   - Get the value for the specified __EXT_UX_INTLIST struct
	//
	//      iPartId         - part number 
	//      iStateId        - state number of part
	//      iPropId         - the property number to get the value for
	//      pIntList        - receives the value of the __EXT_UX_INTLIST property
	//-----------------------------------------------------------------------
	HRESULT GetThemeIntList(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT __EXT_UX_INTLIST * pIntList
		);

	//-----------------------------------------------------------------------
	//  GetThemePropertyOrigin()
	//                      - searches for the specified theme property
	//                        and sets "pOrigin" to indicate where it was 
	//                        found (or not found)
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to search for
	//  pOrigin             - receives the value of the property origin
	//-----------------------------------------------------------------------
	HRESULT GetThemePropertyOrigin(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT enum __EXT_UX_PROPERTYORIGIN * pOrigin
		);

	//---------------------------------------------------------------------------
	//  SetWindowTheme()
	//                      - redirects an existing Window to use a different 
	//                        section of the current theme information than its 
	//                        class normally asks for.
	//
	//  hwnd                - the handle of the window (cannot be NULL)
	//
	//  pszSubAppName       - app (group) name to use in place of the calling
	//                        app's name.  If NULL, the actual calling app 
	//                        name will be used.
	//
	//  pszSubIdList        - semicolon separated list of class Id names to 
	//                        use in place of actual list passed by the 
	//                        window's class.  if NULL, the id list from the 
	//                        calling class is used.
	//---------------------------------------------------------------------------
	// The Theme Manager will remember the "pszSubAppName" and the
	// "pszSubIdList" associations thru the lifetime of the window (even 
	// if themes are subsequently changed).  The window is sent a 
	// "WM_THEMECHANGED" msg at the end of this call, so that the new
	// theme can be found and applied.
	//---------------------------------------------------------------------------
	// When "pszSubAppName" or "pszSubIdList" are NULL, the Theme Manager 
	// removes the previously remember association.  To turn off theme-ing for 
	// the specified window, you can pass an empty string (L"") so it 
	// won't match any section entries.
	//---------------------------------------------------------------------------
	HRESULT SetWindowTheme(
		HWND hwnd, 
		__EXT_UX_LPCWSTR pszSubAppName, 
		__EXT_UX_LPCWSTR pszSubIdList
		);

	//---------------------------------------------------------------------------
	//  GetThemeFilename()  - Get the value for the specified FILENAME property.
	//
	//  iPartId             - part number 
	//  iStateId            - state number of part
	//  iPropId             - the property number to search for
	//  pszThemeFileName    - output buffer to receive the filename
	//  cchMaxBuffChars     - the size of the return buffer, in chars
	//---------------------------------------------------------------------------
	HRESULT GetThemeFilename(
		int iPartId, 
		int iStateId, 
		int iPropId, 
		OUT __EXT_UX_LPWSTR pszThemeFileName, 
		int cchMaxBuffChars
		);

	//---------------------------------------------------------------------------
	//  GetThemeSysColor()  - Get the value of the specified System color. 
	//
	//  bThemeSysColor      - if TRUE, will return color from [SysMetrics] 
	//						  section of theme.
	//                        if FALSE, will return the global system color.
	//  iColorId            - the system color index defined in winuser.h
	//---------------------------------------------------------------------------
	COLORREF GetThemeSysColor( 
		BOOL bThemeSysColor,
		int iColorId 
		);

	//---------------------------------------------------------------------------
	//  GetThemeSysColorBrush() 
	//                      - Get the brush for the specified System color. 
	//
	//  bThemeSysColor      - if TRUE, will return
	//                        brush matching color from [SysMetrics] section of 
	//                        theme.  if FALSE, will return the brush matching
	//                        global system color.
	//  iColorId            - the system color index defined in winuser.h
	//---------------------------------------------------------------------------
	HBRUSH GetThemeSysColorBrush( 
		BOOL bThemeSysColor, 
		int iColorId 
		);

	//---------------------------------------------------------------------------
	//  GetThemeSysBool()   - Get the boolean value of specified System metric. 
	//
	//  bThemeSysColor      - if TRUE, will return
	//                        BOOL from [SysMetrics] section of theme.
	//                        if FALSE, will return the specified system boolean.
	//  iBoolId             - the TMT_XXX BOOL number (first BOOL
	//                        is TMT_FLATMENUS)
	//---------------------------------------------------------------------------
	BOOL GetThemeSysBool(
		BOOL bThemeSysColor,
		int iBoolId
		);

	//---------------------------------------------------------------------------
	//  GetThemeSysSize()   - Get the value of the specified System size metric. 
	//                        (scaled for the current logical screen dpi) 
	//
	//  bThemeSysColor      - if TRUE, will return
	//                        size from [SysMetrics] section of theme.
	//                        if FALSE, will return the global system metric.
	//  iSizeId             - the following values are supported when 
	//                        hTheme is non-NULL:
	//                          SM_CXBORDER   (border width)
	//                          SM_CXVSCROLL  (scrollbar width)
	//                          SM_CYHSCROLL  (scrollbar height)
	//                          SM_CXSIZE     (caption width)
	//                          SM_CYSIZE     (caption height)
	//                          SM_CXSMSIZE   (small caption width)
	//                          SM_CYSMSIZE   (small caption height)
	//                          SM_CXMENUSIZE (menubar width)
	//                          SM_CYMENUSIZE (menubar height)
	//                        when hTheme is NULL, iSizeId is passed directly
	//                        to the GetSystemMetrics() function
	//---------------------------------------------------------------------------
	int GetThemeSysSize(
		BOOL bThemeSysColor,
		int iSizeId
		);

	//---------------------------------------------------------------------------
	//  GetThemeSysFont()   - Get the LOGFONT for the specified System font. 
	//
	//  bThemeSysColor      - if TRUE, will return
	//                        font from [SysMetrics] section of theme.
	//                        if FALSE, will return the specified system font.
	//  iFontId             - the TMT_XXX font number (first font
	//                        is TMT_CAPTIONFONT)
	//  plf                 - ptr to LOGFONT to receive the font value.
	//                        (scaled for the current logical screen dpi) 
	//---------------------------------------------------------------------------
	HRESULT GetThemeSysFont(
		BOOL bThemeSysColor,
		int iFontId, 
		OUT LOGFONT * plf
		);

	//---------------------------------------------------------------------------
	//  GetThemeSysString() - Get the value of specified System string metric. 
	//
	//  iStringId           - must be one of the following values:
	//                          TMT_CSSNAME
	//                          TMT_XMLNAME
	//  pszStringBuff       - the buffer to receive the string value
	//  cchMaxStringChars   - max. number of chars that pszStringBuff can hold
	//---------------------------------------------------------------------------
	HRESULT GetThemeSysString(
		int iStringId, 
		OUT __EXT_UX_LPWSTR pszStringBuff, 
		int cchMaxStringChars
		);

	//---------------------------------------------------------------------------
	//  GetThemeSysInt() - Get the value of specified System int.
	//
	//  iIntId              - must be one of the following values:
	//                          TMT_DPIX
	//                          TMT_DPIY
	//                          TMT_MINCOLORDEPTH
	//  piValue             - ptr to int to receive value
	//---------------------------------------------------------------------------
	HRESULT GetThemeSysInt(
		int iIntId, 
		int * piValue
		);

	//---------------------------------------------------------------------------
	//  IsThemeActive()     - can be used to test if a system theme is active
	//                        for the current user session.  
	//
	//                        use the API "IsAppThemed()" to test if a theme is
	//                        active for the calling process.
	//---------------------------------------------------------------------------
	BOOL IsThemeActive();

	//---------------------------------------------------------------------------
	//  IsAppThemed()       - returns TRUE if a theme is active and available to
	//                        the current process
	//---------------------------------------------------------------------------
	BOOL IsAppThemed();


	//---------------------------------------------------------------------------
	//  GetWindowTheme()    - if window is themed, returns its most recent
	//                        __EXT_UX_HTHEME from OpenThemeData() - otherwise, returns
	//                        NULL.
	//
	//      hwnd            - the window to get the __EXT_UX_HTHEME of
	//---------------------------------------------------------------------------
	__EXT_UX_HTHEME GetWindowTheme( HWND hwnd );


	//---------------------------------------------------------------------------
	//  EnableThemeDialogTexture() 
	//
	//  - Enables/disables dialog background theme.  This method can be used to 
	//    tailor dialog compatibility with child windows and controls that 
	//    may or may not coordinate the rendering of their client area backgrounds 
	//    with that of their parent dialog in a manner that supports seamless 
	//    background texturing.
	//
	//      hwnd         - the window handle of the target dialog
	//      dwFlags      - ETDT_ENABLE to enable the theme-defined dialog background texturing,
	//                     ETDT_DISABLE to disable background texturing,
	//                     ETDT_ENABLETAB to enable the theme-defined background 
	//                          texturing using the Tab texture
	//---------------------------------------------------------------------------
	HRESULT EnableThemeDialogTexture(
		HWND hwnd, 
		DWORD dwFlags
		);

	//---------------------------------------------------------------------------
	//  IsThemeDialogTextureEnabled() 
	//
	//  - Reports whether the dialog supports background texturing.
	//
	//      hdlg         - the window handle of the target dialog
	//---------------------------------------------------------------------------
	BOOL IsThemeDialogTextureEnabled( HWND hwnd );

	//---------------------------------------------------------------------------
	//  GetThemeAppProperties()
	//                      - returns the app property flags that control theming
	//---------------------------------------------------------------------------
	DWORD GetThemeAppProperties();

	//---------------------------------------------------------------------------
	//  SetThemeAppProperties()
	//                      - sets the flags that control theming within the app
	//
	//      dwFlags         - the flag values to be set
	//---------------------------------------------------------------------------
	void SetThemeAppProperties( DWORD dwFlags );

	//---------------------------------------------------------------------------
	//  GetCurrentThemeName()
	//                      - Get the name of the current theme in-use.  
	//                        Optionally, return the ColorScheme name and the 
	//                        Size name of the theme.
	//
	//  pszThemeFileName    - receives the theme path & filename
	//  cchMaxNameChars     - max chars allowed in pszNameBuff
	//  pszColorBuff        - (optional) receives the canonical color scheme name
	//  cchMaxColorChars    - max chars allowed in pszColorBuff
	//  pszSizeBuff         - (optional) receives the canonical size name 
	//  cchMaxSizeChars     - max chars allowed in pszSizeBuff
	//---------------------------------------------------------------------------
	HRESULT GetCurrentThemeName(
		OUT __EXT_UX_LPWSTR pszThemeFileName, 
		int cchMaxNameChars, 
		OUT OPTIONAL __EXT_UX_LPWSTR pszColorBuff, 
		int cchMaxColorChars,
		OUT OPTIONAL __EXT_UX_LPWSTR pszSizeBuff, 
		int cchMaxSizeChars
		);

	//---------------------------------------------------------------------------
	//  GetThemeDocumentationProperty()
	//                      - Get the value for the specified property name from 
	//                        the [documentation] section of the themes.ini file 
	//                        for the specified theme.  If the property has been 
	//                        localized in the theme files string table, the 
	//                        localized version of the property value is returned. 
	//
	//  pszThemeFileName    - filename of the theme file to query
	//  pszPropertyName     - name of the string property to retreive a value for
	//  pszValueBuff        - receives the property string value
	//  cchMaxValChars      - max chars allowed in pszValueBuff
	//---------------------------------------------------------------------------
	HRESULT GetThemeDocumentationProperty(
		__EXT_UX_LPCWSTR pszThemeName,
		__EXT_UX_LPCWSTR pszPropertyName, 
		OUT __EXT_UX_LPWSTR pszValueBuff, 
		int cchMaxValChars
		);

	//---------------------------------------------------------------------------
	// DrawThemeParentBackground()
	//                      - used by partially-transparent or alpha-blended
	//                        child controls to draw the part of their parent
	//                        that they appear in front of.
	//
	//  hwnd                - handle of the child control
	//  hdc                 - hdc of the child control
	//  prc                 - (optional) rect that defines the area to be 
	//                        drawn (CHILD coordinates)
	//---------------------------------------------------------------------------
	HRESULT DrawThemeParentBackground(
		HWND hwnd, 
		HDC hdc, 
		OPTIONAL RECT * prc
		);

	//---------------------------------------------------------------------------
	//  EnableTheming()     - enables or disables themeing for the current user
	//                        in the current and future sessions.
	//
	//  fEnable             - if FALSE, disable theming & turn themes off.
	//                      - if TRUE, enable themeing and, if user previously
	//                        had a theme active, make it active now.
	//---------------------------------------------------------------------------
	HRESULT EnableTheming( BOOL fEnable );

	//------------------------------------------------------------------------
	//  DrawThemeBackgroundEx()   
	//                      - draws the theme-specified border and fill for 
	//                        the "iPartId" and "iStateId".  This could be 
	//                        based on a bitmap file, a border and fill, or 
	//                        other image description.  NOTE: This will be
	//                        merged back into DrawThemeBackground() after 
	//                        BETA 2.
	//
	//  hdc                 - HDC to draw into
	//  iPartId             - part number to draw
	//  iStateId            - state number (of the part) to draw
	//  pRect               - defines the size/location of the part
	//  pOptions            - ptr to optional params
	//------------------------------------------------------------------------
	HRESULT DrawThemeBackgroundEx(
		HDC hdc, 
		int iPartId, 
		int iStateId, 
		const RECT * pRect, 
		OPTIONAL const __EXT_UX_DTBGOPTS * pOptions
		);

protected:
	__EXT_UX_HTHEME m_hUxTheme;
	HINSTANCE m_hUxThemeDll;
	bool m_bNeedToFreeDll:1;
public:
	DWORD m_dwComCtlVersion;
protected:
	
	WCHAR m_szClassListCached[ MAX_PATH ];
	HWND m_hWndCached;
	HRESULT _CloseThemeData( __EXT_UX_HTHEME hUxTheme );

	__EXT_UX_THEMEAPI_( __EXT_UX_HTHEME, m_pfnOpenThemeData )( HWND hwnd, __EXT_UX_LPCWSTR pszClassList );
	__EXT_UX_THEMEAPI_( __EXT_UX_HTHEME, m_pfnOpenThemeDataEx )( HWND hwnd, __EXT_UX_LPCWSTR pszClassList, DWORD dwFlags );
	__EXT_UX_THEMEAPI(  m_pfnCloseThemeData )( __EXT_UX_HTHEME hTheme );
	__EXT_UX_THEMEAPI(  m_pfnDrawThemeBackground )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect);
	__EXT_UX_THEMEAPI(  m_pfnDrawThemeText )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, __EXT_UX_LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect);
	__EXT_UX_THEMEAPI(  m_pfnDrawThemeTextEx )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, __EXT_UX_LPCWSTR pszText, int iCharCount, DWORD dwFlags, LPRECT pRect, const __EXT_UX_DTTOPTS * pOptions );
	__EXT_UX_THEMEAPI(  m_pfnGetThemeBackgroundContentRect )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId,  const RECT *pBoundingRect, RECT *pContentRect );
	__EXT_UX_THEMEAPI(  m_pfnGetThemeBackgroundExtent )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pContentRect, RECT *pExtentRect );
	__EXT_UX_THEMEAPI(  m_pfnGetThemePartSize )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, RECT *prc, enum __EXT_UX_THEMESIZE eSize, SIZE *psz );
	__EXT_UX_THEMEAPI(  m_pfnGetThemeTextExtent )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, __EXT_UX_LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, const RECT *pBoundingRect, RECT *pExtentRect );
	__EXT_UX_THEMEAPI(  m_pfnGetThemeTextMetrics )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, TEXTMETRIC* ptm );
	__EXT_UX_THEMEAPI(  m_pfnGetThemeBackgroundRegion )( __EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, HRGN *pRegion );
	__EXT_UX_THEMEAPI(  m_pfnHitTestThemeBackground)(__EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, DWORD dwOptions, const RECT *pRect, HRGN hrgn, POINT ptTest, WORD *pwHitTestCode);
	__EXT_UX_THEMEAPI(  m_pfnDrawThemeEdge)(__EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pDestRect, UINT uEdge, UINT uFlags, RECT *pContentRect);
	__EXT_UX_THEMEAPI(  m_pfnDrawThemeIcon)(__EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, HIMAGELIST himl, int iImageIndex);
	__EXT_UX_THEMEAPI_( BOOL, m_pfnIsThemePartDefined)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId);
	__EXT_UX_THEMEAPI_( BOOL, m_pfnIsThemeBackgroundPartiallyTransparent)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeColor)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeMetric)(__EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int *piVal);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeString)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId, __EXT_UX_LPWSTR pszBuff, int cchMaxBuffChars);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeBool)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId, BOOL *pfVal);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeInt)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId, int *piVal);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeEnumValue)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId,int *piVal);
	__EXT_UX_THEMEAPI(  m_pfnGetThemePosition)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId,POINT *pPoint);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeFont)(__EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId,LOGFONT *pFont);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeRect)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId,RECT *pRect);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeMargins)(__EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, RECT *prc,__EXT_UX_MARGINS *pMargins);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeIntList)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId,__EXT_UX_INTLIST *pIntList);
	__EXT_UX_THEMEAPI(  m_pfnGetThemePropertyOrigin)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId,enum __EXT_UX_PROPERTYORIGIN *pOrigin);
	__EXT_UX_THEMEAPI(  m_pfnSetWindowTheme)(HWND hwnd, __EXT_UX_LPCWSTR pszSubAppName, __EXT_UX_LPCWSTR pszSubIdList);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeFilename)(__EXT_UX_HTHEME hTheme, int iPartId, int iStateId, int iPropId,__EXT_UX_LPWSTR pszThemeFileName, int cchMaxBuffChars);
	__EXT_UX_THEMEAPI_( COLORREF, m_pfnGetThemeSysColor)(__EXT_UX_HTHEME hTheme, int iColorId);
	__EXT_UX_THEMEAPI_( HBRUSH, m_pfnGetThemeSysColorBrush)(__EXT_UX_HTHEME hTheme, int iColorId);
	__EXT_UX_THEMEAPI_( BOOL, m_pfnGetThemeSysBool)(__EXT_UX_HTHEME hTheme, int iBoolId);
	__EXT_UX_THEMEAPI_( int, m_pfnGetThemeSysSize)(__EXT_UX_HTHEME hTheme, int iSizeId);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeSysFont)(__EXT_UX_HTHEME hTheme, int iFontId,LOGFONT *plf);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeSysString)(__EXT_UX_HTHEME hTheme, int iStringId, __EXT_UX_LPWSTR pszStringBuff, int cchMaxStringChars);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeSysInt)(__EXT_UX_HTHEME hTheme, int iIntId, int *piValue);
	__EXT_UX_THEMEAPI_( BOOL, m_pfnIsThemeActive)();
	__EXT_UX_THEMEAPI_( BOOL, m_pfnIsAppThemed)();
	__EXT_UX_THEMEAPI_( __EXT_UX_HTHEME, m_pfnGetWindowTheme)(HWND hwnd);
	__EXT_UX_THEMEAPI(  m_pfnEnableThemeDialogTexture)(HWND hwnd, DWORD dwFlags);
	__EXT_UX_THEMEAPI_( BOOL, m_pfnIsThemeDialogTextureEnabled)(HWND hwnd);
	__EXT_UX_THEMEAPI_( DWORD, m_pfnGetThemeAppProperties)();
	__EXT_UX_THEMEAPI_( void, m_pfnSetThemeAppProperties)(DWORD dwFlags);
	__EXT_UX_THEMEAPI(  m_pfnGetCurrentThemeName)(__EXT_UX_LPWSTR pszThemeFileName, int cchMaxNameChars, __EXT_UX_LPWSTR pszColorBuff, int cchMaxColorChars, __EXT_UX_LPWSTR pszSizeBuff, int cchMaxSizeChars);
	__EXT_UX_THEMEAPI(  m_pfnGetThemeDocumentationProperty)(__EXT_UX_LPCWSTR pszThemeName, __EXT_UX_LPCWSTR pszPropertyName,__EXT_UX_LPWSTR pszValueBuff, int cchMaxValChars);
	__EXT_UX_THEMEAPI(  m_pfnDrawThemeParentBackground)(HWND hwnd, HDC hdc, RECT* prc);
	__EXT_UX_THEMEAPI(  m_pfnEnableTheming)(BOOL fEnable);
	__EXT_UX_THEMEAPI(  m_pfnDrawThemeBackgroundEx)(__EXT_UX_HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, const __EXT_UX_DTBGOPTS *pOptions);

}; // CExtUxTheme

//////////////////////////////////////////////////////////////////////////
// DWMAPI defines
//////////////////////////////////////////////////////////////////////////

// Define API decoration for direct importing of DLL references.
#define __EXT_DWMAPI(_name_)			HRESULT ( STDAPICALLTYPE * _name_ )
#define __EXT_DWMAPI_(_type_,_name_)	_type_ ( STDAPICALLTYPE * _name_ )

#define __EXT_DWM_COMPOSED_EVENT_NAME_MAX_LENGTH	64
#define __EXT_DWM_COMPOSED_EVENT_BASE_NAME			L"DwmComposedEvent_"
#define __EXT_DWM_COMPOSED_EVENT_NAME_FORMAT		L"%s%d"

// Blur behind data structures
#define __EXT_DWM_BB_ENABLE					0x00000001  // fEnable has been specified
#define __EXT_DWM_BB_BLURREGION				0x00000002  // hRgnBlur has been specified
#define __EXT_DWM_BB_TRANSITIONONMAXIMIZED	0x00000004  // fTransitionOnMaximized has been specified

// Thumbnails
typedef HANDLE __EXT_HTHUMBNAIL;
typedef __EXT_HTHUMBNAIL * __EXT_PHTHUMBNAIL;

#define __EXT_DWM_TNP_RECTDESTINATION       0x00000001
#define __EXT_DWM_TNP_RECTSOURCE            0x00000002
#define __EXT_DWM_TNP_OPACITY               0x00000004
#define __EXT_DWM_TNP_VISIBLE               0x00000008
#define __EXT_DWM_TNP_SOURCECLIENTAREAONLY  0x00000010

// Video enabling api
typedef ULONGLONG __EXT_DWM_FRAME_COUNT;
typedef ULONGLONG __EXT_QPC_TIME;

#define __EXT_DWM_FRAME_DURATION_DEFAULT	-1

//////////////////////////////////////////////////////////////////////////
// CExtDWM class

#include <pshpack1.h>

class __PROF_UIS_API CExtDWM
{
public:
	typedef struct __PROF_UIS_API __EXT__DWM_BLURBEHIND
	{
		DWORD dwFlags;
		BOOL fEnable;
		HRGN hRgnBlur;
		BOOL fTransitionOnMaximized;
	} __EXT_DWM_BLURBEHIND, *__EXT_PDWM_BLURBEHIND;
	
	// Window attributes
	typedef enum __EXT_DWMWINDOWATTRIBUTE
	{
		__EXT_DWMWA_NCRENDERING_ENABLED = 1,   // [get] Is non-client rendering enabled/disabled
		__EXT_DWMWA_NCRENDERING_POLICY,        // [set] Non-client rendering policy
		__EXT_DWMWA_TRANSITIONS_FORCEDISABLED, // [set] Potentially enable/forcibly disable transitions
		__EXT_DWMWA_ALLOW_NCPAINT,             // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
		__EXT_DWMWA_CAPTION_BUTTON_BOUNDS,
		__EXT_DWMWA_NONCLIENT_RTL_LAYOUT,
		__EXT_DWMWA_FORCE_ICONIC_REPRESENTATION,
		__EXT_DWMWA_FLIP3D_POLICY,
		__EXT_DWMWA_LAST
	};
	
	// Non-client rendering policy attribute values
	typedef enum __EXT_DWMNCRENDERINGPOLICY
	{
		__EXT_DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
		__EXT_DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
		__EXT_DWMNCRP_ENABLED         // Enabled non-client rendering; window style is ignored
	};

	// Thumbnails
	typedef struct __PROF_UIS_API __EXT__DWM_THUMBNAIL_PROPERTIES
	{
		DWORD dwFlags;
		RECT rcDestination;
		RECT rcSource;
		BYTE opacity;
		BOOL fVisible;
		BOOL fSourceClientAreaOnly;
	} __EXT_DWM_THUMBNAIL_PROPERTIES, *__EXT_PDWM_THUMBNAIL_PROPERTIES;

	// Video enabling api
	typedef struct __PROF_UIS_API __EXT__UNSIGNED_RATIO
	{
		UINT32 uiNumerator;
		UINT32 uiDenominator;
	} __EXT_UNSIGNED_RATIO;

	typedef struct __EXT__DWM_TIMING_INFO
	{
		UINT32					cbSize;
		__EXT_UNSIGNED_RATIO	rateRefresh;        // Monitor refresh rate
		__EXT_UNSIGNED_RATIO	rateCompose;        // composition rate     
		__EXT_QPC_TIME			qpcVBlank;          // QPC time at VBlank
		__EXT_DWM_FRAME_COUNT	cRefresh;           // DWM refresh counter
		__EXT_QPC_TIME			qpcCompose;         // QPC time at a compose time
		__EXT_DWM_FRAME_COUNT	cFrame;             // Frame number that was composed at qpcCompose
		__EXT_DWM_FRAME_COUNT	cRefreshFrame;      // Refresh count of the frame that was composed at qpcCompose
		__EXT_DWM_FRAME_COUNT	cRefreshConfirmed;  // The target refresh count of the last frame confirmed completed by the GPU
		UINT32					cFlipsOutstanding;  // the number of outstanding flips
		
		// Feedback on previous performance only valid on 2nd and subsequent calls
		__EXT_DWM_FRAME_COUNT	cFrameCurrent;		// Last frame displayed
		__EXT_DWM_FRAME_COUNT	cFramesAvailable;	// number of frames available but not displayed, used or dropped
		__EXT_DWM_FRAME_COUNT	cFrameCleared;		// Source frame number when the following statistics where last cleared
		__EXT_DWM_FRAME_COUNT	cFramesReceived;	// number of new frames received
		__EXT_DWM_FRAME_COUNT	cFramesDisplayed;	// number of unique frames displayed
		__EXT_DWM_FRAME_COUNT	cFramesDropped;		// number of rendered frames that were never displayed 
													// because composition occurred too late
		__EXT_DWM_FRAME_COUNT	cFramesMissed;		// number of times an old frame was composed 
													// when a new frame should have been used but was not available
	} __EXT_DWM_TIMING_INFO;

	typedef enum 
	{
		__EXT_DWM_SOURCE_FRAME_SAMPLING_POINT,		// Use the first source frame that 
													// includes the first refresh of the output frame
		__EXT_DWM_SOURCE_FRAME_SAMPLING_COVERAGE,	// use the source frame that includes the most 
													// refreshes of out the output frame
													// in case of multiple source frames with the 
													// same coverage the last will be used
		__EXT_DWM_SOURCE_FRAME_SAMPLING_LAST		// Sentinel value
	} __EXT_DWM_SOURCE_FRAME_SAMPLING;

	const UINT m_nDwmMaxQueuedBuffers;
	const UINT m_nDwmMaxMonitors;
	const UINT m_nDwmMaxAdapters;

	#pragma warning(push)
	#pragma warning(disable:4201)
	typedef struct __EXT__DWM_PRESENT_PARAMETERS
	{
		UINT32 cbSize;
		BOOL fQueue;
		__EXT_DWM_FRAME_COUNT cRefreshStart;
		UINT cBuffer;
		BOOL fUseSourceRate;
		union
		{
			__EXT_UNSIGNED_RATIO rateSource;
			struct 
			{
				UINT cRefreshesPerFrame;
				__EXT_DWM_SOURCE_FRAME_SAMPLING eSampling;
			};
		};
	} __EXT_DWM_PRESENT_PARAMETERS;
	#pragma warning(pop)

	CExtDWM();
	virtual ~CExtDWM();

	BOOL IsCompositionEnabled();

	BOOL DwmDefWindowProc(
		IN HWND hWnd,
		IN UINT msg,
		IN WPARAM wParam,
		IN LPARAM lParam,
		OUT LRESULT * plResult
		);
	
	HRESULT DwmEnableBlurBehindWindow(
		HWND hWnd, 
		const __EXT_DWM_BLURBEHIND * pBlurBehind
		);
	
	HRESULT DwmEnableComposition(
		BOOL fEnable
		);
	
	HRESULT DwmEnableMMCSS(
		BOOL fEnableMMCSS
		);
	
	HRESULT DwmExtendFrameIntoClientArea(
		HWND hWnd,
		const CExtUxTheme::__EXT_UX__MARGINS * pMarInset
		);
    
	HRESULT DwmGetColorizationColor(
		DWORD * pcrColorization,
		BOOL * pfOpaqueBlend
		);
	
	HRESULT DwmGetCompositionTimingInfo(
		HWND hwnd,
		__EXT_DWM_TIMING_INFO * pTimingInfo
		);
	
	HRESULT DwmGetWindowAttribute(
		HWND hwnd,
		DWORD dwAttribute,
		PVOID pvAttribute, 
		DWORD cbAttribute
		);
	
	HRESULT DwmIsCompositionEnabled(
		BOOL * pfEnabled
		);
	
	HRESULT DwmModifyPreviousDxFrameDuration(
		HWND hwnd, 
		INT cRefreshes,
		BOOL fRelative
		);
	
	HRESULT DwmQueryThumbnailSourceSize(
		__EXT_HTHUMBNAIL hThumbnail, 
		OUT PSIZE pSize
		);
	
	HRESULT DwmRegisterThumbnail(
		HWND hwndDestination, 
		HWND hwndSource, 
		const SIZE * pMinimizedSize,
		__EXT_PHTHUMBNAIL phThumbnailId
		);

	HRESULT DwmSetDxFrameDuration(
		HWND hwnd, 
		INT cRefreshes
		);
	
	HRESULT DwmSetPresentParameters(
		HWND hwnd,
		__EXT_DWM_PRESENT_PARAMETERS * pPresentParams
		);
	
	HRESULT DwmSetWindowAttribute(
		HWND hwnd,
		DWORD dwAttribute,
		LPCVOID pvAttribute, 
		DWORD cbAttribute
		);
	
	HRESULT DwmUnregisterThumbnail(
		__EXT_HTHUMBNAIL hThumbnailId
		);
	
	HRESULT DwmUpdateThumbnailProperties(
		__EXT_HTHUMBNAIL hThumbnailId, 
		const __EXT_DWM_THUMBNAIL_PROPERTIES * ptnProperties
		);

protected:
	HINSTANCE m_hDWMDll;
	bool m_bNeedToFreeDll:1;

	__EXT_DWMAPI_(BOOL, m_pfnDwmDefWindowProc)( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT * plResult );
	__EXT_DWMAPI(m_pfnDwmEnableBlurBehindWindow)( HWND hWnd, const __EXT_DWM_BLURBEHIND * pBlurBehind );
	__EXT_DWMAPI(m_pfnDwmEnableComposition)( BOOL fEnable );
	__EXT_DWMAPI(m_pfnDwmEnableMMCSS)( BOOL fEnableMMCSS );
	__EXT_DWMAPI(m_pfnDwmExtendFrameIntoClientArea)( HWND hWnd, const CExtUxTheme::__EXT_UX__MARGINS * pMarInset );
	__EXT_DWMAPI(m_pfnDwmGetColorizationColor)( DWORD * pcrColorization, BOOL * pfOpaqueBlend );
	__EXT_DWMAPI(m_pfnDwmGetCompositionTimingInfo)( HWND hwnd, __EXT_DWM_TIMING_INFO * pTimingInfo );
	__EXT_DWMAPI(m_pfnDwmGetWindowAttribute)( HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute );
	__EXT_DWMAPI(m_pfnDwmIsCompositionEnabled)( BOOL * pfEnabled );
	__EXT_DWMAPI(m_pfnDwmModifyPreviousDxFrameDuration)( HWND hwnd, INT cRefreshes, BOOL fRelative );
	__EXT_DWMAPI(m_pfnDwmQueryThumbnailSourceSize)( __EXT_HTHUMBNAIL hThumbnail, PSIZE pSize );
	__EXT_DWMAPI(m_pfnDwmRegisterThumbnail)( HWND hwndDestination, HWND hwndSource, const SIZE * pMinimizedSize, __EXT_PHTHUMBNAIL phThumbnailId );
	__EXT_DWMAPI(m_pfnDwmSetDxFrameDuration)( HWND hwnd, INT cRefreshes );
	__EXT_DWMAPI(m_pfnDwmSetPresentParameters)( HWND hwnd, __EXT_DWM_PRESENT_PARAMETERS * pPresentParams );
	__EXT_DWMAPI(m_pfnDwmSetWindowAttribute)( HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute );
	__EXT_DWMAPI(m_pfnDwmUnregisterThumbnail)( __EXT_HTHUMBNAIL hThumbnailId );
	__EXT_DWMAPI(m_pfnDwmUpdateThumbnailProperties)( __EXT_HTHUMBNAIL hThumbnailId, const __EXT_DWM_THUMBNAIL_PROPERTIES * ptnProperties );

}; /// CExtDWM

#include <poppack.h>


//////////////////////////////////////////////////////////////////////////
// CExtArray

template < class TYPE > class CExtArray
	: public CArray < TYPE, TYPE & >
{
protected:
	bool m_bSortedStateFlag:1;
	bool m_bSortedAscending:1;
	static int stat_CompareItems(
		const TYPE & element1,
		const TYPE & element2
		)
	{
		if( element1 < element2 )
			return -1;
		if( element2 < element1 )
			return 1;
		return 0;
	}
	static void stat_SwapItems(
		TYPE & element1,
		TYPE & element2
		)
	{
		TYPE temp = element1;
		element1 = element2;
		element2 = temp;
	}
public:
	virtual int OnCompareItems(
		const TYPE & element1,
		const TYPE & element2
		) const
	{
		return stat_CompareItems( element1, element2 );
	}
	virtual void OnSwapItems(
		TYPE & element1,
		TYPE & element2
		)
	{
		stat_SwapItems( element1, element2 );
	}
protected:
	void QuickSortRecursive( INT nLow, INT nHigh, bool bAscending )
	{
 		if( nLow > nHigh )
			return;
		INT nViewMin = nLow, nViewMax = nHigh, nViewMiddle = ( nLow + nHigh ) / 2L;
		for( ; nViewMin <= nViewMax; )
		{
			for( ; ( nViewMin < nHigh ) && bAscending ? ( OnCompareItems( GetAt(nViewMin), GetAt(nViewMiddle) ) < 0 ) : ( OnCompareItems( GetAt(nViewMin), GetAt(nViewMiddle) ) > 0 ); ++ nViewMin );
			for( ; ( nViewMax > nLow ) && bAscending ? ( OnCompareItems( GetAt(nViewMax), GetAt(nViewMiddle) ) > 0 ) : ( OnCompareItems( GetAt(nViewMax), GetAt(nViewMiddle) ) < 0 ); -- nViewMax );
			if( nViewMin <= nViewMax )
			{
				if( nViewMin != nViewMax && OnCompareItems( GetAt(nViewMin), GetAt(nViewMax) ) != 0 )
				{
					if( nViewMiddle == nViewMin )
						nViewMiddle = nViewMax;
					else if( nViewMiddle == nViewMax )
						nViewMiddle = nViewMin;
					OnSwapItems( ElementAt(nViewMin), ElementAt(nViewMax) );
				}
				++ nViewMin;
				-- nViewMax;
			}
		}
		if( nLow < nViewMax )
			QuickSortRecursive( nLow, nViewMax, bAscending );
		if( nViewMin < nHigh )
			QuickSortRecursive( nViewMin, nHigh, bAscending );
	}
public:
	CExtArray(
		bool bSortItems = false,
		bool bAscending = true
		)
		: m_bSortedStateFlag ( bSortItems )
		, m_bSortedAscending( bAscending )
	{
	}
	CExtArray( const CExtArray < TYPE > & other )
		: m_bSortedStateFlag ( false )
		, m_bSortedAscending( true )
	{
		__EXT_DEBUG_RICH_CONTENT_VERIFY( AssignFromOther( other ) );
	}
	void Clean()
	{
		RemoveAll();
		FreeExtra();
	}
	const TYPE & operator [] ( int i_index ) const
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( 0 <= i_index && i_index < GetSize() );
		return ( GetData() [ i_index ] );
	}
	const TYPE & GetAt( int i_index ) const
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( 0 <= i_index && i_index < GetSize() );
		return ( GetData() [i_index] );
	}
	TYPE & operator [] ( int i_index )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( 0 <= i_index && i_index < GetSize() );
		return ( GetData() [i_index] );
	}
	bool IsSorted() const
	{
		return m_bSortedStateFlag;
	}
	bool IsSortedAscending() const
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_bSortedStateFlag );
		return m_bSortedAscending;
	}
	bool QuickSort( bool bAscending = true )
	{
		m_bSortedStateFlag = false;
		m_bSortedAscending = bAscending;
		bool rc = true;
		if( GetSize() == 0 )
		{
			m_bSortedStateFlag = true;
			return true;
		}
		try
		{
			INT	nLow = 0, nHigh = INT( GetSize() - 1 );
			QuickSortRecursive( nLow, nHigh, bAscending );
			m_bSortedStateFlag = true;
		}
		catch( ... )
		{
			::SetLastError( ERROR_INVALID_PARAMETER );
			rc = false;
		}
		return rc;
	}
	int BinaryInsertPosFind( const TYPE & _key ) const
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_bSortedStateFlag );
		int nCount = int( GetSize() );
		if( nCount == 0 )
			return 0;
		if( nCount == 1 )
		{
			const TYPE & _element =
				( ( CExtArray < TYPE > & ) (*this) )
					.ElementAt( 0 );
			if( m_bSortedAscending )
				return ( _key < _element ) ? 0 : 1;
			else
				return ( _key > _element ) ? 0 : 1;
		} // if( nCount == 1 )

		int nIdx = nCount-1, nIdx2, nMargine1 = 0, nMargine2 = nCount-1;
		for( ; true; )
		{
			const TYPE & _element =
				( ( CExtArray < TYPE > & ) (*this) )
					.ElementAt( nIdx );
			int nCompareResult = OnCompareItems( _key, _element );
			if( nCompareResult == 0 )
				return nIdx;
			if( m_bSortedAscending )
			{
				if( nCompareResult > 0 )
					nMargine1 = nIdx;
				else
					nMargine2 = nIdx;
			} // if( m_bSortedAscending )
			else
			{
				if( nCompareResult > 0 )
					nMargine2 = nIdx;
				else
					nMargine1 = nIdx;
			} // else from if( m_bSortedAscending )
			nIdx2 = (nMargine2 - nMargine1)/2 + nMargine1;
			if( nIdx2 == nIdx )
				break;
			nIdx = nIdx2;
		}
		const TYPE & _element =
			( ( CExtArray < TYPE > & ) (*this) )
				.ElementAt( nIdx );
		int nCompareResult = OnCompareItems( _key, _element );
		return ( nCompareResult < 0 ) ? nIdx : (nIdx+1);
	}
	int BinarySearch( const TYPE & _key ) const
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_bSortedStateFlag );

		int nCount = (int)GetSize();
		if( nCount == 0 )
			return -1;

		int nIdx, nIdx2, nMargine1, nMargine2;

		nMargine1 = 0;
		nMargine2 = nCount - 1;
		nIdx = nMargine2;
		for( ; true; )
		{
			const TYPE & element = GetAt( nIdx );
			int compare_result = OnCompareItems( _key, element );
			if( compare_result == 0 )
				return nIdx;
			if( m_bSortedAscending )
			{
				if( compare_result > 0 )
					nMargine1 = nIdx;
				else
					nMargine2 = nIdx;
			} // if( m_bSortedAscending )
			else
			{
				if( compare_result > 0 )
					nMargine2 = nIdx;
				else
					nMargine1 = nIdx;
			} // else from if( m_bSortedAscending )
			nIdx2 = ( nMargine2 - nMargine1 ) / 2 + nMargine1;
			if( nIdx2 == nIdx )
				break;
			nIdx = nIdx2;
		} // for( ; true; )
		return -1;
	}
	bool AppendFromOther(
		const CExtArray < TYPE > & other,
		bool bResort = false
		)
	{
		bool retVal = true;
		try
		{
			int len = (int)other.GetSize();
			for( int i = 0; i < len; i++ )
			{
				const TYPE & element = other.GetAt(i);
				Add( (TYPE &) element );
			} // for( int i = 0; i < len; i++ )
			if( bResort )
				retVal =
					QuickSort( m_bSortedAscending );
		}
		catch( ... )
		{
			retVal = false;
		}
		return retVal;
	}
	bool AssignFromOther(
		const CExtArray < TYPE > & other,
		bool bCopyItems = true,
		bool bCopyParms = true,
		bool bResort = false
		)
	{
		bool retVal = true;
		try
		{
			if( bCopyItems )
			{
				Clean();
				retVal = AppendFromOther( other, bResort );
				__EXT_DEBUG_RICH_CONTENT_ASSERT( GetSize() == other.GetSize() );
			} // if( bCopyItems )
			if( bCopyParms )
			{
				m_bSortedStateFlag = other.m_bSortedStateFlag;
				m_bSortedAscending = other.m_bSortedAscending;
			} // if( bCopyParms )
		}
		catch( ... )
		{
			retVal = false;
		}
		return retVal;
	}
	bool EvalDifference(
		CExtArray < TYPE > & other,
		CExtArray < TYPE > & result,
		bool bResort = false
		) const
	{ // compute all own items which are not present in other array
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_bSortedStateFlag );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( other.m_bSortedStateFlag );
		result.Clean();
		int len = int( other.GetSize() );
		if( len == 0 )
			return result.AssignFromOther( *this, true, true, bResort );
		bool retVal = true;
		try
		{
			len = int( GetSize() );
			for( int i = 0; i < len; i++ )
			{
				const TYPE & element = GetAt(i);
				int nIdx = other.BinarySearch( element );
				if(nIdx == -1)
					result.Add( (TYPE &) element );
			} // for( int i = 0; i < len; i++ )
			if( bResort )
				retVal = ( const_cast < CExtArray < TYPE > * > ( this ) ) -> QuickSort( m_bSortedAscending );
		}
		catch( ... )
		{
			retVal = false;
		}
		return retVal;
	}
	bool EvalIntersection(
		CExtArray < TYPE > & other,
		CExtArray < TYPE > & result,
		bool bResort = false
		) const
	{ // computer all own items which are present in other array
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_bSortedStateFlag );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( other.m_bSortedStateFlag );
		result.Clean();
		int len = int( GetSize() );
		if( len == 0 )
			return true;
		bool retVal = true;
		try
		{
			for( int i = 0; i < len; i++ )
			{
				const TYPE & element = GetAt(i);
				int nIdx = other.BinarySearch( element );
				if( nIdx != -1 )
					result.Add( const_cast < TYPE & > ( element ) );
			} // for( int i = 0; i < len; i++ )
			if( bResort )
				retVal = ( const_cast < CExtArray < TYPE > * > ( this ) ) -> QuickSort( m_bSortedAscending );
		}
		catch( ... )
		{
			retVal = false;
		}
		return retVal;
	}
	CExtArray < TYPE > & operator = ( const CExtArray < TYPE > & other )
	{
		Clean();
		__EXT_DEBUG_RICH_CONTENT_VERIFY( AssignFromOther( other ) );
		return (*this);
	}
	void operator += ( const CExtArray < TYPE > & other )
	{
		__EXT_DEBUG_RICH_CONTENT_VERIFY( AppendFromOther( other, m_bSortedStateFlag ) );
	}
	int InsertUnique( const TYPE & element )
	{ // returns element index
		int nCount = int( GetSize() );
		if( nCount == 0 )
		{
			m_bSortedStateFlag = true;
			Add( const_cast < TYPE & > ( element ) );
			return 0;
		} // if( nCount == 0 )
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_bSortedStateFlag );
		int nIdx = BinaryInsertPosFind( element );
		if( nIdx == nCount )
		{
			if( ElementAt( nIdx-1 ) == element )
				return nIdx;
			Add( const_cast < TYPE & > ( element ) );
			return nCount;
		} // if( nCount == 0 )
		if( ElementAt( nIdx ) == element )
			return nIdx;
		InsertAt( nIdx, const_cast < TYPE & > ( element ), 1 );
		return nIdx;
	}
	int operator += ( const TYPE & element )
	{
		if( m_bSortedStateFlag || GetSize() == 0 )
			return InsertUnique( element );
		Add( * ( ( TYPE * ) ( &element ) ) );
		return int( GetSize() - 1 );
	}
	TYPE & ElementAt( int nIndex )
	{
		return CArray < TYPE, TYPE & > :: ElementAt( nIndex );
	}
	const TYPE & ElementAt( int nIndex ) const
	{
		return ( ( CArray < TYPE, TYPE & > * ) this ) -> ElementAt( nIndex );
	}
	int CompareWithOther(
		const CExtArray < TYPE > & other
		) const
	{
		int nCount = (int)GetSize();
		int nCountOther = (int)other.GetSize();
		if( nCount < nCountOther )
			return -1;
		if( nCount > nCountOther )
			return 1;
		int nIndex, nCmpResult;
		for( nIndex = 0; nIndex < nCount; nIndex ++ )
		{
			const TYPE & element1 = ElementAt( nIndex );
			const TYPE & element2 = other.ElementAt( nIndex );
			nCmpResult = OnCompareItems( element1, element2 );
			if( nCmpResult != 0 )
				return nCmpResult;
		}
		return 0;
	}
	bool operator == ( const CExtArray < TYPE > & other ) const { return ( CompareWithOther( other ) == 0 ) ? true : false; }
	bool operator != ( const CExtArray < TYPE > & other ) const { return ( CompareWithOther( other ) != 0 ) ? true : false; }
	bool operator <= ( const CExtArray < TYPE > & other ) const { return ( CompareWithOther( other ) <= 0 ) ? true : false; }
	bool operator >= ( const CExtArray < TYPE > & other ) const { return ( CompareWithOther( other ) >= 0 ) ? true : false; }
	bool operator <  ( const CExtArray < TYPE > & other ) const { return ( CompareWithOther( other ) <  0 ) ? true : false; }
	bool operator >  ( const CExtArray < TYPE > & other ) const { return ( CompareWithOther( other ) >  0 ) ? true : false; }
}; // template class CExtArray

/////////////////////////////////////////////////////////////////////////////
// template class CExtHashMapT

template < class _V > class CExtCTH // (type-dependent hash key computation)
{
public:
	inline static __EXT_MFC_UINT_PTR Compute( _V _key )
	{
		__EXT_MFC_UINT_PTR nH = ( (__EXT_MFC_UINT_PTR) (LPVOID) _key ) >> 4;
		return nH;
	}
}; /// template class CExtCTH (type-dependent hash key computation)

template <> class CExtCTH < __int64 >
{
public:
	inline static __EXT_MFC_UINT_PTR Compute( __int64 _key )
	{
		__EXT_MFC_UINT_PTR nHX = ( (__EXT_MFC_UINT_PTR) (LPVOID) ( _key >> 32 ) ) >> 4;
		__EXT_MFC_UINT_PTR nHY = ( (__EXT_MFC_UINT_PTR) (LPVOID) ( _key ) ) >> 4;
		__EXT_MFC_UINT_PTR nH = nHX + nHY;
		return nH;
	}
}; /// template class CExtCTH < __int64 >

template <> class CExtCTH < __int64 & >
{
public:
	inline static __EXT_MFC_UINT_PTR Compute( __int64 & _key )
	{
		__EXT_MFC_UINT_PTR nHX = ( (__EXT_MFC_UINT_PTR) (LPVOID) ( _key >> 32 ) ) >> 4;
		__EXT_MFC_UINT_PTR nHY = ( (__EXT_MFC_UINT_PTR) (LPVOID) ( _key ) ) >> 4;
		__EXT_MFC_UINT_PTR nH = nHX + nHY;
		return nH;
	}
}; /// template class CExtCTH < __int64 >

template < class _V, class _VA > class CExtCTT // (type processing for container elements)
{
public:
	inline static void Construct( _V * pElements, __EXT_MFC_INT_PTR nCount )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( nCount == 0 || AfxIsValidAddress( pElements, nCount * sizeof(_V) ) );
		::memset( (void*)pElements, 0, nCount * sizeof(_V) );
		for( ; nCount--; pElements++ )
			::new((void*)pElements) _V;
	}
	inline static void Destruct( _V * pElements, __EXT_MFC_INT_PTR nCount )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( nCount == 0 || AfxIsValidAddress( pElements, nCount * sizeof(_V) ) );
		for( ; nCount--; pElements++ )
			pElements->~_V();
	}
	inline static void Copy( _V * pDest, const _V * pSrc, __EXT_MFC_INT_PTR nCount )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( nCount == 0 || AfxIsValidAddress( pDest, nCount * sizeof(_V) ) );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( nCount == 0 || AfxIsValidAddress( pSrc, nCount * sizeof(_V) ) );
		while( nCount-- )
			*pDest++ = *pSrc++;
	}
	inline static void Serialize( CArchive & ar, _V * pElements, __EXT_MFC_INT_PTR nCount )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( nCount == 0 || AfxIsValidAddress( pElements, nCount * sizeof(_V) ) );
		if( ar.IsStoring() )
			ar.Write( (void*)pElements, UINT( nCount * sizeof(_V) ) );
		else
			ar.Read( (void*)pElements, UINT( nCount * sizeof(_V) ) );
	}
	inline static bool IsEqual( const _V * pElement1, const _VA * pElement2 )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( AfxIsValidAddress( pElement1, sizeof(_V), FALSE ) );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( AfxIsValidAddress( pElement2, sizeof(_VA), FALSE ) );
		return ( (*pElement1) == (*pElement2) ) ? true : false;
	}
	inline static int Compare( const _V * pElement1, const _VA * pElement2 )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( AfxIsValidAddress( pElement1, sizeof(_V), FALSE ) );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( AfxIsValidAddress( pElement2, sizeof(_VA), FALSE ) );
		if( (*pElement1) < (*pElement2) )
			return -1;
		if( (*pElement1) > (*pElement2) )
			return 1;
		return 0;
	}
#if (defined _DEBUG)
	inline static void Dump( CDumpContext & dc, const _V * pElements, __EXT_MFC_INT_PTR nCount )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( nCount == 0 || AfxIsValidAddress( pElements, nCount * sizeof(_V), FALSE ) );
		&dc;
		pElements;
		nCount;
	}
#endif // (defined _DEBUG)
}; /// template class CExtCTT (type processing for container elements)

#define __EXT_MFC_BEFORE_START_POSITION		((POSITION)-1L)

template < class _K, class _KA, class _V, class _VA > class CExtHashMapT : public CObject
{
protected:
	struct _A_t
	{
		_A_t * m_pNextA;
		__EXT_MFC_UINT_PTR m_nHV;
		_K m_key;
		_V m_value;
	};
	_A_t ** m_ppHT, * m_pFL;
	__EXT_MFC_UINT_PTR m_nSizeHT;
	__EXT_MFC_INT_PTR m_nCount;
	struct CPlex * m_pBlocks;
	__EXT_MFC_INT_PTR m_nBlockSize;
	_A_t * _A_New()
	{
		if( m_pFL == NULL )
		{
			CPlex * newBlock = CPlex::Create( m_pBlocks, m_nBlockSize, sizeof( _A_t ) );
			_A_t * pA = (_A_t*) newBlock->data();
			pA += m_nBlockSize - 1;
			for( __EXT_MFC_INT_PTR i = m_nBlockSize-1; i >= 0; i--, pA-- )
			{
				pA->m_pNextA = m_pFL;
				m_pFL = pA;
			}
		}
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_pFL != NULL );
		_A_t * pA = m_pFL;
		m_pFL = m_pFL->m_pNextA;
		m_nCount++;
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_nCount > 0 );
		CExtCTT < _K, _KA > :: Construct ( &pA->m_key, 1 );
		CExtCTT < _V, _VA > :: Construct ( &pA->m_value, 1 );
		return pA;
	}
	void _A_Free( _A_t * pA )
	{
		CExtCTT < _V, _VA > :: Destruct ( &pA->m_value, 1 );
		CExtCTT < _K, _KA > :: Destruct ( &pA->m_key, 1 );
		pA->m_pNextA = m_pFL;
		m_pFL = pA;
		m_nCount--;
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_nCount >= 0 );
		if( m_nCount == 0 )
			RemoveAll();
	}
	_A_t * _A_GetAt( _KA _key, __EXT_MFC_UINT_PTR & nH ) const
	{
		nH = CExtCTH < _KA > :: Compute ( _key ) % m_nSizeHT;
		if( m_ppHT == NULL )
			return NULL;
		_A_t * pA;
		for( pA = m_ppHT[ nH ]; pA != NULL; pA = pA->m_pNextA )
			if( CExtCTT < _K, _KA > :: IsEqual( &pA->m_key, &_key ) )
				return pA;
		return NULL;
	}
public:
	CExtHashMapT( __EXT_MFC_INT_PTR nBlockSize = 10 )
		: m_ppHT( NULL )
		, m_nSizeHT( 17 )
		, m_nCount( 0 )
		, m_pFL( NULL )
		, m_pBlocks( NULL )
		, m_nBlockSize( nBlockSize )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_nBlockSize > 0 );
	}
	virtual ~CExtHashMapT()
	{
		RemoveAll();
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_nCount == 0 );
	}
#ifdef _DEBUG
	virtual void Dump( CDumpContext & dc ) const
	{
		CObject::Dump( dc );
		dc << "with " << m_nCount << " elements";
		if( dc.GetDepth() > 0 )
		{
			_K _key;
			_V _val;
			POSITION pos = GetStartPosition();
			while( pos != NULL )
			{
				GetNextAssoc( pos, _key, _val );
				dc << "\n\t[";
				CExtCTT < _K, _KA > :: Dump ( dc, &_key, 1 );
				dc << "] = ";
				CExtCTT < _V, _VA > :: Dump ( dc, &_val, 1 );
			}
		}
		dc << "\n";
	}
	virtual void AssertValid() const
	{
		CObject::AssertValid();
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_nSizeHT > 0 );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_nCount == 0 || m_ppHT != NULL );
	}
#endif
	__EXT_MFC_INT_PTR GetCount() const
	{
		return m_nCount;
	}
	bool IsEmpty() const
	{
		bool bIsEmpty = ( m_nCount == 0 ) ? true : false;
		return bIsEmpty;
	}
	bool Lookup( _KA _key, _V & _val ) const
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this );
		__EXT_MFC_UINT_PTR nH;
		_A_t * pA = _A_GetAt( _key, nH );
		if( pA == NULL )
			return false;
		_val = pA->m_value;
		return true;
	}
	_V & operator [] ( _KA _key )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this );
		__EXT_MFC_UINT_PTR nH;
		_A_t * pA;
		if( ( pA = _A_GetAt( _key, nH ) ) == NULL )
		{
			if( m_ppHT == NULL )
				InitHashTable( m_nSizeHT );
			pA = _A_New();
			pA->m_nHV = nH;
			pA->m_key = _key;
			pA->m_pNextA = m_ppHT[ nH ];
			m_ppHT[ nH ] = pA;
		}
		return pA->m_value;
	}
	void SetAt( _KA _key, _VA _val )
	{
		(*this)[ _key ] = _val;
	}
	BOOL RemoveKey( _KA _key )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this );
		if( m_ppHT == NULL )
			return FALSE;
		_A_t ** ppPrevA = &m_ppHT[ CExtCTH < _KA > :: Compute ( _key ) % m_nSizeHT ];
		_A_t * pA;
		for( pA = *ppPrevA; pA != NULL; pA = pA->m_pNextA )
		{
			if( CExtCTT < _K, _KA > :: IsEqual( &pA->m_key, &_key ) )
			{
				(*ppPrevA) = pA->m_pNextA;
				_A_Free( pA );
				return TRUE;
			}
			ppPrevA = &pA->m_pNextA;
		}
		return FALSE;
	}
	void RemoveAll()
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this );
		if( m_ppHT != NULL )
		{
			for( __EXT_MFC_UINT_PTR nH = 0; nH < m_nSizeHT; nH ++ )
			{
				_A_t * pA;
				for( pA = m_ppHT[ nH ]; pA != NULL; pA = pA->m_pNextA )
				{
					CExtCTT < _V, _VA > :: Destruct ( &pA->m_value, 1 );
					CExtCTT < _K, _KA > :: Destruct ( &pA->m_key, 1 );
				}
			}
		}
		delete[] m_ppHT;
		m_ppHT = NULL;
		m_nCount = 0;
		m_pFL = NULL;
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
	}
	POSITION GetStartPosition() const
	{
		POSITION pos = ( m_nCount == 0 ) ? NULL : __EXT_MFC_BEFORE_START_POSITION;
		return pos;
	}
	void GetNextAssoc( POSITION & pos, _K & _key, _V & _val ) const
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_ppHT != NULL );
		_A_t * pRetA = (_A_t*) pos;
		__EXT_DEBUG_RICH_CONTENT_ASSERT( pRetA != NULL );
		if( pRetA == (_A_t*) __EXT_MFC_BEFORE_START_POSITION )
		{
			for( __EXT_MFC_UINT_PTR nBucket = 0; nBucket < m_nSizeHT; nBucket++ )
				if( ( pRetA = m_ppHT[nBucket] ) != NULL )
					break;
			__EXT_DEBUG_RICH_CONTENT_ASSERT( pRetA != NULL );
		}
		__EXT_DEBUG_RICH_CONTENT_ASSERT( AfxIsValidAddress( pRetA, sizeof(_A_t) ) );
		_A_t * pNextA;
		if( ( pNextA = pRetA->m_pNextA ) == NULL )
		{
			for( __EXT_MFC_UINT_PTR nBucket = pRetA->m_nHV + 1; nBucket < m_nSizeHT; nBucket++ )
				if( ( pNextA = m_ppHT[nBucket] ) != NULL )
					break;
		}
		pos = (POSITION) pNextA;
		_key = pRetA->m_key;
		_val = pRetA->m_value;
	}
	__EXT_MFC_UINT_PTR GetHashTableSize() const
	{
		return m_nSizeHT;
	}
	void InitHashTable( __EXT_MFC_UINT_PTR nHashSize, bool bAllocNow = true )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( m_nCount == 0 );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( nHashSize > 0 );
		if( m_ppHT != NULL )
		{
			delete[] m_ppHT;
			m_ppHT = NULL;
		}
		if( bAllocNow )
		{
			m_ppHT = new _A_t * [ nHashSize ];
			::memset( m_ppHT, 0, sizeof(_A_t*) * nHashSize );
		}
		m_nSizeHT = nHashSize;
	}
	virtual void Serialize( CArchive & ar )
	{
		__EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this );
		CObject::Serialize( ar );
		if( ar.IsStoring() )
		{
			ar.WriteCount( m_nCount );
			if( m_nCount == 0 )
				return;
			__EXT_DEBUG_RICH_CONTENT_ASSERT( m_ppHT != NULL );
			for( __EXT_MFC_UINT_PTR nH = 0; nH < m_nSizeHT; nH ++ )
			{
				_A_t * pA;
				for( pA = m_ppHT[ nH ]; pA != NULL; pA = pA->m_pNextA )
				{
					CExtCTT < _K, _KA > :: Serialize ( ar, &pA->m_key, 1 );
					CExtCTT < _V, _VA > :: Serialize ( ar, &pA->m_value, 1 );
				}
			}
		}
		else
		{
			DWORD nNew = (DWORD)ar.ReadCount();
			while( nNew -- )
			{
				_K _key;
				_V _val;
				CExtCTT < _K, _KA > :: Serialize ( ar, &_key, 1 );
				CExtCTT < _V, _VA > :: Serialize ( ar, &_val, 1 );
				SetAt( _key, _val );
			}
		}
	}
}; /// template class CExtHashMapT

/////////////////////////////////////////////////////////////////////////////
/// CExtRichFontDescription

class __PROF_UIS_API CExtRichFontDescription
{
public:
	LOGFONT m_lf;
	LONG m_nLineSpacingPX;	// -1 not used
	DOUBLE m_lfLineSpacingPercent;	// default 1.0, 1.0 is 100 %
//	bool m_bBlink:1, m_bOverline:1; // text decoration

	static int stat_impl_cmp_DOUBLE( const DOUBLE & parm1, const DOUBLE & parm2 )
	{
		if( parm1 < parm2 )
			return -1;
		if( parm1 > parm2 )
			return +1;
		return 0;
	}
	static int stat_impl_cmp_LONG( const LONG & parm1, const LONG & parm2 )
	{
		if( parm1 < parm2 )
			return -1;
		if( parm1 > parm2 )
			return +1;
		return 0;
	}
	static int stat_impl_cmp_BOOL( const BOOL & parm1, const BOOL & parm2 )
	{
		if( (!parm1) && parm2 )
			return -1;
		if( parm1 && (!parm2) )
			return +1;
		return 0;
	}
	static int stat_impl_cmp_bool( const bool parm1, const bool parm2 )
	{
		if( (!parm1) && parm2 )
			return -1;
		if( parm1 && (!parm2) )
			return +1;
		return 0;
	}

	CExtRichFontDescription();
	CExtRichFontDescription( const CExtRichFontDescription & other );
	virtual int Compare( const CExtRichFontDescription & other ) const;
	virtual void Assign( const CExtRichFontDescription & other );
	virtual ~CExtRichFontDescription();

	CExtRichFontDescription & operator = ( const CExtRichFontDescription & other ) { Assign( other ); return (*this); }
	bool operator == ( const CExtRichFontDescription & other ) const { return ( Compare( other ) == 0 ) ? true : false; }
	bool operator != ( const CExtRichFontDescription & other ) const { return ( Compare( other ) != 0 ) ? true : false; }
	bool operator <  ( const CExtRichFontDescription & other ) const { return ( Compare( other ) <  0 ) ? true : false; }
	bool operator <= ( const CExtRichFontDescription & other ) const { return ( Compare( other ) <= 0 ) ? true : false; }
	bool operator >  ( const CExtRichFontDescription & other ) const { return ( Compare( other ) >  0 ) ? true : false; }
	bool operator >= ( const CExtRichFontDescription & other ) const { return ( Compare( other ) >= 0 ) ? true : false; }

	static HFONT stat_dc_get_current_font( HDC hDC );
	static CFont * stat_dc_get_current_font( CDC & dc );

	void FromFont( HFONT hFont );
	virtual void FromFont( CFont & _font );
	void FromSurfaceCurrentFont( CDC & dc );
	void FromSurfaceCurrentFont( HDC hDC );
	void FromDefaultGuiFont();
	void FromSystemFont();
	virtual HFONT CreateFont(
		INT nLO,
		bool bPlainTextUnderlineMode = false
		) const;
}; /// class CExtRichFontDescription

/////////////////////////////////////////////////////////////////////////////
/// CExtRichStyleDescription

class __PROF_UIS_API CExtRichStyleDescription
{
public:
	enum e_align_horz_t
	{
		eahLeft    = 0,
		eahCenter  = 1,
		eahRight   = 2,
		eahJustify = 3,
		eahJustifyWithLastLine = 4,
		eahJustifyRight = 5,
		eahDefault = 0,
		eahUnknownChanging = -1, // for style changing only
	};
	enum e_align_vert_t
	{
		eavTop         = 0,
		eavTextTop     = 1,
		eavMiddle      = 2,
		eavBottom      = 3,
		eavTextBottom  = 4,
		eavBaseLine    = 5,
		eavSub		   = 6,
		eavSuper	   = 7,
		eavDefault     = 5,
		eavUnknownChanging = -1, // for style changing only
	};
	enum e_border_style_t
	{
		ebsUnknown = -1,
		ebsHidden  = 0,
		ebsNone    = 1,
		ebsInherit = 2,
		ebsDotted  = 3,
		ebsDashed  = 4,
		ebsSolid   = 5,
		ebsDouble  = 6,
		ebsGroove  = 7,
		ebsRidge   = 8,
		ebsInset   = 9,
		ebsOutset  = 10,
	};
	enum e_text_transform_t
	{
		ettNone       = -1,
		ettCapitalize = 0,
		ettUppercase  = 1,
		ettLowercase  = 2,
	};
	enum e_white_space_type_t
	{
		ewstNormal  = 0,
		ewstNoWrap  = 1,
		ewstPre     = 2,
		ewstPreLine = 3,
		ewstPreWrap = 4,
		ewstInherit = 5,
	};
	enum e_background_repeat_t
	{
		ebrRepeat   = 0,
		ebrRepeatX  = 1,
		ebrRepeatY  = 2,
		ebrNoRepeat = 3,
		ebrInherit  = 4,
	};
	enum e_background_attachment_t
	{
		ebaScroll  = 0,
		ebaFixed   = 1,
		ebaInherit = 2,
	};
	enum e_background_postion_alignment_t
	{
		// helper implementation constants
		ebpa_impl_basic_zero  = 0,
		ebpa_impl_basic_low   = 1,
		ebpa_impl_basic_mid   = 2,
		ebpa_impl_basic_high  = 3,
		ebpa_impl_horz_shift  = 0,
		ebpa_impl_vert_shift  = 2,
		ebpa_part_mask        = 3,
		// helper horizontal constants
		ebpa_impl_horz_left   = (ebpa_impl_basic_low<<ebpa_impl_horz_shift),
		ebpa_impl_horz_center = (ebpa_impl_basic_mid<<ebpa_impl_horz_shift),
		ebpa_impl_horz_right  = (ebpa_impl_basic_high<<ebpa_impl_horz_shift),
		// helper vertical constants
		ebpa_impl_vert_top    = (ebpa_impl_basic_low<<ebpa_impl_vert_shift),
		ebpa_impl_vert_center = (ebpa_impl_basic_mid<<ebpa_impl_vert_shift),
		ebpa_impl_vert_bottom = (ebpa_impl_basic_high<<ebpa_impl_vert_shift),
		// real constants
		ebpa_top_left         =  (ebpa_impl_vert_top|ebpa_impl_horz_left),      // css
		ebpa_top_center       =  (ebpa_impl_vert_top|ebpa_impl_horz_center),    // css
		ebpa_top_right        =  (ebpa_impl_vert_top|ebpa_impl_horz_right),     // css
		ebpa_center_left      =  (ebpa_impl_vert_center|ebpa_impl_horz_left),   // css
		ebpa_center_center    =  (ebpa_impl_vert_center|ebpa_impl_horz_center), // css
		ebpa_center_right     =  (ebpa_impl_vert_center|ebpa_impl_horz_right),  // css
		ebpa_bottom_left      =  (ebpa_impl_vert_bottom|ebpa_impl_horz_left),   // css
		ebpa_bottom_center    =  (ebpa_impl_vert_bottom|ebpa_impl_horz_center), // css
		ebpa_bottom_right     =  (ebpa_impl_vert_bottom|ebpa_impl_horz_right),  // css
		// default/initial/unspecified value
		ebpa_unspecified      = 0,
		// fully specified range
		ebpa_fs_min_value     = ((ebpa_impl_basic_low<<ebpa_impl_horz_shift)|(ebpa_impl_basic_low<<ebpa_impl_vert_shift)),
		ebpa_fs_max_value     = ((ebpa_impl_basic_high<<ebpa_impl_horz_shift)|(ebpa_impl_basic_high<<ebpa_impl_vert_shift)),
	};
	static inline e_background_postion_alignment_t stat_CheckBackgroundPositionAlignmentFullySpecified(
		e_background_postion_alignment_t eBPA
		)
	{
		e_background_postion_alignment_t eHorzPart = e_background_postion_alignment_t( (INT(eBPA)>>INT(ebpa_impl_horz_shift))&INT(ebpa_part_mask) );
		e_background_postion_alignment_t eVertPart = e_background_postion_alignment_t( (INT(eBPA)>>INT(ebpa_impl_vert_shift))&INT(ebpa_part_mask) );
		if( eHorzPart == ebpa_impl_basic_zero && eVertPart == ebpa_impl_basic_zero )
		{
			__EXT_DEBUG_RICH_CONTENT_ASSERT( eBPA == ebpa_unspecified );
			return ebpa_unspecified;
		}
		if( eHorzPart == ebpa_impl_basic_zero && eVertPart != ebpa_impl_basic_zero )
			eHorzPart = ebpa_impl_basic_mid;
		if( eHorzPart != ebpa_impl_basic_zero && eVertPart == ebpa_impl_basic_zero )
			eVertPart = ebpa_impl_basic_mid;
		eBPA = e_background_postion_alignment_t((INT(eHorzPart)<<INT(ebpa_impl_horz_shift))|(INT(eVertPart)<<INT(ebpa_impl_vert_shift)));
		return eBPA;
	}
	enum e_background_position_type_t
	{
		ebpt_unspecified = 0,
		ebpt_align = 1,
		ebpt_percent = 2,
		ebpt_absolute = 3,
		ebpt_inherit = 4,
	};
	enum e_background_color_type_t
	{
		ebct_transparent = 0,
		ebct_color = 1,
		ebct_inherit = 2,
	};
	enum e_outline_style_t
	{
		eos_none	= 0,
		eos_dotted	= 1,
		eos_dashed	= 2,
		eos_solid	= 3,
		eos_double	= 4,
		eos_groove	= 5,
		eos_ridge	= 6,
		eos_inset	= 7,
		eos_outset	= 8,
		eos_inherit	= 9,
	};
	enum e_list_style_position_t
	{
		elsp_inside = 0,
		elsp_outside = 1,
		elsp_inherit = 2,
	};
	enum e_list_style_type_t
	{
		elst_none = 0,
		elst_circle = 1,
		elst_disc = 2,
		elst_square = 3,
		elst_decimal = 4,
		elst_decimal_leading_zero = 5,
		elst_lower_alpha = 6,
		elst_upper_alpha = 7,
		elst_lower_latin = 8,
		elst_upper_latin = 9,
		elst_lower_roman = 10,
		elst_upper_roman = 11,
		elst_inherit = 12,
		elst_unspecified = -1,
	};
	enum e_display_t
	{
		ed_none = 0,
		ed_block = 1,
		ed_inline = 2,
		ed_inline_block = 3,
		ed_inline_table = 4,
		ed_list_item = 5,
		ed_run_in = 6,
		ed_table = 7,
		ed_table_caption = 8,
		ed_table_cell = 9,
		ed_table_column = 10,
		ed_table_column_group = 11,
		ed_table_footer_group = 12,
		ed_table_header_group = 13,
		ed_table_row = 14,
		ed_table_row_group = 15,
		ed_ui_dock = 16,
		ed_inherit = 17,
	};
	enum e_ui_dock_t
	{
		e_ui_dock_unspecified = 0,
		e_ui_dock_top = 1,
		e_ui_dock_pre_top = 2,
		e_ui_dock_bottom = 3,
		e_ui_dock_pre_bottom = 4,
		e_ui_dock_left = 5,
		e_ui_dock_pre_left = 6,
		e_ui_dock_right = 7,
		e_ui_dock_pre_right = 8,
		e_ui_dock_fill = 9,
		e_ui_dock_pre_fill = 10,
	};
	enum e_visibility_t
	{
		ev_visible = 0,
		ev_hidden = 1,
		ev_collapse = 2,
		ev_inherit = 3,
	};
	enum e_position_t
	{
		ep_absolute = 0,
		ep_fixed = 1,
		ep_relative = 2,
		ep_static = 3,
		ep_inherit = 4,
	};
	enum e_overflow_t
	{
		eo_visible = 0,
		eo_hidden = 1,
		eo_scroll = 2,
		eo_auto = 3,
		eo_inherit = 4,
	};
	enum e_scroll_bar_type_t
	{
		esbt_auto = 0, // overflow-defined
		esbt_hidden = 1,
		esbt_visible = 2,
	};
	enum e_float_t
	{
		ef_left = 0,
		ef_right = 1,
		ef_none = 2,
		ef_inherit = 3,
	};
	enum e_cursor_t
	{
		ec_url = 0,
		ec_auto = 1,
		ec_crosshair = 2,
		ec_default = 3,
		ec_e_resize = 4,
		ec_help = 5,
		ec_move = 6,
		ec_n_resize = 7,
		ec_ne_resize = 8,
		ec_nw_resize = 9,
		ec_pointer = 10,
		ec_progress = 11,
		ec_s_resize = 12,
		ec_se_resize = 13,
		ec_sw_resize = 14,
		ec_text = 15,
		ec_w_resize = 16,
		ec_wait = 17,
		ec_inherit = 18,
	};
	enum e_clip_image_t
	{
		eci_shape = 0,
		eci_auto = 1,
		eci_inherit = 2,
	};
//	enum e_clear_sides_t
//	{
//		ecs_left = 0,
//		ecs_right = 1,
//		ecs_both = 2,
//		ecs_none = 3,
//		ecs_inherit = 4,
//	};
	enum e_outer_type_t
	{
		eot_left = 0,
		eot_top = 1,
		eot_right = 2,
		eot_bottom = 3,
	};
	enum e_z_index_type_t
	{
		ezi_auto = 0,
		ezi_inherit = 1,
		ezi_explicit = 2,
	};
	enum e_page_break_before_after_t
	{
		epbba_auto = 0,
		epbba_always = 1,
		epbba_avoid = 2,
		epbba_left = 3,
		epbba_right = 4,
		epbba_inherit = 5,
	};
	enum e_border_collapse_t
	{
		ebc_collapse = 0,
		ebc_separate = 1,
		ebc_inherit = 2,
	};
	enum e_caption_side_type_t
	{
		ecst_top = 0,
		ecst_bottom = 1,
		ecst_inherit = 2,
	};
	enum e_empty_cells_t
	{
		eec_hide = 0,
		eec_show = 1,
		eec_inherit = 2,
	};
	enum e_table_layout_t
	{
		etl_auto = 0,
		etl_fixed = 1,
		etl_inherit = 2,
	};
	enum e_noshade_t
	{
		ent_noshade_unknown = 0,
		ent_noshade_noshade = 1,
	};
	enum e_ui_list_navigation_t
	{
		e_ui_ln_unspecified = 0,
		e_ui_ln_none = 1,
		e_ui_ln_list_value_first = 2,
		e_ui_ln_list_x = 2,
		e_ui_ln_list_y = 3,
		e_ui_ln_list_xy = 4,
		e_ui_ln_list_value_last = 4,
	};
	enum e_ui_drawmethod_t
	{
		e_ui_dm_hollow		= 0,
		e_ui_dm_center		= 1,
		e_ui_dm_stretch		= 2,
		e_ui_dm_tile_h		= 3,
		e_ui_dm_tile_v		= 4,
		e_ui_dm_tile		= 5,
		e_ui_dm_unspecified	= -1,
	};
	enum e_ui_yes_not_option_t
	{
		e_ui_yn_unspecified = 0,
		e_ui_yn_not = 1,
		e_ui_yn_yes = 2,
	};
	static int stat_impl_cmp_YN( const e_ui_yes_not_option_t & parm1, const e_ui_yes_not_option_t & parm2 )
		{ return CExtRichFontDescription::stat_impl_cmp_LONG( LONG(parm1), LONG(parm2) ); }
	enum e_ui_delayed_draw_t
	{
		e_ui_dd_default = 0,
		e_ui_dd_inherit = 1,
		e_ui_dd_post_sibling = 2,
		e_ui_dd_pre_fixed_absolute = 3,
		e_ui_dd_topmost = 4,
	};
	static int stat_impl_cmp_UI_delayed_draw( const e_ui_delayed_draw_t & parm1, const e_ui_delayed_draw_t & parm2 )
		{ return CExtRichFontDescription::stat_impl_cmp_LONG( LONG(parm1), LONG(parm2) ); }
	enum e_ui_sb_type_t
	{
		e_ui_sbt_default = 0,
		e_ui_sbt_inherit = 1,
		e_ui_sbt_classic = 2,
		e_ui_sbt_pop_over_light = 3,
		e_ui_sbt_pop_over_full = 4,
		e_ui_sbt_none = 5,
	};
	static int stat_impl_cmp_UI_sb_type( const e_ui_sb_type_t & parm1, const e_ui_sb_type_t & parm2 )
		{ return CExtRichFontDescription::stat_impl_cmp_LONG( LONG(parm1), LONG(parm2) ); }

	struct __PROF_UIS_API CQuoutes
	{
		CExtSafeString m_strOpenQuote, m_strCloseQoute;
		CQuoutes() { }
		CQuoutes( const CQuoutes & other ) : m_strOpenQuote( other.m_strOpenQuote ), m_strCloseQoute( other.m_strCloseQoute ) { }
		CQuoutes & operator = ( const CQuoutes & other ) { m_strOpenQuote = other.m_strOpenQuote; m_strCloseQoute = other.m_strCloseQoute; return (*this); }
	}; /// CQuoutes

	struct __PROF_UIS_API pseudo_class_def_t
	{
		e_ui_yes_not_option_t m_eynAfter;
		e_ui_yes_not_option_t m_eynBefore;
		e_ui_yes_not_option_t m_eynFirstChild;
		e_ui_yes_not_option_t m_eynFirstLetter;
		e_ui_yes_not_option_t m_eynFirstLine;
		e_ui_yes_not_option_t m_eynDisabled;
		e_ui_yes_not_option_t m_eynActive;
		e_ui_yes_not_option_t m_eynFocus;
		e_ui_yes_not_option_t m_eynSelection;
		e_ui_yes_not_option_t m_eynHover;
		e_ui_yes_not_option_t m_eynPressed;
		e_ui_yes_not_option_t m_eynLang;
		e_ui_yes_not_option_t m_eynLink;
		e_ui_yes_not_option_t m_eynVisited;
		void InitDefaultSupportedElementStates();
		void Empty();
		bool IsEmpty() const;
		void Assign( const pseudo_class_def_t & other );
		int Compare( const pseudo_class_def_t & other ) const;
		pseudo_class_def_t() { Empty(); }
		pseudo_class_def_t( const pseudo_class_def_t & other ) { Assign( other ); }
		pseudo_class_def_t & operator = ( const pseudo_class_def_t & other ) { Assign( other ); return (*this); }
		bool operator ! () const { return ( ! IsEmpty() ) ? true : false; }
		bool operator == ( const pseudo_class_def_t & other ) const { return ( Compare( other ) == 0 ) ? true : false; }
		bool operator != ( const pseudo_class_def_t & other ) const { return ( Compare( other ) != 0 ) ? true : false; }
		bool operator <  ( const pseudo_class_def_t & other ) const { return ( Compare( other ) <  0 ) ? true : false; }
		bool operator <= ( const pseudo_class_def_t & other ) const { return ( Compare( other ) <= 0 ) ? true : false; }
		bool operator >  ( const pseudo_class_def_t & other ) const { return ( Compare( other ) >  0 ) ? true : false; }
		bool operator >= ( const pseudo_class_def_t & other ) const { return ( Compare( other ) >= 0 ) ? true : false; }
		void Merge( const pseudo_class_def_t & other );
		bool IncElement( const pseudo_class_def_t & elementState ) const;
		void ResetSpecified( const pseudo_class_def_t & statesToReset, e_ui_yes_not_option_t eynSetOn );
		void SetAll( e_ui_yes_not_option_t eynSetOn, bool bSetContentRelated, bool bSetStateRelated );
	}; /// pseudo_class_def_t

	CExtRichFontDescription m_fd;
	e_align_horz_t m_eaH;
	e_align_vert_t m_eaV;
	e_white_space_type_t m_eWST;
	e_background_repeat_t m_eBR;
	e_background_attachment_t m_eBA;
	e_background_postion_alignment_t m_eBPA;
	e_background_position_type_t m_eBPT;
	e_background_color_type_t m_eBCT;
	e_outline_style_t m_eOS;
	e_list_style_position_t m_eLSP;
	e_list_style_type_t m_eLST;
	e_display_t m_eD;
	e_ui_dock_t m_eDock;
	e_visibility_t m_eV;
	e_position_t m_eP;
	e_overflow_t m_eO;
	e_scroll_bar_type_t m_esbtH, m_esbtV;
	e_float_t m_eF;
	e_cursor_t m_eC;
	e_clip_image_t m_eCI;
//	e_clear_sides_t m_eCS;
	e_z_index_type_t m_eZIT;
//	e_page_break_before_after_t m_ePBB, m_ePBA;
	e_border_collapse_t m_eBC;
	e_caption_side_type_t m_eCST;
	e_empty_cells_t m_eEC;
	e_table_layout_t m_eTL;
	e_noshade_t m_eN;
	e_ui_list_navigation_t m_eUiListNavigation;
	e_ui_drawmethod_t m_eUiDrawMethod, m_eUiBackgroundDrawMethod;
	e_ui_yes_not_option_t m_eUiFocusEnsureVisibility;
	e_ui_delayed_draw_t m_eUiDelayedDraw;
	e_ui_sb_type_t m_eUiSBT;
	CPoint m_ptBackgroundAlignment; // percentage or pixels depending from m_eBPT
	LONG m_nParaIndent;
	CRect m_rcPadding, m_rcBorder, m_rcMargin, m_rcCellPadding;
	CRect m_rcClipImageShape;
	CRect m_rcUiSlice, m_rcUiBackgroundSlice, m_rcUiBackgroundExtend;
	CSize m_sizeCellSpacing;
	CSize m_sizeWH; // explicitly specified width and/or height tag attributes, -1 - not specified
	CSize m_sizeWHP; // percent version of m_sizeWH
	CSize m_sizeSpan; // colspan/rowspan values, must be >= 1
	COLORREF m_clrText, m_clrBack;
	COLORREF m_clrBorderLeft, m_clrBorderRight, m_clrBorderTop, m_clrBorderBottom;
	e_border_style_t m_ebsLeft, m_ebsRight, m_ebsTop, m_ebsBottom;
	LONG m_nFirstNumberInList; // <OL>, <UL>, anything else supported too - first number in list
	int m_nUi_transparency, m_nUi_background_transparency;
	CExtSafeString m_strBackgroundImagePath;
	CExtSafeString m_strListStyleImagePath;
	CExtSafeString m_strCursorImagePath;
	CExtSafeString m_strContent;
	CExtSafeString m_strCounterIncrement, m_strCounterReset;
	LONG m_nWordSapcing, m_nLetterSpacing;
	e_text_transform_t m_etT;
//	CExtSafeStringArray m_arrFontNames;
	LONG m_nOutlineWidth;
	COLORREF m_clrOutlineColor;
	LONG m_nMaxHeight, m_nMaxWidth, m_nMinHeight, m_nMinWidth;	// -1 not used; m_nMinHeight, m_nMinWidth - default value is 0
	LONG m_nSpacerModeMinWidth; // space allocation mode
	DOUBLE m_lfMaxHeightP, m_lfMaxWidthP, m_lfMinHeightP, m_lfMinWidthP;	// percent version of m_nMaxHeight, m_nMaxWidth, m_nMinHeight, m_nMinWidth
	CRect m_rcOuter, m_rcOuterP;
	CList < CQuoutes, CQuoutes > m_listQuotes;
	INT m_nZIndex;
	LONG m_nBorderSpacingHoriz, m_nBorderSpacingVert;
	mutable CExtSafeString m_strContentBefore, m_strContentAfter;
	bool m_bDtPrefixOnlyMode:1;
	pseudo_class_def_t m_PCD; // supported element states
	e_ui_yes_not_option_t m_eynFocusOnMakeSelected, m_eynFocusOnMakeUnSelectedSibling, m_eynFocusOffMakeUnSelected, m_eynFocusOffMakeUnSelectedSibling;

	e_display_t GetDisplay( const CExtRichContentItem & _RCI ) const;
	e_visibility_t GetVisibility( const CExtRichContentItem & _RCI ) const;
	bool IsParticipantOfLayout( const CExtRichContentItem & _RCI ) const;
	bool IsParticipantOfPaint( const CExtRichContentItem & _RCI ) const;

	static inline bool stat_ewstIsSingleSpaceMode( e_white_space_type_t eWST )
	{
		if( eWST == ewstNormal || eWST == ewstNoWrap || eWST == ewstPreLine )
			return true;
		return false;
	}
	static inline bool stat_ewstIsNewLineMode( e_white_space_type_t eWST )
	{
		if( eWST == ewstPre || eWST == ewstPreLine || eWST == ewstPreWrap )
			return true;
		return false;
	}
	static inline bool stat_ewstIsLineWrapMode( e_white_space_type_t eWST )
	{
		if( eWST == ewstNormal || eWST == ewstPreLine || eWST == ewstPreWrap )
			return true;
		return false;
	}
	inline static int stat_impl_CompareSizeLEG( const SIZE & size1, const SIZE & size2 )
	{
		if( size1.cx < size2.cx ) return -1;
		if( size1.cx > size2.cx ) return  1;
		if( size1.cy < size2.cy ) return -1;
		if( size1.cy > size2.cy ) return  1;
		return 0;
	}
	inline static bool stat_impl_IsSizeEmptyLEG( const SIZE & size )
	{
		size;
//		if( size.cx < 0 && size.cy < 0 && size.top < 0 && size.bottom < 0 )
//			return true;
//		else
			return false;
	}
	inline static void stat_impl_ApplySizeLEG( SIZE & sizeDst, const SIZE & sizeSrc )
	{
		if( sizeSrc.cx >= 0 ) sizeDst.cx = sizeSrc.cx;
		else                  sizeDst.cx = 0;
		if( sizeSrc.cy >= 0 ) sizeDst.cy = sizeSrc.cy;
		else                  sizeDst.cy = 0;
	}
	inline static void stat_impl_ApplySizeLEG_WH( SIZE & sizeDst, const SIZE & sizeSrc )
	{
		if( sizeSrc.cx >= 0 ) sizeDst.cx = sizeSrc.cx;
		else                  sizeDst.cx = -1;
		if( sizeSrc.cy >= 0 ) sizeDst.cy = sizeSrc.cy;
		else                  sizeDst.cy = -1;
	}
	inline static int stat_impl_ComparePointLEG( const POINT & point1, const POINT & point2 )
	{
		if( point1.x < point2.x ) return -1;
		if( point1.x > point2.x ) return  1;
		if( point1.y < point2.y ) return -1;
		if( point1.y > point2.y ) return  1;
		return 0;
	}
	inline static bool stat_impl_IsPointEmptyLEG( const POINT & point )
	{
		if( point.x < 0 && point.y < 0 ) return true;
		return false;
	}
	inline static void stat_impl_ApplyPointLEG( POINT & pointDst, const POINT & pointSrc )
	{
		if( pointSrc.x >= 0 ) pointDst.x = pointSrc.x;
		else                  pointDst.x = 0;
		if( pointSrc.y >= 0 ) pointDst.y = pointSrc.y;
		else                  pointDst.y = 0;
	}
	inline static void stat_impl_ApplyPointLEG_WH( POINT & pointDst, const POINT & pointSrc )
	{
		if( pointSrc.x >= 0 ) pointDst.x = pointSrc.x;
		else                  pointDst.x = -1;
		if( pointSrc.y >= 0 ) pointDst.y = pointSrc.y;
		else                  pointDst.y = -1;
	}
	inline static int stat_impl_CompareStringArrayLEG( const CExtSafeStringArray & rStrings1, const CExtSafeStringArray & rStrings2 )
	{
		LONG nSize1 = LONG(rStrings1.GetSize()), nSize2 = LONG(rStrings2.GetSize());
		if( nSize1 < nSize2 ) return -1;
		if( nSize1 > nSize2 ) return 1;
		LONG i;
		CExtSafeString strString1, strString2;
		for( i = 0; i < nSize1; i++ )
		{
			int nCmp = _tcscmp( LPCTSTR(rStrings1.GetAt(i)), LPCTSTR(rStrings2.GetAt(i)) );
			if( nCmp != 0 ) return nCmp;
		}
		return 0;
	}
	inline static int stat_impl_CompareStringListLEG( const CList < CQuoutes, CQuoutes > & ltStrings1, const CList < CQuoutes, CQuoutes > & ltStrings2 )
	{
		LONG nSize1 = LONG(ltStrings1.GetCount()), nSize2 = LONG(ltStrings2.GetCount());
		if( nSize1 < nSize2 ) return -1;
		if( nSize1 > nSize2 ) return 1;
		CQuoutes structString1, structString2;
		POSITION pos1 = ltStrings1.GetHeadPosition(), pos2 = ltStrings2.GetHeadPosition();
		for( ; pos1 != NULL; )
		{
			structString1 = ltStrings1.GetNext( pos1 );
			structString2 = ltStrings2.GetNext( pos2 );
			int nCmp = _tcscmp( LPCTSTR(structString1.m_strOpenQuote), LPCTSTR(structString2.m_strOpenQuote) );
			if( nCmp != 0 ) return nCmp;
			nCmp = _tcscmp( LPCTSTR(structString1.m_strCloseQoute), LPCTSTR(structString2.m_strCloseQoute) );
			if( nCmp != 0 ) return nCmp;
		}
		return 0;
	}

	inline static int stat_impl_CompareRectLEG( const RECT & rc1, const RECT & rc2 )
	{
		if( rc1.left   < rc2.left   ) return -1;
		if( rc1.left   > rc2.left   ) return  1;
		if( rc1.right  < rc2.right  ) return -1;
		if( rc1.right  > rc2.right  ) return  1;
		if( rc1.top    < rc2.top    ) return -1;
		if( rc1.top    > rc2.top    ) return  1;
		if( rc1.bottom < rc2.bottom ) return -1;
		if( rc1.bottom > rc2.bottom ) return  1;
		return 0;
	}
	inline static bool stat_impl_IsRectEmptyLEG( const RECT & rc )
	{
		if( rc.left <= 0 && rc.right <= 0 && rc.top <= 0 && rc.bottom <= 0 )
			return true;
		return false;
	}
	inline static bool stat_impl_IsRectEmptyUV( const RECT & rc )
	{
		if( rc.left == __EXT_RICH_CONTENT_NEGATIVE_UNUSED && rc.right == __EXT_RICH_CONTENT_NEGATIVE_UNUSED && rc.top == __EXT_RICH_CONTENT_NEGATIVE_UNUSED && rc.bottom == __EXT_RICH_CONTENT_NEGATIVE_UNUSED )
			return true;
		return false;
	}
	inline static void stat_impl_ApplyRectLEG( RECT & rcDst, const RECT & rcSrc )
	{
		if( rcSrc.left   >= 0 ) rcDst.left   = rcSrc.left;
		else                    rcDst.left   = 0;
		if( rcSrc.right  >= 0 ) rcDst.right  = rcSrc.right;
		else                    rcDst.right  = 0;
		if( rcSrc.top    >= 0 ) rcDst.top    = rcSrc.top;
		else                    rcDst.top    = 0;
		if( rcSrc.bottom >= 0 ) rcDst.bottom = rcSrc.bottom;
		else                    rcDst.bottom = 0;
	}
	inline static void stat_impl_ApplyRectUV( RECT & rcDst, const RECT & rcSrc )
	{
		if( rcSrc.left   != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) rcDst.left   = rcSrc.left;
		if( rcSrc.right  != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) rcDst.right  = rcSrc.right;
		if( rcSrc.top    != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) rcDst.top    = rcSrc.top;
		if( rcSrc.bottom != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) rcDst.bottom = rcSrc.bottom;
	}
	inline static void stat_CopyNonNegativePointMembers( POINT & pointDst, const POINT & pointSrc )
	{
		if( pointSrc.x >= 0 ) pointDst.x = pointSrc.x;
		if( pointSrc.y >= 0 ) pointDst.y = pointSrc.y;
	}
	inline static void stat_CopyNonNegativePointMembersUV( POINT & pointDst, const POINT & pointSrc )
	{
		if( pointSrc.x != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) pointDst.x = pointSrc.x;
		if( pointSrc.y != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) pointDst.y = pointSrc.y;
	}
	inline static void stat_CopyNonNegativeSizeMembers( SIZE & sizeDst, const SIZE & sizeSrc )
	{
		if( sizeSrc.cx >= 0 ) sizeDst.cx = sizeSrc.cx;
		if( sizeSrc.cy >= 0 ) sizeDst.cy = sizeSrc.cy;
	}
	inline static void stat_CopyNonNegativeSizeMembersUV( SIZE & sizeDst, const SIZE & sizeSrc )
	{
		if( sizeSrc.cx != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) sizeDst.cx = sizeSrc.cx;
		if( sizeSrc.cy != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) sizeDst.cy = sizeSrc.cy;
	}
	inline static void stat_CopyNonNegativeRectMembers( RECT & rcDst, const RECT & rcSrc )
	{
		if( rcSrc.left   >= 0 ) rcDst.left   = rcSrc.left;
		if( rcSrc.top    >= 0 ) rcDst.top    = rcSrc.top;
		if( rcSrc.right  >= 0 ) rcDst.right  = rcSrc.right;
		if( rcSrc.bottom >= 0 ) rcDst.bottom = rcSrc.bottom;
	}
	inline static void stat_CopyNonNegativeRectMembersUV( RECT & rcDst, const RECT & rcSrc )
	{
		if( rcSrc.left   != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) rcDst.left   = rcSrc.left;
		if( rcSrc.top    != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) rcDst.top    = rcSrc.top;
		if( rcSrc.right  != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) rcDst.right  = rcSrc.right;
		if( rcSrc.bottom != __EXT_RICH_CONTENT_NEGATIVE_UNUSED ) rcDst.bottom = rcSrc.bottom;
	}


	virtual int Compare( const CExtRichStyleDescription & other ) const;
	virtual void Assign( const CExtRichStyleDescription & other );
	virtual void Empty();

	CExtRichStyleDescription();
	CExtRichStyleDescription( const CExtRichStyleDescription & other );
	virtual ~CExtRichStyleDescription();

	static void stat_RotatePoint( INT nLO, POINT & _pt );
	static void stat_RotateSize( INT nLO, SIZE & _size );
	static void stat_RotateRect( INT nLO, RECT & rc );
	static void stat_RotateBPA( INT nLO, e_background_postion_alignment_t & eBPA );
	static void stat_RotateBR( INT nLO, e_background_repeat_t & eBR );
	static void stat_RotateDwordArray( INT nLO, DWORD * pDwArr );
	void RotateStyleOptions( INT nLO );

	CExtRichStyleDescription & operator = ( const CExtRichStyleDescription & other ) { Assign( other ); return (*this); }
	bool operator == ( const CExtRichStyleDescription & other ) const { return ( Compare( other ) == 0 ) ? true : false; }
	bool operator != ( const CExtRichStyleDescription & other ) const { return ( Compare( other ) != 0 ) ? true : false; }
	bool operator <  ( const CExtRichStyleDescription & other ) const { return ( Compare( other ) <  0 ) ? true : false; }
	bool operator <= ( const CExtRichStyleDescription & other ) const { return ( Compare( other ) <= 0 ) ? true : false; }
	bool operator >  ( const CExtRichStyleDescription & other ) const { return ( Compare( other ) >  0 ) ? true : false; }
	bool operator >= ( const CExtRichStyleDescription & other ) const { return ( Compare( other ) >= 0 ) ? true : false; }

	virtual HFONT CreateFont( INT nLO, bool bPlainTextUnderlineMode = false ) const { return m_fd.CreateFont( nLO, bPlainTextUnderlineMode ); }

	bool ewstIsSingleSpaceMode() const { return stat_ewstIsSingleSpaceMode( m_eWST ); }
	bool ewstIsNewLineMode()     const { return stat_ewstIsNewLineMode    ( m_eWST ); }
	bool ewstIsLineWrapMode()    const { return stat_ewstIsLineWrapMode   ( m_eWST ); }

/*
	class __PROF_UIS_API ptr
	{
		mutable CExtRichStyleDescription * m_ptrObject, * m_ptrAssignFrom;
		mutable bool m_bController:1;
	public:
		CExtRichStyleDescription * GetPtrObjectNoAssign() { return ( m_ptrObject != NULL ) ? m_ptrObject : m_ptrAssignFrom; }
		const CExtRichStyleDescription * GetPtrObjectNoAssign() const { return ( m_ptrObject != NULL ) ? m_ptrObject : m_ptrAssignFrom; }
		CExtRichStyleDescription * GetPtrObject() { return m_ptrObject; }
		const CExtRichStyleDescription * GetPtrObject() const { return m_ptrObject; }
		CExtRichStyleDescription & GetRefObject() { __EXT_DEBUG_RICH_CONTENT_ASSERT( m_ptrObject != NULL ); return (*m_ptrObject); }
		const CExtRichStyleDescription & GetRefObject() const { __EXT_DEBUG_RICH_CONTENT_ASSERT( m_ptrObject != NULL ); return (*m_ptrObject); }
		CExtRichStyleDescription * & GetRefToPtrObject() { return m_ptrObject; }
		CExtRichStyleDescription * GetPtrAssignFrom() { return m_ptrAssignFrom; }
		const CExtRichStyleDescription * GetPtrAssignFrom() const { return m_ptrAssignFrom; }
		CExtRichStyleDescription & GetRefPtrAssignFrom() { __EXT_DEBUG_RICH_CONTENT_ASSERT( m_ptrAssignFrom != NULL ); return (*m_ptrAssignFrom); }
		const CExtRichStyleDescription & GetRefPtrAssignFrom() const { __EXT_DEBUG_RICH_CONTENT_ASSERT( m_ptrAssignFrom != NULL ); return (*m_ptrAssignFrom); }
		CExtRichStyleDescription * & GetRefToPtrAssignFrom() { return m_ptrAssignFrom; }
		void TakeControl( bool bTakeControl = true ) { m_bController = bTakeControl; }
		bool IsController() const { return ( m_bController && m_ptrObject != NULL ) ? true : false; }
		void Release()
		{
			if( m_ptrObject != NULL && m_bController )
				delete m_ptrObject;
			m_ptrObject = m_ptrAssignFrom = NULL;
			m_bController = false;
		}
		void Assign( const CExtRichStyleDescription * ptrObject = NULL, const CExtRichStyleDescription * ptrAssignFrom = NULL, bool bTakeControl = true )
		{
			if( ptrObject == m_ptrObject )
			{
				m_ptrAssignFrom = const_cast < CExtRichStyleDescription * > ( ptrAssignFrom );
				m_bController = bTakeControl;
				return;
			}
			Release();
			m_ptrObject = const_cast < CExtRichStyleDescription * > ( ptrObject );
			m_ptrAssignFrom = const_cast < CExtRichStyleDescription * > ( ptrAssignFrom );
			m_bController = bTakeControl;
		}
		void Assign( const ptr & other, bool bTakeControl = true )
		{
			Assign(
				const_cast < ptr & > ( other ).GetPtrObject(),
				const_cast < ptr & > ( other ).GetPtrAssignFrom(),
				( bTakeControl && other.IsController() ) ? true : false
				);
		}
		CExtRichStyleDescription * AutoInstantiate()
		{
			if( m_ptrObject != NULL )
				return m_ptrObject;
			m_ptrObject = new CExtRichStyleDescription;
			m_bController = true;
			if( m_ptrAssignFrom != NULL )
				m_ptrObject->Assign( * m_ptrAssignFrom );
			return m_ptrObject;
		}
		bool IsEmptyPtrObject() const { return ( m_ptrObject == NULL ) ? true : false; }
		bool IsEqualPtrObject( const CExtRichStyleDescription * other ) const { return ( m_ptrObject == other ) ? true : false; }
		ptr( const CExtRichStyleDescription * ptrObject = NULL, const CExtRichStyleDescription * ptrAssignFrom = NULL, bool bTakeControl = true )
			: m_ptrObject( NULL ), m_ptrAssignFrom( NULL ), m_bController( false ) { Assign( ptrObject, ptrAssignFrom, bTakeControl ); }
		ptr( const ptr & other, bool bTakeControl = true )
			: m_ptrObject( NULL ), m_ptrAssignFrom( NULL ), m_bController( false ) { Assign( other, bTakeControl ); }
		~ptr() { Release(); }
		ptr & operator = ( const ptr & other ) { Assign( other ); return (*this); } // takes control
		ptr & operator = ( const CExtRichStyleDescription * other ) { Assign( other ); return (*this); } // takes control
		operator CExtRichStyleDescription * () { return GetPtrObject(); }
		operator const CExtRichStyleDescription * () const { return GetPtrObject(); }
		CExtRichStyleDescription * operator -> () { return AutoInstantiate(); }
		const CExtRichStyleDescription * operator -> () const { return const_cast < ptr * > ( this ) -> AutoInstantiate(); }
		bool operator ! () const { return IsEmptyPtrObject(); }
		bool operator == ( const ptr & other ) const { return IsEqualPtrObject( other.GetPtrObject() ); }
		bool operator != ( const ptr & other ) const { return (! IsEqualPtrObject( other.GetPtrObject() ) ); }
		bool operator == ( const CExtRichStyleDescription * other ) const { return IsEqualPtrObject( other ); }
		bool operator != ( const CExtRichStyleDescription * other ) const { return (! IsEqualPtrObject( other ) ); }
	}; // class ptr
*/


}; /// CExtRichStyleDescription

/////////////////////////////////////////////////////////////////////////////
/// CExtRichStyleRule

class __PROF_UIS_API CExtRichStyleRule
{
public:
	int m_nFontWeightAbs; // 0 - not changed,  else 100...900, 400 - normal, 700 - bold
	int m_nFontWeightRel; // 0 - not changed,  else relative font weight changing
	int m_nItalic, m_nUnderline, m_nStrikeOut; // 0 - not changed, 1 - changed to bold/italic/strikeout, -1 - changed to normal
//	int m_nBlink, m_nOverline; // text decoration	0 - not changed, 1 - changed to Blink/Overline, -1 - changed to normal
	CExtSafeString m_strFontFaceName; // empty string - not changed
	int m_nFontSizeAbs; // 0 - not changed, else new font size
	int m_nFontSizeRel; // 0 - not changed, else relative shift of font size
	double m_lfFontSizePercentRel; // 1.0 - not changed, else increase/decrease %, e.g. 1.2 - increase 20%, 0.8 - decrease 20%
	CExtRichStyleDescription::e_align_horz_t m_eaH;
	CExtRichStyleDescription::e_align_vert_t m_eaV;
	CExtRichStyleDescription::e_text_transform_t m_etT;
	CExtRichStyleDescription::e_white_space_type_t m_eWST;
	CExtRichStyleDescription::e_background_repeat_t m_eBR;
	CExtRichStyleDescription::e_background_attachment_t m_eBA;
	CExtRichStyleDescription::e_background_position_type_t m_eBPT;
	CExtRichStyleDescription::e_background_postion_alignment_t m_eBPA;
	CExtRichStyleDescription::e_background_color_type_t m_eBCT;
	CExtRichStyleDescription::e_outline_style_t m_eOS;
	CExtRichStyleDescription::e_list_style_position_t m_eLSP;
	CExtRichStyleDescription::e_list_style_type_t m_eLST;
	CExtRichStyleDescription::e_display_t m_eD;
	CExtRichStyleDescription::e_ui_dock_t m_eDock;
	CExtRichStyleDescription::e_visibility_t m_eV;
	CExtRichStyleDescription::e_position_t m_eP;
	CExtRichStyleDescription::e_overflow_t m_eO;
	CExtRichStyleDescription::e_scroll_bar_type_t m_esbtH, m_esbtV;
	CExtRichStyleDescription::e_float_t m_eF;
	CExtRichStyleDescription::e_cursor_t m_eC;
	CExtRichStyleDescription::e_clip_image_t m_eCI;
//	CExtRichStyleDescription::e_clear_sides_t m_eCS;
	CExtRichStyleDescription::e_z_index_type_t m_eZIT;
//	CExtRichStyleDescription::e_page_break_before_after_t m_ePBB, m_ePBA;
	CExtRichStyleDescription::e_border_collapse_t m_eBC;
	CExtRichStyleDescription::e_caption_side_type_t m_eCST;
	CExtRichStyleDescription::e_empty_cells_t m_eEC;
	CExtRichStyleDescription::e_table_layout_t m_eTL;
	CExtRichStyleDescription::e_noshade_t m_eN;
	CExtRichStyleDescription::e_ui_list_navigation_t m_eUiListNavigation;
	CExtRichStyleDescription::e_ui_drawmethod_t m_eUiDrawMethod, m_eUiBackgroundDrawMethod;
	CExtRichStyleDescription::e_ui_yes_not_option_t m_eUiFocusEnsureVisibility;
	CExtRichStyleDescription::e_ui_delayed_draw_t m_eUiDelayedDraw;
	CExtRichStyleDescription::e_ui_sb_type_t m_eUiSBT;
	CPoint m_ptBackgroundAlignment;
	LONG m_nParaIndent; // -1 - do not change style's paragraph indent
	CRect m_rcPadding, m_rcBorder, m_rcMargin, m_rcCellPadding;
	CRect m_rcClipImageShape;
	CRect m_rcUiSlice, m_rcUiBackgroundSlice, m_rcUiBackgroundExtend;
	CSize m_sizeCellSpacing;
	CSize m_sizeWH; // explicitly specified width and/or height tag attributes, <= 0 - not specified and must not be changed
	CSize m_sizeWHP; // percent version of m_sizeWH
	CSize m_sizeSpan; // colspan/rowspan values, must be >= 1 if valid
	COLORREF m_clrText, m_clrBack; // COLORREF(-1L) - do not change, COLORREF(-2L) - restore to transparent COLORREF(-1L) color
	COLORREF m_clrBorderLeft, m_clrBorderRight, m_clrBorderTop, m_clrBorderBottom;
	CExtRichStyleDescription::e_border_style_t m_ebsLeft, m_ebsRight, m_ebsTop, m_ebsBottom;
	LONG m_nFirstNumberInList; // <OL>, <UL>, anything else supported too - first number in list, negative - do not change
	int m_nUi_transparency, m_nUi_background_transparency;
	CExtSafeString m_strUnNumberedListSign; // <UL> - list item sign, empty - do not change
	CExtSafeString m_strBackgroundImagePath;
	CExtSafeString m_strListStyleImagePath;
	CExtSafeString m_strCursorImagePath;
	CExtSafeString m_strContent;
//	CExtSafeString m_strCounterIncrement, m_strCounterReset;
	LONG m_nLineSpacingPX;	// -1 not used
	DOUBLE m_lfLineSpacingPercent;	// default 1.0, 1.0 is 100 %
	LONG m_nWordSapcing, m_nLetterSpacing;	// -1 not used
//	CExtSafeStringArray m_arrFontNames;
	INT m_nZIndex;
	LONG m_nOutlineWidth;
	COLORREF m_clrOutlineColor;
	LONG m_nMaxHeight, m_nMaxWidth, m_nMinHeight, m_nMinWidth;	// -1 not used; m_nMinHeight, m_nMinWidth - default value is 0
	LONG m_nSpacerModeMinWidth; // space allocation mode
	DOUBLE m_lfMaxHeightP, m_lfMaxWidthP, m_lfMinHeightP, m_lfMinWidthP;	// percent version of m_nMaxHeight, m_nMaxWidth, m_nMinHeight, m_nMinWidth
	CRect m_rcOuter, m_rcOuterP;
	CList < CExtRichStyleDescription::CQuoutes, CExtRichStyleDescription::CQuoutes > m_listQuotes;
	LONG m_nBorderSpacingHoriz, m_nBorderSpacingVert;
	mutable CExtSafeString m_strContentBefore, m_strContentAfter;
	CExtRichStyleDescription::pseudo_class_def_t m_PCD; // supported element states
	CExtRichStyleDescription::e_ui_yes_not_option_t m_eynFocusOnMakeSelected, m_eynFocusOnMakeUnSelectedSibling, m_eynFocusOffMakeUnSelected, m_eynFocusOffMakeUnSelectedSibling;

	CExtRichStyleRule();
	CExtRichStyleRule( const CExtRichStyleRule & other );
	virtual ~CExtRichStyleRule();

	virtual void Assign( const CExtRichStyleRule & other );
	virtual void Merge( const CExtRichStyleRule & other );
	virtual bool IsEmpty() const;
	virtual void Empty();
	virtual void GetStyle( const CExtRichStyleDescription & styleSrc, CExtRichStyleDescription & styleDst );

	CExtRichStyleRule & operator = ( const CExtRichStyleRule & other ) { Assign( other ); return (*this); }

	bool ewstIsSingleSpaceMode() const { return CExtRichStyleDescription::stat_ewstIsSingleSpaceMode( m_eWST ); }
	bool ewstIsNewLineMode()     const { return CExtRichStyleDescription::stat_ewstIsNewLineMode    ( m_eWST ); }
	bool ewstIsLineWrapMode()    const { return CExtRichStyleDescription::stat_ewstIsLineWrapMode   ( m_eWST ); }
}; /// CExtRichStyleRule

/////////////////////////////////////////////////////////////////////////////
/// CExtRichTableRow

class __PROF_UIS_API CExtRichTableRow : public CExtArray < CExtRichContentItem * >
{
public:
	CExtRichContentItem * m_pRCI_TableRow; // <tr> or <th> node
	LONG m_nRowHeightCur, m_nRowHeightMin;

	CExtRichTableRow();
	virtual ~CExtRichTableRow();
	void Init( LONG nColCount );
}; /// CExtRichTableRow

/////////////////////////////////////////////////////////////////////////////
/// CExtRichTableLayout

class __PROF_UIS_API CExtRichTableLayout :  public CExtArray < CExtRichTableRow * >
{
public:
	CExtArray < DOUBLE > m_arrWidthPrecents;
	CExtArray < LONG > m_arrCWcur, m_arrCWmin; // column widths
	CArray < CSize, CSize & > m_arrSIA;

	CExtRichTableLayout();
	virtual ~CExtRichTableLayout();

	void Init( LONG nColCount, LONG nRowCount );
	void Init( const SIZE & sizeDim );
	void Empty();
	LONG GetMeasuredWidthMin() const;
	LONG GetMeasuredWidthMax() const;
	CExtRichTableRow * InstantiateAt( LONG nRowNo );
	LONG GetColCount() const;
	LONG GetRowCount() const;
	CSize GetDim() const;
	void SniffTable(
		CExtRichContentItem * pTableLevelRCI,
		CSize & sizeDim,
		CList < CPoint, CPoint & > & listSpanRoots,
		CList < CSize, CSize & > & listSpanSizes,
		CList < CExtRichContentItem *, CExtRichContentItem * & > & listSpanItems,
		CTypedPtrList < CPtrList, CExtRichContentItem * > & listRowItems
		);
	void SniffTableLevel(
		CExtRichContentItem * pTableLevelRCI,
		CSize & sizeDim,
		CList < CPoint, CPoint & > & listSpanRoots,
		CList < CSize, CSize & > & listSpanSizes,
		CList < CExtRichContentItem *, CExtRichContentItem * & > & listSpanItems,
		CTypedPtrList < CPtrList, CExtRichContentItem * > & listRowItems
		);

	void SpanInfoArray_Init();
	void SpanInfoArray_Empty();
	SIZE & SpanInfoArray_GetAt( INT nX, INT nY );
	const SIZE & SpanInfoArray_GetAt( INT nX, INT nY ) const;
	void SpanInfoArray_SetAt( INT nX, INT nY, const SIZE & sizeSpan );
	INT SpanInfoArray_GetSpanTypeAt( INT nX, INT nY ) const; // returns: 0 - 1x1 independent, 1 - span root, -1 - covered
	CRect GetPartAreaRect(
		INT nColNoFrom, INT nRowNoFrom,
		INT nColNoTo, INT nRowNoTo,
		bool bCurrent // true - current pre-computed size, false - minimal pre-computed size
		) const;
}; /// CExtRichTableLayout


/////////////////////////////////////////////////////////////////////////////
/// CExtRichContentHitTestInfo

#define __ERCHT_BASE_SB_LINE_UP					0x00000001
#define __ERCHT_BASE_SB_LINE_DOWN				0x00000002
#define __ERCHT_BASE_SB_PAGE_UP					0x00000003
#define __ERCHT_BASE_SB_PAGE_DOWN				0x00000004
#define __ERCHT_BASE_SB_THUMB					0x00000005
#define __ERCHT_BASE_SB_MASK					0x0000000F

#define __ERCHT_BASE_ON_ITEM_PART_LEFT			0x00000000
#define __ERCHT_BASE_ON_ITEM_PART_RIGHT			0x00000001
#define __ERCHT_BASE_ON_ITEM_PART_TOP			0x00000002
#define __ERCHT_BASE_ON_ITEM_PART_BOTTOM		0x00000003
#define __ERCHT_BASE_ON_ITEM_PART_MASK			0x00000003
#define __ERCHT_BASE_ON_ITEM_PART_LT			(__ERCHT_BASE_ON_ITEM_PART_LEFT |__ERCHT_BASE_ON_ITEM_PART_TOP)
#define __ERCHT_BASE_ON_ITEM_PART_RT			(__ERCHT_BASE_ON_ITEM_PART_RIGHT|__ERCHT_BASE_ON_ITEM_PART_TOP)
#define __ERCHT_BASE_ON_ITEM_PART_LB			(__ERCHT_BASE_ON_ITEM_PART_LEFT |__ERCHT_BASE_ON_ITEM_PART_BOTTOM)
#define __ERCHT_BASE_ON_ITEM_PART_RB			(__ERCHT_BASE_ON_ITEM_PART_RIGHT|__ERCHT_BASE_ON_ITEM_PART_BOTTOM)
#define __ERCHT_BASE_ON_ITEM_PART_ANY			__ERCHT_BASE_ON_ITEM_PART_MASK

// flags for CExtRichContentHitTestInfo::m_dwHtFlags

#define __ERCHT_NOWHERE							0x00000000

#define __ERCHT_SBV_SB_SHIFT					0
#define __ERCHT_SBV_SB_MASK						(__ERCHT_BASE_SB_MASK<<__ERCHT_SBV_SB_SHIFT)
#define __ERCHT_SBV_SB_LINE_UP					(__ERCHT_BASE_SB_LINE_UP<<__ERCHT_SBV_SB_SHIFT)
#define __ERCHT_SBV_SB_LINE_DOWN				(__ERCHT_BASE_SB_LINE_DOWN<<__ERCHT_SBV_SB_SHIFT)
#define __ERCHT_SBV_SB_PAGE_UP					(__ERCHT_BASE_SB_PAGE_UP<<__ERCHT_SBV_SB_SHIFT)
#define __ERCHT_SBV_SB_PAGE_DOWN				(__ERCHT_BASE_SB_PAGE_DOWN<<__ERCHT_SBV_SB_SHIFT)
#define __ERCHT_SBV_SB_THUMB					(__ERCHT_BASE_SB_THUMB<<__ERCHT_SBV_SB_SHIFT)

#define __ERCHT_SBH_SB_SHIFT					4
#define __ERCHT_SBH_SB_MASK						(__ERCHT_BASE_SB_MASK<<__ERCHT_SBH_SB_SHIFT)
#define __ERCHT_SBH_SB_LINE_UP					(__ERCHT_BASE_SB_LINE_UP<<__ERCHT_SBH_SB_SHIFT)
#define __ERCHT_SBH_SB_LINE_DOWN				(__ERCHT_BASE_SB_LINE_DOWN<<__ERCHT_SBH_SB_SHIFT)
#define __ERCHT_SBH_SB_PAGE_UP					(__ERCHT_BASE_SB_PAGE_UP<<__ERCHT_SBH_SB_SHIFT)
#define __ERCHT_SBH_SB_PAGE_DOWN				(__ERCHT_BASE_SB_PAGE_DOWN<<__ERCHT_SBH_SB_SHIFT)
#define __ERCHT_SBH_SB_THUMB					(__ERCHT_BASE_SB_THUMB<<__ERCHT_SBH_SB_SHIFT)

#define __ERCHT_ON_ANY_SCROLL_BAR				(__ERCHT_SBV_SB_MASK|__ERCHT_SBH_SB_MASK)

#define __ERCHT_ON_ITEM_INNER_PADDING_SHIFT		8
#define __ERCHT_ON_ITEM_PADDING_LEFT			(__ERCHT_BASE_ON_ITEM_PART_LEFT<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_RIGHT			(__ERCHT_BASE_ON_ITEM_PART_RIGHT<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_TOP				(__ERCHT_BASE_ON_ITEM_PART_TOP<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_BOTTOM			(__ERCHT_BASE_ON_ITEM_PART_BOTTOM<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_MASK			(__ERCHT_BASE_ON_ITEM_PART_MASK<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_LT				(__ERCHT_BASE_ON_ITEM_PART_LT<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_RT				(__ERCHT_BASE_ON_ITEM_PART_RT<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_LB				(__ERCHT_BASE_ON_ITEM_PART_LB<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_RB				(__ERCHT_BASE_ON_ITEM_PART_RB<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)
#define __ERCHT_ON_ITEM_PADDING_ANY				(__ERCHT_BASE_ON_ITEM_PART_ANY<<__ERCHT_ON_ITEM_INNER_PADDING_SHIFT)

#define __ERCHT_ON_ITEM_INNER_BORDER_SHIFT		10
#define __ERCHT_ON_ITEM_BORDER_LEFT				(__ERCHT_BASE_ON_ITEM_PART_LEFT<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_RIGHT			(__ERCHT_BASE_ON_ITEM_PART_RIGHT<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_TOP				(__ERCHT_BASE_ON_ITEM_PART_TOP<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_BOTTOM			(__ERCHT_BASE_ON_ITEM_PART_BOTTOM<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_MASK				(__ERCHT_BASE_ON_ITEM_PART_MASK<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_LT				(__ERCHT_BASE_ON_ITEM_PART_LT<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_RT				(__ERCHT_BASE_ON_ITEM_PART_RT<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_LB				(__ERCHT_BASE_ON_ITEM_PART_LB<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_RB				(__ERCHT_BASE_ON_ITEM_PART_RB<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)
#define __ERCHT_ON_ITEM_BORDER_ANY				(__ERCHT_BASE_ON_ITEM_PART_ANY<<__ERCHT_ON_ITEM_INNER_BORDER_SHIFT)

#define __ERCHT_ON_ITEM_INNER_MARGIN_SHIFT		12
#define __ERCHT_ON_ITEM_MARGIN_LEFT				(__ERCHT_BASE_ON_ITEM_PART_LEFT<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_RIGHT			(__ERCHT_BASE_ON_ITEM_PART_RIGHT<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_TOP				(__ERCHT_BASE_ON_ITEM_PART_TOP<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_BOTTOM			(__ERCHT_BASE_ON_ITEM_PART_BOTTOM<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_MASK				(__ERCHT_BASE_ON_ITEM_PART_MASK<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_LT				(__ERCHT_BASE_ON_ITEM_PART_LT<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_RT				(__ERCHT_BASE_ON_ITEM_PART_RT<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_LB				(__ERCHT_BASE_ON_ITEM_PART_LB<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_RB				(__ERCHT_BASE_ON_ITEM_PART_RB<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)
#define __ERCHT_ON_ITEM_MARGIN_ANY				(__ERCHT_BASE_ON_ITEM_PART_ANY<<__ERCHT_ON_ITEM_INNER_MARGIN_SHIFT)

#define __ERCHT_ON_ITEM_ANY_EXT_AREA			(__ERCHT_ON_ITEM_MARGIN_ANY|__ERCHT_ON_ITEM_BORDER_ANY|__ERCHT_ON_ITEM_PADDING_ANY)

#define __ERCHT_ON_SB_CORNER					0x00004000

#define __ERCHT_ON_SBH_DISABLED					0x00010000
#define __ERCHT_ON_SBV_DISABLED					0x00020000
#define __ERCHT_ON_SB_DISABLED					(__ERCHT_ON_SBH_DISABLED|__ERCHT_ON_SBV_DISABLED)

#define __ERCHT_ON_ITEM_OUTER_AREA				(__ERCHT_ON_ITEM_ANY_EXT_AREA|__ERCHT_ON_ANY_SCROLL_BAR|__ERCHT_ON_SB_CORNER|__ERCHT_ON_SB_DISABLED)
#define __ERCHT_ON_ITEM_INNER_AREA				0x00008000

#define __ERCHT_ON_ITEM_ANYWHERE				(__ERCHT_ON_ITEM_OUTER_AREA|__ERCHT_ON_ITEM_INNER_AREA)

class __PROF_UIS_API CExtRichContentHitTestInfo
{
public:
	struct __PROF_UIS_API ht_item_t
	{
		CExtRichContentItem * m_pRCI;
		DWORD m_dwHtFlags;
		CRect m_rcOuterParts[4]; // in case of margin, border and padding: 0-top, 1-bottom, 2-left, 3-right
		CRect m_rcPart; // in case of scroll bar: specifies entire scroll bar area; in case of inner area: specifies inner element area
	protected:
		void _EmptyRects()
		{
			m_rcPart.SetRect( 0, 0, 0, 0 );
			for( int i = 0; i < (sizeof(m_rcOuterParts)/sizeof(m_rcOuterParts[0])); i++ ) m_rcOuterParts[i].SetRect( 0, 0, 0, 0 );
		}
		void _Assign( const ht_item_t & other )
		{
			m_pRCI = other.m_pRCI; m_dwHtFlags = other.m_dwHtFlags; m_rcPart = other.m_rcPart;
			for( int i = 0; i < (sizeof(m_rcOuterParts)/sizeof(m_rcOuterParts[0])); i++ ) m_rcOuterParts[i] = other.m_rcOuterParts[i];
		}
		bool _IsEqual( const ht_item_t & other ) const
		{
			if( LPCVOID(m_pRCI) != LPCVOID(other.m_pRCI) || m_dwHtFlags != other.m_dwHtFlags || m_rcPart != other.m_rcPart )
				return false;
			for( int i = 0; i < (sizeof(m_rcOuterParts)/sizeof(m_rcOuterParts[0])); i++ )
				{ if( m_rcOuterParts[i] != other.m_rcOuterParts[i] ) return false; }
			return true;
		}
	public:
		ht_item_t() : m_pRCI( NULL ), m_dwHtFlags( __ERCHT_NOWHERE ) { _EmptyRects(); }
		ht_item_t( CExtRichContentItem * pRCI, DWORD dwHtFlags ) : m_pRCI( pRCI ), m_dwHtFlags( dwHtFlags ) { _EmptyRects(); }
		ht_item_t( const ht_item_t & other ) : m_pRCI( NULL ), m_dwHtFlags( __ERCHT_NOWHERE ) { _EmptyRects(); _Assign( other ); }
		ht_item_t & operator = ( const ht_item_t & other ) { _Assign( other ); return (*this); }
		bool operator == ( const ht_item_t & other ) const { return _IsEqual( other ); }
		bool operator != ( const ht_item_t & other ) const { return _IsEqual( other ) ? false : true; }
	};
	CPoint m_ptHT, m_ptInitialHT;
	CExtRichContentItem * m_pStartHtRCI;
	CList < ht_item_t, ht_item_t & > m_listHtBranch;
	CExtRichContentHitTestInfo();
	CExtRichContentHitTestInfo( const POINT & ptHT, CExtRichContentItem * pStartHtRCI );
	CExtRichContentHitTestInfo( const POINT & ptHT, CExtRichContentLayout & _layout );
	CExtRichContentHitTestInfo( const CExtRichContentHitTestInfo & other );
	void Empty();
	bool IsEmpty() const;
	void HitTest();
	void Assign( const CExtRichContentHitTestInfo & other );
	bool IsEqual( const CExtRichContentHitTestInfo & other ) const;
	CExtRichContentHitTestInfo & operator = ( const CExtRichContentHitTestInfo & other );
	bool operator == ( const CExtRichContentHitTestInfo & other ) const;
	bool operator != ( const CExtRichContentHitTestInfo & other ) const;
}; /// CExtRichContentHitTestInfo

/////////////////////////////////////////////////////////////////////////////
/// CExtRichContentItem

class __PROF_UIS_API CExtRichContentItem
{
public:
	CExtRichContentItem * m_pParent, * m_pPrev, * m_pNext;
	mutable HWND m_hWndAttached;
	enum e_html_item_type_t
	{
		// root item, just a container
		__EHIT_ROOT = -1,
		// text items
		__EHIT_TEXT_RANGE = 0,
		__EHIT_TEXT_SPACE = 1,
		// any tag
		__EHIT_GENERIC_TAG = 2,
		// begin of specific internal nodes
		__EHIT_SPEC_START = 3,
		// parameters of any tag
		__EHIT_PARAMETER_BASIC = 3,
		__EHIT_PARAMETER_CSS_CONTAINER = 4,
		__EHIT_PARAMETER_CSS_VALUE = 5,
		// CSS classes tree node
		__EHIT_CLASS_NODE = 6,
		// special items, for storing only
		__EHIT_SPECIAL_FIRST = 7,
		__EHIT_TITLE = 7,
		__EHIT_STYLE = 8,
		__EHIT_SCRIPT = 9,
		__EHIT_META = 10,
		__EHIT_LINK = 11,
		__EHIT_SPECIAL_LAST = 11,
	};
	enum e_html_item_error_t
	{
		__EHIE_OK = 0,
		__EHIE_NOT_MATCHING_CLOSE_TAG = 1,
		__EHIE_INVALID_TAG_PARAMETERS = 2,
		__EHIE_NOT_CLOSED_FINALLY = 3,
		__EHIE_INVALID_PARAMETER_NAME = 4,
		__EHIE_INVALID_TEXT = 5,
		__EHIE_FAILED_TO_LOAD_LINKED_CSS = 6,
	};

	struct __PROF_UIS_API span_info_t
	{
		CPoint m_ptSpan;
		CSize m_sizeSpan;
		CExtRichContentItem * m_pRCI;
		span_info_t()
			: m_ptSpan( 0, 0 )
			, m_sizeSpan( 0, 0 )
			, m_pRCI( NULL )
		{
		}
		span_info_t( const span_info_t & other )
		{
			m_ptSpan = other.m_ptSpan;
			m_sizeSpan = other.m_sizeSpan;
			m_pRCI = other.m_pRCI;
		}
		span_info_t & operator = ( const span_info_t & other )
		{
			m_ptSpan = other.m_ptSpan;
			m_sizeSpan = other.m_sizeSpan;
			m_pRCI = other.m_pRCI;
			return (*this);
		}
/*
		bool operator < ( const span_info_t & other ) const
		{
		//	if( m_ptSpan.y < other.m_ptSpan.y ) return true;
		//	if( m_ptSpan.y > other.m_ptSpan.y ) return false;
		//	if( m_ptSpan.x < other.m_ptSpan.x ) return true;
		//	return false;
			if( m_ptSpan.x < other.m_ptSpan.x ) return true;
			if( m_ptSpan.x > other.m_ptSpan.x ) return false;
			if( m_ptSpan.y < other.m_ptSpan.y ) return true;
			return false;
		}
*/
		int CompareWithOther( const span_info_t & other ) const
		{
			if( m_ptSpan.x < other.m_ptSpan.x ) return -1;
			if( m_ptSpan.x > other.m_ptSpan.x ) return  1;
			if( m_ptSpan.y < other.m_ptSpan.y ) return -1;
			if( m_ptSpan.y > other.m_ptSpan.y ) return  1;
//			if( m_sizeSpan.cx < other.m_sizeSpan.cx ) return -1;
//			if( m_sizeSpan.cx > other.m_sizeSpan.cx ) return  1;
//			if( m_sizeSpan.cy < other.m_sizeSpan.cy ) return -1;
//			if( m_sizeSpan.cy > other.m_sizeSpan.cy ) return  1;
			return 0;
		}
		bool operator == (const span_info_t & other ) const { return ( CompareWithOther( other ) == 0 ) ? true : false; }
		bool operator != (const span_info_t & other ) const { return ( CompareWithOther( other ) != 0 ) ? true : false; }
		bool operator <= (const span_info_t & other ) const { return ( CompareWithOther( other ) <= 0 ) ? true : false; }
		bool operator >= (const span_info_t & other ) const { return ( CompareWithOther( other ) >= 0 ) ? true : false; }
		bool operator <  (const span_info_t & other ) const { return ( CompareWithOther( other ) <  0 ) ? true : false; }
		bool operator >  (const span_info_t & other ) const { return ( CompareWithOther( other ) >  0 ) ? true : false; }
	}; /// span_info_t

	struct __PROF_UIS_API span_info_array_t : public CExtArray < span_info_t >
	{
		span_info_array_t() { }
		span_info_array_t(
			CList < CPoint, CPoint & > & listSpanRoots,
			CList < CSize, CSize & > & listSpanSizes,
			CList < CExtRichContentItem *, CExtRichContentItem * & > & listSpanItems
			)
		{
			__EXT_DEBUG_RICH_CONTENT_ASSERT( INT( listSpanRoots.GetCount() ) == INT( listSpanSizes.GetCount() ) );
			__EXT_DEBUG_RICH_CONTENT_ASSERT( INT( listSpanRoots.GetCount() ) == INT( listSpanItems.GetCount() ) );
			SetSize( INT( listSpanRoots.GetCount() ) );
			POSITION posSpanRoot = listSpanRoots.GetHeadPosition();
			POSITION posSpanSize = listSpanSizes.GetHeadPosition();
			POSITION posSpanItem = listSpanItems.GetHeadPosition();
			for( INT nIndex = 0; posSpanRoot != NULL; nIndex ++ )
			{
				span_info_t _si;
				_si.m_ptSpan = listSpanRoots.GetNext( posSpanRoot );
				_si.m_sizeSpan = listSpanSizes.GetNext( posSpanSize );
				_si.m_pRCI = listSpanItems.GetNext( posSpanItem );
				__EXT_DEBUG_RICH_CONTENT_ASSERT( _si.m_sizeSpan.cx > 1 || _si.m_sizeSpan.cy > 1 );
				SetAt( nIndex, _si );
			}
			QuickSort();
		}
	}; /// span_info_array_t

	struct __PROF_UIS_API span_info_array_ptr_t
	{
		span_info_array_t * m_ptr;
		span_info_array_ptr_t( span_info_array_t * ptr = NULL ) : m_ptr( ptr ) { }
		~span_info_array_ptr_t() { if( m_ptr != NULL ) delete m_ptr; }
		span_info_array_ptr_t & operator = ( span_info_array_t * ptr ) { m_ptr = ptr; return (*this); }
		operator span_info_array_t * () { return m_ptr; }
		span_info_array_t * operator -> () { return m_ptr; }
		bool IsEmpty() const { return ( m_ptr == NULL ) ? true : false; }
	}; /// span_info_array_ptr_t

	e_html_item_type_t m_eType;
	e_html_item_error_t m_eError;
	CExtSafeString m_strTextParsed, m_strTextEffective, m_strTagPV;
	bool m_bClosingTag:1, m_bSingleTagMode:1, m_bDynamicTagFromParseMode:1;
	CTypedPtrList < CPtrList, CExtRichContentItem * > m_listItems; // children
	CTypedPtrList < CPtrList, CExtRichContentItem * > m_listParms; // styles/parameters
	CTypedPtrList < CPtrList, CExtRichContentItem * > m_listPCD; // CSS pseudo-classes
	CTypedPtrList < CPtrList, CExtRichContentItem * > m_listDS, m_listDF; // dock-side, dock-fill children
	CRect m_rcLayout;
	CSize m_sizePreCalc, m_sizeComputedSpan, m_sizeMaxChild;
	mutable LONG m_nHelperTmAscent, m_nHelperTmInternalLeading, m_nZeroBasedListIndex, m_nAppliedListIndex, m_nListNumberWidth;
	bool m_bHelperWordStartItem:1, m_bDynamicItemBefore:1, m_bDynamicItemAfter:1, m_bDynamicItemList:1, m_bMsWordCssWA:1;
	CExtRichTableLayout * m_pTable;
	CExtRichTableLayout & _GetTable();
	CExtRichContentLayout & m_layout;
	CExtRichStyleDescription::pseudo_class_def_t m_PCD; // current element states in case of HTML node, CSS pseudo class definition in case of CSS class node
protected:
	bool m_bHaveStyleChangingCache:1;
	CExtRichStyleRule * m_pStyleChangingCache;
	bool m_bHaveEffectiveStyleCache:1;
	CExtRichStyleDescription * m_pStyleEffective;
public:
	static const CExtRichStyleRule g_styleRuleEmpty;
	static const CExtRichStyleDescription g_styleEmpty;
	virtual bool _IsStyleOwner() const;
	virtual void ResetStyleInfo(
		bool bDeep = true,
		bool bResetStyleDefinition = true,
		bool bResetStyleRule = true
		);
	virtual CExtRichStyleRule & _GetStyleChangingCache();
	const CExtRichStyleRule & _GetStyleChangingCache() const { return ( const_cast < CExtRichContentItem * > ( this ) ) -> _GetStyleChangingCache(); }
	virtual CExtRichStyleDescription & GetEffectiveStyle();
	const CExtRichStyleDescription & GetEffectiveStyle() const;
	CExtRichStyleDescription::e_display_t GetDisplay() const;
	CExtRichStyleDescription::e_visibility_t GetVisibility() const;
	CExtRichStyleDescription::e_position_t GetPosition() const;
protected:
	virtual bool _CheckParentChainFor_IsParticipantAPIs( bool bSimpleCheck, bool bAcceptPartialVisibility ) const;
public:
	virtual bool IsParticipantOfLayout(
		bool bCheckParentChain = false
		) const;
	virtual bool IsParticipantOfPaint(
		bool bCheckParentChain = false
		) const;
	virtual bool IsParticipantOfHitTest(
		bool bCheckParentChain = false,
		bool bAcceptPartialVisibility = true
		) const;
	virtual bool IsParticipantOfFocus(
		bool bCheckParentChain = true
		) const;
	CPoint m_ptScrollPos;
	CSize m_sizeOverflow;
	CExtSafeStringList * m_pListClassNames;
protected:
	mutable bool m_bIsNoLayoutRectItem_Have:1;
	mutable bool m_bIsNoLayoutRectItem_Val:1;
	mutable bool m_bParagraphItem_Have:1;
	mutable bool m_bParagraphItem_Val:1;
	mutable bool m_bIsSingleItem_Have:1;
	mutable bool m_bIsSingleItem_Val:1;
	mutable bool m_bIsNoWrapSequence_Have:1;
	mutable bool m_bIsNoWrapSequence_Val:1;
	void _ClearHaveFlags();
	mutable bool m_bEwstProcessingComplete:1;
	CExtSafeString m_strElementID, m_strElementName, m_strElementUiBindingSrcID, m_strElementUiBindingDstID;
public:
	static bool stat_IsCharOneOf( __EXT_MFC_SAFE_TCHAR _tchr, __EXT_MFC_SAFE_LPCTSTR str );
	static void stat_FillSolidRect( CDC & dc, const RECT & rc, COLORREF clrFill );
public:
	// plain text helpers
	CSize m_thSizeWH;
	bool m_thUnderline:1, m_thDtPrefixOnlyMode:1;
	// scroll bar tracking states
	bool m_bHorzScrollBarTracking:1;
	INT m_nSBMHT;
	bool m_bSbProcessingClick:1, m_bSbProcessingHover:1, m_bSbProcessingOutClick:1, m_bSbProcessingHorzBar:1;

	class __PROF_UIS_API _StyleRestore_t
	{
	public:
		CDC & m_dc;
		HGDIOBJ m_hOldFont;
		HFONT m_hFont;
		TEXTMETRIC m_TextMetric;
		COLORREF m_clrText, m_clrBack; // restore colors
		int m_nBkMode; // restore background mode
		INT m_nLO;
		_StyleRestore_t( CDC & dc, const CExtRichStyleDescription & style, INT nLO, bool bPlainTextUnderlineMode );
		~_StyleRestore_t();
	};
	class __PROF_UIS_API _RelativePosionShift_t
	{
	public:
		CDC & m_dc;
		const CExtRichContentItem * m_pRCI;
		const CExtRichStyleDescription & m_style;
		CSize m_sizeOffset;
		_RelativePosionShift_t( CDC & dc, const CExtRichContentItem * pRCI, const CExtRichStyleDescription & style );
		~_RelativePosionShift_t();
	};

	CExtRichContentItem(
		CExtRichContentLayout & _layout,
		e_html_item_type_t eType,
		CExtRichContentItem * pParent = NULL,
		CExtRichContentItem * pPrev = NULL,
		CExtRichContentItem * pNext = NULL
		);
	virtual ~CExtRichContentItem();

	virtual CExtRichContentItem * BrowseNext();
	const CExtRichContentItem * BrowseNext() const { return ( const_cast < CExtRichContentItem * > ( this ) ) -> BrowseNext(); }
	virtual CExtRichContentItem * BrowsePrev();
	const CExtRichContentItem * BrowsePrev() const { return ( const_cast < CExtRichContentItem * > ( this ) ) -> BrowsePrev(); }

	virtual void ChangeStyle( __EXT_MFC_SAFE_LPCTSTR strStyle );

	virtual bool RciEvent_ResetElementStateInfo(
		HWND hWndContentViewer,
		const CExtRichStyleDescription::pseudo_class_def_t & statesToReset,
		CExtRichStyleDescription::e_ui_yes_not_option_t eynSetOn,
		bool bDeep = true
		);
	virtual bool RciEvent_OnKey(
		bool bSysKey,
		bool bKeyDownEvent, // true - key-down event, false - key-up event
		UINT nChar, // virtual key code
		UINT nRepCnt, // key-down/key-up press count
		UINT nFlags, // key-down/key-up event flags
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnSelect(
		HWND hWndContentViewer,
		bool bSelected
		);
	virtual bool RciEvent_OnDisable(
		HWND hWndContentViewer,
		bool bDisabled
		);
	virtual bool RciEvent_OnFocusEnter(
		HWND hWndContentViewer,
		CExtRichContentItem * pOldFocusRCI
		);
	virtual bool RciEvent_OnFocusLeave(
		HWND hWndContentViewer,
		CExtRichContentItem * pNewFocusRCI
		);
	virtual bool RciEvent_OnMouseWheel(
		UINT fFlags,
		short zDelta,
		CPoint point,
		const CExtRichContentHitTestInfo & htInfoHover,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnMouseEnter(
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnMouseLeave(
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnPressedEnter(
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnPressedLeave(
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnClick(
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnDoubleClick(
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual CExtRichStyleDescription::e_ui_sb_type_t RciEvent_Scrolling_GetDefaultSBT() const;

	virtual bool HaveAffectedCssPseudoClasses( const CExtRichStyleDescription::pseudo_class_def_t & statesToCheck ) const;
	virtual void ResetElementStateInfo(
		const CExtRichStyleDescription::pseudo_class_def_t & statesToReset,
		CExtRichStyleDescription::e_ui_yes_not_option_t eynSetOn,
		bool bDeep = true
		);
	virtual void _ResetElementStateInfoImpl(
		const CExtRichStyleDescription::pseudo_class_def_t & statesToReset,
		CExtRichStyleDescription::e_ui_yes_not_option_t eynSetOn,
		bool bDeep = true
		);
	virtual CRect CalcEffectiveRect() const;
	virtual bool _DoHitTestImpl_Basic( CExtRichContentHitTestInfo & htInfo, CExtRichContentItem * pRCIFA );
	virtual bool _DoHitTestImpl_ScrollingAdjusted( CExtRichContentHitTestInfo & htInfo, CExtRichContentItem * pRCIFA );
	virtual void _DoHitTestImpl_PostAdjustment( CExtRichContentHitTestInfo & htInfo, CExtRichContentHitTestInfo::ht_item_t & _ht_item, CExtRichContentItem * pRCIFA );
	virtual INT GetScrollBarStepSize( bool bHorz );
	virtual void _DoScroll(
		CPoint ptNewScrollPos,
		HWND hWndContentViewer = NULL
		);

	virtual void ElementUiBindingSrcIdSet( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingSrcID );
	virtual __EXT_MFC_SAFE_LPCTSTR ElementUiBindingSrcIdGet() const;

	virtual void ElementUiBindingDstIdSet( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingDstID );
	virtual __EXT_MFC_SAFE_LPCTSTR ElementUiBindingDstIdGet() const;

	virtual void ElementIdSet( __EXT_MFC_SAFE_LPCTSTR strElementID );
	virtual __EXT_MFC_SAFE_LPCTSTR ElementIdGet() const;

	virtual void ElementNameSet( __EXT_MFC_SAFE_LPCTSTR strElementName );
	virtual __EXT_MFC_SAFE_LPCTSTR ElementNameGet() const;

	LONG GetHelperTmAscent() const;
	LONG GetHelperTmInternalLeading() const;
	void RemoveValueSpaces();

	static bool stat_FontParmIsEmptyOrInherit( __EXT_MFC_SAFE_LPCTSTR strFontFace );
	static bool stat_FontParmIsDesktopFontReference( __EXT_MFC_SAFE_LPCTSTR strFontFace );

	virtual CExtSafeString QueryGenericTagName() const;
	virtual void QueryStyleChanging( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent );

	static void stat_html_parm_handler_class( CExtRichContentItem *, CExtRichContentItem *, CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_ui_binding_id_src( CExtRichContentItem *, CExtRichContentItem *, CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_ui_binding_id_dst( CExtRichContentItem *, CExtRichContentItem *, CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_id( CExtRichContentItem *, CExtRichContentItem *, CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_name( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_size( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_noshade( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_face( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_color( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_bgcolor( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_align( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_valign( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_border( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_cellpadding( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_cellspacing( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_width( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_height( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_colspan( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_rowspan( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_min_height( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_max_height( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_min_width( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_html_parm_handler_max_width( CExtRichContentItem *, CExtRichContentItem *,CExtRichStyleRule &, const CExtRichStyleDescription & );

	static void stat_css_parm_handler_visibility( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_display( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_dock( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_table_layout( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_empty_cells( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_caption_side( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_spacing( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_collapse( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
//	static void stat_css_parm_handler_page_break_before( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
//	static void stat_css_parm_handler_page_break_after( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_z_index( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_quotes( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_counter_increment( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_counter_reset( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_content( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_left( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_top( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_right( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_bottom( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
//	static void stat_css_parm_handler_clear( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_clip( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_cursor( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_float( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_overflow( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_sbh( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_sbv( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_position( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_list_style( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_list_style_image( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_list_style_position( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_list_style_type( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_height( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_max_height( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_max_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_min_height( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_min_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_outline( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_outline_color( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_outline_style( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_outline_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_background( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_background_color( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_background_repeat( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_background_attachment( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_background_image( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_background_position( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_white_space( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_font( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_font_family( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_font_style( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_font_size( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_font_weight( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_padding( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_margin( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_line_height( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_text_indent( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_color( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_direction( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_text_align( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_vertical_align( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_text_decoration( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_text_transform( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_word_spacing( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_letter_spacing( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_margin_left( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_margin_right( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_margin_top( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_margin_bottom( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_padding_left( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_padding_right( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_padding_top( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_padding_bottom( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_color( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_left( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_right( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_style( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_top( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_bottom( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_left_color( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_right_color( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_top_color( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_bottom_color( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_left_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_right_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_top_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_bottom_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_width( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_left_style( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_right_style( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_top_style( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_border_bottom_style( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_can_be_disabled( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_can_be_active( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_can_be_focus( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_can_be_selected( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_can_be_hover( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_can_be_pressed( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_can_be_link( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_can_be_visited( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_focus_on_make_selected( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_focus_on_make_unselected_sibling( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_focus_off_make_unselected( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_focus_off_make_unselected_sibling( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_list_navigation( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_draw_method( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_background_draw_method( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_slice( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_background_slice( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_transparency( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_background_transparency( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_focus_ensure_visibility( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_background_extend( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_delayed_draw( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );
	static void stat_css_parm_handler_ui_sb_type( CExtRichContentItem *, CExtRichContentItem *,  CExtRichStyleRule &, const CExtRichStyleDescription & );

	virtual void ApplyTagParams( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent );
	virtual void ApplyTagParams_Basic( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent );
	virtual void ApplyTagParams_CSS( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent );
	virtual bool ApplyTagParams_CSS_SideBorderStyle( CExtRichStyleRule & styleChanging, __EXT_MFC_SAFE_LPCTSTR strCssSideStyle, bool bApplyLeft, bool bApplyRight, bool bApplyTop, bool bApplyBottom );
	virtual bool ApplyTagParams_CSS_SideBorderColor( CExtRichStyleRule & styleChanging, __EXT_MFC_SAFE_LPCTSTR strCssSideStyle, bool bApplyLeft, bool bApplyRight, bool bApplyTop, bool bApplyBottom );
	virtual bool ApplyTagParams_CSS_SideBorderWH( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strCssSideStyle, bool bApplyLeft, bool bApplyRight, bool bApplyTop, bool bApplyBottom );
	virtual bool ApplyTagParams_CSS_SideMarginWH( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strCssSideStyle, bool bApplyLeft, bool bApplyRight, bool bApplyTop, bool bApplyBottom );
	virtual bool ApplyTagParams_CSS_SidePaddingWH( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strCssSideStyle, bool bApplyLeft, bool bApplyRight, bool bApplyTop, bool bApplyBottom );

	virtual void ComputeLinearListForParagraph( CTypedPtrList < CPtrList, CExtRichContentItem * >  & _listItems, bool bParentIsParagraph = false );

	virtual bool IsSingleTag() const;
	virtual bool IsNoLayoutRectItem() const;
	virtual bool IsParagraphItem() const;
	virtual bool IsNoWrapSequence() const;
	virtual bool IsUnknownItem() const;

	CExtRichStyleDescription & ReStyle(
		CDC & dc,
		const CExtRichStyleDescription & styleWalk,
		bool bForceRestyle = false,
		bool bDeep = true
		);
	void ReStyle(
		const CExtRichStyleDescription & styleWalk,
		bool bForceRestyle = false,
		bool bDeep = true
		)
	{
		CWindowDC dc( NULL );
		ReStyle( dc, styleWalk, bForceRestyle, bDeep );
	}
	virtual CExtRichStyleDescription::e_scroll_bar_type_t GetSBTH();
	virtual CExtRichStyleDescription::e_scroll_bar_type_t GetSBTV();
	virtual void DoLayoutRoot( CDC & dc, const CRect & rcStartLayout ); // should be invoked for root only
	virtual void DoSizePreCalc( CDC & dc, const CRect & rcStartLayout );
	virtual void PreLayout( CDC & dc, const CExtRichStyleDescription & styleWalk, const CRect & rcStartLayout );
	virtual void PostLayout( CDC & dc );
	virtual HDWP LayoutAttachedControl( HDWP hDWP, const CPoint & ptControlScrollPos );
	virtual void DoLayout( CDC & dc, const CRect & rcStartLayout, CExtRichContentItem * pRCIFA, bool bOverflowComputed );
	virtual void DoLayout_JustifiedLine(
		CDC & dc, const CRect & rcStartLayout,
		LONG nWidthOfLine, POSITION posWalkLineSaved, POSITION posLineEOF,
		CTypedPtrList < CPtrList, CExtRichContentItem * >  & _listItems, bool bFirstLine,
		const CExtRichStyleDescription & styleNext
		);
	virtual void DoLayout_ComputeChildUnionForTableGroup();

	virtual CExtSafeString ProcessContentSpec( __EXT_MFC_SAFE_LPCTSTR strContentSpec );

	virtual CExtRichStyleDescription::e_ui_sb_type_t Scrolling_GetEffectiveSBT() const;
	virtual LONG Scrolling_GetHorizontalScrollBarHeight();
	virtual LONG Scrolling_GetVerticalScrollBarWidth();
	virtual void Scrolling_FillScrollBarInfo(
		bool bHorz,
		const CRect & rcSB,
		LPVOID pSbData,
		bool bCorrectScrollingPosition = true
		);
	virtual void Scrolling_CorrectScrollingPosition(
		LPVOID pSbData
		);
	void Scrolling_EnsureVisibility(
		bool bEnsureHorz = true,
		bool bEnsureVert = true,
		bool bPreferLeft = true,
		bool bPreferTop = true,
		bool bDeep = true
		);
	virtual void Scrolling_EnsureVisibility(
		const CRect & rcAbsoluteLocation,
		bool bEnsureHorz = true,
		bool bEnsureVert = true,
		bool bPreferLeft = true,
		bool bPreferTop = true,
		bool bDeep = true
		);

	virtual void DoPaintRoot( CDC & dc, const CRect & rcDocRenderTarget, const CPoint & ptScrollPosTarget ); // should be invoked for root only
	virtual void DoPaintScrollBar( CDC & dc, bool bHorz, const CRect & rcSB, const CRect & rcDocRenderTarget, const CPoint & ptScrollPosTarget, CExtRichContentItem * pRCIFA );
	virtual void DoPaintScrollBarCorner( CDC & dc, const CRect & rcCorner, const CRect & rcDocRenderTarget, const CPoint & ptScrollPosTarget, CExtRichContentItem * pRCIFA );
	virtual void DoPaintScrollBars( CDC & dc, const CRect & rcDocRenderTarget, const CPoint & ptScrollPosTarget, CExtRichContentItem * pRCIFA );
	virtual void DoPaintPopOverScrollBars( CDC & dc, const CRect & rcLayout, CExtRichStyleDescription::e_ui_sb_type_t _eUiSBT, bool bH, bool bV );
	static CExtBitmap & stat_GetScrollThumbBitmap();
	virtual void DoPaintBorder( CDC & dc, const CExtRichStyleDescription & styleNext, const CRect & rcDocRenderTarget, const CPoint & ptScrollPosTarget, CExtRichContentItem * pRCIFA );
	virtual void DoPaint( CDC & dc, const CRect & rcDocRenderTarget, const CPoint & ptScrollPosTarget, CExtRichContentItem * pRCIFA, bool bScrollingAdjusted );
	virtual void DoPaintChildren( CDC & dc, const CRect & rcDocRenderTarget, const CPoint & ptScrollPosTarget, CExtRichContentItem * pRCIFA, bool bScrollingAdjusted );
	virtual CRect CalcListItemMarkerLocation(
		INT nLO,
		CExtRichStyleDescription::e_list_style_position_t eLSP,
		const CRect & rcLayout,
		const CRect & rcDrawListSign,
		const CSize & sizeMarker,
		INT nFontAscent,
		INT nFreeSpaceOverAscent,
		INT nFreeSpaceBeforeText
		);

	virtual CExtRichContentItem * FindParmByName( __EXT_MFC_SAFE_LPCTSTR strParamName ) const;
	virtual CExtRichContentItem * FindParmByNameFirst( POSITION & pos, __EXT_MFC_SAFE_LPCTSTR strParamName ) const;
	virtual CExtRichContentItem * FindParmByNameNext( POSITION & pos, __EXT_MFC_SAFE_LPCTSTR strParamName ) const;
	virtual CExtRichContentItem * FindElementByText( __EXT_MFC_SAFE_LPCTSTR strText, bool bEffectiveText = false ) const;
	virtual CExtRichContentItem * FindElementByTextFirst( POSITION & pos, __EXT_MFC_SAFE_LPCTSTR strText, bool bEffectiveText = false ) const;
	virtual CExtRichContentItem * FindElementByTextNext( POSITION & pos, __EXT_MFC_SAFE_LPCTSTR strText, bool bEffectiveText = false ) const;

	virtual LONG DoVerticalAlign( POSITION posStart, POSITION posEnd, CTypedPtrList < CPtrList, CExtRichContentItem * > * listItems, LONG nLineHeight );

	virtual void OffsetSubTree(
		const SIZE & sizeOffset,
		bool bOffsetThisNode = true,
		bool bOffsetSubTree = true,
		bool bOffsetFloatingElements = false
		);
	virtual void UnionSubTree(
		bool bOneLevelOnly = true
		);
	virtual void CalcualateNoLayoutRectItemsSize();
	virtual void EmptyNoLayoutRectItemsSize();

	virtual CExtBitmap * ImageBySrc();
	const CExtBitmap * ImageBySrc() const;

	virtual bool ParseFontFamily( CExtRichStyleRule & styleChanging, __EXT_MFC_SAFE_LPCTSTR strFontFamily ) const;
	virtual bool ParseFontStyle( CExtRichStyleRule & styleChanging, __EXT_MFC_SAFE_LPCTSTR strFontStyle ) const;
	virtual bool ParseFontSize( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strFontSize ) const;
	virtual bool ParseFontWeight( CExtRichStyleRule & styleChanging, __EXT_MFC_SAFE_LPCTSTR strFontWeight ) const;
	virtual bool ParseLineHeight( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strLineHeight ) const;

	virtual bool ParseOutlineColor( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strOutlineColor ) const;
	virtual bool ParseOutlineStyle( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strOutlineStyle ) const;
	virtual bool ParseOutlineWidth( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strOutlineWidth ) const;

	virtual bool ParseListStyleImage( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strListStyleImage ) const;
	virtual bool ParseListStylePosition( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strListStylePosition ) const;
	virtual bool ParseListStyleType( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strListStyleType ) const;

	virtual void ParseOuter( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strOuter, CExtRichStyleDescription::e_outer_type_t eOT ) const;

//	virtual void ParsePageBreak( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strPageBreak, bool bBefore = true ) const;

	virtual bool ParseBackgroundColor( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strBackgroundColor ) const;
	virtual bool ParseBackgroundImage( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strBackgroundImage ) const;
	virtual bool ParseBackgroundRepeat( CExtRichStyleRule & styleChanging, __EXT_MFC_SAFE_LPCTSTR strBackgroundRepeat ) const;
	virtual bool ParseBackgroundAttachment( CExtRichStyleRule & styleChanging, __EXT_MFC_SAFE_LPCTSTR strBackgroundAttachment ) const;
	virtual bool ParseBackgroundPosition( CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent, __EXT_MFC_SAFE_LPCTSTR strBackgroundPosition, int nCount, int nIndex ) const;

	virtual bool ParseParameters();
	virtual bool ParseTransformCSS();
	virtual bool ParseCSSValue_content( CTypedPtrList < CPtrList, CExtRichContentItem * > & listItems, __EXT_MFC_SAFE_LPCTSTR strParmValue );
	virtual bool ParseCSSValue_font_family( CTypedPtrList < CPtrList, CExtRichContentItem * > & listItems, __EXT_MFC_SAFE_LPCTSTR strParmValue );
	virtual bool ParseCSSValue( CTypedPtrList < CPtrList, CExtRichContentItem * > & listItems, __EXT_MFC_SAFE_LPCTSTR strParmValue );
	virtual bool ParseTextRange(
		bool bParentPreTag = false
		);

	virtual bool IsGlobalScope() const;
	virtual LONG GetWordShift() const;
	
#if (defined __EXT_RICH_CONTENT_DEBUG)
	void _DebugTraceParameter()
	{
		switch( m_eType )
		{
		case __EHIT_PARAMETER_BASIC:
			//_tprintf( _T("%s=\"%s\""), LPCTSTR(m_strTextParsed), LPCTSTR(m_strTagPV) );
			{
				//USES_CONVERSION;
				CExtSafeString str;
				str.Format( _T("%s=\"%s\""), LPCTSTR(m_strTextParsed), LPCTSTR(m_strTagPV) );
				afxDump << str;
				//TRACE0( T2CA( LPCTSTR(str) ) );
			}
		break;
		case __EHIT_PARAMETER_CSS_CONTAINER:
			{
				//_tprintf( _T("CSS( ") );
				TRACE0( "CSS( " );
				POSITION pos = m_listParms.GetHeadPosition();
				bool bFirst = true;
				for( ; pos != NULL; )
				{
					CExtRichContentItem * pRCI = m_listParms.GetNext( pos );
					__EXT_DEBUG_RICH_CONTENT_ASSERT( pRCI != NULL );
					if( ! bFirst )
						//_tprintf( _T(" ") );
						TRACE0( " " );
					pRCI->_DebugTraceParameter();
					bFirst = false;
				} // for( ; pos != NULL; )
				//_tprintf( _T(" )") );
				TRACE0( " )" );
			}
		break;
		case __EHIT_PARAMETER_CSS_VALUE:
			//_tprintf( _T("%s:%s"), LPCTSTR(m_strTextParsed), LPCTSTR(m_strTagPV) );
			{
				CExtSafeString str;
				str.Format( _T("%s:%s"), LPCTSTR(m_strTextParsed), LPCTSTR(m_strTagPV) );
				afxDump << str;
				//TRACE0( LPCTSTR(str) );
			}
		break;
		default:
			__EXT_DEBUG_RICH_CONTENT_ASSERT( FALSE );
		break;
		}
	}
	void _DebugTraceLinearItem( int  nIndent )
	{
		CExtSafeString strTraceLine, strTmp;
		if( m_eType == __EHIT_ROOT )
			return;
		if( m_eError == __EHIE_OK )
		{
			if( nIndent > 0 )
			{
				int i = nIndent;
				for( ; i > 0; i -- )
					strTraceLine += _T("    ");
			}
		}
		else
			strTraceLine.Format( _T("(ERROR %d) "), int(m_eError) );
		strTmp.Format(
			_T("%d--> %s%s%s%s"),
			int(m_eType),
			( m_eType == __EHIT_GENERIC_TAG ) ? _T("<") : _T(""),
			( m_eType == __EHIT_GENERIC_TAG && m_bClosingTag ) ? _T("/") : _T(""),
			LPCTSTR(m_strTextParsed),
			( m_eType == __EHIT_GENERIC_TAG ) ? _T(">") : _T("")
			);
		strTraceLine += strTmp;
		afxDump << strTraceLine;
		//TRACE0( LPCTSTR(strTraceLine) );
//		if( ! m_strTagPV.IsEmpty() )
//			_tprintf( _T(" --parms--> %s"), LPCTSTR(m_strTagPV) );
		if( LONG( m_listParms.GetCount() ) > 0 )
		{
			//strTraceLine += _T(" --parms--> ");
			TRACE0( " --parms--> " );
			POSITION pos = m_listParms.GetHeadPosition();
			bool bFirst = true;
			for( ; pos != NULL; )
			{
				CExtRichContentItem * pRCI = m_listParms.GetNext( pos );
				__EXT_DEBUG_RICH_CONTENT_ASSERT( pRCI != NULL );
				if( ! bFirst )
					//strTraceLine += _T(" ");
					TRACE0( " " );
				pRCI->_DebugTraceParameter();
				bFirst = false;
			} // for( ; pos != NULL; )
		}
		//strTraceLine += _T("\r\n");
		//TRACE( LPCTSTR(strTraceLine) );
		TRACE0( "\r\n" );
	} // void DebugTraceLinearItem( int  nIndent )
	void _DebugTraceTree( int  nIndent )
	{
		if( m_eType != __EHIT_ROOT )
			_DebugTraceLinearItem( nIndent );
		nIndent ++;
		POSITION pos = m_listItems.GetHeadPosition();
		for( ; pos != NULL; )
		{
			CExtRichContentItem * pRCI = m_listItems.GetNext( pos );
			__EXT_DEBUG_RICH_CONTENT_ASSERT( pRCI != NULL );
			pRCI->_DebugTraceTree( nIndent );
		} // for( ; pos != NULL; )
	} // void DebugTraceTree( int  nIndent )

	void _DebugReturnArrayOfStrings( int  nIndent, CArray < CExtSafeString, CExtSafeString > & prStrings )
	{
		if( m_eType != __EHIT_ROOT )
		{
			//DebugTraceLinearItem( nIndent );
			if( m_eType == __EHIT_TEXT_RANGE )
			{
				prStrings.Add(m_strTextParsed);
			}
		}
		POSITION pos = m_listItems.GetHeadPosition();
		for( ; pos != NULL; )
		{
			CExtRichContentItem * pRCI = m_listItems.GetNext( pos );
			__EXT_DEBUG_RICH_CONTENT_ASSERT( pRCI != NULL );
			pRCI->_DebugReturnArrayOfStrings( nIndent, prStrings );
		} // for( ; pos != NULL; )
	} // void DebugTraceTree( int  nIndent )
#endif // (defined __EXT_RICH_CONTENT_DEBUG)

	friend class CExtRichContentLayout;
}; /// CExtRichContentItem

/////////////////////////////////////////////////////////////////////////////
/// CExtRichContentEvents

class __PROF_UIS_API CExtRichContentEvents
{
public:
	CExtRichContentEvents();
	virtual ~CExtRichContentEvents();
	virtual bool RciEvent_ResetElementStateInfo(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		const CExtRichStyleDescription::pseudo_class_def_t & statesToReset,
		CExtRichStyleDescription::e_ui_yes_not_option_t eynSetOn,
		bool bDeep = true
		);
	virtual bool RciEvent_OnKey(
		CExtRichContentItem * pRCI,
		bool bSysKey,
		bool bKeyDownEvent, // true - key-down event, false - key-up event
		UINT nChar, // virtual key code
		UINT nRepCnt, // key-down/key-up press count
		UINT nFlags, // key-down/key-up event flags
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnSelect(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		bool bSelected
		);
	virtual bool RciEvent_OnDisable(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		bool bDisabled
		);
	virtual bool RciEvent_OnFocusEnter(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		CExtRichContentItem * pOldFocusRCI
		);
	virtual bool RciEvent_OnFocusLeave(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		CExtRichContentItem * pNewFocusRCI
		);
	virtual bool RciEvent_OnMouseWheel(
		CExtRichContentItem * pRCI,
		UINT fFlags,
		short zDelta,
		CPoint point,
		const CExtRichContentHitTestInfo & htInfoHover,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnMouseEnter(
		CExtRichContentItem * pRCI,
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnMouseLeave(
		CExtRichContentItem * pRCI,
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnPressedEnter(
		CExtRichContentItem * pRCI,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnPressedLeave(
		CExtRichContentItem * pRCI,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnClick(
		CExtRichContentItem * pRCI,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnDoubleClick(
		CExtRichContentItem * pRCI,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual CExtRichStyleDescription::e_ui_sb_type_t RciEvent_Scrolling_GetDefaultSBT( const CExtRichContentItem * pRCI ) const;
}; /// class CExtRichContentEvents

/////////////////////////////////////////////////////////////////////////////
/// CExtRichContentEventsBroadcast

class __PROF_UIS_API CExtRichContentEventsBroadcast : public CExtRichContentEvents
{
	CList < CExtRichContentEvents *, CExtRichContentEvents * > m_listEventListeners;
public:
	CExtRichContentEventsBroadcast();
	virtual ~CExtRichContentEventsBroadcast();
	virtual void ListenerAdd(
		CExtRichContentEvents * pListener,
		bool bAddToTail = true
		);
	virtual bool ListenerIsPresent( CExtRichContentEvents * pListener );
	bool ListenerIsPresent( const CExtRichContentEvents * pListener ) const { return const_cast < CExtRichContentEventsBroadcast * > ( this ) -> ListenerIsPresent( const_cast < CExtRichContentEvents * > ( pListener ) ); }
	virtual bool ListenerRemove( CExtRichContentEvents * pListener );
	virtual void ListenerRemoveAll();
	virtual POSITION ListenerGetHeadPosition();
	virtual POSITION ListenerGetTailPosition();
	virtual CExtRichContentEvents * ListenerGetNext( POSITION & pos );
	virtual CExtRichContentEvents * ListenerGetPrev( POSITION & pos );

	virtual bool RciEvent_ResetElementStateInfo(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		const CExtRichStyleDescription::pseudo_class_def_t & statesToReset,
		CExtRichStyleDescription::e_ui_yes_not_option_t eynSetOn,
		bool bDeep = true
		);
	virtual bool RciEvent_OnKey(
		CExtRichContentItem * pRCI,
		bool bSysKey,
		bool bKeyDownEvent, // true - key-down event, false - key-up event
		UINT nChar, // virtual key code
		UINT nRepCnt, // key-down/key-up press count
		UINT nFlags, // key-down/key-up event flags
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnSelect(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		bool bSelected
		);
	virtual bool RciEvent_OnDisable(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		bool bDisabled
		);
	virtual bool RciEvent_OnFocusEnter(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		CExtRichContentItem * pOldFocusRCI
		);
	virtual bool RciEvent_OnFocusLeave(
		CExtRichContentItem * pRCI,
		HWND hWndContentViewer,
		CExtRichContentItem * pNewFocusRCI
		);
	virtual bool RciEvent_OnMouseWheel(
		CExtRichContentItem * pRCI,
		UINT fFlags,
		short zDelta,
		CPoint point,
		const CExtRichContentHitTestInfo & htInfoHover,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnMouseEnter(
		CExtRichContentItem * pRCI,
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnMouseLeave(
		CExtRichContentItem * pRCI,
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnPressedEnter(
		CExtRichContentItem * pRCI,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnPressedLeave(
		CExtRichContentItem * pRCI,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnClick(
		CExtRichContentItem * pRCI,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual bool RciEvent_OnDoubleClick(
		CExtRichContentItem * pRCI,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		const CExtRichContentHitTestInfo & htInfoOld,
		const CExtRichContentHitTestInfo & htInfoNew,
		HWND hWndContentViewer
		);
	virtual CExtRichStyleDescription::e_ui_sb_type_t RciEvent_Scrolling_GetDefaultSBT( const CExtRichContentItem * pRCI ) const;
}; /// class CExtRichContentEventsBroadcast

/////////////////////////////////////////////////////////////////////////////
/// CExtRichContentLayout

class __PROF_UIS_API CExtRichContentLayout : public CExtRichContentEventsBroadcast
{
public:
	struct __PROF_UIS_API DRAWSTATE_data_t
	{
		HBRUSH m_hBrush;
		DRAWSTATEPROC m_pOutputFunc;
		UINT m_fuFlags;
		COLORREF m_clrText, m_clrShadow;
		CSize m_sizeOffsetText, m_sizeOffsetShadow;
		DRAWSTATE_data_t()
			: m_hBrush( NULL )
			, m_pOutputFunc( NULL )
			, m_fuFlags( DST_TEXT )
			, m_clrText( COLORREF(-1L) ) // COLORREF(-1L) means use COLOR_3DSHADOW
			, m_clrShadow( COLORREF(-1L) ) // COLORREF(-1L) means use COLOR_3DHIGHLIGHT
			, m_sizeOffsetText( 0, 0 )
			, m_sizeOffsetShadow( 1, 1 )
		{
		}
	};
	struct __PROF_UIS_API BkMode_t
	{
		HDC m_hDC;
		int m_nBkModeOld;
		BkMode_t( HDC hDC, int nBkMode )
			: m_hDC( hDC )
			, m_nBkModeOld( TRANSPARENT )
		{
			m_nBkModeOld = ::SetBkMode( m_hDC, nBkMode );
		}
		~BkMode_t()
		{
			::SetBkMode( m_hDC, m_nBkModeOld );
		}
	};

	bool m_bDrawTextInvocation:1;
	UINT m_nDTF, m_nExtDTF;
	INT m_nPlainTextLineHeight;
	HWND m_hWndOpenThemeData;
	LPCVOID m_wstrOpenThemeDataClassList;
	DWORD m_dwOpenThemeDataFlags;
	int m_nDtThemePartID, m_nDtThemeStateID;
	CExtUxTheme::__EXT_UX_DTTOPTS * m_pDTO;
	CExtRichContentLayout::DRAWSTATE_data_t * m_pDSD;

	virtual CSize GetTextExtent(
		HDC hDC,
		__EXT_MFC_SAFE_LPCTSTR str,
		INT nCount = -1
		);
	virtual bool TextOut(
		HDC hDC,
		int x,
		int y,
		__EXT_MFC_SAFE_LPCTSTR str,
		int nCount = -1
		);
protected:
	void _EmptyList();
	void _EmptyClasses();
	void _EmptyObjectContainerLocal();
	void _EmptyPositionedElementReferences();
	void _EmptySpecialElementReferences();
	void _EmptyElementMaps();
	CExtRichDocObjectBase * m_pObjectContainerLocal;
public:
	virtual CExtRichContentItem * CssClassNew( __EXT_MFC_SAFE_LPCTSTR strClassName );
	virtual CExtRichContentItem * CssClassGet( __EXT_MFC_SAFE_LPCTSTR strClassName );
	const CExtRichContentItem * CssClassGet( __EXT_MFC_SAFE_LPCTSTR strClassName ) const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> CssClassGet( LPCTSTR(strClassName) ); }
	virtual void CssClassMerge( CExtRichContentItem * pRCI, __EXT_MFC_SAFE_LPCTSTR strExplicitClassName, CExtRichStyleRule & styleChanging, const CExtRichStyleDescription & styleCurrent );
	virtual bool HaveAffectedCssPseudoClasses( const CExtRichContentItem * pRCI, const CExtRichStyleDescription::pseudo_class_def_t & statesToCheck ) const;

	virtual void _ElementByUiBindingSrcID_UnRegister( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingSrcID );
	virtual void _ElementByUiBindingSrcID_Register( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingSrcID, CExtRichContentItem * pRCI );
	virtual CExtRichContentItem * ElementByUiBindingSrcID( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingSrcID );
	const CExtRichContentItem * ElementByUiBindingSrcID( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingSrcID ) const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> ElementByUiBindingSrcID( LPCTSTR(strElementUiBindingSrcID) ); }

	virtual void _ElementByUiBindingDstID_UnRegister( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingDstID );
	virtual void _ElementByUiBindingDstID_Register( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingDstID, CExtRichContentItem * pRCI );
	virtual CExtRichContentItem * ElementByUiBindingDstID( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingDstID );
	const CExtRichContentItem * ElementByUiBindingDstID( __EXT_MFC_SAFE_LPCTSTR strElementUiBindingDstID ) const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> ElementByUiBindingDstID( LPCTSTR(strElementUiBindingDstID) ); }

	virtual void LayoutAttachedControls( const CPoint & ptControlScrollPos );
	virtual void AttachHwndToElementID( __EXT_MFC_SAFE_LPCTSTR strElementID, HWND hWnd );
	virtual CExtSafeString GetElementIdByHWND( HWND hWnd ) const;
	virtual HWND GetHwndByElementID( __EXT_MFC_SAFE_LPCTSTR strElementID ) const;
	CWnd * GetWndByElementID( __EXT_MFC_SAFE_LPCTSTR strElementID ) const;
	CWnd * GetWndPermanentByElementID( __EXT_MFC_SAFE_LPCTSTR strElementID ) const;

	virtual void _ElementByID_UnRegister( __EXT_MFC_SAFE_LPCTSTR strElementID );
	virtual void _ElementByID_Register( __EXT_MFC_SAFE_LPCTSTR strElementID, CExtRichContentItem * pRCI );
	virtual CExtRichContentItem * ElementByID( __EXT_MFC_SAFE_LPCTSTR strElementID );
	const CExtRichContentItem * ElementByID( __EXT_MFC_SAFE_LPCTSTR strElementID ) const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> ElementByID( LPCTSTR(strElementID) ); }

	virtual void _ElementByName_UnRegister( __EXT_MFC_SAFE_LPCTSTR strElementName );
	virtual void _ElementByName_Register( __EXT_MFC_SAFE_LPCTSTR strElementName, CExtRichContentItem * pRCI );
	virtual CExtRichContentItem * ElementByName( __EXT_MFC_SAFE_LPCTSTR strElementName );
	const CExtRichContentItem * ElementByName( __EXT_MFC_SAFE_LPCTSTR strElementName ) const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> ElementByName( LPCTSTR(strElementName) ); }

	enum e_layout_format_t
	{
		__ELFMT_UNKNOWN			= 0,
		__ELFMT_AUTO_DETECT		= 0,
		__ELFMT_HTML			= 1,
		__ELFMT_CSS				= 2,
		__ELFMT_PLAIN_TEXT		= 3,
	};
	enum e_layout_orientation_t
	{
		__ELOT_NORMAL			= 0,
		__ELOT_0				= 0,
		__ELOT_90_CW			= 90,
		__ELOT_180_CW			= 180,
		__ELOT_270_CW			= 270,
		__ELOT_90_CCW			= 270,
		__ELOT_180_CCW			= 180,
		__ELOT_270_CCW			= 90,
	};

	e_layout_format_t m_eLFMT;
	e_layout_orientation_t m_eLO;
	CTypedPtrList < CPtrList, CExtRichContentItem * > m_listHeap, m_listSpecial, m_listER, m_listEFA; // heap, relatively positioned elements, fixed/absolute positioned elements
	CTypedPtrList < CPtrList, CExtRichContentItem * > m_listDelayedDraw_PreFixedAbsolute, m_listDelayedDraw_TopMost;
protected:
	CExtSafeMapStringToPtr m_mapClasses, m_mapEmelentsByID, m_mapId2HWND, m_mapEmelentsByName, m_mapEmelentsByUiBindingSrcID, m_mapEmelentsByUiBindingDstID;
	CExtHashMapT < HWND, HWND, CExtSafeString, CExtSafeString > m_mapBoundHWNDs;
public:
	CExtSafeString m_strTab;
	bool m_bRemoveSpacecBetweenLinesInParagraphs:1, m_bRemoveSpacecBetweenParagraphs:1, m_bDrawImagesSmootherAsPossible:1;
	CRect m_rcStartLayoutSaved;

	CExtRichContentLayout(
		__EXT_MFC_SAFE_LPCTSTR strSourceText = NULL,
		e_layout_format_t _eLFMT = __ELFMT_HTML,
		e_layout_orientation_t eLO = __ELOT_NORMAL
		);
	virtual ~CExtRichContentLayout();

	virtual void Empty(
		bool bEmptyLocalObjectContainer = true
		);
	virtual bool IsEmpty() const;
	virtual CExtRichDocObjectBase & GetObjectCountainerLocal();
	const CExtRichDocObjectBase & GetObjectCountainerLocal() const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> GetObjectCountainerLocal(); }
	virtual CExtRichDocObjectBase & GetObjectCountainerGlobal();
	const CExtRichDocObjectBase & GetObjectCountainerGlobal() const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> GetObjectCountainerGlobal(); }
	static CExtRichDocObjectBase & stat_GetObjectCountainerGlobal();
	virtual CExtRichDocObjectBase * ObjectByName( __EXT_MFC_SAFE_LPCTSTR strName );
	const CExtRichDocObjectBase * ObjectByName( __EXT_MFC_SAFE_LPCTSTR strName ) const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> ObjectByName( LPCTSTR(strName) ); }
	virtual CExtBitmap * ImageByName( __EXT_MFC_SAFE_LPCTSTR strName );
	const CExtBitmap * ImageByName( __EXT_MFC_SAFE_LPCTSTR strName ) const { return ( const_cast < CExtRichContentLayout * > ( this ) ) -> ImageByName( LPCTSTR(strName) ); }

	CTypedPtrList < CPtrList, CExtRichContentLayout * > m_listLayoutCSS;
	virtual void _DoLayout_ListOfCSS_Empty();
	virtual void _DoLayout_ListOfCSS_Rebuild();
	virtual INT  _DoLayout_ListOfCSS_LoadLinked( bool bLookupGlobal, bool bLookupLocal );
	virtual bool _DoLayout_ListOfCSS_LoadLinkedItem( __EXT_MFC_SAFE_LPCTSTR strHREF, bool bLookupGlobal, bool bLookupLocal  );
	virtual void _DoLayout_ListOfCSS_LoadThis();
	virtual bool _DoLayout_ListOfCSS_LoadGlobalDefaults();
	virtual void _DoLayout_ListOfCSS_LoadAllAvailable( bool bLoadLocal, bool bLoadGlobal, bool bLoadThis );

	virtual CExtRichStyleDescription::e_ui_sb_type_t Scrolling_GetEffectiveSBT( const CExtRichContentItem * pRCI ) const;
	virtual LONG Scrolling_GetHorizontalScrollBarHeight( const CExtRichContentItem * pRCI );
	virtual LONG Scrolling_GetVerticalScrollBarWidth( const CExtRichContentItem * pRCI );
	virtual void DoLayout( CDC & dc, const CRect & rcStartLayout );
	virtual void DoPaint( CDC & dc, const CRect & rcDocRenderTarget, const CPoint & ptScrollPos );
	virtual void ReStyle(
		CDC & dc,
		bool bForceRestyle = false,
		bool bDeep = true
		);
	void ReStyle(
		bool bForceRestyle = false,
		bool bDeep = true
		)
	{
		CWindowDC dc( NULL );
		ReStyle( dc, bForceRestyle, bDeep );
	}

	virtual bool Parse(
		__EXT_MFC_SAFE_LPCTSTR strSourceText,
		e_layout_format_t _eLFMT,
		UINT nDTF = 0, // DT_*** flags from Win32's DrawText() API (used mainly by plain text format)
		UINT nExtDTF = 0, // __EXT_DT_*** flags (Prof-UIS)
		bool bEmptyLocalObjectContainer = true
		);
	virtual bool ParseCSS(
		__EXT_MFC_SAFE_LPCTSTR strCSS,
		bool bEmptyLocalObjectContainer = true
		);
	virtual bool ParseHTML(
		__EXT_MFC_SAFE_LPCTSTR strHTML,
		bool bEmptyLocalObjectContainer = true
		);
	virtual bool ParseHTML_LoadLinkedCSS( __EXT_MFC_SAFE_LPCTSTR strHREF );
	virtual bool ParsePlainText(
		__EXT_MFC_SAFE_LPCTSTR strPlainText,
		UINT nDTF = 0, // DT_*** flags from Win32's DrawText() API (used mainly by plain text format)
		UINT nExtDTF = 0, // __EXT_DT_*** flags (Prof-UIS)
		bool bEmptyLocalObjectContainer = true
		);
	virtual void ParseClasses( __EXT_MFC_SAFE_LPCTSTR strClasses );
	virtual LONG ParseMeasurementString(
		__EXT_MFC_SAFE_LPCTSTR strMeasurementSpec,
		bool bHorz,
		const CExtRichStyleDescription & styleCurrent,
		LONG nNotParsedRetVal = -1L,
		DOUBLE * p_lfPercent = NULL,
		bool * p_bIsPercent = NULL,
		bool * p_bIsNoMeasurementUnit = NULL,
		bool bAllowAnyPercent = false
		) const;
	virtual COLORREF ParseColorString(
		__EXT_MFC_SAFE_LPCTSTR strColorSpec,
		COLORREF clrNotParsedRetVal = COLORREF(-1L)
		) const;

	virtual LONG ConvDefault2Px( DOUBLE nSrc, bool bHorz ) const;
	virtual LONG ConvPt2Px( DOUBLE lfSrc, bool bHorz ) const;
	virtual LONG ConvCm2Px( DOUBLE lfCm, bool bHorz ) const;
	virtual LONG ConvEm2Px( DOUBLE lgEm, const CExtRichStyleDescription & styleCurrent ) const;
	virtual LONG ConvQEm2Px( DOUBLE lgQEm, const CExtRichStyleDescription & styleCurrent ) const;
	virtual LONG ConvPercent2Px( DOUBLE lfPercent100, const CExtRichStyleDescription & styleCurrent ) const; // 0.0 <= lfPercent100 && lfPercent100 <= 100.0
	virtual LONG ConvIn2Px( DOUBLE lgIn, bool bHorz ) const;
	virtual LONG ConvMm2Px( DOUBLE lgMm, bool bHorz ) const;
	virtual LONG ConvEx2Px( DOUBLE lgEx ) const;
	virtual LONG ConvPc2Px( DOUBLE lgPc, bool bHorz ) const;

	static __EXT_MFC_SAFE_TCHAR stat_SpecToChar( __EXT_MFC_SAFE_LPCTSTR strValue );
	virtual __EXT_MFC_SAFE_TCHAR SpecToChar( __EXT_MFC_SAFE_LPCTSTR strValue );

	static CExtSafeString stat_GenNumberRoman(
		INT nVal,
		bool bUpper = true,
		bool bFormatAsArabicOnError = true
		);
	static CExtSafeString stat_GenNumberAlphaImpl(
		INT nVal,
		__EXT_MFC_SAFE_LPCTSTR strAlphabet,
		bool bFormatAsArabicOnError = true
		);
	static CExtSafeString stat_GenNumberAlpha(
		INT nVal,
		bool bUpper = true,
		bool bFormatAsArabicOnError = true
		);
	static CExtSafeString stat_GenListItemText(
		CExtRichStyleDescription::e_list_style_type_t eLST,
		INT nVal,
		bool bFormatAsArabicOnError = true
		);
	static CSize stat_GetListMarkerSizeDiskAt96DPI();
	static CSize stat_GetListMarkerSizeSquareAt96DPI();

#if (defined __EXT_RICH_CONTENT_DEBUG)
	void _DebugTraceLinearItemList()
	{
		POSITION pos = m_listHeap.GetHeadPosition();
		for( ; pos != NULL; )
		{
			CExtRichContentItem * pRCI = m_listHeap.GetNext( pos );
			__EXT_DEBUG_RICH_CONTENT_ASSERT( pRCI != NULL );
			pRCI->_DebugTraceLinearItem( 0 );
		}
	}
	void _DebugTraceTree()
	{
		if( LONG( m_listHeap.GetCount() ) == 0 )
			return;
		CExtRichContentItem * pRootRCI = m_listHeap.GetHead();
		__EXT_DEBUG_RICH_CONTENT_ASSERT( pRootRCI->m_eType == CExtRichContentItem::__EHIT_ROOT );
		pRootRCI->_DebugTraceTree( 0 );
	}
	void _DebugReturnArrayOfStrings( CArray < CExtSafeString, CExtSafeString > & prStrings )
	{
		if( LONG( m_listHeap.GetCount() ) == 0 )
			return;
		CExtRichContentItem * pRootRCI = m_listHeap.GetHead();
		__EXT_DEBUG_RICH_CONTENT_ASSERT( pRootRCI->m_eType == CExtRichContentItem::__EHIT_ROOT );
		pRootRCI->_DebugReturnArrayOfStrings( 0, prStrings );
	}
#endif // (defined __EXT_RICH_CONTENT_DEBUG)

	typedef e_layout_orientation_t (*validate_orientation_func_t)( e_layout_orientation_t eLO );
	static validate_orientation_func_t g_pfnVaidateOrientation;
	static e_layout_orientation_t stat_VaidateOrientation( e_layout_orientation_t eLO );

	typedef e_layout_format_t (*format_auto_detect_func_t)( __EXT_MFC_SAFE_LPCTSTR str, int nCount );
	static format_auto_detect_func_t g_pfnDetectFormatForDrawText;
	static e_layout_format_t stat_DetectFormatForDrawText( __EXT_MFC_SAFE_LPCTSTR str, int nCount );

	static int stat_DrawText( e_layout_format_t eLFMT, e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( e_layout_format_t eLFMT, e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( e_layout_format_t eLFMT, e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( e_layout_format_t eLFMT, e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );

	static int stat_DrawText( e_layout_format_t eLFMT, HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( e_layout_format_t eLFMT, HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( e_layout_format_t eLFMT, HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( e_layout_format_t eLFMT, HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );

	static int stat_DrawText( HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawText( HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );

	static int stat_DrawHtmlText( e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawHtmlText( e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawHtmlText( e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawHtmlText( e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );

	static int stat_DrawHtmlText( HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawHtmlText( HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawHtmlText( HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawHtmlText( HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );

	static bool g_bUseWin32ForPlainText;

	static int stat_DrawPlainText( e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawPlainText( e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawPlainText( e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawPlainText( e_layout_orientation_t eLO, HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );

	static int stat_DrawPlainText( HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawPlainText( HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, int nCount, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawPlainText( HDC hDC, __EXT_MFC_SAFE_LPCTSTR str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );
	static int stat_DrawPlainText( HDC hDC, __EXT_MFC_SAFE_LPTSTR  str, LPRECT lpRect, UINT nDTF = 0, UINT nExtDTF = 0, HWND hWndOpenThemeData = NULL, LPCVOID wstrOpenThemeDataClassList = NULL, DWORD dwOpenThemeDataFlags = 0, int nDtThemePartID = 0, int nDtThemeStateID = 0, CExtUxTheme::__EXT_UX_DTTOPTS * pDTO = NULL, CExtRichContentLayout::DRAWSTATE_data_t * pDSD = NULL );

}; /// CExtRichContentLayout

/////////////////////////////////////////////////////////////////////////////
/// CExtRichDocObjectBase

class __PROF_UIS_API CExtRichDocObjectBase : public CObject
{
protected:
	CExtSafeString m_strName;
public:
	DECLARE_SERIAL( CExtRichDocObjectBase );
	CExtRichDocObjectBase(
		__EXT_MFC_SAFE_LPCTSTR strName = NULL
		);
	virtual ~CExtRichDocObjectBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	static void stat_SerializeOb( CArchive & ar, CExtRichDocObjectBase ** ppRCO );
	virtual void Serialize( CArchive & ar );
	// name/collection access
	virtual __EXT_MFC_SAFE_LPCTSTR GetName() const;
	virtual bool IsObjectContainer() const;
	virtual bool IsImage() const;
	virtual bool IsLayoutKeeper() const;
	virtual INT ObjectGetCount() const;
	virtual CExtRichDocObjectBase * ObjectByName( __EXT_MFC_SAFE_LPCTSTR strName );
	virtual bool ObjectAdd(
		CExtRichDocObjectBase * pRCO,
		bool bReplace = true
		);
	virtual bool ObjectRemove( __EXT_MFC_SAFE_LPCTSTR strName );
	bool ObjectRemove( CExtRichDocObjectBase * pRCO );
	virtual INT ObjectRemoveAll();
	virtual POSITION ObjectGetStartPosition() const;
	virtual CExtRichDocObjectBase * ObjectGetNext( POSITION & pos );
	const CExtRichDocObjectBase * ObjectGetNext( POSITION & pos ) const { __EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this ); return ( const_cast < CExtRichDocObjectBase * > ( this ) ) -> ObjectGetNext( pos ); }
	const CExtRichDocObjectBase * ObjectByName( __EXT_MFC_SAFE_LPCTSTR strName ) const { __EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this ); return ( const_cast < CExtRichDocObjectBase * > ( this ) ) -> ObjectByName( LPCTSTR(strName) ); }
	CExtRichDocObjectBase * operator [] ( __EXT_MFC_SAFE_LPCTSTR strName ) { __EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this ); return ObjectByName( strName ); }
	const CExtRichDocObjectBase * operator [] ( __EXT_MFC_SAFE_LPCTSTR strName ) const { __EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this ); return ObjectByName( strName ); }
	void FillObjectArray( CArray < CExtRichDocObjectBase *, CExtRichDocObjectBase * > & _arr );
	void FillObjectArray( CArray < const CExtRichDocObjectBase *, const CExtRichDocObjectBase * > & _arr ) const;
}; /// CExtRichDocObjectBase

/////////////////////////////////////////////////////////////////////////////
/// CExtRichDocObjectContainer

class __PROF_UIS_API CExtRichDocObjectContainer : public CExtRichDocObjectBase
{
protected:
	CExtSafeMapStringToPtr m_mapObjects;
public:
	DECLARE_SERIAL( CExtRichDocObjectContainer );
	CExtRichDocObjectContainer(
		__EXT_MFC_SAFE_LPCTSTR strName = NULL
		);
	virtual ~CExtRichDocObjectContainer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	// name/collection access
	virtual bool IsObjectContainer() const;
	virtual INT ObjectGetCount() const;
	virtual POSITION ObjectGetStartPosition() const;
	virtual CExtRichDocObjectBase * ObjectGetNext( POSITION & pos );
	virtual CExtRichDocObjectBase * ObjectByName( __EXT_MFC_SAFE_LPCTSTR strName );
	virtual bool ObjectAdd(
		CExtRichDocObjectBase * pRCO,
		bool bReplace = true
		);
	virtual bool ObjectRemove( __EXT_MFC_SAFE_LPCTSTR strName );
	virtual INT ObjectRemoveAll();
}; /// CExtRichDocObjectContainer

/////////////////////////////////////////////////////////////////////////////
/// CExtRichDocObjectContainerGlobal

class __PROF_UIS_API CExtRichDocObjectContainerGlobal : public CExtRichDocObjectContainer
{
public:
	DECLARE_SERIAL( CExtRichDocObjectContainerGlobal );
	CExtRichDocObjectContainerGlobal(
		__EXT_MFC_SAFE_LPCTSTR strName = NULL
		);
	virtual ~CExtRichDocObjectContainerGlobal();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
}; /// CExtRichDocObjectContainerGlobal

/////////////////////////////////////////////////////////////////////////////
/// CExtRichDocObjectImage

class __PROF_UIS_API CExtRichDocObjectImage : public CExtRichDocObjectBase
{
protected:
	CExtBitmap * m_pBMP;
	BYTE m_nTransparency;
public:
	DECLARE_SERIAL( CExtRichDocObjectImage );
	CExtRichDocObjectImage(
		__EXT_MFC_SAFE_LPCTSTR strName = NULL
		);
	virtual ~CExtRichDocObjectImage();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual bool IsImage() const;
	virtual void Serialize( CArchive & ar );
	// picture access
	virtual CExtBitmap & ImageGet();
	const CExtBitmap & ImageGet() const { __EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this ); return ( const_cast < CExtRichDocObjectImage * > ( this ) ) -> ImageGet(); }
	virtual BYTE TransparencyGet() const;
	virtual void TransparencySet( BYTE nTransparency );
}; /// CExtRichDocObjectImage

/////////////////////////////////////////////////////////////////////////////
/// CExtRichDocObjectLayoutKeeper

class __PROF_UIS_API CExtRichDocObjectLayoutKeeper : public CExtRichDocObjectBase
{
protected:
	CExtSafeString m_strSourceText;
	CExtRichContentLayout m_layout;
public:
	DECLARE_SERIAL( CExtRichDocObjectLayoutKeeper );
	CExtRichDocObjectLayoutKeeper(
		__EXT_MFC_SAFE_LPCTSTR strName = NULL
		);
	virtual ~CExtRichDocObjectLayoutKeeper();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual bool IsLayoutKeeper() const;
	virtual void Serialize( CArchive & ar );
	// picture access
	virtual __EXT_MFC_SAFE_LPCTSTR SourceTextGet() const;
	virtual bool SourceTextSet(
		__EXT_MFC_SAFE_LPCTSTR strSourceText,
		CExtRichContentLayout::e_layout_format_t _eLFMT,
		UINT nDTF = 0, // DT_*** flags from Win32's DrawText() API (used mainly by plain text format)
		UINT nExtDTF = 0, // __EXT_DT_*** flags (Prof-UIS)
		bool bEmptyLocalObjectContainer = true
		);
	virtual CExtRichContentLayout & LayoutGet();
	const CExtRichContentLayout & LayoutGet() const { __EXT_DEBUG_RICH_CONTENT_ASSERT_VALID( this ); return ( const_cast < CExtRichDocObjectLayoutKeeper * > ( this ) ) -> LayoutGet(); }
}; /// CExtRichDocObjectLayoutKeeper

/////////////////////////////////////////////////////////////////////////////
/// CExtRichBidiAlgorithm
/// Based on the sample project written by Asmus Freytag and distributed under the Terms of Use in http://www.unicode.org/copyright.html.
/// Sample Implementation of the Unicode Bidirectional Algorithm as it was revised by Revision 5 of the Uniode Technical Report # 9 (1999-8-17).
	
class __PROF_UIS_API CExtRichBidiAlgorithm
{
public:
	static bool BidiString( CExtSafeString & str, bool bSingleLine = true, int baselevel = 1, int fMirror = 1, int nLowerUpperMode = -1, bool bClean = true );
	// For convenience of external callers, the following constitute the interface to the actual algorithm. 
	// For usage notes and parameter descriptions see the file bidi.cpp
	static void BidiLines( int baselevel, __EXT_MFC_SAFE_LPTSTR pszLine, int * pclsLine, int * plevelLine, int cchPara, int fMirror, bool * pbrk = 0 );
	static int BidiParagraph( int & baselevel,  int * types, int * levels, int cch );
// === BIDI INTERFACE FUNCTIONS ========================================
	/*------------------------------------------------------------------------
		Bidirectional Character Types

		as defined by the Unicode Bidirectional Algorithm Table 3-7.

		Note:

		  The list of bidirectional character types here is not grouped the
		  same way as the table 3-7, since the numberic values for the types
		  are chosen to keep the state and action tables compact.
	------------------------------------------------------------------------*/
	enum bidi_class
	{
		// input types
				 // ON MUST be zero, code relies on ON = N = 0
		ON = 0,  // Other Neutral
		L,		 // Left Letter
		R,		 // Right Letter
		AN, 	 // Arabic Number
		EN, 	 // European Number
		AL, 	 // Arabic Letter (Right-to-left)
		NSM,	 // Non-spacing Mark
		CS, 	 // Common Separator
		ES, 	 // European Separator
		ET, 	 // European Terminator (post/prefix e.g. $ and %)

		// resolved types
		BN, 	 // Boundary neutral (type of RLE etc after explicit levels)

		// input types,
		S,		 // Segment Separator (TAB)		// used only in L1
		WS, 	 // White space					// used only in L1
		B,		 // Paragraph Separator (aka as PS)

		// types for explicit controls
		RLO,	 // these are used only in X1-X9
		RLE,
		LRO,
		LRE,
		PDF,

		// resolved types, also resolved directions
		N = ON,  // alias, where ON, WS and S are treated the same
	};
	static const int chLRM;
	static const int chRLM;
	static const int chLS;
	static const int chRLO;
	static const int chLRO;
	static const int chRLE;
	static const int chLRE;
	static const int chPDF;
	static const int chBN;

	static int TypesFromChar[];
	static int NTypes[];

	static int ClassFromChN( __EXT_MFC_SAFE_TCHAR ch, int nLowerUpperMode );
	static int ClassFromChWS( __EXT_MFC_SAFE_TCHAR ch, int nLowerUpperMode );
// === DISPLAY SUPPORT =================================================
#if ( ! defined UNICODE )
	enum display_symbol	// Display character codes
	{
		RIGHT = '<',			// rtl arrow
		LEFT = '>',				// ltr arrow
		PUSH = '+',				// dn arrow
		POP = '-',				// up arrow
		LSEP =	'=',			// double dagger
		NEUTRAL = ' ',			// rtl/ltr dbl headed arrow
		ALPHA = 'a',
	};
#else
	enum display_symbol	// Display character codes
	{
		RIGHT = 0x2190, 		 // rtl arrow
		LEFT = 0x2192,			 // rtl arrow
		PUSH = 0x2193,			 // dn arrow
		POP = 0x2191,			 // up arrow
		LSEP =	0x2021,			 // double dagger
		NEUTRAL = 0x2194,		 // rtl/ltr dbl headed arrow
		ALPHA = 0x03B1,			 // greek alpha
	};
#endif
	static __EXT_MFC_SAFE_TCHAR CharFromTypes[];
	static __EXT_MFC_SAFE_TCHAR CharFromLevel[];
// === HELPER FUNCTIONS ================================================
	static void reverse( __EXT_MFC_SAFE_LPTSTR psz, int cch );
	static void SetDeferredRun( int *pval, int cval, int iStart, int nval );
// === ASSIGNING BIDI CLASSES ============================================
	static int classify( int nLowerUpperMode, __EXT_MFC_SAFE_LPCTSTR pszText, int * pcls,	int cch, bool fWS = false );
// === THE PARAGRAPH LEVEL ===============================================
	static int resolveParagraphs( int * types, int cch );
	static int baseLevel( const int * pcls,  int cch );
//====== RESOLVE EXPLICIT ================================================
	static int GreaterEven( int i );
	static int GreaterOdd( int i );
	static int EmbeddingDirection( int level );
	static const int MAX_LEVEL;
	static int resolveExplicit( int level, int dir, int * pcls, int * plevel, int cch, int nNest = 0 );
// === RESOLVE WEAK TYPES ================================================
	enum bidi_state // possible states
	{
		xa,		//	arabic letter
		xr,		//	right leter
		xl,		//	left letter

		ao,		//	arabic lett. foll by ON
		ro,		//	right lett. foll by ON
		lo,		//	left lett. foll by ON

		rt,		//	ET following R
		lt,		//	ET following L

		cn,		//	EN, AN following AL
		ra,		//	arabic number foll R
		re,		//	european number foll R
		la,		//	arabic number foll L
		le,		//	european number foll L

		ac,		//	CS following cn
		rc,		//	CS following ra
		rs,		//	CS,ES following re
		lc,		//	CS following la
		ls,		//	CS,ES following le

		ret,	//	ET following re
		let,	//	ET following le
	} ;
	static int stateWeak[][10];

	enum bidi_action // possible actions
	{
		// primitives
		IX = 0x100,					// increment
		XX = 0xF,					// no-op

		// actions
		xxx = (XX << 4) + XX,		// no-op
		xIx = IX + xxx,				// increment run
		xxN = (XX << 4) + ON,		// set current to N
		xxE = (XX << 4) + EN,		// set current to EN
		xxA = (XX << 4) + AN,		// set current to AN
		xxR = (XX << 4) + R,		// set current to R
		xxL = (XX << 4) + L,		// set current to L
		Nxx = (ON << 4) + 0xF,		// set run to neutral
		Axx = (AN << 4) + 0xF,		// set run to AN
		ExE = (EN << 4) + EN,		// set run to EN, set current to EN
		NIx = (ON << 4) + 0xF + IX,	// set run to N, increment
		NxN = (ON << 4) + ON,		// set run to N, set current to N
		NxR = (ON << 4) + R,		// set run to N, set current to R
		NxE = (ON << 4) + EN,		// set run to N, set current to EN

		AxA = (AN << 4) + AN,		// set run to AN, set current to AN
		NxL = (ON << 4) + L,		// set run to N, set current to L
		LxL = (L << 4) + L,			// set run to L, set current to L
	};
	static int actionWeak[][10];

	static int GetDeferredType( int action );
	static int GetResolvedType( int action );
	static bool IsDeferredState( int state );
	static bool IsModifiedClass( int cls );

	static const int state_first;
	static const int state_last;

	static const int cls_first;
	static const int cls_last;

	static int VerifyTables();
	static void resolveWeak( int baselevel, int *pcls, int * plevel, int cch );
// === RESOLVE NEUTAL TYPES ==============================================
	// action values
	enum neutral_action
	{
		// action to resolve previous input
		nL = L, 		// resolve EN to L
		En = 3 << 4,	// resolve neutrals run to embedding level direction
		Rn = R << 4,	// resolve neutrals run to strong right
		Ln = L << 4,	// resolved neutrals run to strong left
		In = (1<<8),	// increment count of deferred neutrals
		LnL = (1<<4)+L, // set run and EN to L
	};

	static int GetDeferredNeutrals( int action, int level );
	static int GetResolvedNeutrals( int action );

	// state values
	enum neutral_state
	{
		// new temporary class
		r,	// R and characters resolved to R
		l,	// L and characters resolved to L
		rn, // N preceded by right
		ln, // N preceded by left
		a,	// AN preceded by left (the abbrev 'la' is used up above)
		na, // N preceded by a
	};

	static int actionNeutrals[][5];
	static int stateNeutrals[][5];
	static void resolveNeutrals( int baselevel, int *pcls, const int *plevel, int cch );
// === RESOLVE IMPLLICIT =================================================
	static int addLevel[][4];
	static void resolveImplicit( const int * pcls, int * plevel, int cch );
// === REORDER ===========================================================
	static int resolveLines( __EXT_MFC_SAFE_LPTSTR pszInput, bool * pbrk, int cch );
	static void resolveWhitespace( int baselevel, const int *pcls, int *plevel, int cch );
	static int reorderLevel( int level, __EXT_MFC_SAFE_LPTSTR pszText, const int * plevel, int cch, bool fReverse = false );
	static int reorder( int baselevel, __EXT_MFC_SAFE_LPTSTR pszText, const int * plevel, int cch );
// === DISPLAY OPTIONS ================================================
	static void mirror( __EXT_MFC_SAFE_LPTSTR pszInput, const int * plevel, int cch );
	static int clean( __EXT_MFC_SAFE_LPTSTR pszInput, int cch );
}; /// class CExtRichBidiAlgorithm

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_RICH_CONTENT_H)
