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

#if (!defined __EXT_BUTTON_H)
	#include <ExtButton.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXTCOLORDLG_H)
	#include <ExtColorDlg.h>
#endif

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __EXT_CONTROLS_COMMON_H)
	#include <ExtControlsCommon.h>
#endif

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif

#include <intshcut.h>

#include <Resources/Resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE( CExtButton, CButton );

CExtButton::CExtButton()
	: m_nButtonType( BS_PUSHBUTTON )
	, m_nCheck( BST_UNCHECKED )
	, m_hCursor( NULL )
	, m_bDrawBorder( true )
	, m_bDrawFocusRect( true )
	, m_bSeparatedDropDown( false )
	, m_bAutoChangeID( false )
	, m_bAutoChangeCmdIcon( false )
	, m_bMouseOver( false )
	, m_bPushed( false )
	, m_bState( false )
	, m_bPushTracking( false )
	, m_bKeyTracking( false )
	, m_bDefaultButton( false )
	, m_nTrackingHT( 0 )
	, m_nIconAlignment( -1 )
	, m_rcIconMargins( 3, 3, 3, 3 )
	, m_rcTextMargins( 3, 3, 3, 3 )
	, m_bNoRefToCmdMngr( false )
	, m_dwMenuOpt( TPMX_TOPALIGN )
	, m_nMenuLeftAreaWidth( 0 )
	, m_clrTextDisabled( ((COLORREF)-1L) )
	, m_clrTextNormal( ((COLORREF)-1L) )
	, m_clrTextHover( ((COLORREF)-1L) )
	, m_clrTextPressed( ((COLORREF)-1L) )
	, m_clrBackground( ((COLORREF)-1L) )
	, m_bQuickActivationEnabled( true )
	, m_bQuickActivationSeparatedDropDown( true )
	, m_bQuickActivationCancelNearest( false )
	, m_nAdvancedTipStyle( INT(CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON) )
	, m_bToolTipActive( false )
	, m_nFocusMode( CExtPaintManager::__EBFM_CAPTION_AREA )
	, m_bIconOnly( false ) // STAR - icon only 
{
	EnableToolTips();
}

CExtButton::~CExtButton()
{
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS != NULL )
		pAcAS->AnimationSite_ClientRemove( this );
}

BEGIN_MESSAGE_MAP(CExtButton, CButton)
    //{{AFX_MSG_MAP(CExtButton)
	ON_WM_CAPTURECHANGED()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_ENABLE()
	ON_WM_CANCELMODE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
	ON_REGISTERED_MESSAGE(
		CExtToolControlBar::g_nMsgPutToPopupMenu,
		_OnPutToPopupMenuImpl
		)
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
END_MESSAGE_MAP()

LRESULT CExtButton::_OnPutToPopupMenuImpl(WPARAM wParam, LPARAM lParam)
{
	lParam;
CExtPopupMenuWnd * pPopup = reinterpret_cast < CExtPopupMenuWnd * > (wParam);
	ASSERT_VALID( pPopup );
	return OnPutToPopupMenu( pPopup ) ? TRUE : FALSE;
}

void CExtButton::OnSize(UINT nType, int cx, int cy) 
{
	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rectButton;
	GetClientRect( &rectButton );
	AnimationClient_TargetRectSet( rectButton );

	CButton::OnSize( nType, cx, cy );
}

bool CExtButton::OnPutToPopupMenu( CExtPopupMenuWnd * pPopup )
{
	ASSERT_VALID( pPopup );

//bool bInBar = false;
//CWnd * pWnd = GetParent();
//	ASSERT_VALID( pWnd );
//	if( pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)) )
//	{
//		bInBar = true;
//		pWnd = pWnd->GetParentFrame();
//		ASSERT_VALID( pWnd );
//		if( pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
//		{
//			pWnd = pWnd->GetParentFrame();
//			ASSERT_VALID( pWnd );
//		}
//	}
	
	if( m_menu.GetSafeHmenu() != NULL )
	{
		ASSERT( ::IsMenu( m_menu.GetSafeHmenu() ) );
		CExtPopupMenuWnd * pPopupIns =
			CExtPopupMenuWnd::InstantiatePopupMenu(
				m_hWnd,
				RUNTIME_CLASS(CExtPopupMenuWnd),
				this
				);
		VERIFY(
			pPopupIns->CreatePopupMenu(
				//pWnd->GetSafeHwnd()
				pPopup->GetCmdReceiverHWND()
				)
			);
		VERIFY(
			pPopupIns->UpdateFromMenu(
				pPopup->GetCmdReceiverHWND(),
				&m_menu
				)
			);
		CExtSafeString sPopupText( _T("") );
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
				GetDlgCtrlID()
				);
		if( pCmdItem != NULL )
			sPopupText = pCmdItem->m_sMenuText;
		if( sPopupText.IsEmpty() )
			OnQueryWindowText( sPopupText );
		VERIFY(
			pPopup->ItemInsertSpecPopup(
				pPopupIns,
				-1,
				(LPCTSTR)sPopupText,
				m_icon
				)
			);
		return true;
	}

	return false;
}

// for back compatibility
void CExtButton::SetIcon(
	__EXT_MFC_SAFE_LPCTSTR sResourceIconInID,
	__EXT_MFC_SAFE_LPCTSTR sResourceIconOutID, // = NULL
	__EXT_MFC_SAFE_LPCTSTR sResourceIconDisabledID, // = NULL
	__EXT_MFC_SAFE_LPCTSTR sResourceIconPressedID, // = NULL
	INT nWidth, // = 0
	INT nHeight // = 0
	)
{
	if( nWidth < 0 )
		nWidth = 0;
	if( nHeight < 0 )
		nHeight = 0;
HICON hIconIn = NULL;
HICON hIconOut = NULL;
HICON hIconDisabled = NULL;
HICON hIconPressed = NULL;
	if( sResourceIconInID != NULL )
	{
		HINSTANCE hInstResource =
			::AfxFindResourceHandle(
				sResourceIconInID,
				RT_GROUP_ICON
				);
		if( hInstResource != NULL )
			hIconIn = (HICON)
				::LoadImage(
					hInstResource,
					sResourceIconInID,
					IMAGE_ICON,
					nWidth,
					nHeight,
					0
					);
	}
	if( sResourceIconOutID != NULL )
	{
		HINSTANCE hInstResource =
			AfxFindResourceHandle(
				sResourceIconOutID,
				RT_GROUP_ICON
				);
		if( hInstResource != NULL )
			hIconOut = (HICON)
				::LoadImage(
					hInstResource,
					sResourceIconOutID,
					IMAGE_ICON,
					nWidth,
					nHeight,
					0
					);
	}
	if( sResourceIconDisabledID != NULL )
	{
		HINSTANCE hInstResource =
			AfxFindResourceHandle(
				sResourceIconDisabledID,
				RT_GROUP_ICON
				);
		if( hInstResource != NULL )
			hIconDisabled = (HICON)
				::LoadImage(
					hInstResource,
					sResourceIconDisabledID,
					IMAGE_ICON,
					nWidth,
					nHeight,
					0
					);
	}
	if( sResourceIconPressedID != NULL )
	{
		HINSTANCE hInstResource =
			AfxFindResourceHandle(
				sResourceIconPressedID,
				RT_GROUP_ICON
				);
		if( hInstResource != NULL )
			hIconPressed = (HICON)
				::LoadImage(
					hInstResource,
					sResourceIconPressedID,
					IMAGE_ICON,
					nWidth,
					nHeight,
					0
					);
	} // if( hInstResource != NULL )
	SetIcon(
		hIconIn,
		hIconOut,
		hIconDisabled,
		hIconPressed
		);
}

// for back compatibility
void CExtButton::SetIcon(
	UINT nResourceIconInID,
	UINT nResourceIconOutID, // = 0
	UINT nResourceIconDisabledID, // = 0
	UINT nResourceIconPressedID, // = 0
	INT nWidth, // = 0
	INT nHeight // = 0
	)
{
	SetIcon(
		( nResourceIconInID != 0 )
			? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE(nResourceIconInID) )
			: __EXT_MFC_SAFE_LPCTSTR( NULL ),
		( nResourceIconOutID != 0 )
			? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE(nResourceIconOutID) )
			: __EXT_MFC_SAFE_LPCTSTR( NULL ),
		( nResourceIconDisabledID != 0 )
			? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE(nResourceIconDisabledID) )
			: __EXT_MFC_SAFE_LPCTSTR( NULL ),
		( nResourceIconPressedID != 0 )
			? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE(nResourceIconPressedID) )
			: __EXT_MFC_SAFE_LPCTSTR( NULL ),
		nWidth,
		nHeight
		);
}

// for back compatibility
void CExtButton::SetIcon(
	HICON hIconIn,
	HICON hIconOut, // = NULL
	HICON hIconDisabled, // = NULL
	HICON hIconPressed // = NULL
	)
{
	m_icon.Empty();
	if(		hIconIn == NULL
		&&	hIconOut == NULL
		&&	hIconDisabled == NULL
		&&	hIconPressed == NULL
		)
		return;
	if( hIconIn != NULL && hIconOut != NULL )
	{
		m_icon.m_bmpHover.AssignFromHICON( hIconIn );
		m_icon.m_dwFlags |= __EXT_ICON_PERSISTENT_BITMAP_HOVER;
	} // if( hIconIn != NULL && hIconOut != NULL )
	if( hIconOut != NULL || hIconIn != NULL )
	{
		m_icon.m_bmpNormal.AssignFromHICON(
			( hIconOut != NULL ) ? hIconOut : hIconIn
			);
	} // if( hIconOut != NULL || hIconIn != NULL )
	if( hIconIn != NULL )
		::DestroyIcon( hIconIn );
	if( hIconOut != NULL )
			::DestroyIcon( hIconOut );
	if( hIconDisabled != NULL )
	{
		m_icon.m_bmpDisabled.AssignFromHICON( hIconDisabled );
		m_icon.m_dwFlags |= __EXT_ICON_PERSISTENT_BITMAP_DISABLED;
		::DestroyIcon( hIconDisabled );
	} // if( hIconDisabled != NULL )
	if( hIconPressed != NULL )
	{
		m_icon.m_bmpPressed.AssignFromHICON( hIconPressed );
		m_icon.m_dwFlags |= __EXT_ICON_PERSISTENT_BITMAP_PRESSED;
		::DestroyIcon( hIconPressed );
	} // if( hIconPressed != NULL )
	if( GetSafeHwnd() != NULL )
		Invalidate();
} 

BOOL CExtButton::SetBtnCursor(int nCursorId)
{
HINSTANCE hInstResource;
	if( m_hCursor != NULL )
	{
		::DestroyCursor( m_hCursor );
		m_hCursor = NULL;
	}
	if( nCursorId != -1 )
	{
		hInstResource =
			::AfxFindResourceHandle(
				MAKEINTRESOURCE( nCursorId ),
				RT_GROUP_CURSOR
				);
		m_hCursor = (HCURSOR)
			::LoadImage(
				hInstResource,
				MAKEINTRESOURCE( nCursorId ),
				IMAGE_CURSOR,
				0,
				0,
				0
				);
		if( m_hCursor == NULL )
		{
			m_hCursor =
				::LoadCursor(
					NULL,
					MAKEINTRESOURCE( nCursorId )
					);
			if( m_hCursor == NULL )
				return FALSE;
		}
	}
	return TRUE;
}

void CExtButton::SetFlat( BOOL bState )
{
	if( GetSafeHwnd() != NULL )
	{
		ModifyStyle(
			bState ? 0 : BS_FLAT,
			bState ? BS_FLAT : 0,
			SWP_FRAMECHANGED
			);
		Invalidate();
	}
}

BOOL CExtButton::GetFlat() const
{
	return ( (GetStyle()&BS_FLAT) != 0 ) ? TRUE : FALSE;
}

BOOL CExtButton::GetDrawBorder() const
{
	return m_bDrawBorder ? TRUE : FALSE;
}

