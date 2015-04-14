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

#if (!defined __EXT_MFC_NO_CHECK_LIST)

#if (!defined __EXT_CHECK_LIST_H)
	#include <ExtCheckListWnd.h>
#endif // (!defined __EXT_CHECK_LIST_H)

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (! defined __VSSYM32_H__)
	#include <vssym32/vssym32.h>
#endif // (! defined __VSSYM32_H__)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtCheckListWnd

IMPLEMENT_DYNAMIC( CExtCheckListWnd, CListBox );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtCheckListWnd );

CExtCheckListWnd::CExtCheckListWnd()
	: m_nStyle( BS_CHECKBOX )
	, m_bUsePM( true )
{
	PmBridge_Install();
}

CExtCheckListWnd::~CExtCheckListWnd()
{
	PmBridge_Uninstall();
}

BEGIN_MESSAGE_MAP(CExtCheckListWnd, CListBox)
	//{{AFX_MSG_MAP(CExtCheckListWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_MESSAGE(LB_ADDSTRING, OnLBAddString)
	ON_MESSAGE(LB_FINDSTRING, OnLBFindString)
	ON_MESSAGE(LB_FINDSTRINGEXACT, OnLBFindStringExact)
	ON_MESSAGE(LB_GETITEMDATA, OnLBGetItemData)
	ON_MESSAGE(LB_GETTEXT, OnLBGetText)
	ON_MESSAGE(LB_INSERTSTRING, OnLBInsertString)
	ON_MESSAGE(LB_SELECTSTRING, OnLBSelectString)
	ON_MESSAGE(LB_SETITEMDATA, OnLBSetItemData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CExtCheckListWnd::SetCheckStyle(
	DWORD nStyle
	)
{
	ASSERT(
			nStyle == BS_CHECKBOX 
		||	nStyle == BS_AUTOCHECKBOX 
		||	nStyle == BS_AUTO3STATE 
		||	nStyle == BS_3STATE
		||	nStyle == BS_AUTORADIOBUTTON 
		||	nStyle == BS_RADIOBUTTON
		);
	m_nStyle = nStyle;
}

void CExtCheckListWnd::SetCheck(
	INT nIndex, 
	INT nCheck
	)
{
	ASSERT_VALID( this );
	if(		nCheck == 2 
		&&	m_nStyle != BS_3STATE
		&&	m_nStyle != BS_AUTO3STATE
		)
		return;
	LRESULT lResult = DefWindowProc( LB_GETITEMDATA, nIndex, 0 );
	if( lResult != LB_ERR )
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) lResult;
		if( pState == NULL )
			pState = new EXT_ITEM_DATA;
		pState->m_nCheck = nCheck;
		VERIFY( DefWindowProc( LB_SETITEMDATA, nIndex, (LPARAM)pState ) != LB_ERR );
		Invalidate();
	}
}

INT CExtCheckListWnd::GetCheck(
	INT nIndex
	)
{
	ASSERT_VALID( this );
	LRESULT lResult = DefWindowProc( LB_GETITEMDATA, nIndex, 0 );
	if( lResult != LB_ERR )
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) lResult;
		if( pState != NULL )
			return pState->m_nCheck;
	}
	return 0;
}

void CExtCheckListWnd::Enable(
	INT nIndex, 
	BOOL bEnabled
	)
{
	ASSERT_VALID( this );
	LRESULT lResult = DefWindowProc( LB_GETITEMDATA, nIndex, 0 );
	if( lResult != LB_ERR )
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) lResult;
		if( pState == NULL )
			pState = new EXT_ITEM_DATA;
		pState->m_bEnabled = bEnabled;
		VERIFY( DefWindowProc( LB_SETITEMDATA, nIndex, (LPARAM)pState) != LB_ERR );
		Invalidate();
	}
}

BOOL CExtCheckListWnd::IsEnabled(
	INT nIndex
	)
{
	ASSERT_VALID( this );
	LRESULT lResult = DefWindowProc( LB_GETITEMDATA, nIndex, 0 );
	if( lResult != LB_ERR )
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) lResult;
		if( pState != NULL )
			return pState->m_bEnabled;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

CRect CExtCheckListWnd::OnQueryItemMargins( 
	INT nIndex 
	) const
{
	ASSERT_VALID( this );
	if( !(nIndex >= 0 && nIndex < GetCount()) )
	{
		ASSERT( FALSE );
		return CRect( 0, 0, 0, 0 );
	}
	return CRect( 2, 1, 2, 1 );
}

