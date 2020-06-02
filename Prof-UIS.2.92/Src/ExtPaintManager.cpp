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

#include "StdAfx.h"

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __EXTDOCKBAR_H)
	#include <../Src/ExtDockBar.h>
#endif

#if (!defined __EXT_STATUSCONTROLBAR_H)
	#include <ExtStatusControlBar.h>
#endif

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
		#include "ExtControlBarTabbedFeatures.h"
	#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H
#endif

#if( !defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
#endif

#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
	#if (!defined __EXTSHORTCUTLISTWND_H)
		#include <ExtShortcutListWnd.h>
	#endif
#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)

#if (!defined __EXT_BUTTON_H)
	#include <ExtButton.h>
#endif

#if (!defined __EXT_GROUPBOX_H)
	#include <ExtGroupBox.h>
#endif

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __EXT_MFC_NO_REPORTGRIDWND)
	#if (!defined __EXT_REPORT_GRID_WND_H)
		#include <ExtReportGridWnd.h>
	#endif
#endif 

#if (!defined __EXT_MFC_NO_DATE_PICKER)
	#if (!defined __EXT_DATE_PICKER_H)
		#include <ExtDatePicker.h>
	#endif
#endif 

#if (!defined __EXT_MFC_NO_TOOLBOX_CTRL)
	#if (!defined __EXTTOOLBOXWND_H)
		#include <ExtToolBoxWnd.h>
	#endif
#endif 

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	#if (!defined __EXT_RIBBON_BAR_H)
		#include <ExtRibbonBar.h>
	#endif // (!defined __EXT_RIBBON_BAR_H)
#endif

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	#if (!defined __EXT_POPUP_CTRL_MENU_H)
		#include <ExtPopupCtrlMenu.h>
	#endif
#endif

#if (!defined __EXT_MFC_NO_GRIDWND)
	#if (!defined __EXT_GRIDWND_H)
		#include <ExtGridWnd.h>
	#endif 
#endif

#if (!defined __EXT_TEMPL_H)
	#include <ExtTempl.h>
#endif

#if (!defined __EXT_SPIN_H)
	#include <ExtSpinWnd.h>
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __EXT_MFC_NO_DURATIONWND)
	#if (!defined __EXT_DURATIONWND_H)
		#include "ExtDurationWnd.h"
	#endif
#endif

#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
	#if (!defined __EXT_PAGECONTAINERWND_H)
		#include <ExtPageContainerWnd.h>
	#endif
#endif

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXT_EDIT_H)
	#include <ExtEdit.h>
#endif

#if (!defined __EXT_COMBO_BOX_H)
	#include <ExtComboBox.h>
#endif

#if (! defined __VSSYM32_H__)
	#include <vssym32/vssym32.h>
#endif // (! defined __VSSYM32_H__)

#include <math.h>
#include <shlwapi.h>

#include <Resources/Resource.h>

#if (! defined CLEARTYPE_QUALITY )
	#define CLEARTYPE_QUALITY 5
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// VC+ 6.0 static builds specific: disable warnings 4305, 4309
// warning C4305: 'initializing' : truncation from 'int' to 'short'
// warning C4309: 'initializing' : truncation of constant value
#if _MFC_VER < 0x700
	#ifdef __EXT_PROFUIS_STATIC_LINK
		#pragma warning( push )
		#pragma warning ( disable : 4305 )
		#pragma warning ( disable : 4309 )
	#endif
#endif

// win9x balloon-rect region fix constants
#define __WIN9X_FIXED_BAR_ROUNDED_AREA_SHIFT_X		2
#define __WIN9X_FIXED_BAR_ROUNDED_AREA_SHIFT_Y		2
#define __WIN9X_FIXED_BAR_ROUNDED_AREA_ADJUST_X		3
#define __WIN9X_FIXED_BAR_ROUNDED_AREA_ADJUST_Y		3

//#define __TRACE_PAINT_SESSIONS

CExtPaintManager::CExtPaintManager()
	: m_bUseTahomaFont( g_PaintManager.m_bIsWin2000orLater )
	, m_eStyle2005( CExtPaintManager::__ES2005_RC )
	, m_bCustomBackgroundInheritanceEnabled( false )
	, m_clrForceSplitterBk( COLORREF(-1L) )
	, m_bScrollBarsHaveHoverEffectIn2kStyle( false )
#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
	, m_bPageContainerUseButtonStyle( false )
	, m_bPageContainerUseGroupBoxStyle( false )
	, m_bPageContainerNoEmbossDisabledText( false )
#endif // (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
	, m_bShortcutListUseButtonStyleInScrollButtons( false )
	, m_bShortcutListUseDockerBkgnd( false )
	, m_bShortcutListItemUseButtonStyle( false )
	, m_clrShortcutListItemAlternativeText( COLORREF(-1L) )
#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
{
	ASSERT( this != NULL );

	m_DSS_DISABLED_style = DSS_DISABLED;

#define __3D_COLOR_DEPTH_R 7
#define __3D_COLOR_DEPTH_G 6
#define __3D_COLOR_DEPTH_B 6

struct
{
    LOGPALETTE    LogPalette;
    PALETTEENTRY  PalEntry[ (__3D_COLOR_DEPTH_R*__3D_COLOR_DEPTH_G*__3D_COLOR_DEPTH_B) ];
} pal;
LOGPALETTE * pLogPalette = (LOGPALETTE*) &pal;
	pLogPalette->palVersion    = 0x300;
	pLogPalette->palNumEntries = (WORD)(__3D_COLOR_DEPTH_R*__3D_COLOR_DEPTH_G*__3D_COLOR_DEPTH_B); 
int i = 0;
    for( int nX = 0; nX < __3D_COLOR_DEPTH_R; nX++ )
    {
		for( int nY = 0; nY < __3D_COLOR_DEPTH_G; nY++ )
		{
			for( int nZ = 0; nZ < __3D_COLOR_DEPTH_B; nZ++ )
			{
				pLogPalette->palPalEntry[i].peRed   =
					BYTE( (nX*255)/(__3D_COLOR_DEPTH_R-1) );
				pLogPalette->palPalEntry[i].peGreen =
					BYTE( (nY*255)/(__3D_COLOR_DEPTH_G-1) );
				pLogPalette->palPalEntry[i].peBlue  =
					BYTE( (nZ*255)/(__3D_COLOR_DEPTH_B-1) );
				pLogPalette->palPalEntry[i].peFlags = 0;
				i++;
			}
		}
    }
    VERIFY( m_PaletteWide.CreatePalette(pLogPalette) );

}

CExtPaintManager::~CExtPaintManager()
{
	ASSERT( this != NULL );

	m_colors.RemoveAll();
	m_brushes.RemoveAll();
}

CExtBitmap & CExtPaintManager::_GetBmpCacheFilter( bool bApplied ) const
{
	if( m_bmpCacheFilterApplied.IsEmpty() )
	{
		VERIFY( m_bmpCacheFilterApplied.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_FILTER_APPLIED ) ) );
		VERIFY( m_bmpCacheFilterApplied.Make32() );
		m_bmpCacheFilterApplied.AlphaColor( RGB(255,0,255), RGB(0,0,0), BYTE(0) );
	}
	if( m_bmpCacheFilterUnApplied.IsEmpty() )
	{
		VERIFY( m_bmpCacheFilterUnApplied.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_FILTER_UNAPPLIED ) ) );
		VERIFY( m_bmpCacheFilterUnApplied.Make32() );
		m_bmpCacheFilterUnApplied.AlphaColor( RGB(255,0,255), RGB(0,0,0), BYTE(0) );
	}
	return bApplied ? m_bmpCacheFilterApplied : m_bmpCacheFilterUnApplied;
}

void CExtPaintManager::SerializeSynchronizationData( CArchive & ar )
{
	ASSERT_VALID( this );
	ar;
}

bool CExtPaintManager::UiScalingIsEnabled(
	CExtPaintManager::e_ui_scaling_type_t eUIST
	) const
{
	ASSERT_VALID( this );
	ASSERT( __EUIST_MIN_VALUE <= eUIST && eUIST <= __EUIST_MAX_VALUE );
bool bEnaled = false;
	switch( eUIST )
	{
	case __EUIST_X:
		bEnaled = g_PaintManager.m_bEnableUiScalingX;
	break;
	case __EUIST_Y:
		bEnaled = g_PaintManager.m_bEnableUiScalingY;
	break;
	case __EUIST_Z:
		bEnaled = g_PaintManager.m_bEnableUiScalingZ;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eUIST )
	return bEnaled;
}

INT CExtPaintManager::UiScalingDo(
	INT nValue,
	CExtPaintManager::e_ui_scaling_type_t eUIST
	) const
{
	ASSERT_VALID( this );
	ASSERT( __EUIST_MIN_VALUE <= eUIST && eUIST <= __EUIST_MAX_VALUE );
INT nNewValue = nValue;
	if( UiScalingIsEnabled( eUIST ) )
	{
		switch( eUIST )
		{
		case __EUIST_X:
			nNewValue = ::MulDiv( nNewValue, g_PaintManager.m_nLPX, 96 );
		break;
		case __EUIST_Y:
			nNewValue = ::MulDiv( nNewValue, g_PaintManager.m_nLPY, 96 );
		break;
		case __EUIST_Z:
			nNewValue = ::MulDiv( nNewValue, g_PaintManager.m_nLPZ, 96 );
		break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
		break;
#endif // _DEBUG
		} // switch( eUIST )
	} // if( UiScalingIsEnabled( eUIST ) )
	return nNewValue;
}

CExtPaintManagerXP::CExtPaintManagerXP()
	: m_bEnabledHoverIconShadows( true )
	, m_bHelperXpStyle8BitBarRect( true )
	, m_bExpBtnSwapVGlyphs( false )
	, m_pGlyphTbEpBtnH0( &g_glyph_btn_expand_bottom )
	, m_pGlyphTbEpBtnH1( &g_glyph_btn_expand_right2 )
	, m_pGlyphTbEpBtnV0( &g_glyph_btn_expand_left )
	, m_pGlyphTbEpBtnV1( &g_glyph_btn_expand_bottom2 )
	, m_nIdxClrMlaNormLeft( -1 )
	, m_nIdxClrMlaNormMiddle( -1 )
	, m_nIdxClrMlaNormRight( -1 )
	, m_nIdxClrMlaRarelyLeft( -1 )
	, m_nIdxClrMlaRarelyMiddle( -1 )
	, m_nIdxClrMlaRarelyRight( -1 )
	, m_nIdxClrBtnHoverLeft( -1 )
	, m_nIdxClrBtnHoverRight( -1 )
	, m_nIdxClrBtnPressedLeft( -1 )
	, m_nIdxClrBtnPressedRight( -1 )
	, m_nIdxClrBtnHovPresLeft( -1 )
	, m_nIdxClrBtnHovPresRight( -1 )
	, m_clrCustomDropDownGlyphEnabled( COLORREF(-1L) )
	, m_clrCustomDropDownGlyphDisabled( COLORREF(-1L) )
{
#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
	m_bPageContainerNoEmbossDisabledText = true;
#endif // (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
}

CExtPaintManagerXP::~CExtPaintManagerXP()
{
}

//////////////////////////////////////////////////////////////////////////
// CExtPaintManagerOffice2003
//////////////////////////////////////////////////////////////////////////

bool CExtPaintManager::ScrollBar_GetMetrics(
	INT * p_nHorzBarHeight, // = NULL
	INT * p_nVertBarWidth, // = NULL
	INT * p_nHorzButtonWidth, // = NULL
	INT * p_nVertButtonHeight, // = NULL
	INT * p_nHorzThumbMinWidth, // = NULL
	INT * p_nVertThumbMinHeight, // = NULL
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	p_nHorzBarHeight; p_nVertBarWidth; p_nHorzButtonWidth; p_nVertButtonHeight; p_nHorzThumbMinWidth; p_nVertThumbMinHeight; pHelperSrc; lParam;
	return false;
}

void CExtPaintManager::ScrollBar_Paint(
	CDC & dc,
	CExtPaintManager::PAINTSCROLLBARDATA & _psbd
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	_psbd.PaintDefault( dc, const_cast < CExtPaintManager * > ( this ) );
}

CExtPaintManager::SCROLLBARSKINDATA * CExtPaintManager::ScrollBar_GetSkinData(
	bool bHorzBar,
	CObject * pHelperSrc,
	LPARAM lParam, // = 0L
	bool bLightAccent // = true
	)
{
	ASSERT_VALID( this );
	bHorzBar; pHelperSrc; lParam; bLightAccent;
	return NULL;
}

#ifndef __EXT_MFC_NO_GRIDWND

bool CExtPaintManager::Grid_PaintButton(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	lParam;
	if( pHelperSrc == NULL )
		return false;
	ASSERT_VALID( pHelperSrc );
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	ASSERT( pWnd != NULL );

bool bPaintPushButton = true;
	if(		(	nButtonType == INT(CExtGridCell::__EBTT_FILTER_APPLIED)
			||	nButtonType == INT(CExtGridCell::__EBTT_FILTER_UNAPPLIED)
			)
		&&	(	(! bEnabled )
			||	(! ( bFocused || bHovered || bPressed ) )
			)
		)
		bPaintPushButton = false;

CRect rc( rcButton );
	if( bPaintPushButton )
		dc.FillSolidRect( &rc, clrWindow );

bool bDefaultDrawing = true;
CExtPaintManager::glyph_t * pGlyph = NULL;
CExtBitmap * pBitmap = NULL;
	if( g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		INT nPartID = -1;
		INT nState = 0;
		__EXT_UX_LPCWSTR pszClassList = NULL;
		switch( nButtonType ) 
		{
		case INT(CExtGridCell::__EBTT_DROPDOWN):
			pszClassList = VSCLASS_COMBOBOX;
			nPartID = CP_DROPDOWNBUTTON;
			nState = 
				(!bEnabled)
					? CBXS_DISABLED
					: bPressed
						? CBXS_PRESSED
						: bHovered
							? CBXS_HOT
							: CBXS_NORMAL;
			break;
		case INT(CExtGridCell::__EBTT_ELLIPSIS):
			pszClassList = VSCLASS_BUTTON;
			nPartID = BP_PUSHBUTTON;
			nState = 
				(!bEnabled)
					? PBS_DISABLED
					: bPressed
						? PBS_PRESSED
						: bHovered
							? PBS_HOT
							: ( bFocused )
								? PBS_DEFAULTED
								: PBS_NORMAL;		
			if( (dwHelperPaintFlags&__EGCPF_NO_BUTTON_ELLIPSIS_DOTS) == 0 )
				pGlyph = 
					CExtPaintManager::g_DockingCaptionGlyphs[
						(INT) CExtPaintManager::__DCBT_ELLIPSIS
						];
			break;
		case INT(CExtGridCell::__EBTT_UPDOWN_UP):
			pszClassList = VSCLASS_SPIN;
			nPartID = SPNP_UP;
			nState = 
				(!bEnabled) 
					? UPS_DISABLED 
					: bPressed 
						? UPS_PRESSED 
						: bHovered
							? UPS_HOT
							: UPS_NORMAL;
			break;
		case INT(CExtGridCell::__EBTT_UPDOWN_DOWN):
			pszClassList = VSCLASS_SPIN;
			nPartID = SPNP_DOWN;
			nState = 
				(!bEnabled) 
					? DNS_DISABLED 
					: bPressed 
						? DNS_PRESSED 
						: bHovered
							? DNS_HOT
							: DNS_NORMAL;
			break;
		case INT(CExtGridCell::__EBTT_FILTER_APPLIED):
			{
				CExtBitmap & aBmp = _GetBmpCacheFilter( true );
				pBitmap = &aBmp;
			}
		break;
		case INT(CExtGridCell::__EBTT_FILTER_UNAPPLIED):
			{
				CExtBitmap & aBmp = _GetBmpCacheFilter( false );
				pBitmap = &aBmp;
			}
		break;
		} // switch( nButtonType ) 

		if(		bPaintPushButton
			&&	nPartID != -1
			&&	pszClassList != NULL
			&&	g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), pszClassList ) != NULL 
			) 
		{
			if( g_PaintManager.m_UxTheme.DrawThemeBackground(
					dc.GetSafeHdc(), 
					nPartID, 
					nState, 
					&rc, 
					&rcButton
					) == S_OK
				)
				bDefaultDrawing = false;
			g_PaintManager.m_UxTheme.CloseThemeData( true );
		}
	}

	if( bDefaultDrawing )
	{
		COLORREF clrOuterLT =
			bPressed ? clrShadow : clrLight;
		COLORREF clrOuterRB =
			bPressed ? clrLight : clrShadow;

		if( bPaintPushButton )
			dc.FillSolidRect( &rc, clrFace );
		rc.DeflateRect( 2, 2 );
		if( bPaintPushButton )
			dc.Draw3dRect( 
				&rcButton, 
				clrOuterLT, 
				clrOuterRB
				);
		if( bPressed )
			rc.OffsetRect( 0, 1 );
		if( nButtonType == INT(CExtGridCell::__EBTT_DROPDOWN) )
		{
			if( (dwHelperPaintFlags&__EGCPF_NO_BUTTON_DROPDOWN_ARROW) == 0 )
				pGlyph = 
					CExtPaintManager::g_DockingCaptionGlyphs[
						(INT) CExtPaintManager::__DCBT_ARROW_DOWN
						];
		}
		else if( nButtonType == INT(CExtGridCell::__EBTT_ELLIPSIS) )
		{
			if( (dwHelperPaintFlags&__EGCPF_NO_BUTTON_ELLIPSIS_DOTS) == 0 )
				pGlyph = 
					CExtPaintManager::g_DockingCaptionGlyphs[
						(INT) CExtPaintManager::__DCBT_ELLIPSIS
						];
		}
		else if( nButtonType == INT(CExtGridCell::__EBTT_UPDOWN_UP) )
		{
			pGlyph = 
				CExtPaintManager::g_DockingCaptionGlyphs[
					(INT) CExtPaintManager::__DCBT_ARROW_SMALL_UP
					];
		}
		else if( nButtonType == INT(CExtGridCell::__EBTT_UPDOWN_DOWN) )
		{
			pGlyph = 
				CExtPaintManager::g_DockingCaptionGlyphs[
					(INT) CExtPaintManager::__DCBT_ARROW_SMALL_DOWN
					];
		}
		else if( nButtonType == INT(CExtGridCell::__EBTT_FILTER_APPLIED) )
		{
			CExtBitmap & aBmp = _GetBmpCacheFilter( true );
			pBitmap = &aBmp;
		}
		else if( nButtonType == INT(CExtGridCell::__EBTT_FILTER_UNAPPLIED) )
		{
			CExtBitmap & aBmp = _GetBmpCacheFilter( false );
			pBitmap = &aBmp;
		}
	} // if( bDefaultDrawing )

	if( pGlyph != NULL )
	{
		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			RGB(0,0,0),
		};
		ColorValues[1] = 
			bEnabled ? clrText : clrLight;
		if( ! bEnabled )
			rc.OffsetRect( 1, 1 );
		PaintGlyphCentered(
			dc,
			rc,
			*pGlyph,
			ColorValues
			);
		if( ! bEnabled )
		{
			rc.OffsetRect( -1, -1 );
			ColorValues[1] = clrShadow;
			PaintGlyphCentered(
				dc,
				rc,
				*pGlyph,
				ColorValues
				);
		} // if( ! bEnabled )
	} // if( pGlyph != NULL )	

	if(		pBitmap != NULL
		&&	(! pBitmap->IsEmpty() )
		)
	{
		CRect rcDst( rc.TopLeft(), pBitmap->GetSize() );
		rcDst.OffsetRect(
			( rc.Width() - rcDst.Width() ) / 2,
			( rc.Height() - rcDst.Height() ) / 2
			);
		pBitmap->AlphaBlend( dc.m_hDC, rcDst );
	} // if( pBitmap != NULL ...

	if(		bFocused 
		&&	nButtonType == INT(CExtGridCell::__EBTT_ELLIPSIS)
		)
	{
		CRect rcFocusRect( rcButton );
		if( bPressed )
		{
			CSize sizePushedOffset = GetPushedOffset();
			rcFocusRect.OffsetRect( -sizePushedOffset );
		}
		rcFocusRect.DeflateRect( 3, 3 );
		dc.DrawFocusRect( &rcFocusRect );
	}

	return true;
}

bool CExtPaintManager::Grid_PaintHeaderSortArrow(
	CDC & dc,
	const RECT & rcSortArrow,
	bool bHorz,
	bool bAscending,
	COLORREF * pColorValues,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	pHelperSrc;
	lParam;
	if( pColorValues == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	PaintGlyphCentered(
		dc,
		rcSortArrow,
		*( g_DockingCaptionGlyphs[
			bHorz
				? ( bAscending
						? __DCBT_ARROW_SORT_LEFT
						: __DCBT_ARROW_SORT_RIGHT )
				: ( bAscending
						? __DCBT_ARROW_SORT_UP
						: __DCBT_ARROW_SORT_DOWN )
			] ),
		pColorValues
		);	
	return true;
}

bool CExtPaintManagerNativeXP::Grid_PaintHeaderSortArrow(
	CDC & dc,
	const RECT & rcSortArrow,
	bool bHorz,
	bool bAscending,
	COLORREF * pColorValues,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	pColorValues;
	pHelperSrc;
	lParam;
COLORREF ColorValues[2] =
{
	RGB(0,0,0),
	::GetSysColor(COLOR_3DSHADOW),
};
	PaintGlyphCentered(
		dc,
		rcSortArrow,
		*( g_DockingCaptionGlyphs[
			bHorz
				? ( bAscending
						? __DCBT_2007_ARROW_SORT_LEFT
						: __DCBT_2007_ARROW_SORT_RIGHT )
				: ( bAscending
						? __DCBT_2007_ARROW_SORT_UP
						: __DCBT_2007_ARROW_SORT_DOWN )
			] ),
		ColorValues
		);
	return true;
}

COLORREF CExtPaintManager::Grid_OnQueryTextColor(
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
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&_cell) );
//	if( (dwHelperPaintFlags&__EGCPF_PRINTING_TARGET_MASK) != 0 )
//		return COLORREF(-1L);
	_cell;
	dc;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	dwAreaFlags;
	dwHelperPaintFlags;
	// used default cell' text color computed
	// by the CExtGridCell::OnQueryTextColor() virtual method
	return COLORREF(-1L);
}

bool CExtPaintManager::Grid_PaintHeaderBackground(
	CDC & dc,
	const RECT & rcArea,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcArea;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	dwAreaFlags;
	dwHelperPaintFlags;
	pHelperSrc;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::Grid_PaintHeaderBackground(
	CDC & dc,
	const RECT & rcArea,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcArea;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	dwAreaFlags;
	dwHelperPaintFlags;
	pHelperSrc;
	lParam;
	return false;
}

#endif // #ifndef __EXT_MFC_NO_GRIDWND
	
#if (!defined __EXT_MFC_NO_REPORTGRIDWND)

bool CExtPaintManager::ReportGrid_PaintHeaderRowBackground(
	CDC & dc,
	const RECT & rcArea,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcArea;
	pHelperSrc;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::ReportGrid_PaintHeaderRowBackground(
	CDC & dc,
	const RECT & rcArea,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
bool bDefaultDrawing = true, bDrawCell = false;
	if( pHelperSrc != NULL )
	{
		if( pHelperSrc->IsKindOf(RUNTIME_CLASS(CExtReportGridColumn)) )
			bDrawCell = true;
	}
	if( g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		HWND hWnd = NULL;
		if( pHelperSrc != NULL )
		{
			if( pHelperSrc->IsKindOf( RUNTIME_CLASS(CWnd) ) )
				hWnd = ((CWnd*)pHelperSrc)->GetSafeHwnd();
			if( hWnd == NULL )
			{
				if( pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtReportGridColumn) ) )
					hWnd = ((CExtReportGridColumn*)pHelperSrc)->GetReportGrid()->GetSafeHwnd();
				if( hWnd == NULL )
					hWnd = ::GetDesktopWindow();
			} // if( hWnd == NULL )
		} // if( pHelperSrc != NULL )
		if(		hWnd != NULL
			&&	g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_HEADER ) != NULL
			)
		{
			if( g_PaintManager.m_UxTheme.DrawThemeBackground(
					dc.GetSafeHdc(), 
					HP_HEADERITEMRIGHT, 
					HIRS_NORMAL, 
					&rcArea, 
					&rcArea
					) == S_OK
				)
				bDefaultDrawing = false;
			g_PaintManager.m_UxTheme.CloseThemeData( true );
		}
		if( (! bDefaultDrawing ) && bDrawCell )
		{
			ASSERT( hWnd != NULL );
			if(	g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_HEADER ) != NULL )
			{
				INT nStateID = HIS_NORMAL;
				if(		pHelperSrc != NULL
					&&	pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtReportGridColumn) )
					&&	(	(lParam&(__EGCPF_HOVER_BY_COLUMN|__EGCPF_OUTER_DND)) != 0
						||	((CExtReportGridColumn*)pHelperSrc)->GetReportGrid()->m_bHelperRenderingOuterDND
						)
					)
					nStateID = HIS_HOT;
				if( g_PaintManager.m_UxTheme.DrawThemeBackground(
						dc.GetSafeHdc(), 
						HP_HEADERITEM, 
						nStateID, 
						&rcArea, 
						&rcArea
						) == S_OK
					)
					bDefaultDrawing = false;
				g_PaintManager.m_UxTheme.CloseThemeData( true );
			}
		} // if( (! bDefaultDrawing ) && bDrawCell )
	} // if( g_PaintManager.m_UxTheme.IsControlsThemed() )
	if( ! bDefaultDrawing )
		return true;
	return
		CExtPaintManager::ReportGrid_PaintHeaderRowBackground(
			dc,
			rcArea,
			pHelperSrc,
			lParam
			);
}

bool CExtPaintManager::ReportGrid_PaintHeaderSortArrow(
	CDC & dc,
	const RECT & rcSortArrow,
	bool bAscending,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcSortArrow;
	bAscending;
	pHelperSrc;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::ReportGrid_PaintHeaderSortArrow(
	CDC & dc,
	const RECT & rcSortArrow,
	bool bAscending,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
//bool bDefaultDrawing = true;
//	if( g_PaintManager.m_UxTheme.IsControlsThemed() )
//	{
//		HWND hWnd = NULL;
//		if( pHelperSrc != NULL )
//		{
//			if( pHelperSrc->IsKindOf( RUNTIME_CLASS(CWnd) ) )
//				hWnd = ((CWnd*)pHelperSrc)->GetSafeHwnd();
//			if( hWnd == NULL )
//			{
//				if( pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtReportGridColumn) ) )
//					hWnd = ((CExtReportGridColumn*)pHelperSrc)->GetReportGrid()->GetSafeHwnd();
//				if( hWnd == NULL )
//					hWnd = ::GetDesktopWindow();
//			} // if( hWnd == NULL )
//		} // if( pHelperSrc != NULL )
//		if(		hWnd != NULL
//			&&	g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_HEADER ) != NULL
//			)
//		{
//			if( g_PaintManager.m_UxTheme.DrawThemeBackground(
//					dc.GetSafeHdc(), 
//					HP_HEADERSORTARROW, 
//					bAscending ? HSAS_SORTEDUP : HSAS_SORTEDDOWN, 
//					&rcSortArrow, 
//					&rcSortArrow
//					) == S_OK
//				)
//				bDefaultDrawing = false;
//			g_PaintManager.m_UxTheme.CloseThemeData( true );
//		}
//	} // if( g_PaintManager.m_UxTheme.IsControlsThemed() )
//	if( ! bDefaultDrawing )
//		return true;
//	return
//		CExtPaintManager::ReportGrid_PaintHeaderSortArrow(
//			dc,
//			rcSortArrow,
//			bAscending,
//			pHelperSrc,
//			lParam
//			);
	pHelperSrc;
	lParam;
COLORREF ColorValues[2] =
{
	RGB(0,0,0),
	::GetSysColor(COLOR_3DSHADOW),
};
	PaintGlyphCentered(
		dc,
		rcSortArrow,
		*( g_DockingCaptionGlyphs[
			bAscending
				? __DCBT_2007_ARROW_SORT_UP
				: __DCBT_2007_ARROW_SORT_DOWN
			] ),
		ColorValues
		);
	return true;
}

bool CExtPaintManager::ReportGrid_PaintGroupAreaBackground(
	CDC & dc,
	const RECT & rcArea,
	__EXT_MFC_SAFE_LPCTSTR strEmptyAreaCaption, // not NULL if area is empty
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcArea;
	strEmptyAreaCaption;
	pHelperSrc;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::ReportGrid_PaintGroupAreaBackground(
	CDC & dc,
	const RECT & rcArea,
	__EXT_MFC_SAFE_LPCTSTR strEmptyAreaCaption, // not NULL if area is empty
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcArea;
	strEmptyAreaCaption;
	pHelperSrc;
	lParam;
	return false;
}

bool CExtPaintManager::ReportGrid_PaintGroupAreaItemBackground(
	CDC & dc,
	const RECT & rcItem,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	dc;
	rcItem;
	pHelperSrc;
	lParam;
	return false;
}

COLORREF CExtPaintManager::ReportGrid_GetGroupConnectorColor(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return COLORREF(-1L);
}

COLORREF CExtPaintManager::ReportGrid_GetHeaderTextColor(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtReportGridColumn) )
		)
	{
		if( (lParam&__EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN) != 0 )
			return GetColor( COLOR_HIGHLIGHTTEXT, pHelperSrc, lParam );
		else
			return GetColor( COLOR_BTNTEXT, pHelperSrc, lParam );
	}
	return COLORREF(-1L);
}

COLORREF CExtPaintManagerNativeXP::ReportGrid_GetHeaderTextColor(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtReportGridColumn) )
		)
	{
		if( (lParam&__EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN) != 0 )
			return ::GetSysColor( COLOR_HIGHLIGHTTEXT );
		else
			return ::GetSysColor( COLOR_BTNTEXT );
	}
	return COLORREF(-1L);
}

#endif // (!defined __EXT_MFC_NO_REPORTGRIDWND)

CSize CExtPaintManager::GetTextSizes(
	bool bBold // = false
	)
{
	ASSERT_VALID( this );
CWindowDC dc( NULL );
CFont * pOldFont =
		dc.SelectObject(
			bBold ? &m_FontBold : &m_FontNormal
			);
	ASSERT( pOldFont != NULL );
TEXTMETRIC tm;
	dc.GetTextMetrics( &tm );
int nTextHeightHorz = tm.tmHeight + 2;
	dc.SelectObject( &m_FontNormal );
	dc.GetTextMetrics( &tm );
int nTextHeightVert = tm.tmHeight + 2;
	dc.SelectObject( pOldFont );
	return CSize(nTextHeightHorz,nTextHeightVert);
}

int CExtPaintManager::GetTextHeight(
	bool bHorz
	)
{
	ASSERT_VALID( this );
CSize _size = GetTextSizes();
	return bHorz ? _size.cx : _size.cy;
}

int CExtPaintManager::GetMenuExpandButtonHeight()
{
	ASSERT_VALID( this );
int nHeight = ::GetSystemMetrics(SM_CYMENUCHECK);
	ASSERT( nHeight > 0 );
	return nHeight;
}

int CExtPaintManagerXP::GetMenuExpandButtonHeight()
{
	ASSERT_VALID( this );
	return CExtPaintManager::GetMenuExpandButtonHeight();
}

void CExtPaintManager::PaintMenuScrollButton(
	CDC & dc,
	bool bHover,
	bool bPushed,
	bool bTopButton,
	const RECT & rcScrollButton,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	bPushed = bPushed || bHover;
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		pHelperSrc,
		true,
		rcScrollButton,
		bTopButton ? _T("t") : _T("u"), // up arrow / down arrow
		NULL,
		true,
		false, // bHover,
		bPushed,
		false,
		true,true,false,false,
		__ALIGN_HORIZ_CENTER | __ALIGN_VERT_CENTER,
		(HFONT)m_FontMarlett.GetSafeHandle()
		);
	_ppbd.m_lParam = lParam;
	PaintPushButton( dc, _ppbd );
}

void CExtPaintManagerXP::PaintMenuScrollButton(
	CDC & dc,
	bool bHover,
	bool bPushed,
	bool bTopButton,
	const RECT & rcScrollButton,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
bool bTransparentBk = false;
bool bDrawBorder = true;
	bPushed = bPushed || bHover;
	if( bPushed )
	{
		PAINTMENUITEMDATA _pmid; // fake
		_pmid.m_bSelected = true;
		if(		PaintMenuItemBackgroundXP( dc, _pmid, rcScrollButton )
			||	PaintMenuItemSelectionAreaXP( dc, _pmid, rcScrollButton ) 
			)
		{
			bTransparentBk = true;
			bDrawBorder = false;
			bPushed = bHover = false;
		}
	}
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		pHelperSrc,
		true,	// bHorz
		rcScrollButton,
		bTopButton ? _T("t") : _T("u"), // up arrow / down arrow
		NULL,	// pIcon
		true,	// bFlat
		bHover,
		bPushed,
		false,	// bIndeterminate
		true,	// bEnabled
		bDrawBorder,
		false,	// bDrawFocusRect
		false,	// bDefaultPushButton
		__ALIGN_HORIZ_CENTER | __ALIGN_VERT_CENTER,
		(HFONT)m_FontMarlett.GetSafeHandle(),
		false,	// bDropDown
		0,		// nHelperSysCmdID
		bTransparentBk
		);
	_ppbd.m_lParam = lParam;
	PaintPushButton( dc, _ppbd );
}

void CExtPaintManager::PaintMenuTearOffButton(
	CDC & dc,
	const RECT & rectButton,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bPushed;
	bHover;
	pHelperSrc;
	lParam;
CRect rcGlyph( rectButton );
	rcGlyph.DeflateRect( 1, 0, 0, 1 );
	dc.FillSolidRect(
		&rcGlyph,
		GetColor( COLOR_HIGHLIGHT )
		);
}

void CExtPaintManagerXP::PaintMenuTearOffButton(
	CDC & dc,
	const RECT & rectButton,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	pHelperSrc;
	lParam;
CRect rcGlyph( rectButton );
	rcGlyph.DeflateRect( 0, 0, 0, 1 );
	dc.FillSolidRect( &rcGlyph, GetColor( ( bHover || bPushed ) ? CLR_3DFACE_IN : XPCLR_3DFACE_DARK ) );
static const int g_nXpTearOffBtnLinesWidth = 33;
CRect rcLines = rcGlyph;
	rcLines.OffsetRect( (rcLines.Width() - g_nXpTearOffBtnLinesWidth) / 2, 0 );
	rcLines.right = rcLines.left + g_nXpTearOffBtnLinesWidth;
	if( bHover || bPushed )
	{
		COLORREF clrBorder = GetColor( XPCLR_HILIGHT_BORDER );
		dc.Draw3dRect( &rcGlyph, clrBorder, clrBorder );
		rcLines.DeflateRect( 0, 2 );
		COLORREF clrLine = GetColor( COLOR_BTNTEXT );
		int yStep = rcLines.top;
		for( ; yStep < rcLines.bottom; yStep += 2 )
			dc.FillSolidRect( rcLines.left, yStep, rcLines.Width(), 1, clrLine );
	} // if( bHover || bPushed )
	else
	{
		rcLines.DeflateRect( 0, 1 );
		COLORREF clrLine = GetColor( COLOR_3DSHADOW );
		int yStep = rcLines.top;
		for( ; yStep < rcLines.bottom; yStep += 2 )
			dc.FillSolidRect( rcLines.left, yStep, rcLines.Width(), 1, clrLine );
	} // else from if( bHover || bPushed )
}

void CExtPaintManager::PaintMenuExpandButton(
	CDC & dc,
	const RECT & rectButton,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bHover;
	
	CRect rcGlyph(rectButton);
	rcGlyph.DeflateRect(1,0);

	if( bPushed )
	{
		dc.FillRect(
			&rcGlyph,
			&m_brushLightDefault
			);
		dc.Draw3dRect(
			&rcGlyph,
			GetColor( CLR_3DHILIGHT_OUT, pHelperSrc, lParam ),
			GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam )
			);
	}
	else
	{
		CExtPaintManager::PAINTMENUITEMDATA _pmid(
			pHelperSrc,
			rcGlyph, rectButton, 0, _T(""),
			_T(""), NULL,
			false, false, false,
			false, false, true, false, false, false,
			0, NULL
			);
		PaintMenuItem( dc, _pmid );
	}
COLORREF ColorValues[] =
{
	RGB(0,0,0),
	GetColor( CLR_MENUTEXT_OUT, pHelperSrc, lParam )
};
	PaintGlyphCentered(
		dc,
		rcGlyph,
		g_glyph_btn_expand_bottom2,
		ColorValues
		);
}

void CExtPaintManagerXP::PaintMenuExpandButton(
	CDC & dc,
	const RECT & rectButton,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bHover;
bool bRTL = false;
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
		CExtPopupMenuWnd * pPopup = DYNAMIC_DOWNCAST( CExtPopupMenuWnd, pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	} // if( pHelperSrc != NULL )
int nIconAreaWidth = 0;
	if(		pHelperSrc != NULL // (+ 2.23)
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) )
		&&	((CExtPopupMenuWnd*)pHelperSrc) ->
				ItemGetCount() > 0
		)
	{
		CExtPopupMenuWnd::MENUITEMDATA & _mii =
			((CExtPopupMenuWnd*)pHelperSrc) ->
				ItemGetInfo(
					((CExtPopupMenuWnd*)pHelperSrc) ->
						ItemGetCount()
					- 1
					);
		if( ! _mii.IsToolButton() )
			nIconAreaWidth = _mii.GetIconAreaWidth();
	}
CRect rcGlyph(rectButton);
	rcGlyph.DeflateRect( 1, 0 );
	if( bPushed )
	{
		CExtPaintManager::PAINTMENUITEMDATA _pmid; // fake
		_pmid.m_bSelected = true;
		if( ! PaintMenuItemBackgroundXP( dc, _pmid, rcGlyph ) )
		{
			if( ! PaintMenuItemSelectionAreaXP( dc, _pmid, rcGlyph ) )
			{
				PAINTPUSHBUTTONDATA _ppbd(
					pHelperSrc,
					true,rcGlyph,_T(""),NULL,
					true,false,bPushed,false,
					false,true,false,false,
					__ALIGN_HORIZ_CENTER | __ALIGN_VERT_CENTER,
					NULL,
					false
					);
				PaintPushButton( dc, _ppbd );
				rcGlyph.OffsetRect( GetPushedOffset() );
			}
		}
	}
	else
	{
		CExtPaintManager::PAINTMENUITEMDATA _pmid(
			pHelperSrc,
			rcGlyph, rectButton, nIconAreaWidth, _T(""),
			_T(""), NULL,
			false, false, false,
			false, false, true, false, false, false,
			0, NULL
			);
		PaintMenuItem( dc, _pmid );
	}
COLORREF ColorValues[] =
{
	RGB(0,0,0),
	GetColor( CLR_MENUTEXT_OUT, pHelperSrc, lParam )
};
	PaintGlyphCentered(
		dc,
		rcGlyph,
		g_glyph_btn_expand_bottom2,
		ColorValues
		);
}

void CExtPaintManagerNativeXP::PaintMenuExpandButton(
	CDC & dc,
	const RECT & rectButton,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintMenuExpandButton(
			dc,
			rectButton,
			bPushed,
			bHover,
			pHelperSrc,
			lParam
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )

CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
	
CRect rcGlyph(rectButton);
	rcGlyph.DeflateRect(1,0);
	
INT nPartID = TP_BUTTON;
INT nStateID = bHover ? TS_HOT : ( bPushed ? TS_PRESSED : TS_NORMAL );

	if( g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_TOOLBAR ) != NULL )
	{
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				pWnd->GetSafeHwnd(),
				dc.GetSafeHdc(), 
				nPartID, 
				nStateID, 
				&rcGlyph, 
				&rcGlyph
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}

	COLORREF ColorValues[] =
	{
		RGB(0,0,0),
		::GetSysColor( COLOR_BTNTEXT )
	};
	PaintGlyphCentered(
		dc,
		rcGlyph,
		g_glyph_btn_expand_bottom2,
		ColorValues
		);
}

CSize CExtPaintManager::FixedBar_GetRoundedAreaMerics() const
{
	ASSERT_VALID( this );
	return CSize( 0, 0 );
}

CSize CExtPaintManagerXP::FixedBar_GetRoundedAreaMerics() const
{
	ASSERT_VALID( this );
	return CExtPaintManager::FixedBar_GetRoundedAreaMerics();
}

void CExtPaintManager::PaintToolbarExpandButton(
	CDC & dc,
	const RECT & rcButtonArea,
	bool bHorz, // if false - down
	bool bBarIsCompletelyVisible,
	bool bEnabled,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam, // = 0L
	bool bTransparentBackground // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rect( rcButtonArea );
	rect.NormalizeRect();
	if( !dc.RectVisible( &rect ) )
		return;
CRect rectButton(rect);
const glyph_t * pGlyph = NULL, * pGlyph2 = NULL;
	if( bHorz )
	{
		pGlyph = &g_glyph_btn_expand_bottom;
		if( ! bBarIsCompletelyVisible )
			pGlyph2 = &g_glyph_btn_expand_right2;
	}
	else
	{
		pGlyph = &g_glyph_btn_expand_left;
		if( ! bBarIsCompletelyVisible )
			pGlyph2 = &g_glyph_btn_expand_bottom2;
	}
	ASSERT( pGlyph != NULL );
PAINTPUSHBUTTONDATA _ppbd(
		pHelperSrc,
		true,rect,_T(""),NULL,true,
		bHover,bPushed,false,bEnabled,
		true,false,false,
		__ALIGN_HORIZ_CENTER | __ALIGN_VERT_CENTER,
		NULL,false,0,bTransparentBackground
		);
	PaintPushButton( dc, _ppbd );
COLORREF clr = GetColor( bEnabled ? CLR_TEXT_OUT : COLOR_3DHILIGHT, pHelperSrc, lParam );
COLORREF ColorValues[] = { 0, clr };
	if( bPushed )
		rect.OffsetRect( GetPushedOffset() );
CRect rectGlyph(rectButton.TopLeft(),pGlyph->Size());
CRect rectGlyph2(rectGlyph);
CSize sizePushedOffset = GetPushedOffset();
	if( bPushed )
	{
		rectGlyph.OffsetRect( sizePushedOffset );
		rectGlyph2.OffsetRect( sizePushedOffset );
	}
	if( bHorz )
	{
		int nGap = (max(sizePushedOffset.cy,1)) * 3;
		rectGlyph.OffsetRect(
			( rectButton.Size().cx - pGlyph->Size().cx ) / 2,
			rectButton.Size().cy - pGlyph->Size().cy - nGap
			);
		if( !bBarIsCompletelyVisible )
		{
			ASSERT( pGlyph2 != NULL );
			rectGlyph2.OffsetRect(
				( rectButton.Size().cx - pGlyph2->Size().cx ) / 2,
				nGap
				);
		}
	}
	else
	{
		int nGap = (max(sizePushedOffset.cx,1)) * 3;
		rectGlyph.OffsetRect(
			nGap,
			(rectButton.Size().cy - pGlyph->Size().cy)/2
			);
		if( !bBarIsCompletelyVisible )
		{
			ASSERT( pGlyph2 != NULL );
			rectGlyph2.OffsetRect(
				rectButton.Size().cx - pGlyph2->Size().cx - nGap,
				( rectButton.Size().cy - pGlyph2->Size().cy ) / 2
				);
		}
	}
	if( bEnabled )
	{
		PaintGlyph( dc, rectGlyph.TopLeft(), *pGlyph, ColorValues );
		if( ! bBarIsCompletelyVisible )
		{
			ASSERT( pGlyph2 != NULL );
			PaintGlyph( dc, rectGlyph2.TopLeft(), *pGlyph2, ColorValues );
		}
	} // if( bEnabled )
	else
	{
		rectGlyph.OffsetRect( 1, 1 );
		PaintGlyph( dc, rectGlyph.TopLeft(), *pGlyph, ColorValues );
		rectGlyph.OffsetRect( -1, -1 );
		ColorValues[1] = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
		PaintGlyph( dc, rectGlyph.TopLeft(), *pGlyph, ColorValues );
	} // else from if( bEnabled )
}

void CExtPaintManagerXP::PaintToolbarExpandButton(
	CDC & dc,
	const RECT & rcButtonArea,
	bool bHorz, // if false - down
	bool bBarIsCompletelyVisible,
	bool bEnabled,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam, // = 0L
	bool bTransparentBackground // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rect( rcButtonArea );
	rect.NormalizeRect();
	if( ! dc.RectVisible( &rect ) )
		return;
CRect rectButton( rect );
const glyph_t * pGlyph = NULL, * pGlyph2 = NULL;
	if( bHorz )
	{
		pGlyph = m_pGlyphTbEpBtnH0;
		pGlyph2 = m_pGlyphTbEpBtnH1;
	} // if( bHorz )
	else
	{
		pGlyph = m_pGlyphTbEpBtnV0;
		pGlyph2 = m_pGlyphTbEpBtnV1;
	} // else from if( bHorz )
	ASSERT( pGlyph != NULL );
	ASSERT( pGlyph2 != NULL );
PAINTPUSHBUTTONDATA _ppbd(
		pHelperSrc,
		true,rect,_T(""),NULL,true,
		bHover,bPushed,false,bEnabled,
		true,false,false,
		__ALIGN_HORIZ_CENTER | __ALIGN_VERT_CENTER,
		NULL,false,0,bTransparentBackground
		);
	PaintPushButton( dc, _ppbd );
COLORREF clr = GetColor( bEnabled ? CLR_TEXT_OUT : COLOR_3DHILIGHT, pHelperSrc, lParam );
COLORREF ColorValues[] = { 0, clr };
	if( bPushed )
		rect.OffsetRect( GetPushedOffset() );
CRect rectGlyph( rectButton.TopLeft(), pGlyph->Size() );
CRect rectGlyph2( rectGlyph );
CSize sizePushedOffset = GetPushedOffset();
	if( bPushed )
	{
		rectGlyph.OffsetRect( sizePushedOffset );
		rectGlyph2.OffsetRect( sizePushedOffset );
	} // if( bPushed )
	if( bHorz )
	{
		int nGap = (max(sizePushedOffset.cy,1)) * 3;
		rectGlyph.OffsetRect(
			( rectButton.Size().cx - pGlyph->Size().cx ) / 2, // + 1
			rectButton.Size().cy - pGlyph->Size().cy - nGap
			);
		rectGlyph2.OffsetRect(
			( rectButton.Size().cx - pGlyph2->Size().cx ) / 2 + 1,
			nGap
			);
	} // if( bHorz )
	else
	{
		int nGap = (max(sizePushedOffset.cx,1)) * 3;
		rectGlyph.OffsetRect(
			nGap,
			( rectButton.Size().cy - pGlyph->Size().cy )/2 // + 1
			);
		rectGlyph2.OffsetRect(
			rectButton.Size().cx - pGlyph2->Size().cx - nGap,
			( rectButton.Size().cy - pGlyph2->Size().cy ) / 2 + 1
			);
		if( m_bExpBtnSwapVGlyphs )
		{
			CRect rcTmp( rectGlyph );
			rectGlyph = rectGlyph2;
			rectGlyph2 = rcTmp;
		}
	} // else from if( bHorz )
	if( bEnabled )
	{
		PaintGlyph( dc,rectGlyph.TopLeft(),*pGlyph,ColorValues );
		if( ! bBarIsCompletelyVisible )
			PaintGlyph( dc, rectGlyph2.TopLeft(), *pGlyph2, ColorValues );
	} // if( bEnabled )
	else
	{
		ColorValues[1] = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
		PaintGlyph( dc, rectGlyph.TopLeft(), *pGlyph, ColorValues );
	} // else from if( bEnabled )
}

void CExtPaintManagerNativeXP::PaintToolbarExpandButton(
	CDC & dc,
	const RECT & rcButtonArea,
	bool bHorz, // if false - down
	bool bBarIsCompletelyVisible,
	bool bEnabled,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam, // = 0L
	bool bTransparentBackground // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rect( rcButtonArea );
	rect.NormalizeRect();
	if( ! dc.RectVisible( &rect ) )
		return;
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintToolbarExpandButton(
			dc, rcButtonArea, bHorz, bBarIsCompletelyVisible,
			bEnabled, bPushed, bHover,
			pHelperSrc, lParam, bTransparentBackground
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
CExtBarButton * pTBB = DYNAMIC_DOWNCAST( CExtBarButton, pHelperSrc );
	ASSERT( pTBB != NULL );
	ASSERT_VALID( pTBB );
CExtToolControlBar * pBar  = pTBB->GetSafeBar();
	ASSERT( pBar != NULL );
	ASSERT_VALID( pBar );
	if( g_PaintManager.m_UxTheme.OpenThemeData( pBar->GetSafeHwnd(), VSCLASS_TOOLBAR ) != NULL )
	{
		INT nStateID = 
			bHover
				? TS_HOT
				: bPushed
					? TS_PRESSED
					: TS_NORMAL;
		//VERIFY(
			g_PaintManager.m_UxTheme.DrawBackground(
				pBar->GetSafeHwnd(),
				dc.GetSafeHdc(), 
				TP_BUTTON, 
				nStateID, 
				&rcButtonArea, 
				&rcButtonArea
				//) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
CRect rcButton( rect );
const glyph_t * pGlyph1 = NULL;
const glyph_t * pGlyph2 = NULL;
	if( bHorz )
	{
		pGlyph1 = &g_glyph_btn_expand_bottom;
		if( ! bBarIsCompletelyVisible )
			pGlyph2 = &g_glyph_btn_expand_right2;
	}
	else
	{
		pGlyph1 = &g_glyph_btn_expand_left;
		if( ! bBarIsCompletelyVisible )
			pGlyph2 = &g_glyph_btn_expand_bottom2;
	}
	ASSERT( pGlyph1 != NULL );
COLORREF ColorValues[] = { 0, ::GetSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT ) };
CRect rectGlyph1( rcButton.TopLeft(), pGlyph1->Size() );
CRect rectGlyph2( rectGlyph1 );
	if( bHorz )
	{
		INT nGap = 3;
		rectGlyph1.OffsetRect(
			( rcButton.Size().cx - pGlyph1->Size().cx ) / 2,
			rcButton.Size().cy - pGlyph1->Size().cy - nGap
			);
		if( pGlyph2 != NULL )
			rectGlyph2.OffsetRect(
				( rcButton.Size().cx - pGlyph2->Size().cx ) / 2,
				nGap
				);
	}
	else
	{
		INT nGap = 3;
		rectGlyph1.OffsetRect(
			nGap,
			(rcButton.Size().cy - pGlyph1->Size().cy)/2
			);
		if( pGlyph2 != NULL )
			rectGlyph2.OffsetRect(
				rcButton.Size().cx - pGlyph2->Size().cx - nGap,
				( rcButton.Size().cy - pGlyph2->Size().cy ) / 2
				);
	}
	PaintGlyph( dc, rectGlyph1.TopLeft(), *pGlyph1, ColorValues );
	if( pGlyph2 != NULL )
		PaintGlyph( dc, rectGlyph2.TopLeft(), *pGlyph2, ColorValues );
}

static int CALLBACK stat_FindFontByNameCallBack(
	ENUMLOGFONT * pEnumLogFont,
	NEWTEXTMETRIC * pNewTextMetric,
	DWORD dwFontType,
	LPARAM lParam // zero terminated string
	)
{
	ASSERT( pEnumLogFont != NULL );
	pNewTextMetric;
	dwFontType;
__EXT_MFC_SAFE_LPCTSTR sFontToFind =
		reinterpret_cast < const TCHAR * > ( lParam );
	ASSERT( sFontToFind != NULL );
	return
		_tcsicoll(
			pEnumLogFont->elfLogFont.lfFaceName,
			sFontToFind
			);
}

static HFONT stat_menu_HFONT_from_current_NONCLIENTMETRICS(
	bool bUseTahomaFont,
	bool bUseCaptionFont
	)
{
NONCLIENTMETRICS _ncNfo;
	memset( &_ncNfo, 0, sizeof(NONCLIENTMETRICS) );
	_ncNfo.cbSize = sizeof(NONCLIENTMETRICS);
	if(	! ::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof(_ncNfo), &_ncNfo, 0 ) )
	{
		return NULL;
	}
	_ncNfo.lfSmCaptionFont.lfCharSet = _ncNfo.lfMenuFont.lfCharSet = (BYTE)GetTextCharset( NULL );
LPCTSTR strDesiredFontFaceName = (LPCTSTR)g_ResourceManager->DesiredFont_GetFaceName();
	if(		LPCTSTR(strDesiredFontFaceName) != NULL
		&&	INT(_tcslen(LPCTSTR(strDesiredFontFaceName))) > 0
		)
	{
		if(	! ::EnumFontFamilies( NULL, NULL, (FONTENUMPROC)stat_FindFontByNameCallBack, reinterpret_cast < LPARAM > ( strDesiredFontFaceName ) ) )
		{
			__EXT_MFC_STRCPY( _ncNfo.lfMenuFont.lfFaceName, LF_FACESIZE, strDesiredFontFaceName );
			bUseTahomaFont = false;
		}
	}
	if( bUseTahomaFont )
	{
		static __EXT_MFC_SAFE_LPCTSTR g_sTahoma = _T("Tahoma");
		if(	! ::EnumFontFamilies( NULL, NULL, (FONTENUMPROC)stat_FindFontByNameCallBack, reinterpret_cast < LPARAM > ( LPCTSTR( g_sTahoma ) ) ) )
			__EXT_MFC_STRCPY( _ncNfo.lfMenuFont.lfFaceName, LF_FACESIZE, g_sTahoma );
	} // else if( bUseTahomaFont )
	if( bUseCaptionFont )
		return CreateFontIndirect( &_ncNfo.lfSmCaptionFont );
	return CreateFontIndirect( &_ncNfo.lfMenuFont );
}

bool CExtPaintManager::NcFrame_IsSupported(
	const CWnd * pWnd,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pWnd;
	lParam;
	return false;
}

HRGN CExtPaintManager::NcFrame_GenerateSkinFrameRGN(
	const RECT & rcWnd,
	const CWnd * pWnd,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWnd );
	rcWnd;
	pWnd;
	lParam;
	return ( (HRGN)NULL );
}

INT CExtPaintManager::NcFrame_GetCaptionHeight(
	bool bActive,
	const CWnd * pWnd,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	bActive;
	pWnd;
	lParam;
	return 0;
}

void CExtPaintManager::NcFrame_GetMetrics(
	RECT & rcNcBorders,
	RECT & rcThemePadding,
	const CWnd * pWnd,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pWnd;
	lParam;
	::SetRectEmpty( &rcNcBorders );
	::SetRectEmpty( &rcThemePadding );
}

CRect CExtPaintManager::NcFrame_GetHtRect(
	UINT nHT,
	bool bScreenMapping,
	bool bLayoutBordersH,
	bool bLayoutBordersV,
	const CExtCmdIcon * pIcon,
	const CWnd * pWnd,
	LPMINMAXINFO pMinMaxInfo, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	nHT;
	bScreenMapping;
	bLayoutBordersH,
	bLayoutBordersV,
	pIcon;
	pWnd;
	pMinMaxInfo,
	lParam;
	return CRect( 0, 0, 0, 0 );
}

void CExtPaintManager::NcFrame_Paint(
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
	CExtPaintManager::e_nc_button_state_t eStateButtonHelp,
	CExtPaintManager::e_nc_button_state_t eStateButtonMinimize,
	CExtPaintManager::e_nc_button_state_t eStateButtonMaximizeRestore,
	CExtPaintManager::e_nc_button_state_t eStateButtonClose,
	const CWnd * pWnd,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	pIcon;
	strCaption;
	nDrawTextAlignFlags;
	rcFrame;
	rcClient;
	rcIcon;
	rcText;
	rcHelp;
	rcMinimize;
	rcMaximizeRestore;
	rcClose;
	bFrameActive;
	bFrameEnabled;
	bFrameMaximized;
	eStateButtonHelp;
	eStateButtonMinimize;
	eStateButtonMaximizeRestore;
	eStateButtonClose;
	pWnd;
	lParam;
}

void CExtPaintManager::NcFrame_GetRects(
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
	LPMINMAXINFO pMinMaxInfo, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pIcon;
	strCaption;
	nDrawTextAlignFlags;
	pWnd;
	pMinMaxInfo;
	lParam;
	::SetRectEmpty( &rcIcon );
	::SetRectEmpty( &rcText );
	::SetRectEmpty( &rcHelp );
	::SetRectEmpty( &rcMinimize );
	::SetRectEmpty( &rcMaximizeRestore );
	::SetRectEmpty( &rcClose );
}

bool CExtPaintManager::Bar_MiniDockFrameNcCalcSize(
	RECT & rcAdjust,
	CExtMiniDockFrameWnd * pMiniFrameWnd,
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pMiniFrameWnd );
	rcAdjust;
	pMiniFrameWnd;
	pBar;
	lParam;
	return false;
}

bool CExtPaintManager::Bar_MiniDockFrameGetNcMetrics(
	INT & nResizingFrameDX,
	INT & nResizingFrameDY,
	INT & nCaptionDY,
	CExtMiniDockFrameWnd * pMiniFrameWnd,
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pMiniFrameWnd );
	pMiniFrameWnd;
	pBar;
	lParam;
	nResizingFrameDX = 3;
	nResizingFrameDY = 3;
	nCaptionDY = 17;
	return true;
}

bool CExtPaintManagerNativeXP::Bar_MiniDockFrameGetNcMetrics(
	INT & nResizingFrameDX,
	INT & nResizingFrameDY,
	INT & nCaptionDY,
	CExtMiniDockFrameWnd * pMiniFrameWnd,
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pMiniFrameWnd );
	pMiniFrameWnd;
	pBar;
	lParam;
	nResizingFrameDX = ::GetSystemMetrics( SM_CXSIZEFRAME );
	nResizingFrameDY = ::GetSystemMetrics( SM_CYSIZEFRAME );
	nCaptionDY = ::GetSystemMetrics( SM_CYSMCAPTION );
	return true;
}

bool CExtPaintManager::Bar_MiniDockFrameCaptionCalcRect(
	RECT & rcCaption,
	CExtMiniDockFrameWnd * pMiniFrameWnd,
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pMiniFrameWnd );

	if( pMiniFrameWnd == NULL )
		return false;

INT nResizingFrameDX = 0;
INT nResizingFrameDY = 0;
INT nCaptionDY = 0;
	Bar_MiniDockFrameGetNcMetrics(
		nResizingFrameDX,
		nResizingFrameDY,
		nCaptionDY,
		pMiniFrameWnd,
		pBar,
		lParam
		);
	
	CRect rcFrameCaption;
	::GetWindowRect( pMiniFrameWnd->GetSafeHwnd(), &rcFrameCaption );
	rcFrameCaption -= rcFrameCaption.TopLeft();
	rcFrameCaption.left  += nResizingFrameDX;
	rcFrameCaption.right -= nResizingFrameDX;
	rcFrameCaption.top   += nResizingFrameDY;
//	rcFrameCaption.bottom =
//		rcFrameCaption.top
//		+ nSmallCaptionDY
//		- ::GetSystemMetrics( SM_CYSMSIZE );
	rcFrameCaption.bottom =
		rcFrameCaption.top
		+ nCaptionDY;
	::CopyRect( &rcCaption, &rcFrameCaption );
	
	return true;
}

void CExtPaintManager::PaintResizableBarSeparator(
	CDC & dc,
	const RECT & rc,
	bool bHorz,
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rc;
	bHorz;
	pBar;
	lParam;
}

bool CExtPaintManager::Bar_LayoutNcAreaButton(
	RECT & rcButton,
	const RECT & rcCaption,
	CExtBarNcAreaButton * pLayoutBtn,
	CExtBarNcAreaButton * pPrevBtn,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pLayoutBtn );
	rcButton;
	rcCaption;
	pLayoutBtn;
	pPrevBtn;
	pHelperSrc;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::Bar_LayoutNcAreaButton(
	RECT & rcButton,
	const RECT & rcCaption,
	CExtBarNcAreaButton * pLayoutBtn,
	CExtBarNcAreaButton * pPrevBtn,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pLayoutBtn );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return
			CExtPaintManager::Bar_LayoutNcAreaButton(
				rcButton,
				rcCaption,
				pLayoutBtn,
				pPrevBtn,
				pHelperSrc,
				lParam
				);

CSize szPart(0,0);
	if( g_PaintManager.m_UxTheme.OpenThemeData( NULL, VSCLASS_WINDOW ) != NULL )
	{
		CWindowDC dc( NULL );
		VERIFY(
			g_PaintManager.m_UxTheme.GetThemePartSize(
				dc.GetSafeHdc(), 
				WP_SMALLCLOSEBUTTON, 
				CBS_NORMAL, 
				NULL, 
				CExtUxTheme::__EXT_UX_TS_TRUE,
				&szPart
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}

const int _nGapToBordersH = 2;
const int _nGapToBordersV = 3;
const int _nGapBetweenButtons = 3;

INT x = rcCaption.right - szPart.cx;
INT y = rcCaption.top;

CRect rc( x, y, x + szPart.cx, y + szPart.cy );
	rc.OffsetRect( 
		-_nGapToBordersH,
		_nGapToBordersV 
		);
	if( pPrevBtn != NULL )
	{
		const CRect & rcPrev = *pPrevBtn;
		rc.OffsetRect(
			rcPrev.left - rc.right - _nGapBetweenButtons,
			0
			);
	}

	rcButton = rc;

	return true;
}

CRect CExtPaintManager::Bar_GetCaptionBordersForText(
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	pBar;
	lParam;
	return CRect( 0, 0, 0, 0 );
}

INT CExtPaintManager::Bar_SeparatorWidthGet(
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	pBar;
	lParam;
	return -1;
}

INT CExtPaintManager::Bar_SeparatorHeightGet(
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	pBar;
	lParam;
	return -1;
}

INT CExtPaintManager::Bar_GripperWidthAtLeftGet(
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	pBar;
	lParam;
	return -1;
}

INT CExtPaintManager::Bar_GripperHeightAtTopGet(
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	pBar;
	lParam;
	return -1;
}

HFONT CExtPaintManager::Bar_GetCaptionFont(
	bool bHorz,
	CExtToolControlBar * pBar,
	LPARAM lParam // = 0
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	bHorz;
	pBar;
	lParam;
	return NULL;
}

CSize CExtPaintManager::Toolbar_GetMinButtonSize(
	CObject * pHelperSrc,
	LPARAM lParam // = 0
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return CSize( 0, 0 );
}

void CExtPaintManager::Toolbar_AdjustButtonSize(
	CExtBarButton * pTBB,
	CSize & sizePreCalc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB;
	sizePreCalc;
}

HFONT CExtPaintManager::Toolbar_GetFont(
	bool bHorz,
	CExtControlBar * pBar,
	LPARAM lParam // = 0
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	bHorz;
	pBar;
	lParam;
	return NULL;
}

bool CExtPaintManager::Toolbar_GetBGInfo(
	RECT & rcBorders,
	INT & nGripWidthAtLeft,
	INT & nGripHeightAtTop,
	CExtToolControlBar * pBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	rcBorders;
	nGripWidthAtLeft;
	nGripHeightAtTop;
	pBar;
	lParam;
	return false;
}

bool CExtPaintManager::Toolbar_GetSizeOfCEB(
	SIZE & sizeReal,
	const SIZE & sizePreCalc,
	CDC & dc,
	BOOL bHorz,
	CExtBarContentExpandButton * pTBB,
	LPARAM lParam // = 0
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	sizeReal;
	sizePreCalc;
	dc;
	bHorz;
	pTBB;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::Toolbar_GetSizeOfCEB(
	SIZE & sizeReal,
	const SIZE & sizePreCalc,
	CDC & dc,
	BOOL bHorz,
	CExtBarContentExpandButton * pTBB,
	LPARAM lParam // = 0
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return
			CExtPaintManager::Toolbar_GetSizeOfCEB(
				sizeReal,
				sizePreCalc,
				dc,
				bHorz,
				pTBB,
				lParam
				);
	
	if( bHorz )
	{
		sizeReal.cx = 15;
		sizeReal.cy = sizePreCalc.cy;
	}
	else
	{
		sizeReal.cx = sizePreCalc.cx;
		sizeReal.cy = 15;
	}
	
	return true;
}

void CExtPaintManager::AdvTip_CalcRgn(
	HRGN hRgnPrecalc,
	const RECT & rcRectRgn,
	bool bDynamicShadowApplied,
	INT nSizeInnerShadowIncluded,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( hRgnPrecalc != NULL );
	ASSERT( nSizeInnerShadowIncluded >= 0 );
	hRgnPrecalc;
	rcRectRgn;
	bDynamicShadowApplied;
	nSizeInnerShadowIncluded;
	pHelperSrc;
	lParam;
}

void CExtPaintManagerNativeXP::AdvTip_CalcRgn(
	HRGN hRgnPrecalc,
	const RECT & rcRectRgn,
	bool bDynamicShadowApplied,
	INT nSizeInnerShadowIncluded,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( hRgnPrecalc != NULL );
	ASSERT( nSizeInnerShadowIncluded >= 0 );
	CExtPaintManager::AdvTip_CalcRgn(
		hRgnPrecalc,
		rcRectRgn,
		bDynamicShadowApplied,
		nSizeInnerShadowIncluded,
		pHelperSrc,
		lParam
		);
}

bool CExtPaintManager::AdvTip_PaintBackground(
	CDC & dc,
	const RECT & rcClient,
	bool bAdvTipWithShadow,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcClient;
	bAdvTipWithShadow;
	pHelperSrc;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::AdvTip_PaintBackground(
	CDC & dc,
	const RECT & rcClient,
	bool bAdvTipWithShadow,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	return
		CExtPaintManager::AdvTip_PaintBackground(
			dc,
			rcClient,
			bAdvTipWithShadow,
			pHelperSrc,
			lParam
			);
}

CFont * CExtPaintManager::AdvTip_GetFont(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return NULL;
}

CFont * CExtPaintManagerNativeXP::AdvTip_GetFont(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	return
		CExtPaintManager::AdvTip_GetFont(
			pHelperSrc,
			lParam
			);
}

COLORREF CExtPaintManager::AdvTip_GetTextColor(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return COLORREF(-1L);
}

COLORREF CExtPaintManagerNativeXP::AdvTip_GetTextColor(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return
		CExtPaintManager::AdvTip_GetTextColor(
			pHelperSrc,
			lParam
			);
}

#ifndef __EXT_MFC_NO_STATUSBAR

bool CExtPaintManager::StatusBar_QuerySkinSupport(
	const CExtStatusControlBar * pStatusBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pStatusBar );
	pStatusBar;
	lParam;
	return false;
}

bool CExtPaintManager::StatusBar_EraseBackground(
	CDC & dc,
	const RECT & rcClient,
	const CExtStatusControlBar * pStatusBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pStatusBar );
	dc;
	rcClient;
	pStatusBar;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::StatusBar_EraseBackground(
	CDC & dc,
	const RECT & rcClient,
	const CExtStatusControlBar * pStatusBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pStatusBar );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return
			CExtPaintManager::StatusBar_EraseBackground(
				dc,
				rcClient,
				pStatusBar,
				lParam
				);
	
	if( g_PaintManager.m_UxTheme.OpenThemeData( pStatusBar->GetSafeHwnd(), VSCLASS_STATUS ) != NULL )
	{
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				pStatusBar->GetSafeHwnd(), 
				dc.GetSafeHdc(), 
				0, 
				0, 
				&rcClient, 
				&rcClient
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}

	return true;
}

bool CExtPaintManager::StatusBar_ErasePaneBackground(
	CDC & dc,
	int nPaneIdx,
	const RECT & rcPane,
	const CExtStatusControlBar * pStatusBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pStatusBar );
	dc;
	nPaneIdx;
	rcPane;
	pStatusBar;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::StatusBar_ErasePaneBackground(
	CDC & dc,
	int nPaneIdx,
	const RECT & rcPane,
	const CExtStatusControlBar * pStatusBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pStatusBar );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return CExtPaintManager::StatusBar_ErasePaneBackground( dc, nPaneIdx, rcPane, pStatusBar, lParam );
	if( g_PaintManager.m_UxTheme.OpenThemeData( pStatusBar->GetSafeHwnd(), VSCLASS_STATUS ) != NULL )
	{
		INT nPaneCount = pStatusBar->GetPaneCount();
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				pStatusBar->GetSafeHwnd(), 
				dc.GetSafeHdc(), 
				( nPaneIdx == nPaneCount - 1 ) 
					? SP_GRIPPERPANE 
					: SP_PANE, 
				0, 
				&rcPane, 
				&rcPane
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
	return true;
}

bool CExtPaintManager::StatusBar_PaintPane(
	CDC & dc,
	int nPaneIdx,
	const RECT & rcPane,
	DWORD dwPaneStyle,
	HICON hIcon,
	__EXT_MFC_SAFE_LPCTSTR sPaneText,
	UINT nDrawTextFlags,
	COLORREF clrText,
	const CExtStatusControlBar * pStatusBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pStatusBar );
	dc; nPaneIdx; rcPane; dwPaneStyle; hIcon; sPaneText; nDrawTextFlags; clrText; pStatusBar; lParam;
	return false;
}

bool CExtPaintManager::StatusBar_PaintSeparator(
	CDC & dc,
	int nPaneIdxAfterSep,
	const RECT & rcPaneBefore,
	const RECT & rcPaneAfter,
	const CExtStatusControlBar * pStatusBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pStatusBar );
	dc;
	nPaneIdxAfterSep;
	rcPaneBefore;
	rcPaneAfter;
	pStatusBar;
	lParam;
	return false;

}

bool CExtPaintManagerNativeXP::StatusBar_PaintSeparator(
	CDC & dc,
	int nPaneIdxAfterSep,
	const RECT & rcPaneBefore,
	const RECT & rcPaneAfter,
	const CExtStatusControlBar * pStatusBar,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pStatusBar );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return
			CExtPaintManager::StatusBar_PaintSeparator(
				dc,
				nPaneIdxAfterSep,
				rcPaneBefore,
				rcPaneAfter,
				pStatusBar,
				lParam
				);

	// do not paint pain separators
	return true;
}

#endif // __EXT_MFC_NO_STATUSBAR

bool CExtPaintManager::AdjustControlFont(
	HFONT & hFont,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( hFont != NULL );
	hFont;
	pHelperSrc;
	lParam;
	return false;
}

DLGTEMPLATE * CExtPaintManager::AdjustDialogTemplate(
	const DLGTEMPLATE * lpDialogTemplate,
	CObject * pHelperSrc,
	bool bWizard, // = false
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( lpDialogTemplate != NULL );
	lpDialogTemplate;
	pHelperSrc;
	bWizard;
	lParam;
	return NULL;
}

HFONT CExtPaintManager::CreateDefaultFont()
{
	ASSERT_VALID( this );
HFONT hDefaultGuiFont = stat_menu_HFONT_from_current_NONCLIENTMETRICS( m_bUseTahomaFont, false );
	if( hDefaultGuiFont == NULL )
		hDefaultGuiFont = (HFONT)::GetStockObject( DEFAULT_GUI_FONT );
	if( hDefaultGuiFont == NULL )
		hDefaultGuiFont = (HFONT)::GetStockObject( SYSTEM_FONT );
	ASSERT( hDefaultGuiFont != NULL );
	return hDefaultGuiFont;
}

HFONT CExtPaintManager::CreateCaptionFont()
{
	ASSERT_VALID( this );
HFONT hCaptionFont = stat_menu_HFONT_from_current_NONCLIENTMETRICS( false, /*m_bUseTahomaFont,*/ true );
	if( hCaptionFont != NULL )
	{
		LOGFONT lf;
		::GetObject( hCaptionFont, sizeof(LOGFONT), &lf );
		lf.lfWeight = 400;
		HFONT hCaptionFont2 = ::CreateFontIndirect( &lf );
		if( hCaptionFont2 != NULL )
		{
			::DeleteObject( hCaptionFont );
			return hCaptionFont2;
		}
	}
	hCaptionFont = (HFONT)::GetStockObject( DEFAULT_GUI_FONT );
	if( hCaptionFont == NULL )
		hCaptionFont = (HFONT)::GetStockObject( SYSTEM_FONT );
	ASSERT( hCaptionFont != NULL );
	return hCaptionFont;
}

void CExtPaintManager::SyncSysColors()
{
	ASSERT_VALID( this );
	g_PaintManager.InitUserExApi();

	// init fonts
	if( m_FontNormal.GetSafeHandle() )
		m_FontNormal.DeleteObject();
HFONT hDefaultGuiFont = CreateDefaultFont();
	ASSERT( hDefaultGuiFont != NULL );
	m_FontNormal.Attach( hDefaultGuiFont );

	if( m_FontCaption.GetSafeHandle() )
		m_FontCaption.DeleteObject();
HFONT hCaptionFont = CreateCaptionFont();
	ASSERT( hCaptionFont != NULL );
	m_FontCaption.Attach( hCaptionFont );

static __EXT_MFC_SAFE_LPCTSTR sVertFontFaceForNt4 = _T("Arial");

LOGFONT lf;
	VERIFY( m_FontNormal.GetLogFont(&lf) );
	lf.lfWeight = 900;
	if( m_FontBold.GetSafeHandle() )
		m_FontBold.DeleteObject();
	VERIFY( m_FontBold.CreateFontIndirect(&lf) );

	VERIFY( m_FontCaption.GetLogFont(&lf) );
	lf.lfWeight = 900;
	if( m_FontCaptionBold.GetSafeHandle() )
		m_FontCaptionBold.DeleteObject();
	VERIFY( m_FontCaptionBold.CreateFontIndirect(&lf) );

	VERIFY( m_FontNormal.GetLogFont(&lf) );
	lf.lfCharSet = SYMBOL_CHARSET;
	lf.lfWeight = 0;
	lf.lfHeight = ::GetSystemMetrics(SM_CYMENUCHECK) - 1;
	__EXT_MFC_STRCPY( lf.lfFaceName, LF_FACESIZE, _T("Marlett") );
	if( m_FontMarlett.GetSafeHandle() )
		m_FontMarlett.DeleteObject();
	VERIFY( m_FontMarlett.CreateFontIndirect(&lf) );

CFont * arrBarCaptFonts[ 2*2 ] =
{
	&m_FontNormalBC,		&m_FontCaption,
	&m_FontBoldBC,			&m_FontCaptionBold,
};
int i = 0;
	for(	i = 0;
			i < sizeof(arrBarCaptFonts)/sizeof(arrBarCaptFonts[0]);
			i += 2
			)
	{
		CFont * pFontSrc = arrBarCaptFonts[i+1];
		ASSERT( pFontSrc->GetSafeHandle() != NULL );
		CFont * pFontDst = arrBarCaptFonts[i];
		if( pFontDst->GetSafeHandle() != NULL )
			pFontDst->DeleteObject();
		VERIFY( pFontSrc->GetLogFont(&lf) );
		VERIFY( pFontDst->CreateFontIndirect(&lf) );
	}

	/// init colors

COLORREF clrDefaultExtendColor = ::GetSysColor( COLOR_3DFACE );
	for(	i = int(m_colors.GetSize());
			i <= __ExtMfc_MAX_SYS_COLOR_VAL;
			i++
			)
	{
		m_colors.Add( clrDefaultExtendColor );
	}

HBRUSH hDefaultExtendBrush = ::GetSysColorBrush( COLOR_3DFACE );
	for(	i = int(m_brushes.GetSize());
			i <= __ExtMfc_MAX_SYS_COLOR_VAL;
			i++
			)
	{
		m_brushes.Add( hDefaultExtendBrush );
	}

	//m_colors.clear();
	//m_brushes.clear();
	for(	i = __ExtMfc_MIN_SYS_COLOR_VAL;
			i <= __ExtMfc_MAX_SYS_COLOR_VAL;
			i++
			)
	{
		m_colors[i] = ::GetSysColor( i );
		m_brushes[i] = ::GetSysColorBrush( i );
	}
	ASSERT( m_colors.GetSize() >= __ExtMfc_MAX_SYS_COLOR_VAL+1 );
	ASSERT( m_brushes.GetSize() >= __ExtMfc_MAX_SYS_COLOR_VAL+1 );

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	Ribbon_SyncFonts();
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
}

// get any system color based brush
HBRUSH CExtPaintManager::GetBrush(
	int nColorIndex
	)
{
	ASSERT_VALID( this );
int nBrushesCount = (int)m_brushes.GetSize();
	if(
			!(
				__ExtMfc_MIN_SYS_COLOR_VAL <= nColorIndex
				&&
				nColorIndex < nBrushesCount
			)
		)
	{
		ASSERT( FALSE );
		nColorIndex = COLOR_3DFACE;
	}
HBRUSH hBrush = m_brushes[nColorIndex];
	return hBrush;
}

COLORREF CExtPaintManager::GetColor(
	int nColorIndex,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;

	if( nColorIndex >= __ExtMfc_COLOR_MAP_BASE )
	{
		int nColorIndex2 = 0;
		if( m_mapColorTranslate.Lookup( nColorIndex, nColorIndex2 ) )
			nColorIndex = nColorIndex2;
		else
		{
			ASSERT( FALSE );
			nColorIndex = COLOR_3DFACE;
		}
	}
int nColorCount = (int)m_colors.GetSize();
	if(
			!(
				__ExtMfc_MIN_SYS_COLOR_VAL <= nColorIndex
				&&
				nColorIndex < nColorCount // <= __ExtMfc_MAX_SYS_COLOR_VAL
			)
		)
	{
		ASSERT( FALSE );
		nColorIndex = COLOR_3DFACE;
	}
COLORREF clr = m_colors[nColorIndex];
	return clr;
}

COLORREF CExtPaintManager::GetColor(
	int nColorIndex,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	return ( const_cast < CExtPaintManager * > ( this ) ) -> GetColor( nColorIndex, pHelperSrc, lParam );
}

int CExtPaintManager::InstallColor(
	COLORREF clr,
	int nColorIndex // = -1
	)
{
	ASSERT_VALID( this );
int nColorCount = (int)m_colors.GetSize();
	if( nColorIndex < 0 || nColorIndex >= nColorCount )
		nColorIndex = (int)m_colors.Add( clr );
	else
		m_colors[nColorIndex] = clr;
	return nColorIndex;
}

void CExtPaintManager::InitTranslatedColors()
{
	ASSERT_VALID( this );

	m_mapColorTranslate.RemoveAll();

	m_mapColorTranslate[CLR_3DFACE_OUT]				= COLOR_3DFACE;
	m_mapColorTranslate[CLR_3DFACE_IN]				= COLOR_3DFACE;
	m_mapColorTranslate[CLR_3DFACE_DISABLED]		= COLOR_3DFACE;

	m_mapColorTranslate[CLR_3DLIGHT_OUT]			= COLOR_3DLIGHT;
	m_mapColorTranslate[CLR_3DLIGHT_IN]				= COLOR_3DLIGHT;
	m_mapColorTranslate[CLR_3DLIGHT_DISABLED]		= COLOR_3DLIGHT;

	m_mapColorTranslate[CLR_3DHILIGHT_OUT]			= COLOR_3DHILIGHT;
	m_mapColorTranslate[CLR_3DHILIGHT_IN]			= COLOR_3DHILIGHT;
	m_mapColorTranslate[CLR_3DHILIGHT_DISABLED]		= COLOR_3DHILIGHT;

	m_mapColorTranslate[CLR_3DSHADOW_OUT]			= COLOR_3DSHADOW;
	m_mapColorTranslate[CLR_3DSHADOW_IN]			= COLOR_3DSHADOW;
	m_mapColorTranslate[CLR_3DSHADOW_DISABLED]		= COLOR_3DSHADOW;

	m_mapColorTranslate[CLR_3DDKSHADOW_OUT]			= COLOR_3DDKSHADOW;
	m_mapColorTranslate[CLR_3DDKSHADOW_IN]			= COLOR_3DDKSHADOW;
	m_mapColorTranslate[CLR_3DDKSHADOW_DISABLED]	= COLOR_3DDKSHADOW;
	
	m_mapColorTranslate[CLR_TEXT_OUT]				= COLOR_WINDOWTEXT;
	m_mapColorTranslate[CLR_TEXT_IN]				= COLOR_WINDOWTEXT;
	m_mapColorTranslate[CLR_TEXT_DISABLED]			= COLOR_GRAYTEXT;

	// Menu text colors
 	m_mapColorTranslate[CLR_MENUTEXT_OUT]			= COLOR_WINDOWTEXT;
	m_mapColorTranslate[CLR_MENUTEXT_IN]			= COLOR_HIGHLIGHTTEXT;
	m_mapColorTranslate[CLR_MENUTEXT_DISABLED]		= COLOR_GRAYTEXT;

	// Task Pane Colors
	m_mapColorTranslate[CLR_TASK_PANE_BK_TOP]		= COLOR_WINDOW;
	m_mapColorTranslate[CLR_TASK_PANE_BK_BOTTOM]	= COLOR_WINDOW;

	m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_TEXT_NORMAL] = 
		COLOR_WINDOWTEXT;
	m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_TEXT_NORMAL_HOVERED] = 
		COLOR_WINDOWTEXT;
	m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_TEXT_HIGHLIGHTED] = 
		COLOR_HIGHLIGHTTEXT;
	m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_TEXT_HIGHLIGHTED_HOVERED]	= 
		COLOR_HIGHLIGHTTEXT;

	m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_BK_LEFT_NORMAL]	= 
		COLOR_3DFACE;
	m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_BK_LEFT_HIGHLIGHTED]	= 
		COLOR_HIGHLIGHT;
	m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_BK_RIGHT_NORMAL]	= 
		COLOR_3DFACE;
	m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_BK_RIGHT_HIGHLIGHTED]	= 
		COLOR_HIGHLIGHT;

	// Page Navigator colors
	m_mapColorTranslate[CLR_PN_CAPTION_TEXT] = COLOR_WINDOW;

	// Group Box colors
	m_mapColorTranslate[CLR_GROUP_BOX_FRAME] = COLOR_3DSHADOW;

	// Resizable Bar non client area color
	m_mapColorTranslate[CLR_WRB_FRAME] = COLOR_3DSHADOW;
}

void CExtPaintManager::InitHelperBrushes()
{
	ASSERT_VALID( this );
	if( m_brushLightDefault.GetSafeHandle() != NULL )
		m_brushLightDefault.DeleteObject();
	if( m_brushLightSystem.GetSafeHandle() != NULL )
		m_brushLightSystem.DeleteObject();
	if( m_brushDarkDefault.GetSafeHandle() != NULL )
		m_brushDarkDefault.DeleteObject();
	if( m_brushDarkSystem.GetSafeHandle() != NULL )
		m_brushDarkSystem.DeleteObject();
	if( m_brushLighterDefault.GetSafeHandle() != NULL )
		m_brushLighterDefault.DeleteObject();
	if( m_brushLighterSystem.GetSafeHandle() != NULL )
		m_brushLighterSystem.DeleteObject();
	if( m_brushDarkerDefault.GetSafeHandle() != NULL )
		m_brushDarkerDefault.DeleteObject();
	if( m_brushDarkerSystem.GetSafeHandle() != NULL )
		m_brushDarkerSystem.DeleteObject();
	if( m_brushLightestDefault.GetSafeHandle() != NULL )
		m_brushLightestDefault.DeleteObject();
	if( m_brushLightestSystem.GetSafeHandle() != NULL )
		m_brushLightestSystem.DeleteObject();
	if( m_brushDarkestDefault.GetSafeHandle() != NULL )
		m_brushDarkestDefault.DeleteObject();
	if( m_brushDarkestSystem.GetSafeHandle() != NULL )
		m_brushDarkestSystem.DeleteObject();

int nBitsPerPixel = stat_GetBPP();
	if( nBitsPerPixel > 8 )
	{
		COLORREF clrDarker = GetColor( COLOR_3DFACE, this );
		COLORREF clrLighter = GetColor( COLOR_3DHILIGHT, this );
		COLORREF clrMixed = RGB (
			GetRValue(clrDarker) + ((GetRValue(clrLighter) -
				GetRValue(clrDarker)) / 2),
			GetGValue(clrDarker) + ((GetGValue(clrLighter) -
				GetGValue(clrDarker)) / 2),
			GetBValue(clrDarker) + ((GetBValue(clrLighter) -
				GetBValue(clrDarker)) / 2)
			);
		m_brushLightDefault.CreateSolidBrush( clrMixed );
		clrMixed = RGB (
			GetRValue(clrDarker) +
				::MulDiv((GetRValue(clrLighter) -
					GetRValue(clrDarker)),3,5),
			GetGValue(clrDarker) +
				::MulDiv((GetGValue(clrLighter) -
					GetGValue(clrDarker)),3,5),
			GetBValue(clrDarker) +
				::MulDiv((GetBValue(clrLighter) -
					GetBValue(clrDarker)),3,5)
			);
		m_brushLighterDefault.CreateSolidBrush( clrMixed );
		clrMixed = RGB (
			GetRValue(clrDarker) +
				::MulDiv((GetRValue(clrLighter) -
					GetRValue(clrDarker)),4,5),
			GetGValue(clrDarker) +
				::MulDiv((GetGValue(clrLighter) -
					GetGValue(clrDarker)),4,5),
			GetBValue(clrDarker) +
				::MulDiv((GetBValue(clrLighter) -
					GetBValue(clrDarker)),4,5)
			);
		m_brushLightestDefault.CreateSolidBrush( clrMixed );
		
		clrLighter = clrDarker;
		clrDarker = GetColor( COLOR_3DSHADOW, this );
		clrMixed = RGB (
			GetRValue(clrDarker) + ((GetRValue(clrLighter) -
				GetRValue(clrDarker)) / 2 ),
			GetGValue(clrDarker) + ((GetGValue(clrLighter) -
				GetGValue(clrDarker)) / 2),
			GetBValue(clrDarker) + ((GetBValue(clrLighter) -
				GetBValue(clrDarker)) / 2)
			);
		m_brushDarkDefault.CreateSolidBrush( clrMixed );
		clrMixed = RGB (
			GetRValue(clrDarker) +
				::MulDiv((GetRValue(clrLighter) -
					GetRValue(clrDarker)),2,5),
			GetGValue(clrDarker) +
				::MulDiv((GetGValue(clrLighter) -
					GetGValue(clrDarker)),2,5),
			GetBValue(clrDarker) +
				::MulDiv((GetBValue(clrLighter) -
					GetBValue(clrDarker)),2,5)
			);
		m_brushDarkerDefault.CreateSolidBrush( clrMixed );
		clrMixed = RGB (
			GetRValue(clrDarker) +
				::MulDiv((GetRValue(clrLighter) -
					GetRValue(clrDarker)),1,5),
			GetGValue(clrDarker) +
				::MulDiv((GetGValue(clrLighter) -
					GetGValue(clrDarker)),1,5),
			GetBValue(clrDarker) +
				::MulDiv((GetBValue(clrLighter) -
					GetBValue(clrDarker)),1,5)
			);
		m_brushDarkestDefault.CreateSolidBrush( clrMixed );

		clrDarker = ::GetSysColor( COLOR_3DFACE );
		clrLighter = ::GetSysColor( COLOR_3DHILIGHT );
		clrMixed = RGB (
			GetRValue(clrDarker) + ((GetRValue(clrLighter) -
				GetRValue(clrDarker)) / 2),
			GetGValue(clrDarker) + ((GetGValue(clrLighter) -
				GetGValue(clrDarker)) / 2),
			GetBValue(clrDarker) + ((GetBValue(clrLighter) -
				GetBValue(clrDarker)) / 2)
			);
		m_brushLightSystem.CreateSolidBrush( clrMixed );
		clrMixed = RGB (
			GetRValue(clrDarker) +
				::MulDiv((GetRValue(clrLighter) -
					GetRValue(clrDarker)),3,5),
			GetGValue(clrDarker) +
				::MulDiv((GetGValue(clrLighter) -
					GetGValue(clrDarker)),3,5),
			GetBValue(clrDarker) +
				::MulDiv((GetBValue(clrLighter) -
					GetBValue(clrDarker)),3,5)
			);
		m_brushLighterSystem.CreateSolidBrush( clrMixed );
		clrMixed = RGB (
			GetRValue(clrDarker) +
				::MulDiv((GetRValue(clrLighter) -
					GetRValue(clrDarker)),4,5),
			GetGValue(clrDarker) +
				::MulDiv((GetGValue(clrLighter) -
					GetGValue(clrDarker)),4,5),
			GetBValue(clrDarker) +
				::MulDiv((GetBValue(clrLighter) -
					GetBValue(clrDarker)),4,5)
			);
		m_brushLightestSystem.CreateSolidBrush( clrMixed );
		
		clrLighter = clrDarker;
		clrDarker = ::GetSysColor( COLOR_3DSHADOW );
		clrMixed = RGB (
			GetRValue(clrDarker) + ((GetRValue(clrLighter) -
				GetRValue(clrDarker)) / 2),
			GetGValue(clrDarker) + ((GetGValue(clrLighter) -
				GetGValue(clrDarker)) / 2),
			GetBValue(clrDarker) + ((GetBValue(clrLighter) -
				GetBValue(clrDarker)) / 2)
			);
		m_brushDarkSystem.CreateSolidBrush( clrMixed );
		clrMixed = RGB (
			GetRValue(clrDarker) +
				::MulDiv((GetRValue(clrLighter) -
					GetRValue(clrDarker)),2,5),
			GetGValue(clrDarker) +
				::MulDiv((GetGValue(clrLighter) -
					GetGValue(clrDarker)),2,5),
			GetBValue(clrDarker) +
				::MulDiv((GetBValue(clrLighter) -
					GetBValue(clrDarker)),2,5)
			);
		m_brushDarkerSystem.CreateSolidBrush( clrMixed );
		clrMixed = RGB (
			GetRValue(clrDarker) +
				::MulDiv((GetRValue(clrLighter) -
					GetRValue(clrDarker)),1,5),
			GetGValue(clrDarker) +
				::MulDiv((GetGValue(clrLighter) -
					GetGValue(clrDarker)),1,5),
			GetBValue(clrDarker) +
				::MulDiv((GetBValue(clrLighter) -
					GetBValue(clrDarker)),1,5)
			);
		m_brushDarkestSystem.CreateSolidBrush( clrMixed );
	} // if( nBitsPerPixel > 8 )
	else
	{
		CBitmap bmp;
		COLORREF clrDarker = GetColor( COLOR_3DFACE, this );
		COLORREF clrLighter = GetColor( COLOR_3DHILIGHT, this );
		HBITMAP hBmp = stat_GenLBDIB( clrDarker, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushLightDefault.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
		hBmp = stat_GenLBDIB( clrDarker, clrLighter, 1, 1, 1, 1, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushLighterDefault.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
		hBmp = stat_GenLBDIB( clrDarker, clrLighter, 1, 1, 2, 2, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushLightestDefault.CreatePatternBrush( &bmp );
		bmp.DeleteObject();

		clrLighter = clrDarker;
		clrDarker = GetColor( COLOR_3DSHADOW, this );
		hBmp = stat_GenLBDIB( clrDarker, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushDarkDefault.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
		hBmp = stat_GenLBDIB( clrDarker, clrLighter, 1, 1, 1, 1 );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushDarkerDefault.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
		clrLighter = clrDarker;
		clrDarker = GetColor( COLOR_3DDKSHADOW, this );
		hBmp = stat_GenLBDIB( clrDarker, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushDarkestDefault.CreatePatternBrush( &bmp );
		bmp.DeleteObject();

		clrDarker = ::GetSysColor( COLOR_3DFACE );
		clrLighter = ::GetSysColor( COLOR_3DHILIGHT );
		hBmp = stat_GenLBDIB( clrDarker, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushLightSystem.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
		hBmp = stat_GenLBDIB( clrDarker, clrLighter, 1, 1, 1, 1, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushLighterSystem.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
		hBmp = stat_GenLBDIB( clrDarker, clrLighter, 1, 1, 2, 2, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushLightestSystem.CreatePatternBrush( &bmp );
		bmp.DeleteObject();

		clrLighter = clrDarker;
		clrDarker = ::GetSysColor( COLOR_3DSHADOW );
		hBmp = stat_GenLBDIB( clrDarker, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushDarkSystem.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
		hBmp = stat_GenLBDIB( clrDarker, clrLighter, 1, 1, 1, 1 );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushDarkerSystem.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
		clrLighter = clrDarker;
		clrDarker = ::GetSysColor( COLOR_3DDKSHADOW );
		hBmp = stat_GenLBDIB( clrDarker, clrLighter );
		ASSERT( hBmp != NULL );
		bmp.Attach( hBmp );
		m_brushDarkestSystem.CreatePatternBrush( &bmp );
		bmp.DeleteObject();
	} // else from if( nBitsPerPixel > 8 )
}

static const UINT stat_pixels_dock_btn_empty[] =
{
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_btn_close[] =
{
	0,0,0,0,0,0,0,0,0,
	0,1,1,0,0,0,0,1,1,
	0,0,1,1,0,0,1,1,0,
	0,0,0,1,1,1,1,0,0,
	0,0,0,0,1,1,0,0,0,
	0,0,0,1,1,1,1,0,0,
	0,0,1,1,0,0,1,1,0,
	0,1,1,0,0,0,0,1,1,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_btn_close_dc2k[] =
{
	0,0,0,0,0,0,0,0,0,
	0,1,1,0,0,0,1,1,0,
	0,0,1,1,0,1,1,0,0,
	0,0,0,1,1,1,0,0,0,
	0,0,0,1,1,1,0,0,0,
	0,0,1,1,0,1,1,0,0,
	0,1,1,0,0,0,1,1,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_btn_thin_close[] =
{
	0,0,0,0,0,0,0,0,0,
	0,1,0,0,0,0,0,1,0,
	0,0,1,0,0,0,1,0,0,
	0,0,0,1,0,1,0,0,0,
	0,0,0,0,1,0,0,0,0,
	0,0,0,1,0,1,0,0,0,
	0,0,1,0,0,0,1,0,0,
	0,1,0,0,0,0,0,1,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_btn_arrow_up[] =
{
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,0,0,0,0,
	0,0,0,1,1,1,0,0,0,
	0,0,1,1,1,1,1,0,0,
	0,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_btn_hollow_up[] =
{
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,0,0,0,0,
	0,0,0,1,0,1,0,0,0,
	0,0,1,0,0,0,1,0,0,
	0,1,0,0,0,0,0,1,0,
	1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_btn_arrow_up_l[] =
{
	0,0,0,0,1,0,0,0,0,
	0,0,0,1,1,1,0,0,0,
	0,0,1,1,1,1,1,0,0,
	0,1,1,1,1,1,1,1,0,
	1,1,1,1,1,1,1,1,1,
	0,0,1,1,1,1,1,0,0,
	0,0,1,1,1,1,1,0,0,
	0,0,1,1,1,1,1,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_wnd_minimize[] =
{
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,1,1,1,1,1,1,1,0,
	0,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_wnd_restore[] =
{
	0,0,1,1,1,1,1,1,0,
	0,0,1,0,0,0,0,1,0,
	1,1,1,1,1,1,0,1,0,
	1,1,1,1,1,1,0,1,0,
	1,0,0,0,0,1,1,1,0,
	1,0,0,0,0,1,0,0,0,
	1,0,0,0,0,1,0,0,0,
	1,1,1,1,1,1,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_wnd_maximize[] =
{
	1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,
};

static const UINT stat_pixels_dock_wnd_keep[] =
{
	0,0,1,1,1,1,1,0,0,
	0,0,1,0,0,1,1,0,0,
	0,0,1,0,0,1,1,0,0,
	0,0,1,0,0,1,1,0,0,
	0,0,1,0,0,1,1,0,0,
	0,1,1,1,1,1,1,1,0,
	0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_dock_btn_contexthelp[] =
{
	0,0,0,1,1,1,1,0,0,
	0,0,1,1,0,0,1,1,0,
	0,0,1,1,0,0,1,1,0,
	0,0,0,0,0,1,1,0,0,
	0,0,0,0,1,1,0,0,0,
	0,0,0,0,1,1,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,1,0,0,0,
	0,0,0,0,1,1,0,0,0,
};

static const UINT stat_pixels_hollow_plus[] =
{
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,
	0,0,1,1,1,1,1,0,0,
	0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_hollow_minus[] =
{
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,1,1,1,1,1,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_tree_plus[] =
{
	1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,1,0,0,0,1,
	1,0,0,0,1,0,0,0,1,
	1,0,1,1,1,1,1,0,1,
	1,0,0,0,1,0,0,0,1,
	1,0,0,0,1,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,
};

static const UINT stat_pixels_tree_minus[] =
{
	1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,1,1,1,1,1,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,
};

static const UINT stat_pixels_tree_plus_filled[] =
{
	1,1,1,1,1,1,1,1,1,
	1,2,2,2,2,2,2,2,1,
	1,2,2,2,1,2,2,2,1,
	1,2,2,2,1,2,2,2,1,
	1,2,1,1,1,1,1,2,1,
	1,2,2,2,1,2,2,2,1,
	1,2,2,2,1,2,2,2,1,
	1,2,2,2,2,2,2,2,1,
	1,1,1,1,1,1,1,1,1,
};

static const UINT stat_pixels_tree_minus_filled[] =
{
	1,1,1,1,1,1,1,1,1,
	1,2,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,2,1,
	1,2,1,1,1,1,1,2,1,
	1,2,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,2,1,
	1,1,1,1,1,1,1,1,1,
};

static const UINT stat_pixels_circle_plus[] =
{
	0,0,1,1,1,1,1,0,0,
	0,1,0,0,0,0,0,1,0,
	1,0,0,0,1,0,0,0,1,
	1,0,0,0,1,0,0,0,1,
	1,0,1,1,1,1,1,0,1,
	1,0,0,0,1,0,0,0,1,
	1,0,0,0,1,0,0,0,1,
	0,1,0,0,0,0,0,1,0,
	0,0,1,1,1,1,1,0,0,
};

static const UINT stat_pixels_circle_minus[] =
{
	0,0,1,1,1,1,1,0,0,
	0,1,0,0,0,0,0,1,0,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,1,1,1,1,1,0,1,
	1,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,1,
	0,1,0,0,0,0,0,1,0,
	0,0,1,1,1,1,1,0,0,
};

static const UINT stat_pixels_arrow_pt_up[] =
{
	0,0,0,0,0,1,0,0,0,0,0,
	0,0,0,0,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,0,0,0,
	0,0,1,1,1,1,1,1,1,0,0,
	0,1,1,1,1,1,1,1,1,1,0,
	1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,0,0,0,
	0,0,0,1,1,1,1,1,0,0,0,
	0,0,0,1,1,1,1,1,0,0,0,
	0,0,0,1,1,1,1,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_arrow_pt_lt[] =
{
	1,1,1,1,1,1,1,1,0,0,0,
	1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,1,0,0,0,
	1,1,0,0,1,1,1,1,1,0,0,
	1,0,0,0,0,1,1,1,0,0,0,
	0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
};

static const UINT stat_pixels_arrow_small_up[] =
{
	0,0,1,0,0,
	0,1,1,1,0,
	1,1,1,1,1,
};

static const UINT stat_pixels_ellipsis[] =
{
	0,0,0,0,0,0,0,0,0,
	0,1,0,0,1,0,0,1,0,
	0,0,0,0,0,0,0,0,0,
};

static CExtPaintManager::glyph_t stat_glyph_dock_empty(
	9,9,2,
	stat_pixels_dock_btn_empty
	);
static CExtPaintManager::glyph_t stat_glyph_dock_close(
	9,9,2,
	stat_pixels_dock_btn_close
	);
static CExtPaintManager::glyph_t stat_glyph_dock_close_dc2k(
	9,9,2,
	stat_pixels_dock_btn_close_dc2k
	);
static CExtPaintManager::glyph_t stat_glyph_dock_xp_thin_close(
	9,9,2,
	stat_pixels_dock_btn_thin_close
	);

static CExtPaintManager::glyph_t stat_glyph_dock_arrow_up(
	9,9,2,
	stat_pixels_dock_btn_arrow_up
	);
static CExtPaintManager::glyph_t stat_glyph_dock_arrow_down(
	stat_glyph_dock_arrow_up,
	180
	);
static CExtPaintManager::glyph_t stat_glyph_dock_arrow_left(
	stat_glyph_dock_arrow_up,
	270
	);
static CExtPaintManager::glyph_t stat_glyph_dock_arrow_right(
	stat_glyph_dock_arrow_left,
	180
	);
static CExtPaintManager::glyph_t stat_glyph_dock_wnd_minimize(
	9,9,2,
	stat_pixels_dock_wnd_minimize
	);
static CExtPaintManager::glyph_t stat_glyph_dock_wnd_restore(
	9,9,2,
	stat_pixels_dock_wnd_restore
	);
static CExtPaintManager::glyph_t stat_glyph_dock_wnd_maximize(
	9,9,2,
	stat_pixels_dock_wnd_maximize
	);
static CExtPaintManager::glyph_t stat_glyph_dock_wnd_keep(
	9,9,2,
	stat_pixels_dock_wnd_keep
	);
static CExtPaintManager::glyph_t stat_glyph_dock_wnd_unkeep(
	stat_glyph_dock_wnd_keep,
	90
	);

static CExtPaintManager::glyph_t stat_glyph_dock_wnd_contexthelp(
	9,9,2,
	stat_pixels_dock_btn_contexthelp
	);

static CExtPaintManager::glyph_t stat_glyph_dock_arrow_up_l(
	9,9,2,
	stat_pixels_dock_btn_arrow_up_l
	);
static CExtPaintManager::glyph_t stat_glyph_dock_arrow_down_l(
	stat_glyph_dock_arrow_up_l,
	180
	);
static CExtPaintManager::glyph_t stat_glyph_dock_arrow_left_l(
	stat_glyph_dock_arrow_up_l,
	270
	);
static CExtPaintManager::glyph_t stat_glyph_dock_arrow_right_l(
	stat_glyph_dock_arrow_left_l,
	180
	);

static CExtPaintManager::glyph_t stat_glyph_dock_hollow_up(
	9,9,2,
	stat_pixels_dock_btn_hollow_up
	);
static CExtPaintManager::glyph_t stat_glyph_dock_hollow_down(
	stat_glyph_dock_hollow_up,
	180
	);
static CExtPaintManager::glyph_t stat_glyph_dock_hollow_left(
	stat_glyph_dock_hollow_up,
	270
	);
static CExtPaintManager::glyph_t stat_glyph_dock_hollow_right(
	stat_glyph_dock_hollow_left,
	180
	);

static CExtPaintManager::glyph_t stat_glyph_hollow_plus(
	9,9,2,
	stat_pixels_hollow_plus
	);
static CExtPaintManager::glyph_t stat_glyph_hollow_minus(
	9,9,2,
	stat_pixels_hollow_minus
	);
static CExtPaintManager::glyph_t stat_glyph_tree_plus(
	9,9,2,
	stat_pixels_tree_plus
	);
static CExtPaintManager::glyph_t stat_glyph_tree_minus(
	9,9,2,
	stat_pixels_tree_minus
	);
static CExtPaintManager::glyph_t stat_glyph_tree_plus_filled(
	9,9,3,
	stat_pixels_tree_plus_filled
	);
static CExtPaintManager::glyph_t stat_glyph_tree_minus_filled(
	9,9,3,
	stat_pixels_tree_minus_filled
	);
static CExtPaintManager::glyph_t stat_glyph_circle_plus(
	9,9,2,
	stat_pixels_circle_plus
	);
static CExtPaintManager::glyph_t stat_glyph_circle_minus(
	9,9,2,
	stat_pixels_circle_minus
	);

static CExtPaintManager::glyph_t stat_glyph_arrow_pt_up(
	11,11,2,
	stat_pixels_arrow_pt_up
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_pt_down(
	stat_glyph_arrow_pt_up,
	180
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_pt_left(
	stat_glyph_arrow_pt_up,
	270
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_pt_right(
	stat_glyph_arrow_pt_left,
	180
	);

static CExtPaintManager::glyph_t stat_glyph_arrow_small_up(
	5,3,2,
	stat_pixels_arrow_small_up
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_small_down(
	stat_glyph_arrow_small_up,
	180
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_small_left(
	stat_glyph_arrow_small_up,
	270
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_small_right(
	stat_glyph_arrow_small_left,
	180
	);
static CExtPaintManager::glyph_t stat_glyph_ellipsis(
	9,3,2,
	stat_pixels_ellipsis
	);

static CExtPaintManager::glyph_t stat_glyph_arrow_pt_lt(
	11,11,2,
	stat_pixels_arrow_pt_lt
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_pt_rt(
	stat_glyph_arrow_pt_lt,
	90
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_pt_rb(
	stat_glyph_arrow_pt_rt,
	90
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_pt_lb(
	stat_glyph_arrow_pt_rb,
	90
	);

static const UINT stat_pixels_arrow_sort_up[] =
{
	0,0,0,0,1,1,0,0,0,0,
	0,0,0,0,1,1,0,0,0,0,
	0,0,0,1,3,1,1,0,0,0,
	0,0,0,3,0,0,1,0,0,0,
	0,0,1,1,0,0,1,1,0,0,
	0,0,3,0,0,0,0,1,0,0,
	0,1,1,0,0,0,0,1,1,0,
	0,3,0,0,0,0,0,0,1,0,
	1,1,0,0,0,0,0,0,1,1,
	1,2,2,2,2,2,2,2,2,2,
};

static const UINT stat_pixels_arrow_sort_down[] =
{
	3,3,3,3,3,3,3,3,3,2,
	1,1,0,0,0,0,0,0,2,2,
	0,3,0,0,0,0,0,0,2,0,
	0,1,1,0,0,0,0,2,2,0,
	0,0,3,0,0,0,0,2,0,0,
	0,0,1,1,0,0,2,2,0,0,
	0,0,0,3,0,0,2,0,0,0,
	0,0,0,1,3,2,2,0,0,0,
	0,0,0,0,1,2,0,0,0,0,
	0,0,0,0,1,2,0,0,0,0,
};

static CExtPaintManager::glyph_t stat_glyph_arrow_sort_up(
	10,10,4, // 8,7,4,
	stat_pixels_arrow_sort_up
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_sort_down(
	10,10,4, // 8,7,4,
	stat_pixels_arrow_sort_down
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_sort_left(
	stat_glyph_arrow_sort_down,
	90
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_sort_right(
	stat_glyph_arrow_sort_up,
	90
	);

static const UINT stat_pixels_arrow_sort_down_2007[] =
{
	1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,0,
	0,0,1,1,1,1,1,0,0,
	0,0,0,1,1,1,0,0,0,
	0,0,0,0,1,0,0,0,0,
};

static CExtPaintManager::glyph_t stat_glyph_arrow_sort_down_2007(
	9,5,2,
	stat_pixels_arrow_sort_down_2007
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_sort_up_2007(
	stat_glyph_arrow_sort_down_2007,
	180
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_sort_left_2007(
	stat_glyph_arrow_sort_down_2007,
	90
	);
static CExtPaintManager::glyph_t stat_glyph_arrow_sort_right_2007(
	stat_glyph_arrow_sort_up_2007,
	90
	);

static const UINT stat_pixels_tabs_list_arrow[] =
{
	1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,
	0,1,1,1,1,1,0,
	0,0,1,1,1,0,0,
	0,0,0,1,0,0,0,
};

static CExtPaintManager::glyph_t stat_glyph_tabs_list_arrow(
	7,7,2,
	stat_pixels_tabs_list_arrow
	);

static const UINT stat_pixels_pn_config_button[] =
{
	1,1,0,1,1,0,0,
	0,1,1,0,1,1,0,
	0,0,1,1,0,1,1,
	0,1,1,0,1,1,0,
	1,1,0,1,1,0,0,
	0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,
	0,1,1,1,1,1,0,
	0,0,1,1,1,0,0,
	0,0,0,1,0,0,0,
};

static CExtPaintManager::glyph_t stat_glyph_pn_config_button(
	7,12,2,
	stat_pixels_pn_config_button
	);

static const UINT stat_pixels_pn_config_button_2007[] =
{
	1,1,1,1,1,
	0,1,1,1,0,
	0,0,1,0,0,
};

static CExtPaintManager::glyph_t stat_glyph_pn_config_button_2007(
	5,3,2,
	stat_pixels_pn_config_button_2007
	);

static const UINT stat_pixels_pn_caption_expanded[] =
{
	0,0,0,1,0,0,0,
	0,0,1,1,1,0,0,
	0,1,1,0,1,1,0,
	1,1,0,0,0,1,1,
	0,0,0,1,0,0,0,
	0,0,1,1,1,0,0,
	0,1,1,0,1,1,0,
	1,1,0,0,0,1,1,
};

static CExtPaintManager::glyph_t stat_glyph_pn_caption_expanded(
	7,8,2,
	stat_pixels_pn_caption_expanded
	);

static const UINT stat_pixels_pn_caption_collapsed[] =
{
	1,1,0,0,0,1,1,
	0,1,1,0,1,1,0,
	0,0,1,1,1,0,0,
	0,0,0,1,0,0,0,
	1,1,0,0,0,1,1,
	0,1,1,0,1,1,0,
	0,0,1,1,1,0,0,
	0,0,0,1,0,0,0,
};

static CExtPaintManager::glyph_t stat_glyph_pn_caption_collapsed(
	7,8,2,
	stat_pixels_pn_caption_collapsed
	);

static const UINT stat_pixels_scroll_top[] =
{
	1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,
	0,0,0,1,0,0,0,
	0,0,1,1,1,0,0,
	0,1,1,1,1,1,0,
	1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,
};

static CExtPaintManager::glyph_t stat_glyph_scroll_top(
	7,7,2,
	stat_pixels_scroll_top
	);

static const UINT stat_pixels_scroll_left[] =
{
	1,0,0,0,0,1,0,
	1,0,0,0,1,1,0,
	1,0,0,1,1,1,0,
	1,0,1,1,1,1,0,
	1,0,0,1,1,1,0,
	1,0,0,0,1,1,0,
	1,0,0,0,0,1,0,
};

static CExtPaintManager::glyph_t stat_glyph_scroll_left(
	7,7,2,
	stat_pixels_scroll_left
	);

static const UINT stat_pixels_scroll_bottom[] =
{
	0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,
	0,1,1,1,1,1,0,
	0,0,1,1,1,0,0,
	0,0,0,1,0,0,0,
	0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,
};

static CExtPaintManager::glyph_t stat_glyph_scroll_bottom(
	7,7,2,
	stat_pixels_scroll_bottom
	);

static const UINT stat_pixels_scroll_right[] =
{
	0,1,0,0,0,0,1,
	0,1,1,0,0,0,1,
	0,1,1,1,0,0,1,
	0,1,1,1,1,0,1,
	0,1,1,1,0,0,1,
	0,1,1,0,0,0,1,
	0,1,0,0,0,0,1,
};

static CExtPaintManager::glyph_t stat_glyph_scroll_right(
	7,7,2,
	stat_pixels_scroll_right
	);

CExtPaintManager::glyph_t *
	CExtPaintManager::g_DockingCaptionGlyphs[__DCBT_VALUE_MAX+1] =
{
	& stat_glyph_dock_empty,
	& stat_glyph_dock_close,
	& stat_glyph_dock_arrow_up,
	& stat_glyph_dock_arrow_down,
	& stat_glyph_dock_arrow_left,
	& stat_glyph_dock_arrow_right,
	& stat_glyph_dock_wnd_minimize,
	& stat_glyph_dock_wnd_restore,
	& stat_glyph_dock_wnd_maximize,
	& stat_glyph_dock_wnd_contexthelp,
	& stat_glyph_dock_wnd_keep,
	& stat_glyph_dock_wnd_unkeep,
	& stat_glyph_dock_arrow_up_l,
	& stat_glyph_dock_arrow_down_l,
	& stat_glyph_dock_arrow_left_l,
	& stat_glyph_dock_arrow_right_l,
	& stat_glyph_dock_xp_thin_close,
	& stat_glyph_dock_hollow_up,
	& stat_glyph_dock_hollow_down,
	& stat_glyph_dock_hollow_left,
	& stat_glyph_dock_hollow_right,
	& stat_glyph_dock_close_dc2k,
	& stat_glyph_hollow_plus,
	& stat_glyph_hollow_minus,
	& stat_glyph_tree_plus,
	& stat_glyph_tree_minus,
	& stat_glyph_circle_plus,
	& stat_glyph_circle_minus,
	& stat_glyph_arrow_pt_up,
	& stat_glyph_arrow_pt_down,
	& stat_glyph_arrow_pt_left,
	& stat_glyph_arrow_pt_right,
	& stat_glyph_arrow_pt_lt,
	& stat_glyph_arrow_pt_rt,
	& stat_glyph_arrow_pt_rb,
	& stat_glyph_arrow_pt_lb,
	& stat_glyph_arrow_small_up,
	& stat_glyph_arrow_small_down,
	& stat_glyph_arrow_small_left,
	& stat_glyph_arrow_small_right,
	& stat_glyph_ellipsis,
	& stat_glyph_arrow_sort_up,
	& stat_glyph_arrow_sort_down,
	& stat_glyph_arrow_sort_left,
	& stat_glyph_arrow_sort_right,
	& stat_glyph_tabs_list_arrow,
	& stat_glyph_pn_config_button,
	& stat_glyph_pn_config_button_2007,
	& stat_glyph_pn_caption_expanded,
	& stat_glyph_pn_caption_collapsed,
	& stat_glyph_tree_plus_filled,
	& stat_glyph_tree_minus_filled,
	& stat_glyph_scroll_top,
	& stat_glyph_scroll_left,
	& stat_glyph_scroll_bottom,
	& stat_glyph_scroll_right,
	& stat_glyph_arrow_sort_up_2007,
	& stat_glyph_arrow_sort_down_2007,
	& stat_glyph_arrow_sort_left_2007,
	& stat_glyph_arrow_sort_right_2007,
};

CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA::PAINTDOCKINGCAPTIONBUTTONDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_rcClient( 0,0,0,0 )
	, m_eType( CExtPaintManager::__DCBT_EMPTY )
	, m_bHover( false )
	, m_bPushed( false )
	, m_bEnabled( false )
	, m_bSideBar( false )
	, m_bBarWndActive( false )
	, m_bFloating( false )
{
}

CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA::PAINTDOCKINGCAPTIONBUTTONDATA(
	CObject * pHelperSrc,
	const RECT & rcClient,
	int eType, // CExtPaintManager::e_docking_caption_button_t
	bool bHover,
	bool bPushed,
	bool bEnabled,
	bool bSideBar,
	bool bBarWndActive,
	bool bFloating
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_rcClient( rcClient )
	, m_eType( eType )
	, m_bHover( bHover )
	, m_bPushed( bPushed )
	, m_bEnabled( bEnabled )
	, m_bSideBar( bSideBar )
	, m_bBarWndActive( bBarWndActive )
	, m_bFloating( bFloating )
{
}

void CExtPaintManager::PaintDockingCaptionButton(
	CDC & dc,
	CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA & _pdcbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( __DCBT_VALUE_MIN <= _pdcbd.m_eType && _pdcbd.m_eType <= __DCBT_VALUE_MAX );
	if( _pdcbd.m_rcClient.IsRectEmpty() )
		return;
bool bHover = _pdcbd.m_bHover;
	if( !_pdcbd.m_bEnabled )
		bHover = false;
COLORREF clr3dFace = ::GetSysColor( COLOR_3DFACE );
COLORREF clr3dShadow = ::GetSysColor( COLOR_3DSHADOW );
COLORREF clr3dDkShadow = ::GetSysColor( COLOR_3DDKSHADOW );
COLORREF clr3dHiLight = ::GetSysColor( COLOR_3DHILIGHT );
COLORREF ColorValues[2] = { RGB(0,0,0), RGB(0,0,0), };
	ColorValues[1] =  _pdcbd.m_bEnabled  ? ::GetSysColor( COLOR_BTNTEXT ) : ::GetSysColor( COLOR_GRAYTEXT );
CRect rcGlyph( _pdcbd.m_rcClient );
	rcGlyph.NormalizeRect();
	if( rcGlyph.Width() & 1 )
		rcGlyph.left++;
	if( rcGlyph.Height() & 1 )
		rcGlyph.top++;
	if( _pdcbd.m_eType == __DCBT_CLOSE )
		_pdcbd.m_eType = __DCBT_CLOSE_DC2K;
glyph_t * pGlyph = g_DockingCaptionGlyphs[_pdcbd.m_eType];
	ASSERT( pGlyph != NULL );
	if( ! _pdcbd.m_bFloating )
		rcGlyph.InflateRect( 1, 1 );
	rcGlyph.InflateRect( 1, 1, 0, 0 );
	dc.FillSolidRect( &rcGlyph, clr3dFace );
CRect rcGlyphShape( rcGlyph );
	if( _pdcbd.m_bPushed )
		rcGlyphShape.OffsetRect( 1, 1 );
	PaintGlyphCentered( dc, rcGlyphShape, *pGlyph, ColorValues );
	if( _pdcbd.m_bEnabled && _pdcbd.m_bPushed )
		dc.Draw3dRect( &rcGlyph, clr3dShadow, clr3dHiLight );
	else if( _pdcbd.m_bEnabled && bHover )
	{
		dc.Draw3dRect( &rcGlyph, clr3dFace, clr3dDkShadow );
		rcGlyph.DeflateRect( 1, 1 );
		dc.Draw3dRect( &rcGlyph, clr3dHiLight, clr3dShadow );
	} // else if( _pdcbd.m_bEnabled && bHover )
	else
		dc.Draw3dRect( &rcGlyph, clr3dHiLight, clr3dShadow );
}

void CExtPaintManagerXP::PaintDockingCaptionButton(
	CDC & dc,
	CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA & _pdcbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( __DCBT_VALUE_MIN <= _pdcbd.m_eType && _pdcbd.m_eType <= __DCBT_VALUE_MAX );
	if( _pdcbd.m_rcClient.IsRectEmpty() )
		return;
bool bHover = _pdcbd.m_bHover;
	if( !_pdcbd.m_bEnabled )
		bHover = false;
COLORREF ColorValues[2] = { RGB(0,0,0), RGB(0,0,0), };
	if( _pdcbd.m_bEnabled )
	{
		if( ! (bHover || _pdcbd.m_bPushed) )
			ColorValues[1] =
				GetColor(
					_pdcbd.m_bSideBar
						? (_pdcbd.m_bBarWndActive ? COLOR_HIGHLIGHTTEXT : COLOR_3DDKSHADOW )
						: COLOR_HIGHLIGHTTEXT 
						,
					_pdcbd.m_pHelperSrc,
					_pdcbd.m_lParam
					);
	} // if( bEnabled )
	else
		ColorValues[1] = GetColor( COLOR_3DSHADOW, _pdcbd.m_pHelperSrc, _pdcbd.m_lParam );
CRect rcGlyph( _pdcbd.m_rcClient );
	rcGlyph.NormalizeRect();
	if( bHover || _pdcbd.m_bPushed )
	{
		dc.FillSolidRect( &rcGlyph, GetColor( _pdcbd.m_bPushed ? XPCLR_HILIGHT : CLR_3DFACE_IN, _pdcbd.m_pHelperSrc, _pdcbd.m_lParam ) );
		CRect rcBtnBorder(rcGlyph);
		rcBtnBorder.InflateRect(1,1);
		COLORREF clrBtnBorder = GetColor( _pdcbd.m_bBarWndActive ? XPCLR_HILIGHT : XPCLR_HILIGHT_BORDER, _pdcbd.m_pHelperSrc, _pdcbd.m_lParam );
		dc.Draw3dRect( rcBtnBorder, clrBtnBorder, clrBtnBorder );
	}
	if( rcGlyph.Width() & 1 )
		rcGlyph.left++;
	if( rcGlyph.Height() & 1 )
		rcGlyph.top++;
int eType = _pdcbd.m_eType;
	if( _pdcbd.m_bSideBar && !_pdcbd.m_bFloating && eType == __DCBT_CLOSE )
		eType = __DCBT_XP_THIN_CLOSE;
glyph_t * pGlyph = g_DockingCaptionGlyphs[eType];
	ASSERT( pGlyph != NULL );
	PaintGlyphCentered( dc, rcGlyph, *pGlyph, ColorValues );
}

void CExtPaintManagerNativeXP::PaintDockingCaptionButton(
	CDC & dc,
	CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA & _pdcbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT(	__DCBT_VALUE_MIN <= _pdcbd.m_eType
		&&	_pdcbd.m_eType <= __DCBT_VALUE_MAX 
		);
	if( _pdcbd.m_rcClient.IsRectEmpty() )
		return;
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintDockingCaptionButton( dc, _pdcbd );
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, _pdcbd.m_pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}
	if( _pdcbd.m_eType == __DCBT_CLOSE )
	{
		if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_WINDOW ) != NULL )
		{
			INT nStateID = 
					(!_pdcbd.m_bEnabled)
						? CBS_DISABLED
						: _pdcbd.m_bPushed
							? CBS_PUSHED
							: _pdcbd.m_bHover
								? CBS_HOT
								: CBS_NORMAL;
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					hWnd,
					dc.GetSafeHdc(), 
					WP_SMALLCLOSEBUTTON, 
					nStateID, 
					&_pdcbd.m_rcClient, 
					&_pdcbd.m_rcClient
					) == S_OK
				);
			g_PaintManager.m_UxTheme.CloseThemeData();
		}
	}
	else
	{
		CRect rcGlyph( _pdcbd.m_rcClient );
		rcGlyph.NormalizeRect();

		if(		(_pdcbd.m_bHover || _pdcbd.m_bPushed)
			&&	_pdcbd.m_bEnabled 
			)
		{
			if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_TOOLBAR ) != NULL )
			{
				INT nStateID = 
					_pdcbd.m_bPushed
						? TS_PRESSED
						: _pdcbd.m_bHover
							? TS_HOT
							: TS_NORMAL;
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWnd,
						dc.GetSafeHdc(), 
						TP_BUTTON, 
						nStateID, 
						&rcGlyph, 
						&rcGlyph
						) == S_OK
					);
				g_PaintManager.m_UxTheme.CloseThemeData();
			}
		}

		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			::GetSysColor(
				(!_pdcbd.m_bEnabled)
					? COLOR_GRAYTEXT
					: (_pdcbd.m_bHover || _pdcbd.m_bPushed) 
						? COLOR_BTNTEXT
						: COLOR_HIGHLIGHTTEXT
				)
		};
		
		glyph_t * pGlyph = g_DockingCaptionGlyphs[ _pdcbd.m_eType ];
		ASSERT( pGlyph != NULL );
		if( _pdcbd.m_bPushed )
			rcGlyph.OffsetRect( 1, 1 );
		PaintGlyphCentered( dc, rcGlyph, *pGlyph, ColorValues );
	}
}

CExtPaintManager::PAINTPUSHBUTTONDATA::PAINTPUSHBUTTONDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_bHorz( false )
	, m_rcClient( 0,0,0,0 )
	, m_sText( _T("") )
	, m_pIcon( NULL )
	, m_bFlat( false )
	, m_bHover( false )
	, m_bPushed( false )
	, m_bChecked( false )
	, m_bIndeterminate( false )
	, m_bEnabled( false )
	, m_bDrawBorder( false )
	, m_bDrawFocusRect( false )
	, m_bDefaultPushButton( false )
	, m_nTextAlignment( __ALIGN_HORIZ_LEFT | __ALIGN_VERT_CENTER )
	, m_nIconAlignment( __ALIGN_HORIZ_LEFT | __ALIGN_VERT_CENTER )
	, m_rcBorderSizes( 0, 0, 0, 0 )
	, m_rcIconMargins( 0, 0, 0, 0 )
	, m_rcTextMargins( 0, 0, 0, 0 )
	, m_nFocusMode( __EBFM_CAPTION_AREA )
	, m_hFont( NULL )
	, m_bDropDown( false )
	, m_nHelperSysCmdID( 0 )
	, m_bTransparentBackground( false )
	, m_bSeparatedDropDown( false )
	, m_bNoDropDownArrow( false )
	, m_bPushedDropDown( false )
	, m_bHoverDropDown( false )
	, m_clrForceTextDisabled( ((COLORREF)-1L) )
	, m_clrForceTextNormal( ((COLORREF)-1L) )
	, m_clrForceTextHover( ((COLORREF)-1L) )
	, m_clrForceTextPressed( ((COLORREF)-1L) )
	, m_clrForceBk( ((COLORREF)-1L) )
	, m_nDrawTextFlagsH( DT_CENTER|DT_VCENTER ) // ( DT_SINGLELINE|DT_CENTER|DT_VCENTER )
	, m_bForceUseDrawTextApiH( false )
	, m_bForceRtlLayout( false )
	, m_bForceRtlText( false )
	, m_bWordBreak( false )
	, m_clrCustomAccentEffectForIcon( COLORREF(-1L) )
	, m_bHelperColorCellButton( false )
{
}

CExtPaintManager::PAINTPUSHBUTTONDATA::PAINTPUSHBUTTONDATA(
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
	HFONT hFont, // = NULL
	bool bDropDown, // = false
	UINT nHelperSysCmdID, // = 0
	bool bTransparentBackground // = false
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_bHorz( bHorz )
	, m_rcClient( rcClient )
	, m_sText( (sText==NULL) ? _T("") : sText )
	, m_pIcon( pIcon )
	, m_bFlat( bFlat )
	, m_bHover( bHover )
	, m_bPushed( bPushed )
	, m_bChecked( false )
	, m_bIndeterminate( bIndeterminate )
	, m_bEnabled( bEnabled )
	, m_bDrawBorder( bDrawBorder )
	, m_bDrawFocusRect( bDrawFocusRect )
	, m_bDefaultPushButton( bDefaultPushButton )
	, m_nTextAlignment( eAlign )
	, m_nIconAlignment( __ALIGN_HORIZ_LEFT | __ALIGN_VERT_CENTER )
	, m_rcBorderSizes( 0, 0, 0, 0 )
	, m_rcIconMargins( 0, 0, 0, 0 )
	, m_rcTextMargins( 0, 0, 0, 0 )
	, m_hFont( hFont )
	, m_bDropDown( bDropDown )
	, m_nHelperSysCmdID( nHelperSysCmdID )
	, m_bTransparentBackground( bTransparentBackground )
	, m_bSeparatedDropDown( false )
	, m_bNoDropDownArrow( false )
	, m_bPushedDropDown( false )
	, m_bHoverDropDown( false )
	, m_clrForceTextDisabled( ((COLORREF)-1L) )
	, m_clrForceTextNormal( ((COLORREF)-1L) )
	, m_clrForceTextHover( ((COLORREF)-1L) )
	, m_clrForceTextPressed( ((COLORREF)-1L) )
	, m_clrForceBk( ((COLORREF)-1L) )
	, m_nDrawTextFlagsH( 0 ) // ( DT_SINGLELINE )
	, m_bForceUseDrawTextApiH( false )
	, m_bForceRtlLayout( false )
	, m_bForceRtlText( false )
	, m_bWordBreak( false )
	, m_clrCustomAccentEffectForIcon( COLORREF(-1L) )
	, m_bHelperColorCellButton( false )
{
	if( (m_nTextAlignment&__ALIGN_HORIZ_CENTER) != 0 )
		m_nDrawTextFlagsH |= DT_CENTER;
	else if( (m_nTextAlignment&__ALIGN_HORIZ_RIGHT) != 0 )
		m_nDrawTextFlagsH |= DT_RIGHT;
	else
		m_nDrawTextFlagsH |= DT_LEFT;

	if( (m_nTextAlignment&__ALIGN_VERT_CENTER) != 0 )
		m_nDrawTextFlagsH |= DT_VCENTER;
	else
		m_nDrawTextFlagsH |= DT_TOP;
}

bool CExtPaintManager::PAINTPUSHBUTTONDATA::IsRTL()
{
	if( m_bForceRtlLayout )
		return true;
	if( m_pHelperSrc == NULL )
		return false;
	ASSERT_VALID( m_pHelperSrc );
CExtPopupMenuWnd * pPopup =
		DYNAMIC_DOWNCAST( CExtPopupMenuWnd, m_pHelperSrc );
	if( pPopup != NULL )
		return pPopup->OnQueryLayoutRTL();
CExtBarButton * pTBB =
		DYNAMIC_DOWNCAST( CExtBarButton, m_pHelperSrc );
	if( pTBB != NULL )
	{
		CExtToolControlBar * pToolBar =
			pTBB->GetBar();
		if( pToolBar != NULL )
		{
			if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
				return true;
			if(		pToolBar->m_pDockSite != NULL
				&&	(pToolBar->m_pDockSite->GetExStyle()&WS_EX_LAYOUTRTL) != 0
				)
				return true;
			if( (pToolBar->GetParent()->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
				return true;
		} // if( pToolBar != NULL )
	} // if( pTBB != NULL )
	return false;
}

LONG CExtPaintManager::_CalcSplitButtonExtentImpl( const CObject * pObjButton, bool bHorz ) const
{
	ASSERT_VALID( this );
	if( pObjButton != NULL )
	{
		ASSERT_VALID( pObjButton );
		if( pObjButton->IsKindOf( RUNTIME_CLASS(CExtBarButton) ) )
			return ((CExtBarButton*)pObjButton)->_CalcSplitButtonExtent( bHorz );
		if( pObjButton->IsKindOf( RUNTIME_CLASS(CExtButton) ) )
			return ((CExtButton*)pObjButton)->_CalcSplitButtonExtent();
	}
	return _CalcSplitButtonExtent( pObjButton, bHorz );
}

LONG CExtPaintManager::_CalcSplitButtonExtent( const CObject * pObjButton, bool bHorz ) const
{
	ASSERT_VALID( this ); pObjButton;
LONG nSplitButtonExtent = __DROPDOWN_ARROW_GAP * 2 + ( bHorz ? g_glyph_btn_expand_bottom.Size().cx : g_glyph_btn_expand_right.Size().cy );
	return nSplitButtonExtent;
}

void CExtPaintManager::PaintPushButton(
	CDC & dc,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( _ppbd.m_pHelperSrc != NULL )
	{
		CExtBarButton * pTBB = DYNAMIC_DOWNCAST( CExtBarButton, _ppbd.m_pHelperSrc );
		if( pTBB != NULL )
		{
			if( pTBB->IsRibbonPaintingMode() )
			{
				Ribbon_PaintPushButton( dc, _ppbd );
				return;
			}
			CExtToolControlBar * pBar = pTBB->GetBar();
			if(		pBar != NULL
				&&	pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) )
				&&	(! pTBB->IsNoRibbonLayout() )
				&&	(! pTBB->IsKindOf( RUNTIME_CLASS( CExtBarMdiRightButton ) ) )
				)
			{
				Ribbon_PaintPushButton( dc, _ppbd );
				return;
			}
		} // if( pTBB != NULL )
	} // if( _ppbd.m_pHelperSrc != NULL )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

bool bRTL = _ppbd.IsRTL();

	if( !_ppbd.m_bEnabled )
	{
		_ppbd.m_bHover = false;
		_ppbd.m_bDrawFocusRect = false;
		_ppbd.m_bIndeterminate = false;
	}

	if( _ppbd.m_bSeparatedDropDown && (_ppbd.m_bPushed || _ppbd.m_bPushedDropDown) )
		_ppbd.m_bHover = false;

COLORREF clrOldText = dc.GetTextColor();
COLORREF clrOldBk = dc.GetBkColor();
int nOldBkMode = dc.SetBkMode( TRANSPARENT );

CBrush * pBrush = NULL;
COLORREF bgColor = COLORREF(-1L);
	if( _ppbd.m_clrForceBk != ((COLORREF)-1L) )
		bgColor = _ppbd.m_clrForceBk;
	else if( _ppbd.m_bHover || _ppbd.m_bPushed )
		bgColor = GetColor( CLR_3DFACE_IN, _ppbd.m_pHelperSrc, _ppbd.m_lParam );
	else if( _ppbd.m_bIndeterminate )
	{
		bgColor = GetColor( CLR_3DFACE_IN, _ppbd.m_pHelperSrc, _ppbd.m_lParam );
		pBrush = &m_brushLightestSystem;
	}
	else
		bgColor = GetColor( CLR_3DFACE_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam );

bool bPushedDropDown = false;
	if( (_ppbd.m_bPushed && !_ppbd.m_bSeparatedDropDown)
		|| _ppbd.m_bPushedDropDown
		)
		bPushedDropDown = true;

CRect rectClient( _ppbd.m_rcClient );
CRect rectCaption( _ppbd.m_rcClient );
CRect rectDropDown( 0, 0, 0, 0 );
	if( _ppbd.m_bDropDown )
	{
		rectDropDown = rectClient;
		if( _ppbd.m_bHorz )
		{
			if( bRTL )
			{
				rectCaption.left += _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
				rectDropDown.right = rectCaption.left + 1;
			} // if( bRTL )
			else
			{
				rectCaption.right -= _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
				rectDropDown.left = rectCaption.right - 1;
			} // else from if( bRTL )
		} // if( _ppbd.m_bHorz )
		else
		{
			rectCaption.bottom -= _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
			rectDropDown.top = rectCaption.bottom - 1;
		} // if( _ppbd.m_bHorz )

		if(		!_ppbd.m_bTransparentBackground 
			||	_ppbd.m_clrForceBk != ((COLORREF)-1L)
			)
		{
			if(		_ppbd.m_bEnabled
				&&	bPushedDropDown
				&&	_ppbd.m_bHover
				&&	_ppbd.m_clrForceBk == ((COLORREF)-1L)
				)
				dc.FillRect( &rectDropDown, &m_brushLightDefault );
			else
				dc.FillSolidRect( &rectDropDown, bgColor );
		} // if( ! _ppbd.m_bTransparentBackground )

		if( _ppbd.m_bSeparatedDropDown )
		{
			rectClient = rectCaption;
			if( bPushedDropDown )
			{
				if( (!_ppbd.m_bFlat) || _ppbd.m_bDrawBorder )
					dc.Draw3dRect(
						rectDropDown,
						GetColor( CLR_3DSHADOW_IN, _ppbd.m_pHelperSrc, _ppbd.m_lParam ),
						GetColor( CLR_3DHILIGHT_IN, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
						);
			} // if( bPushedDropDown )
			else
			{
				if( (!_ppbd.m_bFlat) || (_ppbd.m_bHover && _ppbd.m_bDrawBorder) )
					dc.Draw3dRect(
						rectDropDown,
						GetColor( CLR_3DHILIGHT_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam ),
						GetColor( CLR_3DSHADOW_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
						);
			} // else from if( bPushedDropDown )
		} // if( _ppbd.m_bSeparatedDropDown )
	} // if( _ppbd.m_bDropDown )

	if(		(!_ppbd.m_bTransparentBackground) 
		||	_ppbd.m_clrForceBk != ((COLORREF)-1L)
		)
	{
		if(			_ppbd.m_bEnabled
				&&	_ppbd.m_bPushed
				&&	_ppbd.m_bHover
				&&	_ppbd.m_clrForceBk == ((COLORREF)-1L)
			)
			dc.FillRect( &rectClient, &m_brushLightDefault );
		else
		{
			if( pBrush != NULL )
				dc.FillRect( &rectClient, pBrush );
			else
				dc.FillSolidRect( &rectClient, bgColor );
		}
	} // if( !_ppbd.m_bTransparentBackground )
	
	// Draw pressed button
	if( _ppbd.m_bPushed || _ppbd.m_bIndeterminate )
	{
		if( pBrush != NULL )
			dc.FillRect( &rectClient, pBrush );
		if( (!_ppbd.m_bFlat) || _ppbd.m_bDrawBorder )
			dc.Draw3dRect(
				rectClient,
				GetColor( CLR_3DSHADOW_IN, _ppbd.m_pHelperSrc, _ppbd.m_lParam ),
				GetColor( CLR_3DHILIGHT_IN, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
				);
	} // if( _ppbd.m_bPushed || _ppbd.m_bIndeterminate )
	else // ...else draw non pressed button
	{
		if( (!_ppbd.m_bFlat) || (_ppbd.m_bHover && _ppbd.m_bDrawBorder) )
			dc.Draw3dRect(
				rectClient,
				GetColor( CLR_3DHILIGHT_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam ),
				GetColor( CLR_3DSHADOW_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
				);
		if( (!_ppbd.m_bFlat) && _ppbd.m_bHover )
		{
			CRect cc( _ppbd.m_rcClient );
			dc.Draw3dRect(
				cc,
				GetColor( CLR_3DSHADOW_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam ),
				GetColor( CLR_3DDKSHADOW_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
				);
			cc.DeflateRect(1,1);
			dc.Draw3dRect(
				cc,
				GetColor( CLR_3DHILIGHT_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam ),
				GetColor( CLR_3DSHADOW_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
				);
		} // if( (!_ppbd.m_bFlat) && _ppbd.m_bHover )
	} // else from if( _ppbd.m_bPushed )

	if( _ppbd.m_bDropDown && (! _ppbd.m_bNoDropDownArrow ) )
	{
		CRect rectGlyph( rectDropDown );
		if( bPushedDropDown )
			rectGlyph.OffsetRect( GetPushedOffset() );
		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			GetColor(
				_ppbd.m_bEnabled
					? CLR_TEXT_OUT
					: CLR_3DSHADOW_OUT
					,
				_ppbd.m_pHelperSrc,
				_ppbd.m_lParam
				)
		};
		if( bRTL && (rectGlyph.Width()&1) != 0 )
			rectGlyph.right++;
		PaintGlyphCentered(
			dc,
			rectGlyph,
			_ppbd.m_bHorz ? g_glyph_btn_expand_bottom : g_glyph_btn_expand_right,
			ColorValues
			);
	} // if( _ppbd.m_bDropDown && (! _ppbd.m_bNoDropDownArrow ) )

	dc.SetBkColor( bgColor );

	rectCaption.DeflateRect( _ppbd.m_rcBorderSizes );
	rectClient = rectCaption;
CRect rcFocus( rectCaption );

	// Draw icon
	if(		rectClient.bottom > rectClient.top
		&&	rectClient.right > rectClient.left
		)
		PaintPushButtonIcon( dc, rectClient, rectCaption, rcFocus, _ppbd );

CExtSafeString sTitle( _T("") );
	if( _ppbd.m_sText != NULL )
		sTitle = _ppbd.m_sText;
	// Draw text
	if(		rectCaption.bottom > rectCaption.top
		&&	rectCaption.right > rectCaption.left
		&&	(!sTitle.IsEmpty())
		)
		PaintPushButtonText( dc, rectCaption, _ppbd );

	// Draw focus rectangle
	if(		_ppbd.m_bDrawFocusRect 
		&&	( ( !sTitle.IsEmpty() ) || _ppbd.m_nFocusMode == __EBFM_CLIENT_AREA )
		&&	rcFocus.bottom > rcFocus.top 
		&&	rcFocus.right > rcFocus.left
		)
	{
		rcFocus.DeflateRect( 2, 2 );
		dc.DrawFocusRect( &rcFocus );
	}

	dc.SetBkMode( nOldBkMode );
	dc.SetBkColor( clrOldBk );
	dc.SetTextColor( clrOldText );
}

void CExtPaintManager::PaintPushButtonMdiRight(
	CDC & dc,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	PaintPushButton( dc, _ppbd );
}

void CExtPaintManager::PaintPushButtonText(
	CDC & dc,
	CRect & rcCaption,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;
CExtSafeString sTitle( _T("") );
	if( _ppbd.m_sText != NULL )
		sTitle = _ppbd.m_sText;
bool bRTL = _ppbd.IsRTL();
	if(		rcCaption.bottom > rcCaption.top
		&&	rcCaption.right > rcCaption.left
		&&	(!sTitle.IsEmpty())
		)
	{
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		if( _ppbd.m_bEnabled )
		{
			if( _ppbd.m_bHover || _ppbd.m_bPushed )
			{
				if(			_ppbd.m_bPushed
						&&	_ppbd.m_clrForceTextPressed != ((COLORREF)-1L)
						)
						dc.SetTextColor( _ppbd.m_clrForceTextPressed );
				else if(	_ppbd.m_bHover
						&&	_ppbd.m_clrForceTextHover != ((COLORREF)-1L)
						)
						dc.SetTextColor( _ppbd.m_clrForceTextHover );
				else if( _ppbd.m_clrForceTextNormal != ((COLORREF)-1L) ) 
						dc.SetTextColor( _ppbd.m_clrForceTextNormal );
				else
						dc.SetTextColor( GetColor( COLOR_BTNTEXT, _ppbd.m_pHelperSrc, _ppbd.m_lParam ) );
			}
			else 
			{
				dc.SetTextColor( 
					( _ppbd.m_clrForceTextNormal == ((COLORREF)-1L) )
						? GetColor( COLOR_BTNTEXT, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
						: _ppbd.m_clrForceTextNormal
					);
			}
		}
		else
			dc.SetTextColor(
				( _ppbd.m_clrForceTextDisabled == ((COLORREF)-1L) )
					? GetColor( COLOR_3DFACE, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
					: _ppbd.m_clrForceTextDisabled
				);
		CFont * pOldBtnFont = NULL;
		CFont * pCurrFont = NULL;
		if( _ppbd.m_hFont != NULL )
			pCurrFont = CFont::FromHandle(_ppbd.m_hFont);
		else
		{
			if( _ppbd.m_bDefaultPushButton )
				pCurrFont = &m_FontBold;
			else
				pCurrFont = &m_FontNormal;
		} // else from if( _ppbd.m_hFont != NULL )
		ASSERT( pCurrFont != NULL );
		pOldBtnFont = dc.SelectObject( pCurrFont );
		ASSERT( pOldBtnFont != NULL );

		CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
		if( ! _ppbd.m_bHorz )
			eLO = CExtRichContentLayout::__ELOT_90_CW;

		CRect rcTextLocation( 0, 0, 0, 0 );
		UINT nDtMeasureFlags = DT_LEFT | DT_TOP | DT_CALCRECT | ((bRTL || _ppbd.m_bForceRtlText) ? DT_RTLREADING : 0);
		if( _ppbd.m_bWordBreak )
		{
			rcTextLocation = rcCaption;
			rcTextLocation.OffsetRect( -rcTextLocation.TopLeft() );
			rcTextLocation.bottom = rcTextLocation.top;
			nDtMeasureFlags |= DT_WORDBREAK;
		}
		else
			nDtMeasureFlags |= DT_SINGLELINE;

		CExtRichContentLayout::stat_DrawText(
			CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
			dc.m_hDC, LPCTSTR(sTitle), rcTextLocation, nDtMeasureFlags, 0
			);
		if(		( bRTL || _ppbd.m_bForceRtlText )
			&&	eLO == CExtRichContentLayout::__ELOT_NORMAL
			&&	_ppbd.m_pHelperSrc != NULL
			&&	_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtButton) )
			)
			rcTextLocation.InflateRect( 0, 0, 2, 0 ); // TO-FIX: incorrect horizontally-oriented text position on RTL buttons
		rcTextLocation.OffsetRect( rcCaption.TopLeft() - rcTextLocation.TopLeft() );
			
		UINT nDtDrawFlags = 0;
		if( (_ppbd.m_nTextAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_RIGHT )
		{
			nDtDrawFlags |= DT_RIGHT;
			rcTextLocation.OffsetRect(
				rcCaption.Width() - rcTextLocation.Width(),
				0
				);
		}
		else if( (_ppbd.m_nTextAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
		{
			nDtDrawFlags |= DT_CENTER;
			rcTextLocation.OffsetRect(
				( rcCaption.Width() - rcTextLocation.Width() ) / 2,
				0
				);
		}
		else
			nDtDrawFlags |= DT_LEFT;
		if( (_ppbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		{
			nDtDrawFlags |= DT_BOTTOM;
			rcTextLocation.OffsetRect(
				0,
				rcCaption.Height() - rcTextLocation.Height()
				);
		}
		else if( (_ppbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_TOP )
			nDtDrawFlags |= DT_TOP;
		else
		{
			nDtDrawFlags |= DT_VCENTER;
			rcTextLocation.OffsetRect(
				0,
				( rcCaption.Height() - rcTextLocation.Height() ) / 2
				);
		}

		if( _ppbd.m_bWordBreak )
			nDtDrawFlags |= DT_WORDBREAK;
		else
			nDtDrawFlags |= DT_SINGLELINE;

		if( _ppbd.m_bPushed )
		{
			CSize szPushedOffset = GetPushedOffset();
			rcTextLocation.OffsetRect( szPushedOffset );
		}

		if( _ppbd.m_bForceUseDrawTextApiH )
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, LPCTSTR(sTitle), &rcTextLocation, nDtDrawFlags, 0
				);
		else
		{
			if(		_ppbd.m_bEnabled 
				||	_ppbd.m_clrForceTextDisabled != ((COLORREF)-1L)
				)
				CExtRichContentLayout::stat_DrawText(
					CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
					dc.m_hDC, LPCTSTR(sTitle), &rcTextLocation, nDtDrawFlags, 0
					);
			else
			{
				CRect rcDrawTextOffs = rcTextLocation;
				rcDrawTextOffs.OffsetRect( 1, 1 );
				COLORREF clrTextRestore = dc.SetTextColor( GetColor( COLOR_3DHILIGHT, _ppbd.m_pHelperSrc, _ppbd.m_lParam ) );
				CExtRichContentLayout::stat_DrawText(
					CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
					dc.m_hDC, LPCTSTR(sTitle), &rcDrawTextOffs, nDtDrawFlags, 0
					);
				dc.SetTextColor( GetColor( COLOR_3DSHADOW, _ppbd.m_pHelperSrc, _ppbd.m_lParam ) );
				CExtRichContentLayout::stat_DrawText(
					CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
					dc.m_hDC, LPCTSTR(sTitle), rcTextLocation, nDtDrawFlags, 0
					);
				dc.SetTextColor( clrTextRestore );
			} // else from if( _ppbd.m_bEnabled )
		} // else from if( _ppbd.m_bForceUseDrawTextApiH )
		dc.SelectObject( pOldBtnFont );
		dc.SetBkMode( nOldBkMode );
	}
}

void CExtPaintManager::PaintPushButtonIcon(
	CDC & dc,
	CRect & rcClient,
	CRect & rcCaption,
	CRect & rcFocus,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;

bool bRTL = _ppbd.IsRTL();
bool bDrawIcon = true;

bool bRibbonMode = false;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if(		_ppbd.m_pHelperSrc != NULL
		&&	_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtRibbonButton) )
		)
	{ // ribbon button
		bRibbonMode = true;
		CExtRibbonButton * pTBB = STATIC_DOWNCAST( CExtRibbonButton, _ppbd.m_pHelperSrc );
		INT nILV = pTBB->RibbonILV_Get();
		if( nILV == __EXT_RIBBON_ILV_SIMPLE_LARGE )
		{
			bDrawIcon = false;
			INT nTextToIconDistance = Ribbon_GetTextToIconDistance( dc, nILV, pTBB, _ppbd.m_lParam );
//			CSize _sizeIcon = _ppbd.m_pIcon->GetSize();
			CSize _sizeIcon = Ribbon_GetIconSize( pTBB, nILV, _ppbd.m_lParam );
			CRect rcIconPaint = rcCaption;
			CRect rcCP = pTBB->OnRibbonGetContentPadding();
			rcIconPaint.bottom = rcIconPaint.top + _sizeIcon.cx + rcCP.bottom + nTextToIconDistance;
			rcIconPaint.DeflateRect( rcCP.left, rcCP.top, rcCP.right, 0 );
			rcCaption.top += _sizeIcon.cy + nTextToIconDistance + rcCP.top + rcCP.bottom;
			rcCaption.DeflateRect( rcCP.left, 0, rcCP.right, rcCP.bottom );
			if( _ppbd.m_pIcon != NULL && (! _ppbd.m_pIcon->IsEmpty()) )
			{
				PaintIcon(
					dc,
					_ppbd.m_bHorz,
					_ppbd.m_pIcon,
					rcIconPaint,
					_ppbd.m_bPushed,
					_ppbd.m_bEnabled,
					_ppbd.m_bHover,
					_ppbd.m_nIconAlignment,
					_ppbd.m_bForceRtlLayout && _ppbd.m_bHorz,
					NULL,
					NULL,
					&_ppbd.m_rcIconMargins,
					_ppbd.m_clrCustomAccentEffectForIcon
					);
			} // if( _ppbd.m_pIcon != NULL && (! _ppbd.m_pIcon->IsEmpty()) )
		} // if( nILV == __EXT_RIBBON_ILV_SIMPLE_LARGE )
	} // ribbon button
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	// Draw the icon
	if(		bDrawIcon
		&&	rcClient.bottom > rcClient.top
		&&	rcClient.right > rcClient.left
		)
	{
		if( _ppbd.m_pIcon != NULL && (! _ppbd.m_pIcon->IsEmpty()) )
		{
			PaintIcon(
				dc,
				_ppbd.m_bHorz,
				_ppbd.m_pIcon,
				rcClient,
				_ppbd.m_bPushed,
				_ppbd.m_bEnabled,
				_ppbd.m_bHover,
				_ppbd.m_nIconAlignment,
				_ppbd.m_bForceRtlLayout && _ppbd.m_bHorz,
				NULL,
				&rcCaption,
				&_ppbd.m_rcIconMargins,
				_ppbd.m_clrCustomAccentEffectForIcon
				);
			
			if( _ppbd.m_nFocusMode == __EBFM_CAPTION_AREA )
				rcFocus = rcCaption;

			if(		_ppbd.m_bHorz
				&&	_ppbd.m_pHelperSrc != NULL
				&&	_ppbd.m_pHelperSrc->IsKindOf(RUNTIME_CLASS(CExtPopupMenuWnd))
				)
			{ 
				// painting tool button with icon in popup menu
				if( bRTL )
					rcCaption.right -= 3;
				else
					rcCaption.left += 3;
			}

			if( ! rcCaption.IsRectEmpty() )
			{
				if( _ppbd.m_bHorz )
				{
					rcCaption.DeflateRect( 
						_ppbd.m_rcTextMargins.left,
						_ppbd.m_rcTextMargins.top,
						_ppbd.m_rcTextMargins.right,
						_ppbd.m_rcTextMargins.bottom
						);
				}
				else
				{
					rcCaption.DeflateRect( 
						_ppbd.m_rcTextMargins.top,
						_ppbd.m_rcTextMargins.right,
						_ppbd.m_rcTextMargins.bottom,
						_ppbd.m_rcTextMargins.left
						);			
				}
			}
		} // if( _ppbd.m_pIcon != NULL && (! _ppbd.m_pIcon->IsEmpty()) )
		else
		{
			if( ! bRibbonMode )
			{
				if(		_ppbd.m_pHelperSrc != NULL
					&&	_ppbd.m_pHelperSrc->IsKindOf(RUNTIME_CLASS(CExtPopupMenuWnd))
					)
				{
					if( _ppbd.m_bHorz )
						rcCaption.DeflateRect( 3, 0 );
					else
						rcCaption.DeflateRect( 0, 3 );
				}
			} // if( ! bRibbonMode )
			COLORREF clrForceGlyphColor = COLORREF(-1L);
			if(		_ppbd.m_pHelperSrc != NULL
				&&	(	_ppbd.m_clrForceTextDisabled != COLORREF(-1L)
					||	_ppbd.m_clrForceTextNormal   != COLORREF(-1L)
					||	_ppbd.m_clrForceTextHover    != COLORREF(-1L)
					||	_ppbd.m_clrForceTextPressed  != COLORREF(-1L)
					)
				&&	_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtBarMdiRightButton) )
				)
			{
				if( _ppbd.m_bEnabled )
				{
					if( _ppbd.m_bPushed )
						clrForceGlyphColor = _ppbd.m_clrForceTextPressed;
					else if( _ppbd.m_bHover )
						clrForceGlyphColor = _ppbd.m_clrForceTextHover;
					else if( _ppbd.m_bEnabled )
						clrForceGlyphColor = _ppbd.m_clrForceTextNormal;
				}
				else
					clrForceGlyphColor = _ppbd.m_clrForceTextDisabled;
			}
			PaintSysCmdGlyph(
				dc,
				_ppbd.m_nHelperSysCmdID,
				rcClient,
				false,
				_ppbd.m_bPushed,
				_ppbd.m_bEnabled,
				clrForceGlyphColor
				);
		} // else from if( _ppbd.m_pIcon != NULL && (! _ppbd.m_pIcon->IsEmpty()) )
	}
}

CExtPaintManager::PAINTCHECKRADIOBUTTONDATA::PAINTCHECKRADIOBUTTONDATA()
	: m_pHelperSrc( NULL )
	, m_eState( BOX_UNCHECKED )
	, m_lParam( 0L )
	, m_bLeftText( false )
	, m_bMultiline( false )
	, m_rcClient( 0,0,0,0 )
	, m_sText( _T("") )
	, m_bHover( false )
	, m_bPushed( false )
	, m_bEnabled( false )
	, m_bDrawFocusRect( false )
	, m_nTextAlignment( __ALIGN_HORIZ_LEFT | __ALIGN_VERT_CENTER )
	, m_rcTextMargins( 1, 1, 1, 1 )
	, m_hFont( NULL )
	, m_bTransparentBackground( false )
	, m_clrForceTextDisabled( ((COLORREF)-1L) )
	, m_clrForceTextNormal( ((COLORREF)-1L) )
	, m_clrForceTextHover( ((COLORREF)-1L) )
	, m_clrForceTextPressed( ((COLORREF)-1L) )
	, m_clrForceBk( ((COLORREF)-1L) )
{
}

CExtPaintManager::PAINTCHECKRADIOBUTTONDATA::PAINTCHECKRADIOBUTTONDATA(
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
	HFONT hFont, // = NULL
	bool bTransparentBackground // = false
	)
	: m_pHelperSrc( pHelperSrc )
	, m_eState( eState )
	, m_lParam( 0L )
	, m_bLeftText( bLeftText )
	, m_bMultiline( bMultiline )
	, m_rcClient( rcClient )
	, m_sText( (sText==NULL) ? _T("") : sText )
	, m_bHover( bHover )
	, m_bPushed( bPushed )
	, m_bEnabled( bEnabled )
	, m_bDrawFocusRect( bDrawFocusRect )
	, m_nTextAlignment( eAlign )
	, m_rcTextMargins( 1, 1, 1, 1 )
	, m_hFont( hFont )
	, m_bTransparentBackground( bTransparentBackground )
	, m_clrForceTextDisabled( ((COLORREF)-1L) )
	, m_clrForceTextNormal( ((COLORREF)-1L) )
	, m_clrForceTextHover( ((COLORREF)-1L) )
	, m_clrForceTextPressed( ((COLORREF)-1L) )
	, m_clrForceBk( ((COLORREF)-1L) )
{
}

CSize CExtPaintManager::GetCheckButtonBoxSize(
	CDC & dc,
	CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CSize _sizeBox( 13, 13 );
	_sizeBox.cx = UiScalingDo( _sizeBox.cx, __EUIST_X );
	_sizeBox.cy = UiScalingDo( _sizeBox.cy, __EUIST_Y );
	if( _pcbd.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pcbd.m_pHelperSrc );

		CWnd * pWnd = NULL;
		if( _pcbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarButton ) ) ) 
		{
			CExtBarButton * pTBB = STATIC_DOWNCAST( CExtBarButton, _pcbd.m_pHelperSrc );
			ASSERT_VALID( pTBB );
			CExtToolControlBar * pBar = pTBB->GetBar();
			ASSERT( pBar != NULL );
			ASSERT_VALID( pBar );
			pWnd = pBar;
		}
		else 
			pWnd = DYNAMIC_DOWNCAST( CWnd, _pcbd.m_pHelperSrc );
		ASSERT( pWnd != NULL );

		if(		g_PaintManager.m_UxTheme.IsControlsThemed()
			&&	g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_BUTTON ) != NULL
			)
		{
			INT nState = 0;
			switch( _pcbd.m_eState ) 
			{
			case CExtPaintManager::BOX_UNCHECKED:
				nState = CBS_UNCHECKEDNORMAL;
				break;
			case CExtPaintManager::BOX_LDOWN_UNCHECKED:
				nState = CBS_UNCHECKEDPRESSED;
				break;
			case CExtPaintManager::BOX_DISABLED_UNCHECKED:
				nState = CBS_UNCHECKEDDISABLED;
				break;
			case CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED:
				nState = CBS_UNCHECKEDHOT;
				break;

			case CExtPaintManager::BOX_CHECKED:
				nState = CBS_CHECKEDNORMAL;
				break;
			case CExtPaintManager::BOX_LDOWN_CHECKED:
				nState = CBS_CHECKEDPRESSED;
				break;
			case CExtPaintManager::BOX_MOUSE_HOVER_CHECKED:
				nState = CBS_CHECKEDHOT;
				break;
			case CExtPaintManager::BOX_DISABLED_CHECKED:
				nState = CBS_CHECKEDDISABLED;
				break;

			case CExtPaintManager::BOX_INDETERMINATE:
				nState = CBS_MIXEDNORMAL;
				break;
			case CExtPaintManager::BOX_LDOWN_INDETERMINATE:
				nState = CBS_MIXEDPRESSED;
				break;
			case CExtPaintManager::BOX_MOUSE_HOVER_INDETERMINATE:
				nState = CBS_MIXEDHOT;
				break;
			case CExtPaintManager::BOX_DISABLED_INDETERMINATE:
				nState = CBS_MIXEDDISABLED;
				break;
			} // switch( _pcrbd.m_eState ) 
			
			VERIFY( 
				g_PaintManager.m_UxTheme.GetThemePartSize(
					dc.GetSafeHdc(),
					BP_CHECKBOX,
					nState,
					NULL,
					CExtUxTheme::__EXT_UX_TS_TRUE,
					&_sizeBox
					) == S_OK
				);

			g_PaintManager.m_UxTheme.CloseThemeData( true );
		}	
	} // if( _pcbd.m_pHelperSrc != NULL )
	return _sizeBox;
}

CSize CExtPaintManager::GetRadioButtonBoxSize(
	CDC & dc,
	CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcbd
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CSize _sizeBox( 13, 13 );
	_sizeBox.cx = UiScalingDo( _sizeBox.cx, __EUIST_X );
	_sizeBox.cy = UiScalingDo( _sizeBox.cy, __EUIST_Y );
	if( _pcbd.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pcbd.m_pHelperSrc );
		CWnd * pWnd =
			DYNAMIC_DOWNCAST( CWnd, _pcbd.m_pHelperSrc );
		ASSERT( pWnd != NULL );
		if(		g_PaintManager.m_UxTheme.IsControlsThemed()
			&&	g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_BUTTON ) != NULL
			)
		{
			INT nState = 0;
			switch( _pcbd.m_eState ) 
			{
			case CExtPaintManager::BOX_UNCHECKED:
				nState = CBS_UNCHECKEDNORMAL;
				break;
			case CExtPaintManager::BOX_LDOWN_UNCHECKED:
				nState = CBS_UNCHECKEDPRESSED;
				break;
			case CExtPaintManager::BOX_DISABLED_UNCHECKED:
				nState = CBS_UNCHECKEDDISABLED;
				break;
			case CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED:
				nState = CBS_UNCHECKEDHOT;
				break;

			case CExtPaintManager::BOX_CHECKED:
				nState = CBS_CHECKEDNORMAL;
				break;
			case CExtPaintManager::BOX_LDOWN_CHECKED:
				nState = CBS_CHECKEDPRESSED;
				break;
			case CExtPaintManager::BOX_MOUSE_HOVER_CHECKED:
				nState = CBS_CHECKEDHOT;
				break;
			case CExtPaintManager::BOX_DISABLED_CHECKED:
				nState = CBS_CHECKEDDISABLED;
				break;
			} // switch( _pcrbd.m_eState ) 
			
			VERIFY( 
				g_PaintManager.m_UxTheme.GetThemePartSize(
					dc.GetSafeHdc(), 
					BP_RADIOBUTTON, 
					nState, 
					NULL, 
					CExtUxTheme::__EXT_UX_TS_TRUE,
					&_sizeBox
					) == S_OK
				);

			g_PaintManager.m_UxTheme.CloseThemeData( true );
		}	
	} // if( _pcbd.m_pHelperSrc != NULL )
	return _sizeBox;
}

void CExtPaintManager::PaintCheckButtonBox(
	CDC & dc,
	CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcrbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pcrbd.m_rcClient.IsRectEmpty() )
		return;
	if( _pcrbd.m_pHelperSrc == NULL )
		return;

	ASSERT_VALID( _pcrbd.m_pHelperSrc );

CWnd * pWnd = NULL;
	if( _pcrbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarButton ) ) ) 
	{
		CExtBarButton * pTBB = STATIC_DOWNCAST( CExtBarButton, _pcrbd.m_pHelperSrc );
		ASSERT_VALID( pTBB );
		CExtToolControlBar * pBar = pTBB->GetBar();
		ASSERT( pBar != NULL );
		ASSERT_VALID( pBar );
		pWnd = pBar;
	}
	else 
		pWnd = DYNAMIC_DOWNCAST( CWnd, _pcrbd.m_pHelperSrc );
	ASSERT( pWnd != NULL );

INT nState = 0;
INT nStateForUxTheme = 0;

	switch( _pcrbd.m_eState ) 
	{
	case CExtPaintManager::BOX_UNCHECKED:
		nStateForUxTheme = CBS_UNCHECKEDNORMAL;
		nState = DFCS_BUTTONCHECK;
		break;
	case CExtPaintManager::BOX_LDOWN_UNCHECKED:
		nStateForUxTheme = CBS_UNCHECKEDPRESSED;
		nState = DFCS_BUTTONCHECK | DFCS_PUSHED;
		break;
	case CExtPaintManager::BOX_DISABLED_UNCHECKED:
		nStateForUxTheme = CBS_UNCHECKEDDISABLED;
		nState = DFCS_BUTTONCHECK | DFCS_INACTIVE;
		break;
	case CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED:
		nStateForUxTheme = CBS_UNCHECKEDHOT;
		nState = DFCS_BUTTONCHECK;
		break;

	case CExtPaintManager::BOX_CHECKED:
		nStateForUxTheme = CBS_CHECKEDNORMAL;
		nState = DFCS_BUTTONCHECK | DFCS_CHECKED;
		break;
	case CExtPaintManager::BOX_LDOWN_CHECKED:
		nStateForUxTheme = CBS_CHECKEDPRESSED;
		nState = DFCS_BUTTONCHECK | DFCS_CHECKED | DFCS_PUSHED;
		break;
	case CExtPaintManager::BOX_MOUSE_HOVER_CHECKED:
		nStateForUxTheme = CBS_CHECKEDHOT;
		nState = DFCS_BUTTONCHECK | DFCS_CHECKED;
		break;
	case CExtPaintManager::BOX_DISABLED_CHECKED:
		nStateForUxTheme = CBS_CHECKEDDISABLED;
		nState = DFCS_BUTTONCHECK | DFCS_CHECKED | DFCS_INACTIVE;
		break;

	case CExtPaintManager::BOX_INDETERMINATE:
		nStateForUxTheme = CBS_MIXEDNORMAL;
		nState = DFCS_BUTTON3STATE | DFCS_CHECKED;
		break;
	case CExtPaintManager::BOX_LDOWN_INDETERMINATE:
		nStateForUxTheme = CBS_MIXEDPRESSED;
		nState = DFCS_BUTTON3STATE | DFCS_CHECKED | DFCS_PUSHED;
		break;
	case CExtPaintManager::BOX_MOUSE_HOVER_INDETERMINATE:
		nStateForUxTheme = CBS_MIXEDHOT;
		nState = DFCS_BUTTON3STATE | DFCS_CHECKED;
		break;
	case CExtPaintManager::BOX_DISABLED_INDETERMINATE:
		nStateForUxTheme = CBS_MIXEDDISABLED;
		nState = DFCS_BUTTON3STATE | DFCS_CHECKED | DFCS_INACTIVE;
		break;
	} // switch( _pcrbd.m_eState ) 
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
bool bDefaultDrawing = true;
	if(		g_PaintManager.m_UxTheme.IsControlsThemed()
		&&	g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_BUTTON ) != NULL
		)
	{
		if( g_PaintManager.m_UxTheme.DrawBackground(
				pWnd->GetSafeHwnd(),
				dc.GetSafeHdc(), 
				BP_CHECKBOX, 
				nStateForUxTheme, 
				&_pcrbd.m_rcBox, 
				&_pcrbd.m_rcBox
				) == S_OK
			)
			bDefaultDrawing = false;
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
	if( bDefaultDrawing )
	{
		DWORD dwStyle = pWnd->GetStyle();
		if( dwStyle & BS_FLAT )
			nState |= DFCS_FLAT;
		dc.DrawFrameControl( &_pcrbd.m_rcBox, DFC_BUTTON, nState );
	} // if( bDefaultDrawing )
	dc.SetBkMode( nOldBkMode );
}

void CExtPaintManager::PaintRadioButtonBox(
	CDC & dc,
	CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcrbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pcrbd.m_rcClient.IsRectEmpty() )
		return;
	if( _pcrbd.m_pHelperSrc == NULL )
		return;

	ASSERT_VALID( _pcrbd.m_pHelperSrc );
CWnd * pWnd =
		DYNAMIC_DOWNCAST( CWnd, _pcrbd.m_pHelperSrc );
	ASSERT( pWnd != NULL );

INT nState = 0;
INT nStateForUxTheme = 0;

	switch( _pcrbd.m_eState ) 
	{
	case CExtPaintManager::BOX_UNCHECKED:
		nStateForUxTheme = RBS_UNCHECKEDNORMAL;
		break;
	case CExtPaintManager::BOX_LDOWN_UNCHECKED:
		nStateForUxTheme = RBS_UNCHECKEDPRESSED;
		nState = DFCS_PUSHED;
		break;
	case CExtPaintManager::BOX_DISABLED_UNCHECKED:
		nStateForUxTheme = RBS_UNCHECKEDDISABLED;
		nState = DFCS_INACTIVE;
		break;
	case CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED:
		nStateForUxTheme = RBS_UNCHECKEDHOT;
		break;

	case CExtPaintManager::BOX_CHECKED:
		nStateForUxTheme = RBS_CHECKEDNORMAL;
		nState = DFCS_CHECKED;
		break;
	case CExtPaintManager::BOX_LDOWN_CHECKED:
		nStateForUxTheme = RBS_CHECKEDPRESSED;
		nState = DFCS_CHECKED | DFCS_PUSHED;
		break;
	case CExtPaintManager::BOX_MOUSE_HOVER_CHECKED:
		nStateForUxTheme = RBS_CHECKEDHOT;
		nState = DFCS_CHECKED;
		break;
	case CExtPaintManager::BOX_DISABLED_CHECKED:
		nStateForUxTheme = RBS_CHECKEDDISABLED;
		nState = DFCS_CHECKED | DFCS_INACTIVE;
		break;
	} // switch( _pcrbd.m_eState ) 
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
bool bDefaultDrawing = true;
	if(		g_PaintManager.m_UxTheme.IsControlsThemed()
		&&	g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_BUTTON ) != NULL )
	{
		if( g_PaintManager.m_UxTheme.DrawBackground(
				pWnd->GetSafeHwnd(),
				dc.GetSafeHdc(), 
				BP_RADIOBUTTON, 
				nStateForUxTheme, 
				&_pcrbd.m_rcBox, 
				&_pcrbd.m_rcBox
				) == S_OK
			)
			bDefaultDrawing = false;
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
	if( bDefaultDrawing )
	{
		DWORD dwStyle = pWnd->GetStyle();
		if( dwStyle & BS_FLAT )
			nState |= DFCS_FLAT;
		dc.DrawFrameControl( &_pcrbd.m_rcBox, DFC_BUTTON, nState | DFCS_BUTTONRADIO );
	} // if( bDefaultDrawing )
	dc.SetBkMode( nOldBkMode );
}

void CExtPaintManager::PaintCheckOrRadioButtonText(
	CDC & dc,
	CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcrbd,
	CSize _sizeBox,
	CRect & rcBoxLocation
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pcrbd.m_rcClient.IsRectEmpty() )
		return;

	if( !_pcrbd.m_bEnabled )
	{
		_pcrbd.m_bHover = false;
		_pcrbd.m_bDrawFocusRect = false;
	}

CRect rcTextArea( _pcrbd.m_rcClient );
CRect rcTextLocation( 0, 0, 0, 0);

	rcBoxLocation.SetRect(
		_pcrbd.m_rcClient.left,
		_pcrbd.m_rcClient.top,
		_pcrbd.m_rcClient.left + _sizeBox.cx,
		_pcrbd.m_rcClient.top + _sizeBox.cy
		);

	if( (_pcrbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		rcBoxLocation.OffsetRect(
			0,
			( _pcrbd.m_rcClient.bottom - _sizeBox.cy ) - 2
			);
	else if( (_pcrbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_CENTER )
		rcBoxLocation.OffsetRect(
			0,
			( _pcrbd.m_rcClient.Height() - rcBoxLocation.Height() ) / 2
			);
	else
		rcBoxLocation.OffsetRect( 0, 1 );

	// pre-calculate box coordinates
	if( _pcrbd.m_bLeftText )
	{
		rcBoxLocation.OffsetRect(
			_pcrbd.m_rcClient.Width() - rcBoxLocation.Width(),
			0
			);
		rcTextArea.right = rcBoxLocation.left - 2;
		rcTextArea.left += 1;
	} // if( _pcrbd.m_bLeftText )
	else
		rcTextArea.left = rcBoxLocation.right + 4;

	rcTextArea.DeflateRect( _pcrbd.m_rcTextMargins );
	
	INT nOldBkMode = dc.SetBkMode( TRANSPARENT );

	// draw text
	INT nTextLength =
		(_pcrbd.m_sText == NULL)
			? INT(0)
			: INT(_tcslen(_pcrbd.m_sText));
	if(		nTextLength > 0
		&&	rcTextArea.bottom > rcTextArea.top 
		&&	rcTextArea.right > rcTextArea.left
		)
	{
		// if have valid area, text & focus rect
		ASSERT( _pcrbd.m_sText != NULL );

		COLORREF clrText =
			QueryObjectTextColor(
				dc,
				_pcrbd.m_bEnabled,
				_pcrbd.m_bDrawFocusRect,
				_pcrbd.m_bHover,
				_pcrbd.m_bPushed,
				_pcrbd.m_pHelperSrc
				);
		if( clrText == COLORREF(-1L) )
		{
			if( _pcrbd.m_bEnabled )
			{
				if( _pcrbd.m_bHover || _pcrbd.m_bPushed )
				{
					if(			_pcrbd.m_bPushed
							&&	_pcrbd.m_clrForceTextPressed != ((COLORREF)-1L)
							)
							clrText = _pcrbd.m_clrForceTextPressed;
					else if(	_pcrbd.m_bHover
							&&	_pcrbd.m_clrForceTextHover != ((COLORREF)-1L)
							)
							clrText = _pcrbd.m_clrForceTextHover;
					else if( _pcrbd.m_clrForceTextNormal != ((COLORREF)-1L) ) 
							clrText = _pcrbd.m_clrForceTextNormal;
					else
							clrText = GetColor( COLOR_BTNTEXT, this );
				} // if( _pcrbd.m_bHover || _pcrbd.m_bPushed )
				else
					clrText = 
					( _pcrbd.m_clrForceTextNormal == ((COLORREF)-1L) )
						? GetColor( COLOR_BTNTEXT, this )
						: _pcrbd.m_clrForceTextNormal;
			} // if( _pcrbd.m_bEnabled )
			else
			{
				clrText = 
					( _pcrbd.m_clrForceTextDisabled == ((COLORREF)-1L) )
						? GetColor( CLR_TEXT_DISABLED, this )
						: _pcrbd.m_clrForceTextDisabled;
			} // else from if( _pcrbd.m_bEnabled )
		} // if( clrText == COLORREF(-1L) )

		COLORREF clrOldText = dc.SetTextColor( clrText );

		CFont * pOldBtnFont = NULL;
		CFont * pCurrFont = NULL;
		if( _pcrbd.m_hFont != NULL )
			pCurrFont = CFont::FromHandle( _pcrbd.m_hFont );
		else
			pCurrFont = &m_FontNormal;
		ASSERT( pCurrFont != NULL );
		pOldBtnFont = dc.SelectObject( pCurrFont );
		
		UINT nDtMeasureFlags =
			DT_LEFT|DT_TOP|DT_CALCRECT;
		if( _pcrbd.m_bMultiline )
		{
			rcTextLocation = rcTextArea;
			rcTextLocation.OffsetRect( -rcTextLocation.TopLeft() );
			rcTextLocation.bottom = rcTextLocation.top;
			nDtMeasureFlags |= DT_WORDBREAK;
		}
		else
			nDtMeasureFlags |= DT_SINGLELINE;

		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			_pcrbd.m_sText,  nTextLength,
			rcTextLocation,
			nDtMeasureFlags, 0
			);
		rcTextLocation.OffsetRect(
			rcTextArea.TopLeft() - rcTextLocation.TopLeft()
			);
		
		UINT nDtDrawFlags = 0;
		if( (_pcrbd.m_nTextAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_RIGHT )
		{
			nDtDrawFlags |= DT_RIGHT;
			rcTextLocation.OffsetRect(
				rcTextArea.Width() - rcTextLocation.Width(),
				0
				);
		}
		else if( (_pcrbd.m_nTextAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
		{
			nDtDrawFlags |= DT_CENTER;
			rcTextLocation.OffsetRect(
				( rcTextArea.Width() - rcTextLocation.Width() ) / 2,
				0
				);
		}
		else
			nDtDrawFlags |= DT_LEFT;
		
		if( (_pcrbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		{
			nDtDrawFlags |= DT_BOTTOM;
			rcTextLocation.OffsetRect(
				0,
				rcTextArea.Height() - rcTextLocation.Height()
				);
		}
		else if( (_pcrbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_TOP )
		{
			nDtDrawFlags |= DT_TOP;
		}
		else
		{
			nDtDrawFlags |= DT_VCENTER;
			rcTextLocation.OffsetRect(
				0,
				( rcTextArea.Height() - rcTextLocation.Height() ) / 2
				);
		}

		if( _pcrbd.m_bMultiline )
			nDtDrawFlags |= DT_WORDBREAK;
		else
			nDtDrawFlags |= DT_SINGLELINE;

		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			_pcrbd.m_sText, nTextLength,
			rcTextLocation,
			nDtDrawFlags, 0
			);
		
		dc.SelectObject( pOldBtnFont );
		dc.SetTextColor( clrOldText );

		if( _pcrbd.m_bDrawFocusRect )
		{
			CRect rcFocus( rcTextLocation );
			rcFocus.InflateRect( 2, 2 );
			rcFocus.left = max( rcFocus.left, rcTextArea.left );
			rcFocus.top = max( rcFocus.top, rcTextArea.top );
			rcFocus.right = min( rcFocus.right, rcTextArea.right );
			rcFocus.bottom = min( rcFocus.bottom, rcTextArea.bottom );
			rcFocus.InflateRect(
				_pcrbd.m_bLeftText ? 0 : 1,
				0,
				_pcrbd.m_bLeftText ? 1 : 0,
				0
				);
			dc.DrawFocusRect( &rcFocus );
		} // if( _pcrbd.m_bDrawFocusRect )

	} // if have valid area, text & focus rect
	
	dc.SetBkMode( nOldBkMode );
}

void CExtPaintManager::PaintCheckButton(
	CDC & dc,
	CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcrbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pcrbd.m_rcClient.IsRectEmpty() )
		return;

	if( !_pcrbd.m_bEnabled )
	{
		_pcrbd.m_bHover = false;
		_pcrbd.m_bDrawFocusRect = false;
	}

	CSize szCheckBox = 
		GetCheckButtonBoxSize( dc, _pcrbd );

	// draw text
	CRect rcBoxLocation;
	PaintCheckOrRadioButtonText(
		dc,
		_pcrbd,
		szCheckBox,
		rcBoxLocation
		);

	// draw check box
	_pcrbd.m_rcBox = rcBoxLocation;
	PaintCheckButtonBox( dc, _pcrbd );
}

void CExtPaintManager::PaintRadioButton(
	CDC & dc,
	CExtPaintManager::PAINTCHECKRADIOBUTTONDATA & _pcrbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pcrbd.m_rcClient.IsRectEmpty() )
		return;

	if( !_pcrbd.m_bEnabled )
	{
		_pcrbd.m_bHover = false;
		_pcrbd.m_bDrawFocusRect = false;
	}

	CSize szRadioBox = 
		GetRadioButtonBoxSize( dc, _pcrbd );

	// draw text
	CRect rcBoxLocation;
	PaintCheckOrRadioButtonText(
		dc,
		_pcrbd,
		szRadioBox,
		rcBoxLocation
		);

	// draw radio box
	_pcrbd.m_rcBox = rcBoxLocation;
	PaintRadioButtonBox( dc, _pcrbd );
}

CSize CExtPaintManager::GetDropDividerMerics()
{
	ASSERT_VALID( this );
	return CSize( 6, 6 );
}

void CExtPaintManager::PaintDropDivider(
	CDC & dc,
	const RECT & rcItem,
	bool bBefore,
	bool bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rc( rcItem );
CSize _sizeDDM = GetDropDividerMerics();
COLORREF clr = GetColor( COLOR_BTNTEXT, this );
	if( bHorz )
	{
		rc.InflateRect( 0, 1 );
		if( bBefore )
		{
			rc.right = rc.left + 1;
			rc.left--;
		} // if( bBefore )
		else
		{
			rc.left = rc.right - 1;
			rc.right++;
		} // else from if( bBefore )
		dc.FillSolidRect( &rc, clr );
		CRect rc2( rc );
		rc2.bottom = rc2.top + 1;
		rc2.InflateRect( 2, 0 );
		dc.FillSolidRect( &rc2, clr );
		rc2.DeflateRect( 1, 0 );
		rc2.bottom++;
		dc.FillSolidRect( &rc2, clr );
		rc2 = rc;
		rc2.top = rc2.bottom - 1;
		rc2.InflateRect( 2, 0 );
		dc.FillSolidRect( &rc2, clr );
		rc2.DeflateRect( 1, 0 );
		rc2.top--;
		dc.FillSolidRect( &rc2, clr );
	} // if( bHorz )
	else
	{
		rc.InflateRect( 1, 0 );
		if( bBefore )
		{
			rc.bottom = rc.top + 1;
			rc.top--;
		} // if( bBefore )
		else
		{
			rc.top = rc.bottom - 1;
			rc.bottom++;
			rc.right--;
		} // else from if( bBefore )
		dc.FillSolidRect( &rc, clr );
		CRect rc2( rc );
		rc2.right = rc2.left + 1;
		rc2.InflateRect( 0, 2 );
		dc.FillSolidRect( &rc2, clr );
		rc2.DeflateRect( 0, 1 );
		rc2.right++;
		dc.FillSolidRect( &rc2, clr );
		rc2 = rc;
		rc2.left = rc2.right - 1;
		rc2.InflateRect( 0, 2 );
		dc.FillSolidRect( &rc2, clr );
		rc2.DeflateRect( 0, 1 );
		rc2.left--;
		dc.FillSolidRect( &rc2, clr );
	} // else from if( bHorz )
}

INT CExtPaintManager::GetDropDownButtonWidth(
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc; lParam;
#ifdef _DEBUG
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
	}
#endif // _DEBUG
INT nDD = 15; // 14;
	nDD = UiScalingDo( nDD, __EUIST_X );
	return nDD;
}

INT CExtPaintManagerNativeXP::GetDropDownButtonWidth(
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
	}
#endif // _DEBUG
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return CExtPaintManager::GetDropDownButtonWidth( pHelperSrc, lParam );
INT nDD = ::GetSystemMetrics( SM_CXHTHUMB );
	nDD = UiScalingDo( nDD, __EUIST_X );
	return nDD;
}

INT CExtPaintManager::GetSpinButtonWidth(
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
#ifdef _DEBUG
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
	}
#endif // _DEBUG
INT nDD = 15;
	nDD = UiScalingDo( nDD, __EUIST_X );
	return nDD;
}

INT CExtPaintManagerNativeXP::GetSpinButtonWidth(
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
	}
#endif // _DEBUG

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return CExtPaintManager::GetSpinButtonWidth( pHelperSrc, lParam );
INT nDD = ::GetSystemMetrics( SM_CXHTHUMB );
	nDD = UiScalingDo( nDD, __EUIST_X );
	return nDD;
}

CRect CExtPaintManager::GetSpinButtonMargins(
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
#ifdef _DEBUG
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
	}
#endif // _DEBUG
	return CRect( 0, 3, 3, 3 );
}

CRect CExtPaintManagerNativeXP::GetSpinButtonMargins(
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
#ifdef _DEBUG
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
	}
#endif // _DEBUG
	return CRect( 0, 1, 1, 1 );
}

void CExtPaintManager::PaintDragSourceRect(
	CDC & dc,
	const RECT & rcItem
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( !dc.RectVisible(&rcItem) )
		return;
COLORREF clr = GetColor( COLOR_BTNTEXT, this );
CRect rc( rcItem );
	dc.Draw3dRect( &rc, clr, clr );
	rc.DeflateRect( 1, 1 );
	dc.Draw3dRect( &rc, clr, clr );
}

void CExtPaintManager::PaintMenuSeparator(
	CDC & dc,
	const RECT & rectItem,
	int nIconAreaWidth,
	bool bRarelyUsed,
	bool bIsForceNoLeftGradient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	nIconAreaWidth;
	bIsForceNoLeftGradient;
	if( ::IsRectEmpty(&rectItem) )
		return;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtRibbonGalleryPopupMenuWnd ) )
		&&	( ((CExtPopupMenuWnd*)pHelperSrc)->TrackFlagsGet() & TPMX_RIBBON_FILE_MENU ) == 0
		&&	((CExtRibbonGalleryPopupMenuWnd*)pHelperSrc)->ItemGetCount() > 0
		)
	{
		CRect rc;
		((CExtRibbonGalleryPopupMenuWnd*)pHelperSrc)->_GetClientRect( &rc );
		rc.top = rectItem.top; // + ( rectItem.bottom - rectItem.top ) / 2;
		rc.bottom = rc.top + 2;
		rc.DeflateRect( 3, 0 );
		COLORREF clrBtnShadow = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
		COLORREF clrBtnHilight = GetColor( COLOR_3DHILIGHT, pHelperSrc, lParam );
		dc.Draw3dRect(
			rc,
			clrBtnShadow,
			clrBtnHilight
			);
		return;
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
CRect rectItem2( rectItem );
	if( bRarelyUsed )
		dc.FillRect(
			&rectItem2,
			&m_brushLightDefault
			);
	else
		dc.FillSolidRect(
			&rectItem2,
			GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam )
			);
int nFunnyDeflateMetric =
		::GetSystemMetrics(SM_CXSMICON) / 2;
	rectItem2.DeflateRect(
		nFunnyDeflateMetric,
		0
		);
COLORREF clrBtnShadow =
		GetColor(
			bRarelyUsed ?
				CLR_3DFACE_OUT : CLR_3DSHADOW_OUT,
			pHelperSrc,
			lParam
			);
COLORREF clrBtnHilight =
		GetColor( CLR_3DHILIGHT_OUT, pHelperSrc, lParam );
const int nSeparatorHeight = 2;
	rectItem2.top += ( rectItem2.Height() - nSeparatorHeight ) / 2;
	rectItem2.bottom = rectItem2.top + nSeparatorHeight;
	dc.Draw3dRect(
		rectItem2,
		clrBtnShadow,
		clrBtnHilight
		);
}

void CExtPaintManagerXP::PaintMenuSeparator(
	CDC & dc,
	const RECT & rectItem,
	int nIconAreaWidth,
	bool bRarelyUsed,
	bool bIsForceNoLeftGradient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty(&rectItem) )
		return;
bool bRTL = false;
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
		CExtPopupMenuWnd * pPopup = DYNAMIC_DOWNCAST( CExtPopupMenuWnd, pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	} // if( pHelperSrc != NULL )
CRect rc( rectItem ); 
	if( ! bIsForceNoLeftGradient )
	{
		CRect rcBottomFill( rectItem );
		if( bRTL )
			rcBottomFill.left = rcBottomFill.right - nIconAreaWidth;
		else
			rcBottomFill.right = rcBottomFill.left + nIconAreaWidth;
		COLORREF clr3dFace =
			GetColor(
				bRarelyUsed ? XPCLR_RARELY_BORDER : CLR_3DFACE_OUT,
				pHelperSrc,
				lParam 
			);
		dc.FillSolidRect( rcBottomFill, clr3dFace );
	} // if( ! bIsForceNoLeftGradient )

CRect rcGradient( rectItem );
	if( bRTL )
		rcGradient.left = rcGradient.right - nIconAreaWidth;
	else
		rcGradient.right = rcGradient.left + nIconAreaWidth;
	if( ! bIsForceNoLeftGradient )
	{
		COLORREF clrGradientLeft = ((COLORREF)(-1L)), 
				 clrGradientMiddle((COLORREF)(-1L)),
				 clrGradientRight((COLORREF)(-1L));
		if( bRarelyUsed )
		{
			if(		m_nIdxClrMlaRarelyLeft >= 0
				&&	m_nIdxClrMlaRarelyMiddle >= 0
				&&	m_nIdxClrMlaRarelyRight >= 0
				&&	::GetDeviceCaps( dc.m_hDC, BITSPIXEL ) > 8
				)
			{ // if paint gradient on rarely used item's left part
				clrGradientLeft = GetColor( m_nIdxClrMlaRarelyLeft, pHelperSrc, lParam );
				clrGradientMiddle = GetColor( m_nIdxClrMlaRarelyMiddle, pHelperSrc, lParam );
				clrGradientRight = GetColor( m_nIdxClrMlaRarelyRight, pHelperSrc, lParam );
			} // if paint gradient on rarely used item's left part
		} // if( bRarelyUsed )
		else
		{
			if(		m_nIdxClrMlaNormLeft >= 0
				&&	m_nIdxClrMlaNormMiddle >= 0
				&&	m_nIdxClrMlaNormRight >= 0
				&&	::GetDeviceCaps( dc.m_hDC, BITSPIXEL ) > 8
				)
			{ // if paint gradient on rarely used item's left part
				clrGradientLeft = GetColor( m_nIdxClrMlaNormLeft, pHelperSrc, lParam );
				clrGradientMiddle = GetColor( m_nIdxClrMlaNormMiddle, pHelperSrc, lParam );
				clrGradientRight = GetColor( m_nIdxClrMlaNormRight, pHelperSrc, lParam );
			} // if paint gradient on rarely used item's left part
		} // else from if( bRarelyUsed )
		if( clrGradientLeft != ((COLORREF)(-1L)) )
		{
			ASSERT( clrGradientRight != ((COLORREF)(-1L)) );
			stat_PaintGradientRect2steps( 
				dc,
				rcGradient, 
				clrGradientLeft,
				clrGradientMiddle, 
				clrGradientRight,
				false,
				1,
				2
				);
		} // if( clrGradientLeft != ((COLORREF)(-1L)) )
	} // if( ! bIsForceNoLeftGradient )

	if( bRTL )
		rc.right -= nIconAreaWidth;
	else
		rc.left += nIconAreaWidth;
	dc.FillSolidRect( rc, GetColor( XPCLR_3DFACE_NORMAL, pHelperSrc, lParam ) );

PAINTMENUITEMDATA _pmid; // fake
	_pmid.m_bForceNoLeftGradient = bIsForceNoLeftGradient;
	PaintMenuItemLeftAreaMarginXP(
		dc,
		_pmid,
		rcGradient,
		rc
		);

	if( bRTL )
		rc.right -= __EXT_MENU_GAP;
	else
		rc.left += __EXT_MENU_GAP;
	rc.top += rc.Height()/2 - 1;
	rc.bottom = rc.top + 1;

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtRibbonGalleryPopupMenuWnd ) )
		&&	( ((CExtPopupMenuWnd*)pHelperSrc)->TrackFlagsGet() & TPMX_RIBBON_FILE_MENU ) == 0
		&&	((CExtRibbonGalleryPopupMenuWnd*)pHelperSrc)->ItemGetCount() > 0
		)
	{
		CRect rcC;
		((CExtRibbonGalleryPopupMenuWnd*)pHelperSrc)->_GetClientRect( &rcC );
		rc.left = rcC.left;
		rc.right = rcC.right;
		rc.DeflateRect( 1, 0 );
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	dc.FillSolidRect(
		rc,
		GetColor( XPCLR_SEPARATOR, pHelperSrc, lParam )
		);
}

bool CExtPaintManagerNativeXP::PaintMenuSeparator_UX(
	HWND hWnd,
	CDC & dc,
	const RECT & rectItem,
	int nIconAreaWidth,
	bool bRarelyUsed,
	bool bIsForceNoLeftGradient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bRarelyUsed;
	bIsForceNoLeftGradient;
	pHelperSrc;
	lParam;
	if( ::IsRectEmpty(&rectItem) )
		return true;
	if( hWnd == NULL || ( ! ::IsWindow( hWnd ) ) )
		return false;
	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_MENU ) == NULL )
		return false;
bool bRTL = false;
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
		CExtPopupMenuWnd * pPopup = DYNAMIC_DOWNCAST( CExtPopupMenuWnd, pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}
HRESULT hr0 = S_OK, hr1 = S_OK, hr2 = S_OK;
CRect rcBk( rectItem );
	rcBk.DeflateRect( 1, 0, 1, 0 );
CRect rcFillSelectedBk = rcBk;
	if( nIconAreaWidth > 0 )
	{
		CRect rcLeftArea = rcFillSelectedBk;
		if( bRTL )
			rcFillSelectedBk.right = rcLeftArea.left = rcLeftArea.right - nIconAreaWidth - 1;
		else
			rcFillSelectedBk.left = rcLeftArea.right = rcLeftArea.left + nIconAreaWidth + 1;
		rcLeftArea.InflateRect( 0, 1 );
		hr0 =
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd, 
				dc.GetSafeHdc(), 
				MENU_POPUPGUTTER,
				MPI_NORMAL, 
				&rcLeftArea, 
				&rcLeftArea
				);
	}
	hr1 =
		g_PaintManager.m_UxTheme.DrawBackground(
			hWnd, 
			dc.GetSafeHdc(), 
			MENU_POPUPITEM,
			MPI_NORMAL, 
			&rcFillSelectedBk, 
			&rcFillSelectedBk
			);
CRect rcSeparator( rectItem );
//	rcSeparator.top += ( rectItem.bottom - rectItem.top ) / 2;
//	rcSeparator.bottom = rcSeparator.top + 1;
	rcSeparator.DeflateRect( 3, 1 );
	if( bRTL )
		rcSeparator.right -= nIconAreaWidth;
	else
		rcSeparator.left += nIconAreaWidth;
	hr2 =
		g_PaintManager.m_UxTheme.DrawBackground(
			hWnd, 
			dc.GetSafeHdc(), 
			MENU_POPUPSEPARATOR, 
			0, 
			&rcSeparator,
			&rcSeparator
			);
bool bThemedApproachOK = ( hr0 == S_OK && hr1 == S_OK && hr2 == S_OK ) ? true : false;
	g_PaintManager.m_UxTheme.CloseThemeData();
	return bThemedApproachOK;
}

void CExtPaintManagerNativeXP::PaintMenuSeparator(
	CDC & dc,
	const RECT & rectItem,
	int nIconAreaWidth,
	bool bRarelyUsed,
	bool bIsForceNoLeftGradient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty(&rectItem) )
		return;

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintMenuSeparator(
			dc,
			rectItem,
			nIconAreaWidth,
			bRarelyUsed,
			bIsForceNoLeftGradient,
			pHelperSrc,
			lParam
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )

HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}

	if( PaintMenuSeparator_UX( hWnd, dc, rectItem, nIconAreaWidth, bRarelyUsed, bIsForceNoLeftGradient, pHelperSrc, lParam ) )
		return;

CRect rcSeparator( rectItem );
	rcSeparator.top += ( rectItem.bottom - rectItem.top ) / 2;
	rcSeparator.bottom = rcSeparator.top + 1;
	rcSeparator.DeflateRect( 3, 0 );

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtRibbonGalleryPopupMenuWnd ) )
		&&	( ((CExtPopupMenuWnd*)pHelperSrc)->TrackFlagsGet() & TPMX_RIBBON_FILE_MENU ) == 0
		&&	((CExtRibbonGalleryPopupMenuWnd*)pHelperSrc)->ItemGetCount() > 0
		)
	{
		CRect rcC;
		((CExtRibbonGalleryPopupMenuWnd*)pHelperSrc)->_GetClientRect( &rcC );
		rcSeparator.left = rcC.left;
		rcSeparator.right = rcC.right;
		rcSeparator.DeflateRect( 1, 0 );
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	dc.FillSolidRect(
		&rcSeparator,
		::GetSysColor( COLOR_3DSHADOW )
		);
}

void CExtPaintManager::PaintSysCmdGlyph(
	CDC & dc,
	UINT nHelperSysCmdID,
	const RECT & rcItem,
	bool bHover,
	bool bPushed,
	bool bEnabled,
	COLORREF clrForceGlyphColor // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
e_docking_caption_button_t eGlyph = __DCBT_EMPTY;
	switch( nHelperSysCmdID )
	{
	case SC_CLOSE:
		eGlyph = __DCBT_CLOSE;
	break;
	case SC_RESTORE:
		eGlyph = __DCBT_WND_RESTORE;
	break;
	case SC_MINIMIZE:
		eGlyph = __DCBT_WND_MINIMIZE;
	break;
	case SC_MAXIMIZE:
		eGlyph = __DCBT_WND_MAXIMIZE;
	break;
	case SC_CONTEXTHELP:
		eGlyph = __DCBT_WND_CONTEXTHELP;
	break;
	} // switch( nHelperSysCmdID )
	if( eGlyph == __DCBT_EMPTY )
		return;
COLORREF ColorValues[2] = { RGB(0,0,0), RGB(0,0,0), };
CRect _rcItem( rcItem );
	if( bPushed )
		_rcItem.OffsetRect( GetPushedOffset() );
	if( clrForceGlyphColor != COLORREF(-1L) )
		ColorValues[1] = clrForceGlyphColor;
	else if( bEnabled )
		ColorValues[1] = GetColor( bHover ? CLR_MENUTEXT_IN : CLR_MENUTEXT_OUT, this );
	else
	{
		ColorValues[1] = GetColor( CLR_3DHILIGHT_OUT, this );
		_rcItem.OffsetRect(1,1);
		PaintGlyphCentered( dc, _rcItem, *g_DockingCaptionGlyphs[eGlyph], ColorValues );
		_rcItem.OffsetRect( -1, -1 );
		ColorValues[1] = GetColor( CLR_3DSHADOW_OUT, this );
	} // if( bEnabled )
	PaintGlyphCentered( dc, _rcItem, *g_DockingCaptionGlyphs[eGlyph], ColorValues );
}

void CExtPaintManagerXP::PaintSysCmdGlyph(
	CDC & dc,
	UINT nHelperSysCmdID,
	const RECT & rcItem,
	bool bHover,
	bool bPushed,
	bool bEnabled,
	COLORREF clrForceGlyphColor // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
e_docking_caption_button_t eGlyph = __DCBT_EMPTY;
	switch( nHelperSysCmdID )
	{
	case SC_CLOSE:
		eGlyph = __DCBT_CLOSE;
	break;
	case SC_RESTORE:
		eGlyph = __DCBT_WND_RESTORE;
	break;
	case SC_MINIMIZE:
		eGlyph = __DCBT_WND_MINIMIZE;
	break;
	case SC_MAXIMIZE:
		eGlyph = __DCBT_WND_MAXIMIZE;
	break;
	case SC_CONTEXTHELP:
		eGlyph = __DCBT_WND_CONTEXTHELP;
	break;
	} // switch( nHelperSysCmdID )
	if( eGlyph == __DCBT_EMPTY )
		return;
COLORREF ColorValues[2] = { RGB(0,0,0), RGB(0,0,0), };
CRect _rcItem( rcItem );
	if( bPushed )
		_rcItem.OffsetRect( GetPushedOffset() );
	if( clrForceGlyphColor != COLORREF(-1L) )
		ColorValues[1] = clrForceGlyphColor;
	else if( bEnabled )
	{
		if( m_bEnabledHoverIconShadows && bHover && (!bPushed) )
		{
			// COLOR_3DDKSHADOW changed to COLOR_3DSHADOW
			// by the Dmitry Yakovlev's advice
			ColorValues[1] = GetColor( COLOR_3DSHADOW, this  );
			PaintGlyphCentered( dc, _rcItem, *g_DockingCaptionGlyphs[eGlyph], ColorValues );
			// corrected by the Dmitry Yakovlev's advice
			_rcItem.OffsetRect( -2, -2 );
		} // if( m_bEnabledHoverIconShadows && bHover && (!bPushed) )
		ColorValues[1] = GetColor( bHover ? CLR_MENUTEXT_IN : CLR_MENUTEXT_OUT, this );
	} // if( bEnabled )
	else
		ColorValues[1] = GetColor( CLR_MENUTEXT_DISABLED, this );
	PaintGlyphCentered( dc, _rcItem, *g_DockingCaptionGlyphs[eGlyph], ColorValues );
}

int CExtPaintManager::TearOff_GetCaptionHeight() const
{
	ASSERT_VALID( this );
	return 8;
}

void CExtPaintManager::TearOff_GetCaptionPaint(
	CDC & dc,
	const RECT & rectItem,
	bool bHover,
	bool bPressed,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	dc;
	rectItem;
	bHover;
	bPressed;
	pHelperSrc;
	lParam;
}

CExtPaintManager::PAINTMENUITEMDATA::PAINTMENUITEMDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_rcItem( 0, 0, 0, 0 )
	, m_rcItemWithIndents( 0, 0, 0, 0 )
	, m_nIconAreaWidth( 0 )
	, m_sText( _T("") )
	, m_sAccelText( _T("") )
	, m_sExtendedText( _T("") )
	, m_pIcon( NULL )
	, m_bPopup( false )
	, m_bSelected( false )
	, m_bPushedCheck( false )
	, m_bPushedRadio( false )
	, m_bIndeterminate( false )
	, m_bEnabled( false )
	, m_bRarelyUsed( false )
	, m_bRarelyUsedPrev( false )
	, m_bRarelyUsedNext( false )
	, m_nHelperSysCmdID( 0L )
	, m_bForceNoLeftGradient( false )
	, m_bCustomizeListMode( false )
	, m_bSeparatedDropDown( false )
	, m_rcInPlaceWnd( 0, 0, 0, 0 )
	, m_strInPlaceText( _T("") )
	, m_bXtraMark( false )
	, m_bXtraChecked( false )
	, m_bBigAccent( false )
	, m_bHelperPopupDropped( false )
	, m_hCustomFont( NULL )
	, m_clrCustomAccentEffectForIcon( COLORREF(-1L) )
	, m_clrForceMenuText( COLORREF(-1L) )
{
}

CExtPaintManager::PAINTMENUITEMDATA::PAINTMENUITEMDATA(
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
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_rcItem( rcItem )
	, m_rcItemWithIndents( rcItemWithIndents )
	, m_nIconAreaWidth( nIconAreaWidth )
	, m_sText( (sText == NULL) ? _T("") : sText )
	, m_sAccelText( (sAccelText == NULL) ? _T("") : sAccelText )
	, m_sExtendedText( _T("") )
	, m_pIcon( pIcon )
	, m_bPopup( bPopup )
	, m_bSelected( bSelected )
	, m_bPushedCheck( bPushedCheck )
	, m_bPushedRadio( bPushedRadio )
	, m_bIndeterminate( bIndeterminate )
	, m_bEnabled( bEnabled )
	, m_bRarelyUsed( bRarelyUsed )
	, m_bRarelyUsedPrev( bRarelyUsedPrev )
	, m_bRarelyUsedNext( bRarelyUsedNext )
	, m_nHelperSysCmdID( nHelperSysCmdID )
	, m_bForceNoLeftGradient( false )
	, m_bCustomizeListMode( false )
	, m_bSeparatedDropDown( false )
	, m_rcInPlaceWnd( 0, 0, 0, 0 )
	, m_strInPlaceText( _T("") )
	, m_bXtraMark( false )
	, m_bXtraChecked( false )
	, m_bBigAccent( false )
	, m_bHelperPopupDropped( false )
	, m_hCustomFont( hCustomFont )
	, m_clrCustomAccentEffectForIcon( COLORREF(-1L) )
	, m_clrForceMenuText( COLORREF(-1L) )
{
}

void CExtPaintManager::PaintMenuItem(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pmid.m_rcItem.IsRectEmpty() )
		return;
bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		CExtPopupMenuWnd * pPopup =
			DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	} // if( _pmid.m_pHelperSrc != NULL )
CExtSafeString _sText =
		(_pmid.m_sText==NULL) ? _T("") : _pmid.m_sText;
CExtSafeString _sAccelText =
		(_pmid.m_sAccelText==NULL) ? _T("") : _pmid.m_sAccelText;

COLORREF clr3dFace = GetColor( CLR_3DFACE_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
COLORREF clrBk = clr3dFace;
COLORREF clrText = 
		GetColor( 
			_pmid.m_bEnabled 
				? CLR_MENUTEXT_OUT 
				: CLR_MENUTEXT_DISABLED, 
			_pmid.m_pHelperSrc, 
			_pmid.m_lParam 
			);
CRect rcBk( _pmid.m_rcItem );
CRect rcBkWithIndents( _pmid.m_rcItemWithIndents );
	dc.FillSolidRect( rcBkWithIndents, clrBk );

	if( _pmid.m_bSelected )
	{
		ASSERT( _pmid.m_bEnabled );
		clrBk = GetColor( COLOR_HIGHLIGHT, _pmid.m_pHelperSrc, _pmid.m_lParam );
		clrText = GetColor( CLR_MENUTEXT_IN, _pmid.m_pHelperSrc, _pmid.m_lParam  );
	}

	if( _pmid.m_bRarelyUsed )
	{
		dc.FillRect( rcBkWithIndents, &m_brushLightDefault );
		if( ! _pmid.m_bRarelyUsedPrev )
		{
			CPen penShadow(
				PS_SOLID,
				1,
				GetColor( COLOR_3DSHADOW, _pmid.m_pHelperSrc, _pmid.m_lParam )
				);
			CPen * pOldPen =
				dc.SelectObject( &penShadow );
			ASSERT( pOldPen != NULL );
			dc.MoveTo( rcBkWithIndents.left, rcBkWithIndents.top );
			dc.LineTo( rcBkWithIndents.right, rcBkWithIndents.top );
			dc.SelectObject( pOldPen );
		}
		if( ! _pmid.m_bRarelyUsedNext )
		{
			CPen penLight(
				PS_SOLID,
				1,
				GetColor( COLOR_3DHIGHLIGHT, _pmid.m_pHelperSrc, _pmid.m_lParam )
				);
			CPen * pOldPen =
				dc.SelectObject( &penLight );
			ASSERT( pOldPen != NULL );
			dc.MoveTo( rcBkWithIndents.left, rcBkWithIndents.bottom - 1 );
			dc.LineTo( rcBkWithIndents.right, rcBkWithIndents.bottom - 1 );
			dc.SelectObject( pOldPen );
		}
	}

bool bSmallerSelRect = 
		(		(_pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()))
			||	_pmid.m_bPushedCheck
			||	_pmid.m_bPushedRadio
			) ? true : false;
	if( bSmallerSelRect )
	{
		if( bRTL )
			rcBk.right -= _pmid.m_nIconAreaWidth;
		else
			rcBk.left += _pmid.m_nIconAreaWidth;
	}
	rcBk.DeflateRect( 1, 0, 0, 0 );
	if( _pmid.m_bSelected || (!_pmid.m_bRarelyUsed) )
		dc.FillSolidRect( rcBk, clrBk );
	if( ! bSmallerSelRect )
	{
		if( bRTL )
			rcBk.right -= _pmid.m_nIconAreaWidth;
		else
			rcBk.left += _pmid.m_nIconAreaWidth;
	}
	rcBk.DeflateRect( __EXT_MENU_GAP, __EXT_MENU_GAP );

COLORREF clrOldText = dc.SetTextColor( clrText );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );

	rcBk.DeflateRect( __EXT_MENU_GAP, 0 );
CFont * pRenderFont =
		( _pmid.m_hCustomFont == NULL )
			? (&m_FontNormal)
			: CFont::FromHandle(_pmid.m_hCustomFont)
			;
CFont * pOldFont = dc.SelectObject( pRenderFont );
	ASSERT( pOldFont != NULL );

DWORD dwDtAlignFlags = bRTL ? (DT_RIGHT|DT_RTLREADING) : DT_LEFT;
CRect rcMeasureText( 0, 0, 0, 0 );
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		LPCTSTR(_sText), _sText.GetLength(),
		&rcMeasureText,
		DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags, 0
		);
CRect rcDrawText( rcBk ), rcDrawTextExtended( rcBk );
INT nLenTextExtended = ( _pmid.m_sExtendedText != NULL ) ? INT( _tcslen( _pmid.m_sExtendedText ) ) : 0;
	if( nLenTextExtended > 0 )
	{
		rcDrawText.bottom
			= rcDrawTextExtended.top
			= rcDrawText.top + rcMeasureText.Height();
	}
	else
	{
		rcDrawText.OffsetRect(
			0,
			( rcDrawText.Height() - rcMeasureText.Height() ) / 2
			);
	}

COLORREF clrTextRestore = dc.GetTextColor();
	if( _pmid.m_clrForceMenuText != COLORREF(-1L) )
		dc.SetTextColor( _pmid.m_clrForceMenuText );
	
	if( bRTL )
	{
		rcDrawText.left = rcDrawText.right - rcMeasureText.Width();
		if( _pmid.m_bEnabled )
		{
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(_sText), _sText.GetLength(),
				&rcDrawText,
				DT_SINGLELINE|dwDtAlignFlags, 0
				);
			if( nLenTextExtended > 0 )
			{
				CFont * pOldFont = dc.SelectObject( &m_FontNormal );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					_pmid.m_sExtendedText, nLenTextExtended,
					&rcDrawTextExtended,
					dwDtAlignFlags, 0
					);
				dc.SelectObject( pOldFont );
			}
		}
		else
		{
			CRect rcDrawTextOffs = rcDrawText;
			rcDrawTextOffs.OffsetRect( 1, 1 );
			COLORREF clrTextRestore = COLORREF(-1L);
			if( _pmid.m_clrForceMenuText == COLORREF(-1L) )
				clrTextRestore = 
					dc.SetTextColor(
						GetColor(
							COLOR_3DHILIGHT,
							_pmid.m_pHelperSrc,
							_pmid.m_lParam
							)
						);
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(_sText), _sText.GetLength(),
				&rcDrawTextOffs,
				DT_SINGLELINE|dwDtAlignFlags, 0
				);
			dc.SetTextColor(
				GetColor(
					CLR_MENUTEXT_DISABLED,
					_pmid.m_pHelperSrc,
					_pmid.m_lParam
					)
				);
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(_sText), _sText.GetLength(),
				&rcDrawText,
				DT_SINGLELINE|dwDtAlignFlags, 0
				);
			if( nLenTextExtended > 0 )
			{
				CFont * pOldFont = dc.SelectObject( &m_FontNormal );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					_pmid.m_sExtendedText, nLenTextExtended,
					&rcDrawTextExtended,
					dwDtAlignFlags, 0
					);
				dc.SelectObject( pOldFont );
			}
			if( clrTextRestore != COLORREF(-1L) )
				dc.SetTextColor( clrTextRestore );
		}
	}
	else
	{
		if( _pmid.m_clrForceMenuText == COLORREF(-1L) )
		{
			CExtRichContentLayout::DRAWSTATE_data_t _DSD;
			_DSD.m_fuFlags |= ( _pmid.m_bEnabled ? DSS_NORMAL : m_DSS_DISABLED_style );
			CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_sText), rcDrawText, 0, 0, NULL, NULL, 0, 0, 0, NULL, &_DSD );
			if( nLenTextExtended > 0 )
			{
				CFont * pOldFont = dc.SelectObject( &m_FontNormal );
				COLORREF clrOldTextColor = COLORREF(-1L);
				if( ! _pmid.m_bEnabled )
					clrOldTextColor = dc.SetTextColor( GetColor(COLOR_3DSHADOW) );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					_pmid.m_sExtendedText, nLenTextExtended,
					&rcDrawTextExtended,
					dwDtAlignFlags, 0
					);
				if( ! _pmid.m_bEnabled )
					dc.SetTextColor( clrOldTextColor );
				dc.SelectObject( pOldFont );
			}
		}
		else
		{
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(_sText), _sText.GetLength(),
				&rcDrawText,
				DT_SINGLELINE|dwDtAlignFlags, 0
				);
			if( nLenTextExtended > 0 )
			{
				CFont * pOldFont = dc.SelectObject( &m_FontNormal );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					_pmid.m_sExtendedText, nLenTextExtended,
					&rcDrawTextExtended,
					dwDtAlignFlags, 0
					);
				dc.SelectObject( pOldFont );
			}
		}
	}

	if( ! _sAccelText.IsEmpty() )
	{
		rcMeasureText.SetRect( 0, 0, 0, 0 );
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(_sAccelText), _sAccelText.GetLength(),
			&rcMeasureText,
			DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags, 0
			);
		rcDrawText = rcBk;
		rcDrawText.OffsetRect(
			0,
			( rcDrawText.Height() - rcMeasureText.Height() ) / 2
			);

		if( bRTL )
		{
			rcDrawText.right =
				rcDrawText.left
				+ rcMeasureText.Width()
				+ __EXT_MENU_POPUP_ARROW_AREA_DX
				;
			rcBk.right = rcDrawText.right;
		}
		else
		{
			rcDrawText.left =
				rcDrawText.right
				- rcMeasureText.Width()
				- __EXT_MENU_POPUP_ARROW_AREA_DX
				;
			rcBk.left = rcDrawText.left;
		}

		if( bRTL )
		{
			rcDrawText.left = rcDrawText.right - rcMeasureText.Width();
			if( _pmid.m_bEnabled )
			{
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(_sAccelText), _sAccelText.GetLength(),
					&rcDrawText,
					DT_SINGLELINE|dwDtAlignFlags, 0
					);
			}
			else
			{
				CRect rcDrawTextOffs = rcDrawText;
				rcDrawTextOffs.OffsetRect( 1, 1 );
				COLORREF clrTextRestore = COLORREF(-1L);
				if( _pmid.m_clrForceMenuText == COLORREF(-1L) )
					clrTextRestore = 
						dc.SetTextColor(
							GetColor(
								COLOR_3DHILIGHT,
								_pmid.m_pHelperSrc,
								_pmid.m_lParam
								)
							);
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(_sAccelText), _sAccelText.GetLength(),
					&rcDrawTextOffs,
					DT_SINGLELINE|dwDtAlignFlags, 0
					);
				dc.SetTextColor(
					GetColor(
						CLR_MENUTEXT_DISABLED,
						_pmid.m_pHelperSrc,
						_pmid.m_lParam
						)
					);
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(_sAccelText), _sAccelText.GetLength(),
					&rcDrawText,
					DT_SINGLELINE|dwDtAlignFlags, 0
					);
				if( clrTextRestore != COLORREF(-1L) )
					dc.SetTextColor( clrTextRestore );
			} // else from if( _pmid.m_bEnabled )
		} // if( bRTL )
		else
		{
			if( _pmid.m_clrForceMenuText == COLORREF(-1L) )
			{
				CExtRichContentLayout::DRAWSTATE_data_t _DSD;
				_DSD.m_fuFlags |= ( _pmid.m_bEnabled ? DSS_NORMAL : m_DSS_DISABLED_style );
				CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_sAccelText), rcDrawText, 0, 0, NULL, NULL, 0, 0, 0, NULL, &_DSD );
			}
			else
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(_sAccelText), _sAccelText.GetLength(),
					&rcDrawText,
					DT_SINGLELINE|dwDtAlignFlags, 0
					);
		}
	} // if( ! _sAccelText.IsEmpty() )

	dc.SetTextColor( clrTextRestore );

	if( _pmid.m_bPopup )
	{
		CRect rcPopupArrow( _pmid.m_rcItem );
		rcPopupArrow.DeflateRect( __EXT_MENU_GAP, __EXT_MENU_GAP );
		if( bRTL )
			rcPopupArrow.right =
				rcPopupArrow.left + __EXT_MENU_POPUP_ARROW_AREA_DX;
		else
			rcPopupArrow.left =
				rcPopupArrow.right - __EXT_MENU_POPUP_ARROW_AREA_DX;
		if( _pmid.m_bCustomizeListMode )
		{
			COLORREF ColorValues[2] =
			{
				RGB(0,0,0),
				GetColor(
					_pmid.m_bSelected 
						? CLR_MENUTEXT_IN 
						: CLR_MENUTEXT_OUT,
					_pmid.m_pHelperSrc,
					_pmid.m_lParam 
					)
			};
			PaintGlyphCentered(
				dc,
				rcPopupArrow,
				_pmid.m_bSeparatedDropDown
					? g_glyph_customize_dropdown_src
					: g_glyph_customize_popup_src,
				ColorValues
				);
		} // if( _pmid.m_bCustomizeListMode )
		else
		{
			COLORREF clrPopupArrow = clrText;
			if( _pmid.m_bSeparatedDropDown )
			{
				if( _pmid.m_bSelected )
				{
					if( _pmid.m_bHelperPopupDropped )
						clrPopupArrow = GetColor( CLR_3DHILIGHT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
					else
					{
						clrPopupArrow = GetColor( CLR_MENUTEXT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
						CRect rcButton(
							bRTL ? _pmid.m_rcItem.left : rcPopupArrow.left,
							_pmid.m_rcItem.top,
							bRTL ? rcPopupArrow.right : _pmid.m_rcItem.right,
							_pmid.m_rcItem.bottom
							);
						if( _pmid.m_bRarelyUsed )
							dc.FillRect(
								&rcButton,
								&m_brushLightDefault
								);	
						else
							dc.FillSolidRect(
								&rcButton,
								GetColor( CLR_3DFACE_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam )
								);	
					}
				} // if( _pmid.m_bSelected )
				else
				{
					CRect rcSeparator(
						bRTL ? rcPopupArrow.right : rcPopupArrow.left,
						rcBk.top,
						bRTL ? rcPopupArrow.right : rcPopupArrow.left,
						rcBk.bottom
						);
					PaintSeparator(
						dc,
						rcSeparator,
						true,
						true,
						NULL
						);
				} // else from if( _pmid.m_bSelected )
			} // if( _pmid.m_bSeparatedDropDown )

			dc.SelectObject( &m_FontMarlett );
			CRect _rcPopupArrow( rcPopupArrow );
			if( bRTL )
				_rcPopupArrow.right -= 2;
			else
				_rcPopupArrow.left += 2;
			dc.SetTextColor( clrPopupArrow );
			LPCTSTR strDrawArrow = bRTL ? _T("3") : _T("4");
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				strDrawArrow,
				_rcPopupArrow,
				DT_CENTER|DT_VCENTER|DT_SINGLELINE, 0
				);
		} // else from if( _pmid.m_bCustomizeListMode )
	} // if( _pmid.m_bPopup )
	
	dc.SelectObject( pOldFont );
	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrOldText );

	// paint icon area
	PaintMenuItemIcon( dc, _pmid );

	// paint inplace control area
	PaintMenuItemInPlaceWnd( dc, _pmid );
}

void CExtPaintManager::PaintMenuItemIcon(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pmid.m_rcItem.IsRectEmpty() )
		return;

bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		CExtPopupMenuWnd * pPopup =
			DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}

CSize sizeIcon( __EXT_MENU_ICON_CX, __EXT_MENU_ICON_CY );
	if( _pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()) )
	{
		sizeIcon = _pmid.m_pIcon->GetSize();
		if(	sizeIcon.cx <= 18 && sizeIcon.cy < sizeIcon.cx )
			sizeIcon.cy = sizeIcon.cx;
	}
CRect rcIcon( _pmid.m_rcItem.TopLeft(), sizeIcon );
	if( bRTL )
		rcIcon.OffsetRect( _pmid.m_rcItem.Width() - sizeIcon.cx, 0 );
	if( _pmid.m_bXtraMark )
	{
		rcIcon.OffsetRect(
			( ( _pmid.m_nIconAreaWidth/2 - rcIcon.Width() ) / 2 + _pmid.m_nIconAreaWidth/2 - 2 )
				* (bRTL ? (-1) : 1),
			( _pmid.m_rcItem.Height() - rcIcon.Height() ) / 2
			);
		if( _pmid.m_bXtraChecked )
		{
			CRect rcXtraMark(
				bRTL ? (_pmid.m_rcItem.right - _pmid.m_rcItem.Height() - 1) : (_pmid.m_rcItem.left + 1),
				_pmid.m_rcItem.top + 1,
				bRTL ? (_pmid.m_rcItem.right - 1) : (_pmid.m_rcItem.left + _pmid.m_rcItem.Height() - 1),
				_pmid.m_rcItem.bottom - 1
				);
			dc.FillRect(
				rcXtraMark,
				&m_brushLightDefault
				);
			COLORREF clrCheck = GetColor( CLR_MENUTEXT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
			int x = rcXtraMark.left + rcXtraMark.Width() / 2 - 3;
			int y = rcXtraMark.top + rcXtraMark.Height() / 2 - 3;
			ASSERT( !_pmid.m_bPushedRadio );
			stat_PaintCheckMark( dc, x, y, clrCheck );
			dc.Draw3dRect(
				rcXtraMark,
				GetColor( CLR_3DSHADOW_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam ),
				GetColor( CLR_3DHILIGHT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam )
				);
		}
	}
	else
	{
		rcIcon.OffsetRect(
			( ( _pmid.m_nIconAreaWidth - rcIcon.Width() ) / 2 ) * (bRTL ? (-1) : 1),
			( _pmid.m_rcItem.Height() - rcIcon.Height() ) / 2
			);
	}

CRect rcIconFrame(rcIcon);
	rcIconFrame.InflateRect(
		__EXT_MENU_IMG_RECT_OUT_GAP,
		__EXT_MENU_IMG_RECT_OUT_GAP
		);

	if( _pmid.m_bIndeterminate )
		dc.FillRect(
			rcIconFrame,
			&m_brushLightestSystem
			);
	else if( _pmid.m_bPushedCheck || _pmid.m_bPushedRadio )
		dc.FillRect(
			rcIconFrame,
			&m_brushLightDefault
			);

	if( _pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()) )
	{
		_pmid.m_pIcon->Paint(
			this,
			dc,
			rcIcon.left,
			rcIcon.top,
			-1,
			-1,
			_pmid.m_bEnabled
				? CExtCmdIcon::__PAINT_NORMAL
				: CExtCmdIcon::__PAINT_DISABLED
			);
		if(		_pmid.m_bSelected
			&&	_pmid.m_bEnabled
			&&	(! ( _pmid.m_bPushedCheck || _pmid.m_bPushedRadio ) )
			)
			dc.Draw3dRect(
				&rcIconFrame,
				GetColor( CLR_3DHILIGHT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam ),
				GetColor( CLR_3DSHADOW_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam )
				);
	}
	else
	{
		PaintSysCmdGlyph(
			dc,
			_pmid.m_nHelperSysCmdID,
			rcIcon,
			_pmid.m_bSelected,
			false,
			_pmid.m_bEnabled
			);
	}
	
	if( _pmid.m_bPushedCheck || _pmid.m_bPushedRadio )
	{
		if( _pmid.m_pIcon == NULL || _pmid.m_pIcon->IsEmpty() )
		{
			COLORREF clrCheck = GetColor( CLR_MENUTEXT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
			int x = rcIcon.left + rcIcon.Width()/2 - 3;
			int y = rcIcon.top + rcIcon.Height()/2 - 3;
			if( _pmid.m_bPushedCheck )
			{
				ASSERT( !_pmid.m_bPushedRadio );
				if( _pmid.m_bIndeterminate )
					stat_PaintIndeterminatedSquare( dc, x, y, clrCheck );
				else
					stat_PaintCheckMark( dc, x, y, clrCheck );
			}
			else
			{
				ASSERT( !_pmid.m_bPushedCheck );
				stat_PaintRadioDot( dc, x, y, clrCheck );
			}
		}
		dc.Draw3dRect(
			rcIconFrame,
			GetColor( CLR_3DSHADOW_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam ),
			GetColor( CLR_3DHILIGHT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam )
			);
	}
}

void CExtPaintManager::PaintMenuItemInPlaceWnd(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		_pmid.m_rcItem.IsRectEmpty() 
		||	_pmid.m_rcInPlaceWnd.IsRectEmpty()
		)
		return;

bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		CExtPopupMenuWnd * pPopup =
			DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}

CRect rcInPlaceWndOuterBorder( _pmid.m_rcInPlaceWnd );
	rcInPlaceWndOuterBorder.InflateRect( 2, 2, 2, 3 );
INT nDD = GetDropDownButtonWidth( _pmid.m_pHelperSrc, _pmid.m_lParam );
CRect rcBk( rcInPlaceWndOuterBorder );
	if( _pmid.m_bPopup )
		rcBk.right += nDD;
	dc.FillSolidRect(
		&rcBk,
		GetColor(
			_pmid.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE,
			_pmid.m_pHelperSrc,
			_pmid.m_lParam
			)
		);
	if( _pmid.m_bPopup )
	{
		rcInPlaceWndOuterBorder.right += nDD;
		if( _pmid.m_bEnabled )
		{
			PAINTCOMBOFRAMEDATA _pcfd(
				_pmid.m_pHelperSrc,
				rcInPlaceWndOuterBorder,
				_pmid.m_bSelected, // hover
				_pmid.m_bSelected && _pmid.m_bHelperPopupDropped, // pushed
				_pmid.m_bEnabled
				);
			_pcfd.m_bNoTopOuterBorder = true;
			PaintComboFrame( dc, _pcfd );
		}
		else
		{
			CRect rcBorder( rcInPlaceWndOuterBorder );
			rcBorder.DeflateRect( 2, 2 );
			COLORREF clrBorder =
				GetColor( COLOR_WINDOW, _pmid.m_pHelperSrc, _pmid.m_lParam );
			dc.Draw3dRect(
				&rcBorder,
				clrBorder,
				clrBorder
				);
			// paint arrow down
			CRect rcBtn( rcBorder );
			rcBtn.left = rcBtn.right - nDD;
			glyph_t * pGlyph = g_DockingCaptionGlyphs[ __DCBT_ARROW_SMALL_DOWN ];
			ASSERT( pGlyph != NULL );
			COLORREF ColorValues[2] =
			{
				RGB(0,0,0),
				GetColor( CLR_MENUTEXT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam )
			};
			PaintGlyphCentered(
				dc,
				rcBtn,
				*pGlyph,
				ColorValues
				);
		}
		rcInPlaceWndOuterBorder.DeflateRect( 1, 1 );
		rcInPlaceWndOuterBorder.right -= nDD;
	}
	else
	{
		if( _pmid.m_bEnabled )
		{
			COLORREF clrFace = GetColor( COLOR_3DFACE, _pmid.m_pHelperSrc, _pmid.m_lParam );
			dc.Draw3dRect(
				rcInPlaceWndOuterBorder,
				clrFace,
				clrFace
				);
			rcInPlaceWndOuterBorder.DeflateRect( 1, 1 );
			if( _pmid.m_bSelected )
				dc.Draw3dRect(
					rcInPlaceWndOuterBorder,
					GetColor( COLOR_3DSHADOW, _pmid.m_pHelperSrc, _pmid.m_lParam ),
					GetColor( COLOR_3DHIGHLIGHT, _pmid.m_pHelperSrc, _pmid.m_lParam )
					);
			else
				dc.Draw3dRect(
					rcInPlaceWndOuterBorder,
					clrFace,
					clrFace
					);
		}
		else
		{
			CRect rcBorder( rcInPlaceWndOuterBorder );
			rcBorder.DeflateRect( 2, 2 );
			COLORREF clrBorder = GetColor( COLOR_WINDOW, _pmid.m_pHelperSrc, _pmid.m_lParam );
			dc.Draw3dRect(
				&rcBorder,
				clrBorder,
				clrBorder
				);
		}
	} // else from if( _pmid.m_bPopup )

	if( ! _pmid.m_strInPlaceText.IsEmpty() )
	{
		rcInPlaceWndOuterBorder.DeflateRect( 5, 2, 2, 2 );
		
		COLORREF clrOldText =
			dc.SetTextColor(
				GetColor( 
					_pmid.m_bEnabled 
						? COLOR_WINDOWTEXT 
						: COLOR_3DSHADOW, 
					_pmid.m_pHelperSrc, 
					_pmid.m_lParam 
					)
				);
		
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		
		CFont * pRenderFont =
			( _pmid.m_hCustomFont == NULL )
				? (&m_FontNormal)
				: CFont::FromHandle(_pmid.m_hCustomFont)
				;
		CFont * pOldFont = dc.SelectObject( pRenderFont );
		ASSERT( pOldFont != NULL );
	
		DWORD dwDtAlignFlags = bRTL ? (DT_RIGHT|DT_RTLREADING) : DT_LEFT;
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(_pmid.m_strInPlaceText), _pmid.m_strInPlaceText.GetLength(),
			&rcInPlaceWndOuterBorder,
			DT_SINGLELINE|DT_NOPREFIX|DT_TOP|dwDtAlignFlags, 0
			);

		dc.SelectObject( pOldFont );
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldText );
	}
}

void CExtPaintManagerXP::PaintMenuItemLeftAreaMarginXP(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid,
	const RECT & rcFillLeftPart,
	const RECT & rcFillRightPart
	)
{
	ASSERT_VALID( this );
	dc; _pmid; rcFillLeftPart; rcFillRightPart;
}

bool CExtPaintManagerXP::PaintMenuItemBackgroundXP(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid,
	const RECT & rcSelectionArea
	)
{
	ASSERT_VALID( this );
	dc; _pmid; rcSelectionArea;
	return false;
}

bool CExtPaintManagerXP::PaintMenuItemSelectionAreaXP(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid,
	const RECT & rcSelectionArea
	)
{
	ASSERT_VALID( this );
	dc; _pmid; rcSelectionArea;
	return false;
}

bool CExtPaintManagerXP::PaintMenuItemCheckMarkAreaXP(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid,
	const RECT & rcCheckMarkArea,
	CExtPaintManagerXP::e_CheckMarkAreaTypeXP eCMATXP
	)
{
	ASSERT_VALID( this );
	dc; _pmid; rcCheckMarkArea; eCMATXP;
	return false;
}

void CExtPaintManagerXP::PaintMenuItem(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pmid.m_rcItem.IsRectEmpty() )
		return;
CExtPopupMenuWnd * pPopup = NULL;
bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		pPopup = DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}
CExtSafeString _sText = (_pmid.m_sText==NULL) ? _T("") : _pmid.m_sText;
CExtSafeString _sAccelText = (_pmid.m_sAccelText==NULL) ? _T("") : _pmid.m_sAccelText;
COLORREF clr3dFace = GetColor( _pmid.m_bRarelyUsed ? XPCLR_RARELY_BORDER : CLR_3DFACE_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
COLORREF clrDkBorder = GetColor( XPCLR_HILIGHT_BORDER, _pmid.m_pHelperSrc, _pmid.m_lParam );
COLORREF clrBk = clr3dFace;
COLORREF clrText = 
		GetColor(
			_pmid.m_bEnabled 
				? _pmid.m_bSelected
					? CLR_MENUTEXT_IN
					: CLR_MENUTEXT_OUT 
				: CLR_MENUTEXT_DISABLED, 
			_pmid.m_pHelperSrc, 
			_pmid.m_lParam 
			);
COLORREF clrRight = GetColor( XPCLR_3DFACE_NORMAL, _pmid.m_pHelperSrc, _pmid.m_lParam );
CRect rcBk( _pmid.m_rcItemWithIndents );
CRect rcSelBk( _pmid.m_rcItem );
	// paint left area
COLORREF clrGradientLeft = ((COLORREF)(-1L)), 
		 clrGradientMiddle((COLORREF)(-1L)),
		 clrGradientRight((COLORREF)(-1L));
	if( ::GetDeviceCaps( dc.m_hDC, BITSPIXEL ) > 8 )
	{
		if( _pmid.m_bRarelyUsed )
		{
			if(		m_nIdxClrMlaRarelyLeft >= 0
				&&	m_nIdxClrMlaRarelyMiddle >= 0
				&&	m_nIdxClrMlaRarelyRight >= 0
				)
			{ 
				// if paint gradient on rarely used item's left part
				clrGradientLeft = GetColor( m_nIdxClrMlaRarelyLeft, _pmid.m_pHelperSrc, _pmid.m_lParam );
				clrGradientMiddle = GetColor( m_nIdxClrMlaRarelyMiddle, _pmid.m_pHelperSrc, _pmid.m_lParam );
				clrGradientRight = GetColor( m_nIdxClrMlaRarelyRight, _pmid.m_pHelperSrc, _pmid.m_lParam );
			}
		}
		else
		{
			if(		m_nIdxClrMlaNormLeft >= 0
				&&	m_nIdxClrMlaNormMiddle >= 0
				&&	m_nIdxClrMlaNormRight >= 0
				)
			{ 
				// if paint gradient on rarely used item's left part
				clrGradientLeft = GetColor( m_nIdxClrMlaNormLeft, _pmid.m_pHelperSrc, _pmid.m_lParam );
				clrGradientMiddle = GetColor( m_nIdxClrMlaNormMiddle, _pmid.m_pHelperSrc, _pmid.m_lParam );
				clrGradientRight = GetColor( m_nIdxClrMlaNormRight, _pmid.m_pHelperSrc, _pmid.m_lParam );
			}
		}
	}
		
CRect rcGradient( rcBk );
	if( bRTL )
		rcGradient.left = rcGradient.right - _pmid.m_nIconAreaWidth;
	else
		rcGradient.right = rcGradient.left + _pmid.m_nIconAreaWidth;

CRect rcFillRightPart( rcBk );
	if( ! _pmid.m_bForceNoLeftGradient )
	{
		if(		(!_pmid.m_bForceNoLeftGradient) 
			&&	clrGradientLeft != ((COLORREF)(-1L)) 
			)
		{
			ASSERT( clrGradientRight != ((COLORREF)(-1L)) );
			stat_PaintGradientRect2steps( 
				dc,
				rcGradient, 
				clrGradientLeft,
				clrGradientMiddle, 
				clrGradientRight,
				false,
				1,
				2
				);
		} // if( (!_pmid.m_bForceNoLeftGradient) && clrGradientLeft != ((COLORREF)(-1L)) )
		else
			dc.FillSolidRect( &rcGradient, clrBk );

		if( bRTL )
			rcFillRightPart.right -= _pmid.m_nIconAreaWidth;
		else
			rcFillRightPart.left += _pmid.m_nIconAreaWidth;
		dc.FillSolidRect( &rcFillRightPart, clrRight );
	} // if( ! _pmid.m_bForceNoLeftGradient )
	PaintMenuItemLeftAreaMarginXP( dc, _pmid, rcGradient, rcFillRightPart );
bool bCustomBackgroundDrawn = PaintMenuItemBackgroundXP( dc, _pmid, rcSelBk );
bool bCustomSelectionDrawn = bCustomBackgroundDrawn;
	if( _pmid.m_bSelected && ( ! bCustomSelectionDrawn ) )
	{
		if( ! PaintMenuItemSelectionAreaXP( dc, _pmid, rcSelBk ) )
		{
			ASSERT( _pmid.m_bEnabled );
			dc.FillSolidRect( &rcSelBk, GetColor( CLR_3DFACE_IN, _pmid.m_pHelperSrc, _pmid.m_lParam )  );	
			dc.Draw3dRect( &rcSelBk, clrDkBorder, clrDkBorder );
		}
		else
			bCustomSelectionDrawn = true;
	}

//	if( ! _pmid.m_bForceNoLeftGradient )
//	{
		if( bRTL )
			rcBk.right -= _pmid.m_nIconAreaWidth;
		else
			rcBk.left += _pmid.m_nIconAreaWidth;
//	}

	rcBk.DeflateRect( __EXT_MENU_GAP, __EXT_MENU_IMG_RECT_OUT_GAP-1 );
	if( bRTL )
		rcBk.right -= 5;
	else
		rcBk.left += 5;

COLORREF clrOldText =
		dc.SetTextColor(
			( _pmid.m_clrForceMenuText != COLORREF(-1L) )
				? _pmid.m_clrForceMenuText
				: clrText
			);
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
CFont * pRenderFont = ( _pmid.m_hCustomFont == NULL ) ? (&m_FontNormal) : CFont::FromHandle(_pmid.m_hCustomFont);
CFont * pOldFont = dc.SelectObject( pRenderFont );
CRect rcMeasureText( 0, 0, 0, 0 );
DWORD dwDtAlignFlags = bRTL ? (DT_RIGHT|DT_RTLREADING) : DT_LEFT;
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_sText), _sText.GetLength(), &rcMeasureText, DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags, 0 );
CRect rcDrawText( rcBk ), rcDrawTextExtended( rcBk );
INT nLenTextExtended = ( _pmid.m_sExtendedText != NULL ) ? INT( _tcslen( _pmid.m_sExtendedText ) ) : 0;
	if( nLenTextExtended > 0 )
		rcDrawText.bottom = rcDrawTextExtended.top = rcDrawText.top + rcMeasureText.Height();
	else
		rcDrawText.OffsetRect( 0, ( rcDrawText.Height() - rcMeasureText.Height() ) / 2 );
	if( bRTL )
		rcDrawText.left = rcDrawText.right - rcMeasureText.Width();
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_sText), _sText.GetLength(), &rcDrawText, DT_SINGLELINE|dwDtAlignFlags, 0 );
	if( nLenTextExtended > 0 )
	{
		CFont * pOldFont = dc.SelectObject( &m_FontNormal );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, _pmid.m_sExtendedText, nLenTextExtended, &rcDrawTextExtended, dwDtAlignFlags, 0 );
		dc.SelectObject( pOldFont );
	}

	if( ! _sAccelText.IsEmpty() )
	{
		rcMeasureText.SetRect( 0, 0, 0, 0 );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_sAccelText), _sAccelText.GetLength(), &rcMeasureText, DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags, 0 );
		rcDrawText = rcBk;
		rcDrawText.OffsetRect( bRTL ? 5 : (-5), ( rcDrawText.Height() - rcMeasureText.Height() ) / 2 );
		if( bRTL )
		{
			rcDrawText.right =
				rcDrawText.left
				+ rcMeasureText.Width()
				+ __EXT_MENU_POPUP_ARROW_AREA_DX
				;
			rcBk.right = rcDrawText.right;
		}
		else
		{
			rcDrawText.left =
				rcDrawText.right
				- rcMeasureText.Width()
				- __EXT_MENU_POPUP_ARROW_AREA_DX
				;
			rcBk.left = rcDrawText.left;
		}

		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(_sAccelText), _sAccelText.GetLength(),
			&rcDrawText,
			DT_SINGLELINE|dwDtAlignFlags, 0
			);
	} // if( ! _sAccelText.IsEmpty() )

	if( _pmid.m_bPopup && (! bCustomBackgroundDrawn ) )
	{
		CRect rcPopupArrow( _pmid.m_rcItem );
		rcPopupArrow.DeflateRect( __EXT_MENU_GAP, __EXT_MENU_GAP );
	
		if( bRTL )
			rcPopupArrow.right =
				rcPopupArrow.left + __EXT_MENU_POPUP_ARROW_AREA_DX;
		else
			rcPopupArrow.left =
				rcPopupArrow.right - __EXT_MENU_POPUP_ARROW_AREA_DX;
		
		if( _pmid.m_bCustomizeListMode )
		{
			COLORREF ColorValues[2] =
			{
				RGB( 0, 0, 0 ),
				GetColor( CLR_MENUTEXT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam )
			};
			PaintGlyphCentered(
				dc,
				rcPopupArrow,
				_pmid.m_bSeparatedDropDown
					? g_glyph_customize_dropdown_src
					: g_glyph_customize_popup_src,
				ColorValues
				);
		}
		else
		{
			COLORREF clrPopupArrow = clrText;
			if( _pmid.m_bSeparatedDropDown )
			{
				if( _pmid.m_bSelected )
				{
					clrPopupArrow = GetColor( CLR_MENUTEXT_IN, _pmid.m_pHelperSrc, _pmid.m_lParam );
					CRect rcButton = rcSelBk;
				
					if( bRTL )
					{
						rcButton.right = rcPopupArrow.right + 1;
						rcButton.DeflateRect( 1, 1, 0, 1 );
					}
					else
					{
						rcButton.left = rcPopupArrow.left - 1;
						rcButton.DeflateRect( 0, 1, 1, 1 );
					}
				
					if( _pmid.m_bHelperPopupDropped )
					{
						if( ! bCustomSelectionDrawn )
							dc.FillSolidRect(
								&rcButton,
								GetColor( XPCLR_HILIGHT, _pmid.m_pHelperSrc, _pmid.m_lParam )
								);
					}
				
					if( bRTL )
						rcButton.left = rcButton.right - 1;
					else
						rcButton.right = rcButton.left + 1;
				
					dc.FillSolidRect(
						&rcButton,
						GetColor( XPCLR_HILIGHT_BORDER, _pmid.m_pHelperSrc, _pmid.m_lParam )
						);	
				} // if( _pmid.m_bSelected )
				else
				{
					CRect rcSeparator(
						bRTL ? rcPopupArrow.right : rcPopupArrow.left,
						rcBk.top,
						bRTL ? rcPopupArrow.right : rcPopupArrow.left,
						rcBk.bottom
						);
					PaintSeparator(
						dc,
						rcSeparator,
						true,
						true,
						NULL
						);
				} // else from if( _pmid.m_bSelected )
			} // if( _pmid.m_bSeparatedDropDown )
		
			dc.SelectObject( &m_FontMarlett );
			
			CRect _rcPopupArrow( rcPopupArrow );
			_rcPopupArrow.left += 2;
			dc.SetTextColor( clrPopupArrow );
			
			LPCTSTR strDrawArrow = bRTL ? _T("3") : _T("4");
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				strDrawArrow,
				_rcPopupArrow,
				DT_CENTER|DT_VCENTER|DT_SINGLELINE, 0
				);

		} // else from if( _pmid.m_bCustomizeListMode )

	} // if( _pmid.m_bPopup && (! bCustomBackgroundDrawn ) )
	
	dc.SelectObject( pOldFont );
	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrOldText );

	// paint icon area
	PaintMenuItemIcon( dc, _pmid );

	// paint inplace control area
	PaintMenuItemInPlaceWnd( dc, _pmid );
}

void CExtPaintManagerXP::PaintMenuItemIcon(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pmid.m_rcItem.IsRectEmpty() )
		return;

CExtPopupMenuWnd * pPopup = NULL;
bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		pPopup = DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}

COLORREF clrBkHilight = 
		GetColor( XPCLR_HILIGHT, _pmid.m_pHelperSrc, _pmid.m_lParam );

COLORREF clrIconBkHilight =
		( _pmid.m_bSelected )
// 	||	( _pmid.m_bPushedCheck || _pmid.m_bPushedRadio )
// 	||	( _pmid.m_bXtraMark || _pmid.m_bXtraChecked ) )
		? clrBkHilight
		: GetColor( CLR_3DFACE_IN, _pmid.m_pHelperSrc, _pmid.m_lParam );

COLORREF clrDkBorder = GetColor( XPCLR_HILIGHT_BORDER, _pmid.m_pHelperSrc, _pmid.m_lParam );
COLORREF clrDkBorderSelected = GetColor( XPCLR_HILIGHT_BORDER_SELECTED, _pmid.m_pHelperSrc, _pmid.m_lParam );

CSize sizeIcon( __EXT_MENU_ICON_CX, __EXT_MENU_ICON_CY );
	if( _pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()) )
	{
		sizeIcon = _pmid.m_pIcon->GetSize();
		if(	sizeIcon.cx <= 18 && sizeIcon.cy < sizeIcon.cx )
			sizeIcon.cy = sizeIcon.cx;
	}
CRect rcIcon( _pmid.m_rcItem.TopLeft(), sizeIcon );
	if( bRTL )
		rcIcon.OffsetRect( _pmid.m_rcItem.Width() - sizeIcon.cx, 0 );
	if( _pmid.m_bXtraMark )
	{
		rcIcon.OffsetRect(
			(( _pmid.m_nIconAreaWidth/2 - rcIcon.Width() ) / 2 + _pmid.m_nIconAreaWidth/2 - 2)
				* ( bRTL ? (-1) : 1 ),
			( _pmid.m_rcItem.Height() - rcIcon.Height() ) / 2
			);
		if( _pmid.m_bXtraChecked )
		{
			CRect rcXtraMark(
				bRTL ? (_pmid.m_rcItem.right - _pmid.m_rcItem.Height() - 1) : (_pmid.m_rcItem.left + 1),
				_pmid.m_rcItem.top + 1,
				bRTL ? (_pmid.m_rcItem.right - 1) : (_pmid.m_rcItem.left + _pmid.m_rcItem.Height() - 1),
				_pmid.m_rcItem.bottom - 1
				);
			bool bCustomBackgroundDrawn = false;
			if(		pPopup != NULL
				&&	( pPopup->TrackFlagsGet() & TPMX_RIBBON_MODE ) != 0 
				)
				bCustomBackgroundDrawn = 
					PaintMenuItemCheckMarkAreaXP(
						dc,
						_pmid,
						rcXtraMark,
						__ECMATXP_EXTRA_MARK_CHECKED
						);
			if( ! bCustomBackgroundDrawn )
			{
				int nIdxMainGradientLeft = -1;
				int nIdxMainGradientRight = -1;
				
				if( _pmid.m_bSelected )
				{
					nIdxMainGradientLeft = m_nIdxClrBtnHovPresLeft;
					nIdxMainGradientRight = m_nIdxClrBtnHovPresRight;
				}
				else
				{
					nIdxMainGradientLeft = m_nIdxClrBtnPressedLeft;
					nIdxMainGradientRight = m_nIdxClrBtnPressedRight;
				}

				if( nIdxMainGradientLeft >= 0 && nIdxMainGradientRight >= 0 )
				{
					stat_PaintGradientRect(
						dc,
						rcXtraMark,
						GetColor( nIdxMainGradientRight, _pmid.m_pHelperSrc, _pmid.m_lParam ),
						GetColor( nIdxMainGradientLeft, _pmid.m_pHelperSrc, _pmid.m_lParam ),
						true
						);
				}
				else
				{
					dc.FillSolidRect(
						rcXtraMark,
						clrIconBkHilight
						);
				}
				COLORREF clrCheck = GetColor( CLR_MENUTEXT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
				int x = rcXtraMark.left + rcXtraMark.Width()/2 - 3;
				int y = rcXtraMark.top + rcXtraMark.Height()/2 - 3;
				ASSERT( !_pmid.m_bPushedRadio );
				stat_PaintCheckMark( dc, x, y, clrCheck );
				COLORREF clrBorder = _pmid.m_bSelected ? clrDkBorderSelected : clrDkBorder;
				dc.Draw3dRect( rcXtraMark, clrBorder, clrBorder );
			}
		}
	}
	else
	{
		rcIcon.OffsetRect(
			( ( _pmid.m_nIconAreaWidth - rcIcon.Width() ) / 2 - 1 ) * (bRTL ? (-1) : 1),
			( _pmid.m_rcItem.Height() - rcIcon.Height() ) / 2
			);
	}

CRect rcIconFrame( rcIcon );
	rcIconFrame.InflateRect(__EXT_MENU_IMG_RECT_OUT_GAP,__EXT_MENU_IMG_RECT_OUT_GAP);
	if( _pmid.m_bPushedCheck || _pmid.m_bPushedRadio )
	{
		bool bCustomBackgroundDrawn = false;
		if(		pPopup != NULL
			&&	( pPopup->TrackFlagsGet() & TPMX_RIBBON_MODE ) != 0 
			)
			bCustomBackgroundDrawn = 
				PaintMenuItemCheckMarkAreaXP(
					dc,
					_pmid,
					rcIconFrame,
					__ECMATXP_ICON_OR_CHECK_OR_RADIO_FRAME
					);
		if( ! bCustomBackgroundDrawn )
		{
			int nIdxMainGradientLeft = -1;
			int nIdxMainGradientRight = -1;

			if( _pmid.m_bSelected )
			{
				nIdxMainGradientLeft = m_nIdxClrBtnHovPresLeft;
				nIdxMainGradientRight = m_nIdxClrBtnHovPresRight;
			}
			else if( _pmid.m_bIndeterminate )
			{
				clrIconBkHilight =
					dc.GetNearestColor(
						stat_HLS_Adjust(
							clrIconBkHilight,
							0.0,
							0.85,
							0.5
							)
						);
			}
			else
			{
				nIdxMainGradientLeft = m_nIdxClrBtnPressedLeft;
				nIdxMainGradientRight = m_nIdxClrBtnPressedRight;
			}

			if( nIdxMainGradientLeft >= 0 && nIdxMainGradientRight >= 0 )
			{
				stat_PaintGradientRect(
					dc,
					rcIconFrame,
					GetColor( nIdxMainGradientRight, _pmid.m_pHelperSrc, _pmid.m_lParam ),
					GetColor( nIdxMainGradientLeft, _pmid.m_pHelperSrc, _pmid.m_lParam ),
					true
					);
			}
			else
			{
				dc.FillSolidRect(
					rcIconFrame,
					clrIconBkHilight
					);
			}
		}
	}
	if( _pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()) )
	{
		PaintIcon(
			dc,
			true,
			_pmid.m_pIcon,
			rcIcon,
			_pmid.m_bPushedCheck || _pmid.m_bPushedRadio,
			_pmid.m_bEnabled,
			_pmid.m_bSelected,
			__ALIGN_HORIZ_LEFT | __ALIGN_VERT_TOP,
			false,
			NULL,
			NULL,
			NULL,
			_pmid.m_clrCustomAccentEffectForIcon
			);
	}
	else
	{
		PaintSysCmdGlyph(
			dc,
			_pmid.m_nHelperSysCmdID,
			rcIcon,
			_pmid.m_bSelected,
			false,
			_pmid.m_bEnabled
			);
	}

	if( _pmid.m_bPushedCheck || _pmid.m_bPushedRadio )
	{
		if( _pmid.m_pIcon == NULL || _pmid.m_pIcon->IsEmpty() )
		{
			COLORREF clrCheck = GetColor( CLR_MENUTEXT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
			int x = rcIcon.left + rcIcon.Width()/2 - 3;
			int y = rcIcon.top + rcIcon.Height()/2 - 3;
			if( _pmid.m_bPushedCheck )
			{
				ASSERT( !_pmid.m_bPushedRadio );
				if( _pmid.m_bIndeterminate )
					stat_PaintIndeterminatedSquare( dc, x, y, clrCheck );
				else
					stat_PaintCheckMark( dc, x, y, clrCheck );
			}
			else
			{
				ASSERT( !_pmid.m_bPushedCheck );
				stat_PaintRadioDot( dc, x, y, clrCheck );
			}
		}

		COLORREF clrBorder = 
			_pmid.m_bSelected ? clrDkBorderSelected : clrDkBorder;
		dc.Draw3dRect(
			rcIconFrame,
			clrBorder,
			clrBorder
			);
	}
}

void CExtPaintManagerXP::PaintMenuItemInPlaceWnd(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		_pmid.m_rcItem.IsRectEmpty() 
		||	_pmid.m_rcInPlaceWnd.IsRectEmpty()
		)
		return;

bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		CExtPopupMenuWnd * pPopup =
			DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}

CRect rcInPlaceWndOuterBorder( _pmid.m_rcInPlaceWnd );
	rcInPlaceWndOuterBorder.InflateRect( 1, 2 );
	if( _pmid.m_bPopup )
	{
		INT nDD = GetDropDownButtonWidth( _pmid.m_pHelperSrc, _pmid.m_lParam );
		rcInPlaceWndOuterBorder.right += nDD;
	
		dc.FillSolidRect(
			&rcInPlaceWndOuterBorder,
			::GetSysColor( _pmid.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE )
			);

		PAINTCOMBOFRAMEDATA _pcfd(
			_pmid.m_pHelperSrc,
			rcInPlaceWndOuterBorder,
			_pmid.m_bSelected,
			_pmid.m_bHelperPopupDropped,
			_pmid.m_bEnabled
			);
		PaintComboFrame(
			dc,
			_pcfd
			);

		if( ! _pmid.m_bSelected )
		{
			COLORREF clrBorder = 
				GetColor( 
					_pmid.m_bEnabled 
						? XPCLR_TEXT_FIELD_BORDER_NORMAL 
						: XPCLR_TEXT_FIELD_BORDER_DISABLED
						, 
					_pmid.m_pHelperSrc, 
					_pmid.m_lParam 
				);
			dc.Draw3dRect(
				&rcInPlaceWndOuterBorder,
				clrBorder,
				clrBorder
				);
		}
		rcInPlaceWndOuterBorder.right -= nDD;
	}
	else
	{
		// paint text field
		dc.FillSolidRect(
			&rcInPlaceWndOuterBorder,
			::GetSysColor( _pmid.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE )
			);
		COLORREF clrBorder = 
			_pmid.m_bSelected
				? GetColor( XPCLR_HILIGHT_BORDER, _pmid.m_pHelperSrc, _pmid.m_lParam )
				: GetColor( 
					_pmid.m_bEnabled 
						? XPCLR_TEXT_FIELD_BORDER_NORMAL 
						: XPCLR_TEXT_FIELD_BORDER_DISABLED, 
					_pmid.m_pHelperSrc, 
					_pmid.m_lParam 
					);
		dc.Draw3dRect(
			&rcInPlaceWndOuterBorder,
			clrBorder,
			clrBorder
			);
	}

	if( !_pmid.m_strInPlaceText.IsEmpty() )
	{
		rcInPlaceWndOuterBorder.DeflateRect( 5, 3, 2, 2 );
		
		COLORREF clrOldText =
			dc.SetTextColor(
				GetColor( 
					_pmid.m_bEnabled 
						? COLOR_WINDOWTEXT 
						: COLOR_3DSHADOW, 
					_pmid.m_pHelperSrc, 
					_pmid.m_lParam 
					)
				);
		
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		
		CFont * pRenderFont =
			( _pmid.m_hCustomFont == NULL )
				? (&m_FontNormal)
				: CFont::FromHandle(_pmid.m_hCustomFont)
				;
		CFont * pOldFont = dc.SelectObject( pRenderFont );
		ASSERT( pOldFont != NULL );
	
		DWORD dwDtAlignFlags = bRTL ? (DT_RIGHT|DT_RTLREADING) : DT_LEFT;
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(_pmid.m_strInPlaceText), _pmid.m_strInPlaceText.GetLength(),
			&rcInPlaceWndOuterBorder,
			DT_SINGLELINE|DT_NOPREFIX|DT_TOP|dwDtAlignFlags, 0
			);

		dc.SelectObject( pOldFont );
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldText );
	}
}

bool CExtPaintManagerNativeXP::PaintMenuItem_UX(
	HWND hWnd,
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pmid.m_rcItem.IsRectEmpty() || hWnd == NULL || (! ::IsWindow( hWnd ) ) )
		return false;
bool bThemedApproachOK = false;
	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_MENU ) == NULL )
		return false;
bool bCloseTheme = true;
	try
	{
		HRESULT hr = S_OK;
		bool bRTL = false;
		if( _pmid.m_pHelperSrc != NULL )
		{
			ASSERT_VALID( _pmid.m_pHelperSrc );
			CExtPopupMenuWnd * pPopup =
				DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
			if( pPopup != NULL )
				bRTL = pPopup->OnQueryLayoutRTL();
		}

		CExtSafeString _sText = (_pmid.m_sText==NULL) ? _T("") : _pmid.m_sText;
		CExtSafeString _sAccelText = (_pmid.m_sAccelText==NULL) ? _T("") : _pmid.m_sAccelText;
// COLORREF clrText = 
// _pmid.m_bSelected 
// ? ::GetSysColor( COLOR_HIGHLIGHTTEXT )
// : ::GetSysColor( COLOR_MENUTEXT );
		CRect rcBk( _pmid.m_rcItem );
// bool bSmallerSelRect = 
// 	(		(_pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()))
// 		||	_pmid.m_bPushedCheck
// 		||	_pmid.m_bPushedRadio
// 		) ? true : false;
// if( bSmallerSelRect )
// {
// 	if( bRTL )
// 		rcBk.right -= _pmid.m_nIconAreaWidth;
// 	else
// 		rcBk.left += _pmid.m_nIconAreaWidth;
// }
		rcBk.DeflateRect( 1, 0, 1, 0 );
		CRect rcFillSelectedBk = rcBk;
		if( _pmid.m_rcInPlaceWnd.IsRectEmpty() )
			rcBk.DeflateRect( 0, 1 );
		if( _pmid.m_bSelected )
		{
			if(		_pmid.m_bSeparatedDropDown
				&&	( ! _pmid.m_bHelperPopupDropped )
				)
			{
				if( bRTL )
					rcFillSelectedBk.left += __EXT_MENU_POPUP_ARROW_AREA_DX;
				else
					rcFillSelectedBk.right -= __EXT_MENU_POPUP_ARROW_AREA_DX;
			}
// dc.FillSolidRect( 
// rcFillSelectedBk, 
// ::GetSysColor( COLOR_HIGHLIGHT ) 
// );
		}
		int nMenuItemStateID = ( _pmid.m_bEnabled ? ( _pmid.m_bSelected ? MPI_HOT : MPI_NORMAL ) : MPI_DISABLED );
		if( _pmid.m_nIconAreaWidth > 0 )
		{
			CRect rcLeftArea = rcFillSelectedBk;
			if( bRTL )
				rcLeftArea.left = rcLeftArea.right - _pmid.m_nIconAreaWidth;
			else
				rcLeftArea.right = rcLeftArea.left + _pmid.m_nIconAreaWidth;
			rcLeftArea.InflateRect( 0, 1 );
			hr =
				g_PaintManager.m_UxTheme.DrawBackground(
					hWnd, 
					dc.GetSafeHdc(), 
					MENU_POPUPGUTTER,
					nMenuItemStateID, 
					&rcLeftArea, 
					&rcLeftArea
					);
			if( hr != S_OK )
				::AfxThrowUserException();
		}
		hr =
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd, 
				dc.GetSafeHdc(), 
				MENU_POPUPITEM,
				nMenuItemStateID, 
				&rcFillSelectedBk, 
				&rcFillSelectedBk
				);
		if( hr != S_OK )
			::AfxThrowUserException();

// if( ! bSmallerSelRect )
// {
// 	if( bRTL )
// 		rcBk.right -= _pmid.m_nIconAreaWidth;
// 	else
// 		rcBk.left += _pmid.m_nIconAreaWidth;
// }
		if( bRTL )
			rcBk.right -= _pmid.m_nIconAreaWidth;
		else
			rcBk.left += _pmid.m_nIconAreaWidth;
		rcBk.DeflateRect( __EXT_MENU_GAP, __EXT_MENU_GAP );

// COLORREF clrOldText =
// dc.SetTextColor(
// ( _pmid.m_clrForceMenuText != COLORREF(-1L) )
// 	? _pmid.m_clrForceMenuText
// 	: clrText
// );
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );

		rcBk.DeflateRect( __EXT_MENU_GAP, 0 );

		CFont * pRenderFont =
			( _pmid.m_hCustomFont == NULL )
				? (&m_FontNormal)
				: CFont::FromHandle(_pmid.m_hCustomFont)
				;
		CFont * pOldFont = dc.SelectObject( pRenderFont );
		ASSERT( pOldFont != NULL );

		DWORD dwDtAlignFlags = bRTL ? (DT_RIGHT|DT_RTLREADING) : DT_LEFT;
		CRect rcMeasureText( 0, 0, 0, 0 );
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC, LPCTSTR(_sText), &rcMeasureText, DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags, 0,
			hWnd, VSCLASS_MENU, 0, MENU_POPUPITEM, nMenuItemStateID
			);
// 		hr =
// 			g_PaintManager.m_UxTheme.DrawThemeText(
// 				dc.GetSafeHdc(), 
// 				MENU_POPUPITEM,
// 				nMenuItemStateID, 
// 				LPCTSTR(_sText),
// 				_sText.GetLength(),
// 				DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags,
// 				0,
// 				&rcMeasureText
// 				);
// 		if( hr != S_OK )
// 			::AfxThrowUserException();

		CRect rcDrawText( rcBk ), rcDrawTextExtended( rcBk );
		INT nLenTextExtended = ( _pmid.m_sExtendedText != NULL ) ? INT( _tcslen( _pmid.m_sExtendedText ) ) : 0;

		if( nLenTextExtended > 0 )
		{
			rcDrawText.bottom
				= rcDrawTextExtended.top
				= rcDrawText.top + rcMeasureText.Height();
		}
		else
			rcDrawText.OffsetRect(
				0,
				( rcDrawText.Height() - rcMeasureText.Height() ) / 2
				);

/*
		hr =
			g_PaintManager.m_UxTheme.DrawThemeText(
				dc.GetSafeHdc(), 
				MENU_POPUPITEM,
				nMenuItemStateID, 
				LPCTSTR(_sText),
				_sText.GetLength(),
				DT_SINGLELINE|dwDtAlignFlags,
				0,
				&rcDrawText
				);
		if( hr != S_OK )
			::AfxThrowUserException();
*/
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC, LPCTSTR(_sText), &rcDrawText, DT_SINGLELINE|dwDtAlignFlags, 0,
			hWnd, VSCLASS_MENU, 0, MENU_POPUPITEM, nMenuItemStateID
			);

		if( nLenTextExtended > 0 )
		{
			CFont * pOldFont = dc.SelectObject( &m_FontNormal );
/*
			hr =
				g_PaintManager.m_UxTheme.DrawThemeText(
					dc.GetSafeHdc(), 
					MENU_POPUPITEM,
					nMenuItemStateID, 
					LPCTSTR(_pmid.m_sExtendedText),
					nLenTextExtended,
					dwDtAlignFlags,
					0,
					&rcDrawTextExtended
					);
			if( hr != S_OK )
				::AfxThrowUserException();
*/
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC, LPCTSTR(_pmid.m_sExtendedText), nLenTextExtended, &rcDrawTextExtended, dwDtAlignFlags, 0,
				hWnd, VSCLASS_MENU, 0, MENU_POPUPITEM, nMenuItemStateID
				);
			dc.SelectObject( pOldFont );
		}

		if( ! _sAccelText.IsEmpty() )
		{
			rcMeasureText.SetRect( 0, 0, 0, 0 );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC, LPCTSTR(_sAccelText), _sAccelText.GetLength(), &rcMeasureText,
				DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags, 0
				);
			rcDrawText = rcBk;
			rcDrawText.OffsetRect(
				0,
				( rcDrawText.Height() - rcMeasureText.Height() ) / 2
				);
			if( bRTL )
			{
				rcDrawText.right =
					rcDrawText.left
					+ rcMeasureText.Width()
					+ __EXT_MENU_POPUP_ARROW_AREA_DX
					;
				rcBk.right = rcDrawText.right;
			}
			else
			{
				rcDrawText.left =
					rcDrawText.right
					- rcMeasureText.Width()
					- __EXT_MENU_POPUP_ARROW_AREA_DX
					;
				rcBk.left = rcDrawText.left;
			}
/*
			hr =
				g_PaintManager.m_UxTheme.DrawThemeText(
					dc.GetSafeHdc(), 
					MENU_POPUPITEM,
					nMenuItemStateID, 
					LPCTSTR(_sAccelText),
					_sAccelText.GetLength(),
					DT_SINGLELINE|dwDtAlignFlags,
					0,
					&rcDrawText
					);
			if( hr != S_OK )
				::AfxThrowUserException();
*/
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC, LPCTSTR(_sAccelText), &rcDrawText, DT_SINGLELINE|dwDtAlignFlags, 0,
				hWnd, VSCLASS_MENU, 0, MENU_POPUPITEM, nMenuItemStateID
				);
		} // if( ! _sAccelText.IsEmpty() )

		if( _pmid.m_bPopup )
		{
			CRect rcPopupArrow( _pmid.m_rcItem );
			rcPopupArrow.DeflateRect( __EXT_MENU_GAP, __EXT_MENU_GAP );
			if( bRTL )
				rcPopupArrow.right =
					rcPopupArrow.left + __EXT_MENU_POPUP_ARROW_AREA_DX;
			else
				rcPopupArrow.left =
					rcPopupArrow.right - __EXT_MENU_POPUP_ARROW_AREA_DX;
			if( _pmid.m_bCustomizeListMode )
			{
				COLORREF ColorValues[2] =
				{
					RGB(0,0,0),
					::GetSysColor(
						//_pmid.m_bSelected ? COLOR_HIGHLIGHTTEXT :
							COLOR_MENUTEXT )
				};
				PaintGlyphCentered(
					dc,
					rcPopupArrow,
					_pmid.m_bSeparatedDropDown
						? g_glyph_customize_dropdown_src
						: g_glyph_customize_popup_src,
					ColorValues
					);
			}
			else
			{
				CRect _rcPopupArrow( rcPopupArrow );
				if( bRTL )
					_rcPopupArrow.right -= 2;
				else
					_rcPopupArrow.left += 2;
				hr =
					g_PaintManager.m_UxTheme.DrawBackground(
						hWnd, 
						dc.GetSafeHdc(), 
						MENU_POPUPSUBMENU,
						_pmid.m_bEnabled ? MSM_NORMAL : MSM_DISABLED, 
						&_rcPopupArrow, 
						&_rcPopupArrow
						);
				if( hr != S_OK )
					::AfxThrowUserException();
			} // else from if( _pmid.m_bCustomizeListMode )
		} // if( _pmid.m_bPopup )

		dc.SelectObject( pOldFont );
		dc.SetBkMode( nOldBkMode );
// dc.SetTextColor( clrOldText );

		// paint icon area
		if( ! PaintMenuItemIcon_UX( hWnd, dc, _pmid ) )
			::AfxThrowUserException();

		g_PaintManager.m_UxTheme.CloseThemeData();
		bCloseTheme = false;

		// paint inplace control area
		PaintMenuItemInPlaceWnd( dc, _pmid );

		bThemedApproachOK = true;
	} // try
	catch( CException * pException )
	{
		pException->Delete();
		bThemedApproachOK = false;
	} 
	catch( ... )
	{
		bThemedApproachOK = false;
	}
	if( bCloseTheme )
		g_PaintManager.m_UxTheme.CloseThemeData();
	return bThemedApproachOK;
}

void CExtPaintManagerNativeXP::PaintMenuItem(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pmid.m_rcItem.IsRectEmpty() )
		return;

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintMenuItem(
			dc,
			_pmid
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )

HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, _pmid.m_pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}

	if( PaintMenuItem_UX( hWnd, dc, _pmid ) )
		return;

bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		CExtPopupMenuWnd * pPopup =
			DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}

CExtSafeString _sText = (_pmid.m_sText==NULL) ? _T("") : _pmid.m_sText;
CExtSafeString _sAccelText = (_pmid.m_sAccelText==NULL) ? _T("") : _pmid.m_sAccelText;
COLORREF clrText = 
		_pmid.m_bSelected 
			? ::GetSysColor( COLOR_HIGHLIGHTTEXT )
			: ::GetSysColor( COLOR_MENUTEXT );
CRect rcBk( _pmid.m_rcItem );
bool bSmallerSelRect = 
		(		(_pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()))
			||	_pmid.m_bPushedCheck
			||	_pmid.m_bPushedRadio
			) ? true : false;
	if( bSmallerSelRect )
	{
		if( bRTL )
			rcBk.right -= _pmid.m_nIconAreaWidth;
		else
			rcBk.left += _pmid.m_nIconAreaWidth;
	}
	rcBk.DeflateRect( 1, 0, 1, 0 );

	if( _pmid.m_rcInPlaceWnd.IsRectEmpty() )
		rcBk.DeflateRect( 0, 1 );

	if( _pmid.m_bSelected )
	{
		CRect rcFillSelectedBk = rcBk;
		if(		_pmid.m_bSeparatedDropDown
			&&	( ! _pmid.m_bHelperPopupDropped )
			)
		{
			if( bRTL )
				rcFillSelectedBk.left += __EXT_MENU_POPUP_ARROW_AREA_DX;
			else
				rcFillSelectedBk.right -= __EXT_MENU_POPUP_ARROW_AREA_DX;
		}
		dc.FillSolidRect( 
			rcFillSelectedBk, 
			::GetSysColor( COLOR_HIGHLIGHT ) 
			);
	}

	if( ! bSmallerSelRect )
	{
		if( bRTL )
			rcBk.right -= _pmid.m_nIconAreaWidth;
		else
			rcBk.left += _pmid.m_nIconAreaWidth;
	}
	rcBk.DeflateRect( __EXT_MENU_GAP, __EXT_MENU_GAP );

COLORREF clrOldText =
		dc.SetTextColor(
			( _pmid.m_clrForceMenuText != COLORREF(-1L) )
				? _pmid.m_clrForceMenuText
				: clrText
			);
int nOldBkMode = dc.SetBkMode( TRANSPARENT );

	rcBk.DeflateRect( __EXT_MENU_GAP, 0 );

CFont * pRenderFont =
		( _pmid.m_hCustomFont == NULL )
			? (&m_FontNormal)
			: CFont::FromHandle(_pmid.m_hCustomFont)
			;
CFont * pOldFont = dc.SelectObject( pRenderFont );
	ASSERT( pOldFont != NULL );

DWORD dwDtAlignFlags = bRTL ? (DT_RIGHT|DT_RTLREADING) : DT_LEFT;
CRect rcMeasureText( 0, 0, 0, 0 );
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		LPCTSTR(_sText), _sText.GetLength(),
		&rcMeasureText,
		DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags, 0
		);
CRect rcDrawText( rcBk ), rcDrawTextExtended( rcBk );
INT nLenTextExtended = ( _pmid.m_sExtendedText != NULL ) ? INT( _tcslen( _pmid.m_sExtendedText ) ) : 0;
	
	if( nLenTextExtended > 0 )
	{
		rcDrawText.bottom
			= rcDrawTextExtended.top
			= rcDrawText.top + rcMeasureText.Height();
	}
	else
		rcDrawText.OffsetRect(
			0,
			( rcDrawText.Height() - rcMeasureText.Height() ) / 2
			);

	if( bRTL )
	{
		rcDrawText.left = rcDrawText.right - rcMeasureText.Width();
		if( _pmid.m_bEnabled )
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(_sText), _sText.GetLength(),
				&rcDrawText,
				DT_SINGLELINE|dwDtAlignFlags, 0
				);
		else
		{
			CRect rcDrawTextOffs = rcDrawText;
			rcDrawTextOffs.OffsetRect( 1, 1 );
			COLORREF clrTextRestore =
				dc.SetTextColor( ::GetSysColor( COLOR_3DHILIGHT ) );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(_sText), _sText.GetLength(),
				&rcDrawTextOffs,
				DT_SINGLELINE|dwDtAlignFlags, 0
				);
			dc.SetTextColor( ::GetSysColor( COLOR_3DSHADOW ) );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(_sText), _sText.GetLength(),
				&rcDrawText,
				DT_SINGLELINE|dwDtAlignFlags, 0
				);
			dc.SetTextColor( clrTextRestore );
		}
	}
	else
	{
		CExtRichContentLayout::DRAWSTATE_data_t _DSD;
		_DSD.m_fuFlags |= ( _pmid.m_bEnabled ? DSS_NORMAL : m_DSS_DISABLED_style );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_sText), rcDrawText, 0, 0, NULL, NULL, 0, 0, 0, NULL, &_DSD );
	}
	
	if( nLenTextExtended > 0 )
	{
		CFont * pOldFont = dc.SelectObject( &m_FontNormal );
		COLORREF clrOldTextColor = COLORREF(-1L);
		if( ! _pmid.m_bEnabled )
			clrOldTextColor = dc.SetTextColor( GetColor(COLOR_3DSHADOW) );
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			_pmid.m_sExtendedText, nLenTextExtended,
			&rcDrawTextExtended,
			dwDtAlignFlags, 0
			);
		if( ! _pmid.m_bEnabled )
			dc.SetTextColor( clrOldTextColor );
		dc.SelectObject( pOldFont );
	}

	if( ! _sAccelText.IsEmpty() )
	{
		rcMeasureText.SetRect( 0, 0, 0, 0 );
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(_sAccelText), _sAccelText.GetLength(),
			&rcMeasureText,
			DT_CALCRECT|DT_SINGLELINE|dwDtAlignFlags, 0
			);
		rcDrawText = rcBk;
		rcDrawText.OffsetRect(
			0,
			( rcDrawText.Height() - rcMeasureText.Height() ) / 2
			);
		if( bRTL )
		{
			rcDrawText.right =
				rcDrawText.left
				+ rcMeasureText.Width()
				+ __EXT_MENU_POPUP_ARROW_AREA_DX
				;
			rcBk.right = rcDrawText.right;
		}
		else
		{
			rcDrawText.left =
				rcDrawText.right
				- rcMeasureText.Width()
				- __EXT_MENU_POPUP_ARROW_AREA_DX
				;
			rcBk.left = rcDrawText.left;
		}
		if( bRTL )
		{
			rcDrawText.left = rcDrawText.right - rcMeasureText.Width();
			if( _pmid.m_bEnabled )
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(_sAccelText), _sAccelText.GetLength(),
					&rcDrawText,
					DT_SINGLELINE|dwDtAlignFlags, 0
					);
			else
			{
				CRect rcDrawTextOffs = rcDrawText;
				rcDrawTextOffs.OffsetRect( 1, 1 );
				COLORREF clrTextRestore = dc.SetTextColor( ::GetSysColor( COLOR_3DHILIGHT ) );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(_sAccelText), _sAccelText.GetLength(),
					&rcDrawTextOffs,
					DT_SINGLELINE|dwDtAlignFlags, 0
					);
				dc.SetTextColor( ::GetSysColor( COLOR_3DSHADOW ) );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(_sAccelText), _sAccelText.GetLength(),
					&rcDrawText,
					DT_SINGLELINE|dwDtAlignFlags, 0
					);
				dc.SetTextColor( clrTextRestore );
			}
		} // if( bRTL )
		else
		{
			CExtRichContentLayout::DRAWSTATE_data_t _DSD;
			_DSD.m_fuFlags |= ( _pmid.m_bEnabled ? DSS_NORMAL : m_DSS_DISABLED_style );
			CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_sAccelText), rcDrawText, 0, 0, NULL, NULL, 0, 0, 0, NULL, &_DSD );
		}
	} // if( ! _sAccelText.IsEmpty() )

	if( _pmid.m_bPopup )
	{
		CRect rcPopupArrow( _pmid.m_rcItem );
		rcPopupArrow.DeflateRect( __EXT_MENU_GAP, __EXT_MENU_GAP );
		if( bRTL )
			rcPopupArrow.right =
				rcPopupArrow.left + __EXT_MENU_POPUP_ARROW_AREA_DX;
		else
			rcPopupArrow.left =
				rcPopupArrow.right - __EXT_MENU_POPUP_ARROW_AREA_DX;
		if( _pmid.m_bCustomizeListMode )
		{
			COLORREF ColorValues[2] =
			{
				RGB(0,0,0),
				::GetSysColor( _pmid.m_bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT )
			};
			PaintGlyphCentered(
				dc,
				rcPopupArrow,
				_pmid.m_bSeparatedDropDown
					? g_glyph_customize_dropdown_src
					: g_glyph_customize_popup_src,
				ColorValues
				);
		}
		else
		{
			COLORREF clrPopupArrow = clrText;
			if( _pmid.m_bSeparatedDropDown )
			{
				if( _pmid.m_bSelected )
				{
					clrPopupArrow = ::GetSysColor( _pmid.m_bHelperPopupDropped ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT );
				}
				else
				{
					CRect rcSeparator(
						bRTL ? rcPopupArrow.right : rcPopupArrow.left,
						rcBk.top,
						bRTL ? rcPopupArrow.right : rcPopupArrow.left,
						rcBk.bottom
						);
					PaintSeparator(
						dc,
						rcSeparator,
						true,
						true,
						NULL
						);
				}
			}

			dc.SelectObject( &m_FontMarlett );
			CRect _rcPopupArrow( rcPopupArrow );
			if( bRTL )
				_rcPopupArrow.right -= 2;
			else
				_rcPopupArrow.left += 2;
			dc.SetTextColor( clrPopupArrow );
			LPCTSTR strDrawArrow = bRTL ? _T("3") : _T("4");
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				strDrawArrow,
				_rcPopupArrow,
				DT_CENTER|DT_VCENTER|DT_SINGLELINE, 0
				);
		} // else from if( _pmid.m_bCustomizeListMode )
	} // if( _pmid.m_bPopup )
	
	dc.SelectObject( pOldFont );
	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrOldText );

	// paint icon area
	PaintMenuItemIcon( dc, _pmid );

	// paint inplace control area
	PaintMenuItemInPlaceWnd( dc, _pmid );
}

void CExtPaintManagerNativeXP::PaintMenuItemIcon(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintMenuItemIcon(
			dc,
			_pmid
			);
		return;
	}
	
	if( _pmid.m_rcItem.IsRectEmpty() )
		return;

HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, _pmid.m_pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}

bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		CExtPopupMenuWnd * pPopup =
			DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}

	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_TOOLBAR ) != NULL )
	{
		CSize sizeIcon( __EXT_MENU_ICON_CX, __EXT_MENU_ICON_CY );
		if( _pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()) )
		{
			sizeIcon = _pmid.m_pIcon->GetSize();
			if(		sizeIcon.cx <= 18 
				&&	sizeIcon.cy < sizeIcon.cx 
				)
				sizeIcon.cy = sizeIcon.cx;
		}
		CRect rcIcon( _pmid.m_rcItem.TopLeft(), sizeIcon );
		if( bRTL )
			rcIcon.OffsetRect( _pmid.m_rcItem.Width() - sizeIcon.cx, 0 );
		if( _pmid.m_bXtraMark )
		{
			rcIcon.OffsetRect(
				( ( _pmid.m_nIconAreaWidth/2 - rcIcon.Width() ) / 2 + _pmid.m_nIconAreaWidth/2 - 2 )
					* (bRTL ? (-1) : 1),
				( _pmid.m_rcItem.Height() - rcIcon.Height() ) / 2
				);
			if( _pmid.m_bXtraChecked )
			{
				CRect rcXtraMark(
					bRTL ? (_pmid.m_rcItem.right - _pmid.m_rcItem.Height() - 1) : (_pmid.m_rcItem.left + 1),
					_pmid.m_rcItem.top + 1,
					bRTL ? (_pmid.m_rcItem.right - 1) : (_pmid.m_rcItem.left + _pmid.m_rcItem.Height() - 1),
					_pmid.m_rcItem.bottom - 1
					);
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWnd,
						dc.GetSafeHdc(), 
						TP_BUTTON, 
						_pmid.m_bSelected ? TS_HOTCHECKED : TS_CHECKED, 
						&rcXtraMark, 
						&rcXtraMark
						) == S_OK
					);
				COLORREF clrCheck = ::GetSysColor( COLOR_BTNTEXT );
				int x = rcXtraMark.left + rcXtraMark.Width()/2 - 3;
				int y = rcXtraMark.top + rcXtraMark.Height()/2 - 3;
				ASSERT( !_pmid.m_bPushedRadio );
				stat_PaintCheckMark( dc, x, y, clrCheck );
			}
		}
		else
		{
			rcIcon.OffsetRect(
				( ( _pmid.m_nIconAreaWidth - rcIcon.Width() ) / 2 ) * (bRTL ? (-1) : 1),
				( _pmid.m_rcItem.Height() - rcIcon.Height() ) / 2
				);
		}

		CRect rcIconFrame(rcIcon);
		rcIconFrame.InflateRect(
			__EXT_MENU_IMG_RECT_OUT_GAP,
			__EXT_MENU_IMG_RECT_OUT_GAP
			);

		if( _pmid.m_bPushedCheck || _pmid.m_bIndeterminate || _pmid.m_bPushedRadio )
		{
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					hWnd,
					dc.GetSafeHdc(), 
					TP_BUTTON, 
					_pmid.m_bSelected ? TS_HOTCHECKED : ( _pmid.m_bIndeterminate ? TS_PRESSED : TS_CHECKED ), 
					&rcIconFrame, 
					&rcIconFrame
					) == S_OK
				);

			if( _pmid.m_pIcon == NULL || _pmid.m_pIcon->IsEmpty() )
			{
				COLORREF clrCheck = ::GetSysColor( COLOR_BTNTEXT );
				int x = rcIcon.left + rcIcon.Width()/2 - 3;
				int y = rcIcon.top + rcIcon.Height()/2 - 3;
				if( _pmid.m_bPushedCheck )
				{
					ASSERT( !_pmid.m_bPushedRadio );
					if( _pmid.m_bIndeterminate )
						stat_PaintIndeterminatedSquare( dc, x, y, clrCheck );
					else
						stat_PaintCheckMark( dc, x, y, clrCheck );
				}
				else
				{
					ASSERT( !_pmid.m_bPushedCheck );
					stat_PaintRadioDot( dc, x, y, clrCheck );
				}
			}
		}

		if(		_pmid.m_pIcon != NULL 
			&&	(! _pmid.m_pIcon->IsEmpty()) 
			)
		{
			if(		_pmid.m_bSelected
				&&	_pmid.m_bEnabled
				&&	(! ( _pmid.m_bPushedCheck || _pmid.m_bPushedRadio ) )
				)
			{
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWnd,
						dc.GetSafeHdc(), 
						TP_BUTTON, 
						TS_HOT, 
						&rcIconFrame, 
						&rcIconFrame
						) == S_OK
					);
			}
			_pmid.m_pIcon->Paint(
				this,
				dc,
				rcIcon.left,
				rcIcon.top,
				-1,
				-1,
				_pmid.m_bEnabled
					? CExtCmdIcon::__PAINT_NORMAL
					: CExtCmdIcon::__PAINT_DISABLED
				);
		}
		else
		{
			PaintSysCmdGlyph(
				dc,
				_pmid.m_nHelperSysCmdID,
				rcIcon,
				_pmid.m_bSelected,
				false,
				_pmid.m_bEnabled
				);
		}
		
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

void CExtPaintManagerNativeXP::PaintMenuTearOffButton(
	CDC & dc,
	const RECT & rectButton,
	bool bPushed,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	if( IsHighContrast() )
	{
		CExtPaintManager::PaintMenuTearOffButton( dc, rectButton, bPushed, bHover, pHelperSrc, lParam );
		return;
	}
bool bThemedApproachOK = false;
HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}
	if( hWnd != NULL && g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_REBAR ) != NULL )
	{
		CRect rc = rectButton;
		HRESULT hr =
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd,
				dc.GetSafeHdc(), 
				RP_BACKGROUND, 
				0,
				&rc, 
				&rc
				);
		bThemedApproachOK = ( hr == S_OK ) ? true : false;
		if( bThemedApproachOK )
		{
			rc.DeflateRect( rc.Width() / 8, 2 );
			if( rc.left < rc.right && rc.top < rc.bottom )
			{
				hr =
					g_PaintManager.m_UxTheme.DrawBackground(
						hWnd,
						dc.GetSafeHdc(), 
						RP_GRIPPER, 
						0,
						&rc, 
						&rc
						);
				bThemedApproachOK = ( hr == S_OK ) ? true : false;
			}
		}
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
	if( bThemedApproachOK )
		return;
	CExtPaintManager::PaintMenuTearOffButton( dc, rectButton, bPushed, bHover, pHelperSrc, lParam );
}

bool CExtPaintManagerNativeXP::PaintSysCmdGlyph_UX(
	HWND hWnd,
	CDC & dc,
	UINT nHelperSysCmdID,
	const RECT & rcItem,
	bool bHover,
	bool bPushed,
	bool bEnabled,
	COLORREF clrForceGlyphColor // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( g_PaintManager.m_UxTheme.IsControlsThemed() );
	ASSERT( hWnd != NULL && ::IsWindow( hWnd ) );
//	ASSERT( g_PaintManager.m_UxTheme.m_hUxTheme != NULL ); // menu class should be already opened
	hWnd;
	bHover;
	bPushed;
	clrForceGlyphColor;
	if( ! ( rcItem.left < rcItem.right && rcItem.top < rcItem.bottom ) )
		return true;
int nPartID = 0, nStateID = 0;
	switch( nHelperSysCmdID )
	{
	case SC_CLOSE:
		nPartID = MENU_SYSTEMCLOSE;
		nStateID = bEnabled ? MSYSC_NORMAL : MSYSC_DISABLED;
	break;
	case SC_RESTORE:
		nPartID = MENU_SYSTEMRESTORE;
		nStateID = bEnabled ? MSYSR_NORMAL : MSYSR_DISABLED;
	break;
	case SC_MINIMIZE:
		nPartID = MENU_SYSTEMMINIMIZE;
		nStateID = bEnabled ? MSYSMN_NORMAL : MSYSMN_DISABLED;
	break;
	case SC_MAXIMIZE:
		nPartID = MENU_SYSTEMMAXIMIZE;
		nStateID = bEnabled ? MSYSMX_NORMAL : MSYSMX_DISABLED;
	break;
//	case SC_CONTEXTHELP:
//	break;
	default:
		return false;
	} // switch( nHelperSysCmdID )
	VERIFY( 
		g_PaintManager.m_UxTheme.DrawBackground(
			hWnd,
			dc.GetSafeHdc(), 
			nPartID, 
			nStateID, 
			&rcItem, 
			&rcItem
			) == S_OK
		);
	return true;
}

bool CExtPaintManagerNativeXP::PaintMenuItemIcon_UX(
	HWND hWnd,
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( g_PaintManager.m_UxTheme.IsControlsThemed() );
	ASSERT( hWnd != NULL && ::IsWindow( hWnd ) );
//	ASSERT( g_PaintManager.m_UxTheme.m_hUxTheme != NULL ); // menu class should be already opened
	if( _pmid.m_rcItem.IsRectEmpty() )
		return true;
CSize sizeIcon( __EXT_MENU_ICON_CX, __EXT_MENU_ICON_CY );
	if( _pmid.m_pIcon != NULL && (! _pmid.m_pIcon->IsEmpty()) )
	{
		sizeIcon = _pmid.m_pIcon->GetSize();
		if(		sizeIcon.cx <= 18 
			&&	sizeIcon.cy < sizeIcon.cx 
			)
			sizeIcon.cy = sizeIcon.cx;
	}
CRect rcIcon( _pmid.m_rcItem.TopLeft(), sizeIcon );
CExtPopupMenuWnd * pPopup = NULL;
bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		pPopup = DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}
	if( bRTL )
		rcIcon.OffsetRect( _pmid.m_rcItem.Width() - sizeIcon.cx, 0 );
	if( _pmid.m_bXtraMark )
	{
		rcIcon.OffsetRect(
			( ( _pmid.m_nIconAreaWidth/2 - rcIcon.Width() ) / 2 + _pmid.m_nIconAreaWidth/2 - 2 )
				* (bRTL ? (-1) : 1),
			( _pmid.m_rcItem.Height() - rcIcon.Height() ) / 2
			);
		if( _pmid.m_bXtraChecked )
		{
			CRect rcXtraMark(
				bRTL ? (_pmid.m_rcItem.right - _pmid.m_rcItem.Height() - 1) : (_pmid.m_rcItem.left + 1),
				_pmid.m_rcItem.top + 1,
				bRTL ? (_pmid.m_rcItem.right - 1) : (_pmid.m_rcItem.left + _pmid.m_rcItem.Height() - 1),
				_pmid.m_rcItem.bottom - 1
				);
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					hWnd,
					dc.GetSafeHdc(), 
					MENU_POPUPCHECKBACKGROUND, 
					_pmid.m_bEnabled ? MCB_NORMAL : MCB_DISABLED, 
					&rcXtraMark, 
					&rcXtraMark
					) == S_OK
				);
			COLORREF clrCheck = ::GetSysColor( COLOR_BTNTEXT );
			int x = rcXtraMark.left + rcXtraMark.Width()/2 - 3;
			int y = rcXtraMark.top + rcXtraMark.Height()/2 - 3;
			ASSERT( !_pmid.m_bPushedRadio );
			stat_PaintCheckMark( dc, x, y, clrCheck );
		}
	}
	else
	{
		rcIcon.OffsetRect(
			( ( _pmid.m_nIconAreaWidth - rcIcon.Width() ) / 2 ) * (bRTL ? (-1) : 1),
			( _pmid.m_rcItem.Height() - rcIcon.Height() ) / 2
			);
	}

CRect rcIconFrame(rcIcon);
	rcIconFrame.InflateRect(
		__EXT_MENU_IMG_RECT_OUT_GAP,
		__EXT_MENU_IMG_RECT_OUT_GAP
		);

	if( _pmid.m_bPushedCheck || _pmid.m_bIndeterminate || _pmid.m_bPushedRadio )
	{
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd,
				dc.GetSafeHdc(), 
				MENU_POPUPCHECKBACKGROUND, 
				_pmid.m_bEnabled ? MCB_NORMAL : MCB_DISABLED, 
				&rcIconFrame, 
				&rcIconFrame
				) == S_OK
			);

		if( _pmid.m_pIcon == NULL || _pmid.m_pIcon->IsEmpty() )
		{
			if( ! _pmid.m_bIndeterminate )
			{
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWnd,
						dc.GetSafeHdc(), 
						MENU_POPUPCHECK, 
						_pmid.m_bPushedCheck
							?
								//(	_pmid.m_bIndeterminate
								//		?	MC_BULLETDISABLED
								//		:
											( _pmid.m_bEnabled ? MC_CHECKMARKNORMAL : MC_CHECKMARKDISABLED )
								//)
							:	( _pmid.m_bEnabled ? MC_BULLETNORMAL : MC_BULLETDISABLED )
							, 
						&rcIcon, 
						&rcIcon
						) == S_OK
					);
			}
			else
			{
				COLORREF clrCheck = GetColor( CLR_MENUTEXT_OUT, _pmid.m_pHelperSrc, _pmid.m_lParam );
				int x = rcIcon.left + rcIcon.Width()/2 - 3;
				int y = rcIcon.top + rcIcon.Height()/2 - 3;
				stat_PaintIndeterminatedSquare( dc, x, y, clrCheck );
			}
		}
	}

	if(		_pmid.m_pIcon != NULL 
		&&	(! _pmid.m_pIcon->IsEmpty()) 
		)
	{
		if(		_pmid.m_bSelected
			&&	_pmid.m_bEnabled
			&&	(! ( _pmid.m_bPushedCheck || _pmid.m_bPushedRadio ) )
			)
		{
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					hWnd,
					dc.GetSafeHdc(), 
					MENU_POPUPCHECKBACKGROUND, 
					_pmid.m_bEnabled ? MCB_NORMAL : MCB_DISABLED, 
					&rcIconFrame, 
					&rcIconFrame
					) == S_OK
				);
		}
		_pmid.m_pIcon->Paint(
			this,
			dc,
			rcIcon.left,
			rcIcon.top,
			-1,
			-1,
			_pmid.m_bEnabled
				? CExtCmdIcon::__PAINT_NORMAL
				: CExtCmdIcon::__PAINT_DISABLED
			);
	}
	else
	{
		if( ! PaintSysCmdGlyph_UX(
				hWnd,
				dc,
				_pmid.m_nHelperSysCmdID,
				rcIcon,
				_pmid.m_bSelected,
				false,
				_pmid.m_bEnabled
				)
			)
			PaintSysCmdGlyph(
				dc,
				_pmid.m_nHelperSysCmdID,
				rcIcon,
				_pmid.m_bSelected,
				false,
				_pmid.m_bEnabled
				);
	}

	return true;
}

void CExtPaintManagerNativeXP::PaintMenuItemInPlaceWnd(
	CDC & dc,
	CExtPaintManager::PAINTMENUITEMDATA & _pmid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintMenuItemInPlaceWnd(
			dc,
			_pmid
			);
		return;
	}
	
	if(		_pmid.m_rcItem.IsRectEmpty() 
		||	_pmid.m_rcInPlaceWnd.IsRectEmpty()
		)
		return;

bool bRTL = false;
	if( _pmid.m_pHelperSrc != NULL )
	{
		ASSERT_VALID( _pmid.m_pHelperSrc );
		CExtPopupMenuWnd * pPopup =
			DYNAMIC_DOWNCAST( CExtPopupMenuWnd, _pmid.m_pHelperSrc );
		if( pPopup != NULL )
			bRTL = pPopup->OnQueryLayoutRTL();
	}

CRect rcInPlaceWndOuterBorder( _pmid.m_rcInPlaceWnd );
	rcInPlaceWndOuterBorder.InflateRect( 2, 2, 1, 2 );
INT nDD = GetDropDownButtonWidth( _pmid.m_pHelperSrc, _pmid.m_lParam );
CRect rcBk( rcInPlaceWndOuterBorder );
	if( _pmid.m_bPopup )
		rcBk.right += nDD;
	dc.FillSolidRect(
		&rcBk,
		::GetSysColor( _pmid.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE )
		);
	if( _pmid.m_bPopup )
	{
		rcInPlaceWndOuterBorder.right += nDD;
		if( _pmid.m_bEnabled )
		{
			PAINTCOMBOFRAMEDATA _pcfd(
				_pmid.m_pHelperSrc,
				rcInPlaceWndOuterBorder,
				_pmid.m_bSelected, // hover
				_pmid.m_bSelected && _pmid.m_bHelperPopupDropped, // pushed
				_pmid.m_bEnabled
				);
			_pcfd.m_bNoTopOuterBorder = true;
			PaintComboFrame( dc, _pcfd );
		}
		else
		{
			CRect rcBorder( rcInPlaceWndOuterBorder );
			rcBorder.DeflateRect( 2, 2 );
			COLORREF clrBorder = ::GetSysColor( COLOR_WINDOW );
			dc.Draw3dRect(
				&rcBorder,
				clrBorder,
				clrBorder
				);
			// paint arrow down
			CRect rcBtn( rcBorder );
			rcBtn.left = rcBtn.right - nDD;
			glyph_t * pGlyph = g_DockingCaptionGlyphs[ __DCBT_ARROW_SMALL_DOWN ];
			ASSERT( pGlyph != NULL );
			COLORREF ColorValues[2] =
			{
				RGB(0,0,0),
				::GetSysColor( COLOR_BTNTEXT )
			};
			PaintGlyphCentered(
				dc,
				rcBtn,
				*pGlyph,
				ColorValues
				);
		} // else from if( _pmid.m_bEnabled )
		rcInPlaceWndOuterBorder.DeflateRect( 1, 1 );
		rcInPlaceWndOuterBorder.right -= nDD;
	} // if( _pmid.m_bPopup )
	else
	{
		if( _pmid.m_bEnabled )
		{
			CRect rcItem( rcInPlaceWndOuterBorder );
			rcItem.DeflateRect(3,3);
			PAINTCONTROLFRAMEDATA _pcfd(
					_pmid.m_pHelperSrc,
					rcItem,
					true,
					_pmid.m_bSelected,
					_pmid.m_bEnabled,
					false,
					false
					);
			PaintControlFrame( dc, _pcfd );
		}
		else
		{
			CRect rcBorder( rcInPlaceWndOuterBorder );
			rcBorder.DeflateRect( 2, 2 );
			COLORREF clrBorder = ::GetSysColor( COLOR_WINDOW );
			dc.Draw3dRect(
				&rcBorder,
				clrBorder,
				clrBorder
				);
		}
	} // else from if( _pmid.m_bPopup )

	if( !_pmid.m_strInPlaceText.IsEmpty() )
	{
		rcInPlaceWndOuterBorder.DeflateRect( 5, 2, 2, 2 );
		
		COLORREF clrOldText =
			dc.SetTextColor(
				GetColor( 
					_pmid.m_bEnabled 
						? COLOR_WINDOWTEXT 
						: COLOR_3DSHADOW, 
					_pmid.m_pHelperSrc, 
					_pmid.m_lParam 
					)
				);
		
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		
		CFont * pRenderFont =
			( _pmid.m_hCustomFont == NULL )
				? (&m_FontNormal)
				: CFont::FromHandle(_pmid.m_hCustomFont)
				;
		CFont * pOldFont = dc.SelectObject( pRenderFont );
		ASSERT( pOldFont != NULL );
	
		DWORD dwDtAlignFlags = bRTL ? (DT_RIGHT|DT_RTLREADING) : DT_LEFT;
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(_pmid.m_strInPlaceText), _pmid.m_strInPlaceText.GetLength(),
			&rcInPlaceWndOuterBorder,
			DT_SINGLELINE|DT_NOPREFIX|DT_TOP|dwDtAlignFlags, 0
			);

		dc.SelectObject( pOldFont );
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldText );
	}
}

CSize CExtPaintManager::GetToolBarButtonOffset( BOOL bHorz ) const
{
	ASSERT_VALID( this );
	bHorz;
	return
		CSize(
			0, // bHorz ? 0 : 1,
			0
			);
}

void CExtPaintManager::PaintIcon(
	CDC & dc,
	bool bHorz,
	CExtCmdIcon * pIcon,
	const CRect & rcClient,
	bool bPushed,
	bool bEnabled,
	bool bHover, // = false
	INT nIconAlignment, // = 0
	bool bRTL, // = false
	CRect * prcIcon, // = NULL
	CRect * prcText, // = NULL
	CRect * prcIconMargins, // = NULL
	COLORREF clrCustomAccentEffectForIcon // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bHorz;
	bRTL;
	
	if( pIcon == NULL || pIcon->IsEmpty() )
	{
		if( prcIcon != NULL )
			*prcIcon = CRect( 0, 0, 0, 0 );
		if( prcText != NULL )
			*prcText = rcClient;
		return;
	}

CSize _sizeIcon = pIcon->GetSize();
	ASSERT( _sizeIcon.cx > 0 && _sizeIcon.cy > 0 );

CRect rcIconMargins( 0, 0, 0, 0 );
	if( prcIconMargins != NULL )
		rcIconMargins = *prcIconMargins;

CRect rcTextLocation( rcClient );
CRect rcIconLocation( 0, 0, _sizeIcon.cx, _sizeIcon.cy );

	if( bHorz )
	{
		rcIconLocation.OffsetRect( 
			rcClient.left, 
			rcClient.top 
			);

		if(	( _sizeIcon.cx + rcIconMargins.left + rcIconMargins.right ) > rcClient.Width() )
		{
			nIconAlignment &= ~__ALIGN_HORIZ_MASK;
			nIconAlignment |= __ALIGN_HORIZ_CENTER;
		}

		if(	( _sizeIcon.cy + rcIconMargins.top + rcIconMargins.bottom ) > rcClient.Height() )
		{
			nIconAlignment &= ~__ALIGN_VERT_MASK;
			nIconAlignment |= __ALIGN_VERT_CENTER;
		}

		if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_RIGHT )
		{
			rcIconLocation.OffsetRect( rcClient.right - rcIconLocation.right - rcIconMargins.right, 0 );
			rcTextLocation.right = rcIconLocation.left - rcIconMargins.left;
		}
		else if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
		{
			rcIconLocation.OffsetRect( ( rcClient.right - rcIconLocation.right ) / 2 + rcIconMargins.left - rcIconMargins.right, 0 );
		}
		else if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_LEFT )
		{
			rcIconLocation.OffsetRect( rcIconMargins.left, 0 );
			rcTextLocation.left = rcIconLocation.right + rcIconMargins.right;
		}

		if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_TOP )
		{
			rcIconLocation.OffsetRect( 0, rcIconMargins.top );
			if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
				rcTextLocation.top = rcIconLocation.bottom + rcIconMargins.bottom;
		}
		else if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_CENTER )
		{
			rcIconLocation.OffsetRect( 0, ( rcClient.bottom - rcIconLocation.bottom ) / 2 + rcIconMargins.top - rcIconMargins.bottom );
		}
		else if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		{
			rcIconLocation.OffsetRect( 0, rcClient.bottom - rcIconLocation.bottom - rcIconMargins.bottom );
			if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
				rcTextLocation.bottom = rcIconLocation.top - rcIconMargins.top;
		}
	}
	else
	{
		rcIconLocation.OffsetRect( 
			rcClient.right - rcIconLocation.Width(), 
			rcClient.top 
			);

		if(	( _sizeIcon.cy + rcIconMargins.top + rcIconMargins.bottom ) > rcClient.Height() )
		{
			nIconAlignment &= ~__ALIGN_HORIZ_MASK;
			nIconAlignment |= __ALIGN_HORIZ_CENTER;
		}

		if(	( _sizeIcon.cx + rcIconMargins.left + rcIconMargins.right ) > rcClient.Width() )
		{
			nIconAlignment &= ~__ALIGN_VERT_MASK;
			nIconAlignment |= __ALIGN_VERT_CENTER;
		}

		if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_RIGHT )
		{
			rcIconLocation.OffsetRect( 0, rcClient.bottom - rcIconLocation.bottom - rcIconMargins.bottom );
			rcTextLocation.bottom = rcIconLocation.top - rcIconMargins.top;
		}
		else if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
		{
			rcIconLocation.OffsetRect( 0, ( rcClient.bottom - rcIconLocation.bottom ) / 2 + rcIconMargins.top - rcIconMargins.bottom );
		}
		else if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_LEFT )
		{
			rcIconLocation.OffsetRect( 0, rcIconMargins.top );
			rcTextLocation.top = rcIconLocation.bottom;
		}
		
		if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_TOP )
		{
			rcIconLocation.OffsetRect( rcIconMargins.right, 0 );
			if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
				rcTextLocation.right = rcIconLocation.left;
		}
		else if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_CENTER )
		{
			rcIconLocation.OffsetRect( ( rcClient.left - rcIconLocation.left ) / 2 + rcIconMargins.right - rcIconMargins.left, 0 );
		}
		else if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		{
			rcIconLocation.OffsetRect( rcClient.left - rcIconLocation.left - rcIconMargins.left, 0 );
			if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
				rcTextLocation.left = rcIconLocation.right;
		}
	}

	if(		(nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER 
		&&	(nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_CENTER
		)
		rcTextLocation.SetRectEmpty();

	if(		rcTextLocation.top >= rcTextLocation.bottom
		||	rcTextLocation.left >= rcTextLocation.right
		)
		rcTextLocation.SetRectEmpty();
    
	// if button is pressed then press the icon also
	if( bPushed )
	{
		CSize sizePushedOffset = GetPushedOffset();
		rcIconLocation.OffsetRect( sizePushedOffset.cx, sizePushedOffset.cy );
	}
	
	if(		( ! bEnabled )
		&&	clrCustomAccentEffectForIcon != COLORREF(-1L)
		)
	{
		pIcon->PaintAccentEmboss(
			this,
			dc.GetSafeHdc(),
			clrCustomAccentEffectForIcon,
			rcIconLocation.left,
			rcIconLocation.top
			);
	}
	else
	{
		pIcon->Paint(
			this,
			dc.GetSafeHdc(),
			rcIconLocation.left,
			rcIconLocation.top,
			-1,
			-1,
			(! bEnabled )
				? CExtCmdIcon::__PAINT_DISABLED
				: bPushed
					? CExtCmdIcon::__PAINT_PRESSED
					: bHover
						? CExtCmdIcon::__PAINT_HOVER
						: CExtCmdIcon::__PAINT_NORMAL
			);
	}


	if( prcIcon != NULL )
		*prcIcon = rcIconLocation;
	if( prcText != NULL )
		*prcText = rcTextLocation;
}

void CExtPaintManagerXP::PaintSeparator(
	CDC & dc,
	const RECT & rectItem,
	bool bHorz,
	bool bTransparentBk,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rectItem2(rectItem);
	if( !bTransparentBk )
		dc.FillSolidRect(
			rectItem2,
			GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam )
			);
	if( bHorz )
	{
		rectItem2.left =
			rectItem2.left + rectItem2.Width()/2 - 1;
		rectItem2.right =
			rectItem2.left + 1;
		rectItem2.DeflateRect( 0, 1 );
	}
	else
	{
		rectItem2.top =
			rectItem2.top + rectItem2.Height()/2 - 1;
		rectItem2.bottom =
			rectItem2.top + 1;
		rectItem2.DeflateRect( 1, 0 );
	}
	dc.FillSolidRect(
		&rectItem2,
		GetColor( XPCLR_SEPARATOR, pHelperSrc, lParam )
		);
}

void CExtPaintManager::PaintSeparator(
	CDC & dc,
	const RECT & rectItem,
	bool bHorz,
	bool bTransparentBk,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rectItem2(rectItem);
	if( !bTransparentBk )
		dc.FillSolidRect(
			rectItem2,
			GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam )
			);

COLORREF clrBtnShadow =  GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam );
COLORREF clrBtnHilight = GetColor( CLR_3DHILIGHT_OUT, pHelperSrc, lParam );
	if( bHorz )
	{
		int x = (rectItem2.left + rectItem2.right) / 2;
		rectItem2.left = x-1; rectItem2.right = x+1;
		rectItem2.DeflateRect( 0, 0, 0, 1 );
		dc.Draw3dRect(
			rectItem2,
			clrBtnShadow,
			clrBtnHilight
		);
	} // if( bHorz )
	else
	{
		int y = (rectItem2.top+rectItem2.bottom)/2;
		rectItem2.top = y-1; rectItem2.bottom = y+1;
		rectItem2.DeflateRect( 0, 0, 1, 0 );
		dc.Draw3dRect(
			rectItem2,
			clrBtnShadow,
			clrBtnHilight
			);
	} // else from if( bHorz )
}

void CExtPaintManagerNativeXP::PaintSeparator(
	CDC & dc,
	const RECT & rectItem,
	bool bHorz,
	bool bTransparentBk,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintSeparator(
			dc,
			rectItem,
			bHorz,
			bTransparentBk,
			pHelperSrc,
			lParam
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )

HWND hWnd = NULL;
CWnd * pWnd =
		DYNAMIC_DOWNCAST(
			CWnd,
			pHelperSrc
			);
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}
	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_TOOLBAR ) != NULL )
	{
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd,
				dc.GetSafeHdc(), 
				bHorz ? TP_SEPARATOR : TP_SEPARATORVERT, 
				TS_NORMAL, 
				&rectItem, 
				&rectItem
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

CExtPaintManager::PAINTCONTROLFRAMEDATA::PAINTCONTROLFRAMEDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_rcClient( 0,0,0,0 )
	, m_bFlat( false )
	, m_bHover( false )
	, m_bEnabled( false )
	, m_bFocus( false )
	, m_bReadOnly( false )
{
}

CExtPaintManager::PAINTCONTROLFRAMEDATA::PAINTCONTROLFRAMEDATA(
	CObject * pHelperSrc,
	const RECT & rcClient,
	bool bFlat,
	bool bHover,
	bool bEnabled,
	bool bFocus,
	bool bReadOnly
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_rcClient( rcClient )
	, m_bFlat( bFlat )
	, m_bHover( bHover )
	, m_bEnabled( bEnabled )
	, m_bFocus( bFocus )
	, m_bReadOnly( bReadOnly )
{
}

void CExtPaintManager::PaintControlFrame(
	CDC & dc,
	CExtPaintManager::PAINTCONTROLFRAMEDATA & _pcfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

COLORREF clrForceInner = COLORREF( -1L );
COLORREF clrForceOuter = COLORREF( -1L );
	if( _pcfd.m_pHelperSrc != NULL )
	{
#ifndef __EXT_MFC_NO_DURATIONWND
		if( _pcfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtDurationWnd ) ) )
		{
			CExtDurationWnd * pWnd = DYNAMIC_DOWNCAST( CExtDurationWnd, _pcfd.m_pHelperSrc );
			if( pWnd != NULL )
			{
				ASSERT_VALID( pWnd );
				clrForceInner = pWnd->GetBkColor();
			}
		}
#endif // __EXT_MFC_NO_DURATIONWND
		if( _pcfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtEdit ) ) )
		{
			CExtEdit * pWnd = DYNAMIC_DOWNCAST( CExtEdit, _pcfd.m_pHelperSrc );
			if( pWnd != NULL )
			{
				ASSERT_VALID( pWnd );
				clrForceInner = pWnd->GetBkColor();
				CWnd * pWndParent = pWnd->GetParent();
				if( pWndParent != NULL )
				{
					ASSERT_VALID( pWndParent );
					if( pWndParent->IsKindOf( RUNTIME_CLASS( CExtResizableDialog ) ) )
					{
						CExtResizableDialog * pDlg = DYNAMIC_DOWNCAST( CExtResizableDialog, pWndParent );
						if( pDlg != NULL )
							clrForceOuter = pDlg->GetBkColor();
					}
					else if( pWndParent->IsKindOf( RUNTIME_CLASS( CDialog ) ) )
					{
						clrForceOuter = ::GetSysColor( COLOR_3DFACE );
					}
				}
			}
		}
	} // if( _pcfd.m_pHelperSrc != NULL )
COLORREF clrInner =
	( clrForceInner != COLORREF( -1L ) ) 
		? clrForceInner 
		: GetColor( COLOR_WINDOW, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
COLORREF clrIn1, clrIn2;
	clrIn1 = clrIn2 = GetColor( CLR_3DFACE_OUT, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
COLORREF clrOut1, clrOut2;
	clrOut1 = clrOut2 =
		( clrForceOuter != COLORREF( -1L ) ) 
			? clrForceOuter 
			: GetColor( CLR_3DFACE_OUT, _pcfd.m_pHelperSrc, _pcfd.m_lParam )
			;
	if( _pcfd.m_bEnabled )
	{
		if( _pcfd.m_bHover && _pcfd.m_bFocus )
		{
				clrIn1 = GetColor( COLOR_3DFACE, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
				clrIn2 = GetColor( COLOR_3DDKSHADOW, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
				clrOut1 = GetColor( COLOR_3DHILIGHT, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
				clrOut2 = GetColor( COLOR_3DSHADOW, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
		}
		else
		{
			if( _pcfd.m_bHover || _pcfd.m_bFocus )
			{
				clrIn1 = GetColor( COLOR_3DHILIGHT, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
				clrIn2 = GetColor( COLOR_3DSHADOW, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
			}
			else
				clrIn1 = clrIn2 =
					dc.GetNearestColor(
						stat_HLS_Adjust(
							GetColor( COLOR_3DFACE, _pcfd.m_pHelperSrc, _pcfd.m_lParam ),
							0.0, -0.1, 0.0
							)
						);
		}
	}
CRect rectItem( _pcfd.m_rcClient );
	rectItem.DeflateRect( 1, 1 );
	dc.Draw3dRect( rectItem, clrInner, clrInner );
	if( ! (_pcfd.m_bHover || _pcfd.m_bFocus ) )
		rectItem.InflateRect( 1, 1 );
	dc.Draw3dRect( rectItem, clrIn2, clrIn1 );
	rectItem.InflateRect( 1, 1 );
	dc.Draw3dRect( rectItem, clrOut2, clrOut1 );
}

void CExtPaintManagerXP::PaintControlFrame(
	CDC & dc,
	CExtPaintManager::PAINTCONTROLFRAMEDATA & _pcfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrForceInner = COLORREF( -1L );
COLORREF clrForceOuter = COLORREF( -1L );
	if( _pcfd.m_pHelperSrc != NULL )
	{
#ifndef __EXT_MFC_NO_DURATIONWND
		if( _pcfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtDurationWnd ) ) )
		{
			CExtDurationWnd * pWnd = DYNAMIC_DOWNCAST( CExtDurationWnd, _pcfd.m_pHelperSrc );
			if( pWnd != NULL )
			{
				ASSERT_VALID( pWnd );
				clrForceInner = pWnd->GetBkColor();
			}
		}
#endif // __EXT_MFC_NO_DURATIONWND
		if( _pcfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtEdit ) ) )
		{
			CExtEdit * pWnd = DYNAMIC_DOWNCAST( CExtEdit, _pcfd.m_pHelperSrc );
			if( pWnd != NULL )
			{
				ASSERT_VALID( pWnd );
				clrForceInner = pWnd->GetBkColor();
				CWnd * pWndParent = pWnd->GetParent();
				if( pWndParent != NULL )
				{
					ASSERT_VALID( pWndParent );
					if( pWndParent->IsKindOf( RUNTIME_CLASS( CExtResizableDialog ) ) )
					{
						CExtResizableDialog * pDlg = DYNAMIC_DOWNCAST( CExtResizableDialog, pWndParent );
						if( pDlg != NULL )
							clrForceOuter = pDlg->GetBkColor();
					}
					else if( pWndParent->IsKindOf( RUNTIME_CLASS( CDialog ) ) )
					{
						clrForceOuter = ::GetSysColor( COLOR_3DFACE );
					}
				}
			}
		}
	} // if( _pcfd.m_pHelperSrc != NULL )
COLORREF clrInner =
	( clrForceInner != COLORREF( -1L ) ) 
		? clrForceInner 
		: GetColor( COLOR_WINDOW, _pcfd.m_pHelperSrc, _pcfd.m_lParam )
		;
COLORREF clrIn =
		GetColor( CLR_3DFACE_OUT, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
COLORREF clrOut =
		( clrForceOuter != COLORREF( -1L ) ) 
			? clrForceOuter 
			: GetColor( CLR_3DFACE_OUT, _pcfd.m_pHelperSrc, _pcfd.m_lParam )
			;
	if( _pcfd.m_bEnabled )
	{
		if( _pcfd.m_bHover )
		{
			if( _pcfd.m_bFocus )
				clrIn = clrOut = GetColor( XPCLR_HILIGHT_BORDER, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
			else
				clrIn = GetColor( XPCLR_HILIGHT_BORDER, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
		} // if( _pcfd.m_bHover )
		else
		{
			if( _pcfd.m_bFocus )
				clrIn = GetColor( XPCLR_HILIGHT_BORDER, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
			else
				clrIn = dc.GetNearestColor( stat_HLS_Adjust( clrIn, 0.0, -0.1, 0.0 ) );
		} // else from if( _pcfd.m_bHover )
	} // else from if( _pcfd.m_bEnabled )
CRect rectItem( _pcfd.m_rcClient );
	rectItem.DeflateRect( 1, 1 );
	dc.Draw3dRect( rectItem, clrInner, clrInner );
	if( !(_pcfd.m_bHover || _pcfd.m_bFocus) )
		rectItem.InflateRect( 1, 1 );
	dc.Draw3dRect( rectItem, clrIn, clrIn );
	rectItem.InflateRect( 1, 1 );
	dc.Draw3dRect( rectItem, clrOut, clrOut );
}

void CExtPaintManagerNativeXP::PaintControlFrame(
	CDC & dc,
	CExtPaintManager::PAINTCONTROLFRAMEDATA & _pcfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintControlFrame( dc, _pcfd );
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, _pcfd.m_pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}
	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_EDIT ) != NULL )
	{
		CRect rcItem( _pcfd.m_rcClient );
		INT nStateID = 
				(!_pcfd.m_bEnabled)
					? ETS_DISABLED
					: _pcfd.m_bReadOnly
						? ETS_READONLY
						: _pcfd.m_bFocus
							? ETS_FOCUSED
							: _pcfd.m_bHover
								? ETS_HOT
								: ETS_NORMAL;
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd,
				dc.GetSafeHdc(), 
				EP_EDITTEXT, 
				nStateID, 
				&rcItem, 
				&rcItem
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

CExtPaintManager::PAINTCOMBOFRAMEDATA::PAINTCOMBOFRAMEDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_rcClient( 0,0,0,0 )
	, m_bHover( false )
	, m_bPushed( false )
	, m_bEnabled( false )
	, m_clrForceNABorder( COLORREF(-1L) )
	, m_clrForceNAContent( COLORREF(-1L) )
	, m_clrForceActiveOuterBorder( COLORREF(-1L) )
	, m_bNoTopOuterBorder( false )
	, m_bRightText( false )
{
}

CExtPaintManager::PAINTCOMBOFRAMEDATA::PAINTCOMBOFRAMEDATA(
	CObject * pHelperSrc,
	const RECT & rcClient,
	bool bHover,
	bool bPushed,
	bool bEnabled,
	bool bRightText // = false
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_rcClient( rcClient )
	, m_bHover( bHover )
	, m_bPushed( bPushed )
	, m_bEnabled( bEnabled )
	, m_clrForceNABorder( COLORREF(-1L) )
	, m_clrForceNAContent( COLORREF(-1L) )
	, m_clrForceActiveOuterBorder( COLORREF(-1L) )
	, m_bNoTopOuterBorder( false )
	, m_bRightText( bRightText )
{
}

void CExtPaintManager::PaintComboFrame(
	CDC & dc,
	CExtPaintManager::PAINTCOMBOFRAMEDATA & _pcfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtComboBoxBase * pWndCombo = NULL;
	if(		_pcfd.m_pHelperSrc != NULL 
		&&	_pcfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtComboBoxBase ) )
		)
	{
		pWndCombo = STATIC_DOWNCAST( CExtComboBoxBase, _pcfd.m_pHelperSrc );
		ASSERT_VALID( pWndCombo );
	}
COLORREF clrHilightOut =
		( _pcfd.m_clrForceNABorder == COLORREF(-1L) || _pcfd.m_bPushed || _pcfd.m_bHover )
			? GetColor( CLR_3DHILIGHT_OUT, _pcfd.m_pHelperSrc, _pcfd.m_lParam )
			: _pcfd.m_clrForceNABorder
			;
COLORREF clrFaceOut =
		( _pcfd.m_clrForceNABorder == COLORREF(-1L) || _pcfd.m_bPushed || _pcfd.m_bHover )
			? GetColor( CLR_3DFACE_OUT, _pcfd.m_pHelperSrc, _pcfd.m_lParam )
			: _pcfd.m_clrForceNABorder
			;
COLORREF clrTopLeft, clrBottomRight;
	if( _pcfd.m_clrForceNAContent == COLORREF(-1L) )
	{
		clrTopLeft = clrBottomRight = clrFaceOut;
		clrFaceOut = dc.GetNearestColor( stat_HLS_Adjust( GetColor( CLR_3DFACE_OUT, _pcfd.m_pHelperSrc, _pcfd.m_lParam ), 0.0, -0.1, 0.0 ) );
	}
	else
		clrTopLeft = clrBottomRight = _pcfd.m_clrForceNAContent;
	if( _pcfd.m_bPushed || _pcfd.m_bHover )
	{
		clrTopLeft     = GetColor( CLR_3DSHADOW_IN, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
		clrBottomRight = GetColor( CLR_3DHILIGHT_IN, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
	}
COLORREF clrWindow = COLORREF( -1L );
	if(		_pcfd.m_clrForceNABorder == COLORREF(-1L) 
		||	_pcfd.m_bPushed 
		|| _pcfd.m_bHover 
		)
	{
		if( pWndCombo != NULL )
			clrWindow = pWndCombo->GetBkColor();
		if( clrWindow == COLORREF(-1L) )
		{
			COLORREF clrSysBk = GetColor( _pcfd.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
			clrWindow = clrSysBk;
		}
	}
	else
		clrWindow = _pcfd.m_clrForceNABorder;
 CRect rectClient( _pcfd.m_rcClient );
	dc.Draw3dRect( &rectClient, clrTopLeft, clrBottomRight );
	rectClient.DeflateRect(1,1);
	dc.Draw3dRect(
		&rectClient,
		_pcfd.m_bEnabled ? clrFaceOut : clrHilightOut,
		_pcfd.m_bEnabled ? clrFaceOut : clrHilightOut
		);
INT nDD = GetDropDownButtonWidth( _pcfd.m_pHelperSrc, _pcfd.m_lParam );
CRect rcDDButton( rectClient );
	if( _pcfd.m_bRightText )
		rcDDButton.right = rcDDButton.left + nDD;
	else
		rcDDButton.left = rcDDButton.right - nDD;
	if( _pcfd.m_bHover || _pcfd.m_bPushed )
		dc.Draw3dRect( &rcDDButton, clrFaceOut, clrFaceOut );
	rcDDButton.DeflateRect(1,1);
PAINTPUSHBUTTONDATA _ppbd(
		_pcfd.m_pHelperSrc,
		true,
		rcDDButton,
		NULL,
		NULL,
		true,
		( _pcfd.m_bHover && (!_pcfd.m_bPushed) ) ? true : false,
		_pcfd.m_bPushed ? true : false,
		false,
		_pcfd.m_bEnabled,
		true,
		false,
		false,
		0,
		NULL
		);
	PaintPushButton( dc, _ppbd );
	if( !(_pcfd.m_bHover || _pcfd.m_bPushed) )
		dc.Draw3dRect( &rcDDButton, clrWindow, clrWindow );
CRect rcDDButtonGlyph( rcDDButton );
	if( _pcfd.m_bPushed )
		rcDDButtonGlyph.OffsetRect( 1, 1 );
	// paint arrow down
glyph_t* pGlyph = g_DockingCaptionGlyphs[ __DCBT_ARROW_SMALL_DOWN ];
	ASSERT( pGlyph != NULL );
	COLORREF ColorValues[2] = { RGB(0,0,0), ::GetSysColor( COLOR_BTNTEXT ) };
	PaintGlyphCentered( dc, rcDDButtonGlyph, *pGlyph, ColorValues );
}

#if (!defined __EXT_MFC_NO_TAB_CTRL)

void CExtPaintManager::GetThemeAccentTabColors(
	COLORREF * pClrSetectedTabItemText,
	COLORREF * pClrSetectedTabItemFace, // = NULL
	COLORREF * pClrSetectedTabItemBorderLight, // = NULL
	COLORREF * pClrSetectedTabItemBorderDark, // = NULL
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	if( pClrSetectedTabItemText != NULL )
		(*pClrSetectedTabItemText) = GetColor( COLOR_BTNTEXT, pHelperSrc, lParam );
	if( pClrSetectedTabItemFace != NULL )
		(*pClrSetectedTabItemFace) = GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam );
	if( pClrSetectedTabItemBorderLight != NULL )
		(*pClrSetectedTabItemBorderLight) = GetColor( COLOR_3DHILIGHT, pHelperSrc, lParam );
	if( pClrSetectedTabItemBorderDark != NULL )
		(*pClrSetectedTabItemBorderDark) = GetColor( COLOR_3DDKSHADOW, pHelperSrc, lParam );
}

void CExtPaintManager::PaintTabItem(
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
	LPARAM lParam, // = 0L
	COLORREF clrForceText, // = COLORREF(-1L)
	COLORREF clrForceTabBk, // = COLORREF(-1L)
	COLORREF clrForceTabBorderLT, // = COLORREF(-1L)
	COLORREF clrForceTabBorderRB, // = COLORREF(-1L)
	COLORREF clrForceTabSeparator, // = COLORREF(-1L)
	bool bDwmMode // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

CExtCmdIcon * pIconTabItemCloseButton = NULL;
CExtCmdIcon::e_paint_type_t ePaintStateITICB =
		(CExtCmdIcon::e_paint_type_t) CExtCmdIcon::__PAINT_INVISIBLE;
CRect rcTabItemCloseButton( 0, 0, 0, 0 );
CExtBarButton * pTBB = NULL;
CExtTabWnd * pTabs = NULL;
CExtTabWnd::TAB_ITEM_INFO * pTII = NULL;
bool bNoPrefix = false;
	if( pHelperSrc != NULL )
	{
		pTabs = DYNAMIC_DOWNCAST( CExtTabWnd, pHelperSrc );
		if( pTabs != NULL )
		{
			ASSERT_VALID( pTabs );
			pTII = pTabs->ItemGet( LONG( lParam ) );
			ASSERT( pTII != NULL );
			ASSERT_VALID( pTII );
			pIconTabItemCloseButton =
				pTabs->OnTabWndQueryItemCloseButtonShape( pTII );
			if( pIconTabItemCloseButton != NULL )
			{
				rcTabItemCloseButton = pTII->CloseButtonRectGet();
				ePaintStateITICB = (CExtCmdIcon::e_paint_type_t)
					pTabs->OnTabWndQueryItemCloseButtonPaintState( pTII );
			} // if( pIconTabItemCloseButton != NULL )
			bNoPrefix = ( ( pTabs->GetTabWndStyleEx() & __ETWS_EX_NO_PREFIX ) != 0 ) ? true : false;
		} // if( pTabs != NULL )
		else
		{
			pTBB = DYNAMIC_DOWNCAST( CExtBarButton, pHelperSrc );
#ifdef _DEBUG
			if( pTBB != NULL )
			{
				ASSERT_VALID( pTBB );
			} // if( pTBB != NULL )
#endif // _DEBUG
		} // else from if( pTabs != NULL )
	} // if( pHelperSrc != NULL )

bool bEnabled = true;
	if( pTII != NULL )
		bEnabled = pTII->EnabledGet();
	else if( pTBB != NULL )
		bEnabled = pTBB->IsEnabled();

#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
	if( pTII != NULL )
	{
		CObject * pObject = pTII->EventProviderGet();
		if( pObject != NULL )
		{
			CExtDynamicControlBar * pBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, pObject );
			if( pBar != NULL )
			{
				bool bFlashCaptionHighlightedState = false;
				if( pBar->FlashCaptionIsInProgress( &bFlashCaptionHighlightedState ) )
				{
					if( bFlashCaptionHighlightedState )
					{
						clrForceText = pBar->m_clrFlashCaptionText;
						clrForceTabBk = pBar->m_clrFlashCaptionBackground;
						bSelected = true;
					}
				}
			}
		}
	}
#endif// (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

COLORREF clrText = clrForceText;
	if( clrText == COLORREF(-1L) )
	{
		if( bEnabled ) 
		{
			clrText =
				GetColor(
					(bSelected && !bGroupedMode)
						? COLOR_BTNTEXT
						: ( bGroupedMode
							? COLOR_BTNTEXT
							: COLOR_3DHILIGHT
							)
						,
					pHelperSrc,
					lParam 
					);
		}
		else
		{
			clrText =
				GetColor(
					(bSelected && !bGroupedMode)
						? CLR_TEXT_DISABLED
						: ( bGroupedMode ? CLR_TEXT_DISABLED : COLOR_3DFACE )
						,
					pHelperSrc,
					lParam 
					);
		}
	}

COLORREF clrTabBk = clrForceTabBk;
	if( clrTabBk == COLORREF(-1L) )
		clrTabBk =
			GetColor(
				COLOR_BTNFACE,
				pHelperSrc,
				lParam
				);
COLORREF clrTabBorderLT = clrForceTabBorderLT;
	if( clrTabBorderLT == COLORREF(-1L) )
		clrTabBorderLT =
			GetColor(
				bGroupedMode
					? COLOR_3DSHADOW
					: COLOR_3DHILIGHT
					,
				pHelperSrc,
				lParam 
				);
COLORREF clrTabBorderRB = clrForceTabBorderRB;
	if( clrTabBorderRB == COLORREF(-1L) )
		clrTabBorderRB =
			GetColor(
				bGroupedMode
					? COLOR_3DSHADOW
					: COLOR_3DDKSHADOW
					,
				pHelperSrc,
				lParam 
				);
COLORREF clrTabSeparator = clrForceTabSeparator;
	if( clrTabSeparator == COLORREF(-1L) )
		clrTabSeparator =
			GetColor(
				bGroupedMode
					? COLOR_3DSHADOW
					: COLOR_3DLIGHT
					,
				pHelperSrc,
				lParam 
				);

bool bDrawIcon = (		
			pIcon != NULL 
		&&	(!pIcon->IsEmpty()) 
		&&	( pTabs == NULL || (pTabs->GetTabWndStyle()&__ETWS_HIDE_ICONS) == 0  )
		);
	CExtPaintManager::stat_PaintTabItemImpl(
		dc,
		rcTabItemsArea,
		bTopLeft,
		bHorz,
		bSelected,
		bEnabled,
		bCenteredText,
		bGroupedMode,
		bInGroupActive,
		bInvertedVerticalMode,
		bDrawIcon,
		rcEntireItem,
		sizeTextMeasured,
		pFont,
		sText,
		bNoPrefix,
		pIcon,
		pIconTabItemCloseButton,
		INT(ePaintStateITICB),
		rcTabItemCloseButton,
		clrText,
		clrTabBk,
		clrTabBorderLT,
		clrTabBorderRB,
		clrTabSeparator,
		( pTBB != NULL ) ? false : true,
		pHelperSrc,
		bDwmMode
		);
}

void CExtPaintManagerXP::PaintTabItem(
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
	LPARAM lParam, // = 0L
	COLORREF clrForceText, // = COLORREF(-1L)
	COLORREF clrForceTabBk, // = COLORREF(-1L)
	COLORREF clrForceTabBorderLT, // = COLORREF(-1L)
	COLORREF clrForceTabBorderRB, // = COLORREF(-1L)
	COLORREF clrForceTabSeparator, // = COLORREF(-1L)
	bool bDwmMode // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

CExtCmdIcon * pIconTabItemCloseButton = NULL;
CExtCmdIcon::e_paint_type_t ePaintStateITICB =
		(CExtCmdIcon::e_paint_type_t) CExtCmdIcon::__PAINT_INVISIBLE;
CRect rcTabItemCloseButton( 0, 0, 0, 0 );
CExtBarButton * pTBB = NULL;
CExtTabWnd * pTabs = NULL;
CExtTabWnd::TAB_ITEM_INFO * pTII = NULL;
bool bNoPrefix = false;
	if( pHelperSrc != NULL )
	{
		pTabs = DYNAMIC_DOWNCAST( CExtTabWnd, pHelperSrc );
		if( pTabs != NULL )
		{
			ASSERT_VALID( pTabs );
			pTII = pTabs->ItemGet( LONG( lParam ) );
			ASSERT( pTII != NULL );
			ASSERT_VALID( pTII );
			pIconTabItemCloseButton =
				pTabs->OnTabWndQueryItemCloseButtonShape( pTII );
			if( pIconTabItemCloseButton != NULL )
			{
				rcTabItemCloseButton = pTII->CloseButtonRectGet();
				ePaintStateITICB = (CExtCmdIcon::e_paint_type_t)
					pTabs->OnTabWndQueryItemCloseButtonPaintState( pTII );
			} // if( pIconTabItemCloseButton != NULL )
			bNoPrefix = ( ( pTabs->GetTabWndStyleEx() & __ETWS_EX_NO_PREFIX ) != 0 ) ? true : false;
		} // if( pTabs != NULL )
		else
		{
			pTBB = DYNAMIC_DOWNCAST( CExtBarButton, pHelperSrc );
#ifdef _DEBUG
			if( pTBB != NULL )
			{
				ASSERT_VALID( pTBB );
			} // if( pTBB != NULL )
#endif // _DEBUG
		} // else from if( pTabs != NULL )
	} // if( pHelperSrc != NULL )

bool bEnabled = true;
	if( pTII != NULL )
		bEnabled = pTII->EnabledGet();
	else if( pTBB != NULL )
		bEnabled = pTBB->IsEnabled();

#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
	if( pTII != NULL )
	{
		CObject * pObject = pTII->EventProviderGet();
		if( pObject != NULL )
		{
			CExtDynamicControlBar * pBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, pObject );
			if( pBar != NULL )
			{
				bool bFlashCaptionHighlightedState = false;
				if( pBar->FlashCaptionIsInProgress( &bFlashCaptionHighlightedState ) )
				{
					if( bFlashCaptionHighlightedState )
					{
						clrForceText = pBar->m_clrFlashCaptionText;
						clrForceTabBk = pBar->m_clrFlashCaptionBackground;
						bSelected = true;
					}
				}
			}
		}
	}
#endif// (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

COLORREF clrText = clrForceText;
	if( clrText == COLORREF(-1L) )
	{
		if( bEnabled ) 
			clrText = GetColor( ( bSelected || bGroupedMode ) ? COLOR_BTNTEXT : COLOR_3DSHADOW, pHelperSrc, lParam );
		else
			clrText = GetColor( CLR_TEXT_DISABLED, pHelperSrc, lParam  );
	}

COLORREF clrTabBk = clrForceTabBk;
	if( clrTabBk == COLORREF(-1L) )
		clrTabBk = GetColor( bGroupedMode ? COLOR_3DFACE : XPCLR_3DFACE_DARK , pHelperSrc, lParam );
COLORREF clrTabBorderLT = clrForceTabBorderLT;
	if( clrTabBorderLT == COLORREF(-1L) )
		clrTabBorderLT = GetColor( bGroupedMode ? COLOR_3DSHADOW : COLOR_3DHILIGHT, pHelperSrc, lParam );
COLORREF clrTabBorderRB = clrForceTabBorderRB;
	if( clrTabBorderRB == COLORREF(-1L) )
		clrTabBorderRB = GetColor( bGroupedMode ? COLOR_3DSHADOW : COLOR_3DDKSHADOW, pHelperSrc, lParam  );
COLORREF clrTabSeparator = clrForceTabSeparator;
	if( clrTabSeparator == COLORREF(-1L) )
		clrTabSeparator = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam  );

bool bDrawIcon = (		
		pIcon != NULL 
		&&	(!pIcon->IsEmpty()) 
		&&	( pTabs == NULL || (pTabs->GetTabWndStyle()&__ETWS_HIDE_ICONS) == 0  ) 
		);
	CExtPaintManager::stat_PaintTabItemImpl(
		dc,
		rcTabItemsArea,
		bTopLeft,
		bHorz,
		bSelected,
		bEnabled,
		bCenteredText,
		bGroupedMode,
		bInGroupActive,
		bInvertedVerticalMode,
		bDrawIcon,
		rcEntireItem,
		sizeTextMeasured,
		pFont,
		sText,
		bNoPrefix,
		pIcon,
		pIconTabItemCloseButton,
		INT(ePaintStateITICB),
		rcTabItemCloseButton,
		clrText,
		clrTabBk,
		clrTabBorderLT,
		clrTabBorderRB,
		clrTabSeparator,
		( pTBB != NULL ) ? false : true,
		pHelperSrc,
		bDwmMode
		);
}

void CExtPaintManager::PaintTabClientArea(
	CDC & dc,
	CRect & rcClient,
	CRect & rcTabItemsArea,
	CRect & rcTabNearBorderArea,
	DWORD dwOrientation,
	bool bGroupedMode,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
int idxClrTabNearMargin = COLOR_3DDKSHADOW;
	switch( dwOrientation )
	{
	case __ETWS_ORIENT_TOP:
		idxClrTabNearMargin = COLOR_3DHILIGHT;
	break;
	case __ETWS_ORIENT_BOTTOM:
	break;
	case __ETWS_ORIENT_LEFT:
		idxClrTabNearMargin = COLOR_3DHILIGHT;
	break;
	case __ETWS_ORIENT_RIGHT:
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( dwOrientation )

COLORREF clrTabItemsAreaBk =
		GetColor(
			( bGroupedMode || IsHighContrast() )
				? COLOR_3DFACE 
				: COLOR_3DSHADOW, 
			pHelperSrc, 
			lParam 
			);
COLORREF clrTabNearBorderAreaBk =
		GetColor( COLOR_3DFACE, pHelperSrc, lParam );
COLORREF clrTabNearMargin = 
		GetColor( idxClrTabNearMargin, pHelperSrc, lParam );

	CExtPaintManager::stat_PaintTabClientAreaImpl(
		dc,
		rcClient,
		rcTabItemsArea,
		rcTabNearBorderArea,
		dwOrientation,
		bGroupedMode,
		clrTabItemsAreaBk,
		clrTabNearBorderAreaBk,
		clrTabNearMargin
		);
}

bool CExtPaintManager::QueryTabWndHoverChangingRedraw(
	const CExtTabWnd * pWndTab,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWndTab );
	pWndTab;
	lParam;
	return false;
}

bool CExtPaintManagerNativeXP::QueryTabWndHoverChangingRedraw(
	const CExtTabWnd * pWndTab,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWndTab );
bool bGroupedMode =
		(pWndTab->GetTabWndStyle() & __ETWS_GROUPED) ? true : false;
	if(		bGroupedMode
		||	(! g_PaintManager.m_UxTheme.IsControlsThemed() )
		)
	{
		return 
			CExtPaintManager::QueryTabWndHoverChangingRedraw(
				pWndTab,
				lParam
				);
	}
	return true;
}

void CExtPaintManagerXP::PaintTabClientArea(
	CDC & dc,
	CRect & rcClient,
	CRect & rcTabItemsArea,
	CRect & rcTabNearBorderArea,
	DWORD dwOrientation,
	bool bGroupedMode,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
int idxClrTabNearMargin = COLOR_3DSHADOW;
	switch( dwOrientation )
	{
	case __ETWS_ORIENT_TOP:
		idxClrTabNearMargin = COLOR_3DHILIGHT;
	break;
	case __ETWS_ORIENT_BOTTOM:
	break;
	case __ETWS_ORIENT_LEFT:
		idxClrTabNearMargin = COLOR_3DHILIGHT;
	break;
	case __ETWS_ORIENT_RIGHT:
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( dwOrientation )

COLORREF clrTabItemsAreaBk =
		( ::GetDeviceCaps( dc.m_hDC, BITSPIXEL ) > 8 )
			? GetColor( XPCLR_3DFACE_NORMAL, pHelperSrc, lParam )
			: GetColor( XPCLR_3DFACE_DARK, pHelperSrc, lParam )
			;

COLORREF clrTabNearBorderAreaBk =
		GetColor( XPCLR_3DFACE_DARK, pHelperSrc, lParam );
COLORREF clrTabNearMargin = 
		GetColor( idxClrTabNearMargin, pHelperSrc, lParam );

	CExtPaintManager::stat_PaintTabClientAreaImpl(
		dc,
		rcClient,
		rcTabItemsArea,
		rcTabNearBorderArea,
		dwOrientation,
		bGroupedMode,
		clrTabItemsAreaBk,
		clrTabNearBorderAreaBk,
		clrTabNearMargin
		);
}

void CExtPaintManager::PaintTabbedTabClientArea(
	CDC & dc,
	CRect & rcClient,
	CRect & rcTabItemsArea,
	CRect & rcTabNearBorderArea,
	DWORD dwOrientation,
	bool bGroupedMode,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	rcTabNearBorderArea;
	bGroupedMode;
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf(RUNTIME_CLASS(CExtTabWnd))
		)
	{
		CWnd * pWnd = STATIC_DOWNCAST( CExtTabWnd, pHelperSrc );
		if( ! PaintDockerBkgnd( true, dc, pWnd, lParam ) )
			dc.FillSolidRect( &rcClient, GetColor( CExtPaintManager::CLR_3DFACE_OUT ) );
		COLORREF clr = 0;
		CRect rcTabNearMargin( rcTabItemsArea );
		switch( dwOrientation )
		{
		case __ETWS_ORIENT_TOP:
			rcTabNearMargin.top = rcTabNearMargin.bottom - 1;
			rcTabNearMargin.left = rcClient.left;
			rcTabNearMargin.right = rcClient.right;
			rcTabNearMargin.OffsetRect( 0, 1 );
			clr = GetColor( COLOR_3DHIGHLIGHT );
			break;
		case __ETWS_ORIENT_BOTTOM:
			rcTabNearMargin.bottom = rcTabNearMargin.top + 1;
			rcTabNearMargin.left = rcClient.left;
			rcTabNearMargin.right = rcClient.right;
			rcTabNearMargin.OffsetRect( 0, -1 );
			clr = GetColor( COLOR_3DSHADOW );
			break;
		case __ETWS_ORIENT_LEFT:
			rcTabNearMargin.left = rcTabNearMargin.right - 1;
			rcTabNearMargin.top = rcClient.top;
			rcTabNearMargin.bottom = rcClient.bottom;
			rcTabNearMargin.OffsetRect( 1, 0 );
			clr = GetColor( COLOR_3DHIGHLIGHT );
			break;
		case __ETWS_ORIENT_RIGHT:
			rcTabNearMargin.right = rcTabNearMargin.left + 1;
			rcTabNearMargin.top = rcClient.top;
			rcTabNearMargin.bottom = rcClient.bottom;
			rcTabNearMargin.OffsetRect( -1, 0 );
			clr = GetColor( COLOR_3DSHADOW );
			break;
		default:
			ASSERT( FALSE );
			return;
		} // switch( dwOrientation )

		// paint tab border margin
		dc.FillSolidRect(
			&rcTabNearMargin,
			clr
			);
	}
}

void CExtPaintManager::PaintTabButton(
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
	LPARAM lParam, // = 0L
	bool bFlat // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrGlyph = GetColor( bEnabled ? COLOR_BTNTEXT : CLR_TEXT_DISABLED, pHelperSrc, lParam  );
COLORREF clrTL = GetColor( bPushed ? COLOR_3DDKSHADOW : COLOR_3DHILIGHT, pHelperSrc, lParam  );
COLORREF clrBR = GetColor( bPushed ? COLOR_3DHILIGHT : COLOR_3DDKSHADOW, pHelperSrc, lParam  );
	CExtPaintManager::stat_PaintTabButtonImpl(
		dc,
		rcButton,
		nHitTest,
		bTopLeft,
		bHorz,
		bEnabled,
		bHover,
		bPushed,
		bGroupedMode,
		clrGlyph,
		clrTL,
		clrBR,
		bFlat
		);
}

void CExtPaintManagerXP::PaintTabButton(
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
	LPARAM lParam, // = 0L
	bool bFlat // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrGlyph = GetColor( bEnabled ? COLOR_BTNTEXT : CLR_TEXT_DISABLED, pHelperSrc, lParam  );
COLORREF clrTL = GetColor( bPushed ? COLOR_3DDKSHADOW : COLOR_3DHILIGHT, pHelperSrc, lParam );
COLORREF clrBR = GetColor( bPushed ? COLOR_3DHILIGHT : COLOR_3DDKSHADOW, pHelperSrc, lParam );
	CExtPaintManager::stat_PaintTabButtonImpl(
		dc,
		rcButton,
		nHitTest,
		bTopLeft,
		bHorz,
		bEnabled,
		bHover,
		bPushed,
		bGroupedMode,
		clrGlyph,
		clrTL,
		clrBR,
		bFlat
		);
}

void CExtPaintManager::TabWnd_AdjustTabAreaMetric( CExtTabWnd * pTabWnd, DWORD dwOrientation, LONG & nTabAreaMetric, LPARAM lParam )
{
	ASSERT_VALID( this );
	ASSERT( pTabWnd != NULL && ::IsWindow(pTabWnd->m_hWnd) );
	pTabWnd; dwOrientation; nTabAreaMetric; lParam;
}

void CExtPaintManagerNativeXP::TabWnd_AdjustTabAreaMetric( CExtTabWnd * pTabWnd, DWORD dwOrientation, LONG & nTabAreaMetric, LPARAM lParam )
{
	ASSERT_VALID( this );
	ASSERT( pTabWnd != NULL && ::IsWindow(pTabWnd->m_hWnd) );
	pTabWnd; lParam;
	if( ( dwOrientation == __ETWS_ORIENT_BOTTOM || dwOrientation == __ETWS_ORIENT_RIGHT ) && ( pTabWnd->GetTabWndStyle() & __ETWS_GROUPED ) == 0 )
		nTabAreaMetric += 2;
}

void CExtPaintManager::TabWnd_MeasureItemAreaMargins(
	CExtTabWnd * pTabWnd,
	LONG & nSpaceBefore,
	LONG & nSpaceAfter,
	LONG & nSpaceOver
	)
{
	ASSERT_VALID( this );
	ASSERT( pTabWnd != NULL && ::IsWindow(pTabWnd->m_hWnd) );
	ASSERT_VALID( pTabWnd );
	pTabWnd;
	nSpaceBefore	= 4;
	nSpaceAfter		= 2;
	if( (pTabWnd->GetTabWndStyle() & __ETWS_GROUPED) == 0 )
		nSpaceOver	= 2;
}

void CExtPaintManagerNativeXP::TabWnd_MeasureItemAreaMargins(
	CExtTabWnd * pTabWnd,
	LONG & nSpaceBefore,
	LONG & nSpaceAfter,
	LONG & nSpaceOver
	)
{
	ASSERT_VALID( this );
	ASSERT( pTabWnd != NULL && ::IsWindow(pTabWnd->m_hWnd) );
	ASSERT_VALID( pTabWnd );
bool bGroupedMode = ( pTabWnd->GetTabWndStyle() & __ETWS_GROUPED ) ? true : false;
	if(		bGroupedMode
		||	(! g_PaintManager.m_UxTheme.IsControlsThemed() )
		)
	{
		CExtPaintManager::TabWnd_MeasureItemAreaMargins( pTabWnd, nSpaceBefore, nSpaceAfter, nSpaceOver );
		return;
	}
	nSpaceBefore	= 0;
	nSpaceAfter		= 0;
	nSpaceOver		= 0;
}

void CExtPaintManager::TabWnd_UpdateItemMeasure(
	CExtTabWnd * pTabWnd,
	CExtTabWnd::TAB_ITEM_INFO * pTii,
	CDC & dcMeasure,
	CSize & sizePreCalc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( dcMeasure.GetSafeHdc() != NULL );
	ASSERT( pTii->GetTabWnd() == pTabWnd );
	pTabWnd; pTii; dcMeasure; sizePreCalc;
}

void CExtPaintManager::TabWnd_AdjustItemCloseButtonRect(
	CRect & rcCloseButton,
	CExtTabWnd * pTabWnd
	)
{
	ASSERT_VALID( this );
	pTabWnd; rcCloseButton;
}

bool CExtPaintManager::TabWnd_GetParentSizingMargin(
	INT & nMargin,
	DWORD dwOrientation,
	CExtTabWnd * pTabWnd
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTabWnd );
	nMargin; dwOrientation; pTabWnd;
	return false;
}

bool CExtPaintManagerNativeXP::TabWnd_GetParentSizingMargin(
	INT & nMargin,
	DWORD dwOrientation,
	CExtTabWnd * pTabWnd
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTabWnd );
	if(		(! g_PaintManager.m_UxTheme.IsControlsThemed() )
		||	pTabWnd->_IsCustomLayoutTabWnd() 
		)
		return CExtPaintManager::TabWnd_GetParentSizingMargin( nMargin, dwOrientation, pTabWnd );
	nMargin = 1;
	return true;
}

void CExtPaintManager::PaintTabNcAreaRect(
	CDC & dc,
	const RECT & rc,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrTL = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
COLORREF clrBR = GetColor( COLOR_3DHILIGHT, pHelperSrc, lParam );
	dc.Draw3dRect( &rc, clrTL, clrBR );
}

bool CExtPaintManager::PaintTabMdiOuterBorder(
	CDC & dc,
	const CRect & rcOuterBorder,
	const CRect & rcMdiAreaClient,
	const CRect & rcMdiAreaWnd,
	HWND hWndHooked,
	CExtTabWnd * pTabWnd,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pTabWnd->GetSafeHwnd() != NULL );
	ASSERT( hWndHooked != NULL && ::IsWindow(hWndHooked) );
	dc; rcOuterBorder; rcMdiAreaClient; rcMdiAreaWnd; hWndHooked; pTabWnd; lParam;
	return false;
}

void CExtPaintManagerXP::PaintTabNcAreaRect(
	CDC & dc,
	const RECT & rc,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrTL = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
COLORREF clrBR = clrTL;
	dc.Draw3dRect( &rc, clrTL, clrBR );
}

void CExtPaintManagerNativeXP::PaintTabNcAreaRect(
	CDC & dc,
	const RECT & rc,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		(! g_PaintManager.m_UxTheme.IsControlsThemed() )
		||	pHelperSrc == NULL
		||	(! pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtTabWnd ) ) )
		||	((CExtTabWnd*)pHelperSrc)->_IsCustomLayoutTabWnd()
		)
		CExtPaintManager::PaintTabNcAreaRect( dc, rc, pHelperSrc, lParam );
	PaintDockerBkgnd( false, dc, (CWnd*)pHelperSrc, lParam );
}

#endif // #if (!defined __EXT_MFC_NO_TAB_CTRL)

void CExtPaintManager::PaintDockBarClientArea(
	CDC & dc,
	const RECT & rcClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rcClient ) || ( ! dc.RectVisible( &rcClient ) ) )
		return;
	if( RenderCustomBackground( true, dc, pHelperSrc, lParam ) )
		return;
	dc.FillSolidRect( &rcClient, GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam ) );
}

CExtPaintManager::PAINTINHERITEDBACKGROUNDDATA::PAINTINHERITEDBACKGROUNDDATA(
	bool bClientMapping,
	CDC & dc,
	CWnd * pWnd,
	CObject * pObjHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
	: m_bClientMapping( bClientMapping )
	, m_bBackgroundDrawn( false )
	, m_bSequenceCanceled( false )
	, m_hWndBackgroundRequestPainter( NULL )
	, m_dc( dc )
	, m_pWnd( pWnd )
	, m_pObjHelperSrc( pObjHelperSrc )
	, m_lParam( lParam )
{
	ASSERT_VALID( (&m_dc) );
	ASSERT( m_dc.GetSafeHdc() != NULL );
	ASSERT_VALID( m_pWnd );
	ASSERT( m_pWnd->GetSafeHwnd() != NULL );
}

CExtPaintManager::PAINTINHERITEDBACKGROUNDDATA::PAINTINHERITEDBACKGROUNDDATA(
	bool bClientMapping,
	CDC & dc,
	CObject * pObjHelperSrc,
	LPARAM lParam // = 0L
	)
	: m_bClientMapping( bClientMapping )
	, m_bBackgroundDrawn( false )
	, m_bSequenceCanceled( false )
	, m_hWndBackgroundRequestPainter( NULL )
	, m_dc( dc )
	, m_pWnd( NULL )
	, m_pObjHelperSrc( pObjHelperSrc )
	, m_lParam( lParam )
{
	ASSERT_VALID( (&m_dc) );
	ASSERT( m_dc.GetSafeHdc() != NULL );
	ASSERT_VALID( m_pObjHelperSrc );
	m_pWnd = STATIC_DOWNCAST( CWnd, m_pObjHelperSrc );
	ASSERT_VALID( m_pWnd );
	ASSERT( m_pWnd->GetSafeHwnd() != NULL );
}

bool CExtPaintManager::PAINTINHERITEDBACKGROUNDDATA::Notify()
{
	ASSERT_VALID( (&m_dc) );
	ASSERT( m_dc.GetSafeHdc() != NULL );
	ASSERT_VALID( m_pWnd );
	ASSERT( m_pWnd->GetSafeHwnd() != NULL );
HWND hWnd = m_pWnd->GetSafeHwnd();
	for( ; hWnd != NULL; hWnd = ::GetParent(hWnd) )
	{
		::SendMessage( hWnd, CExtPaintManager::g_nMsgPaintInheritedBackground, *this, 0 );
		if( m_bSequenceCanceled )
		{
			m_bBackgroundDrawn = false;
			m_hWndBackgroundRequestPainter = NULL;
			break;
		} // if( m_bSequenceCanceled )
		if( m_bBackgroundDrawn )
		{
			if( m_hWndBackgroundRequestPainter == NULL )
				m_hWndBackgroundRequestPainter = hWnd;
			return true;
		} // if( m_bBackgroundDrawn )
		__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE );
		if( (dwStyle&WS_CHILD) == 0 )
			break;
	} // for( ; hWnd != NULL; hWnd = ::GetParent(hWnd) )
	return false;
}

CExtPaintManager::PAINTINHERITEDBACKGROUNDDATA::operator WPARAM() const
{
	return WPARAM( this );
}

CExtPaintManager::PAINTINHERITEDBACKGROUNDDATA *
	CExtPaintManager::PAINTINHERITEDBACKGROUNDDATA::FromWPARAM( WPARAM wParam )
{
PAINTINHERITEDBACKGROUNDDATA * pPIBD =
			reinterpret_cast < PAINTINHERITEDBACKGROUNDDATA * > ( wParam );
	ASSERT( pPIBD != NULL );
	return pPIBD;
}

CRect CExtPaintManager::PAINTINHERITEDBACKGROUNDDATA::GetRenderingRect() const
{
CRect rc;
	if( m_bClientMapping )
		m_pWnd->GetClientRect( &rc );
	else
	{
		m_pWnd->GetWindowRect( &rc );
		rc.OffsetRect( -rc.left, -rc.top );
	}
	return rc;
}

bool CExtPaintManager::RenderCustomBackground(
	bool bClientMapping,
	CDC & dc,
	CObject * pObjHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	if(		pObjHelperSrc == NULL
		||	(! GetCustomBackgroundInheritanceMode( pObjHelperSrc, lParam ) )
		||	dc.GetSafeHdc() == NULL
		)
		return false;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pObjHelperSrc );
	if( pWnd->GetSafeHwnd() == NULL )
	{
		CExtBarButton * pTBB = DYNAMIC_DOWNCAST( CExtBarButton, pObjHelperSrc );
		if( pTBB != NULL )
			pWnd = pTBB->GetBar();
	}
	if( pWnd->GetSafeHwnd() == NULL )
		return false;
CExtPaintManager::PAINTINHERITEDBACKGROUNDDATA _pibd(
		bClientMapping,
		dc,
		pWnd,
		pObjHelperSrc,
		lParam
		);
	return _pibd.Notify();
}

UINT CExtPaintManager::g_nMsgPaintInheritedBackground =
	::RegisterWindowMessage(
		_T("CExtPaintManager::g_nMsgPaintInheritedBackground")
		);

bool CExtPaintManager::IsCustomBackgroundInheritanceEnabled() const
{
	ASSERT_VALID( this );
	return m_bCustomBackgroundInheritanceEnabled;
}

bool CExtPaintManager::GetCustomBackgroundInheritanceMode(
	CObject * pObjHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pObjHelperSrc );
	pObjHelperSrc;
	lParam;
	return IsCustomBackgroundInheritanceEnabled();
}

bool CExtPaintManager::GetCb2DbTransparentMode(
	CObject * pObjHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	if( GetCustomBackgroundInheritanceMode(
			pObjHelperSrc,
			lParam
			)
		)
		return true;
	return false;
}

bool CExtPaintManagerNativeXP::GetCb2DbTransparentMode(
	CObject * pObjHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	if( GetCustomBackgroundInheritanceMode(
			pObjHelperSrc,
			lParam
			)
		)
		return true;
//	return false;
	return true;
}

bool CExtPaintManager::PaintDocumentClientAreaBkgnd(
	CDC & dc,
	CWnd * pWnd,
	LPARAM lParam // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pWnd->GetSafeHwnd() != NULL );
CRect rcClient;
	pWnd->GetClientRect( &rcClient );
	if( dc.RectVisible( &rcClient ) )
		dc.FillSolidRect( rcClient, GetColor( COLOR_3DFACE, pWnd, lParam ) );	
	return true;
}

bool CExtPaintManager::PaintDockerBkgnd(
	bool bClientMapping,
	CDC & dc,
	CWnd * pWnd,
	LPARAM lParam // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pWnd->GetSafeHwnd() != NULL );
	if( RenderCustomBackground( bClientMapping, dc, pWnd, lParam ) )
		return true;
CRect rc;
	if( bClientMapping )
		pWnd->GetClientRect( &rc );
	else
	{
		pWnd->GetWindowRect( &rc );
		rc.OffsetRect( -rc.left, -rc.top );
	}
	dc.FillSolidRect( rc, GetColor( CLR_3DFACE_OUT, pWnd, lParam ) );	
	return true;
}

bool CExtPaintManager::PaintDockerBkgnd(
	bool bClientMapping,
	CDC & dc,
	const CRect &rcDst,
	const CRect &rcWnd,
	LPARAM lParam // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	rcWnd; lParam; bClientMapping;
	dc.FillSolidRect( rcDst, GetColor( CLR_3DFACE_OUT, NULL ) );		
	return true;
}

bool CExtPaintManagerNativeXP::PaintDockerBkgnd(
	bool bClientMapping,
	CDC & dc,
	CWnd * pWnd,
	LPARAM lParam // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pWnd->GetSafeHwnd() != NULL );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return  CExtPaintManager::PaintDockerBkgnd( bClientMapping, dc, pWnd, lParam );
	if( RenderCustomBackground( bClientMapping, dc, pWnd, lParam ) )
		return true;
	if(	! GetCb2DbTransparentMode( pWnd ) )
		return false;
bool bToolbarShade = false;
bool bToolbarHorz = false;
CWnd * pFrame = pWnd;
	for( ; pFrame != NULL; pFrame = pFrame->GetParent() )
	{
		if( (pFrame->GetStyle() & WS_CHILD) == 0 )
			break;
		else // frame features - design mode
			if( pFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
				break;
		if(		pFrame != pWnd
			&&	pFrame->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
			)
		{
			if( ((CExtToolControlBar*)pFrame)->m_pDockSite != NULL )
			{
				if(		(!(	pFrame->IsKindOf( RUNTIME_CLASS(CExtPanelControlBar) )
						||	pFrame->IsKindOf( RUNTIME_CLASS(CExtMenuControlBar) )
						) )
					&&	(! ((CExtToolControlBar*)pFrame)->m_bForceNoBalloonWhenRedockable)
					)
				{
					if( ((CExtToolControlBar*)pFrame)->m_pDockBar == NULL )
					{
						if( ((CExtToolControlBar*)pFrame)->m_bForceBalloonGradientInDialogs != 0 )
						{
							bToolbarShade = true;
							DWORD dwBarStyle = ((CExtToolControlBar*)pFrame)->GetBarStyle();
							bToolbarHorz = ( ( dwBarStyle & ( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT ) ) != 0 ) ? false : true;
							break;
						}
						continue;
					}
					if( ! ((CExtToolControlBar*)pFrame)->IsFloating() )
					{
						bToolbarShade = true;
						bToolbarHorz = ((CExtToolControlBar*)pFrame)->IsDockedHorizontally();
					}
					break;
				}
			}
			else if( ((CExtToolControlBar*)pFrame)->m_bForceBalloonGradientInDialogs != 0 )
			{
				bToolbarShade = true;
				DWORD dwBarStyle = ((CExtToolControlBar*)pFrame)->GetBarStyle();
				bToolbarHorz = ( ( dwBarStyle & ( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT ) ) != 0 ) ? false : true;
				break;
			}
		}
	} // for( ; pFrame != NULL; pFrame = pFrame->GetParent() )

	if( pFrame == NULL )
		return false;

CExtToolControlBar::FtLayoutQuery _FtLayoutQuery;
	if( _FtLayoutQuery.Query( pWnd->m_hWnd ) )
	{
		if( bClientMapping )
			_FtLayoutQuery.ReMapRectsToTargetClient();
		else
			_FtLayoutQuery.ReMapRectsToTargetWindow();
		_FtLayoutQuery.DrawRects( dc );
		return true;
	} // if( _FtLayoutQuery.Query( pWnd->m_hWnd ) )

CRect rcPaintGradient;
	pFrame->GetClientRect( &rcPaintGradient );
	pFrame->ClientToScreen( &rcPaintGradient );

	if( bClientMapping )
	{
		pWnd->ScreenToClient( &rcPaintGradient );
	} // if( bClientMapping )
	else
	{
		CRect rc;
		pWnd->GetWindowRect( &rc );
		rcPaintGradient.OffsetRect( -rc.left, -rc.top );
	} // else from if( bClientMapping )
	if(		::IsRectEmpty( &rcPaintGradient )
		||	( ! dc.RectVisible( &rcPaintGradient ) )
		)
		return true;
	if( bToolbarShade )
	{
		bool bRebarDrawingStyle = false;
		CExtControlBar * pWndCB = DYNAMIC_DOWNCAST( CExtControlBar, pFrame );
		if( pWndCB != NULL )
		{
			ASSERT_VALID( pWndCB );
			if( pWndCB->m_pDockBar->GetSafeHwnd() != NULL )
			{
				ASSERT_VALID( pWndCB->m_pDockBar );
				if(		pWndCB->IsFixedDockStyle() 
					&&	(!pWndCB->IsFloating())
					)
					bRebarDrawingStyle = true;
			}
		}
		if( bRebarDrawingStyle )
		{
			CRect rcPaint;
			pWndCB->m_pDockBar->GetWindowRect( &rcPaint );
			pWndCB->ScreenToClient( &rcPaint );
			if( g_PaintManager.m_UxTheme.OpenThemeData( pWndCB->GetSafeHwnd(), VSCLASS_REBAR ) != NULL )
			{
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						pWndCB->GetSafeHwnd(),
						dc.GetSafeHdc(), 
						0, 
						0, 
						&rcPaint, 
						&rcPaint
						) == S_OK
					);
				g_PaintManager.m_UxTheme.CloseThemeData();
			}
		}
		else
			dc.FillSolidRect( &rcPaintGradient, ::GetSysColor( COLOR_3DFACE ) );
	} // else if( bToolbarShade )
	else
		dc.FillSolidRect( rcPaintGradient, ::GetSysColor( COLOR_3DFACE ) );		
	return true;
}

void CExtPaintManager::PaintControlBarClientArea(
	CDC & dc,
	const RECT & rcClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rcClient ) || ( ! dc.RectVisible( &rcClient ) ) )
		return;
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
		&&	( ((CExtToolControlBar*)pHelperSrc)->m_bForceNoBalloonWhenRedockable )
		&&	RenderCustomBackground( true, dc, pHelperSrc, lParam )
		)
		return;
	dc.FillSolidRect( &rcClient, GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam ) );
}

void CExtPaintManagerNativeXP::PaintControlBarClientArea(
	CDC & dc,
	const RECT & rcClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rcClient ) || ( ! dc.RectVisible( &rcClient ) ) )
		return;
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintControlBarClientArea( dc, rcClient, pHelperSrc, lParam );
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
		&&	( ((CExtToolControlBar*)pHelperSrc)->m_bForceNoBalloonWhenRedockable )
		&&	RenderCustomBackground( true, dc, pHelperSrc, lParam )
		)
		return;
bool bRebarDrawingStyle = false;
CExtControlBar * pWnd = DYNAMIC_DOWNCAST( CExtControlBar, pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		if( pWnd->m_pDockBar->GetSafeHwnd() != NULL )
		{
			ASSERT_VALID( pWnd->m_pDockBar );
			if( pWnd->IsFixedDockStyle() && ( ! pWnd->IsFloating() ) )
				bRebarDrawingStyle = true;
		}
	}
	if( bRebarDrawingStyle )
	{
		CRect rcPaint;
		pWnd->m_pDockBar->GetWindowRect( &rcPaint );
		pWnd->ScreenToClient( &rcPaint );
		if( g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_REBAR ) != NULL )
		{
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					pWnd->GetSafeHwnd(),
					dc.GetSafeHdc(), 
					0, 
					0, 
					&rcPaint, 
					&rcPaint
					) == S_OK
				);
			g_PaintManager.m_UxTheme.CloseThemeData();
		}
	}
	else
		dc.FillSolidRect( &rcClient, ::GetSysColor( COLOR_3DFACE ) );
}

void CExtPaintManagerNativeXP::PaintDockBarClientArea(
	CDC & dc,
	const RECT & rcClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rcClient ) || ( ! dc.RectVisible( &rcClient ) ) )
		return;
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintDockBarClientArea( dc, rcClient, pHelperSrc, lParam );
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	if( RenderCustomBackground( true, dc, pHelperSrc, lParam ) )
		return;
HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}
	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_REBAR ) != NULL )
	{
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd,
				dc.GetSafeHdc(), 
				0, 
				0, 
				&rcClient, 
				&rcClient
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

CExtPaintManager::PAINTDOCKINGFRAMEDATA::PAINTDOCKINGFRAMEDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_rcWindow( 0,0,0,0 )
	, m_rcClient( 0,0,0,0 )
	, m_bActive( false )
	, m_bFloating( false )
	, m_bExtBar( false )
	, m_bSideBar( false )
{
}

CExtPaintManager::PAINTDOCKINGFRAMEDATA::PAINTDOCKINGFRAMEDATA(
	CObject * pHelperSrc,
	const RECT & rcWindow,
	const RECT & rcClient,
	bool bActive,
	bool bFloating,
	bool bExtBar,
	bool bSideBar // = false
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_rcWindow( rcWindow )
	, m_rcClient( rcClient )
	, m_bActive( bActive )
	, m_bFloating( bFloating )
	, m_bExtBar( bExtBar )
	, m_bSideBar( bSideBar )
{
}

void CExtPaintManager::PaintDockingFrame(
	CDC & dc,
	CExtPaintManager::PAINTDOCKINGFRAMEDATA & _pdfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pdfd.m_rcWindow.IsRectEmpty() )
		return;
	dc.ExcludeClipRect( _pdfd.m_rcClient );
CRect rcWnd( _pdfd.m_rcWindow );
COLORREF clr3dFace = GetColor( CLR_3DFACE_OUT, _pdfd.m_pHelperSrc, _pdfd.m_lParam );
COLORREF clr3dHilight = GetColor( CLR_3DHILIGHT_OUT, _pdfd.m_pHelperSrc, _pdfd.m_lParam );
COLORREF clr3dShadow = GetColor( CLR_3DSHADOW_OUT, _pdfd.m_pHelperSrc, _pdfd.m_lParam );
COLORREF clr3dDkShadow = GetColor( CLR_3DDKSHADOW_OUT, _pdfd.m_pHelperSrc, _pdfd.m_lParam );
	if(		_pdfd.m_pHelperSrc != NULL
		&&	_pdfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
		&&	( ((CExtToolControlBar*)_pdfd.m_pHelperSrc)->m_pDockSite != NULL )
		&&	( ((CExtToolControlBar*)_pdfd.m_pHelperSrc)->m_bForceNoBalloonWhenRedockable )
		)
	{
	}
	else
	{
		dc.FillSolidRect( &rcWnd, clr3dFace );
		if( _pdfd.m_bFloating )
		{
			dc.Draw3dRect( &rcWnd, clr3dFace, clr3dDkShadow);
			rcWnd.DeflateRect( 1, 1 );
			dc.Draw3dRect( &rcWnd, clr3dHilight, clr3dShadow);
		}
		else if( ! _pdfd.m_bExtBar )
				dc.Draw3dRect( &rcWnd, clr3dHilight, clr3dShadow);
	}
	dc.IntersectClipRect( _pdfd.m_rcWindow );
}

void CExtPaintManagerXP::PaintDockingFrame(
	CDC & dc,
	CExtPaintManager::PAINTDOCKINGFRAMEDATA & _pdfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pdfd.m_rcWindow.IsRectEmpty() )
		return;
	dc.ExcludeClipRect( _pdfd.m_rcClient );
CRect rcWnd( _pdfd.m_rcWindow );
COLORREF clr3dFace = GetColor( COLOR_3DFACE, _pdfd.m_pHelperSrc, _pdfd.m_lParam );
COLORREF clrDark = GetColor( COLOR_3DSHADOW, _pdfd.m_pHelperSrc, _pdfd.m_lParam );
COLORREF clrBkOld = dc.GetBkColor();
COLORREF clrBkNew =
		GetColor(
			( _pdfd.m_bFloating )
				? (	_pdfd.m_bExtBar ? XPCLR_3DFACE_FLOAT_R : XPCLR_3DFACE_FLOAT_F )
				: CLR_3DFACE_OUT
				,
			_pdfd.m_pHelperSrc,
			_pdfd.m_lParam 
			);
	if( _pdfd.m_bFloating )
	{
		dc.Draw3dRect( &rcWnd, clrDark, clrDark);
		rcWnd.DeflateRect(1,1);
		dc.Draw3dRect( &rcWnd, clrDark, clrDark);
		rcWnd.DeflateRect(1,1);
		dc.Draw3dRect( &rcWnd, clrBkNew, clrBkNew);
		rcWnd.DeflateRect(0,0,1,1);
		dc.SetPixel( rcWnd.left, rcWnd.top, clrDark );
		dc.SetPixel( rcWnd.left, rcWnd.bottom, clrDark );
		dc.SetPixel( rcWnd.right, rcWnd.top, clrDark );
		dc.SetPixel( rcWnd.right, rcWnd.bottom, clrDark );
	} // if( _pdfd.m_bFloating )
	else
	{
		if(		_pdfd.m_pHelperSrc != NULL
			&&	_pdfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
			&&	( ((CExtToolControlBar*)_pdfd.m_pHelperSrc)->m_pDockSite != NULL )
			&&	( ((CExtToolControlBar*)_pdfd.m_pHelperSrc)->m_bForceNoBalloonWhenRedockable )
			)
		{
			dc.SelectClipRgn(NULL);
			return;
		}
		dc.FillSolidRect( &rcWnd, clrBkNew );
		if( ! _pdfd.m_bExtBar )
			dc.Draw3dRect( &rcWnd, clr3dFace, clr3dFace);

		if(		_pdfd.m_pHelperSrc != NULL
			&&	_pdfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
			&&	(!_pdfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtMenuControlBar) ))
			&&	IsHighContrast()
			)
		{
			CExtToolControlBar * pBar = STATIC_DOWNCAST( CExtToolControlBar, _pdfd.m_pHelperSrc );
			ASSERT_VALID( pBar );

			if( ! pBar->IsFloating() )
			{
				COLORREF clrOldText =  dc.SetTextColor( GetColor( COLOR_3DSHADOW, _pdfd.m_pHelperSrc, _pdfd.m_lParam ) );
				dc.DrawFocusRect( &rcWnd );
				dc.SetTextColor( clrOldText );
			}
		}	
		if(		(!_pdfd.m_bSideBar)
			&&	(::GetDeviceCaps( dc.m_hDC, BITSPIXEL ) <= 8 )
			)
		{
			static WORD hatchBits[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, };
			CBrush br;
			CBitmap bmp;
			bmp.CreateBitmap( 8, 8, 1, 1, hatchBits );
			br.CreatePatternBrush( &bmp );
			LOGBRUSH lbr;
			br.GetLogBrush( &lbr );
			HPEN hPenDesired = ::ExtCreatePen( PS_GEOMETRIC, 1, &lbr, 0, NULL );
			if( hPenDesired == NULL )
			{
				COLORREF clrDesiredSolidPen = GetColor( COLOR_3DSHADOW, _pdfd.m_pHelperSrc, _pdfd.m_lParam );
				hPenDesired = ::CreatePen( PS_SOLID, 1, clrDesiredSolidPen );
				if( hPenDesired == NULL )
				{
					hPenDesired = (HPEN) ::GetStockObject(BLACK_PEN);
					ASSERT( hPenDesired != NULL );
				} // if( hPenDesired == NULL )
			} // if( hPenDesired == NULL )
			CPen pen;
			VERIFY( pen.Attach( hPenDesired ) );
			CPen * pOldPen = dc.SelectObject( &pen );
			if( m_bHelperXpStyle8BitBarRect )
			{
				dc.MoveTo( rcWnd.left, rcWnd.top );
				dc.LineTo( rcWnd.right-1, rcWnd.top );
				dc.LineTo( rcWnd.right-1, rcWnd.bottom-1 );
				dc.LineTo( rcWnd.left, rcWnd.bottom-1 );
				dc.LineTo( rcWnd.left, rcWnd.top );
			} // if( m_bHelperXpStyle8BitBarRect )
			else
			{
				static const SIZE g_sizeRO = { 2, 2 };
				dc.MoveTo( rcWnd.left+g_sizeRO.cx, rcWnd.top );
				dc.LineTo( rcWnd.right-1-g_sizeRO.cx, rcWnd.top );
				dc.LineTo( rcWnd.right-1, rcWnd.top+g_sizeRO.cy );
				dc.LineTo( rcWnd.right-1, rcWnd.bottom-1-g_sizeRO.cy );
				dc.LineTo( rcWnd.right-1-g_sizeRO.cx, rcWnd.bottom-1 );
				dc.LineTo( rcWnd.left+g_sizeRO.cx, rcWnd.bottom-1 );
				dc.LineTo( rcWnd.left, rcWnd.bottom-1-g_sizeRO.cy );
				dc.LineTo( rcWnd.left, rcWnd.top+g_sizeRO.cy );
				dc.LineTo( rcWnd.left+g_sizeRO.cx, rcWnd.top );
			} // else from if( m_bHelperXpStyle8BitBarRect )
			dc.SelectObject( pOldPen );
		} // if( !_pdfd.m_bSideBar && ::GetDeviceCaps( dc.m_hDC, BITSPIXEL ) < 8 )
	} // else from if( _pdfd.m_bFloating )
	dc.SetBkColor( clrBkOld );
	//dc.IntersectClipRect(rectWindow);
	dc.SelectClipRgn(NULL);
}

void CExtPaintManagerNativeXP::PaintDockingFrame(
	CDC & dc,
	CExtPaintManager::PAINTDOCKINGFRAMEDATA & _pdfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pdfd.m_rcWindow.IsRectEmpty() )
		return;
	if(		_pdfd.m_pHelperSrc != NULL
		&&	_pdfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
		&&	( ((CExtToolControlBar*)_pdfd.m_pHelperSrc)->m_pDockSite != NULL )
		&&	( ((CExtToolControlBar*)_pdfd.m_pHelperSrc)->m_bForceNoBalloonWhenRedockable )
		)
		return;
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintDockingFrame( dc, _pdfd );
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, _pdfd.m_pHelperSrc );
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
CRect rcWnd( _pdfd.m_rcWindow );
	dc.ExcludeClipRect( _pdfd.m_rcClient );
	dc.FillSolidRect( &rcWnd, ::GetSysColor( COLOR_3DFACE ) );
bool bRebarDrawingStyle = false;
CExtControlBar * pCB = DYNAMIC_DOWNCAST( CExtControlBar, _pdfd.m_pHelperSrc );
	if( pCB != NULL )
	{
		ASSERT_VALID( pCB );
		if( pCB->m_pDockBar->GetSafeHwnd() != NULL )
		{
			ASSERT_VALID( pCB->m_pDockBar );
			if(		pCB->IsFixedDockStyle() 
				&&	(!pCB->IsFloating())
				)
				bRebarDrawingStyle = true;
		}
	}
	if(		bRebarDrawingStyle 
		&&	(!_pdfd.m_bFloating)
		&&	(!_pdfd.m_bExtBar)
		)
	{
		CRect rcPaint;
		pCB->m_pDockBar->GetWindowRect( &rcPaint );
		pCB->ScreenToClient( &rcPaint );
		CRect rcAdjust( 0, 0, 0, 0 );
		FixedBar_AdjustClientRect( pCB, rcAdjust );
		rcPaint.DeflateRect( &rcAdjust );

		if(	g_PaintManager.m_UxTheme.OpenThemeData( pCB->GetSafeHwnd(), VSCLASS_REBAR ) != NULL )
		{
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					pCB->GetSafeHwnd(),
					dc.GetSafeHdc(), 
					0, 
					0, 
					&rcPaint, 
					&rcPaint
					) == S_OK
				);
			COLORREF clrEdgeShadow = COLORREF( -1L );
			VERIFY( 
				g_PaintManager.m_UxTheme.GetThemeColor(
					RP_BAND, 
					0,
					TMT_EDGESHADOWCOLOR, 
					&clrEdgeShadow
					) == S_OK
				);
			if( clrEdgeShadow != COLORREF( -1L ) )
				dc.Draw3dRect( &rcWnd, ::GetSysColor( COLOR_BTNHIGHLIGHT ), clrEdgeShadow );
			g_PaintManager.m_UxTheme.CloseThemeData();
		}
	}
	if( _pdfd.m_bFloating )
	{
		if(	g_PaintManager.m_UxTheme.OpenThemeData(  pCB->GetSafeHwnd(), VSCLASS_WINDOW ) != NULL )
		{
// !!! TODO
// _pdfd.m_bActive = true;
// !!! TODO
			INT nStateID = _pdfd.m_bActive ? FS_ACTIVE : FS_INACTIVE;
			CRect rcNcLeft( _pdfd.m_rcWindow );
			rcNcLeft.right = _pdfd.m_rcClient.left;
			if( rcNcLeft.Width() > 0 )
			{
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						pCB->GetSafeHwnd(), 
						dc.GetSafeHdc(), 
						WP_SMALLFRAMELEFT, 
						nStateID, 
						&rcNcLeft, 
						&rcNcLeft
						) == S_OK
					);
			}	
			CRect rcNcRight( _pdfd.m_rcWindow );
			rcNcRight.left = _pdfd.m_rcClient.right;
			if( rcNcRight.Width() > 0 )
			{
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						pCB->GetSafeHwnd(), 
						dc.GetSafeHdc(), 
						WP_SMALLFRAMERIGHT, 
						nStateID, 
						&rcNcRight, 
						&rcNcRight
						) == S_OK
					);
			}
			CRect rcNcTop( _pdfd.m_rcWindow );
			rcNcTop.bottom = _pdfd.m_rcClient.top;
			if( rcNcTop.Height() > 0 )
			{
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						pCB->GetSafeHwnd(), 
						dc.GetSafeHdc(), 
						WP_SMALLCAPTION, 
						_pdfd.m_bActive
							? CS_ACTIVE
							: CS_INACTIVE, 
						&rcNcTop, 
						&rcNcTop
						) == S_OK
					);
			}
			CRect rcNcBottom( _pdfd.m_rcWindow );
			rcNcBottom.top = _pdfd.m_rcClient.bottom;
			if( rcNcBottom.Height() > 0 )
			{
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						pCB->GetSafeHwnd(), 
						dc.GetSafeHdc(), 
						WP_SMALLFRAMEBOTTOM, 
						nStateID, 
						&rcNcBottom, 
						&rcNcBottom
						) == S_OK
					);
			}
		} // if( g_PaintManager.m_UxTheme.OpenThemeData ...
	} // if( _pdfd.m_bFloating )
	dc.IntersectClipRect( _pdfd.m_rcWindow );
}

CExtPaintManager::PAINTGRIPPERDATA::PAINTGRIPPERDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_rcGripper( 0,0,0,0 )
	, m_rcText( 0,0,0,0 )
	, m_bActive( false )
	, m_bFloating( false )
	, m_bHorz( false )
	, m_bSideBar( false )
	, m_sCaption( _T("") )
	, m_bHelperNoRect( false )
	, m_bHelperNoFill( false )
	, m_bForceRTL( false )
	, m_bFlashCaptionHighlightedState( false )
	, m_clrFlashCaptionText( COLORREF(-1L) )
	, m_clrFlashCaptionBackground( COLORREF(-1L) )
{
}

CExtPaintManager::PAINTGRIPPERDATA::PAINTGRIPPERDATA(
	CObject * pHelperSrc,
	const RECT & rcGripper,
	const RECT & rcText,
	bool bActive,
	bool bFloating,
	bool bHorz,
	bool bSideBar, // = false
	__EXT_MFC_SAFE_LPCTSTR sCaption, // = NULL
	bool bForceRTL // = false
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_rcGripper( rcGripper )
	, m_rcText( rcText )
	, m_bActive( bActive )
	, m_bFloating( bFloating )
	, m_bHorz( bHorz )
	, m_bSideBar( bSideBar )
	, m_sCaption( (sCaption == NULL) ? _T("") : sCaption )
	, m_bHelperNoRect( false )
	, m_bHelperNoFill( false )
	, m_bForceRTL( bForceRTL )
	, m_bFlashCaptionHighlightedState( false )
	, m_clrFlashCaptionText( COLORREF(-1L) )
	, m_clrFlashCaptionBackground( COLORREF(-1L) )
{
}

void CExtPaintManager::PaintGripper(
	CDC & dc,
	CExtPaintManager::PAINTGRIPPERDATA & _pgd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rectGripper2( _pgd.m_rcGripper );
COLORREF clrBtnShadow =  GetColor( CLR_3DSHADOW_OUT, _pgd.m_pHelperSrc, _pgd.m_lParam );
COLORREF clrBtnHilight = GetColor( CLR_3DHILIGHT_OUT, _pgd.m_pHelperSrc, _pgd.m_lParam );
	if( _pgd.m_bSideBar || _pgd.m_bFloating )
	{
		if( ! _pgd.m_bFloating )
			rectGripper2.DeflateRect( 1, 1 );
#if (defined COLOR_GRADIENTACTIVECAPTION && defined COLOR_GRADIENTACTIVECAPTION)
		ASSERT( COLOR_GRADIENTACTIVECAPTION == 27 );
		ASSERT( COLOR_GRADIENTINACTIVECAPTION == 28 );
#endif
		INT nTextColorIndex = COLOR_HIGHLIGHTTEXT;
		if( _pgd.m_bFlashCaptionHighlightedState )
		{
			ASSERT( _pgd.m_clrFlashCaptionBackground != COLORREF(-1L) );
			ASSERT( _pgd.m_clrFlashCaptionText != COLORREF(-1L) );
			dc.FillSolidRect( rectGripper2, _pgd.m_clrFlashCaptionBackground );
		} // if( _pgd.m_bFlashCaptionHighlightedState )
		else if( _pgd.m_bSideBar )
		{
#ifdef __FREIWALD_ORIGINAL__
			if( _pgd.m_bFloating || _pgd.m_bActive )
#else
/*
			// draw an active gripper only, 
			// when the bar actually has the focus
			// (and not, whenever it is floating)
*/
			if (_pgd.m_bActive)
#endif
			{
				nTextColorIndex = COLOR_CAPTIONTEXT;
				if( ! _pgd.m_bHelperNoFill )
				{
					if( stat_GetBPP() > 8 )
					{
						COLORREF clrLeft = GetColor( COLOR_ACTIVECAPTION, _pgd.m_pHelperSrc, _pgd.m_lParam );
						COLORREF clrRight = GetColor( 27, _pgd.m_pHelperSrc, _pgd.m_lParam );
						if( (!_pgd.m_bHorz) && _pgd.m_bForceRTL )
						{
							COLORREF clrTemp = clrLeft;
							clrLeft = clrRight;
							clrRight = clrTemp;
						} // if( (!_pgd.m_bHorz) && _pgd.m_bForceRTL )
						stat_PaintGradientRect( dc, rectGripper2, clrLeft, clrRight, _pgd.m_bHorz );
					} // if( stat_GetBPP() > 8 )
					else
						dc.FillSolidRect( rectGripper2, GetColor( COLOR_ACTIVECAPTION, _pgd.m_pHelperSrc, _pgd.m_lParam ) );
				} // if( ! _pgd.m_bHelperNoFill )
			}
			else
			{
				nTextColorIndex = COLOR_INACTIVECAPTIONTEXT;
				if( ! _pgd.m_bHelperNoFill )
				{
					if( stat_GetBPP() > 8 )
					{
						COLORREF clrLeft = GetColor( COLOR_INACTIVECAPTION, _pgd.m_pHelperSrc, _pgd.m_lParam );
						COLORREF clrRight = GetColor( 28, _pgd.m_pHelperSrc, _pgd.m_lParam );
						if( (!_pgd.m_bHorz) && _pgd.m_bForceRTL )
						{
							COLORREF clrTemp = clrLeft;
							clrLeft = clrRight;
							clrRight = clrTemp;
						} // if( (!_pgd.m_bHorz) && _pgd.m_bForceRTL )
						stat_PaintGradientRect( dc, rectGripper2, clrLeft, clrRight, _pgd.m_bHorz );
					} // if( stat_GetBPP() > 8 )
					else
						dc.FillSolidRect( rectGripper2, GetColor( COLOR_INACTIVECAPTION, _pgd.m_pHelperSrc, _pgd.m_lParam ) );
				} // if( !_pgd.m_bHelperNoFill )
			}
		} // else if( _pgd.m_bSideBar )
		else
			dc.FillSolidRect( rectGripper2, GetColor( COLOR_HIGHLIGHT, _pgd.m_pHelperSrc, _pgd.m_lParam ) );
		int nTextLen = 0;
		if(		_pgd.m_sCaption != NULL
			&&	( nTextLen = int(_tcslen(_pgd.m_sCaption)) ) > 0
			&&	(! _pgd.m_rcText.IsRectEmpty() )
			&&	_pgd.m_rcText.right > _pgd.m_rcText.left
			&&	_pgd.m_rcText.bottom > _pgd.m_rcText.top
			)
		{
			COLORREF clrText = _pgd.m_bFlashCaptionHighlightedState ? _pgd.m_clrFlashCaptionText : GetColor( nTextColorIndex, _pgd.m_pHelperSrc, _pgd.m_lParam );
			COLORREF clrOldText = dc.SetTextColor(clrText);
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CFont * pCurrFont = ( _pgd.m_bSideBar && (!_pgd.m_bFloating) ) ? (&m_FontNormalBC) : (&m_FontBoldBC);
			CFont * pOldFont = dc.SelectObject( pCurrFont );
			ASSERT( pOldFont != NULL );
			CRect rcDrawText( _pgd.m_rcText );
			CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
			if( _pgd.m_bHorz )
				eLO = CExtRichContentLayout::__ELOT_270_CW;
			UINT nDtAlign = _pgd.m_bForceRTL ? DT_RIGHT : DT_LEFT;
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, _pgd.m_sCaption, nTextLen, rcDrawText, nDtAlign|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS, 0
				);
			dc.SelectObject( pOldFont );
			dc.SetBkMode(nOldBkMode);
			dc.SetTextColor(clrOldText);
		}
	} // if( _pgd.m_bSideBar || _pgd.m_bFloating )
	else
	{
		if( _pgd.m_bHorz )
		{
			rectGripper2.DeflateRect( 4, 1, 4, 0 );
			rectGripper2.right = rectGripper2.left + 3;
			dc.Draw3dRect( rectGripper2, clrBtnHilight, clrBtnShadow );
			rectGripper2.OffsetRect( 3, 0 );
			dc.Draw3dRect( rectGripper2, clrBtnHilight, clrBtnShadow );
		} // if( bHorz )
		else
		{
			rectGripper2.OffsetRect( 1, 0 );
			rectGripper2.DeflateRect( 0, 4, 1, 4 );
			rectGripper2.bottom = rectGripper2.top + 3;
			dc.Draw3dRect( rectGripper2, clrBtnHilight, clrBtnShadow );
			rectGripper2.OffsetRect( 0, 3 );
			dc.Draw3dRect( rectGripper2, clrBtnHilight, clrBtnShadow );
		} // else from if( bHorz )
	} // else from if( _pgd.m_bSideBar || _pgd.m_bFloating )
}

void CExtPaintManagerXP::PaintGripper(
	CDC & dc,
	CExtPaintManager::PAINTGRIPPERDATA & _pgd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rectGripper( _pgd.m_rcGripper );
	if( _pgd.m_bSideBar || _pgd.m_bFloating )
	{
		if( _pgd.m_bFlashCaptionHighlightedState )
		{
			ASSERT( _pgd.m_clrFlashCaptionBackground != COLORREF(-1L) );
			ASSERT( _pgd.m_clrFlashCaptionText != COLORREF(-1L) );
			dc.FillSolidRect( rectGripper, _pgd.m_clrFlashCaptionBackground );
		} // if( _pgd.m_bFlashCaptionHighlightedState )
		else
		{
			COLORREF clrGrip =
				GetColor(
					_pgd.m_bSideBar ? ( _pgd.m_bActive ? COLOR_HIGHLIGHT : XPCLR_3DFACE_DARK ) : COLOR_3DSHADOW,
					_pgd.m_pHelperSrc,
					_pgd.m_lParam 
					);
			if( ! _pgd.m_bHelperNoFill )
				dc.FillSolidRect( rectGripper, clrGrip );
			if(		_pgd.m_bSideBar
				&&	(!_pgd.m_bHelperNoFill)
				&&	(!_pgd.m_bFloating)
				&&	(!_pgd.m_bActive)
				)
			{ // rect border of docked bar caption like VS-.NET-7.0
				COLORREF clrSideRect = GetColor( COLOR_3DSHADOW, _pgd.m_pHelperSrc, _pgd.m_lParam );
				stat_PaintDotNet3dRect( dc, rectGripper, clrSideRect, clrSideRect );
			} // rect border of docked bar caption like VS-.NET-7.0
		} // else from  // if( _pgd.m_bFlashCaptionHighlightedState )
		int nTextLen = 0;
		if(		_pgd.m_sCaption != NULL
			&&	( nTextLen = int(_tcslen(_pgd.m_sCaption)) ) > 0
			&&	(! _pgd.m_rcText.IsRectEmpty() )
			&&	_pgd.m_rcText.right > _pgd.m_rcText.left
			&&	_pgd.m_rcText.bottom > _pgd.m_rcText.top
			)
		{
			e_paint_manager_name_t ePMN = OnQueryPaintManagerName();
			e_system_theme_t eST = OnQuerySystemTheme();
			COLORREF clrText = _pgd.m_clrFlashCaptionText;
			if( ! _pgd.m_bFlashCaptionHighlightedState )
			{
				if( _pgd.m_bActive || (! _pgd.m_bSideBar) )
					clrText = GetColor(
							COLOR_HIGHLIGHTTEXT,
							_pgd.m_pHelperSrc, _pgd.m_lParam
							);
				else
					clrText = GetColor( COLOR_BTNTEXT, _pgd.m_pHelperSrc, _pgd.m_lParam );
			}
			COLORREF clrOldText =
				dc.SetTextColor(clrText);
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CFont * pCurrFont = ( _pgd.m_bSideBar ? (&m_FontNormalBC) : (&m_FontBoldBC) );
			CFont * pOldFont = dc.SelectObject( pCurrFont );
			CRect rcDrawText( _pgd.m_rcText );
			CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
			if( _pgd.m_bHorz )
				eLO = CExtRichContentLayout::__ELOT_270_CW;
			UINT nDtAlign = _pgd.m_bForceRTL ? DT_RIGHT : DT_LEFT;
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, _pgd.m_sCaption, nTextLen, rcDrawText, nDtAlign|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS, 0
				);
			dc.SelectObject( pOldFont );
			dc.SetBkMode( nOldBkMode );
			dc.SetTextColor( clrOldText );
		}
	} // if( _pgd.m_bSideBar || _pgd.m_bFloating )
	else
	{
		COLORREF clrGrip = GetColor( CLR_3DFACE_OUT, _pgd.m_pHelperSrc, _pgd.m_lParam );
		CRect rcGripToClear( _pgd.m_rcGripper );
		rcGripToClear.DeflateRect( 1, 1 );
		if(		_pgd.m_pHelperSrc != NULL
			&&	_pgd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
			&&	((CControlBar*)_pgd.m_pHelperSrc)->m_pDockSite != NULL
			&&	((CExtToolControlBar*)_pgd.m_pHelperSrc)->m_bForceNoBalloonWhenRedockable
			)
		{
		}
		else
			dc.FillSolidRect( rcGripToClear, clrGrip );
		CPen pen;
		pen.CreatePen( PS_SOLID, 1, GetColor( XPCLR_GRIPPER, _pgd.m_pHelperSrc, _pgd.m_lParam ) );
		CPen * pOldPen = dc.SelectObject( &pen );
		if( _pgd.m_bHorz )
		{
			rectGripper.DeflateRect( 4, 1, 6, 2 );
			rectGripper.right = rectGripper.left + 5;
			rectGripper.DeflateRect( 1, 0 );
			CRect rcLine( rectGripper );
			INT nLineCount = rectGripper.Height() / (1 + 1);
			rcLine.top += rcLine.Height() - (1 + 1) * nLineCount;
			rcLine.bottom = rcLine.top + 1;
			for( INT nLine = 0; nLine < nLineCount; nLine++ )
			{
				dc.MoveTo( rcLine.left, rcLine.top + 1 );
				dc.LineTo( rcLine.right, rcLine.bottom );
				rcLine.OffsetRect( 0, 1 + 1 );
			}
		}
		else
		{
			rectGripper.OffsetRect( 1, 0 );
			rectGripper.DeflateRect( 1, 4, 2, 6 );
			rectGripper.bottom = rectGripper.top + 5;
			rectGripper.DeflateRect( 0, 1 );
			CRect rcLine( rectGripper );
			int nLineCount = rectGripper.Width() / (1 + 1);
			rcLine.left += rcLine.Width() - (1 + 1) * nLineCount;
			rcLine.right = rcLine.left + 1;
			for( int nLine = 0; nLine < nLineCount; nLine++ )
			{
				dc.MoveTo( rcLine.left + 1, rcLine.top );
				dc.LineTo( rcLine.right, rcLine.bottom );
				rcLine.OffsetRect( 1 + 1, 0 );
			}
		} // else from if( _pgd.m_bHorz )
		dc.SelectObject( pOldPen );
	} // else from if( _pgd.m_bSideBar || _pgd.m_bFloating )
}

void CExtPaintManagerNativeXP::PaintGripper(
	CDC & dc,
	CExtPaintManager::PAINTGRIPPERDATA & _pgd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintGripper( dc, _pgd );
		return;
	}
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, _pgd.m_pHelperSrc );
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
	if(		(!_pgd.m_bSideBar)
		&&	(!_pgd.m_bFloating) 
		)
	{
		if( g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_REBAR ) != NULL )
		{
			INT nPartID = _pgd.m_bHorz ? RP_GRIPPER : RP_GRIPPERVERT;
			CSize szPart(0,0);
			VERIFY( 
				g_PaintManager.m_UxTheme.GetThemePartSize(
					dc.GetSafeHdc(), 
					nPartID, 
					0, 
					NULL, 
					CExtUxTheme::__EXT_UX_TS_TRUE,
					&szPart
					) == S_OK
				);

			CRect rcGripper( 
				0, 
				0, 
				_pgd.m_bHorz ? szPart.cx : _pgd.m_rcGripper.Width(),
				_pgd.m_bHorz ? _pgd.m_rcGripper.Height() : szPart.cy
				);
			rcGripper.OffsetRect(
				_pgd.m_rcGripper.left + ( _pgd.m_rcGripper.Width() - rcGripper.Width() ) / 2,
				_pgd.m_rcGripper.top + ( _pgd.m_rcGripper.Height() - rcGripper.Height() ) / 2
				);

//			if( _pgd.m_bFlashCaptionHighlightedState )
//			{
//				ASSERT( _pgd.m_clrFlashCaptionBackground != COLORREF(-1L) );
//				ASSERT( _pgd.m_clrFlashCaptionText != COLORREF(-1L) );
//				dc.FillSolidRect(
//					rcGripper,
//					_pgd.m_clrFlashCaptionBackground
//					);
//			} // if( _pgd.m_bFlashCaptionHighlightedState )
//			else
			{
				//VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						pWnd->GetSafeHwnd(),
						dc.GetSafeHdc(), 
						nPartID, 
						0, 
						&rcGripper, 
						&_pgd.m_rcGripper
						//) == S_OK
					);
			} // else from if( _pgd.m_bFlashCaptionHighlightedState )

			g_PaintManager.m_UxTheme.CloseThemeData();
		}
	}
	else
	{
		INT nTextColorIndex = COLOR_HIGHLIGHTTEXT;
		if( !_pgd.m_bFloating )
		{
#if (defined COLOR_GRADIENTACTIVECAPTION && defined COLOR_GRADIENTACTIVECAPTION)
			ASSERT( COLOR_GRADIENTACTIVECAPTION == 27 );
			ASSERT( COLOR_GRADIENTINACTIVECAPTION == 28 );
#endif
			if( _pgd.m_bFlashCaptionHighlightedState )
			{
				ASSERT( _pgd.m_clrFlashCaptionBackground != COLORREF(-1L) );
				ASSERT( _pgd.m_clrFlashCaptionText != COLORREF(-1L) );
				dc.FillSolidRect( _pgd.m_rcGripper, _pgd.m_clrFlashCaptionBackground );
			} // if( _pgd.m_bFlashCaptionHighlightedState )
			else if( _pgd.m_bActive )
			{
				nTextColorIndex = COLOR_CAPTIONTEXT;
				if( _pgd.m_bFlashCaptionHighlightedState )
				{
					ASSERT( _pgd.m_clrFlashCaptionBackground != COLORREF(-1L) );
					ASSERT( _pgd.m_clrFlashCaptionText != COLORREF(-1L) );
					dc.FillSolidRect( _pgd.m_rcGripper, _pgd.m_clrFlashCaptionBackground );
				} // if( _pgd.m_bFlashCaptionHighlightedState )
				else if( !_pgd.m_bHelperNoFill )
				{
					if( stat_GetBPP() > 8 )
					{
						COLORREF clrLeft = ::GetSysColor( COLOR_ACTIVECAPTION );
						COLORREF clrRight = ::GetSysColor( 27 );
						if( (!_pgd.m_bHorz) && _pgd.m_bForceRTL )
						{
							COLORREF clrTemp = clrLeft;
							clrLeft = clrRight;
							clrRight = clrTemp;
						}
						stat_PaintGradientRect( dc, _pgd.m_rcGripper, clrLeft, clrRight, _pgd.m_bHorz );
					}
					else
						dc.FillSolidRect( _pgd.m_rcGripper, ::GetSysColor( COLOR_ACTIVECAPTION ) );
				}
			}
			else
			{
				nTextColorIndex = COLOR_INACTIVECAPTIONTEXT;
				if( !_pgd.m_bHelperNoFill )
				{
					if( stat_GetBPP() > 8 )
					{
						COLORREF clrLeft = ::GetSysColor( COLOR_INACTIVECAPTION );
						COLORREF clrRight = ::GetSysColor( 28 );
						if( (!_pgd.m_bHorz) && _pgd.m_bForceRTL )
						{
							COLORREF clrTemp = clrLeft;
							clrLeft = clrRight;
							clrRight = clrTemp;
						}
						stat_PaintGradientRect( dc, _pgd.m_rcGripper, clrLeft, clrRight, _pgd.m_bHorz );
					}
					else
						dc.FillSolidRect( _pgd.m_rcGripper, ::GetSysColor( COLOR_INACTIVECAPTION ) );
				}
			}
		} // if( !_pgd.m_bFloating )
		else
		{
				if( _pgd.m_bFlashCaptionHighlightedState )
				{
					ASSERT( _pgd.m_clrFlashCaptionBackground != COLORREF(-1L) );
					ASSERT( _pgd.m_clrFlashCaptionText != COLORREF(-1L) );
					dc.FillSolidRect( _pgd.m_rcGripper, _pgd.m_clrFlashCaptionBackground );
				} // if( _pgd.m_bFlashCaptionHighlightedState )
                else
                {
                    // pfortier - change made here to support ViewCelListBox
                    dc.FillSolidRect(_pgd.m_rcGripper, ::GetSysColor(COLOR_INACTIVECAPTION));
                }
		}

		int nTextLen = 0;
		if(		_pgd.m_sCaption != NULL
			&&	( nTextLen = int(_tcslen(_pgd.m_sCaption)) ) > 0
			&&	(! _pgd.m_rcText.IsRectEmpty() )
			&&	_pgd.m_rcText.right > _pgd.m_rcText.left
			&&	_pgd.m_rcText.bottom > _pgd.m_rcText.top
			)
		{
			COLORREF clrText = _pgd.m_bFlashCaptionHighlightedState ? _pgd.m_clrFlashCaptionText : ::GetSysColor( nTextColorIndex ) ;
			COLORREF clrOldText =
				dc.SetTextColor(clrText);
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CFont * pCurrFont = ( _pgd.m_bSideBar && (!_pgd.m_bFloating) ) ? (&m_FontNormalBC) : (&m_FontBoldBC) ;
			CFont * pOldFont = dc.SelectObject( pCurrFont );
			ASSERT( pOldFont != NULL );
			CRect rcDrawText( _pgd.m_rcText );
			CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
			if( _pgd.m_bHorz )
				eLO = CExtRichContentLayout::__ELOT_270_CW;
			UINT nDtAlign = _pgd.m_bForceRTL ? DT_RIGHT : DT_LEFT;
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, _pgd.m_sCaption, nTextLen, rcDrawText, nDtAlign|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS, 0
				);
			dc.SelectObject( pOldFont );
			dc.SetBkMode(nOldBkMode);
			dc.SetTextColor(clrOldText);
		}
	}
}

CSize CExtPaintManager::GetPushedOffset()
{
	ASSERT_VALID( this );
	return CSize( 1, 1 );
}

CSize CExtPaintManagerXP::GetPushedOffset()
{
	ASSERT_VALID( this );
	return CSize( 0, 0 );
}

CSize CExtPaintManagerNativeXP::GetPushedOffset()
{
	ASSERT_VALID( this );
	if( g_PaintManager.m_UxTheme.IsControlsThemed() )
		return CSize( 0, 0 );
	return CExtPaintManager::GetPushedOffset();
}

#define __XP_DSS_MONO DSS_MONO

void CExtPaintManagerXP::InitTranslatedColors()
{
	ASSERT_VALID( this );

	m_DSS_DISABLED_style = __XP_DSS_MONO;

	CExtPaintManager::InitTranslatedColors();

	if( ! IsHighContrast() )
	{
		int nBitsPerPixel = stat_GetBPP();
		
		COLORREF clrSysWindow = ::GetSysColor(COLOR_WINDOW);
		COLORREF clrSys3dFace = ::GetSysColor(COLOR_3DFACE);
		COLORREF clrSys3dHilight = ::GetSysColor(COLOR_3DHILIGHT);
		COLORREF clrSys3dShadow = ::GetSysColor(COLOR_3DSHADOW);
		COLORREF clrSys3dDkShadow = ::GetSysColor(COLOR_3DDKSHADOW);
		COLORREF clrSysBtnText = ::GetSysColor(COLOR_BTNTEXT);
		
		COLORREF xpclr_MenuLight = clrSysWindow;
		COLORREF xpclr_ControlBarBk = clrSys3dFace;
		COLORREF xpclr_Highlight = clrSys3dFace;
		COLORREF xpclr_Separator = clrSys3dShadow;
		COLORREF xpclr_Gripper = clrSys3dShadow;
		COLORREF xpclr_PanelBorder = clrSys3dShadow;
		COLORREF xpclr_RarelyUsedMenuLeft = clrSys3dShadow;
		COLORREF xpclr_HighlightDarked = clrSys3dShadow;
		COLORREF xpclr_HighlightBorder = clrSysBtnText;
		
		if( nBitsPerPixel > 8 )
		{
			// xp - menu face
			xpclr_MenuLight =
				CExtBitmap::stat_RGB_Enlight(
				::GetSysColor( COLOR_3DFACE ), 
				::GetSysColor( COLOR_WINDOW ),
				143
				);
			
			// xp - rarely used item left side
			xpclr_RarelyUsedMenuLeft =
				CExtBitmap::stat_RGB_Enlight(
				::GetSysColor( COLOR_3DFACE ), 
				::GetSysColor( COLOR_3DSHADOW ),
				90
				);
			
			// xp - control bar background
			xpclr_ControlBarBk =
				CExtBitmap::stat_RGB_Enlight(
				::GetSysColor( COLOR_WINDOW ), 
				::GetSysColor( COLOR_3DFACE ),
				165
				);
			
			// xp - blue like light ver
			xpclr_Highlight =
				CExtBitmap::stat_RGB_Enlight(
				::GetSysColor( COLOR_HIGHLIGHT ), 
				::GetSysColor( COLOR_WINDOW ),
				30
				);
			
			// xp - blue like dark ver
			xpclr_HighlightDarked =
				CExtBitmap::stat_RGB_Enlight(
				::GetSysColor( COLOR_HIGHLIGHT ), 
				::GetSysColor( COLOR_WINDOW ),
				50
				);
			
			// xp - blue dark selected border
			xpclr_HighlightBorder = 
				::GetSysColor( COLOR_HIGHLIGHT );
			
			// xp - dark gray separator
			xpclr_Separator = 
				CExtBitmap::stat_RGB_Enlight(
				::GetSysColor( COLOR_3DSHADOW ), 
				::GetSysColor( COLOR_WINDOW ),
				70
				);

			// xp - dark gray gripper
			xpclr_Gripper = 
				CExtBitmap::stat_RGB_Enlight(
				::GetSysColor( COLOR_3DSHADOW ), 
				::GetSysColor( COLOR_WINDOW ),
				50
				);
			
			// xp - dark panel border (for floating control bars)
			xpclr_PanelBorder =
				RGB(
				( 750L*long(GetRValue(clrSys3dShadow)) + 250L*long(GetRValue(clrSys3dDkShadow)) ) / 1000L,
				( 750L*long(GetGValue(clrSys3dShadow)) + 250L*long(GetGValue(clrSys3dDkShadow)) ) / 1000L,
				( 750L*long(GetBValue(clrSys3dShadow)) + 250L*long(GetBValue(clrSys3dDkShadow)) ) / 1000L
				);
		} // if( nBitsPerPixel > 8 )
		
		m_mapColorTranslate[CLR_3DFACE_OUT]				= InstallColor( xpclr_ControlBarBk );
		m_mapColorTranslate[CLR_3DFACE_IN]				= InstallColor( xpclr_Highlight );
		m_mapColorTranslate[CLR_3DFACE_DISABLED]		= COLOR_3DFACE;
		
		int idxClrDark = InstallColor( xpclr_PanelBorder );
		
		m_mapColorTranslate[CLR_3DLIGHT_OUT]			= idxClrDark;
		m_mapColorTranslate[CLR_3DLIGHT_IN]				= idxClrDark;
		m_mapColorTranslate[CLR_3DLIGHT_DISABLED]		= idxClrDark;
		
		m_mapColorTranslate[CLR_3DHILIGHT_OUT]			= idxClrDark;
		m_mapColorTranslate[CLR_3DHILIGHT_IN]			= idxClrDark;
		m_mapColorTranslate[CLR_3DHILIGHT_DISABLED]		= idxClrDark;
		
		m_mapColorTranslate[CLR_3DSHADOW_OUT]			= idxClrDark;
		m_mapColorTranslate[CLR_3DSHADOW_IN]			= idxClrDark;
		m_mapColorTranslate[CLR_3DSHADOW_DISABLED]		= idxClrDark;
		
		m_mapColorTranslate[CLR_3DDKSHADOW_OUT]			= idxClrDark;
		m_mapColorTranslate[CLR_3DDKSHADOW_IN]			= idxClrDark;
		m_mapColorTranslate[CLR_3DDKSHADOW_DISABLED]	= idxClrDark;
		
		m_mapColorTranslate[CLR_TEXT_OUT]				= COLOR_WINDOWTEXT;
		m_mapColorTranslate[CLR_TEXT_IN] =
			InstallColor(
				(nBitsPerPixel > 8)
					? RGB(0,0,0)
					: clrSys3dHilight
			);
		
		m_mapColorTranslate[XPCLR_TEXT_FIELD_BORDER_NORMAL]		= COLOR_3DFACE;
		m_mapColorTranslate[XPCLR_TEXT_FIELD_BORDER_DISABLED]	= InstallColor( xpclr_Separator );
		
		m_mapColorTranslate[XPCLR_PUSHEDHOVERTEXT]			= COLOR_HIGHLIGHTTEXT;
		
		m_mapColorTranslate[XPCLR_3DFACE_DARK]				= InstallColor( xpclr_ControlBarBk );
		m_mapColorTranslate[XPCLR_3DFACE_NORMAL]			= InstallColor( xpclr_MenuLight );
		m_mapColorTranslate[XPCLR_SEPARATOR]				= InstallColor( xpclr_Separator ); 
		
		m_mapColorTranslate[XPCLR_RARELY_BORDER]			= InstallColor( xpclr_RarelyUsedMenuLeft );
		
		m_mapColorTranslate[CLR_TEXT_DISABLED]				= COLOR_GRAYTEXT;

		// Menu text colors
		m_mapColorTranslate[CLR_MENUTEXT_OUT]				= COLOR_WINDOWTEXT;
		m_mapColorTranslate[CLR_MENUTEXT_IN]				= m_mapColorTranslate[CLR_TEXT_IN];
		m_mapColorTranslate[CLR_MENUTEXT_DISABLED]			= COLOR_GRAYTEXT;
		m_mapColorTranslate[XPCLR_HILIGHT]					= InstallColor( xpclr_HighlightDarked ); 
		m_mapColorTranslate[XPCLR_HILIGHT_BORDER]			= InstallColor( xpclr_HighlightBorder ); 
		m_mapColorTranslate[XPCLR_HILIGHT_BORDER_SELECTED]	= InstallColor( xpclr_HighlightBorder ); 
		
		m_mapColorTranslate[XPCLR_MENU_BORDER] =
			InstallColor(
				CExtBitmap::stat_RGB_Enlight(
					::GetSysColor( COLOR_BTNTEXT ), 
					::GetSysColor( COLOR_BTNSHADOW ),
					20
					)
				);
		
		m_mapColorTranslate[XPCLR_3DFACE_FLOAT_R]		= InstallColor( xpclr_MenuLight );
		m_mapColorTranslate[XPCLR_3DFACE_FLOAT_F]		= InstallColor( GetColor( CLR_3DFACE_OUT, this ) );

		m_mapColorTranslate[XPCLR_GRIPPER]				= InstallColor( xpclr_Gripper ); 		
		
		// Task Pane Colors
		m_mapColorTranslate[CLR_TASK_PANE_BK_TOP]		= InstallColor( xpclr_MenuLight );
		m_mapColorTranslate[CLR_TASK_PANE_BK_BOTTOM]	= InstallColor( xpclr_MenuLight );
		
		m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_TEXT_NORMAL] = 
			COLOR_WINDOWTEXT;
		m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_TEXT_NORMAL_HOVERED] = 
			COLOR_WINDOWTEXT;
		m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_TEXT_HIGHLIGHTED] = 
			COLOR_WINDOWTEXT;
		m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_TEXT_HIGHLIGHTED_HOVERED] =
			COLOR_WINDOWTEXT;
	}
	else
	{
		m_mapColorTranslate[CLR_3DFACE_IN]						= COLOR_HIGHLIGHT;

		m_mapColorTranslate[XPCLR_TEXT_FIELD_BORDER_NORMAL]		= COLOR_3DFACE;
		m_mapColorTranslate[XPCLR_TEXT_FIELD_BORDER_DISABLED]	= COLOR_3DSHADOW;
		
		m_mapColorTranslate[XPCLR_PUSHEDHOVERTEXT]				= COLOR_HIGHLIGHTTEXT;
		
		m_mapColorTranslate[XPCLR_3DFACE_DARK]					= COLOR_3DFACE;
		m_mapColorTranslate[XPCLR_3DFACE_NORMAL]				= COLOR_3DFACE;
		m_mapColorTranslate[XPCLR_SEPARATOR]					= COLOR_3DSHADOW; 
		
		m_mapColorTranslate[XPCLR_RARELY_BORDER]				= COLOR_3DFACE;

		m_mapColorTranslate[XPCLR_HILIGHT]						= COLOR_HIGHLIGHT; 
		m_mapColorTranslate[XPCLR_HILIGHT_BORDER]				= InstallColor( RGB(255,255,255) );
		m_mapColorTranslate[XPCLR_HILIGHT_BORDER_SELECTED]		= InstallColor( RGB(0,0,0) );
		
		m_mapColorTranslate[XPCLR_MENU_BORDER]					= InstallColor( RGB(255,255,255) );

		m_mapColorTranslate[XPCLR_3DFACE_FLOAT_R]				= COLOR_3DFACE;
		m_mapColorTranslate[XPCLR_3DFACE_FLOAT_F]				= COLOR_3DFACE;
	
		m_mapColorTranslate[XPCLR_GRIPPER]						= COLOR_3DSHADOW;
	}
}

void CExtPaintManagerXP::PaintPushButton(
	CDC & dc,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( _ppbd.m_pHelperSrc != NULL )
	{
		CExtBarButton * pTBB = DYNAMIC_DOWNCAST( CExtBarButton, _ppbd.m_pHelperSrc );
		if( pTBB != NULL )
		{
			if( pTBB->IsRibbonPaintingMode() )
			{
				Ribbon_PaintPushButton( dc, _ppbd );
				return;
			}
			CExtToolControlBar * pBar = pTBB->GetBar();
			if(		pBar != NULL
				&&	pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) )
				&&	(! pTBB->IsNoRibbonLayout() )
				&&	(! pTBB->IsKindOf( RUNTIME_CLASS( CExtBarMdiRightButton ) ) )
				)
			{
				Ribbon_PaintPushButton( dc, _ppbd );
				return;
			}
		} // if( pTBB != NULL )
	} // if( _ppbd.m_pHelperSrc != NULL )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

bool bRTL = _ppbd.IsRTL();

	if( !_ppbd.m_bEnabled )
	{
		_ppbd.m_bHover = false;
		_ppbd.m_bDrawFocusRect = false;
		_ppbd.m_bIndeterminate = false;
	} // if( ! _ppbd.m_bEnabled )

COLORREF clrOldText = dc.GetTextColor();
COLORREF clrOldBk = dc.GetBkColor();
int nOldBkMode = dc.SetBkMode( TRANSPARENT );

int nIdxMainGradientLeft = -1;
int nIdxMainGradientRight = -1;
COLORREF bgColor, bgColorDD;
	if( _ppbd.m_clrForceBk != ((COLORREF)-1L) )
		bgColor = bgColorDD = _ppbd.m_clrForceBk;
	else if(_ppbd.m_bHover 
		||	_ppbd.m_bPushed 
		||	_ppbd.m_bIndeterminate
		||	_ppbd.m_bPushedDropDown 
		||	_ppbd.m_bDefaultPushButton
		)
	{
		bgColor = bgColorDD = GetColor( CLR_3DFACE_IN, _ppbd.m_pHelperSrc, _ppbd.m_lParam );
		if( _ppbd.m_bPushed || _ppbd.m_bPushedDropDown || _ppbd.m_bDefaultPushButton )
		{
			if( _ppbd.m_bDefaultPushButton )
				bgColorDD = bgColor = dc.GetNearestColor( stat_HLS_Adjust( GetColor( XPCLR_HILIGHT ), 0.0, 0.2, 0.0 ) );
			if( _ppbd.m_bPushed || _ppbd.m_bPushedDropDown )
				(( _ppbd.m_bPushedDropDown ) ? bgColorDD : bgColor ) =
					GetColor( _ppbd.m_bHover ? XPCLR_HILIGHT : CLR_3DFACE_IN, _ppbd.m_pHelperSrc, _ppbd.m_lParam  );
		}
		if(		(!_ppbd.m_bHover)
			&&	_ppbd.m_bPushed
			&&	(!_ppbd.m_bPushedDropDown)
			&&	_ppbd.m_bSeparatedDropDown
			)
			bgColorDD =  GetColor( CLR_3DFACE_OUT, _ppbd.m_pHelperSrc, _ppbd.m_lParam );
		if( _ppbd.m_bIndeterminate && ( !_ppbd.m_bHover ) && ( !_ppbd.m_bPushed ) )
			bgColor = dc.GetNearestColor( stat_HLS_Adjust( bgColor, 0.0, 0.5, 0.5 ) );
		
		if( _ppbd.m_bHover || ( _ppbd.m_bDefaultPushButton && (! _ppbd.m_bPushed ) ) )
		{
			if( _ppbd.m_bPushed )
			{
				nIdxMainGradientLeft = m_nIdxClrBtnHovPresLeft;
				nIdxMainGradientRight = m_nIdxClrBtnHovPresRight;
			} // if( _ppbd.m_bPushed )
			else
			{
				nIdxMainGradientLeft = m_nIdxClrBtnHoverLeft;
				nIdxMainGradientRight = m_nIdxClrBtnHoverRight;
			} // else from if( _ppbd.m_bPushed )
		} // if( _ppbd.m_bHover )
		else if( _ppbd.m_bPushed )
		{
			nIdxMainGradientLeft = m_nIdxClrBtnPressedLeft;
			nIdxMainGradientRight = m_nIdxClrBtnPressedRight;
		} // else if( _ppbd.m_bPushed )
	} // if(	_ppbd.m_bHover || _ppbd.m_bPushed || _ppbd.m_bIndeterminate || _ppbd.m_bPushedDropDown )
	else
	{
		bgColor = bgColorDD =
			GetColor(
				_ppbd.m_bFlat ? CLR_3DFACE_OUT : COLOR_3DFACE,
				_ppbd.m_pHelperSrc,
				_ppbd.m_lParam 
				);
	}

COLORREF clrDkBorder =
		GetColor(
			(_ppbd.m_bHover || _ppbd.m_bPushed || _ppbd.m_bPushedDropDown )
				? XPCLR_HILIGHT_BORDER
				: ( _ppbd.m_bFlat ? XPCLR_HILIGHT_BORDER : COLOR_3DSHADOW )
				,
			_ppbd.m_pHelperSrc,
			_ppbd.m_lParam 
			);
bool bPushedDropDown = false;
	if( (_ppbd.m_bPushed && !_ppbd.m_bSeparatedDropDown)
		|| _ppbd.m_bPushedDropDown
		)
		bPushedDropDown = true;

CRect rectClient( _ppbd.m_rcClient );
CRect rectCaption( _ppbd.m_rcClient );
CRect rectDropDown( 0, 0, 0, 0 );
	if( _ppbd.m_bDropDown )
	{
		rectDropDown = rectClient;
		if( _ppbd.m_bHorz )
		{
			if( bRTL )
			{
				rectCaption.left += _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
				rectDropDown.right = rectCaption.left + 1;
			} // if( bRTL )
			else
			{
				rectCaption.right -= _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
				rectDropDown.left = rectCaption.right - 1;
			} // else from if( bRTL )
		} // if( _ppbd.m_bHorz )
		else
		{
			rectCaption.bottom -= _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
			rectDropDown.top = rectCaption.bottom - 1;
		} // if( _ppbd.m_bHorz )

		if(		(! _ppbd.m_bTransparentBackground )
			&&	( _ppbd.m_bHover || _ppbd.m_bPushedDropDown )
			||	_ppbd.m_clrForceBk != ((COLORREF)-1L)
			)
			dc.FillSolidRect( &rectDropDown, bgColorDD );

		if( _ppbd.m_bSeparatedDropDown )
		{
			rectClient = rectCaption;

			if( bPushedDropDown )
			{
				if( (!_ppbd.m_bFlat) || _ppbd.m_bDrawBorder )
					dc.Draw3dRect( rectDropDown, clrDkBorder, clrDkBorder );
			} // if( bPushedDropDown )
			else
			{
				if( (!_ppbd.m_bFlat) || (_ppbd.m_bHover && _ppbd.m_bDrawBorder) )
					dc.Draw3dRect( rectDropDown, clrDkBorder, clrDkBorder );
			} // else from if( bPushedDropDown )
		} // if( _ppbd.m_bSeparatedDropDown )
	} // if( _ppbd.m_bDropDown )

	if(		( ! _ppbd.m_bTransparentBackground ) 
		||	_ppbd.m_bIndeterminate
		||	_ppbd.m_clrForceBk != ((COLORREF)-1L)
		)
	{
		if( nIdxMainGradientLeft >= 0 && nIdxMainGradientRight >= 0 )
		{
			COLORREF clrLeft  = GetColor( _ppbd.m_bHorz ? nIdxMainGradientRight : nIdxMainGradientLeft, _ppbd.m_pHelperSrc, _ppbd.m_lParam );
			COLORREF clrRight = GetColor( _ppbd.m_bHorz ? nIdxMainGradientLeft : nIdxMainGradientRight, _ppbd.m_pHelperSrc, _ppbd.m_lParam );
			if( _ppbd.m_bDefaultPushButton && ( ! ( _ppbd.m_bHover || _ppbd.m_bPushed || _ppbd.m_bPushedDropDown ) ) )
			{
				clrLeft  = dc.GetNearestColor( stat_HLS_Adjust( clrLeft,  0.0, 0.2, 0.0 ) );
				clrRight = dc.GetNearestColor( stat_HLS_Adjust( clrRight, 0.0, 0.2, 0.0 ) );
			}
			stat_PaintGradientRect( dc, rectClient, clrLeft, clrRight, _ppbd.m_bHorz );
		} // if( nIdxMainGradientLeft >= 0 && nIdxMainGradientRight >= 0 )
		else
			dc.FillSolidRect( &rectClient, bgColor );
	} // if( ! _ppbd.m_bTransparentBackground . . .
	// Draw pressed button
	if( _ppbd.m_bPushed || _ppbd.m_bIndeterminate || _ppbd.m_bPushedDropDown )
	{
		if( (!_ppbd.m_bFlat) || _ppbd.m_bDrawBorder )
			dc.Draw3dRect( rectClient, clrDkBorder, clrDkBorder );
	} // if( bPushed . . .
	else // ...else draw non pressed button
	{
		if( _ppbd.m_bFlat )
		{
			if( _ppbd.m_bHover && _ppbd.m_bDrawBorder )
				dc.Draw3dRect( rectClient, clrDkBorder, clrDkBorder );
		} // if( _ppbd.m_bFlat )
		else
		{
			dc.Draw3dRect( rectClient, clrDkBorder, clrDkBorder );
			if( _ppbd.m_bHover )
				dc.Draw3dRect( _ppbd.m_rcClient, clrDkBorder, clrDkBorder );
		}  // else from if( _ppbd.m_bFlat )
	} // else from if( _ppbd.m_bPushed || _ppbd.m_bPushedDropDown )

	if( _ppbd.m_bDropDown && (! _ppbd.m_bNoDropDownArrow ) )
	{
		CRect rectGlyph( rectDropDown );
		if( bPushedDropDown )
			rectGlyph.OffsetRect( GetPushedOffset() );
		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			RGB(0,0,0),
		};
		bool bSetDefaultColorForDD = true;
		if( _ppbd.m_bEnabled )
		{
			if( m_clrCustomDropDownGlyphEnabled != COLORREF(-1L) )
			{
				ColorValues[1] = m_clrCustomDropDownGlyphEnabled;
				bSetDefaultColorForDD = false;
			}
		}
		else
		{
			if( m_clrCustomDropDownGlyphDisabled != COLORREF(-1L) )
			{
				ColorValues[1] = m_clrCustomDropDownGlyphDisabled;
				bSetDefaultColorForDD = false;
			}
		}
		if( bSetDefaultColorForDD )
			ColorValues[1] =
				GetColor(
					_ppbd.m_bEnabled
						? COLOR_BTNTEXT
						: COLOR_3DSHADOW
						,
					_ppbd.m_pHelperSrc,
					_ppbd.m_lParam 
					);
		if( bRTL && (rectGlyph.Width()&1) == 0 )
			rectGlyph.right++;
		PaintGlyphCentered(
			dc,
			rectGlyph,
			_ppbd.m_bHorz ? g_glyph_btn_expand_bottom : g_glyph_btn_expand_right,
			ColorValues
			);
	} // if( _ppbd.m_bDropDown && (! _ppbd.m_bNoDropDownArrow ) )

	dc.SetBkColor( bgColor );

	rectCaption.DeflateRect( _ppbd.m_rcBorderSizes );
	rectClient = rectCaption;
CRect rcFocus( rectCaption );

	// Draw icon
	if(		rectClient.bottom > rectClient.top
		&&	rectClient.right > rectClient.left
		)
		PaintPushButtonIcon( dc, rectClient, rectCaption, rcFocus, _ppbd );

CExtSafeString sTitle( _T("") );
	if( _ppbd.m_sText != NULL )
		sTitle = _ppbd.m_sText;

	// Draw text
	if(		rectCaption.bottom > rectCaption.top
		&&	rectCaption.right > rectCaption.left
		&&	(!sTitle.IsEmpty())
		)
		PaintPushButtonText( dc, rectCaption, _ppbd );

	// Draw focus rectangle
	if(		_ppbd.m_bDrawFocusRect 
		&&	( ( !sTitle.IsEmpty() ) || _ppbd.m_nFocusMode == __EBFM_CLIENT_AREA )
		&&	rcFocus.bottom > rcFocus.top 
		&&	rcFocus.right > rcFocus.left
		)
	{
		rcFocus.DeflateRect( 2, 2 );
		dc.DrawFocusRect( &rcFocus );
	}

	dc.SetBkMode( nOldBkMode );
	dc.SetBkColor( clrOldBk );
	dc.SetTextColor( clrOldText );
}

void CExtPaintManagerXP::PaintPushButtonText(
	CDC & dc,
	CRect & rcCaption,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;
CExtSafeString sTitle( _T("") );
	if( _ppbd.m_sText != NULL )
		sTitle = _ppbd.m_sText;
bool bRTL = _ppbd.IsRTL();
	if(		rcCaption.bottom > rcCaption.top
		&&	rcCaption.right > rcCaption.left
		&&	(!sTitle.IsEmpty())
		)
	{
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		if( _ppbd.m_bPushed || _ppbd.m_bPushedDropDown )
		{
			CSize sizePushedOffset = GetPushedOffset();
			rcCaption.OffsetRect(sizePushedOffset);
		}
		if( _ppbd.m_bEnabled ) 
		{
			if( _ppbd.m_bHover || _ppbd.m_bPushed )
			{
				if(			_ppbd.m_bPushed
						&&	_ppbd.m_clrForceTextPressed != ((COLORREF)-1L)
						)
						dc.SetTextColor( _ppbd.m_clrForceTextPressed );
				else if(	_ppbd.m_bHover
						&&	_ppbd.m_clrForceTextHover != ((COLORREF)-1L)
						)
						dc.SetTextColor( _ppbd.m_clrForceTextHover );
				else if( _ppbd.m_clrForceTextNormal != ((COLORREF)-1L) ) 
						dc.SetTextColor( _ppbd.m_clrForceTextNormal );
				else
						dc.SetTextColor( GetColor( COLOR_BTNTEXT, _ppbd.m_pHelperSrc, _ppbd.m_lParam ) );
			}
			else 
			{
				dc.SetTextColor( 
					( _ppbd.m_clrForceTextNormal == ((COLORREF)-1L) )
						? GetColor( 
							( _ppbd.m_bHover && _ppbd.m_bPushed ) ? XPCLR_PUSHEDHOVERTEXT : COLOR_BTNTEXT, 
							_ppbd.m_pHelperSrc, 
							_ppbd.m_lParam 
							)
						: _ppbd.m_clrForceTextNormal
					);
			}
		}
		else 
			dc.SetTextColor(
				( _ppbd.m_clrForceTextDisabled == ((COLORREF)-1L) )
					? GetColor( CLR_TEXT_DISABLED, _ppbd.m_pHelperSrc, _ppbd.m_lParam )
					: _ppbd.m_clrForceTextDisabled
				);
		CFont * pOldBtnFont = NULL;
		CFont * pCurrFont = NULL;
		if( _ppbd.m_hFont != NULL )
			pCurrFont = CFont::FromHandle( _ppbd.m_hFont );
		else
		{
			if( _ppbd.m_bDefaultPushButton )
				pCurrFont = &m_FontBold;
			else
				pCurrFont = &m_FontNormal;
		} // else from if( _ppbd.m_hFont != NULL )
		ASSERT( pCurrFont != NULL );
		pOldBtnFont = dc.SelectObject( pCurrFont );

		CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
		if( ! _ppbd.m_bHorz )
			eLO = CExtRichContentLayout::__ELOT_90_CW;
		CRect rcTextLocation( 0, 0, 0, 0);
		UINT nDtMeasureFlags = DT_LEFT | DT_TOP | DT_CALCRECT | ((bRTL || _ppbd.m_bForceRtlText) ? DT_RTLREADING : 0);
		if( _ppbd.m_bWordBreak )
		{
			rcTextLocation = rcCaption;
			rcTextLocation.OffsetRect( -rcTextLocation.TopLeft() );
			rcTextLocation.bottom = rcTextLocation.top;
			nDtMeasureFlags |= DT_WORDBREAK;
		}
		else
			nDtMeasureFlags |= DT_SINGLELINE;

		CExtRichContentLayout::stat_DrawText(
			CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
			dc.m_hDC, LPCTSTR(sTitle), rcTextLocation, nDtMeasureFlags, 0
			);
		if(		( bRTL || _ppbd.m_bForceRtlText )
			&&	eLO == CExtRichContentLayout::__ELOT_NORMAL
			&&	_ppbd.m_pHelperSrc != NULL
			&&	_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtButton) )
			)
			rcTextLocation.InflateRect( 0, 0, 2, 0 ); // TO-FIX: incorrect horizontally-oriented text position on RTL buttons
		rcTextLocation.OffsetRect( rcCaption.TopLeft() - rcTextLocation.TopLeft() );
			
		UINT nDtDrawFlags = 0;
		if( (_ppbd.m_nTextAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_RIGHT )
		{
			nDtDrawFlags |= DT_RIGHT;
			rcTextLocation.OffsetRect(
				rcCaption.Width() - rcTextLocation.Width(),
				0
				);
		}
		else if( (_ppbd.m_nTextAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
		{
			nDtDrawFlags |= DT_CENTER;
			rcTextLocation.OffsetRect(
				( rcCaption.Width() - rcTextLocation.Width() ) / 2,
				0
				);
		}
		else
			nDtDrawFlags |= DT_LEFT;
			
		if( (_ppbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		{
			nDtDrawFlags |= DT_BOTTOM;
			rcTextLocation.OffsetRect(
				0,
				rcCaption.Height() - rcTextLocation.Height()
				);
		}
		else if( (_ppbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_TOP )
		{
			nDtDrawFlags |= DT_TOP;
		}
		else
		{
			nDtDrawFlags |= DT_VCENTER;
			rcTextLocation.OffsetRect(
				0,
				( rcCaption.Height() - rcTextLocation.Height() ) / 2
				);
		}

		if( _ppbd.m_bWordBreak )
			nDtDrawFlags |= DT_WORDBREAK;
		else
			nDtDrawFlags |= DT_SINGLELINE;

		CExtRichContentLayout::stat_DrawText(
			CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
			dc.m_hDC, LPCTSTR(sTitle), rcTextLocation, nDtDrawFlags, 0
			);
		dc.SelectObject( pOldBtnFont );
		dc.SetBkMode( nOldBkMode );
	}
}

void CExtPaintManagerNativeXP::PaintPushButton(
	CDC & dc,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintPushButton( dc, _ppbd );
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	if( _ppbd.m_bIndeterminate )
		_ppbd.m_bPushed = true;

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( _ppbd.m_pHelperSrc != NULL )
	{
		CExtBarButton * pTBB = DYNAMIC_DOWNCAST( CExtBarButton, _ppbd.m_pHelperSrc );
		if( pTBB != NULL )
		{
			if( pTBB->IsRibbonPaintingMode() )
			{
				Ribbon_PaintPushButton( dc, _ppbd );
				return;
			}
			CExtToolControlBar * pBar = pTBB->GetBar();
			if(		pBar != NULL
				&&	pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) )
				&&	(! pTBB->IsNoRibbonLayout() )
				&&	(! pTBB->IsKindOf( RUNTIME_CLASS( CExtBarMdiRightButton ) ) )
				)
			{
				Ribbon_PaintPushButton( dc, _ppbd );
				return;
			}
		} // if( pTBB != NULL )
	} // if( _ppbd.m_pHelperSrc != NULL )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

HWND hWnd = NULL;
CWnd * pWnd =
		DYNAMIC_DOWNCAST(
			CWnd,
			_ppbd.m_pHelperSrc
			);
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}

enum ButtonDrawingMode
{
	eModeControl,
	eModeToolBar,
	eModeMenuBar,
	eModePopupMenu,
	eModeDatePicker,
	eModeToolBox,
};

bool bHorizontal = true;

ButtonDrawingMode aDrawingMode = eModeControl;
	if( _ppbd.m_pHelperSrc != NULL  )
	{
		if(		_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarButton ) ) 
			&&	(! (_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarSliderButton ) )
				&&	_ppbd.m_lParam == CExtBarSliderButton::__ESA_THUMB) )
			)
		{
			CExtBarButton * pBarButton = (CExtBarButton*)_ppbd.m_pHelperSrc;
			CExtToolControlBar * pToolBar = DYNAMIC_DOWNCAST( CExtToolControlBar, pBarButton->GetBar() );
			ASSERT( pToolBar != NULL );
			
			if( pToolBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
				aDrawingMode = eModeMenuBar;
			else
				aDrawingMode = eModeToolBar;

			if( ! pBarButton->GetNoRotateVerticalLayout() )
			{
				if( pToolBar->IsDockedVertically() )
					bHorizontal = false;
			}
		}
		else if( _ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtPopupMenuWnd ) ) )
		{
			aDrawingMode = eModePopupMenu;
		}
#ifndef __EXT_MFC_NO_DATE_PICKER
		else if( _ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtDatePickerWnd ) ) )
		{
			aDrawingMode = eModeDatePicker;
		}
#endif // #ifndef __EXT_MFC_NO_DATE_PICKER
#ifndef __EXT_MFC_NO_TOOLBOX_CTRL
		else if( _ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtToolBoxWnd ) ) )
		{
			aDrawingMode = eModeToolBox;
		}
#endif // #ifndef __EXT_MFC_NO_TOOLBOX_CTRL
	}

bool bRTL = _ppbd.IsRTL();

CRect rectClient( _ppbd.m_rcClient );
CRect rectCaption( _ppbd.m_rcClient );
CRect rectDropDown( 0, 0, 0, 0 );
	if( _ppbd.m_bDropDown )
	{
		rectDropDown = rectClient;
		if( _ppbd.m_bHorz )
		{
			if( bRTL )
			{
				rectCaption.left += _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
				rectDropDown.right = rectCaption.left;
			}
			else
			{
				rectCaption.right -= _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
				rectDropDown.left = rectCaption.right;
			}
		}
		else
		{
			rectCaption.bottom -= _CalcSplitButtonExtentImpl( _ppbd.m_pHelperSrc, _ppbd.m_bHorz );
			rectDropDown.top = rectCaption.bottom;
		}
	} // if( _ppbd.m_bDropDown )

	if(		aDrawingMode == eModeToolBar 
		||	aDrawingMode == eModePopupMenu
		)
	{
		if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_TOOLBAR ) != NULL )
		{
			if( _ppbd.m_bDropDown )
			{
				if(! _ppbd.m_bNoDropDownArrow )
				{
					if( _ppbd.m_bSeparatedDropDown )
					{
						INT nStateID = 
							(!_ppbd.m_bEnabled)
								? TS_DISABLED
								: ( _ppbd.m_bPushed && (!_ppbd.m_bChecked) )
									? TS_PRESSED
									: ( _ppbd.m_bHover || _ppbd.m_bPushedDropDown )
										? ( _ppbd.m_bChecked ? TS_HOTCHECKED : TS_HOT )
										: ( _ppbd.m_bChecked ? TS_CHECKED : TS_NORMAL );

						VERIFY( 
							g_PaintManager.m_UxTheme.DrawBackground(
								hWnd,
								dc.GetSafeHdc(), 
								TP_SPLITBUTTON, 
								nStateID, 
								&rectCaption, 
								&rectCaption
								) == S_OK
							);

						nStateID = 
							(!_ppbd.m_bEnabled)
								? TS_DISABLED
								: _ppbd.m_bPushedDropDown
									? TS_PRESSED
									: _ppbd.m_bHover
										? ( _ppbd.m_bChecked ? TS_HOTCHECKED : TS_HOT )
										: ( _ppbd.m_bChecked ? TS_CHECKED : TS_NORMAL );
						VERIFY( 
							g_PaintManager.m_UxTheme.DrawBackground(
								hWnd,
								dc.GetSafeHdc(), 
								TP_SPLITBUTTONDROPDOWN, 
								nStateID, 
								&rectDropDown, 
								&rectDropDown
								) == S_OK
							);
					}
					else
					{
						INT nStateID = 
							(!_ppbd.m_bEnabled)
								? TS_DISABLED
								: ( _ppbd.m_bPushed && (!_ppbd.m_bChecked) )
									? TS_PRESSED
									: _ppbd.m_bHover
										? ( _ppbd.m_bChecked ? TS_HOTCHECKED : TS_HOT )
										: ( _ppbd.m_bChecked ? TS_CHECKED : TS_NORMAL );
						VERIFY( 
							g_PaintManager.m_UxTheme.DrawBackground(
								hWnd,
								dc.GetSafeHdc(), 
								TP_DROPDOWNBUTTON, 
								nStateID, 
								&rectClient, 
								&rectClient
								) == S_OK
							);
						VERIFY( 
							g_PaintManager.m_UxTheme.DrawBackground(
								hWnd,
								dc.GetSafeHdc(), 
								TP_SPLITBUTTONDROPDOWN, 
								_ppbd.m_bEnabled ? TS_NORMAL : TS_DISABLED, 
								&rectDropDown, 
								&rectDropDown
								) == S_OK
							);

					} // if( _ppbd.m_bSeparatedDropDown )
				} // if(! _ppbd.m_bNoDropDownArrow )
			}
			else
			{
				INT nStateID = 
					(!_ppbd.m_bEnabled)
						? TS_DISABLED
						: ( _ppbd.m_bPushed && (!_ppbd.m_bChecked) )
							? TS_PRESSED
							: _ppbd.m_bHover
								? ( _ppbd.m_bChecked ? TS_HOTCHECKED : TS_HOT )
								: ( _ppbd.m_bChecked ? TS_CHECKED : TS_NORMAL );
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWnd,
						dc.GetSafeHdc(), 
						TP_BUTTON, 
						nStateID, 
						&rectClient, 
						&rectClient
						) == S_OK
					);
			} // if( _ppbd.m_bDropDown )

			g_PaintManager.m_UxTheme.CloseThemeData();
		}			
	}
	else if(	aDrawingMode == eModeControl 
			||	aDrawingMode == eModeDatePicker
			||	aDrawingMode == eModeToolBox
			)
	{
		if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_BUTTON ) != NULL )
		{
			INT nPartID = BP_PUSHBUTTON;
			INT nStateID = 
				(!_ppbd.m_bEnabled)
					? PBS_DISABLED
					: ( _ppbd.m_bPushed || _ppbd.m_bPushedDropDown )
						? PBS_PRESSED
						: _ppbd.m_bHover
							? PBS_HOT
							: ( _ppbd.m_bDefaultPushButton || _ppbd.m_bDrawFocusRect )
								? PBS_DEFAULTED
								: PBS_NORMAL;		
			if( ! g_PaintManager.m_UxTheme.DrawBackground(
					hWnd,
					dc.GetSafeHdc(), 
					nPartID, 
					nStateID, 
					&rectClient, 
					&rectClient
					) == S_OK
				)
				CExtPaintManager::PaintPushButton( dc, _ppbd );

			if( _ppbd.m_bDropDown )
			{
				CRect rcGlyph( rectDropDown );
				rcGlyph.OffsetRect( 
					_ppbd.m_bHorz ? -1 : 0, 
					_ppbd.m_bHorz ?  0 : -1
					);

				COLORREF ColorValues[2] = { RGB(0,0,0), ::GetSysColor( _ppbd.m_bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT ) };
				if( bRTL && (rcGlyph.Width()&1) != 0 )
					rcGlyph.right++;
				PaintGlyphCentered(
					dc,
					rcGlyph,
					_ppbd.m_bHorz ? g_glyph_btn_expand_bottom : g_glyph_btn_expand_right,
					ColorValues
					);

				if( _ppbd.m_bSeparatedDropDown )
				{
					CRect rcSeparator( rectDropDown );
					if( _ppbd.m_bHorz )
						rcSeparator.right = rcSeparator.left + 2;
					else
						rcSeparator.bottom = rcSeparator.top + 2;
					rcSeparator.OffsetRect( 
						_ppbd.m_bHorz ? -1 : 0, 
						_ppbd.m_bHorz ?  0 : -1
						);
					rcSeparator.DeflateRect(
						_ppbd.m_bHorz ? 0 : 4, 
						_ppbd.m_bHorz ? 4 : 0
						);
					dc.Draw3dRect( &rcSeparator, ::GetSysColor( COLOR_3DSHADOW ), ::GetSysColor( COLOR_WINDOW ) );
				}
			}

			g_PaintManager.m_UxTheme.CloseThemeData();
		}			
	} 
	else if( aDrawingMode == eModeMenuBar )
	{
		if(		_ppbd.m_bEnabled
			&&	( _ppbd.m_bPushed || _ppbd.m_bHover )
			)
			dc.FillSolidRect( &rectClient, ::GetSysColor( COLOR_HIGHLIGHT ) );
	}
	else 
	{
		ASSERT( FALSE );
		return;
	}

COLORREF clrOldText = dc.GetTextColor();
COLORREF clrOldBk = dc.GetBkColor();
int nOldBkMode = dc.SetBkMode( TRANSPARENT );

	rectCaption.DeflateRect( _ppbd.m_rcBorderSizes );
	rectClient = rectCaption;
CRect rcFocus( rectCaption );

	// Draw icon
	if(		rectClient.bottom > rectClient.top
		&&	rectClient.right > rectClient.left
		)
		PaintPushButtonIcon( dc, rectClient, rectCaption, rcFocus, _ppbd );

CExtSafeString sTitle( _T("") );
	if( _ppbd.m_sText != NULL )
		sTitle = _ppbd.m_sText;

	// Draw text
	if(		rectCaption.bottom > rectCaption.top
		&&	rectCaption.right > rectCaption.left
		&&	(!sTitle.IsEmpty())
		)
		PaintPushButtonText( dc, rectCaption, _ppbd );

	// Draw focus rectangle
	if(		_ppbd.m_bDrawFocusRect 
		&&	( ( !sTitle.IsEmpty() ) || _ppbd.m_nFocusMode == __EBFM_CLIENT_AREA )
		&&	rcFocus.bottom > rcFocus.top 
		&&	rcFocus.right > rcFocus.left
		)
	{
		rcFocus.DeflateRect( 2, 2 );
		dc.DrawFocusRect( &rcFocus );
	}

	dc.SetBkMode( nOldBkMode );
	dc.SetBkColor( clrOldBk );
	dc.SetTextColor( clrOldText );
}

void CExtPaintManagerNativeXP::PaintPushButtonText(
	CDC & dc,
	CRect & rcCaption,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintPushButtonText( dc, rcCaption, _ppbd );
		return;
	}
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;
bool bMenuBar = false;
	if( _ppbd.m_pHelperSrc != NULL  )
	{
		if(		_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarButton ) ) 
			&&	(! (_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarSliderButton ) )
				&&	_ppbd.m_lParam == CExtBarSliderButton::__ESA_THUMB) )
			)
		{
			CExtBarButton * pBarButton = (CExtBarButton*)_ppbd.m_pHelperSrc;
			CExtToolControlBar * pToolBar = DYNAMIC_DOWNCAST( CExtToolControlBar, pBarButton->GetBar() );
			ASSERT( pToolBar != NULL );
			if( pToolBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
				bMenuBar = true;
		}
	}
CExtSafeString sTitle( _T("") );
	if( _ppbd.m_sText != NULL )
		sTitle = _ppbd.m_sText;
bool bRTL = _ppbd.IsRTL();
	if(		rcCaption.bottom > rcCaption.top
		&&	rcCaption.right > rcCaption.left
		&&	(!sTitle.IsEmpty())
		)
	{
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		if( _ppbd.m_bPushed )
		{
			CSize sizePushedOffset = GetPushedOffset();
			rcCaption.OffsetRect( sizePushedOffset );
		}
		if( _ppbd.m_bEnabled )
		{
			if( _ppbd.m_bHover || _ppbd.m_bPushed )
			{
				if(		_ppbd.m_bPushed
					&&	_ppbd.m_clrForceTextPressed != ((COLORREF)-1L)
					)
					dc.SetTextColor( _ppbd.m_clrForceTextPressed );
				else if(	_ppbd.m_bHover
						&&	_ppbd.m_clrForceTextHover != ((COLORREF)-1L)
						)
						dc.SetTextColor( _ppbd.m_clrForceTextHover );
				else if( _ppbd.m_clrForceTextNormal != ((COLORREF)-1L) ) 
						dc.SetTextColor( _ppbd.m_clrForceTextNormal );
				else
					dc.SetTextColor( 
						bMenuBar 
							? ::GetSysColor( COLOR_HIGHLIGHTTEXT ) 
							: ::GetSysColor( COLOR_BTNTEXT ) 
						);
			}
			else 
				dc.SetTextColor( 
					( _ppbd.m_clrForceTextNormal == ((COLORREF)-1L) )
						? ::GetSysColor( COLOR_BTNTEXT )
						: _ppbd.m_clrForceTextNormal
					);
		}
		else
			dc.SetTextColor(
				( _ppbd.m_clrForceTextDisabled == ((COLORREF)-1L) )
					? ::GetSysColor( COLOR_3DFACE )
					: _ppbd.m_clrForceTextDisabled
				);
		CFont * pOldBtnFont = NULL;
		CFont * pCurrFont = NULL;
		if( _ppbd.m_hFont != NULL )
			pCurrFont = CFont::FromHandle(_ppbd.m_hFont);
		else
		{
			if( _ppbd.m_bDefaultPushButton )
				pCurrFont = &m_FontBold;
			else
				pCurrFont = &m_FontNormal;
		} // else from if( _ppbd.m_hFont != NULL )

		ASSERT( pCurrFont != NULL );
		pOldBtnFont = dc.SelectObject( pCurrFont );
		ASSERT( pOldBtnFont != NULL );

		CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
		if( ! _ppbd.m_bHorz )
			eLO = CExtRichContentLayout::__ELOT_90_CW;
		CRect rcTextLocation( 0, 0, 0, 0);
		UINT nDtMeasureFlags = DT_LEFT | DT_TOP | DT_CALCRECT | ((bRTL || _ppbd.m_bForceRtlText) ? DT_RTLREADING : 0);
		if( _ppbd.m_bWordBreak )
		{
			rcTextLocation = rcCaption;
			rcTextLocation.OffsetRect( -rcTextLocation.TopLeft() );
			rcTextLocation.bottom = rcTextLocation.top;
			nDtMeasureFlags |= DT_WORDBREAK;
		}
		else
			nDtMeasureFlags |= DT_SINGLELINE;

		CExtRichContentLayout::stat_DrawText(
			CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
			dc.m_hDC, LPCTSTR(sTitle), rcTextLocation, nDtMeasureFlags, 0
			);
		if(		( bRTL || _ppbd.m_bForceRtlText )
			&&	eLO == CExtRichContentLayout::__ELOT_NORMAL
			&&	_ppbd.m_pHelperSrc != NULL
			&&	_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtButton) )
			)
			rcTextLocation.InflateRect( 0, 0, 2, 0 ); // TO-FIX: incorrect horizontally-oriented text position on RTL buttons
		rcTextLocation.OffsetRect( rcCaption.TopLeft() - rcTextLocation.TopLeft() );
			
		UINT nDtDrawFlags = 0;
		if( (_ppbd.m_nTextAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_RIGHT )
		{
			nDtDrawFlags |= DT_RIGHT;
			rcTextLocation.OffsetRect(
				rcCaption.Width() - rcTextLocation.Width(),
				0
				);
		}
		else if( (_ppbd.m_nTextAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
		{
			nDtDrawFlags |= DT_CENTER;
			rcTextLocation.OffsetRect(
				( rcCaption.Width() - rcTextLocation.Width() ) / 2,
				0
				);
		}
		else
			nDtDrawFlags |= DT_LEFT;
		
		if( (_ppbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		{
			nDtDrawFlags |= DT_BOTTOM;
			rcTextLocation.OffsetRect(
				0,
				rcCaption.Height() - rcTextLocation.Height()
				);
		}
		else if( (_ppbd.m_nTextAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_TOP )
		{
			nDtDrawFlags |= DT_TOP;
		}
		else
		{
			nDtDrawFlags |= DT_VCENTER;
			rcTextLocation.OffsetRect(
				0,
				( rcCaption.Height() - rcTextLocation.Height() ) / 2
				);
		}

		if( _ppbd.m_bWordBreak )
			nDtDrawFlags |= DT_WORDBREAK;
		else
			nDtDrawFlags |= DT_SINGLELINE;

		if( _ppbd.m_bPushed )
		{
			CSize szPushedOffset = GetPushedOffset();
			rcTextLocation.OffsetRect( szPushedOffset );
		}

		if( _ppbd.m_bForceUseDrawTextApiH )
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC, LPCTSTR(sTitle), &rcTextLocation, nDtDrawFlags, 0
				);
		else
		{
			if(		_ppbd.m_bEnabled 
				||	_ppbd.m_clrForceTextDisabled != ((COLORREF)-1L)
				)
				CExtRichContentLayout::stat_DrawText(
					CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
					dc.m_hDC, LPCTSTR(sTitle), &rcTextLocation, nDtDrawFlags, 0
					);
			else
			{
				CRect rcDrawTextOffs = rcTextLocation;
				rcDrawTextOffs.OffsetRect( 1, 1 );
				COLORREF clrTextRestore = dc.SetTextColor( GetColor( COLOR_3DHILIGHT, _ppbd.m_pHelperSrc, _ppbd.m_lParam ) );
				CExtRichContentLayout::stat_DrawText(
					CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
					dc.m_hDC, LPCTSTR(sTitle), &rcDrawTextOffs, nDtDrawFlags, 0
					);
				dc.SetTextColor( GetColor( COLOR_3DSHADOW, _ppbd.m_pHelperSrc, _ppbd.m_lParam ) );
				CExtRichContentLayout::stat_DrawText(
					CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
					dc.m_hDC, LPCTSTR(sTitle), rcTextLocation, nDtDrawFlags, 0
					);
				dc.SetTextColor( clrTextRestore );
			} // else from if( _ppbd.m_bEnabled )
		} // else from if( _ppbd.m_bForceUseDrawTextApiH )
		dc.SelectObject(pOldBtnFont);
		dc.SetBkMode( nOldBkMode );
	}
}

void CExtPaintManagerNativeXP::PaintPushButtonIcon(
	CDC & dc,
	CRect & rcClient,
	CRect & rcCaption,
	CRect & rcFocus,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintPushButtonIcon(
			dc,
			rcClient,
			rcCaption,
			rcFocus,
			_ppbd
			);
		return;
	}

	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;

bool bRTL = _ppbd.IsRTL();
bool bDrawIcon = true;

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
bool bRibbonMode = false;
	if(		_ppbd.m_pHelperSrc != NULL
		&&	_ppbd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtRibbonButton) )
		)
	{ // ribbon button
		bRibbonMode = true;
		CExtRibbonButton * pTBB = STATIC_DOWNCAST( CExtRibbonButton, _ppbd.m_pHelperSrc );
		INT nILV = pTBB->RibbonILV_Get();
		if( nILV == __EXT_RIBBON_ILV_SIMPLE_LARGE )
		{
			bDrawIcon = false;
			INT nTextToIconDistance = Ribbon_GetTextToIconDistance( dc, nILV, pTBB, _ppbd.m_lParam );
//			CSize _sizeIcon = _ppbd.m_pIcon->GetSize();
			CSize _sizeIcon = Ribbon_GetIconSize( pTBB, nILV, _ppbd.m_lParam );
			CRect rcIconPaint = rcCaption;
			CRect rcCP = pTBB->OnRibbonGetContentPadding();
			rcIconPaint.bottom = rcIconPaint.top + _sizeIcon.cx + rcCP.bottom + nTextToIconDistance;
			rcIconPaint.DeflateRect( rcCP.left, rcCP.top, rcCP.right, 0 );
			rcCaption.top += _sizeIcon.cy + nTextToIconDistance + rcCP.top + rcCP.bottom;
			rcCaption.DeflateRect( rcCP.left, 0, rcCP.right, rcCP.bottom );
			if( _ppbd.m_pIcon != NULL && (! _ppbd.m_pIcon->IsEmpty()) )
			{
				PaintIcon(
					dc,
					_ppbd.m_bHorz,
					_ppbd.m_pIcon,
					rcIconPaint,
					_ppbd.m_bPushed,
					_ppbd.m_bEnabled,
					_ppbd.m_bHover,
					_ppbd.m_nIconAlignment,
					_ppbd.m_bForceRtlLayout && _ppbd.m_bHorz,
					NULL,
					NULL,
					&_ppbd.m_rcIconMargins,
					_ppbd.m_clrCustomAccentEffectForIcon
					);
			}
		} // if( nILV == __EXT_RIBBON_ILV_SIMPLE_LARGE )
	} // ribbon button
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	// Draw the icon
	if(		bDrawIcon
		&&	rcClient.bottom > rcClient.top
		&&	rcClient.right > rcClient.left
		)
	{
		if(		_ppbd.m_pIcon != NULL 
			&&	(! _ppbd.m_pIcon->IsEmpty()) 
			)
		{
			PaintIcon(
				dc,
				_ppbd.m_bHorz,
				_ppbd.m_pIcon,
				rcClient,
				_ppbd.m_bPushed,
				_ppbd.m_bEnabled,
				_ppbd.m_bHover,
				_ppbd.m_nIconAlignment,
				_ppbd.m_bForceRtlLayout && _ppbd.m_bHorz,
				NULL,
				&rcCaption,
				&_ppbd.m_rcIconMargins,
				_ppbd.m_clrCustomAccentEffectForIcon
				);
			
			if( _ppbd.m_nFocusMode == __EBFM_CAPTION_AREA )
				rcFocus = rcCaption;

			if(		_ppbd.m_bHorz
				&&	_ppbd.m_pHelperSrc != NULL
				&&	_ppbd.m_pHelperSrc->IsKindOf(RUNTIME_CLASS(CExtPopupMenuWnd))
				)
			{	
				// if painting tool button with icon in popup menu
				if( bRTL )
					rcCaption.right -= 3;
				else
					rcCaption.left += 3;
			}

			if( ! rcCaption.IsRectEmpty() )
			{
				if( _ppbd.m_bHorz )
				{
					rcCaption.DeflateRect( 
						_ppbd.m_rcTextMargins.left,
						_ppbd.m_rcTextMargins.top,
						_ppbd.m_rcTextMargins.right,
						_ppbd.m_rcTextMargins.bottom
						);
				}
				else
				{
					rcCaption.DeflateRect( 
						_ppbd.m_rcTextMargins.top,
						_ppbd.m_rcTextMargins.right,
						_ppbd.m_rcTextMargins.bottom,
						_ppbd.m_rcTextMargins.left
						);			
				}
			}
		}
		else
		{
			if(		_ppbd.m_pHelperSrc != NULL
				&&	_ppbd.m_pHelperSrc->IsKindOf(RUNTIME_CLASS(CExtPopupMenuWnd))
				)
			{
				if( _ppbd.m_bHorz )
					rcCaption.DeflateRect( 3, 0 );
				else
					rcCaption.DeflateRect( 0, 3 );
			}
			PaintSysCmdGlyph(
				dc,
				_ppbd.m_nHelperSysCmdID,
				rcClient,
				false,
				_ppbd.m_bPushed,
				_ppbd.m_bEnabled
				);
		}
	}
}

void CExtPaintManagerXP::PaintIcon(
	CDC & dc,
	bool bHorz,
	CExtCmdIcon * pIcon,
	const CRect & rcClient,
	bool bPushed,
	bool bEnabled,
	bool bHover, // = false
	INT nIconAlignment, // = 0
	bool bRTL, // = false
	CRect * prcIcon, // = NULL
	CRect * prcText, // = NULL
	CRect * prcIconMargins, // = NULL
	COLORREF clrCustomAccentEffectForIcon // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	
	if( pIcon == NULL || pIcon->IsEmpty() )
	{
		if( prcIcon != NULL )
			*prcIcon = CRect( 0, 0, 0, 0 );
		if( prcText != NULL )
			*prcText = rcClient;
		return;
	}

CSize _sizeIcon = pIcon->GetSize();
	ASSERT( _sizeIcon.cx > 0 && _sizeIcon.cy > 0 );

CRect rcIconMargins( 0, 0, 0, 0 );
	if( prcIconMargins != NULL )
		rcIconMargins = *prcIconMargins;

CRect rcTextLocation( rcClient );
CRect rcIconLocation( 0, 0, _sizeIcon.cx, _sizeIcon.cy );

	if( bHorz )
	{
		rcIconLocation.OffsetRect( 
			rcClient.left, 
			rcClient.top 
			);

		if(	( _sizeIcon.cx + rcIconMargins.left + rcIconMargins.right ) > rcClient.Width() )
		{
			nIconAlignment &= ~__ALIGN_HORIZ_MASK;
			nIconAlignment |= __ALIGN_HORIZ_CENTER;
		}

		if(	( _sizeIcon.cy + rcIconMargins.top + rcIconMargins.bottom ) > rcClient.Height() )
		{
			nIconAlignment &= ~__ALIGN_VERT_MASK;
			nIconAlignment |= __ALIGN_VERT_CENTER;
		}

		if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_RIGHT )
		{
			rcIconLocation.OffsetRect( rcClient.right - rcIconLocation.right - rcIconMargins.right, 0 );
			rcTextLocation.right = rcIconLocation.left - rcIconMargins.left;
		}
		else if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
		{
			rcIconLocation.OffsetRect( ( rcClient.right - rcIconLocation.right ) / 2 + rcIconMargins.left - rcIconMargins.right, 0 );
		}
		else if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_LEFT )
		{
			rcIconLocation.OffsetRect( rcIconMargins.left, 0 );
			rcTextLocation.left = rcIconLocation.right + rcIconMargins.right;
		}

		if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_TOP )
		{
			rcIconLocation.OffsetRect( 0, rcIconMargins.top );
			if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
				rcTextLocation.top = rcIconLocation.bottom + rcIconMargins.bottom;
		}
		else if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_CENTER )
		{
			rcIconLocation.OffsetRect( 0, ( rcClient.bottom - rcIconLocation.bottom ) / 2 + rcIconMargins.top - rcIconMargins.bottom );
		}
		else if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		{
			rcIconLocation.OffsetRect( 0, rcClient.bottom - rcIconLocation.bottom - rcIconMargins.bottom );
			if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
				rcTextLocation.bottom = rcIconLocation.top - rcIconMargins.top;
		}
	}
	else
	{
		rcIconLocation.OffsetRect( 
			rcClient.right - rcIconLocation.Width(), 
			rcClient.top 
			);

		if(	( _sizeIcon.cy + rcIconMargins.top + rcIconMargins.bottom ) > rcClient.Height() )
		{
			nIconAlignment &= ~__ALIGN_HORIZ_MASK;
			nIconAlignment |= __ALIGN_HORIZ_CENTER;
		}

		if(	( _sizeIcon.cx + rcIconMargins.left + rcIconMargins.right ) > rcClient.Width() )
		{
			nIconAlignment &= ~__ALIGN_VERT_MASK;
			nIconAlignment |= __ALIGN_VERT_CENTER;
		}

		if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_RIGHT )
		{
			rcIconLocation.OffsetRect( 0, rcClient.bottom - rcIconLocation.bottom - rcIconMargins.bottom );
			rcTextLocation.bottom = rcIconLocation.top - rcIconMargins.top;
		}
		else if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
		{
			rcIconLocation.OffsetRect( 0, ( rcClient.bottom - rcIconLocation.bottom ) / 2 + rcIconMargins.top - rcIconMargins.bottom );
		}
		else if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_LEFT )
		{
			rcIconLocation.OffsetRect( 0, rcIconMargins.top );
			rcTextLocation.top = rcIconLocation.bottom;
		}
		
		if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_TOP )
		{
			rcIconLocation.OffsetRect( rcIconMargins.right, 0 );
			if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
				rcTextLocation.right = rcIconLocation.left;
		}
		else if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_CENTER )
		{
			rcIconLocation.OffsetRect( ( rcClient.left - rcIconLocation.left ) / 2 + rcIconMargins.right - rcIconMargins.left, 0 );
		}
		else if( (nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_BOTTOM )
		{
			rcIconLocation.OffsetRect( rcClient.left - rcIconLocation.left - rcIconMargins.left, 0 );
			if( (nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER )
				rcTextLocation.left = rcIconLocation.right;
		}
	}

	if(		(nIconAlignment&__ALIGN_HORIZ_MASK) == __ALIGN_HORIZ_CENTER 
		&&	(nIconAlignment&__ALIGN_VERT_MASK) == __ALIGN_VERT_CENTER
		)
		rcTextLocation.SetRectEmpty();

	if(		rcTextLocation.top >= rcTextLocation.bottom
		||	rcTextLocation.left >= rcTextLocation.right
		)
		rcTextLocation.SetRectEmpty();
    
	// if button is pressed then press the icon also
	if( bPushed )
	{
		CSize sizePushedOffset = GetPushedOffset();
		rcIconLocation.OffsetRect( sizePushedOffset.cx, sizePushedOffset.cy );
	}

CPoint ptTopLeft = rcIconLocation.TopLeft();
	if( m_bEnabledHoverIconShadows )
		ptTopLeft += bRTL ? CSize( -1 ,1 ) : CSize( 1, 1 );
	if( bEnabled )
	{ 
		CPoint ptOffs( 0, 0 );
		if(		m_bEnabledHoverIconShadows 
			&&	bHover 
			&& (!bPushed) 
			)
		{
			CPoint ptPaint = ptTopLeft + ptOffs;
			if( (pIcon->m_dwFlags&(__EXT_ICON_PERSISTENT_BITMAP_DISABLED|__EXT_ICON_PERSISTENT_BITMAP_HOVER|__EXT_ICON_PERSISTENT_BITMAP_PRESSED)) == 0 )
				pIcon->PaintAccentMono(
					this,
					dc,
					clrCustomAccentEffectForIcon,
					ptPaint.x,
					ptPaint.y
					);
			ptOffs.x = ptOffs.y = -2;
		}
		pIcon->Paint(
			this,
			dc,
			ptTopLeft.x + ptOffs.x,
			ptTopLeft.y + ptOffs.y,
			-1,
			-1,
			bPushed
				? CExtCmdIcon::__PAINT_PRESSED
				: bHover
					? CExtCmdIcon::__PAINT_HOVER
					: CExtCmdIcon::__PAINT_NORMAL
			);
	}
	else
	{ 
		if( clrCustomAccentEffectForIcon != COLORREF(-1L) )
			pIcon->PaintAccentColorized(
				this,
				dc.GetSafeHdc(),
				clrCustomAccentEffectForIcon,
				rcIconLocation.left,
				rcIconLocation.top
				-1,
				-1
				);
		else
			pIcon->Paint(
				this,
				dc.GetSafeHdc(),
				rcIconLocation.left,
				rcIconLocation.top,
				-1,
				-1,
				CExtCmdIcon::__PAINT_DISABLED
				);
	}

	if( prcIcon != NULL )
		*prcIcon = rcIconLocation;
	if( prcText != NULL )
		*prcText = rcTextLocation;
}

void CExtPaintManagerXP::PaintComboFrame(
	CDC & dc,
	CExtPaintManager::PAINTCOMBOFRAMEDATA & _pcfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtComboBoxBase * pWndCombo = NULL;
	if(		_pcfd.m_pHelperSrc != NULL 
		&&	_pcfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtComboBoxBase ) )
		)
	{
		pWndCombo = STATIC_DOWNCAST( CExtComboBoxBase, _pcfd.m_pHelperSrc );
		ASSERT_VALID( pWndCombo );
	}
COLORREF clrWindow = COLORREF( -1L );
	if(		_pcfd.m_clrForceNABorder == COLORREF(-1L) 
		||	_pcfd.m_bPushed 
		|| _pcfd.m_bHover 
		)
	{
		if( pWndCombo != NULL )
			clrWindow = pWndCombo->GetBkColor();
		if( clrWindow == COLORREF(-1L) )
		{
			COLORREF clrSysBk = GetColor( _pcfd.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE, _pcfd.m_pHelperSrc, _pcfd.m_lParam );
			clrWindow = clrSysBk;
		}
	}
	else
		clrWindow = _pcfd.m_clrForceNABorder;
COLORREF clrFaceOut =
		( _pcfd.m_clrForceNAContent == COLORREF(-1L) || _pcfd.m_bHover || _pcfd.m_bPushed )
			?	dc.GetNearestColor(
					stat_HLS_Adjust(
						GetColor( CLR_3DFACE_OUT, _pcfd.m_pHelperSrc, _pcfd.m_lParam ),
						0.0, ( _pcfd.m_bHover || _pcfd.m_bPushed || ( ! _pcfd.m_bEnabled ) ) ? 0.0 : -0.1, 0.0
						)
					)
			: _pcfd.m_clrForceNAContent
			;
COLORREF clrHilight =
		( _pcfd.m_clrForceNABorder == COLORREF(-1L) || _pcfd.m_bHover || _pcfd.m_bPushed )
			? GetColor( XPCLR_3DFACE_NORMAL, _pcfd.m_pHelperSrc, _pcfd.m_lParam )
			: _pcfd.m_clrForceNABorder
			;
COLORREF clrBorder =
		( _pcfd.m_clrForceNABorder == COLORREF(-1L) || _pcfd.m_bHover || _pcfd.m_bPushed )
			? GetColor( XPCLR_HILIGHT_BORDER, _pcfd.m_pHelperSrc, _pcfd.m_lParam )
			: _pcfd.m_clrForceNABorder
			;
CRect rectClient( _pcfd.m_rcClient );
COLORREF clrOuterBorder =
		(_pcfd.m_clrForceActiveOuterBorder != COLORREF(-1L) && (_pcfd.m_bHover || _pcfd.m_bPushed) )
			? _pcfd.m_clrForceActiveOuterBorder
			: clrFaceOut
			;
bool bPopupMenuComboFieldMode = false;
	if(		_pcfd.m_pHelperSrc != NULL
		&&	_pcfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) )
		)
		bPopupMenuComboFieldMode = true;
	if(		(! _pcfd.m_bNoTopOuterBorder )
		&&	(! bPopupMenuComboFieldMode )
		)
		dc.Draw3dRect( rectClient, clrOuterBorder, clrOuterBorder  );
	if( ! bPopupMenuComboFieldMode )
		rectClient.DeflateRect( 1, 1 );
	dc.Draw3dRect( rectClient, clrHilight, clrHilight );
	if( _pcfd.m_bHover || _pcfd.m_bPushed )
		dc.Draw3dRect( rectClient, clrBorder, clrBorder );
INT nDD = GetDropDownButtonWidth( _pcfd.m_pHelperSrc, _pcfd.m_lParam );
CRect rcDDButton( rectClient );
	if( _pcfd.m_bRightText )
		rcDDButton.right = rcDDButton.left + nDD;
	else
		rcDDButton.left = rcDDButton.right - nDD;

	if( ! _pcfd.m_bEnabled )
		rcDDButton.DeflateRect(1,1);
	if( bPopupMenuComboFieldMode && (! (_pcfd.m_bHover || _pcfd.m_bPushed) ) )
		rcDDButton.DeflateRect(0,1,1,1);
PAINTPUSHBUTTONDATA _ppbd(
		_pcfd.m_pHelperSrc,
		true,
		rcDDButton,
		NULL,
		NULL,
		true,
		_pcfd.m_bHover,
		_pcfd.m_bPushed,
		false,
		_pcfd.m_bEnabled,
		true,
		false,
		false,
		0,
		NULL,
		false,
		0,
		( _pcfd.m_bEnabled && _pcfd.m_pHelperSrc != NULL &&( ! _pcfd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CListBox) ) ) ) ? false : true // bTransparentBackground
		);
	PaintPushButton( dc, _ppbd );
	if( !(_pcfd.m_bHover || _pcfd.m_bPushed) )
		dc.Draw3dRect( &rcDDButton, clrWindow, clrWindow );
	if( bPopupMenuComboFieldMode && (! (_pcfd.m_bHover || _pcfd.m_bPushed) ) )
		rcDDButton.InflateRect(0,1,1,1);
	// paint arrow down
glyph_t* pGlyph = g_DockingCaptionGlyphs[ __DCBT_ARROW_SMALL_DOWN ];
	ASSERT( pGlyph != NULL );
COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			_pcfd.m_bEnabled 
				? GetColor( 
					_pcfd.m_bPushed  ? COLOR_3DDKSHADOW  : COLOR_BTNTEXT,
					_pcfd.m_pHelperSrc, 
					_pcfd.m_lParam 
					)
				: ::GetSysColor( COLOR_GRAYTEXT )
		};
	PaintGlyphCentered( dc, rcDDButton, *pGlyph, ColorValues );
}

void CExtPaintManagerNativeXP::PaintComboFrame(
	CDC & dc,
	CExtPaintManager::PAINTCOMBOFRAMEDATA & _pcfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintComboFrame(
			dc,
			_pcfd
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )

HWND hWnd = NULL;
CWnd * pWnd =
		DYNAMIC_DOWNCAST(
			CWnd,
			_pcfd.m_pHelperSrc
			);
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}

	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_COMBOBOX ) != NULL )
	{
		INT nStateID = 
			(!_pcfd.m_bEnabled)
				? CBXS_DISABLED
				: _pcfd.m_bPushed
					? CBXS_PRESSED
					: _pcfd.m_bHover
						? CBXS_HOT
						: CBXS_NORMAL;
		CSize szPart(0,0);
		if( g_PaintManager.m_UxTheme.GetThemePartSize(
				dc.GetSafeHdc(), 
				0, 
				nStateID, 
				NULL, 
				CExtUxTheme::__EXT_UX_TS_TRUE,
				&szPart
				) == S_OK
			)
		{
 			CRect rcDDButton( _pcfd.m_rcClient );
			if( ! g_PaintManager.m_bIsWinVistaOrLater )
				rcDDButton.DeflateRect(1,1);
			INT nDD = GetDropDownButtonWidth( _pcfd.m_pHelperSrc, _pcfd.m_lParam );
			if( _pcfd.m_bRightText )
				rcDDButton.right = rcDDButton.left + nDD;
			else
				rcDDButton.left = rcDDButton.right - nDD;

			CRect rcClip( _pcfd.m_rcClient );
			rcClip.DeflateRect( 3, 3, 3, 3 );
			if( _pcfd.m_bRightText )
				rcClip.left += rcDDButton.Width();
			else
				rcClip.right -= rcDDButton.Width();
 			dc.ExcludeClipRect( &rcClip );

			// border
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					hWnd,
					dc.GetSafeHdc(), 
					0, 
					nStateID, 
					&_pcfd.m_rcClient, 
					&_pcfd.m_rcClient
					) == S_OK
				);

			dc.SelectClipRgn( NULL );

			// drop down button
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					hWnd,
					dc.GetSafeHdc(), 
					CP_DROPDOWNBUTTON, 
					nStateID, 
					&rcDDButton, 
					&rcDDButton
					) == S_OK
				);
		}
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

CRect CExtPaintManager::GetMenuBorderMetrics(
	CWnd * pWnd,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	lParam;
	if(		pWnd != NULL
		&&	pWnd->IsKindOf( RUNTIME_CLASS( CExtPopupMenuWnd ) )
		&&	((CExtPopupMenuWnd*)pWnd)->_IsRibbonMode()
		&&	( ((CExtPopupMenuWnd*)pWnd)->TrackFlagsGet() & TPMX_RIBBON_FILE_MENU ) != 0
		)
		return CRect( 2, 2, 2, 2+28 );
	return CRect( 2, 2, 2, 2 );
}

void CExtPaintManager::AdjustMenuDC(
	CExtPopupMenuWnd * pPopup,
	CDC & dc,
	bool bMemoryDC,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pPopup );
	ASSERT( dc.GetSafeHdc() != NULL );
	pPopup;
	dc;
	bMemoryDC;
	lParam;
}

void CExtPaintManager::PaintMenuBorder(
	CDC & dc,
	const RECT & rectClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rectClient ) )
		return;
CRect rc(rectClient);
	dc.Draw3dRect( &rc, GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam ), GetColor( CLR_3DDKSHADOW_OUT, pHelperSrc, lParam ) );
	rc.DeflateRect( 1, 1 );
	dc.Draw3dRect( &rc, GetColor( CLR_3DHILIGHT_OUT, pHelperSrc, lParam ), GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam ) );
COLORREF clrFace = GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam );
	rc.DeflateRect( 1, 1 );
	dc.Draw3dRect( &rc,clrFace,clrFace );
	rc.DeflateRect( 1, 1 );
	dc.Draw3dRect( &rc,clrFace,clrFace );
}

void CExtPaintManagerNativeXP::PaintMenuBorder(
	CDC & dc,
	const RECT & rectClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rectClient ) )
		return;

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintMenuBorder(
			dc,
			rectClient,
			pHelperSrc,
			lParam
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )

HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}
bool bThemedApproachOK = false;
	if(		hWnd != NULL
		&&	g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_MENU ) != NULL
		)
	{
		HRESULT hr =
			g_PaintManager.m_UxTheme.DrawBackground(
				pWnd->GetSafeHwnd(), 
				dc.GetSafeHdc(), 
				MENU_POPUPBACKGROUND, 
				0, 
				&rectClient, 
				&rectClient
				);
		bThemedApproachOK = ( hr == S_OK ) ? true : false;
		if( bThemedApproachOK )
		{
			hr =
				g_PaintManager.m_UxTheme.DrawBackground(
					hWnd, 
					dc.GetSafeHdc(), 
					MENU_POPUPBORDERS, 
					0, 
					&rectClient, 
					&rectClient
					);
			bThemedApproachOK = ( hr == S_OK ) ? true : false;
		}
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
	if( bThemedApproachOK )
		return;
COLORREF clr = ::GetSysColor( COLOR_3DSHADOW );
	dc.Draw3dRect(
		&rectClient,
		clr,
		clr
		);
}

COLORREF CExtPaintManager::GetMenuFrameFillColor(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	return GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam );
}

COLORREF CExtPaintManagerXP::GetMenuFrameFillColor(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	return GetColor( XPCLR_3DFACE_NORMAL, pHelperSrc, lParam );
}

COLORREF CExtPaintManagerNativeXP::GetMenuFrameFillColor(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return
			CExtPaintManager::GetMenuFrameFillColor(
				pHelperSrc,
				lParam
				);
	return ::GetSysColor( COLOR_MENU );
}

int CExtPaintManager::GetTipShadowSize() const
{
	ASSERT_VALID( this );
	return 3;
}

int CExtPaintManager::GetMenuShadowSize() const
{
	ASSERT_VALID( this );
	return 4;
}

int CExtPaintManagerXP::GetMenuShadowSize() const
{
	ASSERT_VALID( this );
	return 4;
}

int CExtPaintManager::GetMenuIconAreaWidth() const
{
	ASSERT_VALID( this );
	return -1;
}

CRect CExtPaintManagerXP::GetMenuBorderMetrics(
	CWnd * pWnd,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	lParam;
	if(		pWnd != NULL
		&&	pWnd->IsKindOf( RUNTIME_CLASS( CExtPopupMenuWnd ) )
		&&	((CExtPopupMenuWnd*)pWnd)->_IsRibbonMode()
		&&	( ((CExtPopupMenuWnd*)pWnd)->TrackFlagsGet() & TPMX_RIBBON_FILE_MENU ) != 0
		)
		return CRect( 1, 1, 1, 1+28 );
	return CRect( 1, 1, 1, 1 );
}

CRect CExtPaintManagerNativeXP::GetMenuBorderMetrics(
	CWnd * pWnd,
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return 
			CExtPaintManager::GetMenuBorderMetrics(
				pWnd,
				lParam
				);
	if(		pWnd != NULL
		&&	pWnd->IsKindOf( RUNTIME_CLASS( CExtPopupMenuWnd ) )
		&&	((CExtPopupMenuWnd*)pWnd)->_IsRibbonMode()
		&&	( ((CExtPopupMenuWnd*)pWnd)->TrackFlagsGet() & TPMX_RIBBON_FILE_MENU ) != 0
		)
		return CRect( 1, 1, 1, 1+28 );
	return CRect( 1, 1, 1, 1 );
}

void CExtPaintManagerXP::PaintMenuBorder(
	CDC & dc,
	const RECT & rectClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rectClient ) )
		return;
CRect rc(rectClient);
COLORREF clr = GetColor( XPCLR_MENU_BORDER, pHelperSrc, lParam );
	dc.Draw3dRect(&rc,clr,clr);
//COLORREF clrFace = GetColor( XPCLR_3DFACE_NORMAL, pHelperSrc, lParam );
//	rc.DeflateRect(1,1);
//	dc.Draw3dRect(&rc,clrFace,clrFace);
//	rc.right--;
//	rc.bottom--;
//	dc.Draw3dRect(&rc,clrFace,clrFace);
}

bool CExtPaintManager::IsMenuMustCombineExcludeArea(
	const CObject * pMenuTrackerObject // = NULL
	) const
{
	ASSERT_VALID( this );
	pMenuTrackerObject;
//
// Implementadion of CExtPaintManager::PaintMenuCombinedArea()
// is provided, but office 98/2k does not combine its menu area
// with exclude area. interesting? uncomment and play with it!
//
//	return true; 
//
	return false;
}

bool CExtPaintManagerXP::IsMenuMustCombineExcludeArea(
	const CObject * pMenuTrackerObject // = NULL
	) const
{
	ASSERT_VALID( this );
	pMenuTrackerObject;
	return true;
}

bool CExtPaintManager::IsToolBarChevronMenuCombined()
{
	ASSERT_VALID( this );
	return false;
}

bool CExtPaintManagerXP::IsToolBarChevronMenuCombined()
{
	ASSERT_VALID( this );
	return true;
}

void CExtPaintManager::PaintMenuCombinedArea(
	CDC & dc,
	const RECT & rcArea,
	const RECT & rcClient,
	int eCombineAlign, // CExtPopupMenuWnd::e_combine_align_t values
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rcArea ) )
		return;
	dc.SelectClipRgn( NULL );
	ASSERT( eCombineAlign != CExtPopupMenuWnd::__CMBA_NONE );
CRect rcMB =
		GetMenuBorderMetrics(
			( pHelperSrc == NULL )
				? NULL
				: ( DYNAMIC_DOWNCAST( CWnd, pHelperSrc ) )
				,
			lParam
			);
CRect _rcClient( rcClient );
CRect _rcArea( rcArea );
	_rcArea.InflateRect(
		(eCombineAlign==CExtPopupMenuWnd::__CMBA_RIGHT) ?
			rcMB.left : 0,
		(eCombineAlign==CExtPopupMenuWnd::__CMBA_BOTTOM) ?
			rcMB.top : 0,
		(eCombineAlign==CExtPopupMenuWnd::__CMBA_LEFT) ?
			rcMB.right : 0,
		(eCombineAlign==CExtPopupMenuWnd::__CMBA_TOP) ?
			rcMB.bottom : 0
		);
	dc.FillSolidRect(
		&_rcArea,
		GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam )
		);
//	_rcArea = rcArea;

COLORREF clrMenuBorderLTo =
		GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam );
COLORREF clrMenuBorderLTi =
		GetColor( CLR_3DHILIGHT_OUT, pHelperSrc, lParam );
COLORREF clrMenuBorderRBo =
		GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam );
COLORREF clrMenuBorderRBi =
		GetColor( CLR_3DDKSHADOW_OUT, pHelperSrc, lParam );

	switch( eCombineAlign )
	{
	case CExtPopupMenuWnd::__CMBA_TOP:
		_rcArea.InflateRect( 0, 0, 0, rcMB.bottom );
		_rcClient.InflateRect( -rcMB.left, 0, -rcMB.right, 0 );
	break;
	case CExtPopupMenuWnd::__CMBA_BOTTOM:
		_rcArea.InflateRect( 0, rcMB.top, 0, 0 );
		_rcClient.InflateRect( -rcMB.left, 0, -rcMB.right, 0 );
	break;
	case CExtPopupMenuWnd::__CMBA_LEFT:
		_rcArea.InflateRect( 0, 0, rcMB.right, 0 );
		_rcClient.InflateRect( 0, -rcMB.top, 0, -rcMB.bottom );
	break;
	case CExtPopupMenuWnd::__CMBA_RIGHT:
		_rcArea.InflateRect( rcMB.left, 0, 0, 0 );
		_rcClient.InflateRect( 0, -rcMB.top, 0, -rcMB.bottom );
	break;
#ifdef _DEBUG
	default:
		ASSERT(FALSE);
	break;
#endif // _DEBUG
	} // switch( eCombineAlign )

	dc.ExcludeClipRect( &_rcClient );

int cx = _rcArea.Width();
int cy = _rcArea.Height();
	if( eCombineAlign != CExtPopupMenuWnd::__CMBA_BOTTOM )
		dc.FillSolidRect(_rcArea.left, _rcArea.top, cx - 1, 1, clrMenuBorderLTo);
	if( eCombineAlign != CExtPopupMenuWnd::__CMBA_RIGHT )
		dc.FillSolidRect(_rcArea.left, _rcArea.top, 1, cy - 1, clrMenuBorderLTo);
	if( eCombineAlign != CExtPopupMenuWnd::__CMBA_LEFT )
		dc.FillSolidRect(_rcArea.left + cx, _rcArea.top, -1, cy, clrMenuBorderRBo);
	if( eCombineAlign != CExtPopupMenuWnd::__CMBA_TOP )
		dc.FillSolidRect(_rcArea.left, _rcArea.top + cy, cx, -1, clrMenuBorderRBo);
	_rcArea.DeflateRect(1,1,1,1);
	cx = _rcArea.Width();
	cy = _rcArea.Height();
	if( eCombineAlign != CExtPopupMenuWnd::__CMBA_BOTTOM )
		dc.FillSolidRect(_rcArea.left, _rcArea.top, cx - 1, 1, clrMenuBorderLTi);
	if( eCombineAlign != CExtPopupMenuWnd::__CMBA_RIGHT )
		dc.FillSolidRect(_rcArea.left, _rcArea.top, 1, cy - 1, clrMenuBorderLTi);
	if( eCombineAlign != CExtPopupMenuWnd::__CMBA_LEFT )
		dc.FillSolidRect(_rcArea.left + cx, _rcArea.top, -1, cy, clrMenuBorderRBi);
	if( eCombineAlign != CExtPopupMenuWnd::__CMBA_TOP )
		dc.FillSolidRect(_rcArea.left, _rcArea.top + cy, cx, -1, clrMenuBorderRBi);
	
/*
	dc.ExcludeClipRect( &_rcClient );
	dc.Draw3dRect(
		&_rcArea,
		clrMenuBorderLTo,
		clrMenuBorderRBo
		);
	_rcArea.DeflateRect(1,1,1,1);
	dc.Draw3dRect(
		&_rcArea,
		clrMenuBorderLTi,
		clrMenuBorderRBi
		);
*/
	dc.SelectClipRgn( NULL );
}

void CExtPaintManagerXP::PaintMenuCombinedArea(
	CDC & dc,
	const RECT & rcArea,
	const RECT & rcClient,
	int eCombineAlign, // CExtPopupMenuWnd::e_combine_align_t values
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ::IsRectEmpty( &rcArea ) )
		return;
	ASSERT( eCombineAlign != CExtPopupMenuWnd::__CMBA_NONE );
CRect _rcArea( rcArea );
CRect _rcClient( rcClient );
CRect _rcExclude( rcClient );
	_rcExclude.DeflateRect( 1, 1 );
	dc.FillSolidRect( &_rcArea, GetColor( XPCLR_3DFACE_DARK, pHelperSrc, lParam ) );
	switch( eCombineAlign )
	{
	//case CExtPopupMenuWnd::__CMBA_TOP:
	//case CExtPopupMenuWnd::__CMBA_BOTTOM:
	//	_rcClient.InflateRect( -1, 0 );
	//break;
	//case CExtPopupMenuWnd::__CMBA_LEFT:
	//case CExtPopupMenuWnd::__CMBA_RIGHT:
	//	_rcClient.InflateRect( 0, -1 );
	//break;
	case CExtPopupMenuWnd::__CMBA_TOP:
		_rcArea.InflateRect( 0, 0, 0, 1 );
	break;
	case CExtPopupMenuWnd::__CMBA_BOTTOM:
		_rcArea.InflateRect( 0, 1, 0, 0 );
	break;
	case CExtPopupMenuWnd::__CMBA_LEFT:
		_rcArea.InflateRect( 0, 0, 1, 0 );
	break;
	case CExtPopupMenuWnd::__CMBA_RIGHT:
		_rcArea.InflateRect( 1, 0, 0, 0 );
	break;
#ifdef _DEBUG
	default:
		ASSERT(FALSE);
	break;
#endif // _DEBUG
	} // switch( eCombineAlign )
	dc.ExcludeClipRect( &_rcExclude );
//	dc.ExcludeClipRect( &_rcClient );
COLORREF clrMenuBorder = GetColor( XPCLR_MENU_BORDER, pHelperSrc, lParam );
	dc.Draw3dRect( &_rcArea, clrMenuBorder, clrMenuBorder );
	dc.SelectClipRgn( NULL );
}

CExtPaintManager::PAINTCONTROLBARBORDERSDATA::PAINTCONTROLBARBORDERSDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_eCBBT( CExtPaintManager::__CB_OUTER_DOCKBAR )
	, m_dwStyle( 0L )
	, m_rc( 0,0,0,0 )
{
}

CExtPaintManager::PAINTCONTROLBARBORDERSDATA::PAINTCONTROLBARBORDERSDATA(
	CObject * pHelperSrc,
	int eCBBT, // CExtPaintManager::e_control_bar_border_type_t
	DWORD dwStyle,
	const RECT & rc
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_eCBBT( eCBBT )
	, m_dwStyle( dwStyle )
	, m_rc( rc )
{
}

void CExtPaintManager::PaintControlBarBordersDefImpl(
	CDC & dc,
	CExtPaintManager::PAINTCONTROLBARBORDERSDATA & _pcbbd,
	COLORREF clrHi,
	COLORREF clrLo
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( !(_pcbbd.m_dwStyle & CBRS_BORDER_ANY) )
		return;

	// prepare for dark lines
//	ASSERT( _pcbbd.m_rc.top == 0 && _pcbbd.m_rc.left == 0 );
CRect rc1( _pcbbd.m_rc ), rc2( _pcbbd.m_rc );
COLORREF clr = clrLo;
		GetColor(
			CExtPaintManager::CLR_3DFACE_OUT, _pcbbd.m_pHelperSrc, _pcbbd.m_lParam 
			);

	// draw dark line one pixel back/up
	if( _pcbbd.m_dwStyle & CBRS_BORDER_3D )
	{
		rc1.right -= __EXT_MFC_CX_BORDER;
		rc1.bottom -= __EXT_MFC_CY_BORDER;
	}
	if( _pcbbd.m_dwStyle & CBRS_BORDER_TOP )
		rc2.top += afxData.cyBorder2;
	if( _pcbbd.m_dwStyle & CBRS_BORDER_BOTTOM )
		rc2.bottom -= afxData.cyBorder2;

	// draw left and top
	if( _pcbbd.m_dwStyle & CBRS_BORDER_LEFT )
		dc.FillSolidRect(
			0, rc2.top, __EXT_MFC_CX_BORDER, rc2.Height(),
			clr
			);
	if( _pcbbd.m_dwStyle & CBRS_BORDER_TOP )
		dc.FillSolidRect(
			0, 0, _pcbbd.m_rc.right, __EXT_MFC_CY_BORDER,
			clr
			);

	// draw right and bottom
	if( _pcbbd.m_dwStyle & CBRS_BORDER_RIGHT )
		dc.FillSolidRect(
			rc1.right, rc2.top, -__EXT_MFC_CX_BORDER, rc2.Height(),
			clr
			);
	if( _pcbbd.m_dwStyle & CBRS_BORDER_BOTTOM )
		dc.FillSolidRect(
			0, rc1.bottom, _pcbbd.m_rc.right, -__EXT_MFC_CY_BORDER,
			clr
			);

	if( _pcbbd.m_dwStyle & CBRS_BORDER_3D )
	{
		// prepare for hilite lines
//		clr = afxData.clrBtnHilite;
		clr = clrHi;

		// draw left and top
		if( _pcbbd.m_dwStyle & CBRS_BORDER_LEFT )
			dc.FillSolidRect(
				1, rc2.top, __EXT_MFC_CX_BORDER, rc2.Height(),
				clr
				);
		if( _pcbbd.m_dwStyle & CBRS_BORDER_TOP )
			dc.FillSolidRect(
				0, 1, _pcbbd.m_rc.right, __EXT_MFC_CY_BORDER,
				clr
				);

		// draw right and bottom
		if( _pcbbd.m_dwStyle & CBRS_BORDER_RIGHT )
			dc.FillSolidRect(
				_pcbbd.m_rc.right, rc2.top, -__EXT_MFC_CX_BORDER, rc2.Height(),
				clr
				);
		if( _pcbbd.m_dwStyle & CBRS_BORDER_BOTTOM )
			dc.FillSolidRect(
				0, _pcbbd.m_rc.bottom, _pcbbd.m_rc.right, -__EXT_MFC_CY_BORDER,
				clr
				);
	}

//	if( _pcbbd.m_dwStyle & CBRS_BORDER_LEFT )
//		_pcbbd.m_rc.left += afxData.cxBorder2;
//	if( _pcbbd.m_dwStyle & CBRS_BORDER_TOP )
//		_pcbbd.m_rc.top += afxData.cyBorder2;
//	if( _pcbbd.m_dwStyle & CBRS_BORDER_RIGHT )
//		_pcbbd.m_rc.right -= afxData.cxBorder2;
//	if( _pcbbd.m_dwStyle & CBRS_BORDER_BOTTOM )
//		_pcbbd.m_rc.bottom -= afxData.cyBorder2;
}

void CExtPaintManager::PaintControlBarBorders(
	CDC & dc,
	CExtPaintManager::PAINTCONTROLBARBORDERSDATA & _pcbbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pcbbd.m_eCBBT == __CB_INNER_STATUSBAR_ITEM )
	{
		dc.Draw3dRect(
			&_pcbbd.m_rc,
			GetColor( CLR_3DSHADOW_OUT, _pcbbd.m_pHelperSrc, _pcbbd.m_lParam ),
			GetColor( CLR_3DHILIGHT_OUT, _pcbbd.m_pHelperSrc, _pcbbd.m_lParam )
			);
		return;
	} // if( _pcbbd.m_eCBBT == __CB_INNER_STATUSBAR_ITEM )
COLORREF clr = GetColor( CLR_3DFACE_OUT, _pcbbd.m_pHelperSrc, _pcbbd.m_lParam );
	PaintControlBarBordersDefImpl(
		dc, _pcbbd, clr, clr
		);
}

void CExtPaintManagerXP::PaintControlBarBorders(
	CDC & dc,
	CExtPaintManager::PAINTCONTROLBARBORDERSDATA & _pcbbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _pcbbd.m_eCBBT == __CB_INNER_STATUSBAR_ITEM )
	{
		COLORREF clr = GetColor( COLOR_3DSHADOW, _pcbbd.m_pHelperSrc, _pcbbd.m_lParam );
		dc.Draw3dRect( &_pcbbd.m_rc, clr, clr );
		return;
	} // if( _pcbbd.m_eCBBT == __CB_INNER_STATUSBAR_ITEM )
COLORREF clr = GetColor( CLR_3DFACE_OUT, _pcbbd.m_pHelperSrc, _pcbbd.m_lParam );
	PaintControlBarBordersDefImpl(
		dc, _pcbbd, clr, clr
		);
}

void CExtPaintManagerNativeXP::PaintControlBarBorders(
	CDC & dc,
	CExtPaintManager::PAINTCONTROLBARBORDERSDATA & _pcbbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintControlBarBorders(
			dc,
			_pcbbd
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
}

void CExtPaintManager::PaintResizableBarChildNcAreaRect(
	CDC & dc,
	const RECT & rc,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

COLORREF clrTL = GetColor( CLR_WRB_FRAME, pHelperSrc, lParam );
COLORREF clrBR = GetColor( COLOR_3DHILIGHT, pHelperSrc, lParam );
	dc.Draw3dRect(
		&rc,
		clrTL,
		clrBR
		);

//	PaintTabNcAreaRect(
//		dc,
//		rc,
//		pHelperSrc,
//		lParam
//		);
}

void CExtPaintManagerXP::PaintResizableBarChildNcAreaRect(
	CDC & dc,
	const RECT & rc,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

COLORREF clrTL = GetColor( CLR_WRB_FRAME, pHelperSrc, lParam );
COLORREF clrBR = clrTL;
	dc.Draw3dRect(
		&rc,
		clrTL,
		clrBR
		);

//	CExtPaintManager::PaintResizableBarChildNcAreaRect(
//		dc,
//		rc,
//		pHelperSrc,
//		lParam
//		);
}

#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)

void CExtPaintManager::PaintShortcutListBkgnd(
	CDC & dc,
	const RECT & rcClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		rcClient.left >= rcClient.right
		||	rcClient.top >= rcClient.bottom
		||	(! dc.RectVisible( &rcClient ) )
		)
		return;
bool bPaintDefaultBackground = true;
	if(		m_bShortcutListUseDockerBkgnd
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CWnd ) )
		&&	PaintDockerBkgnd( true, dc, (CWnd*)pHelperSrc )
		)
		bPaintDefaultBackground = false;
	if( bPaintDefaultBackground )
	{
		COLORREF clrBk = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
		dc.FillSolidRect( &rcClient, clrBk );
	}
}

void CExtPaintManagerXP::PaintShortcutListBkgnd(
	CDC & dc,
	const RECT & rcClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		rcClient.left >= rcClient.right
		||	rcClient.top >= rcClient.bottom
		||	(! dc.RectVisible( &rcClient ) )
		)
		return;
bool bPaintDefaultBackground = true;
	if(		m_bShortcutListUseDockerBkgnd
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CWnd ) )
		&&	PaintDockerBkgnd( true, dc, (CWnd*)pHelperSrc )
		)
		bPaintDefaultBackground = false;
	if( bPaintDefaultBackground )
	{
		COLORREF clrBk = GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam );
		dc.FillSolidRect( &rcClient, clrBk );
	}
}

void CExtPaintManager::PaintShortcutListScrollButton(
	CDC & dc,
	bool bHorzLayout,
	const RECT & rcButton,
	bool bUpOrLeftBtn,
	bool bHover,
	bool bPressed,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		::IsRectEmpty( &rcButton )
		||	( ! dc.RectVisible( &rcButton ) )
		)
		return;
CRect rcGlyph( rcButton );
COLORREF clr3dFace = GetColor( COLOR_3DFACE, pHelperSrc, lParam );
COLORREF clrOuterLT = clr3dFace;
COLORREF clrOuterRB = clr3dFace;
COLORREF clrInnerLT = clr3dFace;
COLORREF clrInnerRB = clr3dFace;
	if( m_bShortcutListUseButtonStyleInScrollButtons )
	{
		PAINTPUSHBUTTONDATA _ppbd(
			pHelperSrc,
			true, // bHorz
			rcButton,
			NULL, // sText
			NULL, // hIcon
			false, //bool bFlat
			bHover,
			bPressed,
			false, // bIndeterminate
			true, // bEnabled
			true, // bDrawBorder
			false, // bDrawFocusRect
			false,
			__ALIGN_HORIZ_CENTER,
			NULL,
			false,
			0,
			false
			);
		CRect _rcButton = rcButton;
		CExtMemoryDC dcX( &dc, &_rcButton );
		PaintPushButton( dcX, _ppbd );
	} // if( m_bShortcutListUseButtonStyleInScrollButtons )
	else
	{
		dc.FillSolidRect( &rcButton, clr3dFace );
		if( bPressed )
		{
			rcGlyph.OffsetRect( 1, 1 );
			clrOuterLT = GetColor( COLOR_3DDKSHADOW, pHelperSrc, lParam );
			clrOuterRB = GetColor( COLOR_3DHILIGHT, pHelperSrc, lParam );
			clrInnerLT = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
		} // if( bPressed )
		else
		{
			if( bHover )
			{
				clrOuterRB = GetColor( COLOR_3DDKSHADOW, pHelperSrc, lParam );
				clrInnerLT = GetColor( COLOR_3DHILIGHT, pHelperSrc, lParam );
				clrInnerRB = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
			} // if( bHover )
			else
			{
				clrOuterLT = GetColor( COLOR_3DHILIGHT, pHelperSrc, lParam );
				clrOuterRB = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
			} // else from if( bHover )
		} // else from if( bPressed )
	} // else from if( m_bShortcutListUseButtonStyleInScrollButtons )

COLORREF ColorValues[2] =
{
	RGB(0,0,0),
	GetColor( COLOR_BTNTEXT, pHelperSrc, lParam )
};
	PaintGlyphCentered(
		dc,
		rcGlyph,
		* g_DockingCaptionGlyphs[
			bHorzLayout
				? ( bUpOrLeftBtn
						? __DCBT_ARROW_LEFT
						: __DCBT_ARROW_RIGHT
						)
				: ( bUpOrLeftBtn
						? __DCBT_ARROW_UP
						: __DCBT_ARROW_DOWN
						)
			],
		ColorValues
		);
	if( ! m_bShortcutListUseButtonStyleInScrollButtons )
	{
		dc.Draw3dRect( &rcButton, clrOuterLT, clrOuterRB );
		CRect rcButton2( rcButton );
		rcButton2.DeflateRect( 1, 1 );
		dc.Draw3dRect( &rcButton2, clrInnerLT, clrInnerRB );
	} // if( ! m_bShortcutListUseButtonStyleInScrollButtons )
}

CExtPaintManager::PAINTSHORTCUTLISTITEMSDATA::PAINTSHORTCUTLISTITEMSDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_dwShortcutListStyle( 0L )
	, m_hItemTextFont( (HFONT)::GetStockObject(DEFAULT_GUI_FONT) )
	, m_pIconBig( NULL )
	, m_pIconSmall( NULL )
	, m_lParamItem( 0L )
	, m_rcBoundsAll( 0,0,0,0 )
	, m_rcBoundsIcon( 0,0,0,0 )
	, m_rcBoundsText( 0,0,0,0 )
	, m_rcCtrlClient( 0,0,0,0 )
	, m_sText( _T("") )
	, m_bHover( false )
	, m_bPressed( false )
	, m_bVisible( false )
	, m_bChecked( false )
	, m_bExpandMode( false )
	, m_bHorzLayout( false )
	, m_bBigIconView( false )
	, m_bEraseQuery( false )
{
	ASSERT( m_hItemTextFont != NULL );
}

CExtPaintManager::PAINTSHORTCUTLISTITEMSDATA::PAINTSHORTCUTLISTITEMSDATA(
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
	bool bBigIconView,
	bool bEraseQuery
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_dwShortcutListStyle( dwShortcutListStyle )
	, m_hItemTextFont( hItemTextFont )
	, m_pIconBig( pIconBig )
	, m_pIconSmall( pIconSmall )
	, m_lParamItem( lParamItem )
	, m_rcBoundsAll( rcBoundsAll )
	, m_rcBoundsIcon( rcBoundsIcon )
	, m_rcBoundsText( rcBoundsText )
	, m_rcCtrlClient( rcCtrlClient )
	, m_sText( (sText == NULL) ? _T("") : sText )
	, m_bHover( bHover )
	, m_bPressed( bPressed )
	, m_bVisible( bVisible )
	, m_bChecked( bChecked )
	, m_bExpandMode( bExpandMode )
	, m_bHorzLayout( bHorzLayout )
	, m_bBigIconView( bBigIconView )
	, m_bEraseQuery( bEraseQuery )
{
	ASSERT( m_hItemTextFont != NULL );
}

void CExtPaintManager::PaintShortcutListItem(
	CDC & dc,
	CExtPaintManager::PAINTSHORTCUTLISTITEMSDATA & _pslid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( _pslid.m_bEraseQuery )
	{
		if( !_pslid.m_bExpandMode )
			return;
		if(		_pslid.m_rcBoundsText.left < _pslid.m_rcBoundsText.right
			&&	_pslid.m_rcBoundsText.top < _pslid.m_rcBoundsText.bottom
			&&	dc.RectVisible( &_pslid.m_rcBoundsText )
			)
		{
			CRect rcExpandErase( _pslid.m_rcBoundsText );
			rcExpandErase.InflateRect(
				__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_X,
				__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_Y
				);
			COLORREF clrDarkBorder = GetColor( COLOR_3DDKSHADOW, _pslid.m_pHelperSrc, _pslid.m_lParam );
			COLORREF clrInfoBk = GetColor( COLOR_INFOBK, _pslid.m_pHelperSrc, _pslid.m_lParam );
			dc.FillSolidRect( &rcExpandErase, clrInfoBk );
			dc.Draw3dRect( &rcExpandErase, clrDarkBorder, clrDarkBorder );
		}
		return;
	} // if( _pslid.m_bEraseQuery )


	if(		_pslid.m_rcBoundsAll.IsRectEmpty()
		||	( ! dc.RectVisible( &_pslid.m_rcBoundsAll ) )
		)
		return;

	if(		(! _pslid.m_rcBoundsIcon.IsRectEmpty() )
		&&	dc.RectVisible( &_pslid.m_rcBoundsIcon )
		)
	{
		CExtCmdIcon * pIcon = _pslid.m_bBigIconView
			? _pslid.m_pIconBig
			: _pslid.m_pIconSmall
			;
		if( pIcon == NULL || pIcon->IsEmpty() )
		{
			pIcon = _pslid.m_pIconBig;
			if( pIcon == NULL || pIcon->IsEmpty() )
				pIcon = _pslid.m_pIconSmall;
		} // if( pIcon == NULL || pIcon->IsEmpty() )
		CRect rcUnderIcon( _pslid.m_rcBoundsIcon );
		rcUnderIcon.InflateRect(
			__EXT_SHORTCUTLISTWND_ICON_BORDER_GAP_X,
			__EXT_SHORTCUTLISTWND_ICON_BORDER_GAP_Y
			);
		if( ( _pslid.m_bPressed || _pslid.m_bHover ) && m_bShortcutListItemUseButtonStyle )
		{
			PAINTPUSHBUTTONDATA _ppbd(
				_pslid.m_pHelperSrc,
				true, // bHorz
				rcUnderIcon,
				NULL, // sText
				NULL, // hIcon
				false, // bFlat
				_pslid.m_bHover,
				_pslid.m_bPressed,
				false, // bIndeterminate
				true, // bEnabled
				true, // bDrawBorder
				false, // bDrawFocusRect
				false,
				__ALIGN_HORIZ_CENTER,
				NULL,
				false,
				0,
				false
				);
			CExtMemoryDC dcX( &dc, &rcUnderIcon );
			PaintPushButton( dcX, _ppbd );
		} // if( ( _pslid.m_bPressed || _pslid.m_bHover ) && m_bShortcutListItemUseButtonStyle )
		if( ( _pslid.m_bPressed || _pslid.m_bHover ) && (! m_bShortcutListItemUseButtonStyle ) )
			dc.FillSolidRect(
				&rcUnderIcon,
				GetColor( COLOR_3DSHADOW, _pslid.m_pHelperSrc, _pslid.m_lParam )
				);
		if( pIcon != NULL && (! pIcon->IsEmpty()) )
		{
			pIcon->Paint(
				this,
				dc.GetSafeHdc(),
				_pslid.m_rcBoundsIcon.left,
				_pslid.m_rcBoundsIcon.top,
				_pslid.m_rcBoundsIcon.Width(),
				_pslid.m_rcBoundsIcon.Height()
				);
		} // if( pIcon != NULL && (! pIcon->IsEmpty()) )
		if( ( _pslid.m_bPressed || _pslid.m_bHover ) && (! m_bShortcutListItemUseButtonStyle ) )
		{
			COLORREF clrLT =
				GetColor(
					_pslid.m_bPressed ? COLOR_3DDKSHADOW : COLOR_3DFACE,
					_pslid.m_pHelperSrc,
					_pslid.m_lParam 
					);
			COLORREF clrRB =
				GetColor(
					_pslid.m_bPressed ? COLOR_3DFACE : COLOR_3DDKSHADOW,
					_pslid.m_pHelperSrc,
					_pslid.m_lParam 
					);
			dc.Draw3dRect(
				&rcUnderIcon,
				clrLT,
				clrRB
				);
		} // if( ( _pslid.m_bPressed || _pslid.m_bHover ) && (! m_bShortcutListItemUseButtonStyle ) )
	}

CExtSafeString sItemText = _pslid.m_sText;
	if( (! _pslid.m_rcBoundsText.IsRectEmpty() )
		&& dc.RectVisible( &_pslid.m_rcBoundsText )
		&& ( ! sItemText.IsEmpty() )
		)
	{
		LONG nItemTextLen = sItemText.GetLength();
		CRect rcCaption( _pslid.m_rcBoundsText );
		ASSERT( _pslid.m_hItemTextFont != NULL );
		CFont * pItemFont = CFont::FromHandle( _pslid.m_hItemTextFont );
		ASSERT( pItemFont != NULL );
		ASSERT( pItemFont->GetSafeHandle() != NULL );
		CFont * pOldFont = dc.SelectObject( pItemFont );
		COLORREF clrText = m_clrShortcutListItemAlternativeText;
		if( clrText == COLORREF(-1L) )
			clrText =
				GetColor(
					_pslid.m_bExpandMode ? COLOR_INFOTEXT : COLOR_3DHILIGHT,
					_pslid.m_pHelperSrc,
					_pslid.m_lParam 
					);
		COLORREF clrTextOld = dc.SetTextColor( clrText );
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CRect rcClient( _pslid.m_rcCtrlClient );
		CRect rcText( rcCaption );
		if( _pslid.m_bHorzLayout )
		{
			rcClient.DeflateRect( 0, 1 );
			rcText.top    = max( rcCaption.top, rcClient.top );
			rcText.bottom = min( rcCaption.bottom, rcClient.bottom );
		}
		else
		{
			rcClient.DeflateRect( 1, 0 );
			rcText.left  = max( rcCaption.left, rcClient.left );
			rcText.right = min( rcCaption.right, rcClient.right );
		}
		if( rcText.left < rcText.right && rcText.top < rcText.bottom )
		{
			CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
			if( _pslid.m_bHorzLayout )
				eLO = CExtRichContentLayout::__ELOT_270_CW;
			UINT nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, LPCTSTR(sItemText), nItemTextLen, &rcText, nFormat, 0
				);
		} // if( rcText.left < rcText.right && rcText.top < rcText.bottom )
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrTextOld );
		dc.SelectObject( pOldFont );
	}
}

void CExtPaintManagerXP::PaintShortcutListItem(
	CDC & dc,
	CExtPaintManager::PAINTSHORTCUTLISTITEMSDATA & _pslid
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( _pslid.m_bEraseQuery )
	{
		if( !_pslid.m_bExpandMode )
			return;
		if(		_pslid.m_rcBoundsText.left < _pslid.m_rcBoundsText.right
			&&	_pslid.m_rcBoundsText.top < _pslid.m_rcBoundsText.bottom
			&&	dc.RectVisible( &_pslid.m_rcBoundsText )
			)
		{
			CRect rcExpandErase( _pslid.m_rcBoundsText );
			rcExpandErase.InflateRect(
				__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_X,
				__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_Y
				);
			COLORREF clrDarkBorder = GetColor( COLOR_3DDKSHADOW, _pslid.m_pHelperSrc, _pslid.m_lParam );
			COLORREF clrInfoBk = GetColor( COLOR_INFOBK, _pslid.m_pHelperSrc, _pslid.m_lParam );
			dc.FillSolidRect( &rcExpandErase, clrInfoBk );
			dc.Draw3dRect( &rcExpandErase, clrDarkBorder, clrDarkBorder );
		}
		return;
	} // if( _pslid.m_bEraseQuery )


	if( _pslid.m_rcBoundsAll.IsRectEmpty()
		||	( ! dc.RectVisible( &_pslid.m_rcBoundsAll ) )
		)
		return;

	if( (! _pslid.m_rcBoundsIcon.IsRectEmpty() )
		&& dc.RectVisible( &_pslid.m_rcBoundsIcon )
		)
	{
		CExtCmdIcon * pIcon = _pslid.m_bBigIconView
			? _pslid.m_pIconBig
			: _pslid.m_pIconSmall
			;
		if( pIcon == NULL || pIcon->IsEmpty() )
		{
			pIcon = _pslid.m_pIconBig;
			if( pIcon == NULL || pIcon->IsEmpty() )
				pIcon = _pslid.m_pIconSmall;
		}
		CRect rcUnderIcon( _pslid.m_rcBoundsIcon );
		rcUnderIcon.InflateRect(
			__EXT_SHORTCUTLISTWND_ICON_BORDER_GAP_X,
			__EXT_SHORTCUTLISTWND_ICON_BORDER_GAP_Y
			);
		if( ( _pslid.m_bPressed || _pslid.m_bHover ) && m_bShortcutListItemUseButtonStyle )
		{
			PAINTPUSHBUTTONDATA _ppbd(
				_pslid.m_pHelperSrc,
				true, // bHorz
				rcUnderIcon,
				NULL, // sText
				NULL, // hIcon
				false, // bFlat
				_pslid.m_bHover,
				_pslid.m_bPressed,
				false, // bIndeterminate
				true, // bEnabled
				true, // bDrawBorder
				false, // bDrawFocusRect
				false,
				__ALIGN_HORIZ_CENTER,
				NULL,
				false,
				0,
				false
				);
			CExtMemoryDC dcX( &dc, &rcUnderIcon );
			PaintPushButton( dcX, _ppbd );
		} // if( ( _pslid.m_bPressed || _pslid.m_bHover ) && m_bShortcutListItemUseButtonStyle )
		if( ( _pslid.m_bPressed || _pslid.m_bHover ) && (! m_bShortcutListItemUseButtonStyle ) )
			dc.FillSolidRect(
				&rcUnderIcon,
				GetColor(
					_pslid.m_bPressed ? XPCLR_HILIGHT : CLR_3DFACE_IN,
					_pslid.m_pHelperSrc,
					_pslid.m_lParam 
					)
				);
		
		if( pIcon != NULL && (! pIcon->IsEmpty()) )
		{
			pIcon->Paint(
				this,
				dc.GetSafeHdc(),
				_pslid.m_rcBoundsIcon.left,
				_pslid.m_rcBoundsIcon.top,
				_pslid.m_rcBoundsIcon.Width(),
				_pslid.m_rcBoundsIcon.Height()
				);
		} // if( pIcon != NULL && (! pIcon->IsEmpty()) )
		if( ( _pslid.m_bPressed || _pslid.m_bHover ) && (! m_bShortcutListItemUseButtonStyle ) )
		{
			COLORREF clr = GetColor( XPCLR_HILIGHT_BORDER, _pslid.m_pHelperSrc, _pslid.m_lParam );
			dc.Draw3dRect( &rcUnderIcon, clr, clr );
		} // if( ( _pslid.m_bPressed || _pslid.m_bHover ) && (! m_bShortcutListItemUseButtonStyle ) )
	}

CExtSafeString sItemText = _pslid.m_sText;
	if( (! _pslid.m_rcBoundsText.IsRectEmpty() )
		&& dc.RectVisible( &_pslid.m_rcBoundsText )
		&& ( ! sItemText.IsEmpty() )
		)
	{
		LONG nItemTextLen = sItemText.GetLength();
		CRect rcCaption( _pslid.m_rcBoundsText );
		ASSERT( _pslid.m_hItemTextFont != NULL );
		CFont * pItemFont = CFont::FromHandle( _pslid.m_hItemTextFont );
		ASSERT( pItemFont != NULL );
		ASSERT( pItemFont->GetSafeHandle() != NULL );
		CFont * pOldFont = dc.SelectObject( pItemFont );
		COLORREF clrText = m_clrShortcutListItemAlternativeText;
		if( clrText == COLORREF(-1L) )
			GetColor(
				_pslid.m_bExpandMode ? COLOR_INFOTEXT : COLOR_BTNTEXT,
				_pslid.m_pHelperSrc,
				_pslid.m_lParam 
				);
		COLORREF clrTextOld = dc.SetTextColor( clrText );
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CRect rcClient( _pslid.m_rcCtrlClient );
		CRect rcText( rcCaption );
		if( _pslid.m_bHorzLayout )
		{
			rcClient.DeflateRect( 0, 1 );
			rcText.top    = max( rcCaption.top, rcClient.top );
			rcText.bottom = min( rcCaption.bottom, rcClient.bottom );
		}
		else
		{
			rcClient.DeflateRect( 1, 0 );
			rcText.left  = max( rcCaption.left, rcClient.left );
			rcText.right = min( rcCaption.right, rcClient.right );
		}
		if( rcText.left < rcText.right && rcText.top < rcText.bottom )
		{
			CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
			if( _pslid.m_bHorzLayout )
				eLO = CExtRichContentLayout::__ELOT_270_CW;
			UINT nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, LPCTSTR(sItemText), nItemTextLen, &rcText, nFormat, 0
				);
		} // if( rcText.left < rcText.right && rcText.top < rcText.bottom )
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrTextOld );
		dc.SelectObject( pOldFont );
	}
}

#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)

COLORREF CExtPaintManager::GetShadowAdjustColor()
{
	ASSERT_VALID( this );
	return ((COLORREF)(-1));
}

COLORREF CExtPaintManagerXP::GetShadowAdjustColor()
{
	ASSERT_VALID( this );
	return ((COLORREF)(-1));
}

void CExtPaintManager::FixedBar_AdjustClientRect(
	const CExtControlBar * pBar,
	CRect & rcPreCalc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtControlBar, pBar );
//	ASSERT( pBar->IsFixedMode() );
	if( pBar->IsDockedVertically() )
		rcPreCalc.DeflateRect( 1, 2 );
	else
		rcPreCalc.DeflateRect( 2, 1 );
}

void CExtPaintManagerXP::FixedBar_AdjustClientRect(
	const CExtControlBar * pBar,
	CRect & rcPreCalc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtControlBar, pBar ); pBar;
//	if( pBar->IsDockedVertically() )
//		rcPreCalc.DeflateRect( 1, 1 );
//	else
		rcPreCalc.DeflateRect( 1, 1 );
}

void CExtPaintManagerNativeXP::FixedBar_AdjustClientRect(
	const CExtControlBar * pBar,
	CRect & rcPreCalc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtControlBar, pBar );
//	ASSERT( pBar->IsFixedMode() );
	CExtPaintManager::FixedBar_AdjustClientRect( pBar, rcPreCalc );
}

INT CExtPaintManager::FixedBar_GetRowDistance(
	const CExtToolControlBar * pBar
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtToolControlBar, pBar );
//	ASSERT( pBar->IsFixedMode() );
	if( pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
		return 0;
	return __EXT_TB_ROW_DISTANCE;
}

INT CExtPaintManagerXP::FixedBar_GetRowDistance(
	const CExtToolControlBar * pBar
	)
{
	ASSERT_VALID( this );
	return CExtPaintManager::FixedBar_GetRowDistance( pBar );
}

bool CExtPaintManager::FixedBar_IsPaintRowBkMode(
	const CExtToolControlBar * pBar
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtToolControlBar, pBar );
//	ASSERT( pBar->IsFixedMode() );
	pBar;
	return false;
}

bool CExtPaintManagerXP::FixedBar_IsPaintRowBkMode(
	const CExtToolControlBar * pBar
	)
{
	ASSERT_VALID( this );
	return CExtPaintManager::FixedBar_IsPaintRowBkMode( pBar );
}

void CExtPaintManager::PaintFloatToolbarRowBk(
	CDC & dc,
	const CExtToolControlBar * pBar,
	int nLastReviewBtnIdx,
	CRect & rcRowBk
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtToolControlBar, pBar );
	ASSERT( nLastReviewBtnIdx >= 0 );
	dc;
	pBar;
	nLastReviewBtnIdx;
	rcRowBk;
}

void CExtPaintManagerXP::PaintFloatToolbarRowBk(
	CDC & dc,
	const CExtToolControlBar * pBar,
	int nLastReviewBtnIdx,
	CRect & rcRowBk
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtToolControlBar, pBar );
	ASSERT( nLastReviewBtnIdx >= 0 );
	dc;
	pBar;
	nLastReviewBtnIdx;
	rcRowBk;
}

void CExtPaintManager::OnSysColorChange()
{
	ASSERT_VALID( this );
	OnPaintSessionComplete( this );
	SyncSysColors();
	InitTranslatedColors();
	InitHelperBrushes();
}

void CExtPaintManager::OnSettingChange(
	UINT uFlags,
	__EXT_MFC_SAFE_LPCTSTR lpszSection
	)
{
	ASSERT_VALID( this );
	uFlags;
	lpszSection;
	OnPaintSessionComplete( this );
	SyncSysColors();
	InitTranslatedColors();
	InitHelperBrushes();
}

void CExtPaintManager::OnDisplayChange(
	INT nDepthBPP,
	CPoint ptSizes
	)
{
	ASSERT_VALID( this );
	nDepthBPP;
	ptSizes;
	OnPaintSessionComplete( this );
	SyncSysColors();
	InitTranslatedColors();
	InitHelperBrushes();
}

void CExtPaintManager::OnThemeChanged(
	WPARAM wParam,
	LPARAM lParam
	)
{
	ASSERT_VALID( this );
	wParam;
	lParam;
	OnPaintSessionComplete( this );
	SyncSysColors();
	InitTranslatedColors();
	InitHelperBrushes();
}

void CExtPaintManager::OnPaintSessionComplete(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
#ifdef __TRACE_PAINT_SESSIONS
		TRACE( "   PAINT_SESSION: Complete\n" );
#endif // __TRACE_PAINT_SESSIONS
	pHelperSrc;
	lParam;
}
	
bool CExtPaintManager::OnStateSerialize(
	CArchive & ar,
	bool bEnableThrowExceptions // = false
	)
{
	ar;
	bEnableThrowExceptions;
	return true;
}

CExtPaintManager::PAINTTOOLBARTEXTFIELDDATA::PAINTTOOLBARTEXTFIELDDATA()
	: m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_nDrawTextFlags( DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOPREFIX )
	, m_sText( _T("") )
	, m_rcButton( 0, 0, 0, 0 )
	, m_rcTextField( 0, 0, 0, 0 )
	, m_bComboField( false )
	, m_bHover( false )
	, m_bPushed( false )
	, m_bEnabled( false )
{
}

CExtPaintManager::PAINTTOOLBARTEXTFIELDDATA::PAINTTOOLBARTEXTFIELDDATA(
	CObject * pHelperSrc,
	__EXT_MFC_SAFE_LPCTSTR sText,
	const RECT & rcButton,
	const RECT & rcTextField,
	bool bComboField,
	bool bHover,
	bool bPushed,
	bool bEnabled
	)
	: m_pHelperSrc( pHelperSrc )
	, m_lParam( 0L )
	, m_nDrawTextFlags( DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOPREFIX )
	, m_sText( (sText == NULL) ? _T("") : sText )
	, m_rcButton( rcButton )
	, m_rcTextField( rcTextField )
	, m_bComboField( bComboField )
	, m_bHover( bHover )
	, m_bPushed( bPushed )
	, m_bEnabled( bEnabled )
{
}

void CExtPaintManager::PaintToolbarTextField(
	CDC & dc,
	PAINTTOOLBARTEXTFIELDDATA & _ptbtfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrWindow = GetColor( _ptbtfd.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE, _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam );
CRect rcTextField( _ptbtfd.m_rcButton );
	if(	!_ptbtfd.m_bEnabled )  
		rcTextField.DeflateRect( 1, 1 );
	if( _ptbtfd.m_bComboField )
	{
		if( _ptbtfd.m_bEnabled )
		{
			dc.FillSolidRect( &rcTextField, clrWindow );
			PAINTCOMBOFRAMEDATA _pcfd( _ptbtfd.m_pHelperSrc, rcTextField, _ptbtfd.m_bHover, _ptbtfd.m_bPushed, _ptbtfd.m_bEnabled );
			_pcfd.m_bNoTopOuterBorder = true;
			PaintComboFrame( dc, _pcfd );
			dc.SelectClipRgn( NULL );
		}
		else
		{
			dc.Draw3dRect(
				&rcTextField,
				GetColor( COLOR_WINDOW, _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam ),
				GetColor( COLOR_WINDOW, _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam )
				);
			INT nDD = GetDropDownButtonWidth( _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam );
			CRect rcBtn( rcTextField );
			rcBtn.left = rcBtn.right - nDD;
			// paint arrow down
			glyph_t* pGlyph = g_DockingCaptionGlyphs[ __DCBT_ARROW_SMALL_DOWN ];
			ASSERT( pGlyph != NULL );
			COLORREF ColorValues[2] = { RGB(0,0,0), ::GetSysColor( COLOR_WINDOWTEXT ) };
			PaintGlyphCentered( dc, rcBtn, *pGlyph, ColorValues );
		}
	} // if( _ptbtfd.m_bComboField )
	else
	{
		if( _ptbtfd.m_bEnabled )
		{
			PAINTCONTROLFRAMEDATA _pcfd( _ptbtfd.m_pHelperSrc, rcTextField, true, _ptbtfd.m_bHover, _ptbtfd.m_bEnabled, false, false );
			PaintControlFrame( dc, _pcfd );
		}
		else
		{
			COLORREF clrBorder = GetColor( COLOR_WINDOW, _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam );
			dc.Draw3dRect( &rcTextField, clrBorder, clrBorder );
		}
	} // else from if( _ptbtfd.m_bComboField )
CRect rcBk( _ptbtfd.m_rcTextField );
	if( ! _ptbtfd.m_bEnabled )
		rcBk.DeflateRect( 0, 1 );
	dc.FillSolidRect( &rcBk, clrWindow );
	if( ! _ptbtfd.m_sText.IsEmpty() )
	{
		COLORREF clrTextOld = dc.SetTextColor( ::GetSysColor( _ptbtfd.m_bEnabled ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT ) );
		CFont * pFontOld = dc.SelectObject( &m_FontNormal );
		int nBkModeOld = dc.SetBkMode( TRANSPARENT );
		CRect rcText( _ptbtfd.m_rcTextField );
		rcText.left += 5;
		rcText.top += 2;
		bool bRTL = ( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 ) ? true : false;
		DWORD dwDtAlignFlags = bRTL ? (DT_RTLREADING|DT_RIGHT) : 0;
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_ptbtfd.m_sText), _ptbtfd.m_sText.GetLength(), &rcText, _ptbtfd.m_nDrawTextFlags|dwDtAlignFlags, 0 );
		dc.SetBkMode( nBkModeOld );
		dc.SelectObject( pFontOld );
		dc.SetTextColor( clrTextOld );
	} // if( !_ptbtfd.m_sText.IsEmpty() )
}

void CExtPaintManagerXP::PaintToolbarTextField(
	CDC & dc,
	PAINTTOOLBARTEXTFIELDDATA & _ptbtfd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrDkBorder = GetColor( XPCLR_HILIGHT_BORDER, _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam );
CRect rcTextFieldOuter( _ptbtfd.m_rcTextField );
	if(		_ptbtfd.m_bHover 
		||	_ptbtfd.m_bPushed 
		||	(!_ptbtfd.m_bEnabled) 
		)
		rcTextFieldOuter.InflateRect( 1, 1 );
	if( _ptbtfd.m_bComboField )
	{
		// paint combo or date field
		INT nDD = GetDropDownButtonWidth( _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam );
		rcTextFieldOuter.right += nDD;
		CRect rcBtn( rcTextFieldOuter );
		rcBtn.left = rcBtn.right - nDD;
		if( (!(_ptbtfd.m_bHover || _ptbtfd.m_bPushed)) && _ptbtfd.m_bEnabled  ) 
		{
			rcBtn.DeflateRect( 3, 1, 1, 1 );
			dc.ExcludeClipRect( &rcBtn );
		}
		dc.FillSolidRect( &rcTextFieldOuter, ::GetSysColor( _ptbtfd.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE ) );
		dc.SelectClipRgn( NULL );
		if( _ptbtfd.m_bHover && _ptbtfd.m_bEnabled )
		{
			PAINTPUSHBUTTONDATA _ppbd(
				_ptbtfd.m_pHelperSrc,
				true,
				rcBtn,
				_T(""),
				NULL,
				true,
				_ptbtfd.m_bHover,
				_ptbtfd.m_bPushed,
				false,
				_ptbtfd.m_bEnabled,
				true,
				false,
				false,
				0
				);
			PaintPushButton( dc, _ppbd );
			dc.Draw3dRect( &rcTextFieldOuter, clrDkBorder, clrDkBorder );
		}
		else
		{
			if( ! _ptbtfd.m_bEnabled )
			{
				COLORREF clrBorder = GetColor( XPCLR_TEXT_FIELD_BORDER_DISABLED, _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam );
				dc.Draw3dRect( &rcTextFieldOuter, clrBorder, clrBorder );
			}
		}
		// paint arrow down
		rcBtn.top++;
		if( ! _ptbtfd.m_bEnabled )
			rcBtn.left++;
		glyph_t* pGlyph = g_DockingCaptionGlyphs[ __DCBT_ARROW_SMALL_DOWN ];
		ASSERT( pGlyph != NULL );
		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			_ptbtfd.m_bEnabled 
				? GetColor( _ptbtfd.m_bPushed ? COLOR_3DDKSHADOW : COLOR_WINDOWTEXT, _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam )
				: ::GetSysColor( COLOR_GRAYTEXT )
		};
		PaintGlyphCentered( dc, rcBtn, *pGlyph, ColorValues );
		rcTextFieldOuter.right -= nDD;
	}
	else
	{
		// paint text field
		dc.FillSolidRect( &rcTextFieldOuter, ::GetSysColor( _ptbtfd.m_bEnabled ? COLOR_WINDOW : COLOR_3DFACE ) );
		if( _ptbtfd.m_bEnabled )
		{
			if( _ptbtfd.m_bHover || _ptbtfd.m_bPushed )
				dc.Draw3dRect( &rcTextFieldOuter, clrDkBorder, clrDkBorder );
		}
		else
		{
			COLORREF clrBorder = GetColor( XPCLR_TEXT_FIELD_BORDER_DISABLED, _ptbtfd.m_pHelperSrc, _ptbtfd.m_lParam );
			dc.Draw3dRect( &rcTextFieldOuter, clrBorder, clrBorder );
		}
	}
	if( ! _ptbtfd.m_sText.IsEmpty() )
	{
		COLORREF clrTextOld = dc.SetTextColor( ::GetSysColor( _ptbtfd.m_bEnabled ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT ) );
		CFont * pFontOld = dc.SelectObject( &m_FontNormal );
		int nBkModeOld = dc.SetBkMode( TRANSPARENT );
		CRect rcText( _ptbtfd.m_rcTextField );
		rcText.left += 5;
		rcText.top += 2;
		bool bRTL = ( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 ) ? true : false;
		DWORD dwDtAlignFlags = bRTL ? (DT_RTLREADING|DT_RIGHT) : 0;
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_ptbtfd.m_sText), _ptbtfd.m_sText.GetLength(), &rcText, _ptbtfd.m_nDrawTextFlags|dwDtAlignFlags, 0 );
		dc.SetBkMode( nBkModeOld );
		dc.SelectObject( pFontOld );
		dc.SetTextColor( clrTextOld );
	} // if( !_ptbtfd.m_sText.IsEmpty() )
}

COLORREF CExtPaintManager::QueryObjectTextColor(
	CDC & dc,
	bool bEnabled,
	bool bFocused,
	bool bHovered,
	bool bPressed,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	dc; bEnabled; bFocused; bHovered; bPressed; pHelperSrc; lParam;
	return COLORREF(-1L);
}

COLORREF CExtPaintManager::GetIconAlphaColor() const
{
	ASSERT_VALID( this );
	return (COLORREF(-1L)); // no alpha icon effect
}

COLORREF CExtPaintManagerXP::GetIconAlphaColor() const
{
	ASSERT_VALID( this );
	return (COLORREF(-1L)); // no alpha icon effect
}

double CExtPaintManager::GetIconAlphaAdjustPercent() const
{
	ASSERT_VALID( this );
	return 0.0; // no alpha adjustment
}

double CExtPaintManagerXP::GetIconAlphaAdjustPercent() const
{
	ASSERT_VALID( this );
	return 0.0; // no alpha adjustment
}

bool CExtPaintManager::AdjustIcon( 
	CExtCmdIcon * pIcon
	)
{
	ASSERT_VALID( this );
	if( pIcon == NULL )
	{
		ASSERT( FALSE );
		return false;
	}

	if( (pIcon->m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_DISABLED) == 0 )
	{
		pIcon->m_bmpDisabled.Empty();
		pIcon->m_bmpDisabled = pIcon->m_bmpNormal;
		pIcon->m_bmpDisabled.Make32();

		pIcon->m_bmpDisabled.MakeMono();
		pIcon->m_bmpDisabled.AdjustAlpha( -0.75 );

		return true;
	}

	return false;
}

bool CExtPaintManagerXP::AdjustIcon( 
	CExtCmdIcon * pIcon
	)
{
	ASSERT_VALID( this );
	if( pIcon == NULL )
	{
		ASSERT( FALSE );
		return false;
	}

	if( (pIcon->m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_DISABLED) == 0 )
	{
		pIcon->m_bmpDisabled.Empty();
		pIcon->m_bmpDisabled = pIcon->m_bmpNormal;
		pIcon->m_bmpDisabled.Make32();

		COLORREF clrAccent = GetColor( XPCLR_3DFACE_NORMAL );
		double H = 0.0, S = 0.0, L = 0.0;
		CExtBitmap::stat_RGBtoHSL( clrAccent, &H, &S, &L );
		clrAccent = CExtBitmap::stat_HLStoRGB( H, L, S );
		pIcon->m_bmpDisabled.AlphaColor( RGB(255,255,255), RGB(32,32,32), 0 );
		pIcon->m_bmpDisabled.AdjustHLS( COLORREF(-1L), 0, 0.0, -1.0, -1.0 );
		pIcon->m_bmpDisabled.MakeMono( clrAccent );
		pIcon->m_bmpDisabled.AdjustHLS( COLORREF(-1L), 0, 0.0, 0.5, 0.0 );
		pIcon->m_bmpDisabled.AdjustAlpha( -0.5 );

		return true;
	}

	return false;
}

CSize CExtPaintManager::GetResizingGriperSize(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc; lParam;
	return CSize( 16, 16 );	
}

void CExtPaintManager::PaintResizingGripper(
	CDC & dc,
	const RECT & rcGrip,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		rcGrip.left > rcGrip.right
		||	rcGrip.top > rcGrip.bottom
		||	( ! dc.RectVisible(&rcGrip) )
		)
		return;
COLORREF clrLineShadow = GetColor( COLOR_WINDOW, pHelperSrc, lParam );
COLORREF clrLineFace = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
CPen penLineShadow(PS_SOLID, 1, clrLineShadow);
CPen penLineFace(PS_SOLID, 1, clrLineFace);
CPen * pOldPen = NULL;
CPoint ptTop = CPoint( rcGrip.right, rcGrip.bottom - 3 );
CPoint ptBottom = CPoint( rcGrip.right - 2, rcGrip.bottom - 1 );
	for( int nLine = 0; nLine < 3; nLine++ )
	{
		pOldPen = dc.SelectObject( &penLineFace );
		dc.MoveTo( ptBottom );
		dc.LineTo( ptTop );
		ptBottom.Offset( -1, 0 );
		ptTop.Offset( 0, -1 );
		dc.MoveTo( ptBottom );
		dc.LineTo( ptTop );
		dc.SelectObject( pOldPen );

		pOldPen = dc.SelectObject( &penLineShadow );
		ptBottom.Offset( -1, 0 );
		ptTop.Offset( 0, -1 );
		dc.MoveTo( ptBottom );
		dc.LineTo( ptTop );
		dc.SelectObject( pOldPen );
		
		ptBottom.Offset( -2, 0 );
		ptTop.Offset( 0, -2 );
	}
}

void CExtPaintManagerNativeXP::PaintResizingGripper(
	CDC & dc,
	const RECT & rcGrip,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		rcGrip.left > rcGrip.right
		||	rcGrip.top > rcGrip.bottom
		||	( ! dc.RectVisible(&rcGrip) )
		)
		return;
	if(		(! g_PaintManager.m_UxTheme.IsControlsThemed() )
		||	(	pHelperSrc != NULL
			&&	pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) )
			)
		)
	{
		CExtPaintManager::PaintResizingGripper( dc, rcGrip, pHelperSrc, lParam );
		return;
	}
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
	if( g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_STATUS ) != NULL )
	{
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				pWnd->GetSafeHwnd(), 
				dc.GetSafeHdc(), 
				SP_GRIPPER, 
				0, 
				&rcGrip, 
				&rcGrip
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

#if (!defined __EXT_MFC_NO_DATE_PICKER)

void CExtPaintManager::PaintDatePickerPushButton(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	lParam;
	ASSERT( strCaption != NULL );
	nButtonType;
	if( ! dc.RectVisible(&rcButton) )
		return;
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		pHelperSrc,
		true, rcButton, strCaption,
		NULL, bFlat, bHover, bPushed,
		false, true,
		bDrawBorder,
		false, false, 
		__ALIGN_HORIZ_CENTER | __ALIGN_VERT_CENTER,
		hFont
		);
	PaintPushButton( dc, _ppbd );
}

void CExtPaintManager::PaintDatePickerButtonsSeparator(
	CDC & dc,
	const RECT & rcSeparator,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	pHelperSrc;
	lParam;
	if( ! dc.RectVisible(&rcSeparator) )
		return;
	dc.FillSolidRect( 
		&rcSeparator, 
		::GetSysColor( COLOR_3DSHADOW )
		);
}

void CExtPaintManager::PaintDatePickerClientArea(
	CDC & dc,
	const RECT & rcTotalClient,
	const RECT & rcUsedClient,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcTotalClient) )
		return;
bool bTransparent = false;
CWnd * pWnd = NULL;
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
		pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
		if( pWnd->GetSafeHwnd() != NULL )
		{
			if( GetCb2DbTransparentMode( pWnd ) )
			{
				CExtPaintManager::stat_ExcludeChildAreas(
					dc,
					pWnd->GetSafeHwnd(),
					CExtPaintManager::stat_DefExcludeChildAreaCallback
					);
				if( PaintDockerBkgnd( true, dc, pWnd ) )
					bTransparent = true;
			} // if( GetCb2DbTransparentMode( pWnd ) )
		} // if( pWnd->GetSafeHwnd() != NULL )
	} // if( pHelperSrc != NULL )
	if( ! bTransparent )
		dc.FillSolidRect(
			&rcTotalClient,
			GetColor( CExtPaintManager::CLR_3DFACE_OUT, pHelperSrc, lParam )
			);
	dc.FillSolidRect(
		&rcUsedClient,
		GetColor( COLOR_WINDOW, pHelperSrc, lParam )
		);
}

void CExtPaintManager::PaintDatePickerBorder(
	CDC & dc,
	const RECT & rcControl,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcControl) )
		return;
COLORREF clrFace = GetColor( COLOR_BTNFACE, pHelperSrc, lParam );
COLORREF clrShadow = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
COLORREF clrDkShadow = GetColor( COLOR_3DDKSHADOW, pHelperSrc, lParam );
COLORREF clrLight = RGB(255,255,255);
CRect rcBorders( rcControl );
	dc.Draw3dRect( rcBorders, clrFace, clrDkShadow );
	rcBorders.DeflateRect(1,1);
	dc.Draw3dRect( rcBorders, clrLight, clrShadow );
	rcBorders.DeflateRect(1,1);
	dc.Draw3dRect( rcBorders, clrFace, clrFace );
	rcBorders.DeflateRect(1,1);
	dc.Draw3dRect( rcBorders, clrShadow, clrLight );
}

void CExtPaintManagerXP::PaintDatePickerBorder(
	CDC & dc,
	const RECT & rcControl,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	CExtPaintManager::PaintDatePickerBorder(
		dc,
		rcControl,
		pHelperSrc,
		lParam
		);
}

void CExtPaintManager::PaintDatePickerHeaderBackground(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	nCol;
	nRow;
	nYear;
	nMonth;
	rcHeaderDate;
	rcScrollBtnBackward;
	rcScrollBtnForward;
	dc.Draw3dRect(
		&rcHeader,
		RGB(255,255,255),
		GetColor( COLOR_3DSHADOW, pHelperSrc, lParam )
		);
	dc.FillSolidRect(
		&rcHeaderWithoutBorders,
		GetColor( CExtPaintManager::CLR_3DFACE_OUT, pHelperSrc, lParam )
		);
}

void CExtPaintManagerXP::PaintDatePickerHeaderBackground(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	CExtPaintManager::PaintDatePickerHeaderBackground(
		nCol,
		nRow,
		dc,
		nYear,
		nMonth,
		rcHeader,
		rcHeaderWithoutBorders,
		rcHeaderDate,
		rcScrollBtnBackward,
		rcScrollBtnForward,
		pHelperSrc,
		lParam
		);
}

#endif // (!defined __EXT_MFC_NO_DATE_PICKER)

#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP

void CExtPaintManager::PaintUndoRedoCaption(
	CDC & dc,
	const CRect & rcCaption,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	dc.FillSolidRect(
		&rcCaption,
		GetMenuFrameFillColor( this )
		);

	CRect rcFrame( rcCaption );
	rcFrame.DeflateRect( 3, 3 );
	rcFrame.OffsetRect( 0, 1 );
	dc.Draw3dRect( 
		&rcFrame,
		GetColor( COLOR_3DSHADOW, pHelperSrc, lParam ),
		GetColor( COLOR_WINDOW, pHelperSrc, lParam )
		);

	LOGFONT lf = { sizeof(LOGFONT) };
	VERIFY(
		::SystemParametersInfo(
		SPI_GETICONTITLELOGFONT,
		sizeof(LOGFONT), 
		&lf, 
		0 )
		);
	lf.lfWeight = FW_NORMAL;
	CFont font;
	font.CreateFontIndirect(&lf);
	
	CFont * pOldFont = dc.SelectObject( &font );
	int nOldBkMode = dc.SetBkMode( TRANSPARENT );
	COLORREF clrOldText =
		dc.SetTextColor( GetColor( COLOR_WINDOWTEXT, pHelperSrc, lParam ) );

	// text metrics
CExtSafeString sText( strCaption );
UINT nFormat = DT_LEFT|DT_TOP;
	if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
		nFormat |= DT_EXPANDTABS;
CRect rcMeasureText( rcCaption );
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		LPCTSTR(sText), sText.GetLength(),
		rcMeasureText,
		nFormat|DT_CALCRECT, 0
		);
	// align text
CRect rcText( 
		rcFrame.left + ( rcFrame.Width() - rcMeasureText.Width() ) / 2,
		rcFrame.top + ( rcFrame.Height() - rcMeasureText.Height() ) / 2,
		rcFrame.right,
		rcFrame.bottom
		);
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		strCaption, int(_tcslen(strCaption)),
		&rcText,
		nFormat|DT_END_ELLIPSIS, 0
		);
	dc.SetTextColor( clrOldText );
	dc.SetBkMode( nOldBkMode );
	dc.SelectObject( pOldFont );
}

void CExtPaintManagerXP::PaintUndoRedoCaption(
	CDC & dc,
	const CRect & rcCaption,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	dc.FillSolidRect(
		&rcCaption,
		GetMenuFrameFillColor( this )
		);

	if( strCaption == NULL )
		return;

	LOGFONT lf = { sizeof(LOGFONT) };
	VERIFY(
		::SystemParametersInfo(
		SPI_GETICONTITLELOGFONT,
		sizeof(LOGFONT), 
		&lf, 
		0 )
		);
	lf.lfWeight = FW_NORMAL;
	CFont font;
	font.CreateFontIndirect(&lf);
	
	CFont * pOldFont = dc.SelectObject( &font );
	int nOldBkMode = dc.SetBkMode( TRANSPARENT );
	COLORREF clrOldText =
		dc.SetTextColor( GetColor( COLOR_WINDOWTEXT, pHelperSrc, lParam ) );

	// text metrics
CExtSafeString sText( strCaption );
	UINT nFormat = DT_LEFT|DT_TOP;
	if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
		nFormat |= DT_EXPANDTABS;
CRect rcMeasureText( rcCaption );
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		LPCTSTR(sText), sText.GetLength(),
		rcMeasureText,
		nFormat|DT_CALCRECT, 0
		);
	// align text
CRect rcText( 
		rcCaption.left + ( rcCaption.Width() - rcMeasureText.Width() ) / 2,
		rcCaption.top + ( rcCaption.Height() - rcMeasureText.Height() ) / 2,
		rcCaption.right,
		rcCaption.bottom
		);
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		strCaption, int(_tcslen(strCaption)),
		&rcText,
		nFormat|DT_END_ELLIPSIS, 0
		);
	dc.SetTextColor( clrOldText );
	dc.SetBkMode( nOldBkMode );
	dc.SelectObject( pOldFont );
}

#endif // __EXT_MFC_NO_UNDO_REDO_POPUP

#if( !defined __EXT_MFC_NO_SPIN || !defined __EXT_MFC_NO_DURATIONWND )

CRect CExtPaintManager::Spin_QueryNcAreaMargins(
	bool bSpinAlignRight,
	bool bSpinAlignLeft,
	bool bHasScrolBars
	) const
{
	ASSERT_VALID( this );
CRect rcNcAreaMargins( 0, 0, 0, 0 );
 	rcNcAreaMargins.SetRect( 0, 3, 0, 3 );
	if( bSpinAlignRight )
	{
		rcNcAreaMargins.left = bHasScrolBars ? 2 : 0;
		rcNcAreaMargins.right = 3;
	}
	else if( bSpinAlignLeft )
	{
		rcNcAreaMargins.left = 3;
		rcNcAreaMargins.right = bHasScrolBars ? 2 : 0;
	}
	return rcNcAreaMargins;
}

CRect CExtPaintManagerNativeXP::Spin_QueryNcAreaMargins(
	bool bSpinAlignRight,
	bool bSpinAlignLeft,
	bool bHasScrolBars
	) const
{
	ASSERT_VALID( this );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return 
			CExtPaintManager::Spin_QueryNcAreaMargins(
				bSpinAlignRight,
				bSpinAlignLeft,
				bHasScrolBars
				);

CRect rcNcAreaMargins( 0, 0, 0, 0 );
 	rcNcAreaMargins.SetRect( 0, 1, 0, 1 );
	if( bSpinAlignRight )
	{
		rcNcAreaMargins.left = bHasScrolBars ? 2 : 0;
		rcNcAreaMargins.right = 1;
	}
	else if( bSpinAlignLeft )
	{
		rcNcAreaMargins.left = 1;
		rcNcAreaMargins.right = bHasScrolBars ? 2 : 0;
	}
	return rcNcAreaMargins;
}

void CExtPaintManager::PaintSpinButtonArrow(
	CDC & dc,
	const CRect & rcButton,
	bool bUp,		
	bool bHorz,
	bool bEnabled,
	bool bPushed,
	bool bHover,
	bool bHotTrack,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	CFont font;
	INT nPt = max( 6, ::MulDiv( min( rcButton.Width(), rcButton.Height() ), 2, 3 ) );
	INT nPPI = ::GetDeviceCaps( dc.m_hDC, LOGPIXELSX );
	INT nPointSize = ::MulDiv( nPt * 10, 96, nPPI );
	font.CreatePointFont( nPointSize, _T("Marlett") );

	INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
	CFont * pOldFont = dc.SelectObject( &font );

	CString sArrow = 
		bHorz 
			? ( bUp ? _T("3") : _T("4") )
			: ( bUp ? _T("5") : _T("6") );

UINT nFormat = DT_LEFT | DT_TOP | DT_SINGLELINE;
CRect rcTextDimension( rcButton );
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		LPCTSTR(sArrow), sArrow.GetLength(),
		rcTextDimension,
		nFormat|DT_CALCRECT, 0
		);
	CRect rcText( rcButton );
	rcText.OffsetRect(
		( rcButton.Width() - rcTextDimension.Width() ) / 2,
		( rcButton.Height() - rcTextDimension.Height() ) / 2
		);
	if( ( rcTextDimension.Width() & 0x01 ) == 0 )
		rcText.left ++;
	if( bPushed )
		rcText.OffsetRect( GetPushedOffset() );
	rcText.right = rcText.left + rcTextDimension.Width();
	rcText.bottom = rcText.top + rcTextDimension.Height();

	if( !bEnabled )
	{
		COLORREF clrTextBk = GetColor( COLOR_WINDOW, pHelperSrc, lParam );
		COLORREF clrTextBkOld = dc.SetTextColor( clrTextBk );
		CRect rcTextBk( rcText );
		rcTextBk.OffsetRect(1,1);
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(sArrow), -1,
			rcTextBk,
			nFormat, 0
			);
		dc.SetTextColor( clrTextBkOld );
	}

COLORREF clrText = 
		bEnabled 
			? ( (bHover && bHotTrack) ? RGB(0,0,128) : GetColor( COLOR_BTNTEXT, pHelperSrc, lParam ) )
			: GetColor( CLR_TEXT_DISABLED, pHelperSrc, lParam );
COLORREF clrTextOld = dc.SetTextColor( clrText );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(sArrow), -1, rcText, nFormat, 0 );
	dc.SetTextColor( clrTextOld );
	dc.SelectObject( pOldFont );
	dc.SetBkMode( nOldBkMode );
}

void CExtPaintManager::PaintSpinButton(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( !bEnabled )
		bHover = bPushed = false;
	dc.FillSolidRect( &rcButton, GetColor( COLOR_3DFACE, pHelperSrc, lParam ) );

	// Draw border
	if( bPushed )
	{
		if( (!bFlat) || bDrawBorder )
			dc.Draw3dRect(
				rcButton,
				GetColor( CLR_3DSHADOW_IN, pHelperSrc, lParam ),
				GetColor( CLR_3DHILIGHT_IN, pHelperSrc, lParam )
				);
	}
	else
	{
		if( (!bFlat) || (bHover && bDrawBorder) )
			dc.Draw3dRect(
				rcButton,
				GetColor( CLR_3DHILIGHT_OUT, pHelperSrc, lParam ),
				GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam )
				);
		if( (!bFlat) && bHover )
		{
			CRect rcTemp( rcButton );
			dc.Draw3dRect(
				rcTemp,
				GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam ),
				GetColor( CLR_3DDKSHADOW_OUT, pHelperSrc, lParam )
				);
			rcTemp.DeflateRect(1,1);
			dc.Draw3dRect(
				rcTemp,
				GetColor( CLR_3DHILIGHT_OUT, pHelperSrc, lParam ),
				GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam )
				);
		}
	}

	// Draw arrow
	PaintSpinButtonArrow(
		dc,
		rcButton,
		bUp,		
		bHorz,
		bEnabled,
		bPushed,
		bHover,
		bHotTrack,
		pHelperSrc,
		lParam
		);
}

void CExtPaintManagerXP::PaintSpinButton(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( !bEnabled )
		bHover = bPushed = false;

	COLORREF clrBk = GetColor( bFlat ? CLR_3DFACE_OUT : COLOR_3DFACE, pHelperSrc, lParam );
	if(	bHover )
		clrBk = GetColor( bPushed ? XPCLR_HILIGHT : CLR_3DFACE_IN, pHelperSrc, lParam );
	else if( bPushed )
		clrBk = GetColor( XPCLR_HILIGHT, pHelperSrc, lParam );
	dc.FillSolidRect( &rcButton, clrBk );

	COLORREF clrDkBorder =
		GetColor(
			( bHover || bPushed )
				? XPCLR_HILIGHT_BORDER
				: ( bFlat ? XPCLR_HILIGHT_BORDER : COLOR_3DSHADOW )
				,
			pHelperSrc,
			lParam 
			);

	// Draw border
	if( bPushed )
	{
		if( (!bFlat) || bDrawBorder )
			dc.Draw3dRect( rcButton, clrDkBorder, clrDkBorder );
	}
	else
	{
		if( (!bFlat) || ( bHover && bDrawBorder ) )
			dc.Draw3dRect( rcButton, clrDkBorder, clrDkBorder );
	}

	// Draw arrow
	PaintSpinButtonArrow(
		dc,
		rcButton,
		bUp,		
		bHorz,
		bEnabled,
		bPushed,
		bHover,
		bHotTrack,
		pHelperSrc,
		lParam
		);
}

void CExtPaintManagerNativeXP::PaintSpinButton(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::PaintSpinButton(
			dc,
			rcButton,
			bUp,		
			bHorz,
			bEnabled,
			bFlat,
			bDrawBorder,
			bPushed,
			bHover,
			bHotTrack,
			pHelperSrc,
			lParam
			);
		return;
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )

HWND hWnd = NULL;
	if(	pHelperSrc != NULL )
	{
		if( pHelperSrc->IsKindOf( RUNTIME_CLASS( CWnd ) ) )
		{
			CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
			ASSERT_VALID( pWnd );
			hWnd = pWnd->GetSafeHwnd();
		}
		else if( pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarButton ) ) )
		{
			CExtBarButton * pTBB = DYNAMIC_DOWNCAST( CExtBarButton, pHelperSrc );
			ASSERT_VALID( pTBB );
			hWnd = pTBB->GetBar()->GetSafeHwnd();
		}
	}
	ASSERT( hWnd != NULL );

INT nPartID = 0;
INT nStateID = 0;

	if( bUp )
	{
		if( bHorz )
		{
			nPartID = SPNP_UPHORZ;
			nStateID = 
				(!bEnabled) 
					? UPHZS_DISABLED 
					: bPushed 
						? UPHZS_PRESSED 
						: bHover
							? UPHZS_HOT
							: UPHZS_NORMAL;
		}
		else
		{
			nPartID = SPNP_UP;
			nStateID = 
				(!bEnabled) 
					? UPS_DISABLED 
					: bPushed 
						? UPS_PRESSED 
						: bHover
							? UPS_HOT
							: UPS_NORMAL;
		}
	}
	else
	{
		if( bHorz )
		{
			nPartID = SPNP_DOWNHORZ;
			nStateID = 
				(!bEnabled) 
					? DNHZS_DISABLED 
					: bPushed 
						? DNHZS_PRESSED 
						: bHover
							? DNHZS_HOT
							: DNHZS_NORMAL;
		}
		else
		{
			nPartID = SPNP_DOWN;
			nStateID = 
				(!bEnabled) 
					? DNS_DISABLED 
					: bPushed 
						? DNS_PRESSED 
						: bHover
							? DNS_HOT
							: DNS_NORMAL;
		}
	}

	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_SPIN ) != NULL )
	{
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd, 
				dc.GetSafeHdc(), 
				nPartID, 
				nStateID, 
				&rcButton, 
				&rcButton
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

#endif // #if( !defined __EXT_MFC_NO_SPIN || !defined __EXT_MFC_NO_DURATIONWND )

#ifndef __EXT_MFC_NO_DURATIONWND

INT CExtPaintManager::Duration_GetSpinButtonWidth(
	const CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return 15;
}


INT CExtPaintManagerNativeXP::Duration_GetSpinButtonWidth(
	const CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return 
			CExtPaintManager::Duration_GetSpinButtonWidth(
				pHelperSrc,
				lParam
				);

HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}

	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_SPIN ) != NULL )
	{
		CWindowDC dc( NULL );
		CSize szPart(0,0);
		if( g_PaintManager.m_UxTheme.GetThemePartSize(
				dc.GetSafeHdc(), 
				SPNP_UPHORZ, 
				0, 
				NULL, 
				CExtUxTheme::__EXT_UX_TS_TRUE,
				&szPart
				) == S_OK
			)
			return szPart.cy;
	}
	
	return 15;
}
INT CExtPaintManager::Duration_GetDropDownButtonWidth(
	const CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return 15;
}

INT CExtPaintManagerNativeXP::Duration_GetDropDownButtonWidth(
	const CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return 
			CExtPaintManager::Duration_GetDropDownButtonWidth(
				pHelperSrc,
				lParam
				);

//HWND hWnd = NULL;
//CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
//	if( pWnd != NULL )
//	{
//		ASSERT_VALID( pWnd );
//		hWnd = pWnd->GetSafeHwnd();
//	}
//
//	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_COMBOBOX ) != NULL )
//	{
//		CWindowDC dc( NULL );
//		CSize szPart(0,0);
//		if( g_PaintManager.m_UxTheme.GetThemePartSize(
//				dc.GetSafeHdc(), 
//				CP_DROPDOWNBUTTON, 
//				CBXS_NORMAL, 
//				NULL, 
//				CExtUxTheme::__EXT_UX_TS_TRUE,
//				&szPart
//				) == S_OK
//			)
//			return szPart.cx;
//	}
//
//	return 15;

	return  Duration_GetSpinButtonWidth( pHelperSrc, lParam );
}

CRect CExtPaintManager::Duration_GetSpinButtonExtraSpace(
	const CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return CRect( 1, 1, 1, 1 );
}

CRect CExtPaintManagerNativeXP::Duration_GetSpinButtonExtraSpace(
	const CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return CRect( 0, 0, 0, 0 );
}

CRect CExtPaintManager::Duration_GetDropDownButtonExtraSpace(
	const CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return CRect( 1, 1, 1, 1 );
}

CRect CExtPaintManagerNativeXP::Duration_GetDropDownButtonExtraSpace(
	const CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return CRect( 1, 0, 0, 0 );
}

void CExtPaintManager::Duration_PaintSpinButton(
	CDC & dc,
	const CRect & rcButton,
	bool bUp,		
	bool bEnabled,
	bool bPushed,
	bool bHovered,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcButton ) )
		return;
	PaintSpinButton(
		dc,
		rcButton,
		bUp,
		false,
		bEnabled,
		false,
		false,
		bPushed,
		bHovered,
		false,
		pHelperSrc,
		lParam
		);
}

void CExtPaintManagerNativeXP::Duration_PaintSpinButton(
	CDC & dc,
	const CRect & rcButton,
	bool bUp,		
	bool bEnabled,
	bool bPushed,
	bool bHovered,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcButton ) )
		return;

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::Duration_PaintSpinButton(
			dc,
			rcButton,
			bUp,
			bEnabled,
			bPushed,
			bHovered,
			pHelperSrc,
			lParam
			);
		return;
	}

HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}

	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_SPIN ) != NULL )
	{
		INT nPartID = 0;
		INT nStateID = 0;

		if( bUp )
		{
			nPartID = SPNP_UP;
			nStateID = 
				(!bEnabled) 
					? UPS_DISABLED 
					: bPushed 
						? UPS_PRESSED 
						: bHovered
							? UPS_HOT
							: UPS_NORMAL;
		}
		else
		{
			nPartID = SPNP_DOWN;
			nStateID = 
				(!bEnabled) 
					? DNS_DISABLED 
					: bPushed 
						? DNS_PRESSED 
						: bHovered
							? DNS_HOT
							: DNS_NORMAL;
		}

		CRect rc( rcButton );
		rc.InflateRect( 1, bUp ? 1 : -1 , 1, bUp ? -1 : 1 );

		// drop spin button
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd,
				dc.GetSafeHdc(), 
				nPartID, 
				nStateID, 
				&rc, 
				&rc
				) == S_OK
			);

		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

void CExtPaintManager::Duration_PaintDropDownButton(
	CDC & dc,
	const CRect & rcButton,
	bool bEnabled,
	bool bPushed,
	bool bHovered,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcButton ) )
		return;
	PaintSpinButton(
		dc,
		rcButton,
		false,
		false,
		bEnabled,
		false,
		false,
		bPushed,
		bHovered,
		false,
		pHelperSrc,
		lParam
		);
}

void CExtPaintManagerNativeXP::Duration_PaintDropDownButton(
	CDC & dc,
	const CRect & rcButton,
	bool bEnabled,
	bool bPushed,
	bool bHovered,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcButton ) )
		return;

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		CExtPaintManager::Duration_PaintDropDownButton(
			dc,
			rcButton,
			bEnabled,
			bPushed,
			bHovered,
			pHelperSrc,
			lParam
			);
		return;
	}

HWND hWnd = NULL;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		hWnd = pWnd->GetSafeHwnd();
	}

	if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_COMBOBOX ) != NULL )
	{
		INT nStateID = 
			(!bEnabled)
				? CBXS_DISABLED
				: bPushed
					? CBXS_PRESSED
					: bHovered
						? CBXS_HOT
						: CBXS_NORMAL;

		CRect rc( rcButton );
		if( ! g_PaintManager.m_bIsWinVistaOrLater )
			rc.InflateRect( 1, 1 );

		// drop down button
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				hWnd,
				dc.GetSafeHdc(), 
				CP_DROPDOWNBUTTON, 
				nStateID, 
				&rc, 
				&rc
				) == S_OK
			);

		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

#endif // __EXT_MFC_NO_DURATIONWND

#ifndef __EXT_MFC_NO_SLIDER

bool CExtPaintManager::PaintSliderChannel(
	CDC & dc,
	const CRect & rcChannel,
	bool bHorz,
	bool bEnabled,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bHorz;
	bEnabled;
	if( ! dc.RectVisible( &rcChannel ) )
		return false;

	dc.FillSolidRect(
		&rcChannel,
		GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam ) 
		);
	dc.Draw3dRect(
		&rcChannel,
		GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam ), 
		GetColor( CLR_3DSHADOW_OUT, pHelperSrc, lParam )
		);

	return true;
}

bool CExtPaintManagerNativeXP::PaintSliderChannel(
	CDC & dc,
	const CRect & rcChannel,
	bool bHorz,
	bool bEnabled,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcChannel ) )
		return false;

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return 
			CExtPaintManager::PaintSliderChannel(
				dc,
				rcChannel,
				bHorz,
				bEnabled,
				pHelperSrc,
				lParam
				);

CWnd * pWnd =
		DYNAMIC_DOWNCAST(
			CWnd,
			pHelperSrc
			);
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );

	if( g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_TRACKBAR ) != NULL )
	{
		INT nPartID = 
			bHorz ? TKP_TRACK : TKP_TRACKVERT;
		INT nStateID = 
			bHorz ? TKS_NORMAL : TRVS_NORMAL;
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				pWnd->GetSafeHwnd(),
				dc.GetSafeHdc(), 
				nPartID, 
				nStateID, 
				&rcChannel, 
				&rcChannel
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
		return true;
	}

	return false;
}
	
bool CExtPaintManager::PaintSliderThumb(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	lParam;
	bLeft;
	bRight;
	bFocused;
	if( ! dc.RectVisible( &rcThumb ) )
		return false;

	PAINTPUSHBUTTONDATA _ppbd(
		pHelperSrc,
		bHorz,
		rcThumb,
		_T(""),
		NULL,	// pIcon
		false,	// bFlat
		bHover,
		bPushed,
		false,	// bIndeterminate
		bEnabled,
		false,	// bDrawBorder
		false,	// bDrawFocusRect
		false,	// bDefaultPushButton
		0,
		NULL,	// hFont
		false,	// bDropDown
		0,
		false	// bTransparentBackground
		);

	PaintPushButton( dc, _ppbd );

	return true;
}

bool CExtPaintManagerNativeXP::PaintSliderThumb(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcThumb ) )
		return false;

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return 
			CExtPaintManager::PaintSliderThumb(
				dc,
				rcThumb,
				bHorz,
				bLeft,
				bRight,
				bEnabled,
				bPushed,
				bFocused,
				bHover,
				pHelperSrc,
				lParam
				);

CWnd * pWnd =
		DYNAMIC_DOWNCAST(
			CWnd,
			pHelperSrc
			);
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );

	if( g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_TRACKBAR ) != NULL )
	{
		INT nPartID = 
			bHorz 
				? (bLeft && bRight) 
					? TKP_THUMB
					: bLeft
						? TKP_THUMBTOP
						: TKP_THUMBBOTTOM
				: (bLeft && bRight) 
					? TKP_THUMBVERT
					: bLeft
						? TKP_THUMBLEFT
						: TKP_THUMBRIGHT;
		INT nStateID = 
				(!bEnabled)
					? TUS_DISABLED
					: bPushed
						? TUS_PRESSED
						: bHover
							? TUS_HOT
							: bFocused
								? TUS_FOCUSED
								: TUS_NORMAL;

 		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				pWnd->GetSafeHwnd(),
				dc.GetSafeHdc(), 
				nPartID, 
				nStateID, 
				&rcThumb, 
				&rcThumb
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
		return true;
	}

	return false;
}

bool CExtPaintManager::PaintSliderTics(
	CDC & dc,
	const CRect & rcClient,
	bool bHorz,
	bool bEnabled,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bHorz;
	bEnabled;
	pHelperSrc;
	lParam;
	if( ! dc.RectVisible( &rcClient ) )
		return false;
	return false;
}

#endif // __EXT_MFC_NO_SLIDER

bool CExtPaintManager::PaintGroupBoxFrame(
	CDC & dc,
	const RECT & rcBorder,
	COLORREF clrFrame, // = COLORREF(-1L)
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

CWnd * pWnd =
		DYNAMIC_DOWNCAST(
			CWnd,
			pHelperSrc
			);
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );

	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() ) 
	{
		if( clrFrame == COLORREF(-1L) )
			clrFrame = GetColor( CLR_GROUP_BOX_FRAME, pHelperSrc, lParam );
		CRect rcClient( rcBorder );
		dc.Draw3dRect(
			&rcClient,
			clrFrame,
			GetColor( COLOR_WINDOW, pHelperSrc, lParam )
			);
		rcClient.DeflateRect(1,1);
		dc.Draw3dRect(
			&rcClient,
			GetColor( COLOR_WINDOW, pHelperSrc, lParam ),
			clrFrame
			);
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	else
	{
		if( clrFrame == COLORREF(-1L) )
		{
			if( g_PaintManager.m_UxTheme.OpenThemeData( pWnd->GetSafeHwnd(), VSCLASS_BUTTON ) != NULL )
			{
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawThemeBackground(
						dc.GetSafeHdc(), 
						BP_GROUPBOX, 
						pWnd->IsWindowEnabled() 
						? GBS_NORMAL 
						: GBS_DISABLED, 
						&rcBorder, 
						&rcBorder
						) == S_OK
					);
				g_PaintManager.m_UxTheme.CloseThemeData();
			}
		}
		else
		{
			CBrush * pBrush =
				CBrush::FromHandle( (HBRUSH)::GetStockObject(NULL_BRUSH) );
			CBrush * pOldBrush = dc.SelectObject( pBrush );
			CPen pen;
			pen.CreatePen(
				PS_SOLID,
				1,
				clrFrame				
				);
			CPen * pOldPen = dc.SelectObject( &pen );
			dc.RoundRect(
				&rcBorder,
				CPoint( 10, 10 )
				);
			dc.SelectObject( pOldPen );
			dc.SelectObject( pOldBrush );
		}	
	} // else from if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )

	return true;
}

#if (!defined __EXT_MFC_NO_TAB_CTRL)

void CExtPaintManagerNativeXP::TabWnd_UpdateItemMeasure(
	CExtTabWnd * pTabWnd,
	CExtTabWnd::TAB_ITEM_INFO * pTii,
	CDC & dcMeasure,
	CSize & sizePreCalc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( dcMeasure.GetSafeHdc() != NULL );
	ASSERT( pTii->GetTabWnd() == pTabWnd );
bool bGroupedMode = (pTabWnd->GetTabWndStyle() & __ETWS_GROUPED) ? true : false;
	if(		bGroupedMode
		||	(! g_PaintManager.m_UxTheme.IsControlsThemed() )
		)
	{
		CExtPaintManager::TabWnd_UpdateItemMeasure( pTabWnd, pTii, dcMeasure, sizePreCalc );
		return;
	}
CSize szPart( 18, 18 );
	if( g_PaintManager.m_UxTheme.OpenThemeData( NULL, VSCLASS_TAB ) != NULL )
	{
		VERIFY(
			g_PaintManager.m_UxTheme.GetThemePartSize(
				dcMeasure.GetSafeHdc(), 
				TABP_TABITEM, 
				TIS_NORMAL, 
				NULL, 
				CExtUxTheme::__EXT_UX_TS_TRUE,
				&szPart
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
INT nPartHeight = szPart.cy;
	if( pTabWnd->OrientationIsHorizontal() )
	{
		sizePreCalc.cy = max( sizePreCalc.cy, nPartHeight );
		sizePreCalc.cx += 2*2;
	}
	else
	{
		sizePreCalc.cx = max( sizePreCalc.cx, nPartHeight );
		sizePreCalc.cy += 2*2;
	}
}

void CExtPaintManagerNativeXP::PaintTabClientArea(
	CDC & dc,
	CRect & rcClient,
	CRect & rcTabItemsArea,
	CRect & rcTabNearBorderArea,
	DWORD dwOrientation,
	bool bGroupedMode,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		bGroupedMode
		||	pHelperSrc == NULL
		||	(! g_PaintManager.m_UxTheme.IsControlsThemed() )
		)
	{
		CExtPaintManager::PaintTabClientArea( dc, rcClient, rcTabItemsArea, rcTabNearBorderArea, dwOrientation, bGroupedMode, pHelperSrc, lParam );
		return;
	}
CWnd * pGenWnd = DYNAMIC_DOWNCAST( CExtTabWnd, pHelperSrc );
//CExtTabWnd * pWndTabs = NULL;
	if( pGenWnd != NULL )
	{
		ASSERT_VALID( pGenWnd );
		PaintDockerBkgnd( true, dc, pGenWnd );
//		pWndTabs = DYNAMIC_DOWNCAST( CExtTabWnd, pGenWnd );
	}
CRect rcTabNearMargin( rcTabNearBorderArea ); // prepare tab border margin rect
	switch( dwOrientation )
	{
	case __ETWS_ORIENT_TOP:
		rcTabNearMargin.bottom = rcTabNearMargin.top + 1;
	break;
	case __ETWS_ORIENT_BOTTOM:
		rcTabNearMargin.top = rcTabNearMargin.bottom - 1;
	break;
	case __ETWS_ORIENT_LEFT:
		rcTabNearMargin.right = rcTabNearMargin.left + 1;
	break;
	case __ETWS_ORIENT_RIGHT:
		rcTabNearMargin.left = rcTabNearMargin.right - 1;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( dwOrientation )
	if( g_PaintManager.m_UxTheme.OpenThemeData( pGenWnd->GetSafeHwnd(), VSCLASS_EDIT ) != NULL )
	{
		COLORREF clrBorder = COLORREF(-1L);
		VERIFY( g_PaintManager.m_UxTheme.GetThemeColor( EP_EDITTEXT, 0, TMT_BORDERCOLOR, &clrBorder ) == S_OK );
		g_PaintManager.m_UxTheme.CloseThemeData();
		dc.FillSolidRect( &rcTabNearMargin, clrBorder );
	}
}

void CExtPaintManagerNativeXP::PaintTabItem(
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
	LPARAM lParam, // = 0L
	COLORREF clrForceText, // = COLORREF(-1L)
	COLORREF clrForceTabBk, // = COLORREF(-1L)
	COLORREF clrForceTabBorderLT, // = COLORREF(-1L)
	COLORREF clrForceTabBorderRB, // = COLORREF(-1L)
	COLORREF clrForceTabSeparator, // = COLORREF(-1L)
	bool bDwmMode // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if(		bGroupedMode
		||	(! g_PaintManager.m_UxTheme.IsControlsThemed() )
		)
	{
		CExtPaintManager::PaintTabItem(
			dc,
			rcTabItemsArea,
			bTopLeft,
			bHorz,
			bSelected,
			bCenteredText,
			bGroupedMode,
			bInGroupActive,
			bInvertedVerticalMode,
			rcEntireItem,
			sizeTextMeasured,
			pFont,
			sText,
			pIcon,
			pHelperSrc,
			lParam,
			clrForceText,
			clrForceTabBk,
			clrForceTabBorderLT,
			clrForceTabBorderRB,
			clrForceTabSeparator,
			bDwmMode
			);
		return;
	}

LONG nItemIndex = 1;

CExtCmdIcon * pIconTabItemCloseButton = NULL;
CExtCmdIcon::e_paint_type_t ePaintStateITICB =
		(CExtCmdIcon::e_paint_type_t) CExtCmdIcon::__PAINT_INVISIBLE;
CRect rcTabItemCloseButton( 0, 0, 0, 0 );
CExtBarButton * pTBB = NULL;
CExtTabWnd * pTabs = NULL;
CExtTabWnd::TAB_ITEM_INFO * pTII = NULL;
bool bNoPrefix = false;
	if( pHelperSrc != NULL )
	{
		pTabs = DYNAMIC_DOWNCAST( CExtTabWnd, pHelperSrc );
		if( pTabs != NULL )
		{
			ASSERT_VALID( pTabs );
			pTII = pTabs->ItemGet( LONG( lParam ) );
			ASSERT( pTII != NULL );
			ASSERT_VALID( pTII );
			nItemIndex = (LONG)lParam;
			pIconTabItemCloseButton =
				pTabs->OnTabWndQueryItemCloseButtonShape( pTII );
			if( pIconTabItemCloseButton != NULL )
			{
				rcTabItemCloseButton = pTII->CloseButtonRectGet();
				ePaintStateITICB = (CExtCmdIcon::e_paint_type_t)
					pTabs->OnTabWndQueryItemCloseButtonPaintState( pTII );
			} // if( pIconTabItemCloseButton != NULL )
			bNoPrefix = ( ( pTabs->GetTabWndStyleEx() & __ETWS_EX_NO_PREFIX ) != 0 ) ? true : false;
		} // if( pTabs != NULL )
		else
		{
			pTBB = DYNAMIC_DOWNCAST( CExtBarButton, pHelperSrc );
#ifdef _DEBUG
			if( pTBB != NULL )
			{
				ASSERT_VALID( pTBB );
			} // if( pTBB != NULL )
#endif // _DEBUG
		} // else from if( pTabs != NULL )
	} // if( pHelperSrc != NULL )

LONG nSelectionIndex = -1;
	if( pTabs != NULL )
		nSelectionIndex = pTabs->SelectionGet();

bool bInGroupFirst = false;
bool bInGroupLast = false;
	if( bGroupedMode && pTII != NULL )
	{
		ASSERT( pTabs != NULL );
		CExtTabWnd::TAB_ITEM_INFO * pTiiFirst = pTII->GetInGroupFirst();
		CExtTabWnd::TAB_ITEM_INFO * pTiiLast = pTII->GetInGroupLast();
		ASSERT_VALID( pTiiFirst );
		ASSERT_VALID( pTiiLast );
		bInGroupFirst = ( nItemIndex == pTabs->ItemGetIndexOf( pTiiFirst ) );
		bInGroupLast = ( nItemIndex == pTabs->ItemGetIndexOf( pTiiLast ) );
	}

bool bFirstItem = false, bLastItem = false;
	if( pTII != NULL )
	{
		ASSERT( pTabs != NULL );
		bFirstItem =
			(	nItemIndex == 0
			||	(	bInGroupFirst
				&&	nItemIndex != nSelectionIndex
				)
			);
		bLastItem = 
			(	nItemIndex == pTabs->ItemGetCount() - 1
			||	(	bInGroupLast
				&&	nItemIndex != nSelectionIndex
				)
			);
	}

bool bHover = false;
bool bPressed = false;
bool bEnabled = true;
	if( pTII != NULL )
	{
		ASSERT( pTabs != NULL );
		bEnabled = pTII->EnabledGet();
		bHover = ( pTabs->GetHoverTrackingItem() == nItemIndex ) ? true : false;
		bPressed = ( pTabs->GetPushedTrackingItem() == nItemIndex ) ? true : false;
	}
	else if( pTBB != NULL )
	{
		bEnabled = pTBB->IsEnabled();
		bHover = pTBB->IsHover();
		bPressed = pTBB->IsPressed();
	}

CRect rcItem( rcEntireItem );

HWND hWndOpenTheme = pTabs->GetSafeHwnd();
	if( hWndOpenTheme == NULL )
		hWndOpenTheme = ::GetDesktopWindow();
	if( g_PaintManager.m_UxTheme.OpenThemeData( hWndOpenTheme, VSCLASS_TAB ) != NULL )
	{
		INT nPartID = TABP_TABITEM;
		INT nStateID = 
				(!bEnabled)
					? TIS_DISABLED
					: bSelected
						? TIS_SELECTED
						: bPressed
							? TIS_FOCUSED
							: bHover
								? TIS_HOT
								: TIS_NORMAL;
		CExtBitmap _bmp1, _bmp2;
		COLORREF * pClrSurface = NULL;
		CBitmap _bmp;
		CBitmap * pOldBmp = NULL;
		CDC dcX;

bool bDetectedDynTPC = false;
	bDetectedDynTPC;
#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
		if( pTII != NULL )
		{
			CObject * pObject = pTII->EventProviderGet();
			if( pObject != NULL )
			{
				CExtDynamicControlBar * pBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, pObject );
				if( pBar != NULL )
				{
					bool bFlashCaptionHighlightedState = false;
					if( pBar->FlashCaptionIsInProgress( &bFlashCaptionHighlightedState ) )
					{
						if( bFlashCaptionHighlightedState )
						{
							clrForceText = pBar->m_clrFlashCaptionText;
							clrForceTabBk = pBar->m_clrFlashCaptionBackground;
							bSelected = true;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
							bDetectedDynTPC = true;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						}
					}
				}
			}
		}
#endif// (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

COLORREF clrColorizeTabShape = COLORREF(-1L);
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if(		pHelperSrc != NULL
			&&	clrForceTabBk != COLORREF(-1L)
			&&	(	bDetectedDynTPC
				||	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtDynTabWnd ) )
				||	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtDynAutoHideArea ) )
				)
			)
			clrColorizeTabShape = clrForceTabBk;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
		if(		pHelperSrc != NULL
			&&	clrForceTabBk != COLORREF(-1L)
			&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtTabWnd ) )
			&&	((CExtTabWnd*)pHelperSrc)->_IsMdiTabCtrl()
			)
			clrColorizeTabShape = clrForceTabBk;
#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)
		if( bHorz )
		{
			if( bTopLeft && clrColorizeTabShape == COLORREF(-1L) )
			{
				rcItem.OffsetRect( 0, 2 );
				if( bSelected )
					rcItem.InflateRect( bFirstItem ? 0 : 2, 2, bLastItem ? 0 : 2, 2 );
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWndOpenTheme,
						dc.GetSafeHdc(), 
						nPartID, 
						nStateID, 
						&rcItem, 
						&rcItem
						) == S_OK
					);
			}
			else
			{
				if( bSelected )
					rcItem.InflateRect( bFirstItem ? 0 : 2, 2, bLastItem ? 0 : 2, 2 );
				VERIFY( _bmp1.FromSurface( dc.m_hDC, rcItem ) );
				if( ! bTopLeft )
				{
					VERIFY(
						_bmp2.CreateRotated9xStack(
							_bmp1,
							180,
							1,
							rcItem.Width(),
							rcItem.Height(),
							false,
							false
							)
						);
				}
				else
					_bmp2 = _bmp1;
				HBITMAP hBmp = _bmp2.CreateBitmap( &pClrSurface );
				ASSERT( hBmp != NULL && pClrSurface != NULL );
				_bmp.Attach( hBmp );
				VERIFY( dcX.CreateCompatibleDC( NULL ) );
				pOldBmp = dcX.SelectObject( &_bmp );
				CRect rcItemX( 0, 0, rcItem.Width(), rcItem.Height() );
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWndOpenTheme,
						dcX.GetSafeHdc(), 
						nPartID, 
						nStateID, 
						&rcItemX, 
						&rcItemX
						) == S_OK
					);
				dcX.SelectObject( pOldBmp );
				_bmp2.Empty();
				VERIFY( _bmp2.FromBitmap( hBmp ) );
				_bmp1.Empty();
				if( ! bTopLeft )
				{
					VERIFY(
						_bmp1.CreateRotated9xStack(
							_bmp2,
							180,
							1,
							rcItem.Width(),
							rcItem.Height(),
							false,
							false
							)
						);
				}
				else
					_bmp1 = _bmp2;
				if( clrColorizeTabShape != COLORREF(-1L) )
				{
					_bmp1.Make32();
					_bmp1.AdjustHLS( COLORREF(-1L), COLORREF(-1L), 0.0, -0.5 );
					_bmp1.MakeMono( clrColorizeTabShape );
					_bmp1.AlphaBlend( dc.m_hDC, rcItem, 64 );
				}
				else
					_bmp1.Draw( dc.m_hDC, rcItem );
			}
		} // if( bHorz )
		else
		{
			if( bTopLeft && clrColorizeTabShape == COLORREF(-1L) )
			{
				rcItem.OffsetRect( 2, 0 );
				if( bSelected )
					rcItem.InflateRect( 2, bFirstItem ? 0 : 2, 2, bLastItem ? 0 : 2 );
				VERIFY( _bmp1.FromSurface( dc.m_hDC, rcItem ) );
				VERIFY(
					_bmp2.CreateRotated9xStack(
						_bmp1,
						90,
						1,
						rcItem.Width(),
						rcItem.Height(),
						false,
						false
						)
					);
				HBITMAP hBmp = _bmp2.CreateBitmap( &pClrSurface );
				ASSERT( hBmp != NULL && pClrSurface != NULL );
				_bmp.Attach( hBmp );
				VERIFY( dcX.CreateCompatibleDC( NULL ) );
				pOldBmp = dcX.SelectObject( &_bmp );
				CRect rcItemX( 0, 0, rcItem.Height(), rcItem.Width() );
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWndOpenTheme,
						dcX.GetSafeHdc(), 
						nPartID, 
						nStateID, 
						&rcItemX, 
						&rcItemX
						) == S_OK
					);
				dcX.SelectObject( pOldBmp );
				_bmp2.Empty();
				VERIFY( _bmp2.FromBitmap( hBmp ) );
				_bmp1.Empty();
				VERIFY(
					_bmp1.CreateRotated9xStack(
						_bmp2,
						270,
						1,
						rcItem.Height(),
						rcItem.Width(),
						false,
						false
						)
					);
				_bmp1.Draw( dc.m_hDC, rcItem );
			}
			else
			{
				if( bSelected )
					rcItem.InflateRect( 2, bFirstItem ? 0 : 2, 2, bLastItem ? 0 : 2 );
				VERIFY( _bmp1.FromSurface( dc.m_hDC, rcItem ) );
				if( ! bTopLeft )
				{
					VERIFY(
						_bmp2.CreateRotated9xStack(
							_bmp1,
							270,
							1,
							rcItem.Width(),
							rcItem.Height(),
							false,
							false
							)
						);
				}
				else
					_bmp2 = _bmp1;
				HBITMAP hBmp = _bmp2.CreateBitmap( &pClrSurface );
				ASSERT( hBmp != NULL && pClrSurface != NULL );
				_bmp.Attach( hBmp );
				VERIFY( dcX.CreateCompatibleDC( NULL ) );
				pOldBmp = dcX.SelectObject( &_bmp );
				CRect rcItemX( 0, 0, rcItem.Height(), rcItem.Width() );
				VERIFY( 
					g_PaintManager.m_UxTheme.DrawBackground(
						hWndOpenTheme,
						dcX.GetSafeHdc(), 
						nPartID, 
						nStateID, 
						&rcItemX, 
						&rcItemX
						) == S_OK
					);
				dcX.SelectObject( pOldBmp );
				_bmp2.Empty();
				VERIFY( _bmp2.FromBitmap( hBmp ) );
				_bmp1.Empty();
				if( ! bTopLeft )
				{
					VERIFY(
						_bmp1.CreateRotated9xStack(
							_bmp2,
							90,
							1,
							rcItem.Height(),
							rcItem.Width(),
							false,
							false
							)
						);
				}
				else
					_bmp1 = _bmp2;
				if( clrColorizeTabShape != COLORREF(-1L) )
				{
					_bmp1.Make32();
					_bmp1.AdjustHLS( COLORREF(-1L), COLORREF(-1L), 0.0, -0.5 );
					_bmp1.MakeMono( clrColorizeTabShape );
					_bmp1.AlphaBlend( dc.m_hDC, rcItem, 64 );
				}
				else
					_bmp1.Draw( dc.m_hDC, rcItem );
			}
		} // else if( bHorz )
		
		if( _bmp.GetSafeHandle() != NULL )
			_bmp.DeleteObject();
		
		if( dcX.GetSafeHdc() != NULL )
			dcX.DeleteDC();

		g_PaintManager.m_UxTheme.CloseThemeData();

	} // if( g_PaintManager.m_UxTheme.OpenThemeData( ...


COLORREF clrText = clrForceText;
	if( clrText == COLORREF(-1L) )
		clrText = 
			::GetSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT );

#define __EXTTAB_INDENT_TOP		2
#define __EXTTAB_INDENT_BOTTOM	2
#define __EXTTAB_INDENT_LEFT	2
#define __EXTTAB_INDENT_RIGHT	2

	rcItem.DeflateRect(
		bHorz 
			? __EXTTAB_INDENT_LEFT 
			: __EXTTAB_INDENT_TOP,
		bHorz 
			? __EXTTAB_INDENT_TOP 
			: __EXTTAB_INDENT_LEFT,
		bHorz 
			? __EXTTAB_INDENT_RIGHT 
			: __EXTTAB_INDENT_TOP,
		bHorz 
			? __EXTTAB_INDENT_BOTTOM 
			: __EXTTAB_INDENT_RIGHT
		);
		
CSize _sizeIcon( 0, 0 );
	bool bDrawIcon = (		
		  pIcon != NULL 
		  &&	(!pIcon->IsEmpty()) 
		  &&	( pTabs == NULL || (pTabs->GetTabWndStyle()&__ETWS_HIDE_ICONS) == 0  ) 
		  );
	if( bDrawIcon )
	{
		_sizeIcon = pIcon->GetSize();
		ASSERT( _sizeIcon.cx > 0 && _sizeIcon.cy > 0 );
	}

CRect rcItemForIcon( rcItem );
	if(		bDrawIcon
		&&	_sizeIcon.cx > 0
		&&	_sizeIcon.cy > 0
		)
	{
//		rcItemForIcon.DeflateRect(
//			bHorz ? 0 : ( rcItem.Width() - _sizeIcon.cx),
//			bHorz ? ( rcItem.Height() - _sizeIcon.cy) : 0
//			);
		rcItemForIcon.right = rcItemForIcon.left + _sizeIcon.cx;
		rcItemForIcon.bottom = rcItemForIcon.top + _sizeIcon.cy;
		rcItemForIcon.OffsetRect(
			bHorz ? 0 : ((rcItem.Width() - _sizeIcon.cx) / 2),
			bHorz ? ((rcItem.Height() - _sizeIcon.cy) / 2) : 0
			);
		if( rcItemForIcon.left < (rcItem.left+1) )
			rcItemForIcon.left = (rcItem.left+1);
		if( rcItemForIcon.right < (rcItem.right-1) )
			rcItemForIcon.right = (rcItem.right-1);
		if( rcItemForIcon.top < (rcItem.top+1) )
			rcItemForIcon.top = (rcItem.top+1);
		if( rcItemForIcon.bottom < (rcItem.bottom-1) )
			rcItemForIcon.bottom = (rcItem.bottom-1);
	}

CExtSafeString sItemText( (sText == NULL) ? _T("") : sText );

// IMPORTANT:  the rcText calculation fixed by Genka
CRect rcText(
		rcItem.left // rcItemForIcon.left
			+	(	bHorz
					? (_sizeIcon.cx +
						((_sizeIcon.cx > 0) ? __EXTTAB_MARGIN_ICON2TEXT_X : 0)
						)
					: 0
				),
		rcItem.top // rcItemForIcon.left
			+	(	bHorz
					? 0
					: (_sizeIcon.cy +
						((_sizeIcon.cy > 0) ? __EXTTAB_MARGIN_ICON2TEXT_Y : 0)
						)
				),
		rcItem.right,
		rcItem.bottom
		);
	if( !bHorz )
	{
		if( pIconTabItemCloseButton != NULL )
			rcText.bottom = min( rcText.bottom, rcTabItemCloseButton.top );
	} // if( !bHorz )
	else
	{
		if( pIconTabItemCloseButton != NULL )
			rcText.right = min( rcText.right, rcTabItemCloseButton.left );
	}

bool bDrawText = ( ( ! sItemText.IsEmpty() ) && rcText.Width() > 0 && rcText.Height() > 0 ) ? true : false;

INT nIconAlignment = __ALIGN_HORIZ_LEFT | __ALIGN_VERT_TOP;
	if( (!bDrawText) && !( bGroupedMode && (!bInGroupActive) ) )
	{
		if( bCenteredText )
			nIconAlignment = __ALIGN_HORIZ_CENTER|__ALIGN_VERT_CENTER;
		else
			rcItemForIcon.OffsetRect(
				bHorz ? (rcItem.Width() - _sizeIcon.cx)/2 : 0,
				bHorz ? 0 : (rcItem.Height() - _sizeIcon.cy)/2
				);
	}

	if( bDrawIcon )
	{
		if(		(bHorz && rcItemForIcon.Width() >= _sizeIcon.cx )
			||	(!bHorz && rcItemForIcon.Height() >= _sizeIcon.cy)
			)
		{
			PaintIcon(
				dc,
				true,
				pIcon,
				rcItemForIcon,
				false,
				bEnabled,
				false,
				nIconAlignment
				);
		}
	} // if( bDrawIcon )

	if( bDrawText )
	{ // if we have sense to paint text on tab item
		ASSERT( pFont != NULL );
		ASSERT( pFont->GetSafeHandle() != NULL );
		COLORREF clrOldText = dc.SetTextColor( clrText );
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );		
		CFont * pOldFont = dc.SelectObject( pFont );
		CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
		if( ! bHorz )
			eLO = bInvertedVerticalMode ? CExtRichContentLayout::__ELOT_270_CW : CExtRichContentLayout::__ELOT_90_CW;
		UINT nFormat = DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS;
		if( bCenteredText )
			nFormat |= DT_CENTER;
		else
			nFormat |= DT_LEFT;
		if( bNoPrefix )
			nFormat |= DT_NOPREFIX;
		CExtRichContentLayout::stat_DrawText(
			CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
			dc.m_hDC, LPCTSTR(sItemText), sItemText.GetLength(), rcText, nFormat, 0
			);
		dc.SelectObject( pOldFont );
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldText );
	} // if we have sense to paint text on tab item

	if( pIconTabItemCloseButton != NULL )
	{
		ASSERT( ! pIconTabItemCloseButton->IsEmpty() );
		if( dc.RectVisible( &rcTabItemCloseButton ) )
		{
			CRect _rcTabItemCloseButton = rcTabItemCloseButton;
			if( bHorz )
			{
				if( bTopLeft )
					_rcTabItemCloseButton.OffsetRect( 0, 2 );
			}
			else
			{
				if( bTopLeft )
					_rcTabItemCloseButton.OffsetRect( 2, 0 );
			}
			pIconTabItemCloseButton->Paint(
				g_PaintManager.GetPM(),
				dc.m_hDC,
				_rcTabItemCloseButton,
				ePaintStateITICB
				);
		}
	} // if( pIconTabItemCloseButton != NULL )
}

#endif // #if (!defined __EXT_MFC_NO_TAB_CTRL)

int CExtPaintManager::GetThemeDockingType() const
{
	ASSERT_VALID( this );
	return int(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2003);
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

int CExtPaintManager::GetAutoHideTabsType() const
{
	ASSERT_VALID( this );
	return int(CExtControlBar::__AUTOHIDE_TABS_TYPE_STUDIO_2003);
}

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	
bool CExtPaintManagerXP::OnQueryUseThemeColors() const
{
	ASSERT_VALID( this );
e_system_theme_t eCurrentTheme = OnQuerySystemTheme();
bool bUseThemedColors = ( g_PaintManager.m_UxTheme.IsAppThemed() &&	eCurrentTheme != ThemeUnknown );
	return bUseThemedColors;
}

bool CExtPaintManager::OnQueryUseThemeColors() const
{
	ASSERT_VALID( this );
	return false;
}

CExtPaintManager::e_system_theme_t CExtPaintManager::OnQuerySystemTheme() const
{
	if( g_PaintManager.m_eCurrentTheme == ThemeUnknown )
	{
		if( g_PaintManager.m_bIsWinVistaOrLater )
		{
			if( g_PaintManager.m_DWM.IsCompositionEnabled() )
				return ThemeVistaOrLaterDWM;
			if( g_PaintManager.m_UxTheme.IsAppThemed() )
				return ThemeVistaOrLaterUX;
		}
	}
	return g_PaintManager.m_eCurrentTheme;
}

bool CExtPaintManager::IsHighContrast() const
{
COLORREF clr3DFace = ::GetSysColor( COLOR_3DFACE );

	if(		clr3DFace == RGB(255,255,255) 
		||	clr3DFace == RGB(0,0,0)
		)
		return true;
	return false;
}

void CExtPaintManager::PaintBarButtonCombinedArea(
	CDC & dc,
	CExtBarButton * pTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pTBB );
	ASSERT_VALID( pTBB->GetBar() );
	dc;
	pTBB;
	lParam;
}

#if (!defined __EXT_MFC_NO_PAGE_NAVIGATOR )

INT CExtPaintManager::PageNavigator_GetSplitterHeight() const
{
	ASSERT_VALID( this );
	return UiScalingDo( 6, __EUIST_Y );
}

INT CExtPaintManager::PageNavigator_GetItemCaptionHeight() const
{
	ASSERT_VALID( this );
	return UiScalingDo( 24, __EUIST_Y );
}

INT CExtPaintManager::PageNavigator_GetItemPaneCaptionHeight() const
{
	ASSERT_VALID( this );
	return UiScalingDo( 19, __EUIST_Y );
}

void CExtPaintManager::PaintPageNavigatorClientArea(
	CDC & dc,
	const RECT & rcClient,
	CObject * pHelperSrc
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcClient) )
		return;
	dc.FillSolidRect( &rcClient, GetColor( COLOR_WINDOW, pHelperSrc ) );
}

void CExtPaintManager::PaintPageNavigatorBorder(
	CDC & dc,
	const RECT & rcControl
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcControl) )
		return;
	dc.Draw3dRect( &rcControl, GetColor( COLOR_3DSHADOW, this ), GetColor( COLOR_WINDOW, this ) );
}

void CExtPaintManagerXP::PaintPageNavigatorBorder(
	CDC & dc,
	const RECT & rcControl
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcControl) )
		return;
	CBrush brBorder( GetColor( COLOR_3DSHADOW, this ) );
	dc.FrameRect( &rcControl, &brBorder );
}

void CExtPaintManager::PaintPageNavigatorItemCaptionText(
	CDC & dc,
	const CRect & rcCaption,
	__EXT_MFC_SAFE_LPCTSTR strCaption
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcCaption) )
		return;
	if( strCaption == NULL )
		return;
	// caption text
	LOGFONT lf = { sizeof(LOGFONT) };
	VERIFY( ::SystemParametersInfo( SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0 ) );
	__EXT_MFC_STRCPY( lf.lfFaceName, LF_FACESIZE, _T("Arial") );
	lf.lfWeight = FW_BOLD;
	lf.lfHeight = 20;
CFont font;
	font.CreateFontIndirect(&lf);
	CFont * pOldFont = dc.SelectObject( &font );
	COLORREF clrOldText = dc.SetTextColor( GetColor( CLR_PN_CAPTION_TEXT, this ) );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
	// text metrics
	CExtSafeString sText( strCaption );
	UINT nFormat = DT_LEFT | DT_TOP | DT_END_ELLIPSIS;
	if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
		nFormat |= DT_EXPANDTABS;
CRect rcMeasureText( rcCaption );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(sText), sText.GetLength(), rcMeasureText, nFormat|DT_CALCRECT, 0 );
	// align text vertically
INT nLeftMargin = 4;
	CRect rcDrawText(
		rcCaption.left + nLeftMargin,
		rcCaption.top + ( ( rcCaption.Height() - rcMeasureText.Height() ) / 2 ),
		rcCaption.left + nLeftMargin + rcMeasureText.Width(),
		rcCaption.top + ( ( rcCaption.Height() + rcMeasureText.Height() ) / 2 )
		);
	rcDrawText.IntersectRect( rcCaption, rcDrawText );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(strCaption), int(_tcslen(strCaption)), &rcDrawText, nFormat, 0 );
	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrOldText );
	dc.SelectObject( pOldFont );
}

void CExtPaintManager::PaintPageNavigatorItemCaption(
	CDC & dc,
	const CRect & rcCaption,
	__EXT_MFC_SAFE_LPCTSTR strCaption
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcCaption) )
		return;
	dc.FillSolidRect( &rcCaption, GetColor( COLOR_3DSHADOW, this ) );
	PaintPageNavigatorItemCaptionText( dc, rcCaption, strCaption );
}

void CExtPaintManager::PaintPageNavigatorCaptionTextImpl(
	CDC & dc,
	const CRect & rcItem,
	__EXT_MFC_SAFE_LPCTSTR strCaption
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcItem) )
		return;
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
	// text metrics
CExtSafeString sText( strCaption );
UINT nFormat = DT_LEFT|DT_TOP|DT_WORDBREAK;
	if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
		nFormat |= DT_EXPANDTABS;
CRect rcMeasureText( rcItem );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(sText),  sText.GetLength(), rcMeasureText, nFormat|DT_CALCRECT, 0 );
	// align text vertically
CRect rcDrawText(
		rcItem.left,
		rcItem.top + ( ( rcItem.Height() - rcMeasureText.Height() ) / 2 ),
		rcItem.left + rcMeasureText.Width(),
		rcItem.top + ( ( rcItem.Height() + rcMeasureText.Height() ) / 2 )
		);
	rcDrawText.IntersectRect( rcItem, rcDrawText );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, strCaption, int(_tcslen(strCaption)), &rcDrawText, nFormat|DT_END_ELLIPSIS, 0 );
	dc.SetBkMode( nOldBkMode );
}

void CExtPaintManager::PaintPageNavigatorCaptionText(
	CDC & dc,
	const CRect & rcItem,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	bool bSelected,
	bool bPushed,
	bool bHover,
	bool bAllowInvertTextColor, // = true
	COLORREF clrForceText // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcItem) )
		return;
LOGFONT lf = { sizeof(LOGFONT) };
	VERIFY( ::SystemParametersInfo( SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0 ) );
	__EXT_MFC_STRCPY( lf.lfFaceName, LF_FACESIZE, _T("Arial") );
	lf.lfWeight = FW_BOLD;
CFont font;
	font.CreateFontIndirect(&lf);
CFont * pOldFont = dc.SelectObject( &font );
COLORREF clrOldText =
		dc.SetTextColor( 
			( clrForceText != COLORREF(-1L) )
				? clrForceText
				: GetColor( ( ( bPushed || bSelected ) && bHover && bAllowInvertTextColor ) ? COLOR_WINDOW : COLOR_BTNTEXT, this ) 
			);
	PaintPageNavigatorCaptionTextImpl( dc, rcItem, strCaption );
	dc.SetTextColor( clrOldText );
	dc.SelectObject( pOldFont );
}

void CExtPaintManager::PaintPageNavigatorCaption(
	CDC & dc,
	const CRect & rcItem,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	CExtCmdIcon * pIcon,
	bool bExpanded,
	bool bSelected,
	bool bPushed,
	bool bHover,
	bool bAllowInvertTextColor, // = true
	COLORREF clrForceText // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcItem) )
		return;
CSize rcIconSize = CSize(0,0);
	if( pIcon != NULL )
		rcIconSize = pIcon->GetSize();
INT nIconAreaWidth = rcIconSize.cx + ::MulDiv( rcIconSize.cx, 1, 2 );
	// caption text
	if( bExpanded && strCaption != NULL )
	{
		CRect rcText( rcItem );
		rcText.left += nIconAreaWidth;
		PaintPageNavigatorCaptionText( dc, rcText, strCaption, bSelected, bPushed, bHover, bAllowInvertTextColor, clrForceText );
	}
	// caption icon
	if( pIcon != NULL )
	{
		CRect rcIcon;
		if(	bExpanded )
			rcIcon.SetRect( 
				rcItem.left + (rcItem.left + nIconAreaWidth - rcIconSize.cx) / 2,
				rcItem.top  + (rcItem.Height() - rcIconSize.cy) / 2,
				rcItem.left + (rcItem.left + nIconAreaWidth + rcIconSize.cx) / 2,
				rcItem.top  + (rcItem.Height() + rcIconSize.cy) / 2
				);
		else
			rcIcon.SetRect( 
				rcItem.left + (rcItem.Width() - rcIconSize.cx ) / 2,
				rcItem.top  + (rcItem.Height() - rcIconSize.cy) / 2,
				rcItem.left + (rcItem.Width() + rcIconSize.cx ) / 2,
				rcItem.top  + (rcItem.Height() + rcIconSize.cy) / 2
				);
		PaintIcon( dc, true, pIcon, rcIcon, bPushed, true, bHover, __ALIGN_HORIZ_LEFT|__ALIGN_VERT_TOP );
	}
}

void CExtPaintManager::PaintPageNavigatorItemPaneCaptionTextImpl(
	CDC & dc,
	const CRect & rcCaption,
	__EXT_MFC_SAFE_LPCTSTR strCaption
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcCaption) )
		return;
	if( strCaption == NULL )
		return;
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
	// text metrics
CExtSafeString sText( strCaption );
UINT nFormat = DT_LEFT | DT_TOP;
	if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
		nFormat |= DT_EXPANDTABS;
CRect rcMeasureText( rcCaption );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(sText), sText.GetLength(), rcMeasureText, nFormat|DT_CALCRECT, 0 );
	// align text vertically
INT nLeftMargin = 7;
	CRect rcDrawText(
		rcCaption.left + nLeftMargin,
		rcCaption.top + ( ( rcCaption.Height() - rcMeasureText.Height() ) / 2 ),
		rcCaption.left + nLeftMargin + rcMeasureText.Width(),
		rcCaption.top + ( ( rcCaption.Height() + rcMeasureText.Height() ) / 2 )
		);
	rcDrawText.IntersectRect( rcCaption, rcDrawText );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, strCaption, int(_tcslen(strCaption)), &rcDrawText, nFormat|DT_END_ELLIPSIS, 0 );
	dc.SetBkMode( nOldBkMode );
}

void CExtPaintManager::PaintPageNavigatorItemPaneCaptionText(
	CDC & dc,
	const CRect & rcCaption,
	__EXT_MFC_SAFE_LPCTSTR strCaption
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcCaption) )
		return;
	if( strCaption == NULL )
		return;
LOGFONT lf = { sizeof(LOGFONT) };
	VERIFY( ::SystemParametersInfo( SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0 ) );
CFont font;
	font.CreateFontIndirect(&lf);
CFont * pOldFont = dc.SelectObject( &font );
COLORREF clrOldText = dc.SetTextColor( GetColor( COLOR_WINDOWTEXT, this ) );
	PaintPageNavigatorItemPaneCaptionTextImpl( dc, rcCaption, strCaption );
	dc.SetTextColor( clrOldText );
	dc.SelectObject( pOldFont );
}

void CExtPaintManager::PaintPageNavigatorItemPaneCaption(
	CDC & dc,
	const CRect & rcCaption,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	bool bHover,
	bool bPressed,
	bool bExpandable,
	bool bExpanded,
	bool bDrawTopLine // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bHover; bPressed; bDrawTopLine;
	if( ! dc.RectVisible(&rcCaption) )
		return;
	dc.FillSolidRect( &rcCaption, GetColor( COLOR_3DFACE, this ) );
CRect rcBottomLine( rcCaption );
	rcBottomLine.top = rcBottomLine.bottom - 1;
	dc.FillSolidRect( &rcBottomLine, GetColor( COLOR_3DSHADOW, this ) );
CRect rcText( rcCaption );
	if( bExpandable )
	{
		CExtPaintManager::glyph_t * pGlyph = 
			CExtPaintManager::g_DockingCaptionGlyphs[
				bExpanded ? CExtPaintManager::__DCBT_PN_CAPTION_EXPANDED : CExtPaintManager::__DCBT_PN_CAPTION_COLLAPSED
				];
		ASSERT( pGlyph != NULL );
		COLORREF ColorValues[2] = {  RGB(0,0,0),  GetColor( COLOR_3DDKSHADOW, this )  };
		CRect rcExpandButton( rcText );
		rcExpandButton.right -= 6;
		rcExpandButton.left = rcExpandButton.right - pGlyph->Size().cx - 1*2;
		PaintGlyphCentered( dc, rcExpandButton, *pGlyph, ColorValues );
		rcText.right = rcExpandButton.left;
	}
	PaintPageNavigatorItemPaneCaptionText( dc, rcText, strCaption );
}

void CExtPaintManager::PaintPageNavigatorConfigButton(
	CDC & dc,
	const CRect & rcItem,
	bool bPushed,
	bool bHover
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bHover; bPushed;
	if( ! dc.RectVisible(&rcItem) )
		return;
CExtPaintManager::glyph_t * pGlyph =  CExtPaintManager::g_DockingCaptionGlyphs[ CExtPaintManager::__DCBT_PN_CONFIG_BUTTON ];
	ASSERT( pGlyph != NULL );
COLORREF ColorValues[2] = {  RGB(0,0,0),  GetColor( COLOR_BTNTEXT, this )  };
	PaintGlyphCentered( dc, rcItem, *pGlyph, ColorValues );
}

void CExtPaintManager::PaintPageNavigatorItem(
	CDC & dc,
	const CRect & rcItem,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	CExtCmdIcon * pIcon,
	bool bExpanded,
	bool bSelected,
	bool bPushed,
	bool bHover
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcItem) )
		return;
CRect rcItemInner( rcItem );
	if( bExpanded )
		rcItemInner.DeflateRect( 1, 0, 1, 1 );
	dc.FillSolidRect( &rcItemInner, GetColor( ( bHover || bSelected || bPushed ) ? COLOR_3DSHADOW : COLOR_3DFACE, this ) );
	if( bExpanded )
	{
		CRect rcTopLine( rcItemInner );
		rcTopLine.bottom = rcTopLine.top + 1;
		dc.FillSolidRect( &rcTopLine, GetColor( COLOR_3DDKSHADOW, this ) );
	}
	if( bHover && (! ( bPushed || bSelected ) ) )
		dc.Draw3dRect( &rcItemInner, GetColor( COLOR_WINDOW, this ), GetColor( COLOR_3DDKSHADOW, this ) );
	else if( bPushed || bSelected ) 
		dc.Draw3dRect( &rcItemInner, GetColor( COLOR_3DDKSHADOW, this ), GetColor( COLOR_WINDOW, this ) );
	PaintPageNavigatorCaption( dc, rcItem, strCaption, pIcon, bExpanded, bSelected, bPushed, bHover, true );
}

void CExtPaintManagerXP::PaintPageNavigatorItem(
	CDC & dc,
	const CRect & rcItem,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	CExtCmdIcon * pIcon,
	bool bExpanded,
	bool bSelected,
	bool bPushed,
	bool bHover
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcItem) )
		return;
	// border
	if( bExpanded )
	{
		CBrush brBorder( GetColor( COLOR_3DSHADOW, this ) );
		dc.FrameRect( &rcItem, &brBorder );
	}
CRect rcItemInner( rcItem );
	if( bExpanded )
		rcItemInner.DeflateRect(1,1);
	if( bHover || bPushed || bSelected )
	{
		PAINTPUSHBUTTONDATA _ppbd(
			this,	// pHelperSrc
			true,	// bHorz
			rcItemInner,
			_T(""),
			NULL,	// pIcon
			true,	// bFlat
			bHover  || bSelected,	// bHover
			bPushed || bSelected,
			false,	// bIndeterminate
			true,	// bEnabled
			false,	// bDrawBorder
			false,	// bDrawFocusRect
			bSelected, // false,	// bDefaultPushButton
			0,		// eAlign
			NULL,	// hFont
			false	// bDropDown
			);
		PaintPushButton( dc, _ppbd );
	}
	else
		dc.FillSolidRect(
			&rcItemInner,
			( ::GetDeviceCaps( dc.m_hDC, BITSPIXEL ) > 8 ) ? GetColor( CLR_3DFACE_OUT, this ) : GetColor( XPCLR_3DFACE_NORMAL, this )
			);
	PaintPageNavigatorCaption( dc, rcItem, strCaption, pIcon, bExpanded, bSelected, bPushed, bHover, true );
}

void CExtPaintManager::PaintPageNavigatorSplitterDotsImpl(
	CDC & dc,
	const CRect & rcSplitter,
	COLORREF clrDark,
	COLORREF clrLight,
	INT nOffsetV, // = 0
	INT nDotCount // = 9
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
const CSize szGripDot( 2, 2 ), szGripDist( 1, 1 ), szGripShadowOffset( 1, 1 );
INT nDotsAreaHeight = szGripDot.cy + szGripShadowOffset.cy;
INT nDotsAreaWidth = nDotCount * ( szGripDot.cx + szGripDist.cx + szGripShadowOffset.cx );
CRect rcDotArea( rcSplitter );
	rcDotArea.DeflateRect( ( rcSplitter.Width() - nDotsAreaWidth ) / 2, ( rcSplitter.Height() - nDotsAreaHeight ) / 2 + 1 );
	if( nOffsetV != 0 )
		rcDotArea.OffsetRect(0, nOffsetV );
	for( INT nDot = 0; nDot < nDotCount; nDot++ )
	{
		INT nOffsetX = nDot * (szGripDot.cx + szGripShadowOffset.cx + szGripDist.cx);
		CRect rcDot( 
			rcDotArea.left + nOffsetX,
			rcDotArea.top,
			rcDotArea.left + nOffsetX + szGripDot.cx,
			rcDotArea.top + szGripDot.cy
			);
		CRect rcShadow( rcDot );
		rcShadow.OffsetRect( szGripShadowOffset );
		dc.FillSolidRect( &rcShadow, clrLight );
		dc.FillSolidRect( &rcDot, clrDark );
	} // for( INT nDot = 0; nDot < nDotCount; nDot++ )
}

void CExtPaintManager::PaintPageNavigatorSplitterDots(
	CDC & dc,
	const CRect & rcSplitter,
	INT nOffsetV // = 0
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	PaintPageNavigatorSplitterDotsImpl( dc, rcSplitter, GetColor( COLOR_3DDKSHADOW, this ), GetColor( COLOR_WINDOW, this ), nOffsetV, 9 );
}

void CExtPaintManager::PaintPageNavigatorSplitter(
	CDC & dc,
	const CRect & rcSplitter,
	bool bDrawDots // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc.FillSolidRect( &rcSplitter, GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) );
CRect rcLine( rcSplitter );
	rcLine.bottom = rcLine.top + 1;
	dc.FillSolidRect( &rcLine, GetColor( COLOR_3DSHADOW, this ) );
	rcLine.OffsetRect(0,1);
	dc.FillSolidRect( &rcLine, GetColor( COLOR_WINDOW, this ) );
	rcLine.SetRect(
		rcSplitter.left,
		rcSplitter.top + 1,
		rcSplitter.left + 1,
		rcSplitter.bottom
		);
	dc.FillSolidRect( &rcLine, GetColor( COLOR_WINDOW, this ) );
	if( bDrawDots )
		PaintPageNavigatorSplitterDots( dc, rcSplitter );
}

void CExtPaintManagerXP::PaintPageNavigatorSplitter(
	CDC & dc,
	const CRect & rcSplitter,
	bool bDrawDots // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc.FillSolidRect( &rcSplitter, GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) );
CRect rcLine( rcSplitter );
	rcLine.bottom = rcLine.top + 1;
	dc.FillSolidRect( &rcLine, GetColor( COLOR_3DSHADOW, this ) );
	rcLine.OffsetRect(0,1);
	dc.FillSolidRect( &rcLine, GetColor( COLOR_WINDOW, this ) );
	rcLine.SetRect(
		rcSplitter.left,
		rcSplitter.top + 1,
		rcSplitter.left + 1,
		rcSplitter.bottom
		);
	dc.FillSolidRect( &rcLine, GetColor( COLOR_WINDOW, this ) );
	if( bDrawDots )
		PaintPageNavigatorSplitterDots( dc, rcSplitter );
}

#endif // (!defined __EXT_MFC_NO_PAGE_NAVIGATOR )

#ifndef __EXT_MFC_NO_TAB_ONENOTE_CTRL

void CExtPaintManager::TabOneNoteWnd_DrawClientArea(
	CDC & dc,
	CRect & rcClient,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	dc.FillSolidRect(
		&rcClient, 
		GetColor( COLOR_3DFACE, pHelperSrc, lParam ) 
		);
}

void CExtPaintManager::GetTabOneNoteItemColors(
	bool bSelected,
	bool bHover,
	bool bEnabled,
	COLORREF &clrBorderLight,
	COLORREF &clrBorderDark,
	COLORREF &clrBkLight,
	COLORREF &clrBkDark,
	COLORREF &clrText
	)
{
	ASSERT_VALID( this );
	if( bHover && bEnabled )
	{
		clrText = clrBorderLight = clrBorderDark = RGB(0, 0, 128);
		clrBkDark = RGB(179, 179, 217);
		clrBkLight = RGB(233, 233, 244);
	}
	else
	{
		clrText =  GetColor( bEnabled ? COLOR_BTNTEXT : CLR_TEXT_DISABLED, this );
		clrBorderLight = GetColor( bSelected ? COLOR_WINDOW : COLOR_3DFACE, this );
		clrBorderDark = GetColor( COLOR_3DSHADOW, this );
		clrBkDark = clrBkLight = GetColor( COLOR_3DFACE, this );
	}
}

#endif // __EXT_MFC_NO_TAB_ONENOTE_CTRL

#ifndef __EXT_MFC_NO_TAB_WHIDBEY_CTRL

void CExtPaintManager::GetTabWhidbeyItemColors(
	bool bSelected,
	bool bHover,
	bool bEnabled,
	COLORREF &clrBorder,
	COLORREF &clrBkLight,
	COLORREF &clrBkDark,
	COLORREF &clrText
	)
{
	ASSERT_VALID( this );
	bHover;
	clrText = GetColor(  bEnabled ? ( bSelected ? COLOR_WINDOWTEXT : COLOR_BTNTEXT ) : CLR_TEXT_DISABLED, this );
	clrBorder = GetColor( COLOR_3DSHADOW, this );
	clrBkDark = GetColor( bSelected ? COLOR_WINDOW : COLOR_3DLIGHT, this );
	clrBkLight = GetColor( bSelected ? COLOR_WINDOW : COLOR_3DLIGHT, this );
}

#endif // __EXT_MFC_NO_TAB_WHIDBEY_CTRL


//////////////////////////////////////////////////////////////////////////
// CExtPaintManagerNativeXP
//////////////////////////////////////////////////////////////////////////

CExtPaintManagerNativeXP::CExtPaintManagerNativeXP()
{
#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
	m_bPageContainerUseButtonStyle = true;
	m_bPageContainerUseGroupBoxStyle = true;
	m_bPageContainerNoEmbossDisabledText = true;
#endif // (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
	m_bShortcutListUseButtonStyleInScrollButtons = true;
	m_bShortcutListUseDockerBkgnd = true;
	m_bShortcutListItemUseButtonStyle = true;
#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
}

CExtPaintManagerNativeXP::~CExtPaintManagerNativeXP()
{
}

bool CExtPaintManagerNativeXP::OnQueryUseThemeColors() const
{
	ASSERT_VALID( this );
	e_system_theme_t eCurrentTheme = OnQuerySystemTheme();
bool bUseThemedColors = (	
			g_PaintManager.m_UxTheme.IsAppThemed()
		&&	eCurrentTheme != ThemeUnknown
		);
	return bUseThemedColors;
}

void CExtPaintManagerNativeXP::InitTranslatedColors()
{
	ASSERT_VALID( this );
	CExtPaintManager::InitTranslatedColors();

	if(	stat_GetBPP() > 8 )
	{
		if( OnQueryUseThemeColors() )
		{
			e_system_theme_t eCurrentTheme = OnQuerySystemTheme();
			// Task Pane Colors
			COLORREF clrTPBkTop    = (COLORREF)(-1L);
			COLORREF clrTPBkBottom = (COLORREF)(-1L);
			COLORREF clrTPGroupCaptionTextNormal           = (COLORREF)(-1L);
			COLORREF clrTPGroupCaptionTextNormalHover      = (COLORREF)(-1L);
			COLORREF clrTPGroupCaptionTextHighlighted      = (COLORREF)(-1L);
			COLORREF clrTPGroupCaptionTextHighlightedHover = (COLORREF)(-1L);
			COLORREF clrTPGroupCaptionBkLeftNormal         = (COLORREF)(-1L);
			COLORREF clrTPGroupCaptionBkRightNormal        = (COLORREF)(-1L);
			COLORREF clrTPGroupCaptionBkLeftHighlighted    = (COLORREF)(-1L);
			COLORREF clrTPGroupCaptionBkRightHighlighted   = (COLORREF)(-1L);
			switch( eCurrentTheme ) 
			{
			case ThemeLunaRoyale:       // +2.87
			case ThemeVistaOrLaterUX:   // +2.87
			case ThemeVistaOrLaterDWM:  // +2.87
			case ThemeLunaBlue:
				clrTPBkTop		= RGB(123, 162, 231);
				clrTPBkBottom	= RGB(99, 117, 214);
				clrTPGroupCaptionTextNormal				= RGB( 33,  93, 198);
				clrTPGroupCaptionTextNormalHover		= RGB( 66, 142, 255);
				clrTPGroupCaptionTextHighlighted		= RGB(255, 255, 255);
				clrTPGroupCaptionTextHighlightedHover	= RGB( 66, 142, 255);
				clrTPGroupCaptionBkLeftNormal		= RGB(255, 255, 255);
				clrTPGroupCaptionBkRightNormal		= RGB(198, 211, 247);
				clrTPGroupCaptionBkLeftHighlighted	= RGB(0, 73, 181);
				clrTPGroupCaptionBkRightHighlighted	= RGB(41, 93, 206);
			break;
			case ThemeLunaOlive:
				clrTPBkTop		= RGB(204, 217, 173);
				clrTPBkBottom	= RGB(165, 189, 132);
				clrTPGroupCaptionTextNormal				= RGB( 86, 102,  45);
				clrTPGroupCaptionTextNormalHover		= RGB(114, 146, 29);
				clrTPGroupCaptionTextHighlighted		= RGB(255, 255, 255);
				clrTPGroupCaptionTextHighlightedHover	= RGB(224, 231, 184);
				clrTPGroupCaptionBkLeftNormal		= RGB(255, 252, 236);
				clrTPGroupCaptionBkRightNormal		= RGB(224, 231, 184);
				clrTPGroupCaptionBkLeftHighlighted	= RGB(119, 140, 64);
				clrTPGroupCaptionBkRightHighlighted	= RGB(150, 168, 103);
			break;
			case ThemeLunaSilver:
				clrTPBkTop		= RGB(196, 200, 212);
				clrTPBkBottom	= RGB(177, 179, 200);
				clrTPGroupCaptionTextNormal				= RGB( 63,  61, 61);
				clrTPGroupCaptionTextNormalHover		= RGB(126, 124, 124);
				clrTPGroupCaptionTextHighlighted		= RGB(255, 255, 255);
				clrTPGroupCaptionTextHighlightedHover	= RGB(230, 230, 230);
				clrTPGroupCaptionBkLeftNormal		= RGB(255, 255, 255);
				clrTPGroupCaptionBkRightNormal		= RGB(214, 215, 224);
				clrTPGroupCaptionBkLeftHighlighted	= RGB(119, 119, 146);
				clrTPGroupCaptionBkRightHighlighted	= RGB(180, 182, 199);
			break;
			default:
				clrTPBkTop = CExtBitmap::stat_RGB_Blend( GetColor( COLOR_INACTIVECAPTION, this ), GetColor( COLOR_WINDOW, this ), 128 );
				clrTPBkBottom = CExtBitmap::stat_RGB_Blend( GetColor( COLOR_ACTIVECAPTION, this ), GetColor( COLOR_WINDOW, this ), 192 );
				clrTPGroupCaptionTextNormal = CExtBitmap::stat_RGB_Blend( GetColor( COLOR_HIGHLIGHT, this ), GetColor( COLOR_WINDOWTEXT, this ), 192 );
				clrTPGroupCaptionTextNormalHover = CExtBitmap::stat_RGB_Blend( GetColor( COLOR_HIGHLIGHT, this ), GetColor( COLOR_WINDOWTEXT, this ), 240 );
				clrTPGroupCaptionTextHighlighted = GetColor( COLOR_HIGHLIGHTTEXT, this );
				clrTPGroupCaptionTextHighlightedHover = CExtBitmap::stat_RGB_Blend( GetColor( COLOR_HIGHLIGHTTEXT, this ), GetColor( COLOR_HIGHLIGHT, this ), 128 );
				clrTPGroupCaptionBkLeftNormal = GetColor( COLOR_WINDOW, this );
				clrTPGroupCaptionBkRightNormal = CExtBitmap::stat_RGB_Blend( GetColor( COLOR_INACTIVECAPTION, this ), GetColor( COLOR_WINDOW, this ), 92 );
				clrTPGroupCaptionBkLeftHighlighted = GetColor( COLOR_ACTIVECAPTION, this );
				clrTPGroupCaptionBkRightHighlighted	= CExtBitmap::stat_RGB_Blend( GetColor( COLOR_ACTIVECAPTION, this ), GetColor( COLOR_WINDOW, this ), 92 );
			break;
			} // switch( eCurrentTheme ) 
			m_mapColorTranslate[ CLR_TASK_PANE_BK_TOP ]    = InstallColor( clrTPBkTop );
			m_mapColorTranslate[ CLR_TASK_PANE_BK_BOTTOM ] = InstallColor( clrTPBkBottom );
			m_mapColorTranslate[ CLR_TASK_PANE_GROUP_CAPTION_TEXT_NORMAL ]              = InstallColor( clrTPGroupCaptionTextNormal );
			m_mapColorTranslate[ CLR_TASK_PANE_GROUP_CAPTION_TEXT_NORMAL_HOVERED ]      = InstallColor( clrTPGroupCaptionTextNormalHover );
			m_mapColorTranslate[ CLR_TASK_PANE_GROUP_CAPTION_TEXT_HIGHLIGHTED ]         = InstallColor( clrTPGroupCaptionTextHighlighted );
			m_mapColorTranslate[ CLR_TASK_PANE_GROUP_CAPTION_TEXT_HIGHLIGHTED_HOVERED ] = InstallColor( clrTPGroupCaptionTextHighlightedHover );
			m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_BK_LEFT_NORMAL]	      = InstallColor( clrTPGroupCaptionBkLeftNormal );
			m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_BK_LEFT_HIGHLIGHTED]  = InstallColor( clrTPGroupCaptionBkLeftHighlighted );
			m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_BK_RIGHT_NORMAL]      = InstallColor( clrTPGroupCaptionBkRightNormal );
			m_mapColorTranslate[CLR_TASK_PANE_GROUP_CAPTION_BK_RIGHT_HIGHLIGHTED] = InstallColor( clrTPGroupCaptionBkRightHighlighted );
		} // if( OnQueryUseThemeColors() )
	} // if( stat_GetBPP() > 8 )

#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
	m_clrShortcutListItemAlternativeText = GetColor( COLOR_BTNTEXT );
#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
}

#if (!defined __EXT_MFC_NO_RIBBON_BAR)

CRect CExtPaintManager::Ribbon_CalcLargeDropDownRect(
	const CExtBarButton * pTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	lParam;
INT nAdvance = UiScalingDo( 39, __EUIST_Y );
CRect rc = *pTBB;
	rc.top += nAdvance;
	return rc;
}

void CExtPaintManager::Ribbon_PaintPushButton(
	CDC & dc,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;
	if( ! dc.RectVisible( &_ppbd.m_rcClient ) )
		return;
	ASSERT_VALID( _ppbd.m_pHelperSrc );
CExtBarButton * pTBB = DYNAMIC_DOWNCAST( CExtBarButton, _ppbd.m_pHelperSrc );
	ASSERT( pTBB != NULL );
CExtBarButton * pParentTBB = pTBB->ParentButtonGet();
CExtRibbonButton * pRibbonTBB = DYNAMIC_DOWNCAST( CExtRibbonButton, _ppbd.m_pHelperSrc );
CExtSafeString strText = pTBB->GetText();
INT nTextLength = strText.GetLength();
enum e_layout_case_t
{
	__ELC_TOOL,
	__ELC_SMALL,
	__ELC_LARGE,
};
e_layout_case_t eLC = __ELC_TOOL;
	if( pRibbonTBB != NULL )
	{
		if(		pParentTBB == NULL
			||	pParentTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonToolGroup ) )
			)
		{
			if( pParentTBB == NULL )
			{
				const CExtRibbonNode * pRibbonNode = pTBB->Ribbon_GetNode();
				if( pRibbonNode != NULL )
				{
					const CExtCustomizeCmdTreeNode * pParentNode = pRibbonNode->GetParentNode();
					if(		pParentNode != NULL
						&&	(	pParentNode->IsKindOf( RUNTIME_CLASS( CExtRibbonNodeRightButtonsCollection ) )
							||	pParentNode->IsKindOf( RUNTIME_CLASS( CExtRibbonNodeQuickAccessButtonsCollection ) )
							)
						)
					eLC = __ELC_SMALL;
				} // if( pRibbonNode != NULL )
			} // if( pParentTBB == NULL )
		}
		else
		{
			if( pRibbonTBB->RibbonILV_Get() == __EXT_RIBBON_ILV_SIMPLE_LARGE )
				eLC = __ELC_LARGE;
			else
				eLC = __ELC_SMALL;
		}
	} // if( pRibbonTBB != NULL )
	else
	{
		if(		pParentTBB == NULL
			||	( ! pParentTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonToolGroup ) ) )
			)
			eLC = __ELC_SMALL;
	} // if( pRibbonTBB != NULL )
INT nILV = pTBB->RibbonILV_Get();
CRect rcCP = pTBB->OnRibbonGetContentPadding();
CRect rcIcon = _ppbd.m_rcClient;
	rcIcon.DeflateRect( rcCP.left, rcCP.top, rcCP.right, rcCP.bottom );
INT nTextToIconDistance = 0;
CRect rcText =  rcIcon;
CSize _sizeIcon( 0, 0 );
CExtCmdIcon * pCmdIcon = pTBB->GetIconPtr();
	if( pCmdIcon != NULL && (! pCmdIcon->IsEmpty() ) )
	{
		_sizeIcon = Ribbon_GetIconSize( pTBB, nILV, _ppbd.m_lParam );
		if( nTextLength >= 0 )
			nTextToIconDistance = Ribbon_GetTextToIconDistance( dc, nILV, pTBB, _ppbd.m_lParam );
		if( eLC == __ELC_LARGE )
		{
			rcIcon.bottom = rcIcon.top + _sizeIcon.cy;
			rcIcon.OffsetRect( ( rcIcon.Width() - _sizeIcon.cx ) / 2, 4 );
			if( nTextLength > 0 )
			{
				rcText.top = rcIcon.bottom;
				rcText.top += nTextToIconDistance;
			};
		} // if( eLC == __ELC_LARGE )
		else
		{
			if( nTextLength > 0 )
			{
				rcIcon.right = rcText.left = rcIcon.left + _sizeIcon.cx;
				rcText.left += nTextToIconDistance;
			}
			else
			{
				rcIcon = _ppbd.m_rcClient;
				if(		pTBB->IsAbleToTrackMenu()
					//&&	_ppbd.m_bSeparatedDropDown
					&&	(! pTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonGroup ) ) )
					)
					rcIcon.right -= GetDropDownButtonWidth( _ppbd.m_pHelperSrc, _ppbd.m_lParam );
				rcIcon.OffsetRect( ( rcIcon.Width() - _sizeIcon.cx ) / 2 + 1, 0 );
			}
			rcIcon.OffsetRect( 0, ( rcIcon.Height() - _sizeIcon.cy ) / 2 );
		} // else from if( eLC == __ELC_LARGE )
	} // if( pCmdIcon != NULL && (! pCmdIcon->IsEmpty() ) )
	else
		pCmdIcon = NULL;

bool bDrawDropDownWithText = _ppbd.m_bDropDown;
	switch( eLC )
	{
	case __ELC_TOOL:
	{
		bDrawDropDownWithText = false;
		PAINTPUSHBUTTONDATA _ppbd2 = _ppbd;
		_ppbd2.m_pHelperSrc = NULL;
		_ppbd2.m_pIcon = NULL;
		_ppbd2.m_sText = _T("");
		_ppbd2.m_bTransparentBackground = false;
//		_ppbd2.m_bDropDown = false;
//		_ppbd2.m_bSeparatedDropDown = false;
//		_ppbd2.m_bPushedDropDown = false;
		_ppbd2.m_bFlat = false;
		if( ! ( _ppbd2.m_bHover || _ppbd2.m_bPushed || _ppbd2.m_bIndeterminate ) )
			_ppbd2.m_clrForceBk = GetColor( CLR_3DFACE_OUT, _ppbd2.m_pHelperSrc, _ppbd2.m_lParam );
		PaintPushButton( dc, _ppbd2 );
		if( _ppbd2.m_bDropDown )
			rcIcon.OffsetRect( _ppbd.m_rcClient.left - rcIcon.left + 5, 0 );
	}
	break; // case __ELC_TOOL
	case __ELC_LARGE:
		ASSERT_VALID( pTBB );
		rcText = Ribbon_CalcLargeDropDownRect( pTBB );
		if( ! pTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButton ) ) )
			bDrawDropDownWithText = false;
	case __ELC_SMALL:
	{
		if( _ppbd.m_bEnabled && ( _ppbd.m_bHover || _ppbd.m_bPushed || _ppbd.m_bIndeterminate )  )
		{
			PAINTPUSHBUTTONDATA _ppbd2 = _ppbd;
			_ppbd2.m_pHelperSrc = NULL;
			_ppbd2.m_pIcon = NULL;
			_ppbd2.m_sText = _T("");
			_ppbd2.m_bTransparentBackground = false;
			_ppbd2.m_bNoDropDownArrow = true;
			bool bDrawSepH = false;
			if( eLC == __ELC_LARGE )
			{
				_ppbd2.m_bDropDown = false;
				_ppbd2.m_bSeparatedDropDown = false;
				_ppbd2.m_bPushedDropDown = false;
				bDrawSepH = _ppbd.m_bDropDown && _ppbd.m_bSeparatedDropDown;
			}
			_ppbd2.m_bFlat = true;
			PaintPushButton( dc, _ppbd2 );
			if( bDrawSepH )
			{
				CRect rcSepH = rcText;
				rcSepH.bottom = rcSepH.top + 3;
				rcSepH.DeflateRect( 3, 0, 2, 0 );
// 				if( g_PaintManager.m_nLPY > 96 )
// 				{
// 					INT nAdvace = UiScalingDo( 4, __EUIST_Y );
// 					rcSepH.OffsetRect( 0, nAdvace );
// 				}
				PaintSeparator( dc, rcSepH, false, true, NULL );
			} // if( bDrawSepH )
		} // if( _ppbd.m_bEnabled && ( _ppbd.m_bHover || _ppbd.m_bPushed || _ppbd.m_bIndeterminate )  )
	}
	break; // cases __ELC_LARGE and __ELC_SMALL
	} // switch( eLC )

	if( pCmdIcon != NULL )
	{
		ASSERT( ! pCmdIcon->IsEmpty() );
		CSize _sizeRealIcon = pCmdIcon->GetSize();
		if( _sizeRealIcon != _sizeIcon )
			rcIcon.OffsetRect(
				( _sizeIcon.cx - _sizeRealIcon.cx ) / 2,
				( _sizeIcon.cy - _sizeRealIcon.cy) / 2
				);
		rcIcon.right = rcIcon.left + _sizeRealIcon.cx;
		rcIcon.bottom = rcIcon.top + _sizeRealIcon.cy;
		bool bCenterBigIconVerticallyMode = ( eLC == __ELC_LARGE && nTextLength == 0 && ( ! _ppbd.m_bDropDown ) ) ? true : false;
		if( bCenterBigIconVerticallyMode)
			rcIcon.OffsetRect(
				0,
				_ppbd.m_rcClient.top - rcIcon.top + ( _ppbd.m_rcClient.Height() - rcIcon.Height() ) / 2
				);

		CExtCmdIcon::e_paint_type_t ePT = CExtCmdIcon::__PAINT_DISABLED;
		if( _ppbd.m_bEnabled )
		{
			if( _ppbd.m_bPushed )
				ePT = CExtCmdIcon::__PAINT_PRESSED;
			else if( _ppbd.m_bHover )
				ePT = CExtCmdIcon::__PAINT_HOVER;
			else
				ePT = CExtCmdIcon::__PAINT_NORMAL;
		} // if( _ppbd.m_bEnabled )
		pCmdIcon->Paint(
			this,
			dc.m_hDC,
			rcIcon,
			ePT
			);
	} // if( pCmdIcon != NULL )

	if( nTextLength > 0 || _ppbd.m_bDropDown )
	{
		UINT nDT = 0;
		if( eLC != __ELC_LARGE )
		{
			strText.Replace( _T("\r"), _T(" ") );
			strText.Replace( _T("\n"), _T(" ") );
			strText.Replace( _T("\t"), _T(" ") );
			strText.Replace( _T("  "), _T(" ") );
			strText.TrimLeft( _T(" ") );
			strText.TrimRight( _T(" ") );
			nTextLength = INT( strText.GetLength() );
			nDT = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
		} // if( eLC != __ELC_LARGE )
		else
			nDT = DT_CENTER|DT_TOP;
		if( nTextLength > 0 || _ppbd.m_bDropDown )
		{
			CFont * pFont = pTBB->GetBar()->OnGetToolbarFont( false, false, _ppbd.m_pHelperSrc );
			ASSERT( pFont->GetSafeHandle() != NULL );
			CFont * pOldFont = dc.SelectObject( pFont );
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			COLORREF clrText = GetColor( _ppbd.m_bEnabled ? COLOR_BTNTEXT : CLR_TEXT_DISABLED );
			COLORREF clrOldText = dc.SetTextColor( clrText );
			Ribbon_PaintText(
				dc,
				( nTextLength > 0 ) ? LPCTSTR(strText) : _T(""),
				rcText,
				nDT,
				bDrawDropDownWithText,
				NULL,
				_ppbd.m_pHelperSrc,
				_ppbd.m_lParam
				);
			dc.SetTextColor( clrOldText );
			dc.SetBkMode( nOldBkMode );
			dc.SelectObject( pOldFont );
		} // if( nTextLength > 0 || _ppbd.m_bDropDown )
	} // if( nTextLength > 0 || _ppbd.m_bDropDown )
}

#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

#if (!defined __EXT_MFC_NO_RIBBON_BAR)

CSize CExtPaintManager::Ribbon_GetIconSize(
	const CObject * pHelperSrc,
	INT nILV,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
CSize _size( 16, 16 );
	if( nILV < 0 || nILV == __EXT_RIBBON_ILV_SIMPLE_LARGE )
		_size.cx = _size.cy = 32;
	_size.cx = UiScalingDo( _size.cx, __EUIST_X );
	_size.cy = UiScalingDo( _size.cy, __EUIST_Y );
	return _size;
}

INT CExtPaintManager::Ribbon_GetOuterGroupDistance(
	const CExtBarButton * pTBB,
	bool bDistanceBefore,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB;
	bDistanceBefore;
	lParam;
	return 1;
}

bool CExtPaintManager::Ribbon_OptionsPageBackgroundIsDefault() const
{
	ASSERT_VALID( this );
	return false;
}

void CExtPaintManager::Ribbon_DwmAreaAdjust(
	CExtRibbonBar * pRibbonBar,
	CExtUxTheme::__EXT_UX_MARGINS & _margins,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonBar );
	lParam;
	if( Ribbon_DwmAreaCoversTabs() )
		_margins.cyTopHeight += pRibbonBar->RibbonLayout_GetTabLineHeight();
}

bool CExtPaintManager::Ribbon_DwmAreaCoversTabs() const
{
	ASSERT_VALID( this );
	return false;
}

INT CExtPaintManager::Ribbon_GetContractedMarginHeight(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pRibbonPage;
	lParam;
	return 0;
}

void CExtPaintManager::Ribbon_PaintContractedMargin(
	CDC & dc,
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	pRibbonPage;
	lParam;
}

INT CExtPaintManager::Ribbon_GetGroupHeight(
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	lParam;
CFont * pMeasureFont = (CFont*)&m_FontRibbonNormal;
	ASSERT( pMeasureFont->GetSafeHandle() != NULL );
CWindowDC dc( NULL );
CFont * pOldFont = dc.SelectObject( pMeasureFont );
TEXTMETRIC _TxtM;
	::memset( &_TxtM, 0, sizeof(TEXTMETRIC) );
	if( ! dc.GetTextMetrics( &_TxtM ) )
		_TxtM.tmHeight = 17;
	dc.SelectObject( pOldFont );
// 			//INT nBasicHeight = 95;
// 			INT nBasicHeight = _TxtM.tmHeight*3 + 56;
// 			INT nRetVal = UiScalingDo( nBasicHeight, __EUIST_Y );
INT nRetVal = _TxtM.tmHeight*3;
	if( g_PaintManager.m_nLPY > 120 )
		nRetVal += UiScalingDo( 62, __EUIST_Y );
	else
		nRetVal += UiScalingDo( 56, __EUIST_Y );
	if( pGroupTBB != NULL )
	{
		ASSERT_VALID( pGroupTBB );
		const CExtRibbonPage * pRibbonPage = pGroupTBB->GetRibbonPage();
		if( pRibbonPage != NULL )
		{
			ASSERT_VALID( pRibbonPage );
			if(		pRibbonPage->m_bHelperPopupMode
				&&	( ! pRibbonPage->m_bHelperAutoHideMode )
				&&	( ! Ribbon_IsPopupGroupWithCaption( (CObject*)pRibbonPage ) )
				)
			{
				INT nCaptionHeight = Ribbon_GroupCaptionGetHeight( NULL );
				nRetVal -= nCaptionHeight;
			}
		} // if( pRibbonPage != NULL )
	} // if( pGroupTBB != NULL )
	return nRetVal;
}

INT CExtPaintManagerNativeXP::Ribbon_GetGroupHeight(
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
INT nHeight = CExtPaintManager::Ribbon_GetGroupHeight( pGroupTBB, lParam );
	if( g_PaintManager.m_nLPY > 96 )
		nHeight += 2;
	return nHeight;
}

INT CExtPaintManager::Ribbon_GetToolGroupExtraOffset(
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( pGroupTBB );
	pGroupTBB;
	lParam;
	return -1;
}

INT CExtPaintManager::Ribbon_GetToolGroupExtraLineDistance(
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( pGroupTBB );
	pGroupTBB;
	lParam;
	return 1;
}

CRect CExtPaintManager::Ribbon_GetContentPadding(
	const CExtBarButton * pTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	lParam;
	if( pTBB->IsSeparator() )
		return CRect( 1, 1, 1, 1 );
const CExtRibbonButtonGroup * pGroupTBB = DYNAMIC_DOWNCAST( CExtRibbonButtonGroup, pTBB );
	if( pGroupTBB != NULL )
	{
		if( pGroupTBB->ParentButtonGet() == NULL )
			return CRect( 2, 2, 2, 2 );
		if( pGroupTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonToolGroup ) ) )
			return CRect( 1, 0, 1, 0 );
		return CRect( 0, 0, 0, 0 );
	}
	if(		pTBB->IsKindOf( RUNTIME_CLASS(CExtRibbonButton) )
		||	pTBB->IsKindOf( RUNTIME_CLASS(CExtBarColorButton) )
		)
	{
		const CExtBarButton * pParentTBB = pTBB->ParentButtonGet();
		if( pParentTBB != NULL )
		{
			if( pParentTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonToolGroup ) ) )
				return CRect( 0, 0, 0, 0 );
			if( pParentTBB->ParentButtonGet() == NULL )
				return CRect( 2, 0, 2, 0 );
		}
		return CRect( 3, 0, 3, 0 );
	}
	return CRect( 0, 0, 0, 0 );
}

CSize CExtPaintManager::Ribbon_GetDropDownArrowSize(
	CDC & dc,
	INT nILV,
	const CExtBarButton * pTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	dc;
	nILV;
	pTBB;
	lParam;
CSize _size( 10, 6 );
	_size.cx = UiScalingDo( _size.cx, __EUIST_X );
	_size.cy = UiScalingDo( _size.cy, __EUIST_Y );
	return _size;
}

INT CExtPaintManager::Ribbon_GetTextToIconDistance(
	CDC & dc,
	INT nILV,
	const CExtBarButton * pTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	dc;
	pTBB;
	lParam;
INT nTextToIconDistance = 0;
	if( nILV == __EXT_RIBBON_ILV_SIMPLE_LARGE )
		nTextToIconDistance = UiScalingDo( 1, __EUIST_Y );
	else
		nTextToIconDistance = UiScalingDo( 2, __EUIST_X );
	return nTextToIconDistance;
}

CSize CExtPaintManager::Ribbon_CalcButtonSize(
	CDC & dc,
	INT nILV,
	const CExtBarButton * pTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	if( pTBB->IsSeparator() )
		return CSize( 2, 2 );
const CExtRibbonNode * pRibbonNode = pTBB->Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return CSize( 0, 0 );
	ASSERT_VALID( pRibbonNode );
	if( nILV < 0 )
		nILV = pTBB->RibbonILV_Get( 0 );
CSize _sizeIcon = Ribbon_GetIconSize( pTBB, nILV, lParam );
const CExtCmdIcon * pCmdIcon = pTBB->GetIconPtr();
bool bHaveIcon = ( pCmdIcon != NULL && (! pCmdIcon->IsEmpty() ) ) ? true : false;
CExtSafeString strText;
	if( ! ( nILV == __EXT_RIBBON_ILV_SIMPLE_SMALL && bHaveIcon ) )
		strText = pTBB->GetText();
	if( nILV != __EXT_RIBBON_ILV_SIMPLE_LARGE && pCmdIcon == 0 )
		_sizeIcon.cx = _sizeIcon.cy = 0;
CSize _sizeText( 0, 0 );
UINT nDT = 0;
	if( nILV != __EXT_RIBBON_ILV_SIMPLE_LARGE )
	{
		strText.Replace( _T("\r"), _T(" ") );
		strText.Replace( _T("\n"), _T(" ") );
		strText.Replace( _T("\t"), _T(" ") );
		strText.Replace( _T("  "), _T(" ") );
		strText.TrimLeft( _T(" ") );
		strText.TrimRight( _T(" ") );
		nDT = DT_SINGLELINE|DT_LEFT|DT_TOP;
	} // if( nILV != __EXT_RIBBON_ILV_SIMPLE_LARGE )
	else
		nDT = DT_LEFT|DT_TOP;
	if( ! strText.IsEmpty() )
	{
		CFont * pFont = ((CExtToolControlBar*)(pTBB->GetBar()))->OnGetToolbarFont( false, true, const_cast < CExtBarButton * > ( pTBB ) );
		_sizeText = stat_CalcTextDimension( dc, *pFont, strText, nDT|DT_CALCRECT ).Size();
// 		_sizeText.cx = UiScalingDo( _sizeText.cx, __EUIST_X );
// 		_sizeText.cy = UiScalingDo( _sizeText.cy, __EUIST_Y );
		if( _sizeText.cx > 0 )
			_sizeText.cx += 4;
	} // if( ! strText.IsEmpty() )
CSize _sizeDDA( 0, 0 );
	if( pTBB->IsAbleToTrackMenu() )
		_sizeDDA = Ribbon_GetDropDownArrowSize( dc, nILV, pTBB, lParam );
CRect rcCP = pTBB->OnRibbonGetContentPadding();
	rcCP.left = UiScalingDo( rcCP.left, __EUIST_X );
	rcCP.top = UiScalingDo( rcCP.top, __EUIST_Y );
	rcCP.right = UiScalingDo( rcCP.right, __EUIST_X );
	rcCP.bottom = UiScalingDo( rcCP.bottom, __EUIST_Y );
INT nTextToIconDistance = Ribbon_GetTextToIconDistance( dc, nILV, pTBB, lParam );
CSize _size = _sizeIcon;
CFont * pMeasureFont = (CFont*)&m_FontRibbonNormal;
	ASSERT( pMeasureFont->GetSafeHandle() != NULL );
CFont * pOldFont = dc.SelectObject( pMeasureFont );
TEXTMETRIC _TxtM;
	::memset( &_TxtM, 0, sizeof(TEXTMETRIC) );
	if( ! dc.GetTextMetrics( &_TxtM ) )
		_TxtM.tmHeight = 17;
	dc.SelectObject( pOldFont );
INT nBasicHeight = _TxtM.tmHeight;

	if( nILV == __EXT_RIBBON_ILV_SIMPLE_LARGE )
	{
		_sizeText.cx += _sizeDDA.cx;
		if( ! strText.IsEmpty() )
		{
			_sizeText.cy = max( _sizeText.cy, _sizeDDA.cy );
			_sizeText.cy += nTextToIconDistance;
		}
		_size.cy += _sizeText.cy;
		_size.cx = max( _sizeText.cx, _size.cx );
		_size.cx += rcCP.left + rcCP.right;
		_size.cy += rcCP.top + rcCP.bottom;
//		 CSize _sizeAdjust( 42, 66 );
		 CSize _sizeAdjust( 42, nBasicHeight * 2 + 40 );
		_sizeAdjust.cx = UiScalingDo( _sizeAdjust.cx, __EUIST_X );
		_sizeAdjust.cy = UiScalingDo( _sizeAdjust.cy, __EUIST_Y );
		_size.cx = max( _size.cx, _sizeAdjust.cx );
		_size.cy = max( _size.cy, _sizeAdjust.cy );
	}
	else
	{
		if( ! strText.IsEmpty() )
		{
			_sizeText.cy = max( _sizeText.cy, _sizeDDA.cy );
			_sizeText.cx += _sizeDDA.cx;
			if( bHaveIcon )
				_sizeText.cx += nTextToIconDistance;
			_size.cx += 4;
		}
		else
		{
			const CExtBarButton * pParentTBB = pTBB->ParentButtonGet();
			if(		pParentTBB != NULL
				&&	pParentTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonToolGroup ) )
				)
				_size.cx += 8;
			else if(
					pTBB->IsAbleToTrackMenu()
				&&	(! pTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButton ) ) )
				)
				_size.cx += 7;
		}
		_size.cx += _sizeText.cx;
		_size.cy = max( _sizeText.cy, _size.cy );
		_size.cx += rcCP.left + rcCP.right;
		_size.cy += rcCP.top + rcCP.bottom;
		if( pTBB->IsAbleToTrackMenu() )
		{
			const CExtBarButton * pParentTBB = pTBB->ParentButtonGet();
			bool bInToolGroup =
				(	pParentTBB != NULL
					&&	pParentTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonToolGroup ) )
				) ? true : false;
			if(		bInToolGroup
				||	(	nILV < __EXT_RIBBON_ILV_SIMPLE_LARGE
					&&	(! pTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonGroup ) ) )
					)
				)
			{
				INT nDropDownButtonWidth = GetDropDownButtonWidth( (CObject*)pTBB, lParam );
				_size.cx += nDropDownButtonWidth;
			}
		}
//		 CSize _sizeAdjust( 22, 22 );
		 CSize _sizeAdjust( nBasicHeight + 6, nBasicHeight + 6 );
		 _sizeAdjust.cx = max( _sizeAdjust.cx, 22 );
		 _sizeAdjust.cy = max( _sizeAdjust.cy, 22 );
		_sizeAdjust.cx = UiScalingDo( _sizeAdjust.cx, __EUIST_X );
		_sizeAdjust.cy = UiScalingDo( _sizeAdjust.cy, __EUIST_Y );
		_size.cx = max( _size.cx, _sizeAdjust.cx );
		_size.cy = max( _size.cy, _sizeAdjust.cy );
	}
	return _size;
}

bool CExtPaintManager::Ribbon_GroupCaptionIsAtTop(
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pGroupTBB;
	lParam;
	return true;
}

INT CExtPaintManager::Ribbon_GroupCaptionGetHeight(
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	lParam;
	if( pGroupTBB != NULL )
	{
		ASSERT_VALID( pGroupTBB );
		const CExtRibbonPage * pRibbonPage = pGroupTBB->GetRibbonPage();
		if( pRibbonPage != NULL )
		{
			ASSERT_VALID( pRibbonPage );
			if(		pRibbonPage->m_bHelperPopupMode
				&&	( ! pRibbonPage->m_bHelperAutoHideMode )
				&&	( ! Ribbon_IsPopupGroupWithCaption( (CObject*)pRibbonPage ) )
				)
				return 0;
		} // if( pRibbonPage != NULL )
	} // if( pGroupTBB != NULL )

CFont * pMeasureFont = (CFont*)&m_FontRibbonGroupCaption;
	ASSERT( pMeasureFont->GetSafeHandle() != NULL );
CWindowDC dc( NULL );
CFont * pOldFont = dc.SelectObject( pMeasureFont );
TEXTMETRIC _TxtM;
	::memset( &_TxtM, 0, sizeof(TEXTMETRIC) );
	if( ! dc.GetTextMetrics( &_TxtM ) )
		_TxtM.tmHeight = 17;
	dc.SelectObject( pOldFont );
INT nCaptionHeight = _TxtM.tmHeight;
	nCaptionHeight = UiScalingDo( nCaptionHeight, __EUIST_Y );
	if( g_PaintManager.m_nLPY <= 96 )
		nCaptionHeight += 5;
	return nCaptionHeight;
}

CExtCmdIcon * CExtPaintManager::Ribbon_GetGroupCollapsedIcon(
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	lParam;
	if( pGroupTBB == NULL )
		return NULL;
	ASSERT_VALID( pGroupTBB );
CExtRibbonNode * pRibbonNode = (CExtRibbonNode *)pGroupTBB->Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return NULL;
	ASSERT_VALID( pRibbonNode );
	if( ! pRibbonNode->m_iconBig.IsEmpty() )
		return (& pRibbonNode->m_iconBig );
	if( ! pRibbonNode->m_iconSmall.IsEmpty() )
	{
		CSize _size = Ribbon_GetIconSize( pGroupTBB, __EXT_RIBBON_ILV_SIMPLE_LARGE );
		pRibbonNode->m_iconBig = pRibbonNode->m_iconSmall;
		pRibbonNode->m_iconBig.Scale( _size );
		return (& pRibbonNode->m_iconBig );
	}
	return NULL;
}

void CExtPaintManager::Ribbon_PaintText(
	CDC & dc,
	__EXT_MFC_SAFE_LPCTSTR strText,
	const RECT & rcText,
	UINT nDrawTextFlags,
	bool bDrawDropDownArrow,
	CExtBitmap * pBmpAlternativeDropDownArrow,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	pHelperSrc;
	lParam;
CExtSafeString strCaptionText = ( strText == NULL ) ? _T("") : strText;
	strCaptionText.TrimLeft( _T(" \r\t") );
	strCaptionText.TrimRight( _T(" \r\t") );
CExtSafeStringArray strArray;
INT nPartIndex, nPartCount, nMeasuredTextHeight = 0;
	if( ! strCaptionText.IsEmpty() )
	{
		nPartCount = 2;
		for( nPartIndex = 0; nPartIndex < nPartCount; nPartIndex ++ )
		{
			INT nSepPos = strCaptionText.Find( '\n' );
			if( nSepPos >= 0 )
			{
				CExtSafeString s = strCaptionText.Left( nSepPos );
				s.TrimLeft( _T(" \r\t") );
				s.TrimRight( _T(" \r\t") );
				if( ! s.IsEmpty() )
				{
					strArray.Add( s );
					CRect rcCalc( 0, 0, 0, 0 );
					CExtRichContentLayout::stat_DrawText(
						dc.m_hDC,
						LPCTSTR(s), INT( _tcslen( LPCTSTR(s) ) ),
						rcCalc,
						DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT, 0
						);
					nMeasuredTextHeight += rcCalc.Height();
				}
				INT nRightPartSize =
					strCaptionText.GetLength() - nSepPos - 1;
				ASSERT( nRightPartSize >= 0 );
				if( nRightPartSize == 0 )
					break;
				strCaptionText = strCaptionText.Right( nRightPartSize );
				strCaptionText.TrimLeft( _T(" \r\t") );
				strCaptionText.TrimRight( _T(" \r\t") );
				if( strCaptionText.IsEmpty() )
					break;
			} // if( nSepPos >= 0 )
			else
			{
				strArray.Add( strCaptionText );
				CRect rcCalc( 0, 0, 0, 0 );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(strCaptionText), INT( _tcslen( LPCTSTR(strCaptionText) ) ),
					rcCalc,
					DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT, 0
					);
				nMeasuredTextHeight += rcCalc.Height();
				break;
			} // else from if( nSepPos >= 0 )
		} // for( nPartIndex = 0; nPartIndex < nPartCount; nPartIndex ++ )
	} // if( ! strCaptionText.IsEmpty() )
	nPartCount = INT( strArray.GetSize() );
static const POINT g_ptArrDist = { 2, 4 };
CPoint ptArrow( 0, 0 );
CSize _sizeArrow( 0, 0 );
glyph_t * pGlyph = g_DockingCaptionGlyphs[ __DCBT_ARROW_DOWN ];
	if( bDrawDropDownArrow )
	{
		if(		pBmpAlternativeDropDownArrow != NULL
			&&	(! pBmpAlternativeDropDownArrow->IsEmpty() )
			)
			_sizeArrow = pBmpAlternativeDropDownArrow->GetSize();
		else
			_sizeArrow = pGlyph->Size();
	} // if( bDrawDropDownArrow )
CRect rcWalk = rcText;
	if(		nPartCount <= 1
		&&	pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarButton ) )
		&&	((CExtBarButton*)pHelperSrc)->RibbonILV_Get() < __EXT_RIBBON_ILV_SIMPLE_LARGE
		)
	{
		CRect rcDraw = rcText;
		CRect rcArrow = rcDraw;
		if( nPartCount == 1 || bDrawDropDownArrow )
		{
			if(		_sizeArrow.cx > 0
				&&	_sizeArrow.cy > 0
				)
			{
				rcDraw.right -= _sizeArrow.cx;
				rcArrow.left = rcDraw.right;
				if(		pBmpAlternativeDropDownArrow != NULL
					&&	(! pBmpAlternativeDropDownArrow->IsEmpty() )
					)
				{
					rcArrow.left += ( rcArrow.Width() - _sizeArrow.cx ) / 2;
					rcArrow.top  += ( rcArrow.Height() - _sizeArrow.cy ) / 2;
					rcArrow.right = rcArrow.left + _sizeArrow.cx;
					rcArrow.bottom = rcArrow.top + _sizeArrow.cy;
					if( nPartCount == 0 )
						rcArrow.OffsetRect( 0, 1 );
// rcArrow.OffsetRect(
// 	0, 
// 	( rcText.bottom - rcText.top - nMeasuredTextHeight ) / 2
// 	);
					pBmpAlternativeDropDownArrow->AlphaBlend( dc.m_hDC, rcArrow );
				}
				else
				{
					ptArrow.y -= 4;
					COLORREF ColorValues1[2] =
					{
						RGB(0,0,0),
						GetColor(COLOR_3DHILIGHT),
					};
					COLORREF ColorValues2[2] =
					{
						RGB(0,0,0),
						GetColor(COLOR_BTNTEXT),
					};
					if( nPartCount == 0 )
					{
						ptArrow.x += 3;
						ptArrow.y ++;
					}
// ptArrow.y += ( rcText.bottom - rcText.top - nMeasuredTextHeight ) / 2;
					ptArrow.y ++;
					PaintGlyphCentered( dc, rcArrow, *pGlyph, ColorValues1 );
					ptArrow.y --;
					PaintGlyphCentered( dc, rcArrow, *pGlyph, ColorValues2 );
				}
			}
		}
		if( nPartCount == 1 )
		{
			CExtSafeString & s = strArray.ElementAt( 0 );
			INT nLen = s.GetLength();
			ASSERT( nLen > 0 );
// rcDraw.OffsetRect(
// 	0, 
// 	( rcText.bottom - rcText.top - nMeasuredTextHeight ) / 2
// 	);
			CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(s), nLen, rcDraw, nDrawTextFlags, 0 );
		}
	}
	else
	{
		nDrawTextFlags &= ~(DT_SINGLELINE);
		for( nPartIndex = 0; nPartIndex < nPartCount; nPartIndex ++ )
		{
			CExtSafeString & s = strArray.ElementAt( nPartIndex );
			INT nLen = s.GetLength();
			ASSERT( nLen > 0 );
			CRect rcCalc( 0, 0, 0, 0 );
			CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(s), nLen, rcCalc, DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT, 0 );
			INT nTextHeight = rcCalc.Height();
			rcWalk.bottom = rcWalk.top + nTextHeight;
			CRect rcDraw = rcWalk;
			if(		_sizeArrow.cx > 0
				&&	_sizeArrow.cy > 0
				&&	nPartCount > 1
				&&	nPartIndex == ( nPartCount - 1 )
				)
			{
				if( ( nDrawTextFlags & DT_CENTER ) != 0 )
				{
					INT nTextWidth = rcCalc.Width();
					rcDraw.left += ( rcWalk.Width() - nTextWidth - _sizeArrow.cx - g_ptArrDist.x ) / 2;
					rcDraw.right = rcDraw.left + nTextWidth;
				} // if( ( nDrawTextFlags & DT_CENTER ) != 0 )
				ptArrow.x = rcDraw.right + g_ptArrDist.x;
				ptArrow.y = rcWalk.top + ( nTextHeight - _sizeArrow.cy ) / 2;
			}
// rcDraw.OffsetRect(
// 	0, 
// 	( rcText.bottom - rcText.top - nMeasuredTextHeight ) / 2
// 	);
			CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(s), nLen, rcDraw, nDrawTextFlags, 0 );
			rcWalk.OffsetRect( 0, nTextHeight );
		} // for( nPartIndex = 0; nPartIndex < nPartCount; nPartIndex ++ )
		if(		_sizeArrow.cx > 0
			&&	_sizeArrow.cy > 0
			)
		{
			if( nPartCount <= 1 )
			{
				ptArrow.x = rcWalk.left + ( rcWalk.Width() - _sizeArrow.cx ) / 2;
				ptArrow.y = rcWalk.top + g_ptArrDist.y;
			}
			if(		pBmpAlternativeDropDownArrow != NULL
				&&	(! pBmpAlternativeDropDownArrow->IsEmpty() )
				)
			{
				CRect rcArrow( ptArrow, _sizeArrow );
// rcArrow.OffsetRect(
// 	0, 
// 	( rcText.bottom - rcText.top - nMeasuredTextHeight ) / 2
// 	);
				pBmpAlternativeDropDownArrow->AlphaBlend( dc.m_hDC, rcArrow );
			}
			else
			{
				ptArrow.y -= 4;
				COLORREF ColorValues1[2] =
				{
					RGB(0,0,0),
					GetColor(COLOR_3DHILIGHT),
				};
				COLORREF ColorValues2[2] =
				{
					RGB(0,0,0),
					GetColor(COLOR_BTNTEXT),
				};
// ptArrow.y += ( rcText.bottom - rcText.top - nMeasuredTextHeight ) / 2;
				ptArrow.y ++;
				PaintGlyph( dc, ptArrow, *pGlyph, ColorValues1 );
				ptArrow.y --;
				PaintGlyph( dc, ptArrow, *pGlyph, ColorValues2 );
			}
		}
	}
}

bool CExtPaintManager::Ribbon_PaintQuickAccessGroupButton(
	CDC & dc,
	const RECT & rcGroupBk,
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc; rcGroupBk; pGroupTBB; lParam;
	return false;
}

void CExtPaintManager::Ribbon_PaintGroupBk(
	CDC & dc,
	const RECT & rcGroupBk,
	const CExtRibbonButtonGroup * pGroupTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pGroupTBB );
	if( pGroupTBB->ParentButtonGet() != NULL )
		return;
	if( ::IsRectEmpty( &rcGroupBk ) )
		return;
	if( ! dc.RectVisible( &rcGroupBk ) )
		return;
CRect _rcGroupBk = rcGroupBk;
	_rcGroupBk.InflateRect( 0, 0, 1, 1 );
CExtSafeString strCaptionText = pGroupTBB->GetText();
INT nCaptionTextLength = strCaptionText.GetLength();
bool bEnabled = pGroupTBB->IsEnabled();
bool bHover = ( bEnabled && pGroupTBB->IsHover() ) ? true : false;
	if( bHover && pGroupTBB->GetBar()->GetMenuTrackingButton() >= 0 )
		bHover = false;
bool bCollapsed = pGroupTBB->TopCollapsedStateGet();
	if( bCollapsed )
	{
		bool bPressed = false;
		if( bEnabled )
			bPressed = pGroupTBB->IsPressed();
		PAINTPUSHBUTTONDATA _ppbd;
		_ppbd.m_rcClient = _rcGroupBk;
		_ppbd.m_bFlat = false;
		_ppbd.m_bEnabled = bEnabled;
		_ppbd.m_bHover = bHover;
		_ppbd.m_bPushed = bPressed;
		_ppbd.m_bHorz = true;
		PaintPushButton( dc, _ppbd );
		CExtCmdIcon * pCmdIcon = Ribbon_GetGroupCollapsedIcon( pGroupTBB, lParam );
		//CExtCmdIcon * pCmdIcon = ((CExtBarButton*)pGroupTBB)->GetIconPtr();
		CRect rcIconArea = _rcGroupBk, rcTextArea = _rcGroupBk;
		if( pCmdIcon != NULL && (! pCmdIcon->IsEmpty() ) )
		{
			CSize _sizeIcon = pCmdIcon->GetSize();
			rcIconArea.left += ( rcIconArea.Width() - _sizeIcon.cx ) / 2;
			rcIconArea.top += ( rcIconArea.Height() - _sizeIcon.cy ) / 2 - 6;
			rcIconArea.right = rcIconArea.left + _sizeIcon.cx;
			rcIconArea.bottom = rcIconArea.top + _sizeIcon.cy;
			rcTextArea.top = rcIconArea.bottom + 7;
		} // if( pCmdIcon != NULL && (! pCmdIcon->IsEmpty() ) )
		else
			rcTextArea.top += 16;
		rcIconArea.OffsetRect( 0, -10 );
		if( pCmdIcon != NULL && (! pCmdIcon->IsEmpty() ) )
		{
			pCmdIcon->Paint(
				this,
				dc,
				rcIconArea.left,
				rcIconArea.top,
				-1,
				-1
				);
		} // if( pCmdIcon != NULL && (! pCmdIcon->IsEmpty() ) )
		if( nCaptionTextLength > 0 )
		{
			CExtRibbonPage * pRibbonPage =
				const_cast < CExtRibbonPage * >
				( pGroupTBB->GetRibbonPage() );
			ASSERT_VALID( pRibbonPage );
			ASSERT( pRibbonPage->GetSafeHwnd() != NULL );
			CFont * pFont = pRibbonPage->OnGetToolbarFont( false, false, const_cast < CExtRibbonButtonGroup * > ( pGroupTBB ) );
			ASSERT( pFont->GetSafeHandle() != NULL );
			//CFont * pOldFont = dc.SelectObject( pFont );
			CFont * pOldFont = dc.SelectObject( &m_FontRibbonGroupCaption );
			INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
			COLORREF clrOldTextColor = dc.SetTextColor( GetColor( COLOR_BTNTEXT ) );
			CRect rcCaption = rcTextArea;
rcCaption.OffsetRect( 0, -5 );
			Ribbon_PaintText(
				dc,
				LPCTSTR( strCaptionText ),
				rcCaption,
				DT_CENTER|DT_TOP,
				true,
				NULL,
				(CObject*)pGroupTBB,
				lParam
				);
			dc.SetTextColor( clrOldTextColor );
			dc.SetBkMode( nOldBkMode );
			dc.SelectObject( pOldFont );
		} // if( nCaptionTextLength > 0 )
	} // if( bCollapsed )
	else
	{
		const CExtRibbonPage * pRibbonPage = pGroupTBB->GetRibbonPage();
		ASSERT_VALID( pRibbonPage );
		CWnd * pWndParent = pRibbonPage->GetParent();
		bool bDrawCaption = true, bPopupMode =
				(	pWndParent != NULL
				&&	pWndParent->IsKindOf( RUNTIME_CLASS( CExtRibbonPopupMenuWnd ) )
				&&	(! ((CExtRibbonPopupMenuWnd*)pWndParent)->m_wndRibbonPage.m_bHelperAutoHideMode )
				) ? true : false;
		INT nCaptionHeight = 0;
		if( bPopupMode && (! Ribbon_IsPopupGroupWithCaption( (CObject*) pRibbonPage ) ) )
		{
			bDrawCaption = false;
			strCaptionText.Empty();
			nCaptionTextLength = 0;
		}
		else
		{
			strCaptionText.Replace( _T("\r"), _T(" ") );
			strCaptionText.Replace( _T("\n"), _T(" ") );
			strCaptionText.Replace( _T("\t"), _T(" ") );
			strCaptionText.Replace( _T("  "), _T(" ") );
			strCaptionText.TrimLeft( _T(" ") );
			strCaptionText.TrimRight( _T(" ") );
			nCaptionHeight = Ribbon_GroupCaptionGetHeight( pGroupTBB, lParam );
			ASSERT( nCaptionHeight >= 0 );
		}
		COLORREF clrBorder = RGB(0,0,0);
		if( bEnabled )
		{
			if( bHover )
			{
				clrBorder = GetColor( COLOR_3DDKSHADOW );
				dc.FillRect( &_rcGroupBk, &m_brushLighterDefault );
			} // if( bHover )
			else
				clrBorder = GetColor( COLOR_3DSHADOW );
		} // if( bEnabled )
		else
			clrBorder = GetColor( COLOR_3DSHADOW );
		if( nCaptionHeight > 0 && nCaptionTextLength > 0 )
		{
			CRect rcCaption = _rcGroupBk;
			bool bTopCaption = Ribbon_GroupCaptionIsAtTop( pGroupTBB );
			if( bTopCaption )
				rcCaption.bottom = rcCaption.top + nCaptionHeight;
			else
				rcCaption.top = rcCaption.bottom - nCaptionHeight;
			dc.FillSolidRect( &rcCaption, GetColor( COLOR_3DSHADOW ) );
			dc.Draw3dRect( &rcCaption, clrBorder, clrBorder );
			CExtRibbonPage * pRibbonPage =
				const_cast < CExtRibbonPage * >
				( pGroupTBB->GetRibbonPage() );
			ASSERT_VALID( pRibbonPage );
			ASSERT( pRibbonPage->GetSafeHwnd() != NULL );
			CFont * pFont = pRibbonPage->OnGetToolbarFont( false, false, const_cast < CExtRibbonButtonGroup * > ( pGroupTBB ) );
			ASSERT( pFont->GetSafeHandle() != NULL );

			CRect rcCaptionTextAlignmentDLB =
				pGroupTBB->GetCaptionTextAlignmentRect();
			if( ! rcCaptionTextAlignmentDLB.IsRectEmpty() )
			{
				rcCaption.left =
					max(
						rcCaption.left,
						rcCaptionTextAlignmentDLB.left
						);
				rcCaption.right =
					min(
						rcCaption.right,
						rcCaptionTextAlignmentDLB.right
						);
			} // if( ! rcCaptionTextAlignmentDLB.IsRectEmpty() )

			//CFont * pOldFont = dc.SelectObject( pFont );
			CFont * pOldFont = dc.SelectObject( &m_FontRibbonGroupCaption );
			INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
			COLORREF clrOldTextColor = dc.SetTextColor( GetColor( COLOR_3DDKSHADOW ) );
			rcCaption.OffsetRect( 1, 1 );
			UINT nRibbonGroupCaptionTextShadowDF = DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS;
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR( strCaptionText ), nCaptionTextLength,
				&rcCaption,
				nRibbonGroupCaptionTextShadowDF, 0
				);
			rcCaption.OffsetRect( -1, -1 );
			dc.SetTextColor( GetColor( COLOR_3DHIGHLIGHT ) );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR( strCaptionText ), nCaptionTextLength,
				&rcCaption,
				nRibbonGroupCaptionTextShadowDF, 0
				);
			dc.SetTextColor( clrOldTextColor );
			dc.SetBkMode( nOldBkMode );
			dc.SelectObject( pOldFont );
		} // if( nCaptionHeight > 0 && nCaptionTextLength > 0 )
//		dc.Draw3dRect( &_rcGroupBk, clrBorder, clrBorder );
	} // else from if( bCollapsed )
}

CRect CExtPaintManager::Ribbon_GetPageOuterContentPadding(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage;
	lParam;
	return CRect( 3, 3, 3, 3 );
}

CRect CExtPaintManagerNativeXP::Ribbon_GetPageOuterContentPadding(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
CRect rc =
		CExtPaintManager::Ribbon_GetPageOuterContentPadding( pRibbonPage, lParam );
e_system_theme_t eST = OnQuerySystemTheme();
	if(		eST == ThemeLunaRoyale       // +2.87
		||	eST == ThemeVistaOrLaterUX   // +2.87
		||	eST == ThemeVistaOrLaterDWM  // +2.87
		||	eST == ThemeLunaBlue
		||	eST == ThemeLunaOlive
		||	eST == ThemeLunaSilver
		)
		rc.bottom += 2;
	return rc;
}

void CExtPaintManager::Ribbon_PaintPageBk(
	CDC & dc,
	const RECT & rcPageBk,
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pRibbonPage );
	lParam;
	if( ! dc.RectVisible( &rcPageBk ) )
		return;
	if( pRibbonPage->GetSafeHwnd() != NULL )
	{
		CWnd * pWndParent = pRibbonPage->GetParent();
		if(		pWndParent != NULL
			&&	pWndParent->IsKindOf( RUNTIME_CLASS( CExtPopupMenuWnd ) )
			)
			return;
	} // if( pRibbonPage->GetSafeHwnd() != NULL )
CRect rc = rcPageBk;
	rc.DeflateRect( 2, 0 );
	stat_PaintGradientRect(
		dc,
		rc,
		GetColor( COLOR_3DHILIGHT ),
		GetColor( COLOR_3DFACE ),
		true
		);
	dc.Draw3dRect(
		&rc,
		GetColor( COLOR_3DHIGHLIGHT ),
		GetColor( COLOR_3DSHADOW )
		);
}

void CExtPaintManagerNativeXP::Ribbon_PaintPageBk(
	CDC & dc,
	const RECT & rcPageBk,
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pRibbonPage );
	lParam;
	if( ! dc.RectVisible( &rcPageBk ) )
		return;
	if( pRibbonPage->GetSafeHwnd() != NULL )
	{
		CWnd * pWndParent = pRibbonPage->GetParent();
		if(		pWndParent != NULL
			&&	pWndParent->IsKindOf( RUNTIME_CLASS( CExtPopupMenuWnd ) )
			)
			return;
	} // if( pRibbonPage->GetSafeHwnd() != NULL )
	if( g_PaintManager.m_UxTheme.IsAppThemed() )
	{
		HWND hWndOpenTheme = pRibbonPage->GetSafeHwnd();
		if( hWndOpenTheme != NULL )
			hWndOpenTheme = ::GetDesktopWindow();
		ASSERT( hWndOpenTheme != NULL );
		if(	g_PaintManager.m_UxTheme.OpenThemeData( hWndOpenTheme, VSCLASS_TAB ) != NULL )
		{
			CRect rcItem = rcPageBk;
			rcItem.DeflateRect( 2, 0 );
			INT nPartID = TABP_PANE;
			INT nStateID = 0;
			VERIFY( 
				g_PaintManager.m_UxTheme.DrawBackground(
					hWndOpenTheme,
					dc.GetSafeHdc(), 
					nPartID, 
					nStateID, 
					&rcItem, 
					&rcItem
					) == S_OK
				);
			g_PaintManager.m_UxTheme.CloseThemeData();
		}
	} // if( g_PaintManager.m_UxTheme.IsAppThemed() )
}

bool CExtPaintManager::Ribbon_HaveSeparatorsBetweenToolGroups() const
{
	ASSERT_VALID( this );
	return false;
}

void CExtPaintManager::Ribbon_PaintSeparator(
	CDC & dc,
	const RECT & rcTBB,
	const CExtBarButton * pSepTBB, // can be NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	pSepTBB;
	lParam;
CRect _rcTBB = rcTBB;
	if( ! dc.RectVisible( &_rcTBB ) )
		return;
	for( ; _rcTBB.Width() < 3; _rcTBB.InflateRect( 1, 0 ) );
	for( ; _rcTBB.Height() < 3; _rcTBB.InflateRect( 0, 1 ) );
	PaintSeparator(
		dc,
		_rcTBB,
		( _rcTBB.Width() < _rcTBB.Height() ) ? true : false,
		true,
		(CObject*)pSepTBB,
		lParam
		);
}

void CExtPaintManager::Ribbon_SyncFonts()
{
	ASSERT_VALID( this );
	ASSERT( m_FontNormal.GetSafeHandle() != NULL );
	if( m_FontRibbonNormal.GetSafeHandle() != NULL )
		m_FontRibbonNormal.DeleteObject();
	if( m_FontRibbonGroupCaption.GetSafeHandle() != NULL )
		m_FontRibbonGroupCaption.DeleteObject();
	if( m_FontRibbonTabItem.GetSafeHandle() != NULL )
		m_FontRibbonTabItem.DeleteObject();
//bool bUseDesiredRibbonFontFaceName = false;
//static __EXT_MFC_SAFE_LPCTSTR
//		g_sDesiredRibbonFontFaceName = _T("Segoe UI");
//CWindowDC dc( NULL );
//	if(	! ::EnumFontFamilies(
//			dc.m_hDC,
//			NULL,
//			(FONTENUMPROC)stat_FindFontByNameCallBack,
//			reinterpret_cast < LPARAM > ( LPCTSTR(g_sDesiredRibbonFontFaceName) )
//			)
//		)
//		bUseDesiredRibbonFontFaceName = true;
LOGFONT _lf;
	::memset( &_lf, 0, sizeof(LOGFONT) );
	m_FontNormal.GetLogFont( &_lf );
//	if( bUseDesiredRibbonFontFaceName )
//	{
//		__EXT_MFC_STRCPY( _lf.lfFaceName, LF_FACESIZE, g_sDesiredRibbonFontFaceName );
//		_lf.lfWidth = 0;
//		_lf.lfHeight = -11;
//	}
//	else
//	{
//		__EXT_MFC_STRCPY( _lf.lfFaceName, LF_FACESIZE, _T("Arial") );
//		_lf.lfWidth = 5;
//		_lf.lfHeight = 14;
//		_lf.lfWeight = 550;
//	}
	if( g_PaintManager.m_bIsWinXPorLater )
		_lf.lfQuality = CLEARTYPE_QUALITY;
	if( g_PaintManager.m_bIsWinVistaOrLater )
	{
		static __EXT_MFC_SAFE_LPCTSTR g_sDesiredRibbonFontFaceName = _T("Segoe UI");
		CWindowDC dc( NULL );
		if(	! ::EnumFontFamilies(
				dc.m_hDC,
				NULL,
				(FONTENUMPROC)stat_FindFontByNameCallBack,
				reinterpret_cast < LPARAM > ( LPCTSTR(g_sDesiredRibbonFontFaceName) )
				)
			)
		{
			__EXT_MFC_STRCPY( _lf.lfFaceName, LF_FACESIZE, g_sDesiredRibbonFontFaceName );
			_lf.lfWidth = 0;
			if(		UiScalingIsEnabled( __EUIST_Y )
				&&	g_PaintManager.m_nLPY > 96
				)
				_lf.lfHeight = -13;
			else
				_lf.lfHeight = -11;
		}
	}
	VERIFY( m_FontRibbonNormal.CreateFontIndirect( &_lf ) );
	VERIFY( m_FontRibbonGroupCaption.CreateFontIndirect( &_lf ) );
	VERIFY( m_FontRibbonTabItem.CreateFontIndirect( &_lf ) );
}

CRect CExtPaintManager::Ribbon_GetPopupGroupBorderMetrics(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage; lParam;
	return CRect( 2, 2, 2, 2 );
}

CRect CExtPaintManagerXP::Ribbon_GetPopupGroupBorderMetrics(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage; lParam;
	return CRect( 1, 1, 1, 1 );
}

bool CExtPaintManager::Ribbon_IsPopupGroupWithCaption(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	pHelperSrc; lParam;
	return false;
}

INT CExtPaintManager::RibbonQuickAccessBar_GetBottomHeight(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage; lParam;
INT nHeight = UiScalingDo( 24, __EUIST_Y );
	return nHeight;
}

INT CExtPaintManager::RibbonQuickAccessBar_GetButtonsAlignment(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage; lParam;
	return 0;
}

void CExtPaintManager::RibbonQuickAccessBar_PaintBkAtTheBottom(
	CDC & dc,
	CRect rcQATB,
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pRibbonPage );
	dc; rcQATB; pRibbonPage; lParam;
}

void CExtPaintManagerNativeXP::RibbonQuickAccessBar_PaintBkAtTheBottom(
	CDC & dc,
	CRect rcQATB,
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage;
	lParam;
	if( g_PaintManager.m_UxTheme.OpenThemeData( pRibbonPage->GetSafeHwnd(), VSCLASS_REBAR ) != NULL )
	{
		CRect rcPaint = rcQATB;
		VERIFY( 
			g_PaintManager.m_UxTheme.DrawBackground(
				pRibbonPage->GetSafeHwnd(),
				dc.GetSafeHdc(), 
				0, 
				0, 
				&rcPaint, 
				&rcPaint
				) == S_OK
			);
		g_PaintManager.m_UxTheme.CloseThemeData();
	}
}

void CExtPaintManager::RibbonQuickAccessBar_AdjustButtonLocation(
	CExtBarButton * pTBB,
	CRect & rcTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB; rcTBB; lParam;
}

INT CExtPaintManager::RibbonQuickAccessBar_GetAdditionalSpaceAtRight(
	bool bDwmMode,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	bDwmMode; pHelperSrc; lParam;
	return 0;
}	

void CExtPaintManager::Ribbon_PaintTabItem(
	CDC & dc,
	CRect & rcTabItemsArea,
	bool bSelected,
	const CRect & rcEntireItem,
	CFont * pFont,
	__EXT_MFC_SAFE_LPCTSTR sText,
	CExtCmdIcon * pIcon,
	CObject * pHelperSrc,
	LPARAM lParam, // = 0L
	COLORREF clrForceText, // = COLORREF(-1L)
	COLORREF clrForceTabBk, // = COLORREF(-1L)
	COLORREF clrForceTabBorderLT, // = COLORREF(-1L)
	COLORREF clrForceTabBorderRB, // = COLORREF(-1L)
	COLORREF clrForceTabSeparator // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pFont->GetSafeHandle() != NULL );
	if( ! dc.RectVisible( &rcEntireItem ) )
		return;
	if( clrForceText == COLORREF(-1L) )
		clrForceText = GetColor( COLOR_BTNTEXT );
	if( clrForceTabSeparator == COLORREF(-1L) )
		clrForceTabSeparator = GetColor( COLOR_BTNTEXT );
CRect _rcEntireItem = rcEntireItem;
	if( bSelected )
		_rcEntireItem.DeflateRect( 0, 2, 0, 0 );
// 	else if(
// 			pHelperSrc != NULL
// 		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtBarButton ) )
// 		&&	((CExtBarButton*)pHelperSrc)->IsHover()
// 		)
// 	{
// 		bSelected = true;
// 		_rcEntireItem.DeflateRect( 2, 1, 3, 1 );
// 		clrForceText = GetColor( COLOR_3DHIGHLIGHT );
// 		clrForceTabBk = clrForceTabBorderLT = clrForceTabBorderRB =
// 			GetColor( COLOR_3DDKSHADOW );
// 	}
	else
		_rcEntireItem.DeflateRect( 0, 2, 0, 2 );
CSize sizeTextMeasured = stat_CalcTextDimension( dc, *pFont, CExtSafeString(sText), DT_SINGLELINE|DT_LEFT|DT_TOP ).Size();
	PaintTabItem(
		dc,
		rcTabItemsArea,
		true,
		true,
		bSelected,
		true,
		false,
		false,
		false,
		_rcEntireItem,
		sizeTextMeasured,
		pFont,
		sText,
		pIcon,
		pHelperSrc,
		lParam,
		clrForceText,
		clrForceTabBk,
		clrForceTabBorderLT,
		clrForceTabBorderRB,
		clrForceTabSeparator
		);
}

void CExtPaintManagerNativeXP::Ribbon_PaintTabItem(
	CDC & dc,
	CRect & rcTabItemsArea,
	bool bSelected,
	const CRect & rcEntireItem,
	CFont * pFont,
	__EXT_MFC_SAFE_LPCTSTR sText,
	CExtCmdIcon * pIcon,
	CObject * pHelperSrc,
	LPARAM lParam, // = 0L
	COLORREF clrForceText, // = COLORREF(-1L)
	COLORREF clrForceTabBk, // = COLORREF(-1L)
	COLORREF clrForceTabBorderLT, // = COLORREF(-1L)
	COLORREF clrForceTabBorderRB, // = COLORREF(-1L)
	COLORREF clrForceTabSeparator // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pFont->GetSafeHandle() != NULL );
	if( ! dc.RectVisible( &rcEntireItem ) )
		return;
	if( clrForceText == COLORREF(-1L) )
		clrForceText = GetColor( COLOR_BTNTEXT );
	if( clrForceTabSeparator == COLORREF(-1L) )
		clrForceTabSeparator = GetColor( COLOR_BTNTEXT );
CRect _rcEntireItem = rcEntireItem;
	_rcEntireItem.DeflateRect( 0, 2, 0, 2 );
CSize sizeTextMeasured = stat_CalcTextDimension( dc, *pFont, CExtSafeString(sText), DT_SINGLELINE|DT_LEFT|DT_TOP ).Size();
	PaintTabItem(
		dc,
		rcTabItemsArea,
		true,
		true,
		bSelected,
		true,
		false,
		false,
		false,
		_rcEntireItem,
		sizeTextMeasured,
		pFont,
		sText,
		pIcon,
		pHelperSrc,
		lParam,
		clrForceText,
		clrForceTabBk,
		clrForceTabBorderLT,
		clrForceTabBorderRB,
		clrForceTabSeparator
		);
}

void CExtPaintManager::Ribbon_PaintTabClientArea(
	CDC & dc,
	CRect & rcClient,
	CRect & rcTabItemsArea,
	CRect & rcTabNearBorderArea,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcClient;
	rcTabItemsArea;
	rcTabNearBorderArea;
	pHelperSrc;
	lParam;
}

INT CExtPaintManager::Ribbon_GetTabIntersectionHeight(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage;
	lParam;
	return 0;
}

INT CExtPaintManagerNativeXP::Ribbon_GetTabIntersectionHeight(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage;
	lParam;
	return 0;
}

INT CExtPaintManager::Ribbon_GetTabLineHeight(
	const CExtRibbonPage * pRibbonPage,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonPage );
	pRibbonPage;
	lParam;
CFont * pMeasureFont = (CFont*)&m_FontRibbonNormal;
	ASSERT( pMeasureFont->GetSafeHandle() != NULL );
CWindowDC dc( NULL );
CFont * pOldFont = dc.SelectObject( pMeasureFont );
TEXTMETRIC _TxtM;
	::memset( &_TxtM, 0, sizeof(TEXTMETRIC) );
	if( ! dc.GetTextMetrics( &_TxtM ) )
		_TxtM.tmHeight = 17;
	dc.SelectObject( pOldFont );
// 			INT nBasicHeight = _TxtM.tmHeight + 10;
// 			INT nTabLineHeight = UiScalingDo( nBasicHeight, __EUIST_Y );
INT nTabLineHeight = _TxtM.tmHeight + UiScalingDo( 10, __EUIST_Y );
	return nTabLineHeight;
}

HRGN CExtPaintManager::Ribbon_GetRgnCornerExclude(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return ((HRGN)NULL);
}

HRGN CExtPaintManager::Ribbon_GetRgnCornerResizing(
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	return ((HRGN)NULL);
}

void CExtPaintManager::Ribbon_EmbeddedCaptionPaintText(
	CDC & dc,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	CRect rcDrawText,
	__EXT_MFC_SAFE_LPCTSTR strCaptionDelimiter,
	const CExtRibbonBar * pRibbonBar,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonBar );
	dc;
	pRibbonBar;
	strCaption;
	rcDrawText;
	strCaptionDelimiter;
	pRibbonBar;
	lParam;
}

UINT CExtPaintManager::Ribbon_EmbeddedCaptionGetTextAlignmentFlags(
	const CExtRibbonBar * pRibbonBar,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonBar );
	pRibbonBar;
	lParam;
	return (DT_SINGLELINE|DT_CENTER|DT_VCENTER);
}

void CExtPaintManager::Ribbon_EmbeddedCaptionAdjustTextRect(
	CRect & rcAdjust,
	const CExtRibbonBar * pRibbonBar,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonBar );
	rcAdjust;
	pRibbonBar;
	lParam;
}

void CExtPaintManager::Ribbon_NcOverPaint(
	CDC & dc,
	bool bFrameActive,
	INT nWidthFromLeft,
	bool bDwmMode,
	LPCRECT pRectDwmText,
	__EXT_MFC_SAFE_LPCTSTR strDwmCaptionTitle,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc; bFrameActive; nWidthFromLeft; bDwmMode; pRectDwmText; strDwmCaptionTitle; pHelperSrc; lParam;
}

bool CExtPaintManager::Ribbon_QuickAccessContentExpandButtonCalcLayout(
	CDC & dc,
	CExtRibbonButtonQuickAccessContentExpand * pQACEB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pQACEB );
	dc;
	lParam;
	pQACEB->Size( CSize(13,22) );
	return true;
}

bool CExtPaintManager::Ribbon_QuickAccessContentExpandButtonPaint(
	CDC & dc,
	CExtRibbonButtonQuickAccessContentExpand * pQACEB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pQACEB );
CRect rcTBB = *pQACEB;
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		pQACEB, true, rcTBB, _T(""), NULL, true, pQACEB->IsHover(), pQACEB->IsPressed(), false, pQACEB->IsEnabled(), true, false, false,
		__ALIGN_HORIZ_CENTER|__ALIGN_VERT_CENTER, NULL, true, 0, true
		);
	_ppbd.m_lParam = lParam;
	PaintPushButton( dc, _ppbd );
	return true;
}

CSize CExtPaintManager::Ribbon_QuickAccessContentExpandGetShift(
	CDC & dc,
	CExtRibbonButtonQuickAccessContentExpand * pQACEB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pQACEB );
	dc; pQACEB; lParam;
	return CSize( 0, 0 );
}

bool CExtPaintManager::Ribbon_DLB_IsVisible(
	CExtRibbonButtonDialogLauncher * pTBB,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	lParam;
CExtToolControlBar * pBar = pTBB->GetSafeBar();
	if( pBar == NULL )
		return false;
CExtRibbonPage * pRibbonPage =
		DYNAMIC_DOWNCAST( CExtRibbonPage, pBar );
	if( pRibbonPage == NULL )
		return false;
	if(		pRibbonPage->m_bHelperPopupMode
		&&	( ! pRibbonPage->m_bHelperAutoHideMode )
		)
		return false;
	return true;
}

CSize CExtPaintManager::Ribbon_DLB_CalcSize(
	CDC & dc,
	CExtRibbonButtonDialogLauncher * pTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pTBB );
	dc; pTBB; lParam;
CSize _size( 17, 15 );
	_size.cx = UiScalingDo( _size.cx, __EUIST_X );
	_size.cy = UiScalingDo( _size.cy, __EUIST_Y );
	return _size;
}

void CExtPaintManager::Ribbon_DLB_Paint(
	CDC & dc,
	CExtRibbonButtonDialogLauncher * pTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pTBB );
	lParam;
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		pTBB, true, *pTBB, _T("..."), NULL, false, pTBB->IsHover(), pTBB->IsPressed(), false, pTBB->IsEnabled(), true, false, false,
		__ALIGN_HORIZ_CENTER|__ALIGN_VERT_CENTER, NULL, false, 0, false
		);
	PaintPushButton( dc, _ppbd );
}

void CExtPaintManager::Ribbon_FileMenuButtonQuery(
	LPVOID pData,
	CExtCmdIcon * pIcon, // = NULL
	__EXT_MFC_SAFE_LPCTSTR sText, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( pData != NULL );
CExtPopupMenuWnd::RIBBONFILEMENUBUTTONQUERY & _rfmbq = *((CExtPopupMenuWnd::RIBBONFILEMENUBUTTONQUERY*)pData);
	_rfmbq.m_bQueryHandled = true;
	if( _rfmbq.m_pDC != NULL )
	{
		ASSERT( _rfmbq.m_pDC->GetSafeHdc() != NULL );
		PAINTPUSHBUTTONDATA _ppbd(
			_rfmbq.m_pPopup, true, _rfmbq.m_rcButton, sText, pIcon,
			false, _rfmbq.m_bHover, _rfmbq.m_bPressed, false, true, true, false, false,
			__ALIGN_HORIZ_CENTER|__ALIGN_VERT_CENTER, NULL, false, 0, false
			);
		_ppbd.m_lParam = lParam;
		PaintPushButton( *_rfmbq.m_pDC, _ppbd );
	} // if( _rfmbq.m_pDC != NULL )
	else
	{
		CSize _sizeIcon( 0, 0 ), _sizeText( 0, 0 );
		if(		pIcon != NULL
			&&	( ! pIcon->IsEmpty() )
			)
			_sizeIcon = pIcon->GetSize();
		if(		sText != NULL
			&&	INT( _tcslen( sText ) ) > 0
			)
		{
			CWindowDC dc( NULL );
			CExtSafeString s = sText;
			_sizeText = stat_CalcTextDimension( dc, m_FontNormal, s, DT_SINGLELINE|DT_LEFT|DT_TOP ).Size();
		}
		_rfmbq.m_nMeasuredWidth = _sizeIcon.cx + _sizeText.cx + 10;
		if( _sizeIcon.cx > 0 )
			_rfmbq.m_nMeasuredWidth += 5;
	} // else from if( _rfmbq.m_pDC != NULL )
}

bool CExtPaintManager::Ribbon_FileButtonIsItegrationSupported() const
{
	ASSERT_VALID( this );
	return false;
}

INT CExtPaintManager::Ribbon_FileButtonGetMenuIntersectionHeight() const
{
	ASSERT_VALID( this );
	return 0;
}

bool CExtPaintManager::Ribbon_FileButtonCalcLayout(
	CDC & dc,
	CExtNcFrameImpl * pExtNcFrameImpl,
	CExtRibbonButtonFile * pFileTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pExtNcFrameImpl != NULL );
	ASSERT_VALID( pFileTBB );
	dc; pExtNcFrameImpl; pFileTBB; lParam;
	return false;
}

bool CExtPaintManager::Ribbon_FileButtonPaint(
	CDC & dc,
	CExtRibbonButtonFile * pFileTBB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pFileTBB );
	dc; pFileTBB; lParam;
	return false;
}

INT CExtPaintManager::RibbonLayout_CalcQatbAboveRibbonHorzStart(
	CExtRibbonBar * pRibbonBar,
	LPARAM lParam //= 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRibbonBar );
	pRibbonBar;
	lParam;
	return -1;
}

bool CExtPaintManager::Ribbon_IsSystemMenuIconPresent() const
{
	ASSERT_VALID( this );
	return false;
}

CFont * CExtPaintManager::RibbonGallery_GetItemFont(
	CExtRibbonGalleryWnd * pGalleryWnd,
	LPVOID pGalleryItemData,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pGalleryWnd );
	pGalleryWnd; pGalleryItemData; lParam;
	return NULL;
}

CSize CExtPaintManager::RibbonGallery_MeasureItem(
	CDC & dc,
	CExtRibbonGalleryWnd * pGalleryWnd,
	LPVOID pGalleryItemData,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pGalleryWnd );
	dc; pGalleryWnd; pGalleryItemData; lParam;
	return CSize( -1, -1 );
}

bool CExtPaintManager::RibbonGallery_DrawItem(
	CDC & dc,
	CExtRibbonGalleryWnd * pGalleryWnd,
	const RECT & rcItem,
	LPVOID pGalleryItemData,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pGalleryWnd );
	dc; pGalleryWnd; rcItem; pGalleryItemData; lParam;
	return false;
}

bool CExtPaintManager::RibbonGallery_EraseEntireBk(
	CDC & dc,
	const CRect & rcClient,
	const CRect & rcActiveChildArea,
	CRgn & rgnActiveChildArea,
	const CRect & rcActiveChildArea2,
	CRgn & rgnActiveChildArea2,
	CExtRibbonGalleryWnd * pGalleryWnd,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pGalleryWnd );
	rcActiveChildArea; rgnActiveChildArea; rcActiveChildArea2; rgnActiveChildArea2; pGalleryWnd; lParam;
	rcClient;
CRect _rcClient;
	pGalleryWnd->GetClientRect( &_rcClient );
COLORREF clrBkgnd = GetColor( COLOR_WINDOW );
	if(		pGalleryWnd->m_bInPlaceGallery
		&&	pGalleryWnd->_IsKeyFocusGallery()
		)
		clrBkgnd = GetColor( COLOR_HIGHLIGHT );
	dc.FillSolidRect( &_rcClient, clrBkgnd );
	return true;
}

void CExtPaintManager::RibbonGallery_AdjustClientRect(
	CRect & rcClient,
	const CExtRibbonGalleryWnd * pGalleryWnd,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pGalleryWnd );
	lParam;
	if(		pGalleryWnd->m_bInPlaceGallery
		&&	pGalleryWnd->GetScrollBarCtrl(SB_VERT)->GetSafeHwnd() != NULL
		)
		rcClient.DeflateRect( 1, 1, 0, 1 );
}

void CExtPaintManager::RibbonGallery_PaintScrollArea(
	CDC & dc,
	CExtRibbonGalleryInplaceScrollBar * pSB,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pSB );
	ASSERT( pSB->GetSafeHwnd() != NULL );
bool bKeyFocusMode = false;
CExtRibbonGalleryWnd * pRibbonGalleryWnd =
		STATIC_DOWNCAST( CExtRibbonGalleryWnd, pSB->GetParent() );
	if(		pRibbonGalleryWnd->m_bInPlaceGallery
		&&	pRibbonGalleryWnd->_IsKeyFocusGallery()
		)
		bKeyFocusMode = true;
INT nIndex, nCount = sizeof(pSB->m_arrButtons) / sizeof(pSB->m_arrButtons[0]);
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CRect rc = pSB->RgBtnGetRect( (CExtRibbonGalleryInplaceScrollBar::e_ButtonType_t)nIndex );
		LPCTSTR strText = NULL;
		switch( nIndex )
		{
		case INT(CExtRibbonGalleryInplaceScrollBar::__BTT_UP):
			strText = _T("t");
		break;
		case INT(CExtRibbonGalleryInplaceScrollBar::__BTT_DOWN):
			strText = _T("u");
		break;
		case INT(CExtRibbonGalleryInplaceScrollBar::__BTT_MENU):
			strText = _T("7");
		break;
		} // switch( nIndex )
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
			pSB,
			true,
			rc,
			strText, //bTopButton ? _T("t") : _T("u"), // up arrow / down arrow
			NULL,
			true,
			( pSB->m_arrButtons[ nIndex ] == CExtRibbonGalleryInplaceScrollBar::__BST_HOVER || ( bKeyFocusMode && nIndex == INT(CExtRibbonGalleryInplaceScrollBar::__BTT_MENU) ) ) ? true : false,
			( pSB->m_arrButtons[ nIndex ] == CExtRibbonGalleryInplaceScrollBar::__BST_PRESSED ) ? true : false,
			false,
			( pSB->m_arrButtons[ nIndex ] != CExtRibbonGalleryInplaceScrollBar::__BST_DISABLED ) ? true : false,
			true,false,false,
			__ALIGN_HORIZ_CENTER | __ALIGN_VERT_CENTER,
			(HFONT)m_FontMarlett.GetSafeHandle()
			);
		_ppbd.m_lParam = lParam;
		PaintPushButton( dc, _ppbd );
	}
}

#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

CExtAnimationParameters CExtPaintManager::g_DefAnimationParametersEmpty( 0, 0 );
CExtAnimationParameters CExtPaintManager::g_DefAnimationParametersNormal( __EXT_MFC_DEF_ANIMATION_STEP_COUNT, __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE );
CExtAnimationParameters CExtPaintManager::g_DefAnimationParametersSlow( __EXT_MFC_DEF_ANIMATION_STEP_COUNT+1, __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE );
CExtAnimationParameters CExtPaintManager::g_DefAnimationParametersVerySlow( __EXT_MFC_DEF_ANIMATION_STEP_COUNT+2, __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE );
//CExtAnimationParameters CExtPaintManager::g_DefAnimationParametersVerySlowAndSmooth( __EXT_MFC_DEF_ANIMATION_STEP_COUNT+4, __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE );
CExtAnimationParameters CExtPaintManager::g_DefAnimationParametersVerySlowAndSmooth( 4, 80 );

const CExtAnimationParameters *
	CExtPaintManager::Animation_GetParameters(
		INT eAPT, // __EAPT_*** animation type
		CObject * pHelperSrc,
		const CExtAnimationClient * pAC,
		LPARAM lParam // = 0L
		) const
{
	ASSERT_VALID( this );
	ASSERT( pAC != NULL );
	eAPT;
	pHelperSrc;
	pAC;
	lParam;
//					if( ! g_PaintManager.m_bIsWin2000orLater )
//						return (&g_PaintManager->g_DefAnimationParametersEmpty);
//					if( pHelperSrc != NULL )
//					{
//						ASSERT_VALID( pHelperSrc );
//						if( g_PaintManager.m_DWM.IsCompositionEnabled() )
//						{
//							if( pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtScrollBar ) ) )
//								return (&g_PaintManager->g_DefAnimationParametersVerySlow);
//						} // if( g_PaintManager.m_DWM.IsCompositionEnabled() )
//				#if (!defined __EXT_MFC_NO_RIBBON_BAR)
//						CExtBarButton * pTBB = DYNAMIC_DOWNCAST( CExtBarButton, pHelperSrc );
//						if( pTBB != NULL )
//						{
//							CExtToolControlBar * pToolBar = pTBB->GetBar();
//							if( pToolBar != NULL )
//							{
//								ASSERT_VALID( pToolBar );
//								CExtRibbonPage * pRibbonPage = DYNAMIC_DOWNCAST( CExtRibbonPage, pToolBar );
//								if( pRibbonPage != NULL )
//								{
//									if( pTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonDialogLauncher ) ) )
//									{
//										if(		eAPT == __EAPT_BY_HOVERED_STATE_TURNED_ON
//											||	eAPT == __EAPT_BY_HOVERED_STATE_TURNED_OFF
//											)
//											return (&g_DefAnimationParametersEmpty);
//									}
//									CExtBarButton * pParentTBB = pTBB->ParentButtonGet();
//									CExtRibbonButtonGroup * pGroupTBB = DYNAMIC_DOWNCAST( CExtRibbonButtonGroup, pTBB );
//									if( pGroupTBB != NULL )
//									{
//										if( pParentTBB != NULL )
//											return (&g_DefAnimationParametersEmpty);
//				//						if( pGroupTBB->TopCollapsedStateGet() )
//											return (&g_DefAnimationParametersNormal);
//				//						return (&g_DefAnimationParametersVerySlowAndSmooth);
//									}
//									if( pTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonTabPage ) ) )
//									{
//										if( eAPT == __EAPT_BY_HOVERED_STATE_TURNED_ON )
//											return (&g_DefAnimationParametersEmpty);
//										return (&g_DefAnimationParametersVerySlow);
//									}
//									if(		(	pParentTBB != NULL
//											&&	pParentTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonToolGroup ) )
//											)
//										||	pTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonFile ) )
//										||	pTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonQuickAccessContentExpand ) )
//										)
//										return (&g_DefAnimationParametersNormal);
//									// final case
//									if( eAPT == __EAPT_BY_HOVERED_STATE_TURNED_ON )
//										return (&g_DefAnimationParametersEmpty);
//									return (&g_DefAnimationParametersNormal);
//								} // if( pRibbonPage != NULL )
//							} // if( pToolBar != NULL )
//						} // if( pTBB != NULL )
//				#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
//					} // if( pHelperSrc != NULL )
	return (&g_DefAnimationParametersEmpty);
}

const CExtAnimationParameters *
	CExtPaintManagerNativeXP::Animation_GetParameters(
		INT eAPT, // __EAPT_*** animation type
		CObject * pHelperSrc,
		const CExtAnimationClient * pAC,
		LPARAM lParam // = 0L
		) const
{
	ASSERT_VALID( this );
	ASSERT( pAC != NULL );
	if( ! g_PaintManager.m_bIsWin2000orLater )
		return (&g_PaintManager->g_DefAnimationParametersEmpty);
	if( pHelperSrc != NULL )
	{
		ASSERT_VALID( pHelperSrc );
		if( g_PaintManager.m_DWM.IsCompositionEnabled() )
		{
			if( pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtButton ) ) )
				return (&g_PaintManager->g_DefAnimationParametersVerySlow);
#if (!defined __EXT_MFC_NO_SPIN)
			if( pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtSpinWnd ) ) )
				return (&g_PaintManager->g_DefAnimationParametersVerySlow);
#endif // (!defined __EXT_MFC_NO_SPIN)
		} // if( g_PaintManager.m_DWM.IsCompositionEnabled() )
	} // if( pHelperSrc != NULL )
const CExtAnimationParameters * pAnimationParameters =
		CExtPaintManager::Animation_GetParameters(
			eAPT,
			pHelperSrc,
			pAC,
			lParam
			);
	return pAnimationParameters;
}

void CExtPaintManager::MenuBottomResizingMarginGetInfo(
	INT & nMarginHeight,
	INT & nResizingCornerPartWidth,
	CExtPopupMenuWnd * pPopup,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pPopup );
	lParam;
	nMarginHeight = 0;
	nResizingCornerPartWidth = 0;
DWORD dwTrackFlags = pPopup->TrackFlagsGet();
	if( (dwTrackFlags&TPMX_RIBBON_RESIZING) != 0 )
	{
		nMarginHeight = 10;
		if( (dwTrackFlags&TPMX_RIBBON_RESIZING_VERTICAL_ONLY) == 0 )
			nResizingCornerPartWidth = 10;
	}
}

void CExtPaintManager::MenuBottomResizingMarginPaint(
	CDC & dc,
	const RECT & rcResizingArea,
	CExtPopupMenuWnd * pPopup,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pPopup );
	lParam;
	if( ! dc.RectVisible( &rcResizingArea ) )
		return;
DWORD dwTrackFlags = pPopup->TrackFlagsGet();
	if( (dwTrackFlags&TPMX_RIBBON_RESIZING) == 0 )
		return;
	if( (dwTrackFlags&TPMX_RIBBON_RESIZING_VERTICAL_ONLY) != 0 )
	{
		PaintMenuTearOffButton( dc, rcResizingArea, false, false, pPopup, lParam );
		return;
	}
INT nMarginHeight = 0, nResizingCornerPartWidth = 0;
	MenuBottomResizingMarginGetInfo( nMarginHeight, nResizingCornerPartWidth, pPopup, lParam );
CRect rcGrip = rcResizingArea;
	rcGrip.left = rcGrip.right - nResizingCornerPartWidth;
	if( ! dc.RectVisible( &rcGrip ) )
		return;
	PaintResizingGripper( dc, rcGrip, pPopup, lParam );
}

CFont * CExtPaintManager::MenuCaptionGetFont(
	CExtPopupMenuWnd * pPopup,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pPopup );
	pPopup;
	lParam;
	return ( const_cast < CFont * > ( & m_FontNormal ) );
}

CSize CExtPaintManager::MenuCaption_Measure(
	CDC & dc,
	__EXT_MFC_SAFE_LPCTSTR sMenuCaptionText,
	CExtPopupMenuWnd * pPopup,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pPopup );
	if( sMenuCaptionText == NULL )
		return CSize( 0, 0 );
CExtSafeString _sText = sMenuCaptionText;
	if( _sText.GetLength() == 0 )
		return CSize( 0, 0 );
CFont * pFont = MenuCaptionGetFont(	pPopup, lParam );
	ASSERT( pFont->GetSafeHandle() != NULL );
CSize _size =
		stat_CalcTextDimension(
			dc,
			*pFont,
			_sText,
			DT_SINGLELINE|DT_TOP|DT_LEFT
			).Size();
	_size.cx += 20;
	_size.cy += 8;
	return _size;
}

void CExtPaintManager::MenuCaption_Paint(
	CDC & dc,
	const RECT & rcMenuCaption,
	__EXT_MFC_SAFE_LPCTSTR sMenuCaptionText,
	CExtPopupMenuWnd * pPopup,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		( ! dc.RectVisible( &rcMenuCaption ) )
		//||	sMenuCaptionText == NULL
		)
		return;
INT nTextLen = ( sMenuCaptionText != NULL ) ? INT( _tcslen( sMenuCaptionText ) ) : 0;
//	if( nTextLen == 0 )
//		return;
CRect _rcMenuCaption = rcMenuCaption;
	if(		_rcMenuCaption.IsRectEmpty()
		||	( ! dc.RectVisible( &_rcMenuCaption ) )
		)
		return;
	_rcMenuCaption.DeflateRect( 2, 2, 2, 0 );
	dc.FillSolidRect( &_rcMenuCaption, GetColor( COLOR_3DSHADOW ) );
	_rcMenuCaption.DeflateRect( 8, 0 );
	if( nTextLen )
	{
		bool bRTL = (pPopup != NULL) ? pPopup->OnQueryLayoutRTL() : false;
		UINT nDT = DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_NOCLIP
				| ( bRTL ? DT_RIGHT : DT_LEFT );
		CFont * pFont = MenuCaptionGetFont(	pPopup, lParam );
			ASSERT( pFont->GetSafeHandle() != NULL );
		CFont * pOldFont = dc.SelectObject( pFont );
		COLORREF clrOldText = dc.SetTextColor( GetColor( COLOR_3DHIGHLIGHT ) );
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR( sMenuCaptionText ), nTextLen,
			&_rcMenuCaption,
			nDT, 0
			);
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldText );
		dc.SelectObject( pOldFont );
	} // if( nTextLen )
}

bool CExtPaintManager::PaintProgress( CDC & dc, CExtPaintManager::PAINTPROGRESSDATA & _ppd )
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc; _ppd;
	return false;
}

bool CExtPaintManagerNativeXP::PaintProgress( CDC & dc, CExtPaintManager::PAINTPROGRESSDATA & _ppd )
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return CExtPaintManager::PaintProgress( dc, _ppd );
	if( g_PaintManager.m_UxTheme.OpenThemeData( ::GetDesktopWindow(), L"PROGRESS" ) == NULL )
		return false;
CRect rcProgress( _ppd.m_rcClient );
	if( g_PaintManager.m_UxTheme.DrawThemeBackground(
			dc.GetSafeHdc(), 
			_ppd.m_bHorz ? PP_BAR : PP_BARVERT, 
			0, 
			&rcProgress, 
			&rcProgress
			) == S_OK
		)
	{
		if( _ppd.m_bEndlessMode )
		{
			
			ASSERT( _ppd.m_nEndlessNominator > 0 );
			ASSERT( _ppd.m_nEndlessDenominator > 0 );
			ASSERT( _ppd.m_nEndlessNominator < _ppd.m_nEndlessDenominator );
			if( _ppd.m_bHorz )
			{
				LONG nBasic = rcProgress.Width();
				LONG nCross = rcProgress.Height();
				if( nCross <= 0 )
					nCross = 1;
				LONG nAll = nBasic - nBasic % nCross;
				LONG nPart = ::MulDiv( nAll, _ppd.m_nEndlessNominator, _ppd.m_nEndlessDenominator );
				LONG nExt = nAll - nPart;
				rcProgress.left += ::MulDiv( nExt, _ppd.m_nPos, _ppd.m_nRange );
				rcProgress.right = rcProgress.left + nPart;
			}
			else
			{
				LONG nBasic = rcProgress.Height();
				LONG nCross = rcProgress.Width();
				if( nCross <= 0 )
					nCross = 1;
				LONG nAll = nBasic - nBasic % nCross;
				LONG nPart = ::MulDiv( nAll, _ppd.m_nEndlessNominator, _ppd.m_nEndlessDenominator );
				LONG nExt = nAll - nPart;
				rcProgress.bottom -= ::MulDiv( nExt, _ppd.m_nPos, _ppd.m_nRange );
				rcProgress.top = rcProgress.bottom - nPart;
			}
		} // if( _ppd.m_bEndlessMode )
		else
		{
			if( _ppd.m_bHorz )
				rcProgress.right =
					  rcProgress.left
					+ ::MulDiv( rcProgress.Width(), _ppd.m_nPos, _ppd.m_nRange );
			else
				rcProgress.top =
					  rcProgress.bottom
					- ::MulDiv( rcProgress.Height(), _ppd.m_nPos, _ppd.m_nRange );
		} // else from if( _ppd.m_bEndlessMode )
		INT nChunkSize = 0; // The size of the progress control "chunk" shapes that define how far an operation has progressed
		INT nSpaceSize = 0; // The total size of all of the progress control "chunks"
		if(		g_PaintManager.m_UxTheme.GetThemeMetric(
					dc.GetSafeHdc(), 
					_ppd.m_bHorz ? PP_CHUNK : PP_CHUNKVERT,
					0, 
					TMT_PROGRESSCHUNKSIZE, 
					&nChunkSize
					) == S_OK
			&&	g_PaintManager.m_UxTheme.GetThemeMetric(
					dc.GetSafeHdc(), 
					_ppd.m_bHorz ? PP_CHUNK : PP_CHUNKVERT,
					0, 
					TMT_PROGRESSSPACESIZE, 
					&nSpaceSize
					) == S_OK
			)
		{
			rcProgress.DeflateRect( 2, 2 );
			CRect rcChunk( rcProgress );
			INT nChunkShift = nChunkSize + nSpaceSize;
			rcChunk.SetRect(
				_ppd.m_bHorz ? rcChunk.left : rcChunk.left,
				_ppd.m_bHorz ? rcChunk.top  : (rcChunk.bottom - nChunkSize ),
				_ppd.m_bHorz ? ( rcChunk.left + nChunkSize ) : rcChunk.right,
				_ppd.m_bHorz ? rcChunk.bottom : rcChunk.bottom
				);
			ASSERT( 
					( (!_ppd.m_bHorz) && rcChunk.Height() == nChunkSize )
				||	( _ppd.m_bHorz    && rcChunk.Width() == nChunkSize )
				);
			while( true )
			{
				CRect rcProgressFullClip = rcProgress;
				rcProgressFullClip.DeflateRect( 2, 2 );
				CRect rcChunkClip;
				rcChunkClip.IntersectRect( &rcProgressFullClip, rcChunk );
				if( g_PaintManager.m_UxTheme.DrawThemeBackground(
						dc.GetSafeHdc(), 
						_ppd.m_bHorz ? PP_CHUNK : PP_CHUNKVERT,
						0, 
						&rcChunk, 
						&rcChunkClip
						) != S_OK
					)
					break;
				if( _ppd.m_bHorz )
				{
					if( rcChunk.right > rcProgress.right )
						break;
				}
				else
				{
					if( rcChunk.top < rcProgress.top )
						break;
				}
				rcChunk.OffsetRect( 
					_ppd.m_bHorz ? nChunkShift : 0, 
					_ppd.m_bHorz ? 0 : ( - nChunkShift )
					);
			}
		}
	}
	g_PaintManager.m_UxTheme.CloseThemeData( true );
	return true;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CFont & CExtPaintManager::ScreenTip_GetFont(
	bool bCaption,
	bool bMainPart,
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( &_CCST );
	bMainPart;
	_CCST;
	pHelperSrc;
	lParam;
	return bCaption ? m_FontBold : m_FontNormal;
}

CRect CExtPaintManager::ScreenTip_GetPaddingOuter(
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( &_CCST );
	pHelperSrc;
	lParam;
bool bMainTextMode = false;
CExtSafeString strCaptionMain = _CCST.CaptionMainGet();
CExtSafeString strTextMain = _CCST.TextMainGet();
CExtSafeString strCaptionSecondary = _CCST.CaptionSecondaryGet();
CExtSafeString strTextSecondary = _CCST.TextSecondaryGet();
	if(		strCaptionMain.IsEmpty()
		&&	(! strTextMain.IsEmpty() )
		&&	strCaptionSecondary.IsEmpty()
		&&	strTextSecondary.IsEmpty()
		)
		bMainTextMode = true;
INT n = bMainTextMode ? 0 : 5;
CRect rcPadding( n, n, n, n );
	rcPadding.left = UiScalingDo( rcPadding.left, __EUIST_X );
	rcPadding.top = UiScalingDo( rcPadding.top, __EUIST_Y );
	rcPadding.right = UiScalingDo( rcPadding.right, __EUIST_X );
	rcPadding.bottom = UiScalingDo( rcPadding.bottom, __EUIST_Y );
	return rcPadding;
}

CRect CExtPaintManager::ScreenTip_GetPaddingInnerBmp(
	bool bMainPart,
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( &_CCST );
	bMainPart;
	_CCST;
	pHelperSrc;
	lParam;
CRect rcPadding( 2, 2, 2, 2 );
	rcPadding.left = UiScalingDo( rcPadding.left, __EUIST_X );
	rcPadding.top = UiScalingDo( rcPadding.top, __EUIST_Y );
	rcPadding.right = UiScalingDo( rcPadding.right, __EUIST_X );
	rcPadding.bottom = UiScalingDo( rcPadding.bottom, __EUIST_Y );
	return rcPadding;
}

CRect CExtPaintManager::ScreenTip_GetPaddingInnerText(
	bool bCaption,
	bool bMainPart,
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( &_CCST );
	bCaption;
	bMainPart;
	_CCST;
	pHelperSrc;
	lParam;
CRect rcPadding( 3, 3, 3, 3 );
	if( ! bCaption )
		rcPadding.left += 10;
	rcPadding.left = UiScalingDo( rcPadding.left, __EUIST_X );
	rcPadding.top = UiScalingDo( rcPadding.top, __EUIST_Y );
	rcPadding.right = UiScalingDo( rcPadding.right, __EUIST_X );
	rcPadding.bottom = UiScalingDo( rcPadding.bottom, __EUIST_Y );
	return rcPadding;
}

INT CExtPaintManager::ScreenTip_GetSeparatorHeight(
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( &_CCST );
	pHelperSrc;
	lParam;
bool bHaveSeparator = ( (_CCST.GetFlags()&__ECCST_SEPARATOR) != 0 ) ? true : false;
	if( ! bHaveSeparator )
		return 0;
INT nSeparatorHeight = UiScalingDo( 11, __EUIST_Y );
	return nSeparatorHeight;
}

COLORREF CExtPaintManager::ScreenTip_GetTextColor(
	bool bCaption,
	bool bMainPart,
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( &_CCST );
	bCaption;
	bMainPart;
	_CCST;
COLORREF clrText = GetColor( COLOR_INFOTEXT, pHelperSrc, lParam );
	return clrText;
}

CSize CExtPaintManager::ScreenTip_Measure(
	CDC & dc,
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( &_CCST );
	if( _CCST.IsEmpty() )
		return CSize( 0, 0 );
bool bMainPartEmpty = true, bSecondaryPartEmpty = true;
CSize _sizeBmpMain = _CCST.BmpMainGet().GetSize();
	_sizeBmpMain.cx = UiScalingDo( _sizeBmpMain.cx, __EUIST_X );
	_sizeBmpMain.cy = UiScalingDo( _sizeBmpMain.cy, __EUIST_Y );
	if( _sizeBmpMain.cx > 0 && _sizeBmpMain.cy > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerBmp( true, _CCST, pHelperSrc, lParam );
		_sizeBmpMain.cx += rcPadding.left + rcPadding.right;
		_sizeBmpMain.cy += rcPadding.top + rcPadding.bottom;
		bMainPartEmpty = false;
	}
CSize _sizeBmpSecondary = _CCST.BmpSecondaryGet().GetSize();
	_sizeBmpSecondary.cx = UiScalingDo( _sizeBmpSecondary.cx, __EUIST_X );
	_sizeBmpSecondary.cy = UiScalingDo( _sizeBmpSecondary.cy, __EUIST_Y );
	if( _sizeBmpSecondary.cx > 0 && _sizeBmpSecondary.cy > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerBmp( false, _CCST, pHelperSrc, lParam );
		_sizeBmpSecondary.cx += rcPadding.left + rcPadding.right;
		_sizeBmpSecondary.cy += rcPadding.top + rcPadding.bottom;
		bSecondaryPartEmpty = false;
	}
CExtSafeString str;
	str = _CCST.CaptionMainGet();
CSize _sizeCaptionMain( 0, 0 );
	if( str.GetLength() > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerText( true, true, _CCST, pHelperSrc, lParam );
		CFont & _font = ScreenTip_GetFont( true, true, _CCST, pHelperSrc, lParam );
		_sizeCaptionMain = stat_CalcTextDimension( dc, _font, str, DT_CALCRECT|DT_SINGLELINE|DT_LEFT|DT_TOP ).Size();
		_sizeCaptionMain.cx += rcPadding.left + rcPadding.right;
		_sizeCaptionMain.cy += rcPadding.top + rcPadding.bottom;
		bMainPartEmpty = false;
	}
	str = _CCST.CaptionSecondaryGet();
CSize _sizeCaptionSecondary( 0, 0 );
	if( str.GetLength() > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerText( true, false, _CCST, pHelperSrc, lParam );
		CFont & _font = ScreenTip_GetFont( true, false, _CCST, pHelperSrc, lParam );
		_sizeCaptionSecondary = stat_CalcTextDimension( dc, _font, str, DT_CALCRECT|DT_SINGLELINE|DT_LEFT|DT_TOP ).Size();
		_sizeCaptionSecondary.cx += rcPadding.left + rcPadding.right;
		_sizeCaptionSecondary.cy += rcPadding.top + rcPadding.bottom;
		bSecondaryPartEmpty = false;
	}
CSize _sizeTextMain( 0, 0 );
	str = _CCST.TextMainGet();
	if( str.GetLength() > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerText( false, true, _CCST, pHelperSrc, lParam );
		CFont & _font = ScreenTip_GetFont( false, true, _CCST, pHelperSrc, lParam );
		_sizeTextMain = stat_CalcTextDimension( dc, _font, str, DT_CALCRECT|DT_LEFT|DT_TOP ).Size();
		_sizeTextMain.cx += rcPadding.left + rcPadding.right;
		_sizeTextMain.cy += rcPadding.top + rcPadding.bottom;
		bMainPartEmpty = false;
	}
CSize _sizeTextSecondary( 0, 0 );
	str = _CCST.TextSecondaryGet();
	if( str.GetLength() > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerText( false, false, _CCST, pHelperSrc, lParam );
		CFont & _font = ScreenTip_GetFont( false, false, _CCST, pHelperSrc, lParam );
		_sizeTextSecondary = stat_CalcTextDimension( dc, _font, str, DT_CALCRECT|DT_LEFT|DT_TOP ).Size();
		_sizeTextSecondary.cx += rcPadding.left + rcPadding.right;
		_sizeTextSecondary.cy += rcPadding.top + rcPadding.bottom;
		bSecondaryPartEmpty = false;
	}
	if( bMainPartEmpty && bSecondaryPartEmpty )
		return CSize( 0, 0 );
CRect rcPadding = ScreenTip_GetPaddingOuter( _CCST, pHelperSrc, lParam );
CSize _sizeAll( rcPadding.left + rcPadding.right, rcPadding.top + rcPadding.bottom ), _sizeMain( 0, 0 ), _sizeSecondary( 0, 0 );
	if( ! ( bMainPartEmpty || bSecondaryPartEmpty ) )
		_sizeAll.cy += ScreenTip_GetSeparatorHeight( _CCST, pHelperSrc, lParam );
	if( ! bMainPartEmpty )
	{
		_sizeMain = _sizeCaptionMain;
		INT nWidth = _sizeBmpMain.cx + _sizeTextMain.cx;
		_sizeMain.cx = max( _sizeMain.cx, nWidth );
		INT nHeight = max( _sizeBmpMain.cy, _sizeTextMain.cy );
		_sizeMain.cy += nHeight;
	}
	if( ! bSecondaryPartEmpty )
	{
		_sizeSecondary.cx = max( _sizeCaptionSecondary.cx, _sizeTextSecondary.cx );
		_sizeSecondary.cx += _sizeBmpSecondary.cx;
		_sizeSecondary.cy = _sizeCaptionSecondary.cy + _sizeTextSecondary.cy;
		_sizeSecondary.cy = max( _sizeBmpSecondary.cy, _sizeSecondary.cy );
	}
	_sizeAll.cx += max( _sizeMain.cx, _sizeSecondary.cx );
	_sizeAll.cy += _sizeMain.cy + _sizeSecondary.cy;
	return _sizeAll;
}

void CExtPaintManager::ScreenTip_PaintBackground(
	CDC & dc,
	const RECT & rcScreenTip,
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( &_CCST );
	_CCST;
COLORREF clrOldText = dc.GetTextColor();
COLORREF clrBk = GetColor( COLOR_INFOBK, pHelperSrc, lParam );
	dc.FillSolidRect( &rcScreenTip, clrBk );
COLORREF clrLT = GetColor( COLOR_3DFACE, pHelperSrc, lParam );
COLORREF clrRB = GetColor( COLOR_3DSHADOW, pHelperSrc, lParam );
	dc.Draw3dRect( &rcScreenTip, clrLT, clrRB );
	dc.SetTextColor( clrOldText );
}

void CExtPaintManager::ScreenTip_Paint(
	CDC & dc,
	const RECT & rcScreenTip,
	CExtCustomizeCmdScreenTip & _CCST,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( &_CCST );
	if( ! dc.RectVisible( &rcScreenTip ) )
		return;
bool bPaintBackGround = true;
	if(		pHelperSrc != NULL
		&&	pHelperSrc->IsKindOf( RUNTIME_CLASS( CExtPopupMenuTipWnd ) )
		)
	{
		CExtPopupMenuTipWnd::e_tip_style_t eTS = ((CExtPopupMenuTipWnd*)pHelperSrc)->GetTipStyle();
		if(		eTS == CExtPopupMenuTipWnd::__ETS_BALLOON
			||	eTS == CExtPopupMenuTipWnd::__ETS_BALLOON
			)
			bPaintBackGround = false;
	}
	if( bPaintBackGround )
		ScreenTip_PaintBackground(
			dc,
			rcScreenTip,
			_CCST,
			pHelperSrc,
			lParam
			);
	if( _CCST.IsEmpty() )
		return;
bool bMainPartEmpty = true, bSecondaryPartEmpty = true;
CSize _sizeBmpMain = _CCST.BmpMainGet().GetSize();
	_sizeBmpMain.cx = UiScalingDo( _sizeBmpMain.cx, __EUIST_X );
	_sizeBmpMain.cy = UiScalingDo( _sizeBmpMain.cy, __EUIST_Y );
	if( _sizeBmpMain.cx > 0 && _sizeBmpMain.cy > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerBmp( true, _CCST, pHelperSrc, lParam );
		_sizeBmpMain.cx += rcPadding.left + rcPadding.right;
		_sizeBmpMain.cy += rcPadding.top + rcPadding.bottom;
		bMainPartEmpty = false;
	}
CSize _sizeBmpSecondary = _CCST.BmpSecondaryGet().GetSize();
	_sizeBmpSecondary.cx = UiScalingDo( _sizeBmpSecondary.cx, __EUIST_X );
	_sizeBmpSecondary.cy = UiScalingDo( _sizeBmpSecondary.cy, __EUIST_Y );
	if( _sizeBmpSecondary.cx > 0 && _sizeBmpSecondary.cy > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerBmp( false, _CCST, pHelperSrc, lParam );
		_sizeBmpSecondary.cx += rcPadding.left + rcPadding.right;
		_sizeBmpSecondary.cy += rcPadding.top + rcPadding.bottom;
		bSecondaryPartEmpty = false;
	}
CExtSafeString str;
	str = _CCST.CaptionMainGet();
CSize _sizeCaptionMain( 0, 0 );
	if( str.GetLength() > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerText( true, true, _CCST, pHelperSrc, lParam );
		CFont & _font = ScreenTip_GetFont( true, true, _CCST, pHelperSrc, lParam );
		_sizeCaptionMain = stat_CalcTextDimension( dc, _font, str, DT_CALCRECT|DT_SINGLELINE|DT_LEFT|DT_TOP ).Size();
		_sizeCaptionMain.cx += rcPadding.left + rcPadding.right;
		_sizeCaptionMain.cy += rcPadding.top + rcPadding.bottom;
		bMainPartEmpty = false;
	}
	str = _CCST.CaptionSecondaryGet();
CSize _sizeCaptionSecondary( 0, 0 );
	if( str.GetLength() > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerText( true, false, _CCST, pHelperSrc, lParam );
		CFont & _font = ScreenTip_GetFont( true, false, _CCST, pHelperSrc, lParam );
		_sizeCaptionSecondary = stat_CalcTextDimension( dc, _font, str, DT_CALCRECT|DT_SINGLELINE|DT_LEFT|DT_TOP ).Size();
		_sizeCaptionSecondary.cx += rcPadding.left + rcPadding.right;
		_sizeCaptionSecondary.cy += rcPadding.top + rcPadding.bottom;
		bSecondaryPartEmpty = false;
	}
CSize _sizeTextMain( 0, 0 );
	str = _CCST.TextMainGet();
	if( str.GetLength() > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerText( false, true, _CCST, pHelperSrc, lParam );
		CFont & _font = ScreenTip_GetFont( false, true, _CCST, pHelperSrc, lParam );
		_sizeTextMain = stat_CalcTextDimension( dc, _font, str, DT_CALCRECT|DT_LEFT|DT_TOP ).Size();
		_sizeTextMain.cx += rcPadding.left + rcPadding.right;
		_sizeTextMain.cy += rcPadding.top + rcPadding.bottom;
		bMainPartEmpty = false;
	}
CSize _sizeTextSecondary( 0, 0 );
	str = _CCST.TextSecondaryGet();
	if( str.GetLength() > 0 )
	{
		CRect rcPadding = ScreenTip_GetPaddingInnerText( false, false, _CCST, pHelperSrc, lParam );
		CFont & _font = ScreenTip_GetFont( false, false, _CCST, pHelperSrc, lParam );
		_sizeTextSecondary = stat_CalcTextDimension( dc, _font, str, DT_CALCRECT|DT_LEFT|DT_TOP ).Size();
		_sizeTextSecondary.cx += rcPadding.left + rcPadding.right;
		_sizeTextSecondary.cy += rcPadding.top + rcPadding.bottom;
		bSecondaryPartEmpty = false;
	}
	if( bMainPartEmpty && bSecondaryPartEmpty )
		return;
CSize _sizeMain( 0, 0 ), _sizeSecondary( 0, 0 );
	if( ! bMainPartEmpty )
	{
		_sizeMain = _sizeCaptionMain;
		INT nWidth = _sizeBmpMain.cx + _sizeTextMain.cx;
		_sizeMain.cx = max( _sizeMain.cx, nWidth );
		INT nHeight = max( _sizeBmpMain.cy, _sizeTextMain.cy );
		_sizeMain.cy += nHeight;
	}
	if( ! bSecondaryPartEmpty )
	{
		_sizeSecondary.cx = max( _sizeCaptionSecondary.cx, _sizeTextSecondary.cx );
		_sizeSecondary.cx += _sizeBmpSecondary.cx;
		_sizeSecondary.cy = _sizeCaptionSecondary.cy + _sizeTextSecondary.cy;
		_sizeSecondary.cy = max( _sizeBmpSecondary.cy, _sizeSecondary.cy );
	}
INT nSeparatorHeight = 0;
	if( ! ( bMainPartEmpty || bSecondaryPartEmpty ) )
		nSeparatorHeight = ScreenTip_GetSeparatorHeight( _CCST, pHelperSrc, lParam );
CRect rcPadding = ScreenTip_GetPaddingOuter( _CCST, pHelperSrc, lParam );
CRect rcMainAll = rcScreenTip;
	rcMainAll.DeflateRect( rcPadding.left, rcPadding.top, rcPadding.right, 0 );
	rcMainAll.bottom = rcMainAll.top + _sizeMain.cy;
CRect rcSecondaryAll( rcMainAll.left, rcMainAll.bottom, rcMainAll.right, rcMainAll.bottom + _sizeSecondary.cy );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
CRect rcCaption = rcMainAll, rcBmp = rcMainAll, rcText = rcMainAll;
	if( _sizeMain.cx > 0 && _sizeMain.cy > 0 )
	{
		str = _CCST.CaptionMainGet();
		rcCaption.bottom = rcBmp.top = rcText.top = rcCaption.top + _sizeCaptionMain.cy;
		rcBmp.right = rcText.left = rcBmp.left + _sizeBmpMain.cx;
		if( _sizeCaptionMain.cx > 0 && _sizeCaptionMain.cy > 0 )
		{
			CRect rcPadding = ScreenTip_GetPaddingInnerText( true, true, _CCST, pHelperSrc, lParam );
			rcCaption.DeflateRect( rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom );
			COLORREF clrText = ScreenTip_GetTextColor( true, true, _CCST, pHelperSrc, lParam );
			COLORREF clrOld = dc.SetTextColor( clrText );
			CFont & _font = ScreenTip_GetFont( true, true, _CCST, pHelperSrc, lParam );
			CFont * pOldFont = dc.SelectObject( &_font );
			str = _CCST.CaptionMainGet();
			ASSERT( str.GetLength() > 0 );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR( str ), str.GetLength(),
				&rcCaption,
				DT_SINGLELINE|DT_LEFT|DT_TOP, 0
				);
			dc.SelectObject( pOldFont );
			dc.SetTextColor( clrOld );
		}
		if( _sizeTextMain.cx > 0 && _sizeTextMain.cy > 0 )
		{
			UINT nDT = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
			if( ! str.IsEmpty() )
			{
				nDT = DT_LEFT|DT_TOP;
				CRect rcPadding = ScreenTip_GetPaddingInnerText( false, true, _CCST, pHelperSrc, lParam );
				rcText.DeflateRect( rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom );
			}
			COLORREF clrText = ScreenTip_GetTextColor( false, true, _CCST, pHelperSrc, lParam );
			COLORREF clrOld = dc.SetTextColor( clrText );
			CFont & _font = ScreenTip_GetFont( false, true, _CCST, pHelperSrc, lParam );
			CFont * pOldFont = dc.SelectObject( &_font );
			str = _CCST.TextMainGet();
			ASSERT( str.GetLength() > 0 );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR( str ), str.GetLength(),
				&rcText,
				nDT, 0
				);
			dc.SelectObject( pOldFont );
			dc.SetTextColor( clrOld );
		}
		if( _sizeBmpMain.cx > 0 && _sizeBmpMain.cy > 0 )
		{
			CRect rcPadding = ScreenTip_GetPaddingInnerBmp( true, _CCST, pHelperSrc, lParam );
			rcBmp.DeflateRect( rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom );
			CSize _size = _CCST.BmpMainGet().GetSize();
			rcBmp.right = rcBmp.left + _size.cx;
			rcBmp.bottom = rcBmp.top + _size.cy;
			_CCST.BmpMainGet().AlphaBlend( dc.m_hDC, rcBmp );
		}
	}
	if( nSeparatorHeight != 0 )
	{
		CRect rcSeparator(
			rcMainAll.left,
			rcMainAll.top + _sizeMain.cy,
			rcMainAll.right,
			rcMainAll.top + _sizeMain.cy + nSeparatorHeight
			);
		rcSeparator.DeflateRect( 0, rcSeparator.Height() / 2 );
		rcSeparator.bottom = rcSeparator.top + 2;
//		COLORREF clrLT = GetColor( COLOR_3DDKSHADOW, pHelperSrc, lParam );
//		COLORREF clrRB = GetColor( COLOR_3DHILIGHT, pHelperSrc, lParam );
		dc.Draw3dRect(
			&rcSeparator,
			RGB(145,153,164), // clrLT,
			RGB(224,224,224) // clrRB
			);
	}
	if( _sizeSecondary.cx > 0 && _sizeSecondary.cy > 0 )
	{
		rcCaption = rcMainAll;
		rcCaption.top += _sizeMain.cy + nSeparatorHeight;
		rcBmp = rcCaption;
		rcCaption.left += _sizeBmpSecondary.cx;
		rcText = rcCaption;
		rcCaption.bottom = rcText.top = rcCaption.top + _sizeCaptionSecondary.cy;
		rcText.bottom = rcBmp.bottom = rcMainAll.bottom + _sizeSecondary.cy + nSeparatorHeight;
		if( _sizeCaptionSecondary.cx > 0 && _sizeCaptionSecondary.cy > 0 )
		{
			CRect rcPadding = ScreenTip_GetPaddingInnerText( true, false, _CCST, pHelperSrc, lParam );
			rcCaption.DeflateRect( rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom );
			COLORREF clrText = ScreenTip_GetTextColor( true, false, _CCST, pHelperSrc, lParam );
			COLORREF clrOld = dc.SetTextColor( clrText );
			CFont & _font = ScreenTip_GetFont( true, false, _CCST, pHelperSrc, lParam );
			CFont * pOldFont = dc.SelectObject( &_font );
			str = _CCST.CaptionSecondaryGet();
			ASSERT( str.GetLength() > 0 );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR( str ), str.GetLength(),
				&rcCaption,
				DT_SINGLELINE|DT_LEFT|DT_TOP, 0
				);
			dc.SelectObject( pOldFont );
			dc.SetTextColor( clrOld );
		}
		if( _sizeTextSecondary.cx > 0 && _sizeTextSecondary.cy > 0 )
		{
			CRect rcPadding = ScreenTip_GetPaddingInnerText( false, false, _CCST, pHelperSrc, lParam );
			rcText.DeflateRect( rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom );
			COLORREF clrText = ScreenTip_GetTextColor( false, false, _CCST, pHelperSrc, lParam );
			COLORREF clrOld = dc.SetTextColor( clrText );
			CFont & _font = ScreenTip_GetFont( false, false, _CCST, pHelperSrc, lParam );
			CFont * pOldFont = dc.SelectObject( &_font );
			str = _CCST.TextSecondaryGet();
			ASSERT( str.GetLength() > 0 );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR( str ), str.GetLength(),
				&rcText,
				DT_LEFT|DT_TOP, 0
				);
			dc.SelectObject( pOldFont );
			dc.SetTextColor( clrOld );
		}
		if( _sizeBmpSecondary.cx > 0 && _sizeBmpSecondary.cy > 0 )
		{
			CRect rcPadding = ScreenTip_GetPaddingInnerBmp( false, _CCST, pHelperSrc, lParam );
			rcBmp.DeflateRect( rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom );
			CSize _size = _CCST.BmpSecondaryGet().GetSize();
			rcBmp.right = rcBmp.left + _size.cx;
			rcBmp.bottom = rcBmp.top + _size.cy;
			_CCST.BmpSecondaryGet().AlphaBlend( dc.m_hDC, rcBmp );
		}
	}
	dc.SetBkMode( nOldBkMode );
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)

void CExtPaintManager::PageContainer_EraseEntire(
	CDC & dc,
	CRect rcClient,
	CExtPageContainerWnd * pWndPageContainer,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pWndPageContainer );
	if( ! dc.RectVisible( &rcClient ) )
		return;
	if(		(! GetCb2DbTransparentMode( pWndPageContainer, lParam ) )
		||	(! PaintDockerBkgnd( true, dc, pWndPageContainer, lParam ) )
		)
		dc.FillSolidRect( &rcClient, GetColor( CExtPaintManager::CLR_3DFACE_OUT, pWndPageContainer, lParam ) );
}

void CExtPaintManager::PageContainer_EraseScrollPosArea(
	CDC & dc,
	CRect rcScrollPosArea,
	LONG nScrollPos,
	LONG nScrollSize,
	CExtPageContainerWnd * pWndPageContainer,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pWndPageContainer );
	ASSERT( nScrollPos <= nScrollSize );
	nScrollPos;
	nScrollSize;
	if(		(rcScrollPosArea.right - rcScrollPosArea.left) <= 1
		||	(rcScrollPosArea.bottom - rcScrollPosArea.top) <= 1
		)
		return;
	if( ! dc.RectVisible( &rcScrollPosArea ) )
		return;
COLORREF clrLight = GetColor( COLOR_3DHIGHLIGHT, pWndPageContainer, lParam );
COLORREF clrShadow = GetColor( COLOR_3DDKSHADOW, pWndPageContainer, lParam );
COLORREF clrFace = GetColor( COLOR_3DSHADOW, pWndPageContainer, lParam );
	dc.FillSolidRect(
		&rcScrollPosArea,
		clrFace
		);
	dc.Draw3dRect(
		&rcScrollPosArea,
		clrShadow,
		clrLight
		);
}

void CExtPaintManager::PageContainer_DrawScrollPosArea(
	CDC & dc,
	CRect rcScrollPosArea,
	LONG nScrollPos,
	LONG nScrollSize,
	CExtPageContainerWnd * pWndPageContainer,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pWndPageContainer );
	ASSERT( nScrollPos <= nScrollSize );
	if(		(rcScrollPosArea.right - rcScrollPosArea.left) <= 1
		||	(rcScrollPosArea.bottom - rcScrollPosArea.top) <= 1
		)
		return;
	if( ! dc.RectVisible( &rcScrollPosArea ) )
		return;
CRect rcScrollBtn( rcScrollPosArea );
	rcScrollBtn.DeflateRect( 1, 1 );
	if(		(rcScrollBtn.right - rcScrollBtn.left) <= 1
		||	(rcScrollBtn.bottom - rcScrollBtn.top) <= 1
		)
		return;
	if( nScrollSize > 0 )
	{
		ASSERT( nScrollPos <= nScrollSize );
		bool bHorzLayout = pWndPageContainer->IsHorizontalLayout();
		LONG nScrollPecrent =
			::MulDiv(
				nScrollPos,
				100,
				nScrollSize
				);
		LONG nWindowMetric =
			bHorzLayout
				? rcScrollPosArea.Width()
				: rcScrollPosArea.Height()
				;
		LONG nBtnMetric =
			::MulDiv(
				nWindowMetric,
				100,
				( nScrollSize + nWindowMetric )
				);
		nBtnMetric =
			::MulDiv(
				nBtnMetric,
				nWindowMetric,
				100
				);
		if( nBtnMetric < 10L )
			nBtnMetric = 10L;
		LONG nBtnMetricMax =
			bHorzLayout
				? rcScrollBtn.Width()
				: rcScrollBtn.Height()
				;
		LONG nBtnOffset = 0L;
		if( nBtnMetric >= nBtnMetricMax
			|| nBtnMetric >= ( nWindowMetric - 2 )
			)
			nBtnMetric =
				min(
					nBtnMetricMax,
					( nWindowMetric - 2 )
					);
		else
		{
			nBtnOffset =
				::MulDiv(
					nWindowMetric - nBtnMetric,
					nScrollPecrent,
					100
					);
		} // else from if( nBtnMetric >= nBtnMetricMax )
		if( bHorzLayout )
		{
			rcScrollBtn.left += nBtnOffset;
			rcScrollBtn.right =
				rcScrollBtn.left + nBtnMetric;
			if( (rcScrollBtn.right+1) >= rcScrollPosArea.right )
				rcScrollBtn.right =
					rcScrollPosArea.right - 1;
		} // if( bHorzLayout )
		else
		{
			rcScrollBtn.top += nBtnOffset;
			rcScrollBtn.bottom =
				rcScrollBtn.top + nBtnMetric;
			if( (rcScrollBtn.bottom+1) >= rcScrollPosArea.bottom )
				rcScrollBtn.bottom =
					rcScrollPosArea.bottom - 1;
		} // else from if( bHorzLayout )
	} // if( nScrollSize > 0 )
COLORREF clrLight = GetColor( COLOR_3DHIGHLIGHT, pWndPageContainer, lParam );
COLORREF clrShadow = GetColor( COLOR_3DDKSHADOW, pWndPageContainer, lParam );
COLORREF clrFace = GetColor( COLOR_3DFACE, pWndPageContainer, lParam );
	dc.FillSolidRect(
		&rcScrollBtn,
		clrFace
		);
	dc.Draw3dRect(
		&rcScrollBtn,
		clrLight,
		clrShadow
		);
}

void CExtPaintManager::PageContainer_EraseItem(
	CDC & dc,
	LPVOID pPageInfo, // CExtPageContainerWnd::PAGE_ITEM_INFO
	CRect rcPage,
	CRect rcCaption,
	CRect rcWnd,
	CExtPageContainerWnd * pWndPageContainer,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPageInfo != NULL );
	ASSERT( ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsVisible() );
	ASSERT_VALID( pWndPageContainer );
	rcCaption;
	rcWnd;
	if(		rcPage.left >= rcPage.right
		||	rcPage.top >= rcPage.bottom
		)
		return;
	if( ! dc.RectVisible( &rcPage ) )
		return;
DWORD dwPageContainerStyle = pWndPageContainer->GetPageContainerStyle();
	if( (dwPageContainerStyle & __EPCWS_PAGE_BORDERS) == 0 )
		return;
bool bPageIsExpanded = ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsExpanded();
	if( ! bPageIsExpanded )
		return;
	ASSERT(
			rcPage.left <= rcWnd.left
		&&	rcPage.top <= rcWnd.top
		&&	rcPage.right >= rcWnd.right
		&&	rcPage.bottom >= rcWnd.bottom
		);
CRect rcMiddle(
		(rcPage.left + rcWnd.left) / 2,
		(rcPage.top + rcWnd.top) / 2,
		(rcPage.right + rcWnd.right) / 2,
		(rcPage.bottom + rcWnd.bottom) / 2
	   );
	if( m_bPageContainerUseGroupBoxStyle )
	{
		PaintGroupBoxFrame(
			dc,
			rcMiddle,
			COLORREF(-1L),
			pWndPageContainer,
			lParam
			);
	} // if( m_bPageContainerUseGroupBoxStyle )
	else
	{
		if(		rcMiddle.left < rcMiddle.right
			&&	rcMiddle.top < rcMiddle.bottom
			)
		{
			if(		( dwPageContainerStyle & __EPCWS_CAPTION_FLAT ) != 0
				&&	(! ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsCaptionHover() )
				&&	(! ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsCaptionPressed() )
				)
			{
				HWND hWndPage = ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->GetSafePageHwnd();
				ASSERT( hWndPage != NULL );
				ASSERT( ::IsWindow( hWndPage ) );
				bool bEnabled = ( ::IsWindowEnabled( hWndPage ) ) ? true : false;
				COLORREF clrFrame =
					GetColor(
						bEnabled
							? COLOR_3DDKSHADOW
							: COLOR_3DSHADOW
							,
						pWndPageContainer,
						lParam
						);
				dc.Draw3dRect( &rcMiddle, clrFrame, clrFrame );
			}
			else
			{
				COLORREF clrLight = GetColor( COLOR_3DHIGHLIGHT, pWndPageContainer, lParam );
				COLORREF clrShadow = GetColor( COLOR_3DSHADOW, pWndPageContainer, lParam );
				dc.Draw3dRect( &rcMiddle, clrShadow, clrLight );
				rcMiddle.DeflateRect( 1, 1 );
				dc.Draw3dRect( &rcMiddle, clrLight, clrShadow );
			}
		}
	} // else from if( m_bPageContainerUseGroupBoxStyle )
}

void CExtPaintManager::PageContainer_DrawItem(
	CDC & dc,
	LPVOID pPageInfo, // CExtPageContainerWnd::PAGE_ITEM_INFO
	CRect rcPage,
	CRect rcCaption,
	CRect rcWnd,
	CExtPageContainerWnd * pWndPageContainer,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPageInfo != NULL );
	ASSERT_VALID( pWndPageContainer );
	dc;
	pPageInfo; // CExtPageContainerWnd::PAGE_ITEM_INFO
	rcPage;
	rcCaption;
	rcWnd;
	pWndPageContainer;
	lParam;
}

void CExtPaintManager::PageContainer_EraseCaption(
	CDC & dc,
	LPVOID pPageInfo, // CExtPageContainerWnd::PAGE_ITEM_INFO
	CRect rcCaption,
	CExtPageContainerWnd * pWndPageContainer,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPageInfo != NULL );
	ASSERT_VALID( pWndPageContainer );
	ASSERT( pPageInfo != NULL );
	ASSERT( ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsVisible() );
	pPageInfo;
	if(		(rcCaption.right - rcCaption.left) <= 4 
		||	(rcCaption.bottom - rcCaption.top) <= 4 
		)
		return;
	if( ! dc.RectVisible( &rcCaption ) )
		return;
	if( m_bPageContainerUseButtonStyle )
	{
		HWND hWndPage = ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->GetSafePageHwnd();
		ASSERT( hWndPage != NULL );
		ASSERT( ::IsWindow( hWndPage ) );
		bool bEnabled = ( ::IsWindowEnabled( hWndPage ) ) ? true : false;
		DWORD dwPageContainerStyle = pWndPageContainer->GetPageContainerStyle();
		PAINTPUSHBUTTONDATA _ppbd(
			pWndPageContainer,
			true, // bHorz
			rcCaption,
			NULL, // sText
			NULL, // hIcon
			( dwPageContainerStyle & __EPCWS_CAPTION_FLAT ) ? true : false, //bool bFlat
			((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsCaptionHover(),
			((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsCaptionPressed(),
			false, // bIndeterminate
			bEnabled,
			true, // bDrawBorder
			false, // bDrawFocusRect
			false,
			__ALIGN_HORIZ_CENTER,
			NULL,
			false,
			0,
			false
			);
		CExtMemoryDC dcX( &dc, &rcCaption );
		PaintPushButton( dcX, _ppbd );
	} // if( m_bPageContainerUseButtonStyle )
	else
	{
		COLORREF clrFace = GetColor( CExtPaintManager::CLR_3DFACE_OUT, pWndPageContainer, lParam );
		dc.FillSolidRect( &rcCaption, clrFace );
	} // else from if( m_bPageContainerUseButtonStyle )
}

void CExtPaintManager::PageContainer_DrawCaption(
	CDC & dc,
	LPVOID pPageInfo, // CExtPageContainerWnd::PAGE_ITEM_INFO
	CRect rcCaption,
	CExtPageContainerWnd * pWndPageContainer,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPageInfo != NULL );
	ASSERT_VALID( pWndPageContainer );
	ASSERT( pPageInfo != NULL );
	ASSERT( ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsVisible() );
	if(		(rcCaption.right - rcCaption.left) <= 4 
		||	(rcCaption.bottom - rcCaption.top) <= 4 
		)
		return;
	if( ! dc.RectVisible( &rcCaption ) )
		return;
CRgn rgnCaption;
	if( rgnCaption.CreateRectRgnIndirect( &rcCaption ) )
	{
		ASSERT( rgnCaption.GetSafeHandle() != NULL );
		dc.SelectClipRgn( &rgnCaption );
	}
HWND hWndPage = ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->GetSafePageHwnd();
	ASSERT( hWndPage != NULL );
	ASSERT( ::IsWindow( hWndPage ) );
bool bEnabled = ( ::IsWindowEnabled( hWndPage ) ) ? true : false;
COLORREF clrLight = GetColor( COLOR_3DHIGHLIGHT, pWndPageContainer, lParam );
COLORREF clrFace = GetColor( CExtPaintManager::CLR_3DFACE_OUT, pWndPageContainer, lParam );
COLORREF clrShadow = GetColor( COLOR_3DSHADOW, pWndPageContainer, lParam );
COLORREF clrDkShadow = GetColor( COLOR_3DDKSHADOW, pWndPageContainer, lParam );
CRect rcCaptBorder( rcCaption );
bool bHorzLayout = pWndPageContainer->IsHorizontalLayout();
	if( bHorzLayout )
	{
		rcCaption.DeflateRect(
			__EXT_PAGECONTAINERWND_CAPT_HBORDER_DX,
			__EXT_PAGECONTAINERWND_CAPT_HBORDER_DY
			);
		rcCaption.bottom -= __EXT_PAGECONTAINERWND_CAPT_BEFORE_DY;
		rcCaption.top += __EXT_PAGECONTAINERWND_CAPT_AFTER_DY;
	} // if( bHorzLayout )
	else
	{
		rcCaption.DeflateRect(
			__EXT_PAGECONTAINERWND_CAPT_VBORDER_DX,
			__EXT_PAGECONTAINERWND_CAPT_VBORDER_DY
			);
		rcCaption.left += __EXT_PAGECONTAINERWND_CAPT_BEFORE_DX;
		rcCaption.right -= __EXT_PAGECONTAINERWND_CAPT_AFTER_DX;
	} // else from if( bHorzLayout )

DWORD dwPageContainerStyle = pWndPageContainer->GetPageContainerStyle();
	if( ( dwPageContainerStyle & __EPCWS_CAPTION_EXPBTN ) != 0 )
	{
		CExtPaintManager::glyph_t & _glyph =
			pWndPageContainer->OnPageContainerQueryItemGlyph(
				((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo),
				((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsExpanded()
				);
		CSize sizeGlyph = _glyph.Size();
		if( sizeGlyph.cx > 0 && sizeGlyph.cy > 0 )
		{
			CRect rcGlyph( rcCaption );
			if( bHorzLayout )
			{
				rcGlyph.left += (rcCaption.Width() - sizeGlyph.cx)/2;
				rcGlyph.top = rcGlyph.bottom - sizeGlyph.cy;
				rcGlyph.right = rcGlyph.left + sizeGlyph.cx;
				rcCaption.bottom = rcGlyph.top - __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DY;
			} // if( bHorzLayout )
			else
			{
				rcGlyph.right = rcGlyph.left + sizeGlyph.cx;
				rcGlyph.top += (rcCaption.Height() - sizeGlyph.cy)/2;
				rcGlyph.bottom = rcGlyph.top + sizeGlyph.cy;
				rcCaption.left = rcGlyph.right + __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DX;
			} // else from if( bHorzLayout )

			COLORREF ColorValues[] =
			{
				RGB(0,0,0),
				GetColor( bEnabled ? COLOR_BTNTEXT : CLR_TEXT_DISABLED, pWndPageContainer, lParam )
			};
			PaintGlyphCentered( dc, rcGlyph, _glyph, ColorValues );
		} // if( sizeGlyph.cx > 0 && sizeGlyph.cy > 0 )
//		if( bHorzLayout )
//		{
//			if( sizeGlyph.cy > 0 )
//				rcCaption.top +=
//					sizeGlyph.cy
//					+ __EXT_PAGECONTAINERWND_CAPT_AFTER_GLYPH_DY
//					;
//		} // if( bHorzLayout )
//		else
//		{
//			if( sizeGlyph.cx > 0 )
//				rcCaption.left +=
//					sizeGlyph.cx
//					+ __EXT_PAGECONTAINERWND_CAPT_AFTER_GLYPH_DX
//					;
//		} // else from if( bHorzLayout )
	} // if( ( dwPageContainerStyle & __EPCWS_CAPTION_EXPBTN ) != 0 )
CExtCmdIcon & _icon = ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IconGet();
CSize sizeIcon = _icon.GetSize();
	if( sizeIcon.cx > 0 && sizeIcon.cy > 0 )
	{
		CRect rcIcon( rcCaption );
		if( bHorzLayout )
		{
			rcIcon.left += (rcCaption.Width() - sizeIcon.cx)/2;
			rcIcon.top = rcIcon.bottom - sizeIcon.cy;
			rcIcon.right = rcIcon.left + sizeIcon.cx;
			rcCaption.bottom = rcIcon.top - __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DY;
		} // if( bHorzLayout )
		else
		{
			rcIcon.right = rcIcon.left + sizeIcon.cx;
			rcIcon.top += (rcCaption.Height() - sizeIcon.cy)/2;
			rcIcon.bottom = rcIcon.top + sizeIcon.cy;
			rcCaption.left = rcIcon.right + __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DX;
		} // else from if( bHorzLayout )

		if(		rcIcon.right > rcIcon.left
			&&	rcIcon.bottom > rcIcon.top
			)
		{
			_icon.Paint(
				this,
				dc.GetSafeHdc(),
				rcIcon,
				bEnabled
					? CExtCmdIcon::__PAINT_NORMAL
					: CExtCmdIcon::__PAINT_DISABLED
				);
		}
	} // if( sizeIcon.cx > 0 && sizeIcon.cy > 0 )
bool bCenteredText =
		(dwPageContainerStyle & __EPCWS_CAPTION_CENTER_TEXT)
			? true :  false;
__EXT_MFC_SAFE_LPCTSTR sItemText = ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->TextGet();
LONG nItemTextLen = 0;
	if(		sItemText != NULL
		&&	( nItemTextLen = LONG(_tcslen(sItemText)) ) > 0
		&&	rcCaption.right > rcCaption.left
		&&	rcCaption.bottom > rcCaption.top
		)
	{ // if can draw text
		CFont * pItemFont = ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->GetCaptionFont( bHorzLayout );
		ASSERT( pItemFont != NULL );
		ASSERT( pItemFont->GetSafeHandle() != NULL );
		CFont * pOldFont = dc.SelectObject( pItemFont );
		COLORREF clrTextOld =
			dc.SetTextColor(
				GetColor(
					bEnabled ? COLOR_BTNTEXT : ( m_bPageContainerNoEmbossDisabledText ? CLR_TEXT_DISABLED : COLOR_3DHILIGHT ),
					pWndPageContainer,
					lParam
					)
				);
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
		if( bHorzLayout )
			eLO = CExtRichContentLayout::__ELOT_270_CW;
		UINT nFormat = DT_SINGLELINE|DT_END_ELLIPSIS;
		if( bCenteredText )
			nFormat |= DT_CENTER|DT_VCENTER;
		else
			nFormat |= DT_VCENTER|DT_LEFT;
		CExtRichContentLayout::stat_DrawText(
			CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
			dc.m_hDC, sItemText, nItemTextLen, &rcCaption, nFormat, 0
			);
		if( ( ! bEnabled ) && ( ! m_bPageContainerNoEmbossDisabledText ) )
		{
			dc.SetTextColor( GetColor( CLR_TEXT_DISABLED, pWndPageContainer, lParam ) );
			rcCaption.OffsetRect( -1, -1 );
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, sItemText, nItemTextLen, &rcCaption, nFormat, 0
				);
		} // if( ( ! bEnabled ) && ( ! m_bPageContainerNoEmbossDisabledText ) )
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrTextOld );
		dc.SelectObject( pOldFont );
	} // if can draw text
	if( ! m_bPageContainerUseButtonStyle )
	{
		if( ( dwPageContainerStyle & __EPCWS_CAPTION_FLAT ) != 0 )
		{ // if flat captions
			if( ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsCaptionPressed() )
			{ // if caption is pressed
				dc.Draw3dRect(
					&rcCaptBorder,
					clrShadow,
					clrLight
					);
			} // if caption is pressed
			else if( ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsCaptionHover() )
			{ // if caption is under mouse pointer
				dc.Draw3dRect(
					&rcCaptBorder,
					clrLight,
					clrShadow
					);
			} // if caption is under mouse pointer
			else
			{ // if caption is neither pressed nor under mouse pointer
				dc.Draw3dRect(
					&rcCaptBorder,
					bEnabled ? clrDkShadow : clrShadow,
					bEnabled ? clrDkShadow : clrShadow
					);
			} // if caption is neither pressed nor under mouse pointer
		} // if flat captions
		else
		{ // if semi-flat captions
			if( ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsCaptionPressed() )
			{ // if caption is pressed
				dc.Draw3dRect(
					&rcCaptBorder,
					clrDkShadow,
					clrLight
					);
				rcCaptBorder.DeflateRect( 1, 1 );
				dc.Draw3dRect(
					&rcCaptBorder,
					clrShadow,
					clrFace
					);
			} // if caption is pressed
			else if( ((CExtPageContainerWnd::PAGE_ITEM_INFO*)pPageInfo)->IsCaptionHover() )
			{ // if caption is under mouse pointer
				dc.Draw3dRect(
					&rcCaptBorder,
					clrLight,
					clrDkShadow
					);
				rcCaptBorder.DeflateRect( 1, 1 );
				dc.Draw3dRect(
					&rcCaptBorder,
					clrFace,
					clrShadow
					);
			} // if caption is under mouse pointer
			else
			{ // if caption is neither pressed nor under mouse pointer
				dc.Draw3dRect(
					&rcCaptBorder,
					clrLight,
					clrShadow
					);
			} // if caption is neither pressed nor under mouse pointer
		} // if semi-flat captions
	} // if( ! m_bPageContainerUseButtonStyle )
	dc.SelectClipRgn( NULL );
}

#endif // (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