void CExtButton::SetDrawBorder( BOOL bEnable )
{
	m_bDrawBorder = bEnable ? true : false;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

BOOL CExtButton::GetDrawFocusRect() const
{
	return m_bDrawFocusRect ? TRUE : FALSE;
}

void CExtButton::SetDrawFocusRect( BOOL bEnable )
{
	m_bDrawFocusRect = bEnable ? true : false;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

BOOL CExtButton::GetSeparatedDropDown() const
{
	return m_bSeparatedDropDown ? TRUE : FALSE;
}

void CExtButton::SetSeparatedDropDown(
	BOOL bSeparatedDropDown // = TRUE
	)
{
	m_bSeparatedDropDown = bSeparatedDropDown ? true : false;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

BOOL CExtButton::GetAutoChangeID() const
{
	return m_bAutoChangeID;
}

void CExtButton::SetAutoChangeID(
	BOOL bAutoChangeID // = TRUE
	)
{
	m_bAutoChangeID = bAutoChangeID ? true : false;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

BOOL CExtButton::GetAutoChangeCmdIcon() const
{
	return m_bAutoChangeCmdIcon ? TRUE : FALSE;
}

void CExtButton::SetAutoChangeCmdIcon(
	BOOL bAutoChangeCmdIcon // = TRUE
	)
{
	m_bAutoChangeCmdIcon = bAutoChangeCmdIcon ? true : false;
}

void CExtButton::SetTextColorDisabled( COLORREF clrText )
{ 
	ASSERT_VALID( this );
	m_clrTextDisabled = clrText; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtButton::SetTextColorNormal( COLORREF clrText )
{ 
	ASSERT_VALID( this );
	m_clrTextNormal = clrText; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtButton::SetTextColorHover( COLORREF clrText )
{ 
	ASSERT_VALID( this );
	m_clrTextHover = clrText; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtButton::SetTextColorPressed( COLORREF clrText )
{ 
	ASSERT_VALID( this );
	m_clrTextPressed = clrText; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtButton::SetBkColor( COLORREF clrBk )
{ 
	ASSERT_VALID( this );
	m_clrBackground = clrBk; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

COLORREF CExtButton::GetTextColorDisabled() const
{
	ASSERT_VALID( this );
	return m_clrTextDisabled; 
}

COLORREF CExtButton::GetTextColorNormal() const
{ 
	ASSERT_VALID( this );
	return m_clrTextNormal; 
}

COLORREF CExtButton::GetTextColorHover() const
{
	ASSERT_VALID( this );
	return m_clrTextHover; 
}

COLORREF CExtButton::GetTextColorPressed() const
{ 
	ASSERT_VALID( this );
	return m_clrTextPressed; 
}

COLORREF CExtButton::GetBkColor() const
{ 
	ASSERT_VALID( this );
	return m_clrBackground; 
}

void CExtButton::SetTextAlignment( 
	INT nAlignment 
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
DWORD dwStyle = 0;
	if( (nAlignment&CExtPaintManager::__ALIGN_HORIZ_MASK) == CExtPaintManager::__ALIGN_HORIZ_RIGHT )
		dwStyle |= BS_RIGHT;
	else if( (nAlignment&CExtPaintManager::__ALIGN_HORIZ_MASK) == CExtPaintManager::__ALIGN_HORIZ_CENTER )
		dwStyle |= BS_CENTER;
	else  if( (nAlignment&CExtPaintManager::__ALIGN_HORIZ_MASK) == CExtPaintManager::__ALIGN_HORIZ_LEFT )
		dwStyle |= BS_LEFT;
	
	if( (nAlignment&CExtPaintManager::__ALIGN_VERT_MASK) == CExtPaintManager::__ALIGN_VERT_BOTTOM )
		dwStyle |= BS_BOTTOM;
	else if( (nAlignment&CExtPaintManager::__ALIGN_VERT_MASK) == CExtPaintManager::__ALIGN_VERT_TOP )
		dwStyle |= BS_TOP;
	else
		dwStyle |= BS_VCENTER;

const DWORD __BS_H_MASK = (BS_LEFT|BS_CENTER|BS_RIGHT);
const DWORD __BS_V_MASK = (BS_TOP|BS_VCENTER|BS_BOTTOM);
	ModifyStyle( 
		__BS_H_MASK | __BS_V_MASK, 
		dwStyle
		);
	
	Invalidate();
}

INT CExtButton::GetTextAlignment() const
{
	ASSERT_VALID( this );
	return OnQueryAlignmentText();
}

void CExtButton::SetIconAlignment( 
	INT nAlignment 
	)
{
	ASSERT_VALID( this );
	m_nIconAlignment = nAlignment;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

INT CExtButton::GetIconAlignment() const
{
	ASSERT_VALID( this );
	return OnQueryAlignmentIcon();
}

void CExtButton::SetTextMargins(
	INT nLeft,
	INT nTop,
	INT nRight,
	INT nBottom
	)
{
	ASSERT_VALID( this );
	m_rcTextMargins.SetRect(
		nLeft,
		nTop,
		nRight,
		nBottom
		);
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtButton::SetTextMargins(
	const RECT & rcMargins
	)
{
	ASSERT_VALID( this );
	SetTextMargins(
		rcMargins.left,
		rcMargins.top,
		rcMargins.right,
		rcMargins.bottom
		);
}

void CExtButton::SetTextMargins(
	LPCRECT lprcMargins
	)
{
	ASSERT_VALID( this );
	if( lprcMargins == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	SetTextMargins(
		lprcMargins->left,
		lprcMargins->top,
		lprcMargins->right,
		lprcMargins->bottom
		);
}

void CExtButton::SetIconMargins(
	INT nLeft,
	INT nTop,
	INT nRight,
	INT nBottom
	)
{
	ASSERT_VALID( this );
	m_rcIconMargins.SetRect(
		nLeft,
		nTop,
		nRight,
		nBottom
		);
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtButton::SetIconMargins(
	const RECT & rcMargins
	)
{
	ASSERT_VALID( this );
	SetIconMargins(
		rcMargins.left,
		rcMargins.top,
		rcMargins.right,
		rcMargins.bottom
		);
}

void CExtButton::SetIconMargins(
	LPCRECT lprcMargins
	)
{
	ASSERT_VALID( this );
	if( lprcMargins == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	SetIconMargins(
		lprcMargins->left,
		lprcMargins->top,
		lprcMargins->right,
		lprcMargins->bottom
		);
}

void CExtButton::GetTextMargins(
	INT & nLeft,
	INT & nTop,
	INT & nRight,
	INT & nBottom
	) const
{
	ASSERT_VALID( this );
CRect rcTextMargins = GetTextMargins();
	nLeft = rcTextMargins.left;
	nTop = rcTextMargins.top;
	nRight = rcTextMargins.right;
	nBottom = rcTextMargins.bottom;
}

CRect CExtButton::GetTextMargins() const
{
	ASSERT_VALID( this );
	return m_rcTextMargins;
}

void CExtButton::GetIconMargins(
	INT & nLeft,
	INT & nTop,
	INT & nRight,
	INT & nBottom
	) const
{
	ASSERT_VALID( this );
CRect rcIconMargins = GetTextMargins();
	nLeft = rcIconMargins.left;
	nTop = rcIconMargins.top;
	nRight = rcIconMargins.right;
	nBottom = rcIconMargins.bottom;
}

CRect CExtButton::GetIconMargins() const
{
	ASSERT_VALID( this );
	return m_rcIconMargins;
}

INT CExtButton::GetFocusMode() const
{
	ASSERT_VALID( this );
	return m_nFocusMode;
}

INT CExtButton::SetFocusMode( 
	INT nFocusMode 
	)
{
	ASSERT_VALID( this );
INT nFocusModeOld = m_nFocusMode;
	m_nFocusMode = nFocusMode;
	if( GetSafeHwnd() != NULL )
		Invalidate();
	return nFocusModeOld;
}

LONG CExtButton::_CalcSplitButtonExtent() const
{
	ASSERT_VALID( this );
	return PmBridge_GetPM()->_CalcSplitButtonExtent( this, true );
}

void CExtButton::_CalcClientRects(
	CRect & rcMainArea,
	CRect & rcDropDown
	) const
{
	ASSERT_VALID( this );
	GetClientRect( &rcMainArea );
	rcDropDown = rcMainArea;
	rcDropDown.left = rcDropDown.right;
	if( ! m_bSeparatedDropDown )
		return;
	rcDropDown.left -= _CalcSplitButtonExtent();
	rcMainArea.right = rcDropDown.left;
}

INT CExtButton::_HtMouseOnButton( const POINT & ptClient ) const // 0 - not, 1-main area, 2-dropdown area
{
CRect rcMainArea, rcDropDown;
	_CalcClientRects( rcMainArea, rcDropDown );
	if( rcMainArea.PtInRect(ptClient) )
		return 1;
	if( !m_bSeparatedDropDown )
		return 0;
	if( rcDropDown.PtInRect(ptClient) )
		return 2;
	return 0;
}

INT CExtButton::_HtMouseOnButton() const // 0 - not, 1-main area, 2-dropdown area
{
POINT point;
	if( ! ::GetCursorPos( &point ) )
		return 0;
HWND hWndUnderMouse = ::WindowFromPoint( point );
	if( hWndUnderMouse == NULL || hWndUnderMouse != m_hWnd )
		return false;
	if( !m_bSeparatedDropDown )
		return 1;
	ScreenToClient( &point );
	return _HtMouseOnButton( point );
}

void CExtButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bKeyTracking )
		return;

	CButton::OnMouseMove(nFlags, point);

bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
INT nTrackingHT = _HtMouseOnButton();
	if(		(	((!m_bPushTracking) && nTrackingHT!=0)
			||	( m_bPushTracking && nTrackingHT==m_nTrackingHT )
			)
		&&	CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( GetSafeHwnd() )
		)
	{ // if mouse is really on button
		if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
			CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
		if( m_bMouseOver )
			return;

		if( ! bAnimationLocked )
		{
			AnimationClient_CacheGeneratorLock();
//			if( AnimationClient_StateGet(true).IsEmpty() )
 				AnimationClient_CacheNextStateMinInfo(
					false,
					__EAPT_BY_HOVERED_STATE_TURNED_ON
					);
		}

		m_bMouseOver = true;
		m_bPushed =
			((nFlags & MK_LBUTTON) && m_bPushTracking )
				? true : false;

		if( ! bAnimationLocked )
		{
 			AnimationClient_CacheNextStateMinInfo(
				true,
				m_bPushed ? __EAPT_BY_PRESSED_STATE_TURNED_ON : __EAPT_BY_HOVERED_STATE_TURNED_ON
				);
			AnimationClient_CacheGeneratorUnlock();
		}

		Invalidate();
	} // if mouse is really on button
	else
	{ // if mouse is really NOT on button
		if( ! bAnimationLocked )
		{
			AnimationClient_CacheGeneratorLock();
//			if( AnimationClient_StateGet(true).IsEmpty() )
 				AnimationClient_CacheNextStateMinInfo(
					false,
					__EAPT_BY_HOVERED_STATE_TURNED_OFF
					);
		}

		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() && ( !m_bPushTracking ) )
			CExtMouseCaptureSink::ReleaseCapture();
//		if( ! m_bMouseOver )
//			return;

		bool bWasPushed = m_bPushed;
		m_bMouseOver = m_bPushed = false;

		if( ! bAnimationLocked )
		{
 			AnimationClient_CacheNextStateMinInfo(
				true,
				bWasPushed ? __EAPT_BY_PRESSED_STATE_TURNED_OFF : __EAPT_BY_HOVERED_STATE_TURNED_OFF
				);
			AnimationClient_CacheGeneratorUnlock();
		}

		Invalidate();
	} // if mouse is really NOT on button

	if(		m_nAdvancedTipStyle != INT(CExtPopupMenuTipWnd::__ETS_NONE)
		&&	m_bToolTipActive
		&&	( ! m_strToolTipText.IsEmpty() )
		&&	(! CExtPopupMenuWnd::IsMenuTracking() )
		)
	{
		CExtPopupMenuTipWnd * pATTW =
			OnAdvancedPopupMenuTipWndGet();
		if( pATTW != NULL )
		{
			CRect rcArea;
			GetWindowRect( &rcArea );
			OnAdvancedPopupMenuTipWndDisplay(
				*pATTW,
				rcArea
				);
		} // if( pATTW != NULL )
	}
}

void CExtButton::OnSetFocus( CWnd * pOldWnd )
{
	ASSERT_VALID( this );
	CButton::OnSetFocus( pOldWnd );
//	if( ( GetStyle() & BS_NOTIFY ) != 0 )
//		::SendMessage( ::GetParent( m_hWnd ), WM_COMMAND, MAKEWPARAM(WORD(GetDlgCtrlID()),BN_SETFOCUS), LPARAM(m_hWnd) );
}

void CExtButton::OnKillFocus( CWnd * pNewWnd )
{
	ASSERT_VALID( this );
	CButton::OnKillFocus( pNewWnd );
	SendMessage( WM_CANCELMODE );
	m_bState = false;
//	if( ( GetStyle() & BS_NOTIFY ) != 0 )
//		::SendMessage( ::GetParent( m_hWnd ), WM_COMMAND, MAKEWPARAM(WORD(GetDlgCtrlID()),BN_KILLFOCUS), LPARAM(m_hWnd) );
}

#if _MFC_VER < 0x700
void CExtButton::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtButton::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CButton::OnActivateApp(bActive, hTask);
	if( ! bActive )
		SendMessage( WM_CANCELMODE );
}

void CExtButton::OnCancelMode() 
{
	CButton::OnCancelMode();
	
bool bExitingPushedState = ( m_bPushed || m_bPushTracking ) ? true : false;
bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
	if( ! bAnimationLocked )
	{
		AnimationClient_CacheGeneratorLock();
//		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				bExitingPushedState ? __EAPT_BY_PRESSED_STATE_TURNED_OFF : __EAPT_BY_HOVERED_STATE_TURNED_OFF
				);
	}

	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();

	m_bMouseOver = false;
	m_bPushed = false;
	m_bPushTracking = false;
	m_bKeyTracking = false;

	if( ! bAnimationLocked )
	{
 		AnimationClient_CacheNextStateMinInfo(
			true,
			bExitingPushedState ? __EAPT_BY_PRESSED_STATE_TURNED_OFF : __EAPT_BY_HOVERED_STATE_TURNED_OFF
			);
		AnimationClient_CacheGeneratorUnlock();
	}

	Invalidate();
}

void CExtButton::OnCaptureChanged(CWnd *pWnd) 
{
	CButton::OnCaptureChanged( pWnd );
	if( CExtMouseCaptureSink::GetCapture() != m_hWnd )
		SendMessage( WM_CANCELMODE );
}

void CExtButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();
DWORD dwWndStyle = GetStyle();
DWORD dwWndType = dwWndStyle&0x0F;
	m_nButtonType = INT(dwWndType);
	m_bIconOnly = ( ( dwWndStyle & BS_ICON ) != 0 ) ? true : false;  // STAR - icon only
	if( m_bIconOnly ) // STAR
		m_nFocusMode = CExtPaintManager::__EBFM_CLIENT_AREA; // STAR
	SetButtonStyle( BS_OWNERDRAW );
	m_bDefaultButton = ( m_nButtonType == BS_DEFPUSHBUTTON ) ? true : false;
	m_nCheck = GetCheck();
bool bEnabled = OnQueryWindowEnabledState();
	PostMessage( // delayed repainting
		WM_ENABLE,
		(WPARAM) bEnabled ? TRUE : FALSE
		);
	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rcClient;
	GetClientRect( &rcClient );
	AnimationClient_TargetRectSet( rcClient );
	ModifyStyle( 0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN );

	m_bQuickActivationCancelNearest = false;
	if( m_bQuickActivationEnabled )
	{
		CString strText;
		GetWindowText( strText );
		if( ! strText.IsEmpty() )
		{
			int nAmpPos = strText.Find( _T('&') );
			if( ! ( nAmpPos < 0 || nAmpPos == (strText.GetLength()-1) ) )
			{
				TCHAR nCharOwn = strText.GetAt( nAmpPos + 1 );
				DWORD dwCharOwn = (DWORD)_totupper( nCharOwn );
//				BYTE lpKeyState[256];
//				::GetKeyboardState( lpKeyState );
				UINT wScanCode = ::MapVirtualKey( (UINT)dwCharOwn, 2 );
//				HKL hKeyboardLayout = ::GetKeyboardLayout( ( ::AfxGetThread() ) -> m_nThreadID );
//#if (defined _UNICODE)
//				TCHAR szChar[2] = { _T('\0'), _T('\0') };
//				::ToUnicodeEx( nCharOwn, wScanCode, lpKeyState, szChar, 1, 1, hKeyboardLayout );
//				WORD nMapped = WORD( szChar[0] );
//#else
//				WORD nMapped = 0;
//				::ToAsciiEx( nCharOwn, wScanCode, lpKeyState, &nMapped, 1, hKeyboardLayout );
//				//TCHAR szChar[2] = { (TCHAR)nMapped, _T('\0') };
//#endif
				if( CExtPopupMenuWnd::IsKeyPressed( wScanCode ) )
					m_bQuickActivationCancelNearest = true;
			} // if( ! ( nAmpPos < 0 || nAmpPos == (strText.GetLength()-1) ) )
		} // if( ! strText.IsEmpty() )
	} // if( m_bQuickActivationEnabled )
}

void CExtButton::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	ASSERT_VALID( this );
	m_icon.OnEmptyGeneratedBitmaps();
	CExtAnimationSingleton::PmBridge_OnPaintManagerChanged( pGlobalPM );
}

void CExtButton::AnimationSite_OnProgressShutdownTimer( UINT nTimerID )
{
	ASSERT_VALID( this );
	CExtAnimationSingleton::AnimationSite_OnProgressShutdownTimer( nTimerID );
}

HWND CExtButton::AnimationSite_GetSafeHWND() const
{
__PROF_UIS_MANAGE_STATE;
HWND hWnd = GetSafeHwnd();
	return hWnd;
}

const CExtAnimationParameters *
	CExtButton::AnimationClient_OnQueryAnimationParameters(
		INT eAPT // __EAPT_*** animation type
		) const
{
	ASSERT_VALID( this );
//return (&PmBridge_GetPM()->g_DefAnimationParametersVerySlow);
const CExtAnimationParameters * pAnimationParameters =
		PmBridge_GetPM()->Animation_GetParameters(
			eAPT,
			(CObject*)this,
			this
			);
	return pAnimationParameters;
}

bool CExtButton::AnimationClient_CacheNextState(
	CDC & dc,
	const RECT & rcAcAnimationTarget,
	bool bAnimate,
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( AnimationClient_CacheGeneratorIsLocked() );
	_RenderImpl( dc );
	return
		CExtAnimationSingleton::AnimationClient_CacheNextState(
			dc,
			rcAcAnimationTarget,
			bAnimate,
			eAPT
			);
}

BOOL CExtButton::PreTranslateMessage(MSG* pMsg)
{
	if(		(! CExtPopupMenuWnd::IsMenuTracking() )
		&&	OnAdvancedPopupMenuTipWndGet() == NULL
		)
	{
		InitToolTip();
		m_wndToolTip.RelayEvent( pMsg );
	}

	if(		( pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP )
		&&	( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_SPACE )
		)
	{
		bool bEnabled = OnQueryWindowEnabledState();
		if(		bEnabled
			&&	IsWindowVisible()
   			)
		{
			if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
			{
				HWND hWndFocus = ::GetFocus();
				if( hWndFocus != NULL && hWndFocus == m_hWnd )
				{
					if( /*m_nButtonType == BS_RADIOBUTTON ||*/ m_nButtonType == BS_AUTORADIOBUTTON )
						return FALSE;
					_OnClick( true, false );
					return TRUE;
				}
				__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( m_hWnd, GWL_STYLE );
#ifdef BS_TYPEMASK
				ASSERT( BS_TYPEMASK == 0x0000000FL );
#endif
				__EXT_MFC_LONG_PTR dwWndType = ( dwWndStyle & 0x0000000FL );
				if(		GetFocus() == this 
					&&	dwWndType != BS_DEFPUSHBUTTON 
					)
				{
					HWND hWndParent = ::GetParent( m_hWnd ); 
					HWND hWnd = ::GetWindow( hWndParent, GW_CHILD ); 
					while( hWnd != NULL )  
					{
						TCHAR szCompare[512] = _T("");
						::GetClassName(
							hWnd,
							szCompare,
							sizeof( szCompare )/sizeof( szCompare[0] )
							);
						if( _tcsicmp( szCompare, _T("BUTTON") ) == 0 )
						{
							bool bDefault = false;
							CWnd * pWnd = CWnd::FromHandlePermanent( hWnd ); 
							if(		pWnd->GetSafeHwnd() != NULL 
								&&	pWnd->IsKindOf( RUNTIME_CLASS( CExtButton ) ) 
								)
							{
								CExtButton * pBtn = STATIC_DOWNCAST( CExtButton, pWnd );
								ASSERT_VALID( pBtn );
								bDefault = pBtn->GetDefault( false ) ? true : false;
							}
							else
							{
								bDefault = ( ( ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE ) & 0x0000000FL ) == BS_DEFPUSHBUTTON ) ? true : false;
							}

							if( bDefault )
							{
								int nDlgCtrlID = ::GetDlgCtrlID( hWnd );
								::SendMessage(
									hWndParent,
									WM_COMMAND,
									MAKEWPARAM( nDlgCtrlID, BN_CLICKED ),
									(LPARAM)hWnd
									);
								return TRUE;							
							}
						}

						hWnd = ::GetWindow( hWnd, GW_HWNDNEXT );  
					}
				}
				_OnClick( true, false );
				return TRUE;
			}

			if(		pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE 
				&&	(!m_bPushTracking)
				)
			{
				bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
				if( ! bAnimationLocked )
				{
					AnimationClient_CacheGeneratorLock();
//					if( AnimationClient_StateGet(true).IsEmpty() )
 						AnimationClient_CacheNextStateMinInfo(
							false,
							__EAPT_BY_PRESSED_STATE_TURNED_ON
							);
				}
				m_bPushed = m_bPushTracking = m_bKeyTracking = true;
				if( ! bAnimationLocked )
				{
 					AnimationClient_CacheNextStateMinInfo(
						true,
						__EAPT_BY_PRESSED_STATE_TURNED_ON
						);
					AnimationClient_CacheGeneratorUnlock();
				}
				Invalidate();
				return TRUE;
			}

			if(	pMsg->message == WM_KEYUP && pMsg->wParam == VK_SPACE )
			{
				bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
				if( ! bAnimationLocked )
				{
					AnimationClient_CacheGeneratorLock();
//					if( AnimationClient_StateGet(true).IsEmpty() )
 						AnimationClient_CacheNextStateMinInfo(
							false,
							__EAPT_BY_PRESSED_STATE_TURNED_OFF
							);
				}
				m_bPushed = m_bPushTracking = m_bKeyTracking = m_bMouseOver = false;
				if( ! bAnimationLocked )
				{
 					AnimationClient_CacheNextStateMinInfo(
						true,
						__EAPT_BY_PRESSED_STATE_TURNED_OFF
						);
					AnimationClient_CacheGeneratorUnlock();
				}
				_OnClick( true, false );
				return TRUE;
			}
		}
		return TRUE;
	}
	if(		m_bQuickActivationEnabled
		&&	(	pMsg->message == WM_KEYUP
			||	pMsg->message == WM_SYSKEYUP
			)
		)
	{
		if( _QuickActivationCheck( DWORD(pMsg->wParam) ) )
		{
			if( _QuickActivationHandle() )
				return TRUE;
		}
	}

	return CButton::PreTranslateMessage(pMsg);
}

bool CExtButton::_QuickActivationHandle(
	bool bCheckWindowState // = true
	)
{
	ASSERT_VALID( this );
	if( m_bQuickActivationCancelNearest )
	{
		m_bQuickActivationCancelNearest = false;
		return true;
	}
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL );
	if( bCheckWindowState )
	{
		CWnd * pWnd = GetParent();
		for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
		{
			DWORD dwWndStyle = GetStyle();
			if( ( dwWndStyle & ( WS_VISIBLE|WS_DISABLED ) ) != WS_VISIBLE )
				return false;
			if( ( dwWndStyle & WS_CHILD ) == 0 )
				break;
		}
	}

bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
	if( ! bAnimationLocked )
	{
		m_bPushed = true;
		AnimationClient_CacheGeneratorLock();
//		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				__EAPT_BY_PRESSED_STATE_TURNED_ON
				);
		Invalidate();
		UpdateWindow();
		m_bPushed = false;
	}

	_OnClick( true, m_bQuickActivationSeparatedDropDown );

	if(		hWndOwn == NULL
		||	(! ::IsWindow( hWndOwn ) )
		)
		return true;

	if( ! bAnimationLocked )
	{
 		AnimationClient_CacheNextStateMinInfo(
			true,
			__EAPT_BY_HOVERED_STATE_TURNED_ON
			);
		AnimationClient_CacheGeneratorUnlock();
	}
	if( CExtPopupMenuWnd::IsKeyPressed( VK_MENU ) )
		CExtMenuControlBar::g_bCancelNearestAltUp = true;
	else
		CExtMenuControlBar::g_bCancelNearestAltUp = false;
	return true;
}

bool CExtButton::_QuickActivationCheck(
	DWORD dwChar,
	bool bCheckWindowState // = true
	)
{
	ASSERT_VALID( this );
	if(		(! m_bQuickActivationEnabled )
		||	GetSafeHwnd() == NULL
		||	( bCheckWindowState && ( GetStyle() & ( WS_VISIBLE|WS_DISABLED ) ) != WS_VISIBLE )
		)
		return false;
	if(		CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT )
		||	CExtPopupMenuWnd::IsKeyPressed( VK_CONTROL )
		)
		return false;
CExtSafeString strText;
	OnQueryWindowText( strText );
	if( strText.IsEmpty() )
		return false;
int nAmpPos = strText.Find( _T('&') );
	if( nAmpPos < 0 || nAmpPos == (strText.GetLength()-1) )
		return false;
TCHAR nCharOwn = strText.GetAt( nAmpPos + 1 );
DWORD dwCharOwn = (DWORD)_totupper( nCharOwn );
	if( dwCharOwn != dwChar )
		return false;
	if( bCheckWindowState )
	{
		CWnd * pWnd = GetParent();
		for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
		{
			DWORD dwWndStyle = GetStyle();
			if( ( dwWndStyle & ( WS_VISIBLE|WS_DISABLED ) ) != WS_VISIBLE )
				return false;
			if( ( dwWndStyle & WS_CHILD ) == 0 )
				break;
		}
	}
	return true;
}

LRESULT CExtButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
bool bInvalidate = false;
	switch( message )
	{
	case WM_SETTEXT:
		m_bQuickActivationCancelNearest = false;
		// continue falling ...
	case BM_SETCHECK:
	case BM_SETSTATE:
	case BM_SETSTYLE:
#if (WINVER >= 0x0400)
	case BM_CLICK:
	case BM_SETIMAGE:
#endif
	case WM_SIZE:
	case WM_WINDOWPOSCHANGED:
		bInvalidate = true;
	break;
	} // switch( message )

LRESULT lResult =
		CButton::DefWindowProc(message, wParam, lParam);
	if( bInvalidate )
		Invalidate();
	return lResult;
}

BOOL CExtButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_hCursor != NULL )
	{
		::SetCursor( m_hCursor );
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CExtButton::SetTooltipText(
	CExtSafeString * spText,
	BOOL bActivate // = TRUE
	)
{
	if( spText == NULL )
		return;
	SetTooltipText( *spText, bActivate );
}

void CExtButton::SetTooltipText(
	CExtSafeString & sText,
	BOOL bActivate // = TRUE
	)
{
	SetTooltipText(
		__EXT_MFC_SAFE_LPCTSTR(
			sText.IsEmpty() ? LPCTSTR( NULL ) : LPCTSTR( sText )
			),
		bActivate
		);
}

void CExtButton::SetTooltipText(
	int nId,
	BOOL bActivate // = TRUE
	)
{
CExtSafeString sText;
	g_ResourceManager->LoadString( sText, nId );
	SetTooltipText( &sText, bActivate );
}

void CExtButton::SetTooltipText(
	__EXT_MFC_SAFE_LPCTSTR sText,
	BOOL bActivate // = TRUE
	)
{
	if( sText == NULL )
		sText = _T("");
	CWnd::CancelToolTips();
CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
	m_strToolTipText = sText;
	if( pATTW != NULL )
	{
		//pATTW->SetText( sText );
		m_bToolTipActive = bActivate ? true : false;
	} // if( pATTW != NULL )
	else
	{
		InitToolTip();
		if( m_wndToolTip.GetToolCount() == 0 )
		{
			CRect rectBtn; 
			GetClientRect(rectBtn);
			m_wndToolTip.AddTool(this,sText,rectBtn,1);
		}
		m_wndToolTip.UpdateTipText(sText,this,1);
		m_wndToolTip.SetMaxTipWidth( -1 );
		CString strText( sText );
		if( strText.Find( _T("\r") ) >= 0 )
			m_wndToolTip.SetMaxTipWidth( OnQueryMaxTipWidth( sText ) );
		m_wndToolTip.Activate( bActivate );
	} // else from if( pATTW != NULL )
}

int CExtButton::OnQueryMaxTipWidth( 
	__EXT_MFC_SAFE_LPCTSTR lpszText 
	)
{
	lpszText;
	return 250;
}

void CExtButton::ActivateTooltip( BOOL bActivate )
{
	m_bToolTipActive = bActivate ? true : false;
	if( OnAdvancedPopupMenuTipWndGet() != NULL )
		return;
	if( m_wndToolTip.GetToolCount() == 0 )
		return;
	m_wndToolTip.Activate(bActivate);
}

__EXT_MFC_INT_PTR CExtButton::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	) const
{
	__PROF_UIS_MANAGE_STATE;
	__EXT_MFC_IMPLEMENT_TT_REDIR_OnToolHitTest( CExtButton, CExtToolControlBar );
	return CButton::OnToolHitTest( point, pTI );
}

BOOL CExtButton::GetDefault(
	bool bInitial // = true
	) const
{
	return 
		bInitial 
			? m_bDefaultButton ? TRUE : FALSE
			: ( m_nButtonType == BS_DEFPUSHBUTTON ) ? TRUE : FALSE;
}

BOOL CExtButton::SupportsCommand() const
{
	if(		m_nButtonType == BS_PUSHBUTTON
		||	m_nButtonType == BS_DEFPUSHBUTTON
		||	m_nButtonType == BS_USERBUTTON
		||	m_nButtonType == BS_OWNERDRAW
		)
		return TRUE;
	return FALSE;
}

BOOL CExtButton::SupportsCheck() const
{
	if(		m_nButtonType == BS_CHECKBOX
		||	m_nButtonType == BS_3STATE
		||	m_nButtonType == BS_RADIOBUTTON
		||	m_nButtonType == BS_AUTOCHECKBOX
		||	m_nButtonType == BS_AUTO3STATE
		||	m_nButtonType == BS_AUTORADIOBUTTON 
		)
		return TRUE;
	return FALSE;
}

BOOL CExtButton::SupportsAutoCheck() const
{
	if(		m_nButtonType == BS_AUTOCHECKBOX
		||	m_nButtonType == BS_AUTO3STATE
		||	m_nButtonType == BS_AUTORADIOBUTTON 
		)
		return TRUE;
	return FALSE;
}

INT CExtButton::GetType() const
{
	return m_nButtonType;
}

BOOL CExtButton::Is3StateType() const
{
INT nType = GetType();
BOOL bIs3State = ( nType == BS_AUTO3STATE || nType == BS_3STATE ) ? TRUE : FALSE;
	return bIs3State;
}

void CExtButton::InitToolTip()
{
	//if( OnAdvancedPopupMenuTipWndGet() != NULL )
	//	return;
	if( m_wndToolTip.m_hWnd == NULL )
	{
		m_wndToolTip.Create(this);
		m_wndToolTip.Activate(FALSE);
	}
}

void CExtButton::OnQueryWindowText(
	CExtSafeString & strWindowText
	) const
{
	ASSERT_VALID( this );
	strWindowText.Empty();

// STAR - icon only
   if( m_bIconOnly && ( ! m_icon.IsEmpty() ) )
      return;
// STAR - icon only - END

int nTextLen = GetWindowTextLength();
	if( nTextLen > 0 )
	{
		GetWindowText( strWindowText.GetBuffer( nTextLen+2 ), nTextLen+1 );
		strWindowText.ReleaseBuffer();
	} // if( nTextLen > 0 )
}

HFONT CExtButton::OnQueryFont() const
{
	ASSERT_VALID( this );
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

void CExtButton::OnSysColorChange() 
{
	CButton::OnSysColorChange();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSysColorChange( this );
//	g_CmdManager.OnSysColorChange( pPM, this );
}

void CExtButton::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	CButton::OnSettingChange( uFlags, lpszSection );
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
//	g_CmdManager.OnSettingChange( pPM, this, uFlags, lpszSection );
}

LRESULT CExtButton::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
//LRESULT lResult = CButton::OnDisplayChange( wParam, lParam );

	CButton::OnDisplayChange(wParam, LOWORD(lParam), HIWORD(lParam));

LRESULT lResult = 0;



//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
//	g_CmdManager.OnDisplayChange( pPM, this, (INT)wParam, CPoint(lParam) );
	return lResult;
}