CRect CExtCheckListWnd::OnQueryItemCheckMargins( 
	INT nIndex 
	) const
{
	ASSERT_VALID( this );
	if( !(nIndex >= 0 && nIndex < GetCount()) )
	{
		ASSERT( FALSE );
		return CRect( 0, 0, 0, 0 );
	}
	return CRect( 1, 0, 2, 0 );
}

HFONT CExtCheckListWnd::OnQueryItemFont( 
	INT nIndex 
	) const
{
	ASSERT_VALID( this );
	if( !(nIndex >= 0 && nIndex < GetCount()) )
	{
		ASSERT( FALSE );
		return NULL;
	}

	HFONT hFont = (HFONT)
		::SendMessage( m_hWnd, WM_GETFONT, 0L, 0L );
	if( hFont == NULL )
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		if( hWndParent != NULL )
			hFont = (HFONT)
				::SendMessage( hWndParent, WM_GETFONT, 0L, 0L );
	} // if( hFont == NULL )
	if( hFont == NULL )
	{
		hFont = (HFONT)::GetStockObject( DEFAULT_GUI_FONT );
		if( hFont == NULL )
			hFont = (HFONT)::GetStockObject( SYSTEM_FONT );
	} // if( hFont == NULL )
	return hFont;
}

CSize CExtCheckListWnd::OnQueryItemCheckSize( 
	CDC & dc, 
	INT nIndex 
	)
{
	ASSERT_VALID( this );
	ASSERT( nIndex >= 0 && nIndex < GetCount() );

	if( m_bUsePM )
	{
		CExtPaintManager * pPM = PmBridge_GetPM();
		bool bPushed = false, bEnabled = true;
		CExtPaintManager::eBoxState_t eState = CExtPaintManager::BOX_UNCHECKED;
		CRect rcCheck( 0, 0, 13, 13 );
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd(
			this,
			eState,
			false,
			false,
			rcCheck,
			_T(""),
			false,		
			bPushed,
			bEnabled,
			false, 
			0,
			(HFONT)pPM->m_FontNormal.GetSafeHandle(),
			true
			);
		return pPM->GetCheckButtonBoxSize( dc, _pcrbd );
	} // if( m_bUsePM )

	if(		g_PaintManager.m_UxTheme.IsControlsThemed()
		&&	g_PaintManager.m_UxTheme.OpenThemeData( GetSafeHwnd(), L"BUTTON" ) != NULL
		)
	{
		INT nCheck = GetCheck( nIndex );
		BOOL bDisabled = (!IsEnabled( nIndex ));

		INT nState = 0;
		INT nPartID = 0;
		switch( m_nStyle )
		{
		case BS_CHECKBOX:
		case BS_AUTOCHECKBOX:
		case BS_3STATE:
		case BS_AUTO3STATE:
		{
			nPartID = BP_CHECKBOX;
			if( nCheck > 0 )
			{
				nState = 
					bDisabled 
						? ( nCheck == 2 ? CBS_MIXEDDISABLED : CBS_CHECKEDDISABLED ) 
						: ( nCheck == 2 ? CBS_MIXEDNORMAL : CBS_CHECKEDNORMAL );
			}
			else
			{
				nState = 
					bDisabled 
						? CBS_UNCHECKEDDISABLED 
						: CBS_UNCHECKEDNORMAL;
			}
		}
		break;
		case BS_RADIOBUTTON:
		case BS_AUTORADIOBUTTON:
		{
			nPartID = BP_RADIOBUTTON;
			if( nCheck > 0 )
			{
				nState = 
					bDisabled 
						? RBS_CHECKEDDISABLED
						: RBS_CHECKEDNORMAL;
			}
			else
			{
				nState = 
					bDisabled 
						? RBS_UNCHECKEDDISABLED 
						: RBS_UNCHECKEDNORMAL;
			}
		}
		break;
		default:
			ASSERT( FALSE );
		} // switch( m_nStyle )
		
		CSize szPart(0,0);
		if( g_PaintManager.m_UxTheme.GetThemePartSize(
				dc.GetSafeHdc(), 
				nPartID, 
				nState, 
				NULL, 
				CExtUxTheme::__EXT_UX_TS_TRUE, 
				&szPart
				) != S_OK
			)
			szPart = CSize( 0, 0 );
		g_PaintManager.m_UxTheme.CloseThemeData( true );

		if( szPart != CSize( 0, 0 ) )
			return szPart;
	}	
	return CSize( 13, 13 );
}

//////////////////////////////////////////////////////////////////////////

void CExtCheckListWnd::OnPaintItemCheck(
	CDC & dc,
	const RECT & rcCheck,
	INT nIndex
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcCheck) )
		return;
	ASSERT( nIndex >= 0 && nIndex < GetCount() );
	