LRESULT CExtButton::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
	wParam;
	lParam;
LRESULT lResult = Default();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnThemeChanged( this, wParam, lParam );
//	g_CmdManager.OnThemeChanged( pPM, this, wParam, lParam );
	return lResult;
}

void CExtButton::_DoClick()
{
	m_nTrackingHT = _HtMouseOnButton();
	if( m_nTrackingHT == 0 )
		return;

bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
	if( ! bAnimationLocked )
	{
		AnimationClient_CacheGeneratorLock();
//		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				__EAPT_BY_PRESSED_STATE_TURNED_ON
				);
	}

	m_bMouseOver = m_bPushed = m_bPushTracking = true;

	if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
		CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
	
	if( ! bAnimationLocked )
	{
 		AnimationClient_CacheNextStateMinInfo(
			true,
			__EAPT_BY_PRESSED_STATE_TURNED_ON
			);
		AnimationClient_CacheGeneratorUnlock();
	}

	Invalidate();
}

void CExtButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_bKeyTracking )
		return;
	CButton::OnLButtonDown( nFlags, point );
HWND hWndOwn = m_hWnd;
	if( IsWindowEnabled() )
	{
		CFrameWnd * pParentFrame = GetParentFrame();
		if(		pParentFrame != NULL
			&&	pParentFrame->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) )
			)
		{
			CFrameWnd * pMDIFrame = pParentFrame->GetParentFrame();
			if(		pMDIFrame != NULL
				&&	pMDIFrame->IsKindOf( RUNTIME_CLASS(CMDIFrameWnd) )
				)
			{
				CMDIChildWnd * pActive = ((CMDIFrameWnd*)pMDIFrame)->MDIGetActive();
				if( LPVOID(pParentFrame) != LPVOID(pActive) )
					((CMDIChildWnd*)pParentFrame)->MDIActivate();
			}
		}
		if( ! ::IsWindow( hWndOwn ) )
			return;
		HWND hWndFocus = ::GetFocus();
		if( hWndFocus != hWndOwn )
		{
///			::SetFocus( hWndOwn );
//			return; // DDX validation failed
		}
	} // if( IsWindowEnabled() )
	if( ! ::IsWindow( hWndOwn ) )
		return;
	_DoClick();
}

void CExtButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( m_bKeyTracking )
		return;
	CButton::OnLButtonDblClk(nFlags, point);
	_DoClick();
}

void CExtButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	nFlags;
	point;

	if( m_bKeyTracking )
		return;

bool bExitingPushedState = ( m_bPushed || m_bPushTracking ) ? true : false;
bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();

	if( ! bAnimationLocked )
	{
		AnimationClient_CacheGeneratorLock();
//		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				bExitingPushedState ? __EAPT_BY_PRESSED_STATE_TURNED_OFF : __EAPT_BY_HOVERED_STATE_TURNED_OFF
				);
	}

INT nTrackingHT = _HtMouseOnButton();
BOOL bWasPushed = m_bPushed && m_bPushTracking && (nTrackingHT == m_nTrackingHT);
	m_bMouseOver = m_bPushed = m_bPushTracking = false;
	m_nTrackingHT = 0;

	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();

	if( ! bAnimationLocked )
	{
 		AnimationClient_CacheNextStateMinInfo(
			true,
			bExitingPushedState ? __EAPT_BY_PRESSED_STATE_TURNED_OFF : __EAPT_BY_HOVERED_STATE_TURNED_OFF
			);
		AnimationClient_CacheGeneratorUnlock();
	}

	Invalidate();

	if( !bWasPushed )
		return;

bool bEnabled = OnQueryWindowEnabledState();
	if(		bEnabled
		&&	IsWindowVisible()
		)
		_OnClick(
			false,
			(m_bSeparatedDropDown && nTrackingHT == 2) ? true : false
			);
}

void CExtButton::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
CWnd * pWnd = GetParent();
	if( pWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pWnd );
	
	if( SupportsCheck() )
	{
		if( SupportsAutoCheck() )
		{
			INT nCheck = GetCheck();
			nCheck = ( ( nCheck + 1 ) % ( Is3StateType() ? 3 : 2 ) );
			SetCheck( nCheck );
		}
		else
			SetCheck( ( GetCheck() == 0 ) ? 1 : 0 );
		HWND hWndThis = GetSafeHwnd();
		int nDlgCtrlID = GetDlgCtrlID();
		pWnd->SendMessage(
			WM_COMMAND,
			MAKEWPARAM(nDlgCtrlID,BN_CLICKED),
			(LPARAM)m_hWnd
			);
		if( ::IsWindow( hWndThis ) )
			Invalidate();
		return;
	} // if( SupportsCheck() )

	if( ! SupportsCommand() )
		return;

	if(		(!m_bSeparatedDropDown)
		||	bSeparatedDropDownClicked
		)
	{
		if( _OnTrackPopup( bSelectAny ) )
			return;
	}
	if(		m_menu.GetSafeHmenu() == NULL
		|| (m_bSeparatedDropDown && (!bSeparatedDropDownClicked))
		)
	{
		HWND hWndThis = GetSafeHwnd();
		int nDlgCtrlID = GetDlgCtrlID();
		pWnd->SendMessage(
			WM_COMMAND,
			MAKEWPARAM(nDlgCtrlID,BN_CLICKED),
			(LPARAM)hWndThis
			);
		if( ::IsWindow( hWndThis ) )
			Invalidate();
		return;
	}
}

bool CExtButton::_IsMenuAvail()
{
	if( m_menu.GetSafeHmenu() == NULL )
		return false;
	return true;
}

bool CExtButton::_OnTrackPopup(
	bool bSelectAny
	)
{
	if( m_menu.GetSafeHmenu() == NULL )
		return false;

	ASSERT( ::IsMenu(m_menu.GetSafeHmenu()) );
	VERIFY(
		g_CmdManager->UpdateFromMenu(
			g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
			m_menu.GetSafeHmenu()
			)
		);
CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			m_hWnd,
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	pPopup->SetLeftAreaWidth( m_nMenuLeftAreaWidth );
CWnd * pWnd = GetParent();
	ASSERT_VALID( pWnd );
DWORD nAlignOpt = 0;
DWORD dwMenuOpt = m_dwMenuOpt;
	while( pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		if( nAlignOpt == 0 && ((CControlBar *)pWnd)->m_pDockBar != NULL )
		{
			ASSERT_VALID( ((CControlBar *)pWnd)->m_pDockBar );
			dwMenuOpt &= ~TPMX_ALIGN_MASK;
			UINT nDockBarID = ((CControlBar *)pWnd)->m_pDockBar->GetDlgCtrlID();
			switch( nDockBarID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				nAlignOpt = TPMX_TOPALIGN;
				break;
			case AFX_IDW_DOCKBAR_BOTTOM:
				nAlignOpt = TPMX_BOTTOMALIGN;
				break;
			case AFX_IDW_DOCKBAR_LEFT:
				nAlignOpt = TPMX_LEFTALIGN;
				break;
			case AFX_IDW_DOCKBAR_RIGHT:
				nAlignOpt = TPMX_RIGHTALIGN;
				break;
			case AFX_IDW_DOCKBAR_FLOAT:
				nAlignOpt = TPMX_TOPALIGN;
				break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
				break;
#endif // _DEBUG
			} // switch( nDockBarID )
		} // if( nAlignOpt == 0 && ((CControlBar *)pWnd)->m_pDockBar != NULL )
		pWnd = pWnd->GetParent();
		if( pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		{
			pWnd = pWnd->GetParentFrame();
			ASSERT_VALID( pWnd );
			break;
		}
	}
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
	VERIFY(
		pPopup->UpdateFromMenu( 
			pWnd->GetSafeHwnd(), 
			&m_menu, 
			true,
			true,
			m_bNoRefToCmdMngr
			)
		);
CRect wr;
	GetWindowRect( &wr );
CPoint pt = wr.CenterPoint();
	SetFocus();

	if( m_bAutoChangeID )
	{
		UINT nResultCmdID = 0;
		HWND hWndOwn = m_hWnd;
		if( ! pPopup->TrackPopupMenu(
				dwMenuOpt
					| nAlignOpt
					| ( (bSelectAny) ? TPMX_SELECT_ANY : 0 )
					| TPMX_DO_MESSAGE_LOOP | TPMX_NO_WM_COMMAND
					| TPMX_COMBINE_DEFAULT,
				pt.x,
				pt.y,
				&wr,
				this,
				_CbPaintCombinedContent,
				& nResultCmdID,
				true
				)
			)
		{
			//delete pPopup;
			ASSERT( FALSE );
			if( ! ::IsWindow( hWndOwn ) )
				return true;
		}
		else
		{
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			if( nResultCmdID != 0 )
			{
				SetDlgCtrlID( nResultCmdID );
				if( m_bAutoChangeCmdIcon )
				{
					__EXT_MFC_SAFE_LPCTSTR sProfileName =
						g_CmdManager->ProfileNameFromWnd(m_hWnd);
					if( sProfileName != NULL )
					{
						CExtCmdItem * pCmdItem =
							g_CmdManager->CmdGetPtr(
								sProfileName,
								nResultCmdID
								);
						if( pCmdItem != NULL )
						{
							CExtSafeString sText =
								pCmdItem->m_sToolbarText.IsEmpty()
									? pCmdItem->m_sMenuText
									: pCmdItem->m_sToolbarText
									;
							SetWindowText( sText );
							CExtCmdIcon * pIcon = 
								g_CmdManager->CmdGetIconPtr(
									sProfileName,
									nResultCmdID
									);
							if(		pIcon == NULL
								||	pIcon->IsEmpty()
								)
								SetIcon( (HICON)NULL );
							else
								SetIcon( pIcon->ExtractHICON() );
						}
					}
				}
				CWnd * pWnd = GetParent();
				ASSERT( pWnd != NULL );
				ASSERT_VALID( pWnd );
				HWND hWndThis = GetSafeHwnd();
				pWnd->SendMessage(
					WM_COMMAND,
					MAKEWPARAM(nResultCmdID,BN_CLICKED),
					(LPARAM)hWndThis
					);
				if( ::IsWindow(hWndThis) )
					Invalidate();
			}
		}
	} // if( m_bAutoChangeID )
	else
	{
		if( !pPopup->TrackPopupMenu(
				dwMenuOpt
					| nAlignOpt
					| ( (bSelectAny) ? TPMX_SELECT_ANY : 0 )
					| TPMX_COMBINE_DEFAULT,
				pt.x,
				pt.y,
				&wr,
				this,
				_CbPaintCombinedContent,
				NULL,
				true
				)
			)
		{
			//delete pPopup;
			ASSERT( FALSE );
		}
	} // else from if( m_bAutoChangeID )

	return true;
}

void CExtButton::_CbPaintCombinedContent(
	LPVOID pCookie,
	CDC & dc,
	const CWnd & refWndMenu,
	const CRect & rcExcludeArea, // in screen coords
	int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
	)
{
	pCookie;
	dc;
	refWndMenu;
	rcExcludeArea;
	eCombineAlign;
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( refWndMenu.GetSafeHwnd() != NULL );

CExtButton * pBtn = (CExtButton *)pCookie;
	ASSERT( pBtn != NULL );
	ASSERT_VALID( pBtn );
	ASSERT( pBtn->IsKindOf(RUNTIME_CLASS(CExtButton)) );
	
	ASSERT( eCombineAlign != CExtPopupMenuWnd::__CMBA_NONE );

	if( rcExcludeArea.IsRectEmpty() )
		return;

CRect rcClientBar,rcClientExcludeArea;
	pBtn->GetClientRect( &rcClientBar );
	pBtn->ClientToScreen( &rcClientBar );
	refWndMenu.GetClientRect( &rcClientExcludeArea );
	refWndMenu.ClientToScreen( &rcClientExcludeArea );

CPoint ptOffset =
		rcClientBar.TopLeft()
		- rcClientExcludeArea.TopLeft()
		;
CPoint ptViewportOrg = dc.GetViewportOrg();
	dc.SetViewportOrg( ptOffset );
	CExtPopupMenuWnd::g_pCombinedHelper = pBtn;
	pBtn->_RenderImpl( dc, true, true );
	CExtPopupMenuWnd::g_pCombinedHelper = NULL;
	dc.SetViewportOrg( ptViewportOrg );
}

void CExtButton::_RenderImpl(
	CDC & dc,
	bool bTransparent, // = false
	bool bCombinedContent // = false
	)
{
	ASSERT_VALID( this );
	bCombinedContent;

CRect rcClient;
	GetClientRect( &rcClient );
CExtSafeString sWindowText( _T("") );
	OnQueryWindowText( sWindowText );
bool bPB = true;
bool bDrawFocusRect = (GetFocus() == this && m_bDrawFocusRect) ? true : false;
bool bDefault = GetDefault() ? true : false;
bool bPushed = m_bPushed ? true : false;
//bool bPushed = ( m_bPushed || m_bState ) ? true : false;

//bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bFlat = GetFlat() ? true : false;
bool bHover =
		( m_bMouseOver && !CExtPopupMenuWnd::IsMenuTracking() ) ? true : false;
// HICON hIcon = (bHover) ? m_hIconIn : m_hIconOut;
// CExtCmdIcon icon;
// 	icon.AssignFromHICON( hIcon, true );
bool bCleanIcon = false;
CExtCmdIcon & icon = m_icon;
bool bSupportsCheck = SupportsCheck() ? true : false;
INT nCheckedStateNo = 0;
	if( bSupportsCheck )
	{
		nCheckedStateNo = GetCheck();
		if( (GetStyle()&BS_PUSHLIKE) == 0 )
			bPB = false;
		if( ! bPB )
		{
			bDrawFocusRect = false;
			if( icon.IsEmpty() )
			{
				if( nCheckedStateNo != 0 )
				{
					if(		m_nButtonType == BS_RADIOBUTTON
						||	m_nButtonType == BS_AUTORADIOBUTTON 
						)
					{
						VERIFY( icon.CreateRadioMarkIcon( PmBridge_GetPM(), COLORREF(-1L), COLORREF(-1L), CSize(24,16) ) );
						bCleanIcon = true;
					}
					else
					{
						if( nCheckedStateNo == 2 )
						{
							VERIFY( icon.CreateIndeterminatedSquareIcon( PmBridge_GetPM(), COLORREF(-1L), COLORREF(-1L), CSize(24,16) ) );
							bCleanIcon = true;
						}
						else
						{
							VERIFY( icon.CreateCheckMarkIcon( PmBridge_GetPM(), COLORREF(-1L), COLORREF(-1L), CSize(24,16) ) );
							bCleanIcon = true;
						}
					}
				} // if( nCheckedStateNo != 0 )
				else
				{
						VERIFY( icon.CreateEmptyIcon( PmBridge_GetPM(), COLORREF(-1L), CSize(24,16) ) );
				} // else from if( nCheckedStateNo != 0 )
			} // if( icon.IsEmpty() )
			else if( nCheckedStateNo != 0 )
				bPushed = true;
			bFlat = true;
		} // if( ! bPB )
		else
		{
			if( nCheckedStateNo != 0 )
				bPushed = true;
		} // if( ! bPB )
	} // if( bSupportsCheck )
	if( GetParent()->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		bDrawFocusRect = false;
		bFlat = true;
	}

bool _bTransparentBk = false;
COLORREF clrBackground = GetBkColor();
	if(		(! (bHover || bPushed) )
		&&	PmBridge_GetPM()->GetCb2DbTransparentMode(this)
		&&	( clrBackground == COLORREF(-1L) )
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			_bTransparentBk = true;
	}
	if( ! _bTransparentBk )
		dc.FillSolidRect( 
			&rcClient, 
			(clrBackground != COLORREF(-1L)) 
				? clrBackground 
				: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
			);
	
HFONT hFont = OnQueryFont();
	if( ! PmBridge_GetPM()->AdjustControlFont(
			hFont,
			this
			)
		)
	{
		LOGFONT _lf;
		::memset( &_lf, 0, sizeof(LOGFONT) );
		::GetObject( (HGDIOBJ)hFont, sizeof(LOGFONT), (LPVOID) &_lf );
		if( GetDefault() )
			_lf.lfWeight = 
				(_lf.lfWeight > FW_BOLD) 
					? _lf.lfWeight 
					: FW_BOLD;
		hFont = ::CreateFontIndirect( &_lf );
	}
	ASSERT( hFont != NULL );
CFont _fontDestructor;
	_fontDestructor.Attach( hFont );

CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		this,										   
		true, 
		rcClient, 
		(LPCTSTR)sWindowText,
		&icon, 
		bFlat, 
		bHover, 
		bPushed,
		false, 
		bEnabled,
		m_bDrawBorder ? true : false,
		bDrawFocusRect, 
		bDefault, 
		OnQueryAlignmentText(),
		hFont, 
		_IsMenuAvail(), 
		0, 
		bTransparent
		);

	_ppbd.m_rcBorderSizes = OnQueryBorderSizes();
	_ppbd.m_nIconAlignment = OnQueryAlignmentIcon();
	_ppbd.m_rcIconMargins = OnQueryMarginsIcon();
	_ppbd.m_rcTextMargins = OnQueryMarginsText();
	_ppbd.m_nFocusMode = m_nFocusMode;
	
	_ppbd.m_bSeparatedDropDown = m_bSeparatedDropDown;
	if(		_ppbd.m_bSeparatedDropDown
		&&	m_nTrackingHT == 2
		&&	_ppbd.m_bPushed
		)
	{
		_ppbd.m_bPushed = false;
		_ppbd.m_bPushedDropDown = true;
//		_ppbd.m_bHover = false;
	}

	_ppbd.m_bChecked = ( nCheckedStateNo != 0 ) ? true : false;

	_ppbd.m_clrForceBk = GetBkColor();
	_ppbd.m_clrForceTextDisabled = GetTextColorDisabled();
	_ppbd.m_clrForceTextNormal = GetTextColorNormal();
	_ppbd.m_clrForceTextHover = GetTextColorHover();
	_ppbd.m_clrForceTextPressed = GetTextColorPressed();

	if( /*bCombinedContent &&*/ (GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
		_ppbd.m_bForceRtlLayout = true;

	if( (GetStyle()&BS_MULTILINE) != 0 )
		_ppbd.m_bWordBreak = true;

	PmBridge_GetPM()->PaintPushButton( dc, _ppbd );

	if( bCleanIcon )
		icon.Empty();
}

void CExtButton::_DrawImpl(
	CDC & dc,
	bool bTransparent, // = false
	bool bCombinedContent // = false
	)
{
	if( AnimationClient_StatePaint( dc ) )
		return;
CRect rcClient;
	GetClientRect( &rcClient );
CExtMemoryDC dcmm( &dc, &rcClient );
	_RenderImpl( dcmm, bTransparent, bCombinedContent );
}

void CExtButton::OnPaint()
{
DWORD dwWndStyle = GetStyle();
DWORD dwWndType = dwWndStyle&0x0F;
bool bOwnerDraw =
		(dwWndType == BS_OWNERDRAW || dwWndType == BS_USERBUTTON)
			? true : false;
	if( bOwnerDraw )
	{
		CButton::OnPaint();
		return;
	} // if( bOwnerDraw )
CPaintDC dcPaint( this );
	_DrawImpl( dcPaint );
}

BOOL CExtButton::OnEraseBkgnd(CDC* pDC) 
{
//DWORD dwWndStyle = GetStyle();
//DWORD dwWndType = dwWndStyle&0x0F;
//bool bOwnerDraw =
//		(dwWndType == BS_OWNERDRAW || dwWndType == BS_USERBUTTON)
//			? true : false;
//	if( bOwnerDraw )
//		return CButton::OnEraseBkgnd(pDC);
//	return FALSE;
	pDC;
	return TRUE;
}

void CExtButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT( lpDrawItemStruct != NULL );
	ASSERT( lpDrawItemStruct->hDC != NULL );
	_DrawImpl( * CDC::FromHandle(lpDrawItemStruct->hDC) );
}

void CExtButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	AnimationSite_ClientProgressStop( this );
CClientDC dc( this );
	_DrawImpl( dc );
}

CRect CExtButton::OnQueryBorderSizes() const
{
	ASSERT_VALID( this );
	return CRect( 1, 1, 1, 1 );
}

INT CExtButton::OnQueryAlignmentText() const
{
	ASSERT_VALID( this );
DWORD dwStyle = GetStyle();
INT nAlign = 0;
const UINT __BS_H_MASK = (BS_LEFT|BS_CENTER|BS_RIGHT);
const UINT __BS_V_MASK = (BS_TOP|BS_VCENTER|BS_BOTTOM);

	if( (dwStyle&__BS_H_MASK) == BS_RIGHT )
		nAlign |= CExtPaintManager::__ALIGN_HORIZ_RIGHT;
	else if( (dwStyle&__BS_H_MASK) == BS_LEFT )
		nAlign |= CExtPaintManager::__ALIGN_HORIZ_LEFT;
	else
		nAlign |= CExtPaintManager::__ALIGN_HORIZ_CENTER;
	
	if( (dwStyle&__BS_V_MASK) == BS_BOTTOM )
		nAlign |= CExtPaintManager::__ALIGN_VERT_BOTTOM;
	else if( (dwStyle&__BS_V_MASK) == BS_TOP )
		nAlign |= CExtPaintManager::__ALIGN_VERT_TOP;
	else
		nAlign |= CExtPaintManager::__ALIGN_VERT_CENTER;
	return nAlign;
}

INT CExtButton::OnQueryAlignmentIcon() const
{
	ASSERT_VALID( this );
CExtSafeString sWindowText( _T("") );
	OnQueryWindowText( sWindowText );
	return 
		m_nIconAlignment >= 0 
			? m_nIconAlignment
			: sWindowText.IsEmpty()
				? CExtPaintManager::__ALIGN_HORIZ_CENTER | CExtPaintManager::__ALIGN_VERT_CENTER
				: CExtPaintManager::__ALIGN_HORIZ_LEFT | CExtPaintManager::__ALIGN_VERT_CENTER;
}

CRect CExtButton::OnQueryMarginsText() const
{
	ASSERT_VALID( this );
	return m_rcTextMargins;
}

CRect CExtButton::OnQueryMarginsIcon() const
{
	ASSERT_VALID( this );
	return m_rcIconMargins;
}