INT nCheck = GetCheck( nIndex );
BOOL bDisabled = (!IsEnabled( nIndex ));

	if( m_bUsePM )
	{
		bool bPushed = false, bHover = false, bPushTracking = false, bKeyTracking = false, bEnabled = bDisabled ? false : true;
		CExtPaintManager::eBoxState_t eState = CExtPaintManager::BOX_UNCHECKED;

		switch( nCheck ) 
		{
		case BST_UNCHECKED:
			if( bEnabled )
			{
				if(		( bHover && !bPushTracking) 
					||  (!bHover &&  bPushTracking && !bKeyTracking) 
					)
					eState = CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED;
				else if( (bHover && bPushTracking) || bKeyTracking )
					eState = CExtPaintManager::BOX_LDOWN_UNCHECKED;
				else
					eState = CExtPaintManager::BOX_UNCHECKED;
			}
			else
				eState = CExtPaintManager::BOX_DISABLED_UNCHECKED;
			break;
			
		case BST_CHECKED:
			if( bEnabled )
			{
				if(		( bHover && !bPushTracking) 
					||  (!bHover &&  bPushTracking && !bKeyTracking) 
					)
					eState = CExtPaintManager::BOX_MOUSE_HOVER_CHECKED;
				else if( (bHover && bPushTracking) || bKeyTracking )
					eState = CExtPaintManager::BOX_LDOWN_CHECKED;
				else
					eState = CExtPaintManager::BOX_CHECKED;
			}
			else
				eState = CExtPaintManager::BOX_DISABLED_CHECKED;
			break;
			
		case BST_INDETERMINATE:
			if( bEnabled )
			{
				if(		( bHover && !bPushTracking) 
					||  (!bHover &&  bPushTracking && !bKeyTracking ) 
					)
					eState = CExtPaintManager::BOX_MOUSE_HOVER_INDETERMINATE;
				else if( (bHover && bPushTracking) || bKeyTracking )
					eState = CExtPaintManager::BOX_LDOWN_INDETERMINATE;
				else
					eState = CExtPaintManager::BOX_INDETERMINATE;
			}
			else
				eState = CExtPaintManager::BOX_DISABLED_INDETERMINATE;
			break;
		}
		CExtPaintManager * pPM = PmBridge_GetPM();
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd(
			this,
			eState,
			false,
			false,
			rcCheck,
			_T(""),
			false,		
			bPushed,
			bEnabled,
			false, 
			0,
			(HFONT)pPM->m_FontNormal.GetSafeHandle(),
			true
			);
// 		_pcrbd.m_rcTextMargins.SetRect(0,0,0,0);
// 		_pcrbd.m_clrForceBk = GetBkColor();
// 		_pcrbd.m_clrForceTextDisabled = GetTextColorDisabled();
// 		_pcrbd.m_clrForceTextNormal = GetTextColorNormal();
// 		_pcrbd.m_clrForceTextHover = GetTextColorHover();
// 		_pcrbd.m_clrForceTextPressed = GetTextColorPressed();
		pPM->PaintCheckButton( dc, _pcrbd );
		return;
	} // if( m_bUsePM )

bool bDefaultDrawing = true;
	if(		g_PaintManager.m_UxTheme.IsControlsThemed()
		&&	g_PaintManager.m_UxTheme.OpenThemeData( GetSafeHwnd(), L"BUTTON" ) != NULL
		)
	{
		INT nState = 0;
		INT nPartID = 0;
		switch( m_nStyle )
		{
		case BS_CHECKBOX:
		case BS_AUTOCHECKBOX:
		case BS_3STATE:
		case BS_AUTO3STATE:
		{
			nPartID = BP_CHECKBOX;
			if( nCheck > 0 )
			{
				nState = 
					bDisabled 
						? ( nCheck == 2 ? CBS_MIXEDDISABLED : CBS_CHECKEDDISABLED ) 
						: ( nCheck == 2 ? CBS_MIXEDNORMAL : CBS_CHECKEDNORMAL );
			}
			else
			{
				nState = 
					bDisabled 
						? CBS_UNCHECKEDDISABLED 
						: CBS_UNCHECKEDNORMAL;
			}
		}
		break;
		case BS_RADIOBUTTON:
		case BS_AUTORADIOBUTTON:
		{
			nPartID = BP_RADIOBUTTON;
			if( nCheck > 0 )
			{
				nState = 
					bDisabled 
						? RBS_CHECKEDDISABLED
						: RBS_CHECKEDNORMAL;
			}
			else
			{
				nState = 
					bDisabled 
						? RBS_UNCHECKEDDISABLED 
						: RBS_UNCHECKEDNORMAL;
			}
		}
		break;
		default:
			ASSERT( FALSE );
			return;
		} // switch( m_nStyle )
		
		if( g_PaintManager.m_UxTheme.
				DrawThemeBackground(
					dc.GetSafeHdc(), 
					nPartID, 
					nState, 
					&rcCheck,
					&rcCheck
					) == S_OK
			)
			bDefaultDrawing = false;
			g_PaintManager.m_UxTheme.CloseThemeData( true );
	}
	if( bDefaultDrawing )
	{
		UINT nState = 0;
		switch( m_nStyle )
		{
		case BS_CHECKBOX:
		case BS_AUTOCHECKBOX:
			nState |= DFCS_BUTTONCHECK;
		break;
		case BS_RADIOBUTTON:
		case BS_AUTORADIOBUTTON:
			nState |= DFCS_BUTTONRADIO;
		break;
		case BS_3STATE:
		case BS_AUTO3STATE:
			nState |= DFCS_BUTTONCHECK;
			if( nCheck == 2 )
				nState |= DFCS_BUTTON3STATE;
		break;
		default:
			ASSERT( FALSE );
		break;
		}; // switch( m_nStyle )

		if( nCheck > 0 )
			nState |= DFCS_CHECKED;

		if( bDisabled )
			nState |= DFCS_INACTIVE;

		dc.DrawFrameControl(
			(RECT *)&rcCheck,
			DFC_BUTTON,
			nState //| DFCS_FLAT
			);
	} // if( bDefaultDrawing )
}