CExtPopupMenuTipWnd * CExtButton::OnAdvancedPopupMenuTipWndGet() const
{
	if( m_nAdvancedTipStyle == INT(CExtPopupMenuTipWnd::__ETS_NONE) )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

void CExtButton::OnAdvancedPopupMenuTipWndDisplay(
	CExtPopupMenuTipWnd & _ATTW,
	const RECT & rcExcludeArea
	) const
{
	ASSERT_VALID( this );
	if( ! m_bToolTipActive )
		return;
	if( m_strToolTipText.IsEmpty() )
		return;
	_ATTW.SetText( LPCTSTR(m_strToolTipText) );
	_ATTW.SetTipStyle( (CExtPopupMenuTipWnd::e_tip_style_t)m_nAdvancedTipStyle );
	_ATTW.Show( (CWnd*)this, rcExcludeArea );
}

CExtColorButton::CExtColorButton()
{
	m_bUseStdColorDlg = false;
	m_bEnableBtnColorDefault = true;
	m_bEnableBtnColorCustom = true;
	m_clrDefault = (COLORREF)(-1); // unexisting
	m_clrSelected = RGB(0,0,0);
	m_clrLastHover = (COLORREF)(-1); // unexisting
	m_bTrackColorPickerSelection = true;
	m_lParamCookie = 0;
	m_sBtnTextColorDefault = _T("");
	m_sBtnTextColorCustom = _T("");
}

CExtColorButton::~CExtColorButton()
{
}

IMPLEMENT_DYNCREATE(CExtColorButton, CExtButton);

BEGIN_MESSAGE_MAP(CExtColorButton, CExtButton)
    //{{AFX_MSG_MAP(CExtColorButton)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChanged,
		OnColorChanged
		)
	ON_REGISTERED_MESSAGE(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
		OnColorChangedFinally
		)
	ON_REGISTERED_MESSAGE(
		CExtPopupColorMenuWnd::g_nMsgNotifyCustColor,
		OnColorSelectCustom
		)
END_MESSAGE_MAP()

bool CExtColorButton::_IsMenuAvail()
{
	return true;
}

bool CExtColorButton::_OnTrackPopup(
	bool bSelectAny
	)
{
HWND hWndOwn = GetSafeHwnd();
	if( hWndOwn == NULL )
		return false;
	m_clrLastHover = (COLORREF)(-1); // unexisting

bool bInBar = false;
CWnd * pWnd = GetParent();
	ASSERT_VALID( pWnd );
DWORD nAlignOpt = 0;
DWORD dwMenuOpt = m_dwMenuOpt;
	while( pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		bInBar = true;
		if( nAlignOpt == 0 && ((CControlBar *)pWnd)->m_pDockBar != NULL )
		{
			ASSERT_VALID( ((CControlBar *)pWnd)->m_pDockBar );
			dwMenuOpt &= ~TPMX_ALIGN_MASK;
			UINT nDockBarID = ((CControlBar *)pWnd)->m_pDockBar->GetDlgCtrlID();
			switch( nDockBarID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				nAlignOpt = TPMX_TOPALIGN;
				break;
			case AFX_IDW_DOCKBAR_BOTTOM:
				nAlignOpt = TPMX_BOTTOMALIGN;
				break;
			case AFX_IDW_DOCKBAR_LEFT:
				nAlignOpt = TPMX_LEFTALIGN;
				break;
			case AFX_IDW_DOCKBAR_RIGHT:
				nAlignOpt = TPMX_RIGHTALIGN;
				break;
			case AFX_IDW_DOCKBAR_FLOAT:
				nAlignOpt = TPMX_TOPALIGN;
				break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
				break;
#endif // _DEBUG
			} // switch( nDockBarID )
		} // if( nAlignOpt == 0 && ((CControlBar *)pWnd)->m_pDockBar != NULL )
		pWnd = pWnd->GetParent();
		if( pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		{
			pWnd = pWnd->GetParentFrame();
			ASSERT_VALID( pWnd );
			break;
		}
	}

//	if( ! (	CExtPopupMenuSite ::g_DefPopupMenuSite.IsEmpty()
//			|| CExtPopupMenuSite ::g_DefPopupMenuSite.IsShutdownMode()
//			)
//		)
//	{
//		CExtPopupMenuWnd * pPopupCurr =
//			CExtPopupMenuSite ::g_DefPopupMenuSite.GetInstance();
//		ASSERT_VALID( pPopupCurr );
//		if(		pPopupCurr->GetSafeHwnd() != NULL
//			&&	pPopupCurr->GetParent()->GetSafeHwnd() ==
//					( bInBar ? pWnd->GetSafeHwnd() : GetSafeHwnd() )
//			)
//			return true;
//	}

CExtPopupColorMenuWnd * pPopup =
		STATIC_DOWNCAST(
			CExtPopupColorMenuWnd,
			CExtPopupMenuWnd::InstantiatePopupMenu(
				GetSafeHwnd(),
				RUNTIME_CLASS(CExtPopupColorMenuWnd),
				this
				)
			);
	pPopup->SetLeftAreaWidth( m_nMenuLeftAreaWidth );
	pPopup->m_lParamCookie = m_lParamCookie;

	VERIFY(
		pPopup->CreatePopupMenu(
			bInBar ? pWnd->GetSafeHwnd() : GetSafeHwnd()
			)
		);
	pPopup->m_bEnableBtnColorDefault =
		m_bEnableBtnColorDefault;
	pPopup->m_bEnableBtnColorCustom =
		m_bEnableBtnColorCustom;
	pPopup->m_clrDefault =
		m_clrDefault;
	pPopup->m_clrInitial =
		m_clrSelected;
	if( !m_sBtnTextColorDefault.IsEmpty() )
		pPopup->m_sBtnTextColorDefault = m_sBtnTextColorDefault;
	if( !m_sBtnTextColorCustom.IsEmpty() )
		pPopup->m_sBtnTextColorCustom = m_sBtnTextColorCustom;

CRect wr;
	GetWindowRect( &wr );
CPoint pt = wr.CenterPoint();
	if( bInBar)
		pWnd->SetFocus();
	else
		SetFocus();
bool bToolMenuTracking = false;
	if( bInBar )
	{
		CExtToolControlBar::_CloseTrackingMenus();
		bToolMenuTracking = true;
	}
	if( ! pPopup->TrackPopupMenu(
			dwMenuOpt
				| nAlignOpt
				| ( (bSelectAny) ? TPMX_SELECT_ANY : 0 )
				| TPMX_COMBINE_DEFAULT
				| TPMX_DO_MESSAGE_LOOP
				,
			pt.x,
			pt.y,
			&wr,
			this,
			_CbPaintCombinedContent,
			NULL,
			true
			)
		)
	{
		//delete pPopup;
		ASSERT( FALSE );
	}
	else
	{
		if( bToolMenuTracking )
			CExtToolControlBar::g_bMenuTracking = true;
	}
	if( ! ::IsWindow( hWndOwn ) )
		return true;
	if( m_clrLastHover != COLORREF(-1L) )
	{
		m_clrLastHover = COLORREF(-1L);
		Invalidate();
	}
	return true;
}

void CExtColorButton::OnGenerateColorBtnIcon(
	CExtCmdIcon & icon
	)
{
	icon.Empty();
COLORREF clrIcon = m_clrSelected;
	if(		m_bTrackColorPickerSelection
		&&	m_clrLastHover != COLORREF(-1L)
		)
		clrIcon = m_clrLastHover;
	if( clrIcon == COLORREF(-1L) )
	{
		VERIFY( icon.m_bmpNormal.FromColor( RGB(0,0,0), CSize(16,16), BYTE(0) ) );
		return;
	}
//TRACE1(" .... color .... %06X\r\n", clrIcon );
HICON hIcon = CExtPaintManager::stat_GenerateColorIconBox( clrIcon );
	//ASSERT( hIcon != NULL );
	if( hIcon == NULL )
		return;
	icon.AssignFromHICON( hIcon, false );
}

void CExtColorButton::_RenderImpl( // non-buffered paint
	CDC & dc,
	bool bTransparent, // = false
	bool bCombinedContent // = false
	)
{
	OnGenerateColorBtnIcon( m_icon );
CRect rcClient;
	GetClientRect( &rcClient );
CExtSafeString sWindowText( _T("") );
	OnQueryWindowText( sWindowText );
bool bDrawFocusRect = (GetFocus() == this && m_bDrawFocusRect) ? true : false;
bool bDefault = GetDefault() ? true : false;
bool bPushed = m_bPushed;

//bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bFlat = GetFlat() ? true : false;
bool bHover =
		( m_bMouseOver && !CExtPopupMenuWnd::IsMenuTracking() ) ? true : false;
	if( GetParent()->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		bDrawFocusRect = false;
		bFlat = true;
	}

	COLORREF clrBackground = GetBkColor();

bool _bTransparentBk = false;
	if(		(! (bHover || bPushed) )
		&&	PmBridge_GetPM()->GetCb2DbTransparentMode(this)
		&&	( clrBackground == COLORREF(-1L) )
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			_bTransparentBk = true;
	}
	if( ! _bTransparentBk )
		dc.FillSolidRect( 
			&rcClient, 
			(clrBackground != COLORREF(-1L)) 
				? clrBackground 
				: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
			);

HFONT hFont = OnQueryFont();
	if( ! PmBridge_GetPM()->AdjustControlFont(
			hFont,
			this
			)
		)
	{
		LOGFONT _lf;
		::memset( &_lf, 0, sizeof(LOGFONT) );
		::GetObject( (HGDIOBJ)hFont, sizeof(LOGFONT), (LPVOID) &_lf );
		if( GetDefault() )
			_lf.lfWeight = 
				(_lf.lfWeight > FW_BOLD) 
					? _lf.lfWeight 
					: FW_BOLD;
		hFont = ::CreateFontIndirect( &_lf );
	}
	ASSERT( hFont != NULL );
CFont _fontDestructor;
	_fontDestructor.Attach( hFont );

CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		this,
		true, 
		rcClient, 
		(LPCTSTR)sWindowText,
		&m_icon, 
		bFlat, 
		bHover, 
		bPushed,
		false, 
		bEnabled,
		( m_bDrawBorder && (!CExtPopupMenuWnd::IsMenuTracking()) )
			? true : false,
		( bDrawFocusRect && (!CExtPopupMenuWnd::IsMenuTracking()) && (!sWindowText.IsEmpty()) )
			? true : false,
		bDefault, 
		0,
		hFont, 
		_IsMenuAvail(), 
		0, 
		bTransparent
		);

	_ppbd.m_rcBorderSizes = OnQueryBorderSizes();
	_ppbd.m_nIconAlignment = OnQueryAlignmentIcon();
	_ppbd.m_nTextAlignment = OnQueryAlignmentText();
	_ppbd.m_rcIconMargins = OnQueryMarginsIcon();
	_ppbd.m_rcTextMargins = OnQueryMarginsText();
	_ppbd.m_nFocusMode = m_nFocusMode;

	_ppbd.m_bSeparatedDropDown = m_bSeparatedDropDown ? true : false;
	if( _ppbd.m_bSeparatedDropDown
		&& m_nTrackingHT == 2
		&& _ppbd.m_bPushed
		)
	{
		_ppbd.m_bPushed = false;
		_ppbd.m_bPushedDropDown = true;
//		_ppbd.m_bHover = false;
	}

	_ppbd.m_bChecked = ( SupportsCheck() && GetCheck() != 0 ) ? true : false;

	_ppbd.m_clrForceBk = GetBkColor();
	_ppbd.m_clrForceTextDisabled = GetTextColorDisabled();
	_ppbd.m_clrForceTextNormal = GetTextColorNormal();
	_ppbd.m_clrForceTextHover = GetTextColorHover();
	_ppbd.m_clrForceTextPressed = GetTextColorPressed();

	if( bCombinedContent && (GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
		_ppbd.m_bForceRtlLayout = true;

	if( (GetStyle()&BS_MULTILINE) != 0 )
		_ppbd.m_bWordBreak = true;

	PmBridge_GetPM()->PaintPushButton( dc, _ppbd );
}

LRESULT CExtColorButton::OnColorChanged(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;

	if( m_bTrackColorPickerSelection )
	{
		m_clrLastHover = (COLORREF)wParam;
		Invalidate();
		CExtPopupMenuWnd * pPopup =
			CExtPopupMenuWnd::GetTrackingMenu();
		if(		pPopup != NULL
			&&	pPopup->GetSafeHwnd() != NULL
			)
		{
			pPopup->Invalidate();
			pPopup->UpdateWindow();
		}
	} // if( m_bTrackColorPickerSelection )

	return
		GetParent()->SendMessage(
			CExtPopupColorMenuWnd::g_nMsgNotifyColorChanged,
			wParam,
			lParam
			);
}

LRESULT CExtColorButton::OnColorChangedFinally(WPARAM wParam, LPARAM lParam)
{
	lParam;
COLORREF clr = (COLORREF)wParam;
//	m_clrSelected = (clr == ((COLORREF)(-1))) ? RGB(0,0,0) : clr;
	m_clrSelected = clr;
	Invalidate();
	return
		GetParent()->SendMessage(
			CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
			wParam,
			lParam
			);
}

LRESULT CExtColorButton::OnColorSelectCustom(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	if( m_bUseStdColorDlg )
	{
		CColorDialog dlgSysColor(m_clrSelected,CC_FULLOPEN,this);
		if( dlgSysColor.DoModal() != IDOK )
			return 0;
		m_clrSelected = dlgSysColor.m_cc.rgbResult;
	} // if( m_bUseStdColorDlg )
	else
	{
		CExtColorDlg dlgColor( m_clrSelected,m_clrSelected,this );
		if( dlgColor.DoModal() != IDOK )
			return 0;
		m_clrSelected = dlgColor.m_clrNew;
	} // else from if( m_bUseStdColorDlg )
	GetParent()->SendMessage(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
		WPARAM( m_clrSelected ),
		lParam
		);
	Invalidate();
	return 0;
}

LRESULT CExtButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
#if (defined WM_UPDATEUISTATE)	
	ASSERT( WM_UPDATEUISTATE == 0x0128 );
#endif
	// WM_UPDATEUISTATE causes repaint without WM_PAINT, so we eat it
	if( message == 0x0128 )
		return 0;
	if(		message == WM_SETTEXT 
		||	message == WM_ENABLE
		
		)
	{
		LRESULT lResult = CButton::WindowProc(message, wParam, lParam);
		Invalidate();
		return lResult;
	}
	
	if( message == BM_GETCHECK )
		return m_nCheck;
	if( message == BM_SETCHECK )
	{
		m_nCheck = (INT)wParam;
		Invalidate();
		return 0L;
	}
	if( message == BM_SETSTYLE )
	{
		m_nButtonType = (INT)wParam;
		m_nButtonType &= 0x0F;
		Invalidate();
		return 0L;
	}
	if( message == CExtToolControlBar::g_nMsgContinueTrackMenu )
	{
		_OnTrackPopup( lParam ? true : false );
		return (!0L);
	}
	if(		OnAdvancedPopupMenuTipWndGet() == NULL
		&&	message == WM_NOTIFY
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		&&	((LPNMHDR)lParam) != NULL
		&&	((LPNMHDR)lParam)->hwndFrom == m_wndToolTip.GetSafeHwnd()
		&&	((LPNMHDR)lParam)->code == TTN_SHOW
		)
		::SetWindowPos(
			m_wndToolTip.GetSafeHwnd(),
			HWND_TOP,
			0,0,0,0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
			);
	
	if(		message == CExtPopupMenuWnd::g_nMsgPrepareMenu
		||	message == CExtPopupMenuWnd::g_nMsgPrepareOneMenuLevel
		||	message == CExtPopupMenuWnd::g_nMsgPopupNext
		||	message == CExtPopupMenuWnd::g_nMsgPopupPrev
		||	message == CExtPopupMenuWnd::g_nMsgNotifyMenuExpanded
		||	message == CExtPopupMenuWnd::g_nMsgNotifyMenuClosed
		||	message == CExtPopupMenuWnd::g_nMsgPopupDrawItem
		||	message == CExtPopupMenuWnd::g_nMsgPopupDrawLeftArea
		)
	{
		CWnd * pParent = GetParent();
		if( pParent )
			return pParent->SendMessage( message, wParam, lParam );
		else
			return 0L;
	}

bool bInvalidate = false;
	switch( message )
	{
	case WM_GETDLGCODE:
		if( GetDefault( false ) )
			return DLGC_DEFPUSHBUTTON;
		else
			return ( ( SupportsCheck() ? 0 : DLGC_UNDEFPUSHBUTTON ) | DLGC_BUTTON );
	case WM_MOUSEMOVE:
		if( CExtToolControlBar::g_bMenuTracking )
		{
			CWnd * pWndParent = GetParent();
			if(		pWndParent != NULL
				&&	pWndParent->IsKindOf(RUNTIME_CLASS(CExtToolControlBar))
				)
			{
				_OnTrackPopup( false );
				return 0L;
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
	case BM_SETSTATE:
		m_bState = (wParam > 0L) ? true : false;
	case BM_SETSTYLE:
#if (WINVER >= 0x0400)
	case BM_CLICK:
	case BM_SETIMAGE:
#endif
	case WM_SIZE:
	case WM_WINDOWPOSCHANGED:
		bInvalidate = true;
	break;
	case WM_TIMER:
		if( AnimationSite_OnHookTimer( UINT(wParam) ) )
			return 0L;
	break;
	} // switch( message )
	if( message == WM_PRINT || message == WM_PRINTCLIENT )
	{
		CDC * pDC = CDC::FromHandle( (HDC)wParam );
		_DrawImpl( *pDC );
		return (!0);
	}	

LRESULT lResult =
		CButton::WindowProc( message, wParam, lParam );

//	if(
//			CExtPopupColorMenuWnd::g_nMsgNotifyColorChanged
//		||	CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally
//		||	CExtPopupColorMenuWnd::g_nMsgNotifyCustColor
//		)
//	{
//		CWnd * pParent = GetParent();
//		if( pParent )
//			pParent->SendMessage( message, wParam, lParam );
//	}

	if( bInvalidate )
		Invalidate();
	return lResult;
}

void CExtColorButton::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
	if(		(!m_bSeparatedDropDown)
		||	bSeparatedDropDownClicked
		)
	{
		if( _OnTrackPopup( bSelectAny ) )
			return;
	}

CWnd * pWnd = GetParent();
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
	pWnd->SendMessage(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
		(WPARAM)m_clrSelected,
		m_lParamCookie
		);
}

void AFXAPI __EXT_MFC_DDX_ColorButton( CDataExchange * pDX, INT nIDC, COLORREF & value )
{
HWND hWndCtrl = pDX->PrepareCtrl( nIDC );
	ASSERT( hWndCtrl != NULL );
	ASSERT( ::IsWindow( hWndCtrl ) );
CExtColorButton * pWnd = 
		DYNAMIC_DOWNCAST( CExtColorButton, CWnd::FromHandle( hWndCtrl ) );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		if( pDX->m_bSaveAndValidate )
			value = pWnd->m_clrSelected;
		else
		{
			pWnd->m_clrSelected = value;
			pWnd->Invalidate();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExtCheckBox

IMPLEMENT_DYNCREATE( CExtCheckBox, CExtButton );

CExtCheckBox::CExtCheckBox()
{
	m_rcTextMargins = CRect( 1, 1, 1, 1 );
}

CExtCheckBox::~CExtCheckBox()
{
}

BEGIN_MESSAGE_MAP(CExtCheckBox, CExtButton)
//{{AFX_MSG_MAP(CExtCheckBox)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtCheckBox::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
	ASSERT_VALID( this );
	if( !SupportsCheck() )
	{
		CExtButton::_OnClick(
			bSelectAny,
			bSeparatedDropDownClicked
			);
		return;
	} // if( ! SupportsCheck() )

	if( SupportsAutoCheck() )
	{
		INT nCheck = GetCheck();
		nCheck = ( ( nCheck + 1 ) % ( Is3StateType() ? 3 : 2 ) );
		SetCheck( nCheck );
	}
	
CWnd * pWnd = GetParent();
	if( pWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pWnd );
HWND hWndThis = GetSafeHwnd();
int nDlgCtrlID = GetDlgCtrlID();
	pWnd->SendMessage(
		WM_COMMAND,
		MAKEWPARAM(nDlgCtrlID,BN_CLICKED),
		(LPARAM)hWndThis
		);
	if( ::IsWindow( hWndThis ) )
		Invalidate();
}

void CExtCheckBox::_RenderImpl(
	CDC & dc,
	bool bTransparent, // = false
	bool bCombinedContent // = false
	)
{
	ASSERT_VALID( this );

	if( (GetStyle()&BS_PUSHLIKE) != 0 )
	{
		CExtButton::_RenderImpl( dc, bTransparent, bCombinedContent );
		return;
	}

CRect rcClient;
	GetClientRect( &rcClient );
CExtSafeString sWindowText( _T("") );
	OnQueryWindowText( sWindowText );
bool bDrawFocusRect = (GetFocus() == this && m_bDrawFocusRect) ? true : false;
//bool bPushed = (m_bPushed || GetCheck()) ? true : false;
bool bPushed = m_bPushed;
//bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bHover =
		( m_bMouseOver && !CExtPopupMenuWnd::IsMenuTracking() ) ? true : false;
	if( GetParent()->IsKindOf(RUNTIME_CLASS(CControlBar)) )
		bDrawFocusRect = false;

	COLORREF clrBackground = GetBkColor();

bool _bTransparentBk = false;
	if(		PmBridge_GetPM()->GetCb2DbTransparentMode(this)
		&&	( clrBackground == COLORREF(-1L) )
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			_bTransparentBk = true;
	}
	if( ! _bTransparentBk )
		dc.FillSolidRect( 
			&rcClient, 
			(clrBackground != COLORREF(-1L)) 
				? clrBackground 
				: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
			);

HFONT hFont = OnQueryFont();
	if( ! PmBridge_GetPM()->AdjustControlFont(
			hFont,
			this
			)
		)
	{
		LOGFONT _lf;
		::memset( &_lf, 0, sizeof(LOGFONT) );
		::GetObject( (HGDIOBJ)hFont, sizeof(LOGFONT), (LPVOID) &_lf );
		if( GetDefault() )
			_lf.lfWeight = 
				(_lf.lfWeight > FW_BOLD) 
					? _lf.lfWeight 
					: FW_BOLD;
		hFont = ::CreateFontIndirect( &_lf );
	}
	ASSERT( hFont != NULL );
CFont _fontDestructor;
	_fontDestructor.Attach( hFont );

bool bLeftText = false;
DWORD dwStyle = GetStyle();
DWORD dwExStyle = GetExStyle();
	if(		(dwStyle&BS_LEFTTEXT) != 0
		||	(dwStyle&BS_RIGHTBUTTON) != 0
		||	(dwExStyle&WS_EX_RIGHT) != 0
		)
		bLeftText = true;

bool bMultiline = (dwStyle&BS_MULTILINE) != 0 ? true : false;

INT nCheck = GetCheck();
CExtPaintManager::eBoxState_t eState = CExtPaintManager::BOX_UNCHECKED;

	switch( nCheck ) 
	{
	case BST_UNCHECKED:
		if( bEnabled )
		{
			if(		( bHover && !m_bPushTracking) 
				||  (!bHover &&  m_bPushTracking && !m_bKeyTracking) 
				)
				eState = CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED;
			else if( (bHover && m_bPushTracking) || m_bKeyTracking )
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
			if(		( bHover && !m_bPushTracking) 
				||  (!bHover &&  m_bPushTracking && !m_bKeyTracking) 
				)
				eState = CExtPaintManager::BOX_MOUSE_HOVER_CHECKED;
			else if( (bHover && m_bPushTracking) || m_bKeyTracking )
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
			if(		( bHover && !m_bPushTracking) 
				||  (!bHover &&  m_bPushTracking && !m_bKeyTracking ) 
				)
				eState = CExtPaintManager::BOX_MOUSE_HOVER_INDETERMINATE;
			else if( (bHover && m_bPushTracking) || m_bKeyTracking )
				eState = CExtPaintManager::BOX_LDOWN_INDETERMINATE;
			else
				eState = CExtPaintManager::BOX_INDETERMINATE;
		}
		else
			eState = CExtPaintManager::BOX_DISABLED_INDETERMINATE;
		break;
	}
	
CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd(
		this,
		eState,
		bLeftText,
		bMultiline,
		rcClient,
		(LPCTSTR) sWindowText,
		bHover,		
		bPushed,
		bEnabled,
		bDrawFocusRect, 
		OnQueryAlignmentText(),
		hFont,
		bTransparent
		);

	_pcrbd.m_rcTextMargins = OnQueryMarginsText();
	
	_pcrbd.m_clrForceBk = GetBkColor();
	_pcrbd.m_clrForceTextDisabled = GetTextColorDisabled();
	_pcrbd.m_clrForceTextNormal = GetTextColorNormal();
	_pcrbd.m_clrForceTextHover = GetTextColorHover();
	_pcrbd.m_clrForceTextPressed = GetTextColorPressed();

	PmBridge_GetPM()->PaintCheckButton( dc, _pcrbd );
	
	// by default this method does nothing
	OnPaintBox( 
		dc, 
		_pcrbd.m_rcBox, 
		_pcrbd.m_eState 
		);
	
	PmBridge_GetPM()->OnPaintSessionComplete( this );
}

bool CExtCheckBox::OnPaintBox(
	CDC & dc,
	CRect & rcBox,
	CExtPaintManager::eBoxState_t eState
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcBox;
	eState;
	return false;
}

INT CExtCheckBox::OnQueryAlignmentText() const
{
	ASSERT_VALID( this );

DWORD dwStyle = GetStyle();
	if( (dwStyle&BS_PUSHLIKE) != 0 )
		return CExtButton::OnQueryAlignmentText();

INT nAlign = 0;
const UINT __BS_H_MASK = (BS_LEFT|BS_CENTER|BS_RIGHT);
const UINT __BS_V_MASK = (BS_TOP|BS_VCENTER|BS_BOTTOM);

	if( (dwStyle&__BS_H_MASK) == BS_RIGHT )
		nAlign |= CExtPaintManager::__ALIGN_HORIZ_RIGHT;
	else if( (dwStyle&__BS_H_MASK) == BS_CENTER )
		nAlign |= CExtPaintManager::__ALIGN_HORIZ_CENTER;
	else
		nAlign |= CExtPaintManager::__ALIGN_HORIZ_LEFT;
	
	if( (dwStyle&__BS_V_MASK) == BS_BOTTOM )
		nAlign |= CExtPaintManager::__ALIGN_VERT_BOTTOM;
	else if( (dwStyle&__BS_V_MASK) == BS_TOP )
		nAlign |= CExtPaintManager::__ALIGN_VERT_TOP;
	else
		nAlign |= CExtPaintManager::__ALIGN_VERT_CENTER;
	return nAlign;
}

/////////////////////////////////////////////////////////////////////////////
// CExtRadioButton

IMPLEMENT_DYNCREATE( CExtRadioButton, CExtCheckBox );

CExtRadioButton::CExtRadioButton()
{
}

CExtRadioButton::~CExtRadioButton()
{
}

BEGIN_MESSAGE_MAP(CExtRadioButton, CExtCheckBox)
//{{AFX_MSG_MAP(CExtRadioButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

BOOL CExtRadioButton::PreTranslateMessage(MSG* pMsg) 
{
	if(		pMsg->hwnd == m_hWnd && GetParent() != NULL 
		&&	pMsg->message == WM_KEYDOWN
		)
	{
		switch(pMsg->wParam) 
		{
		case VK_UP:
		case VK_LEFT:
			if( _CheckNextPrevRadioButton( false ) )
				return TRUE;
			break;
			
		case VK_DOWN:
		case VK_RIGHT:
			if( _CheckNextPrevRadioButton( true ) )
				return TRUE;
			break;
		}
		
	} // if( pMsg->hwnd == m_hWnd )
	return CExtCheckBox::PreTranslateMessage(pMsg);
}

LRESULT CExtRadioButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_GETDLGCODE )
		return (DLGC_BUTTON | DLGC_RADIOBUTTON);

	if(		message == BM_SETCHECK 
		&&	IsWindowEnabled()
		&&	IsWindowVisible()
		)
	{
		LRESULT lRes = CExtCheckBox::WindowProc( message, wParam, lParam );
		CWnd * pWndParent = GetParent();
		ASSERT_VALID( pWndParent );

		CWnd * pWnd = pWndParent->GetNextDlgGroupItem( this, TRUE );
		while( pWnd != NULL && pWnd != this ) 
		{
			if(		( pWnd->SendMessage( WM_GETDLGCODE ) & DLGC_RADIOBUTTON ) != 0 
				&&	pWnd->SendMessage( BM_GETCHECK ) == BST_UNCHECKED
				)
				pWnd->ModifyStyle( WS_TABSTOP, 0 );
			pWnd = pWndParent->GetNextDlgGroupItem( pWnd, TRUE );
		}

		pWnd = pWndParent->GetNextDlgGroupItem( this, FALSE );
		while( pWnd != NULL && pWnd != this ) 
		{
			if(		( pWnd->SendMessage( WM_GETDLGCODE ) & DLGC_RADIOBUTTON ) != 0 
				&&	pWnd->SendMessage( BM_GETCHECK ) == BST_UNCHECKED
				)
				pWnd->ModifyStyle( WS_TABSTOP, 0 );
			pWnd = pWndParent->GetNextDlgGroupItem( pWnd, FALSE );
		}
		bool bCheck = 
			( wParam != BST_UNCHECKED ) ? true : false;
		this->ModifyStyle( 
			bCheck ? 0 : WS_TABSTOP, 
			bCheck ? WS_TABSTOP : 0
			);		
		return lRes;
	}
	
	return
		CExtCheckBox::WindowProc( message, wParam, lParam );
}

bool CExtRadioButton::_CheckNextPrevRadioButton( bool bNext )
{
	ASSERT_VALID (this);
CWnd * pWndParent = GetParent();
	ASSERT_VALID( pWndParent );
CWnd * pWnd = pWndParent->GetNextDlgGroupItem( this, !bNext );
	for( ; pWnd != this; pWnd = pWndParent->GetNextDlgGroupItem( pWnd, !bNext ) )
	{
		if(		( pWnd->SendMessage( WM_GETDLGCODE ) & DLGC_RADIOBUTTON ) == 0 
			||	(pWnd->GetStyle()&(WS_DISABLED|WS_VISIBLE)) != WS_VISIBLE
			)
			continue;
		break;
	}
	if(		pWnd != NULL 
		&&	pWnd != this 
		&&	pWnd->SendMessage( BM_GETCHECK ) == BST_UNCHECKED 
		)
	{
		ASSERT_VALID( pWnd );
		_UncheckRadioButtonsInGroup();
		SetCheck( BST_UNCHECKED );
		pWnd->SendMessage( BM_SETCHECK, BST_CHECKED );
		pWnd->SetFocus();
		::SendMessage(
			pWnd->GetParent()->GetSafeHwnd(), 
			WM_COMMAND,
			MAKELONG( ::__EXT_MFC_GetWindowLong( pWnd->GetSafeHwnd(), GWL_ID ), BN_CLICKED ),
			(LPARAM) pWnd->GetSafeHwnd()
			);
		return true;
	}
	return false;
}

void CExtRadioButton::_UncheckRadioButtonsInGroup()
{
	ASSERT_VALID (this);
CWnd * pWndParent = GetParent();
	if( pWndParent == NULL )
		return;
	ASSERT_VALID( pWndParent );
	
bool bEnabled = OnQueryWindowEnabledState();
	if( !bEnabled )
		return;

CWnd * pWnd = pWndParent->GetNextDlgGroupItem( this, TRUE );
	while( pWnd != NULL && pWnd != this ) 
	{
		if(		( pWnd->SendMessage( WM_GETDLGCODE ) & DLGC_RADIOBUTTON ) != 0 
			&&	pWnd->SendMessage( BM_GETCHECK ) != BST_UNCHECKED
			)
			pWnd->SendMessage( BM_SETCHECK, BST_UNCHECKED );
		pWnd = pWndParent->GetNextDlgGroupItem( pWnd, TRUE );
	}

	pWnd = pWndParent->GetNextDlgGroupItem( this, FALSE );
	while( pWnd != NULL && pWnd != this ) 
	{
		if(		( pWnd->SendMessage( WM_GETDLGCODE ) & DLGC_RADIOBUTTON ) != 0 
			&&	pWnd->SendMessage( BM_GETCHECK ) != BST_UNCHECKED
			)
			pWnd->SendMessage( BM_SETCHECK, BST_UNCHECKED );
		pWnd = pWndParent->GetNextDlgGroupItem( pWnd, FALSE );
	}
}

void CExtRadioButton::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
 	ASSERT_VALID( this );
	if( ! SupportsCheck() )
	{
		CExtCheckBox::_OnClick(
			bSelectAny,
			bSeparatedDropDownClicked
			);
		return;
	} // if( ! SupportsCheck() )

	if( SupportsAutoCheck() )
	{
		INT nCheck = GetCheck();
		if( !nCheck )
		{
			nCheck = !nCheck;
			_UncheckRadioButtonsInGroup();
		}
		SetCheck( nCheck );
	}

CWnd * pWnd = GetParent();
	if( pWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pWnd );
HWND hWndThis = GetSafeHwnd();
int nDlgCtrlID = GetDlgCtrlID();
	pWnd->SendMessage(
		WM_COMMAND,
		MAKEWPARAM(nDlgCtrlID,BN_CLICKED),
		(LPARAM)m_hWnd
		);
	if( ::IsWindow( hWndThis ) )
		Invalidate();
}

void CExtRadioButton::_RenderImpl(
	CDC & dc,
	bool bTransparent, // = false
	bool bCombinedContent // = false
	)
{
	ASSERT_VALID( this );
	bCombinedContent;

	if( (GetStyle()&BS_PUSHLIKE) != 0 )
	{
		CExtButton::_RenderImpl( dc, bTransparent, bCombinedContent );
		return;
	}
	
CRect rcClient;
	GetClientRect( &rcClient );
CExtSafeString sWindowText( _T("") );
	OnQueryWindowText( sWindowText );
bool bDrawFocusRect = (GetFocus() == this && m_bDrawFocusRect) ? true : false;
//bool bPushed = (m_bPushed || GetCheck()) ? true : false;
bool bPushed = m_bPushed;
//bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bHover =
		( m_bMouseOver && !CExtPopupMenuWnd::IsMenuTracking() ) ? true : false;
	if( GetParent()->IsKindOf(RUNTIME_CLASS(CControlBar)) )
		bDrawFocusRect = false;

	COLORREF clrBackground = GetBkColor();

bool _bTransparentBk = false;
	if(		PmBridge_GetPM()->GetCb2DbTransparentMode(this)
		&&	( clrBackground == COLORREF(-1L) )
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			_bTransparentBk = true;
	}
	if( ! _bTransparentBk )
		dc.FillSolidRect( 
			&rcClient, 
			(clrBackground != COLORREF(-1L)) 
				? clrBackground 
				: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
			);

HFONT hFont = OnQueryFont();
	if( ! PmBridge_GetPM()->AdjustControlFont(
			hFont,
			this
			)
		)
	{
		LOGFONT _lf;
		::memset( &_lf, 0, sizeof(LOGFONT) );
		::GetObject( (HGDIOBJ)hFont, sizeof(LOGFONT), (LPVOID) &_lf );
		if( GetDefault() )
			_lf.lfWeight = 
				(_lf.lfWeight > FW_BOLD) 
					? _lf.lfWeight 
					: FW_BOLD;
		hFont = ::CreateFontIndirect( &_lf );
	}
	ASSERT( hFont != NULL );
CFont _fontDestructor;
	_fontDestructor.Attach( hFont );

bool bLeftText = false;
DWORD dwStyle = GetStyle();
DWORD dwExStyle = GetExStyle();
	if(		(dwStyle&BS_LEFTTEXT) != 0
		||	(dwStyle&BS_RIGHTBUTTON) != 0
		||	(dwExStyle&WS_EX_RIGHT) != 0
		)
		bLeftText = true;

bool bMultiline = (dwStyle&BS_MULTILINE) != 0 ? true : false;

INT nCheck = GetCheck();
CExtPaintManager::eBoxState_t eState =
		CExtPaintManager::BOX_UNCHECKED;
	
	switch( nCheck ) 
	{
	case BST_UNCHECKED:
		if( bEnabled )
		{
			if(		( bHover && !m_bPushTracking) 
				||  (!bHover &&  m_bPushTracking && !m_bKeyTracking) 
				)
				eState = CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED;
			else if( (bHover && m_bPushTracking) || m_bKeyTracking )
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
			if(		( bHover && !m_bPushTracking) 
				||  (!bHover &&  m_bPushTracking && !m_bKeyTracking) 
				)
				eState = CExtPaintManager::BOX_MOUSE_HOVER_CHECKED;
			else if( (bHover && m_bPushTracking) || m_bKeyTracking )
				eState = CExtPaintManager::BOX_LDOWN_CHECKED;
			else
				eState = CExtPaintManager::BOX_CHECKED;
		}
		else
			eState = CExtPaintManager::BOX_DISABLED_CHECKED;
		break;
	}
	
CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd(
		this,
		eState,
		bLeftText,
		bMultiline,
		rcClient,
		(LPCTSTR) sWindowText,
		bHover,		
		bPushed,
		bEnabled,
		bDrawFocusRect, 
		OnQueryAlignmentText(),
		hFont,
		bTransparent
		);

	_pcrbd.m_rcTextMargins = OnQueryMarginsText();
	
	_pcrbd.m_clrForceBk = GetBkColor();
	_pcrbd.m_clrForceTextDisabled = GetTextColorDisabled();
	_pcrbd.m_clrForceTextNormal = GetTextColorNormal();
	_pcrbd.m_clrForceTextHover = GetTextColorHover();
	_pcrbd.m_clrForceTextPressed = GetTextColorPressed();

	PmBridge_GetPM()->PaintRadioButton( dc, _pcrbd );
	
	// by default this method does nothing
	OnPaintBox( 
		dc, 
		_pcrbd.m_rcBox, 
		_pcrbd.m_eState 
		);
	
	PmBridge_GetPM()->OnPaintSessionComplete( this );
}