/////////////////////////////////////////////////////////////////////////////
// CExtCheckListWnd message handlers

void CExtCheckListWnd::PreSubclassWindow() 
{
	CListBox::PreSubclassWindow();
	DWORD dwStyle = GetStyle();
	ASSERT( (dwStyle & LBS_OWNERDRAWFIXED) != 0 || (dwStyle & LBS_OWNERDRAWVARIABLE) != 0 );
}

BOOL CExtCheckListWnd::PreCreateWindow( CREATESTRUCT& cs ) 
{
	if( ! CListBox::PreCreateWindow(cs) )
		return FALSE;

	if( (cs.style & LBS_OWNERDRAWVARIABLE) == 0 )
		cs.style |= LBS_OWNERDRAWVARIABLE;
	if( (cs.style & LBS_HASSTRINGS) == 0 )
		cs.style |= LBS_HASSTRINGS;

	return TRUE;
}

void CExtCheckListWnd::DrawItem( LPDRAWITEMSTRUCT lpDIS ) 
{
	ASSERT_VALID( this );
	ASSERT( lpDIS != NULL && lpDIS->hDC != NULL );
DWORD dwStyle = GetStyle();
INT nItem = (INT)lpDIS->itemID;
	if( nItem < 0 || nItem >= GetCount() )
		return;
CRect rcItem( lpDIS->rcItem );
CDC dc;
	dc.Attach( lpDIS->hDC );
	if( lpDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT) )
	{
		CRect rcErase( lpDIS->rcItem );
		if( nItem == (GetCount() - 1) )
		{
			CRect rcClient;
			GetClientRect( &rcClient );
			if( rcErase.bottom < rcClient.bottom )
				rcErase.bottom = rcClient.bottom;
		}
		dc.FillSolidRect(
			rcErase, 
			PmBridge_GetPM()->GetColor( COLOR_WINDOW, this )
			);
	}
CSize szCheck = OnQueryItemCheckSize( dc, nItem );
	ASSERT( szCheck.cx >= 0 && szCheck.cy >= 0 );
CRect rcCheck( rcItem );
	rcCheck.right = rcCheck.left + szCheck.cx;