bool CExtRadioButton::OnPaintBox(
	CDC & dc,
	CRect & rcBox,
	CExtPaintManager::eBoxState_t eState
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcBox;
	eState;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtHyperLinkButton

IMPLEMENT_DYNCREATE( CExtHyperLinkButton, CExtButton );

CExtHyperLinkButton::CExtHyperLinkButton()
	: m_bUseStdCommandNotification( false )
	, m_bAllowVisited( true )
	, m_bVisited( false )
	, m_clrTextVisited( RGB( 128, 0, 128 ) )
	, m_bTextUnderline( false )
	, m_bAutoAdjustSizeByText( true )
{
	m_rcTextMargins = CRect( 2, 2, 2, 2 );
	m_bQuickActivationEnabled = false;
	m_clrTextNormal = RGB( 0, 0, 255 );
	m_clrTextHover = RGB( 255, 0, 0 );
	
	SetBtnCursor( IDC_EXT_HAND );
}

CExtHyperLinkButton::~CExtHyperLinkButton()
{
}

BEGIN_MESSAGE_MAP(CExtHyperLinkButton, CExtButton)
//{{AFX_MSG_MAP(CExtHyperLinkButton)
//}}AFX_MSG_MAP
    ON_MESSAGE( WM_SETTEXT, OnSetText )
END_MESSAGE_MAP()

LRESULT CExtHyperLinkButton::OnSetText( WPARAM wParam, LPARAM lParam )
{ // Added by Waldemar Witt
	wParam;
	if( ! m_bAutoAdjustSizeByText )
		return Default();
LPCTSTR strTmp = (LPCTSTR)lParam;
CClientDC dc(this);
CSize szRectTxt = dc.GetTextExtent( strTmp );
// add a couple blank spaces as well as the border spacing in
CSize szSpace = dc.GetTextExtent( _T("  ") );
	szSpace.cx += 2 * ::GetSystemMetrics( SM_CXBORDER );
CRect rcRect;
	GetWindowRect( &rcRect );
	if( ( szRectTxt.cx + szSpace.cx ) != rcRect.Width() )
		SetWindowPos( NULL, 0, 0, szRectTxt.cx + szSpace.cx, rcRect.Height(), SWP_NOMOVE | SWP_NOZORDER );
	return Default();
}  

void CExtHyperLinkButton::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
	ASSERT_VALID( this );
	if( m_bUseStdCommandNotification )
	{
		HWND hWndOwn = m_hWnd;
		CWnd * pWndOwn = this;
		CExtButton::_OnClick(
			bSelectAny,
			bSeparatedDropDownClicked
			);
		if( ::IsWindow( hWndOwn ) && CWnd::FromHandlePermanent( hWndOwn ) == pWndOwn )
		{
			m_bVisited = true;
			Invalidate();
		}
		return;
	} // if( m_bUseStdCommandNotification )
	if( m_strURL.IsEmpty() )
	{
		CExtSafeString strURL;
		OnQueryWindowText( strURL );
		strURL.TrimLeft();
		strURL.TrimRight();
		VERIFY( OnHyperLinkOpen( LPCTSTR(strURL) ) );
	} // if( m_strURL.IsEmpty() )
	else
		VERIFY( OnHyperLinkOpen( LPCTSTR(m_strURL) ) );
}

void CExtHyperLinkButton::_RenderImpl( // non-buffered paint
	CDC & dc,
	bool bTransparent, // = false
	bool bCombinedContent // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bTransparent;
	bCombinedContent;
CRect rcClient;
	GetClientRect( &rcClient );
COLORREF clrBackground = GetBkColor();

bool _bTransparentBk = false;
	if(		PmBridge_GetPM()->GetCb2DbTransparentMode(this)
		&&	( clrBackground == COLORREF(-1L) )
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
				);
		if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			_bTransparentBk = true;
	}
	if( ! _bTransparentBk )
		dc.FillSolidRect( 
			&rcClient, 
			(clrBackground != COLORREF(-1L)) 
				? clrBackground 
				: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
		);

bool bFocus = (GetFocus() == this) ? true : false;
bool bPushed = m_bPushed ? true : false;

//bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bHover =
		( m_bMouseOver && !CExtPopupMenuWnd::IsMenuTracking() ) ? true : false;
bool bVisited = (m_bVisited && m_bAllowVisited) ? true : false;
	OnPaintHyperLink(
		dc,
		rcClient,
		bVisited,
		bEnabled,
		m_bDrawFocusRect,
		bFocus,
		bHover,
		bPushed
		);
}

COLORREF CExtHyperLinkButton::OnQueryHyperLinkTextColor(
	bool bVisited,
	bool bEnabled,
	bool bDrawFocusRect,
	bool bFocus,
	bool bHover,
	bool bPushed
	) const
{
	ASSERT_VALID( this );
	bDrawFocusRect;
CWindowDC dcFake( NULL );
COLORREF clrText =
		PmBridge_GetPM()->QueryObjectTextColor(
			dcFake,
			bEnabled,
			bFocus,
			bHover,
			bPushed,
			(CObject*)this
			);
	if( clrText != COLORREF(-1L) )
		return clrText;
	clrText = m_clrTextNormal;
	if( bEnabled )
	{
		if( bHover || bPushed || bFocus )
			clrText = m_clrTextHover;
		else if( bVisited )
			clrText = m_clrTextVisited;
		if( clrText == COLORREF(-1L) )
			clrText =
				PmBridge_GetPM()->GetColor(
					COLOR_BTNTEXT, (CObject*)this
					);
	} // if( bEnabled )
	else
	{
		if( m_clrTextDisabled == COLORREF(-1L) )
			clrText =
				PmBridge_GetPM()->GetColor( 
					CExtPaintManager::CLR_TEXT_DISABLED, 
					(CObject*)this
					);
		else
			clrText = m_clrTextDisabled;
	} // else from if( bEnabled )
	return clrText;
}

void CExtHyperLinkButton::OnPaintHyperLink(
	CDC & dc,
	CRect rcText,
	bool bVisited,
	bool bEnabled,
	bool bDrawFocusRect,
	bool bFocus,
	bool bHover,
	bool bPushed
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtSafeString strHyperLinkURL = OnQueryHyperLinkURL( true );
	if( strHyperLinkURL.IsEmpty() )
		return;
CRect rcInitial( rcText );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );

HFONT hFont = OnQueryFont();
	if( ! PmBridge_GetPM()->AdjustControlFont(
			hFont,
			(CObject*)this
			)
		)
	{
		LOGFONT _lf;
		::memset( &_lf, 0, sizeof(LOGFONT) );
		::GetObject( (HGDIOBJ)hFont, sizeof(LOGFONT), (LPVOID) &_lf );
		if( GetDefault() )
			_lf.lfWeight = 
				(_lf.lfWeight > FW_BOLD) 
					? _lf.lfWeight 
					: FW_BOLD;
		if(		( bHover /*|| bFocus*/ || bPushed )
			&&	GetTextUnderline()
			)
			_lf.lfUnderline = 1;
		hFont = ::CreateFontIndirect( &_lf );
	}
	ASSERT( hFont != NULL );
CFont _fontDestructor;
	_fontDestructor.Attach( hFont );

CFont * pFont = CFont::FromHandle( hFont );
CFont * pFontOld = dc.SelectObject( pFont );

COLORREF clrText =
			OnQueryHyperLinkTextColor(
				bVisited,
				bEnabled,
				bDrawFocusRect,
				bFocus,
				bHover,
				bPushed
				);
COLORREF clrTextOld = dc.SetTextColor( clrText );

INT nTextLength = strHyperLinkURL.GetLength();

CRect rcTextMargins = OnQueryMarginsText();
CRect rcTextLocation( 0, 0, 0, 0);
	rcText.DeflateRect( rcTextMargins );

DWORD dwStyle = GetStyle();
bool bMultiline = (dwStyle&BS_MULTILINE) != 0 ? true : false;

UINT nDtMeasureFlags =
		DT_LEFT|DT_TOP|DT_CALCRECT;

	if( bMultiline )
	{
		rcTextLocation = rcText;
		rcTextLocation.OffsetRect( -rcTextLocation.TopLeft() );
		rcTextLocation.bottom = rcTextLocation.top;
		nDtMeasureFlags |= DT_WORDBREAK;
	}
	else
		nDtMeasureFlags |= DT_SINGLELINE;

	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		LPCTSTR( strHyperLinkURL ), 
		nTextLength,
		rcTextLocation,
		nDtMeasureFlags, 0
		);
	rcTextLocation.OffsetRect(
		rcText.TopLeft() - rcTextLocation.TopLeft()
		);
	
INT nAlign = OnQueryAlignmentText();
UINT nDtDrawFlags = 0;
	if( (nAlign&CExtPaintManager::__ALIGN_HORIZ_MASK) == CExtPaintManager::__ALIGN_HORIZ_RIGHT )
	{
		nDtDrawFlags |= DT_RIGHT;
		rcTextLocation.OffsetRect(
			rcText.Width() - rcTextLocation.Width(),
			0
			);
	}
	else if( (nAlign&CExtPaintManager::__ALIGN_HORIZ_MASK) == CExtPaintManager::__ALIGN_HORIZ_CENTER )
	{
		nDtDrawFlags |= DT_CENTER;
		rcTextLocation.OffsetRect(
			( rcText.Width() - rcTextLocation.Width() ) / 2,
			0
			);
	}
	else
		nDtDrawFlags |= DT_LEFT;
	
	if( (nAlign&CExtPaintManager::__ALIGN_VERT_MASK) == CExtPaintManager::__ALIGN_VERT_BOTTOM )
	{
		nDtDrawFlags |= DT_BOTTOM;
		rcTextLocation.OffsetRect(
			0,
			rcText.Height() - rcTextLocation.Height()
			);
	}
	else if( (nAlign&CExtPaintManager::__ALIGN_VERT_MASK) == CExtPaintManager::__ALIGN_VERT_TOP )
	{
		nDtDrawFlags |= DT_TOP;
	}
	else
	{
		nDtDrawFlags |= DT_VCENTER;
		rcTextLocation.OffsetRect(
			0,
			( rcText.Height() - rcTextLocation.Height() ) / 2
			);
	}

	if( bMultiline )
		nDtDrawFlags |= DT_WORDBREAK;
	else
		nDtDrawFlags |= DT_SINGLELINE;

	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		LPCTSTR( strHyperLinkURL ),
		nTextLength,
		rcTextLocation,
		nDtDrawFlags, 0
		);

CRect rcFocus( rcTextLocation );
	rcFocus.InflateRect( 2, 2, 2, 1 );		

	dc.SetTextColor( clrTextOld );
	dc.SelectObject( pFontOld );
	dc.SetBkMode( nOldBkMode );

	if( bDrawFocusRect && bFocus )
		dc.DrawFocusRect( &rcFocus );
}

CExtSafeString CExtHyperLinkButton::OnQueryHyperLinkURL(
	bool bDisplayURL // true - display URL, false - navigate URL
	) const
{
	ASSERT_VALID( this );
	bDisplayURL;
CExtSafeString strHyperLinkURL;
	OnQueryWindowText( strHyperLinkURL );
	return strHyperLinkURL;
}

bool CExtHyperLinkButton::OnHyperLinkOpen(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	) const
{
	if(		_tcsnicmp( strURL, _T("http:////"), 7 ) == 0
		||	_tcsnicmp( strURL, _T("file:////"), 7 ) == 0
		||	_tcsnicmp( strURL, _T("www."), 4 ) == 0
		||	_tcsnicmp( strURL, _T("web."), 4 ) == 0
		)
	{
		if( ! _OpenVia_RegInfo( strURL, eOM, bAllowErrorAlert ) )
		{
			if( ! _OpenVia_COM( strURL, eOM, bAllowErrorAlert ) )
			{
				if( ! _OpenVia_Shell( strURL, eOM, bAllowErrorAlert ) )
				{
						TRACE0( "*** Failed to open hyper link (CExtHyperLinkButton::OnHyperLinkOpen) ***\n" );
						return false;
				}
			}
		}
	}
	else
	{
		int nLen = int( _tcslen( LPCTSTR(strURL) ) );
		if( nLen > 4 )
		{ // open .LNK shortcuts in a special way
			LPCTSTR strExtLookup = LPCTSTR(strURL) + nLen - 4;
			if(		_tcsnicmp( strExtLookup, _T(".lnk"), 4 ) == 0
				||	_tcsnicmp( strExtLookup, _T(".cpl"), 4 ) == 0
				)
			{
				HINSTANCE hInstance =
					::ShellExecute(
						::GetDesktopWindow(), // GetParent()->GetSafeHwnd(),
						g_PaintManager.m_bIsWin2000orLater ? LPCTSTR(NULL) : _T("open"),
						LPCTSTR(strURL),
						NULL,
						NULL,
						SW_SHOW
						);
				if( __EXT_MFC_LONG_PTR(hInstance) <= __EXT_MFC_LONG_PTR(HINSTANCE_ERROR) )
					return false;
				return true;
			}
		} // open .LNK shortcuts in a special way
		if( ! _OpenVia_COM( strURL, eOM ) )
		{
			if( ! _OpenVia_Shell( strURL, eOM ) )
			{
				if( ! _OpenVia_RegInfo( strURL, eOM ) )
				{
					TRACE0( "*** Failed to open hyper link (CExtHyperLinkButton::OnHyperLinkOpen) ***\n" );
					return false;
				}
			}
		}
	}
	m_bVisited = true;
	((CWnd *)this)->Invalidate();
	return true;
}

bool CExtHyperLinkButton::stat_HyperLinkOpen(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	)
{
	if(		_tcsnicmp( strURL, _T("http:////"), 7 ) == 0
		||	_tcsnicmp( strURL, _T("file:////"), 7 ) == 0
		||	_tcsnicmp( strURL, _T("www."), 4 ) == 0
		||	_tcsnicmp( strURL, _T("web."), 4 ) == 0
		)
	{
		if( ! stat_OpenVia_RegInfo( strURL, eOM, bAllowErrorAlert ) )
		{
			if( ! stat_OpenVia_COM( strURL, eOM, bAllowErrorAlert ) )
			{
				if( ! stat_OpenVia_Shell( strURL, eOM, bAllowErrorAlert ) )
				{
						TRACE0( "*** Failed to open hyper link (CExtHyperLinkButton::stat_HyperLinkOpen) ***\n" );
						return false;
				}
			}
		}
	}
	else
	{
		int nLen = int( _tcslen( LPCTSTR(strURL) ) );
		if( nLen > 4 )
		{ // open .LNK shortcuts in a special way
			LPCTSTR strExtLookup = LPCTSTR(strURL) + nLen - 4;
			if(		_tcsnicmp( strExtLookup, _T(".lnk"), 4 ) == 0
				||	_tcsnicmp( strExtLookup, _T(".cpl"), 4 ) == 0
				)
			{
				HINSTANCE hInstance =
					::ShellExecute(
						::GetDesktopWindow(), // GetParent()->GetSafeHwnd(),
						g_PaintManager.m_bIsWin2000orLater ? LPCTSTR(NULL) : _T("open"),
						LPCTSTR(strURL),
						NULL,
						NULL,
						SW_SHOW
						);
				if( __EXT_MFC_LONG_PTR(hInstance) <= __EXT_MFC_LONG_PTR(HINSTANCE_ERROR) )
					return false;
				return true;
			}
		} // open .LNK shortcuts in a special way
		if( ! stat_OpenVia_COM( strURL, eOM ) )
		{
			if( ! stat_OpenVia_Shell( strURL, eOM ) )
			{
				if( ! stat_OpenVia_RegInfo( strURL, eOM ) )
				{
					TRACE0( "*** Failed to open hyper link (CExtHyperLinkButton::stat_HyperLinkOpen) ***\n" );
					return false;
				}
			}
		}
	}
	return true;
}

bool CExtHyperLinkButton::_OpenVia_COM(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	) const
{
	ASSERT_VALID( this );
	return stat_OpenVia_COM( strURL, eOM, bAllowErrorAlert );
}

bool CExtHyperLinkButton::stat_OpenVia_COM(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	)
{
	ASSERT( strURL != NULL );
	ASSERT( _tcslen( strURL ) > 0 );
	bAllowErrorAlert;
IUniformResourceLocator *pURL;
HRESULT hr =
		::CoCreateInstance(
			CLSID_InternetShortcut,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IUniformResourceLocator,
			(LPVOID*)&pURL
			);
	if( FAILED(hr) )
	{
		//ASSERT( FALSE );
		return false;
	}
	hr =
		pURL->SetURL(
			LPCTSTR(strURL),
			IURL_SETURL_FL_GUESS_PROTOCOL
			);
	if( FAILED(hr) )
	{
		//ASSERT( FALSE );
		pURL->Release();
		return false;
	}
URLINVOKECOMMANDINFO _uici;
	::memset( &_uici, 0, sizeof(URLINVOKECOMMANDINFO) );
	_uici.dwcbSize = sizeof(URLINVOKECOMMANDINFO);
	_uici.dwFlags = IURL_INVOKECOMMAND_FL_ALLOW_UI;
	_uici.hwndParent = ::GetDesktopWindow(); // GetParent()->GetSafeHwnd();
	switch( eOM )
	{
	case __EOM_OPEN:
		_uici.pcszVerb = _T("open");
	break;
	case __EOM_EDIT:
		_uici.pcszVerb = _T("edit");
	break;
	case __EOM_PRINT:
		_uici.pcszVerb = _T("print");
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eOM )
	hr = pURL->InvokeCommand(&_uici);
	if( FAILED(hr) )
	{
		//ASSERT( FALSE );
		pURL->Release();
		return false;
	}
	pURL->Release();
	return true;
}

bool CExtHyperLinkButton::_OpenVia_Shell(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	) const
{
	ASSERT_VALID( this );
	return stat_OpenVia_Shell( strURL, eOM, bAllowErrorAlert );
}

bool CExtHyperLinkButton::stat_OpenVia_Shell(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	)
{
	ASSERT( strURL != NULL );
	ASSERT( _tcslen( strURL ) > 0 );
LPCTSTR strOpenMode = NULL;
	switch( eOM )
	{
	case __EOM_OPEN:
		strOpenMode = _T("open");
	break;
	case __EOM_EDIT:
		strOpenMode = _T("edit");
	break;
	case __EOM_PRINT:
		strOpenMode = _T("print");
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eOM )
	ASSERT( strOpenMode != NULL );
HINSTANCE hInstance =
		::ShellExecute(
			::GetDesktopWindow(), // GetParent()->GetSafeHwnd(),
			strOpenMode,
			LPCTSTR(strURL),
			NULL,
			NULL,
			SW_SHOW
			);
	if( __EXT_MFC_LONG_PTR(hInstance) <= __EXT_MFC_LONG_PTR(HINSTANCE_ERROR) )
	{
		if( bAllowErrorAlert )
		{
			CExtSafeString strErrorDescription;
			switch( __EXT_MFC_LONG_PTR(hInstance) )
			{
			case 0:
				strErrorDescription =
					_T("OS is out of memory or resources.");
			break;
			case SE_ERR_PNF:
				strErrorDescription = _T("Path was not found.");
			break;
			case SE_ERR_FNF:
				strErrorDescription =
					_T("File was not found.");
			break;
			case ERROR_BAD_FORMAT:
				strErrorDescription =
					_T("EXE file is invalid (must run on other platform or error in its image).");
			break;
			case SE_ERR_ACCESSDENIED:
				strErrorDescription =
					_T("OS denied access to the file.");
			break;
			case SE_ERR_ASSOCINCOMPLETE:
				strErrorDescription =
					_T("The filename association is incomplete or invalid.");
			break;
			case SE_ERR_DDEBUSY:
				strErrorDescription =
					_T("DDE transaction was not completed due to other DDE transactions.");
			break;
			case SE_ERR_DDEFAIL:
				strErrorDescription =
					_T("DDE transaction failed.");
			break;
			case SE_ERR_DDETIMEOUT:
				strErrorDescription =
					_T("DDE transaction could not be completed due to request timed out.");
			break;
			case SE_ERR_DLLNOTFOUND:
				strErrorDescription =
					_T("DLL was not found.");
			break;
			case SE_ERR_NOASSOC:
				strErrorDescription =
					_T("There is no application associated with the given filename extension.");
			break;
			case SE_ERR_OOM:
				strErrorDescription =
					_T("Not enough memory.");
			break;
			case SE_ERR_SHARE:
				strErrorDescription =
					_T("A sharing violation occurred.");
			break;
			default:
				strErrorDescription.Format(
					_T("Unknown Error (%d=0x%X) occurred."), __EXT_MFC_LONG_PTR(hInstance), __EXT_MFC_LONG_PTR(hInstance) );
			break;
			} // switch( LONG(hInstance) )
			CExtSafeString strErrorMessage;
			strErrorMessage.Format(
				_T("Unable to open hyper link:\n%s"),
				LPCTSTR(strErrorDescription)
				);
			UINT nType = MB_OK|MB_ICONERROR;
			if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
				nType |= MB_RTLREADING;
#if (! defined __EXT_MFC_NO_MSG_BOX )
			::ProfUISMsgBox( NULL, LPCTSTR(strErrorDescription), NULL, nType );
#else
			::AfxMessageBox( LPCTSTR(strErrorDescription), nType );
#endif
		} // if( bAllowErrorAlert )
		return false;
	} // if( LONG(hInstance) <= LONG(HINSTANCE_ERROR) )
	return true;
}

bool CExtHyperLinkButton::_OpenVia_RegInfo(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	) const
{
	ASSERT_VALID( this );
	return stat_OpenVia_RegInfo( strURL, eOM, bAllowErrorAlert );
}

bool CExtHyperLinkButton::stat_OpenVia_RegInfo(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	)
{
	ASSERT( strURL != NULL );
	ASSERT( _tcslen( strURL ) > 0 );
TCHAR strKey[ MAX_PATH + MAX_PATH + 2];
HINSTANCE hInstance = NULL;
LPTSTR strExt = _T(".htm");
HKEY hRegKey;
//	if( GetRegKey(HKEY_CLASSES_ROOT, strExt, strKey) != ERROR_SUCCESS )
//		return false;
	if( ::RegOpenKeyEx( HKEY_CLASSES_ROOT, strExt, 0, KEY_QUERY_VALUE, &hRegKey ) != ERROR_SUCCESS )
		return false;
	{ // BLOCK
		long nSize = MAX_PATH;
		TCHAR strData[MAX_PATH];
		::RegQueryValue( hRegKey, NULL, strData, &nSize );
		__EXT_MFC_STRCPY( strKey, sizeof(strKey)/sizeof(strKey[0]), strData );
		::RegCloseKey( hRegKey );
	} // BLOCK
LPCTSTR strOpenMode = NULL;
	switch( eOM )
	{
	case __EOM_OPEN:
		strOpenMode = _T("\\shell\\open\\command");
	break;
	case __EOM_EDIT:
		strOpenMode = _T("\\shell\\edit\\command");
	break;
	case __EOM_PRINT:
		strOpenMode = _T("\\shell\\print\\command");
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eOM )
	ASSERT( strOpenMode != NULL );
	__EXT_MFC_STRCAT( strKey, sizeof(strKey)/sizeof(strKey[0]), strOpenMode );
//	if( GetRegKey(HKEY_CLASSES_ROOT, strKey, strKey) != ERROR_SUCCESS )
//		return false;
	if( ::RegOpenKeyEx( HKEY_CLASSES_ROOT, strKey, 0, KEY_QUERY_VALUE, &hRegKey ) != ERROR_SUCCESS )
		return false;
	{ // BLOCK
		long nSize = MAX_PATH;
		TCHAR strData[MAX_PATH];
		::RegQueryValue( hRegKey, NULL, strData, &nSize );
		__EXT_MFC_STRCPY( strKey, MAX_PATH, strData );
		::RegCloseKey( hRegKey );
	} // BLOCK
	
LPTSTR strPos = _tcsstr( strKey, _T("\"%1\"") );
	if( strPos == NULL )
		strPos = _tcsstr( strKey, _T("%1") );
	if( strPos != NULL )
		*strPos = _T('\0');
	__EXT_MFC_STRCAT( strKey, sizeof(strKey)/sizeof(strKey[0]), _T(" ") );
	__EXT_MFC_STRCAT( strKey, sizeof(strKey)/sizeof(strKey[0]), LPCTSTR(strURL) );

USES_CONVERSION;
	hInstance = (HINSTANCE)(__EXT_MFC_UINT_PTR)::WinExec(T2A(strKey), SW_SHOW);
	if( __EXT_MFC_LONG_PTR(hInstance) <= __EXT_MFC_LONG_PTR(HINSTANCE_ERROR) )
	{
		if( bAllowErrorAlert )
		{
			CExtSafeString strErrorDescription;
			switch( __EXT_MFC_LONG_PTR(hInstance) )
			{
			case 0:
				strErrorDescription =
					_T("OS is out of memory or resources.");
			break;
			case SE_ERR_PNF:
				strErrorDescription = _T("Path was not found.");
			break;
			case SE_ERR_FNF:
				strErrorDescription =
					_T("File was not found.");
			break;
			case ERROR_BAD_FORMAT:
				strErrorDescription =
					_T("EXE file is invalid (must run on other platform or error in its image).");
			break;
			case SE_ERR_ACCESSDENIED:
				strErrorDescription =
					_T("OS denied access to the file.");
			break;
			case SE_ERR_ASSOCINCOMPLETE:
				strErrorDescription =
					_T("The filename association is incomplete or invalid.");
			break;
			case SE_ERR_DDEBUSY:
				strErrorDescription =
					_T("DDE transaction was not completed due to other DDE transactions.");
			break;
			case SE_ERR_DDEFAIL:
				strErrorDescription =
					_T("DDE transaction failed.");
			break;
			case SE_ERR_DDETIMEOUT:
				strErrorDescription =
					_T("DDE transaction could not be completed due to request timed out.");
			break;
			case SE_ERR_DLLNOTFOUND:
				strErrorDescription =
					_T("DLL was not found.");
			break;
			case SE_ERR_NOASSOC:
				strErrorDescription =
					_T("There is no application associated with the given filename extension.");
			break;
			case SE_ERR_OOM:
				strErrorDescription =
					_T("Not enough memory.");
			break;
			case SE_ERR_SHARE:
				strErrorDescription =
					_T("A sharing violation occurred.");
			break;
			default:
				strErrorDescription.Format(
					_T("Unknown Error (%d=0x%X) occurred."), __EXT_MFC_LONG_PTR(hInstance), __EXT_MFC_LONG_PTR(hInstance) );
			break;
			} // switch( LONG(hInstance) )
			CExtSafeString strErrorMessage;
			strErrorMessage.Format(
				_T("Unable to open hyper link:\n%s"),
				LPCTSTR(strErrorDescription)
				);
			UINT nType = MB_OK|MB_ICONERROR;
			if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
				nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
			::ProfUISMsgBox( NULL, LPCTSTR(strErrorDescription), NULL, nType );
#else
			::AfxMessageBox( LPCTSTR(strErrorDescription), nType );
#endif
		} // if( bAllowErrorAlert )
		return false;
	} // if( LONG(hInstance) <= LONG(HINSTANCE_ERROR) )
	return true;
}

void CExtHyperLinkButton::SetTextColorVisited( 
	COLORREF clrText 
	)
{ 
	ASSERT_VALID( this );
	m_clrTextVisited = clrText; 
	Invalidate();
}

COLORREF CExtHyperLinkButton::GetTextColorVisited()
{
	ASSERT_VALID( this );
	return m_clrTextVisited; 
}

void CExtHyperLinkButton::SetTextUnderline( 
	bool bTextUnderline 
	)
{ 
	ASSERT_VALID( this );
	m_bTextUnderline = bTextUnderline; 
	Invalidate();
}

bool CExtHyperLinkButton::GetTextUnderline() const
{
	ASSERT_VALID( this );
	return m_bTextUnderline; 
}

/////////////////////////////////////////////////////////////////////////////
// CExtIconButton

IMPLEMENT_DYNCREATE( CExtIconButton, CExtButton );

CExtIconButton::CExtIconButton()
	: m_rcFocusRectPadding( 1, 1, 1, 1 )
{
}

CExtIconButton::~CExtIconButton()
{
}

BEGIN_MESSAGE_MAP( CExtIconButton, CExtButton )
//{{AFX_MSG_MAP(CExtIconButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtIconButton::_RenderImpl(
	CDC & dc,
	bool bTransparent, // = false
	bool bCombinedContent // = false
	)
{
	bTransparent;
	ASSERT_VALID( this );
	m_bSeparatedDropDown = false; // should never become a split button
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
CRect rcClient;
	GetClientRect( &rcClient );
COLORREF clrBackground = GetBkColor();
bool _bTransparentBk = false;
	if(		pPM->GetCb2DbTransparentMode(this)
		&&	( clrBackground == COLORREF(-1L) )
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( pPM->PaintDockerBkgnd( true, dc, this ) )
			_bTransparentBk = true;
	}
	if( ! _bTransparentBk )
		dc.FillSolidRect( 
			&rcClient, 
			(clrBackground != COLORREF(-1L)) 
				? clrBackground 
				: pPM->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
			);
	if( ! m_icon.IsEmpty() )
	{
		//bool bDefault = GetDefault() ? true : false;
		bool bPushed = m_bPushed ? true : false;
		bool bEnabled = OnQueryWindowEnabledState();
		//bool bFlat = GetFlat() ? true : false;
		bool bHover = ( m_bMouseOver && !CExtPopupMenuWnd::IsMenuTracking() ) ? true : false;
		CExtCmdIcon::e_paint_type_t ePT =
			bEnabled
				? ( bPushed
						? CExtCmdIcon::__PAINT_PRESSED
						: ( bHover
								? CExtCmdIcon::__PAINT_HOVER
								: CExtCmdIcon::__PAINT_NORMAL
								)
						)
				: CExtCmdIcon::__PAINT_DISABLED
				;
		CSize _sizeClient = rcClient.Size();
		CSize _sizeIcon = m_icon.GetSize();
		CRect rcIcon(
			rcClient.left,
			rcClient.top,
			rcClient.left + _sizeIcon.cx,
			rcClient.top + _sizeIcon.cy
			);
		if( _sizeClient != _sizeIcon )
			rcIcon.OffsetRect(
				( _sizeClient.cx - _sizeIcon.cx ) / 2,
				( _sizeClient.cy - _sizeIcon.cy ) / 2
				);
		m_icon.Paint( pPM, dc.m_hDC, rcIcon, ePT );
	} // if( ! m_icon.IsEmpty() )
bool bDrawFocusRect = ( (!bCombinedContent) && GetFocus() == this && m_bDrawFocusRect ) ? true : false;
	if( GetParent()->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		bDrawFocusRect = false;
		//bFlat = true;
	}
	if( bDrawFocusRect )
	{
		CRect rcFocus( rcClient );
		rcFocus.DeflateRect( &m_rcFocusRectPadding );
		dc.DrawFocusRect( &rcClient );
	}
}

/////////////////////////////////////////////////////////////////////////////