CRect rcCheckMargins = OnQueryItemCheckMargins( nItem );
	rcCheck.OffsetRect( rcCheckMargins.TopLeft() );
	rcCheck.right += rcCheckMargins.right;
	rcCheck.bottom += rcCheckMargins.bottom;
	rcItem.left = rcCheck.right;
	rcCheck.DeflateRect( 0, (rcCheck.Height() - szCheck.cy) / 2 );
	OnPaintItemCheck( dc, rcCheck, nItem );
	if( (GetStyle() & LBS_HASSTRINGS) != 0 && nItem >= 0 )
	{
		CString sText;
		GetText( nItem, sText );
		if( ! sText.IsEmpty() )
		{
			HFONT hFont = OnQueryItemFont( nItem );
			ASSERT( hFont != NULL );
			CFont * pOldFont =
				dc.SelectObject( CFont::FromHandle( hFont ) );
			COLORREF clrTextOld =
				dc.SetTextColor(
					PmBridge_GetPM()->GetColor(
						( (lpDIS->itemState & ODS_SELECTED) != 0 && (dwStyle & LBS_NOSEL) == 0 )
							? COLOR_HIGHLIGHTTEXT
							: COLOR_BTNTEXT,
							this
						)
					);
/////////////////////////////////////////////////////////////////////////////////////
// Blurry text problem fix problem reported by Scott Sanders
//	OLD CODE:	int nOldBkMode = dc.SetBkMode( TRANSPARENT );
bool bSelMode = false;
			if(		( lpDIS->itemState & ODS_SELECTED ) != 0 
				&&	(dwStyle & LBS_NOSEL) == 0 
				)
				bSelMode = true;
int nOldBkMode = dc.SetBkMode( OPAQUE );
dc.SetBkColor ( PmBridge_GetPM()->GetColor( bSelMode ? COLOR_HIGHLIGHT : COLOR_WINDOW, this ) );
/////////////////////////////////////////////////////////////////////////////////////

			CRect rcText( rcItem );
			UINT nFormat = 
					DT_SINGLELINE
				|	DT_LEFT
				|	DT_NOPREFIX
				;
			if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
				nFormat |= DT_EXPANDTABS;

			CRect rcMeasureText(0,0,0,0);
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(sText), sText.GetLength(),
				rcMeasureText,
				(nFormat|DT_CALCRECT), 0
				);

			rcText.DeflateRect( 
				0,
				(rcText.Height() - rcMeasureText.Height()) / 2 
				);
			CRect rcMargins = OnQueryItemMargins( nItem );
			rcText.OffsetRect( rcMargins.left, 0 );
			
			if( bSelMode )
				dc.FillSolidRect(
					&rcItem,
					PmBridge_GetPM()->GetColor( COLOR_HIGHLIGHT, this )
					);

			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(sText), &rcText,
				nFormat, 0
				);

			dc.SetBkMode( nOldBkMode );
			dc.SetTextColor( clrTextOld );

			if(		( lpDIS->itemState & ODS_SELECTED ) != 0 
				&&	(dwStyle & LBS_NOSEL) == 0 
				)
			{
				COLORREF clrTextOld = 
					dc.SetTextColor( RGB(255,255,255) );
				COLORREF clrBkOld =
					dc.SetBkColor( RGB(0,0,0) );
				dc.DrawFocusRect( &rcItem );
				dc.SetBkColor( clrBkOld );
				dc.SetTextColor( clrTextOld );
			}
			
			dc.SelectObject( pOldFont );
		} // if( ! sText.IsEmpty() )
	} // if( nItem >= 0 )
	dc.Detach();
}

void CExtCheckListWnd::MeasureItem( LPMEASUREITEMSTRUCT lpMIS ) 
{
	ASSERT_VALID( this );
	ASSERT( lpMIS != NULL );
	
	INT nItem = (INT)lpMIS->itemID;
	if( nItem < 0 || nItem >= GetCount() )
		return;

	lpMIS->itemWidth = 0; 
	lpMIS->itemHeight = 15;

	CWindowDC dc( this );
	if(		(GetStyle() & LBS_HASSTRINGS) != 0
		&&	nItem >= 0 
		)
	{
		CString sText;
		GetText( nItem, sText );
		if( ! sText.IsEmpty() )
		{
			HFONT hFont = OnQueryItemFont( nItem );
			ASSERT( hFont != NULL );
			UINT nFormat = 
					DT_SINGLELINE
				|	DT_LEFT
				|	DT_NOPREFIX
				;
			if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
				nFormat |= DT_EXPANDTABS;
			CRect rcMeasure =
				CExtPaintManager::stat_CalcTextDimension( 
					dc, 
					( * CFont::FromHandle( hFont ) ), 
					CExtSafeString(LPCTSTR(sText)),
					nFormat
					);
			lpMIS->itemWidth = rcMeasure.Width();
			lpMIS->itemHeight = rcMeasure.Height();
		} // if( ! sText.IsEmpty() )
	} // if( nItem >= 0 )

	CSize szCheck = 
		OnQueryItemCheckSize( dc, nItem );
	ASSERT( szCheck.cx >= 0 && szCheck.cy >= 0 );
	CRect rcCheckMargins = 
		OnQueryItemCheckMargins( nItem );
	szCheck.cx += (rcCheckMargins.left + rcCheckMargins.right);
	szCheck.cy += (rcCheckMargins.top + rcCheckMargins.bottom);

	lpMIS->itemWidth += szCheck.cx;
	lpMIS->itemHeight = max( UINT(szCheck.cy), lpMIS->itemHeight );

	CRect rcMargins = 
		OnQueryItemMargins( nItem );
	
	lpMIS->itemWidth += (rcMargins.left + rcMargins.right);
	lpMIS->itemHeight += (rcMargins.top + rcMargins.bottom);
}

int CExtCheckListWnd::CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct ) 
{
	ASSERT_VALID( this );
	COMPAREITEMSTRUCT compareItem;
	__EXT_MFC_MEMCPY( 
		&compareItem, 
		sizeof(COMPAREITEMSTRUCT),
		lpCompareItemStruct, 
		sizeof(COMPAREITEMSTRUCT) 
		);
	if(		compareItem.itemData1 != 0 
		&&	compareItem.itemData1 != LB_ERR
		)
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) compareItem.itemData1;
		compareItem.itemData1 = pState->m_dwUserData;
	}
	if(		compareItem.itemData2 != 0 
		&&	compareItem.itemData2 != LB_ERR
		)
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) compareItem.itemData2;
		compareItem.itemData2 = pState->m_dwUserData;
	}
	return CListBox::CompareItem( &compareItem );
}

void CExtCheckListWnd::DeleteItem( LPDELETEITEMSTRUCT lpDeleteItemStruct ) 
{
	ASSERT_VALID( this );
	DELETEITEMSTRUCT deleteItem;
	__EXT_MFC_MEMCPY( 
		&deleteItem, 
		sizeof(DELETEITEMSTRUCT),
		lpDeleteItemStruct, 
		sizeof(DELETEITEMSTRUCT) 
		);

	// WINBUG: The following if block is required because Windows NT
	// version 3.51 does not properly fill out the LPDELETEITEMSTRUCT.
	if( deleteItem.itemData == 0 )
	{
		LRESULT lResult = DefWindowProc( LB_GETITEMDATA, deleteItem.itemID, 0 );
		if( lResult != LB_ERR )
			deleteItem.itemData = (UINT)lResult;
	}

	if(		deleteItem.itemData != 0 
		&&	deleteItem.itemData != LB_ERR
		)
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) deleteItem.itemData;
		deleteItem.itemData = pState->m_dwUserData;
		delete pState;
	}
	CListBox::DeleteItem( &deleteItem );
}

INT CExtCheckListWnd::CheckBoxFromPoint(
	CPoint point, 
	BOOL & bInCheck
	)
{
	ASSERT_VALID( this );
	bInCheck = FALSE;
	INT nIndex = -1;
	for( INT nItem = GetTopIndex(); nItem < GetCount(); nItem++ )
	{
		CRect rcItem;
		GetItemRect( nItem, &rcItem );
		if( rcItem.PtInRect( point ) )
		{
			CWindowDC dc( this );
			CSize szCheck = 
				OnQueryItemCheckSize( dc, nItem );
			ASSERT( szCheck.cx >= 0 && szCheck.cy >= 0 );
			CRect rcCheckMargins = 
				OnQueryItemCheckMargins( nItem );
			szCheck.cx += (rcCheckMargins.left + rcCheckMargins.right);
			szCheck.cy += (rcCheckMargins.top + rcCheckMargins.bottom);
		
			nIndex = nItem;
			if( point.x < rcItem.left + szCheck.cx )
				bInCheck = !bInCheck;
			break;
		}
	}
	return nIndex;
}

void CExtCheckListWnd::SetSelectionCheck( 
	INT nCheck 
	)
{
	ASSERT_VALID( this );
	INT nSelectedItems = GetSelCount();
	if( nSelectedItems > 0 )
	{
		LPINT pnSelectedItems = new INT[ nSelectedItems * sizeof(INT) ];
		GetSelItems( 
			nSelectedItems, 
			pnSelectedItems 
			);
		for( INT nSelectedItem = 0; nSelectedItem < nSelectedItems; nSelectedItem++ )
		{
			if( IsEnabled( pnSelectedItems[ nSelectedItem ] ) )
			{
				SetCheck( pnSelectedItems[ nSelectedItem ], nCheck );
				Invalidate();
			}
		}
		delete [] pnSelectedItems;
		pnSelectedItems = NULL;
	}
}

void CExtCheckListWnd::OnLButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	SetFocus();

	// determine where the click is
	BOOL bInCheck = FALSE;
	INT nIndex = CheckBoxFromPoint( point, bInCheck );

	// toggle the check mark automatically if the check mark was hit
	if(		bInCheck 
		&&	IsEnabled( nIndex )
		)
	{
		CWnd * pParent = GetParent();
		ASSERT_VALID( pParent );

		INT nModulo = (m_nStyle == BS_AUTO3STATE) ? 3 : 2;
		INT nCheck = GetCheck( nIndex );
		nCheck = (nCheck == nModulo) ? nCheck - 1 : nCheck;
		INT nNewCheck = (nCheck + 1) % nModulo;
		SetCheck( nIndex, nNewCheck );
		Invalidate();

		if(		( GetStyle() & (LBS_EXTENDEDSEL|LBS_MULTIPLESEL) ) != 0
			&&	GetSel( nIndex ) > 0
			)
		{
			// The listbox is a multi-select listbox, and the user clicked on
			// a selected check, so change the check on all of the selected
			// items.
			SetSelectionCheck( nNewCheck );
		}
		else
		{
			CListBox::OnLButtonDown( nFlags, point );
		}
		// Inform parent of check
		pParent->SendMessage( 
			WM_COMMAND, 
			MAKEWPARAM( GetDlgCtrlID(), CLBN_CHKCHANGE ), 
			(LPARAM)m_hWnd 
			);
		return;
	} // if( bInCheck )

	// do default listbox selection logic
	CListBox::OnLButtonDown( nFlags, point );
}

void CExtCheckListWnd::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	BOOL bInCheck = FALSE;
INT nHitIndex = CheckBoxFromPoint( point, bInCheck );
	if( bInCheck )
	{
		// Double and single clicks act the same on the check box
		OnLButtonDown( nFlags, point );
		return;
	}
	if( nHitIndex >= 0 && IsEnabled( nHitIndex ) )
	{
		CWnd * pParent = GetParent();
		ASSERT_VALID( pParent );

		INT nModulo = (m_nStyle == BS_AUTO3STATE) ? 3 : 2;
		INT nCheck = GetCheck( nHitIndex );
		nCheck = (nCheck == nModulo) ? nCheck - 1 : nCheck;
		INT nNewCheck = (nCheck + 1) % nModulo;
		SetCheck( nHitIndex, nNewCheck );
		Invalidate();

		if(		( GetStyle() & (LBS_EXTENDEDSEL|LBS_MULTIPLESEL) ) != 0
			&&	GetSel( nHitIndex ) > 0
			)
		{
			// The listbox is a multi-select listbox, and the user clicked on
			// a selected check, so change the check on all of the selected
			// items.
			SetSelectionCheck( nNewCheck );
		}
		else
		{
			CListBox::OnLButtonDblClk( nFlags, point );
		}
		// Inform parent of check
		pParent->SendMessage( 
			WM_COMMAND, 
			MAKEWPARAM( GetDlgCtrlID(), CLBN_CHKCHANGE ), 
			(LPARAM)m_hWnd 
			);
		return;
	} // if( nHitIndex >= 0 && IsEnabled( nHitIndex ) )
	CListBox::OnLButtonDblClk( nFlags, point );
}

void CExtCheckListWnd::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	if( nChar == VK_SPACE )
	{
		CWnd * pParent = GetParent();
		ASSERT_VALID( pParent );

		INT nIndex = GetCaretIndex();
		if( nIndex != LB_ERR )
		{
			if( (GetStyle() & LBS_MULTIPLESEL) != 0 )
			{
				if( IsEnabled( nIndex ) )
				{
					BOOL bSelected = GetSel( nIndex ) > 0 ? TRUE : FALSE;
					if( bSelected )
					{
						INT nModulo = (m_nStyle == BS_AUTO3STATE) ? 3 : 2;
						INT nCheck = GetCheck( nIndex );
						nCheck = (nCheck == nModulo) ? nCheck - 1 : nCheck;
						SetCheck( nIndex, (nCheck + 1) % nModulo );
						// Inform of check
						pParent->SendMessage(
							WM_COMMAND,
							MAKEWPARAM(GetDlgCtrlID(), CLBN_CHKCHANGE),
							(LPARAM)m_hWnd
							);
					}
					SetSel( nIndex, !bSelected );
				}
				else
					SetSel( nIndex, FALSE ); // unselect disabled items
				return;
			}
			else
			{
				// If there is a selection, the space bar toggles that check,
				// all other keys are the same as a standard listbox.
				if( IsEnabled( nIndex ) )
				{
					INT nModulo = (m_nStyle == BS_AUTO3STATE) ? 3 : 2;
					INT nCheck = GetCheck(nIndex);
					nCheck = (nCheck == nModulo) ? nCheck - 1 : nCheck;
					INT nNewCheck = (nCheck + 1) % nModulo;
					SetCheck( nIndex, nNewCheck );
					Invalidate();

					if( (GetStyle() & LBS_EXTENDEDSEL) != 0 )
					{
						// The listbox is a multi-select listbox, and the user
						// clicked on a selected check, so change the check on all
						// of the selected items.
						SetSelectionCheck( nNewCheck );
					}
					// Inform of check
					pParent->SendMessage(
						WM_COMMAND,
						MAKEWPARAM(GetDlgCtrlID(), CLBN_CHKCHANGE),
						(LPARAM)m_hWnd
						);
				}
				else
					SetSel( nIndex, FALSE ); // unselect disabled items
				return;
			}
		} // if( nIndex != LB_ERR )
	} // if( nChar == VK_SPACE )
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

LRESULT CExtCheckListWnd::OnLBAddString(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	EXT_ITEM_DATA * pState = NULL;
	if( (GetStyle() & LBS_HASSTRINGS) == 0 )
	{
		pState = new EXT_ITEM_DATA;
		pState->m_dwUserData = (DWORD)lParam;
		lParam = (LPARAM) pState;
	}
	LRESULT lResult = DefWindowProc( LB_ADDSTRING, wParam, lParam );
	if(		lResult == LB_ERR 
		&&	pState != NULL 
		)
		delete pState;
	return lResult;
}

LRESULT CExtCheckListWnd::OnLBFindString(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	if( (GetStyle() & LBS_HASSTRINGS) != 0 )
		return DefWindowProc( LB_FINDSTRING, wParam, lParam );
	INT nIndex = (INT)wParam;
	if( nIndex == -1 ) 
		nIndex = 0;
	for( ; nIndex < GetCount(); nIndex++ )
		if( (DWORD)lParam == GetItemData( nIndex ) )
			return nIndex;
	return LB_ERR;
}

LRESULT CExtCheckListWnd::OnLBFindStringExact(WPARAM wParam, LPARAM lParam)
{
	if( (GetStyle() & (LBS_HASSTRINGS | LBS_SORT)) != 0 )
		return DefWindowProc( LB_FINDSTRINGEXACT, wParam, lParam );
	INT nIndex = (INT)wParam;
	if( nIndex == -1 ) 
		nIndex = 0;
	for( ; nIndex < GetCount(); nIndex++ )
		if( (DWORD)lParam == GetItemData( nIndex ) )
			return nIndex;
	return LB_ERR;
}

LRESULT CExtCheckListWnd::OnLBGetItemData(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc( LB_GETITEMDATA, wParam, lParam );
	if( lResult != LB_ERR )
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) lResult;
		if( pState == NULL )
			return 0;
		lResult = pState->m_dwUserData;
	}
	return lResult;
}

LRESULT CExtCheckListWnd::OnLBGetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc( LB_GETTEXT, wParam, lParam);
	if( (GetStyle() & LBS_HASSTRINGS) != 0 )
		return lResult;
	if( lResult != LB_ERR )
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*) lParam;
		if( pState != NULL )
			lParam = pState->m_dwUserData;
	}
	return lResult;
}

LRESULT CExtCheckListWnd::OnLBInsertString(WPARAM wParam, LPARAM lParam)
{
	EXT_ITEM_DATA * pState = NULL;
	if( (GetStyle() & LBS_HASSTRINGS) == 0 )
	{
		pState = new EXT_ITEM_DATA;
		pState->m_dwUserData = (DWORD)lParam;
		lParam = (LPARAM) pState;
	}
	LRESULT lResult = DefWindowProc( LB_INSERTSTRING, wParam, lParam);
	if( lResult == LB_ERR && pState != NULL )
		delete pState;
	return lResult;
}

LRESULT CExtCheckListWnd::OnLBSelectString(WPARAM wParam, LPARAM lParam)
{
	if( (GetStyle() & LBS_HASSTRINGS) != 0 )
		return DefWindowProc( LB_SELECTSTRING, wParam, lParam );
	INT nIndex = (INT)wParam;
	if( nIndex == -1 ) 
		nIndex = 0;
	for( ; nIndex < GetCount(); nIndex++ )
		if( (DWORD)lParam == GetItemData( nIndex ) )
		{
			SetCurSel( nIndex );
			return nIndex;
		}
	return LB_ERR;
}

LRESULT CExtCheckListWnd::OnLBSetItemData(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc( LB_GETITEMDATA, wParam, 0 );
	if( lResult != LB_ERR )
	{
		EXT_ITEM_DATA * pState = (EXT_ITEM_DATA*)lResult;
		if(pState == NULL)
			pState = new EXT_ITEM_DATA;
		pState->m_dwUserData = (DWORD)lParam;
		lResult = DefWindowProc( LB_SETITEMDATA, wParam, (LPARAM) pState );
		if( lResult == LB_ERR )
			delete pState;
	}
	return lResult;
}

#endif // (!defined __EXT_MFC_NO_CHECK_LIST)