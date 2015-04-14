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

#if (!defined __EXT_EDIT_H)
	#include <ExtEdit.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtEditBase

IMPLEMENT_DYNCREATE( CExtEditBase, CEdit );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtEditBase );

HWND CExtEditBase::g_hWndEditInAltKeyCodeMode = NULL;

CExtEditBase::CExtEditBase()
	: m_bHandleCtxMenus( true )
	, m_clrBack( COLORREF(-1L) )
	, m_clrBackPrev( COLORREF(-1L) )
	, m_clrText( COLORREF(-1L) )
	, m_bRichMode( false )
	, m_bHandleAltKeyCodes( true )
	, m_bAltKeyCodeMode( false )
	, m_nTrackedAltKeyCode( 0L )
	, m_bToolTipActive( false )
	, m_nAdvancedTipStyle( INT(CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON) )
{
	EnableToolTips();
	PmBridge_Install();
}

CExtEditBase::~CExtEditBase()
{
	PmBridge_Uninstall();
	if( m_brBack.GetSafeHandle() != NULL )
		m_brBack.DeleteObject();
}

BEGIN_MESSAGE_MAP( CExtEditBase, CEdit )
	//{{AFX_MSG_MAP(CExtEditBase)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtPopupMenuTipWnd * CExtEditBase::OnAdvancedPopupMenuTipWndGet() const
{
	if( m_nAdvancedTipStyle == INT(CExtPopupMenuTipWnd::__ETS_NONE) )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

void CExtEditBase::OnAdvancedPopupMenuTipWndDisplay(
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

void CExtEditBase::OnMouseMove(UINT nFlags, CPoint point)
{
	CEdit::OnMouseMove( nFlags, point );

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

DWORD CALLBACK CExtEditBase::stat_Rich_CallBack_StreamIn( __EXT_MFC_DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb )
{
	ASSERT( pcb != NULL );
CExtSafeString * pStr = (CExtSafeString*)dwCookie;
	ASSERT( pStr != NULL );
LONG nLength = pStr->GetLength();
//	if( nLength < cb )
//	{
//		(*pcb) = nLength;
//		::memcpy( pbBuff, (LPCTSTR)*pStr, (*pcb)*sizeof(TCHAR) );
//		pStr->Empty();
//	}
//	else
//	{
//		(*pcb) = cb;
//		::memcpy( pbBuff, (LPCTSTR)*pStr, (*pcb)*sizeof(TCHAR) );
//		*pStr = pStr->Right( nLength - cb );
//	}
USES_CONVERSION;
	if( nLength < cb )
	{
		LPCSTR strA = T2CA( LPCTSTR(*pStr) );
		(*pcb) = nLength;
		::memcpy( pbBuff, LPVOID(strA), (*pcb)*sizeof(CHAR) );
		pStr->Empty();
	}
	else
	{
		CExtSafeString strPart = pStr->Left( cb );
		LPCSTR strA = T2CA( LPCTSTR(strPart) );
		(*pcb) = cb;
		::memcpy( pbBuff, LPVOID(strA), (*pcb) * sizeof(CHAR) );
		*pStr = pStr->Right( nLength - cb );
	}
	return 0;
}

DWORD CALLBACK CExtEditBase::stat_Rich_CallBack_StreamOut( __EXT_MFC_DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb )
{
	ASSERT( pcb != NULL );
	(*pcb) = 0;
CExtSafeString * pStr = (CExtSafeString*) dwCookie;
	if( cb <= 0 )
		return 0;
CHAR * pLocalBuffer = (CHAR *) ::malloc( cb + 1 );
	if( pLocalBuffer == NULL )
		return 0;
	pLocalBuffer[ cb ] = CHAR(0);
	::memcpy( pLocalBuffer, pbBuff, cb * sizeof(CHAR) );
	USES_CONVERSION;
LPCTSTR strT = A2CT(pLocalBuffer);
	(*pStr) += strT;
	::free( pLocalBuffer );
	(*pcb) = cb;
	return 0;
}

void CExtEditBase::OnPaint()
{
    // Let base class do it's thing
    CEdit::OnPaint();
    // Now we can do our painting for cue banner stuff
	if( GetWindowTextLength() > 0 || ::GetFocus() == m_hWnd )
        return;
CExtSafeString strCueBanner = GetCueBanner();
    if( strCueBanner.IsEmpty() )
        return;
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
LOGFONT _lf;
	::memset( &_lf, 0 , sizeof(LOGFONT) );
    if( ! pPM->m_FontNormal.GetLogFont( &_lf ) )
		return;
CFont _font;
	_lf.lfItalic = TRUE;
	if( ! _font.CreateFontIndirect( &_lf ) )
		return;
CRect rcClient;
	GetClientRect( &rcClient );
CRect rcCueBanner = rcClient;
	rcCueBanner.DeflateRect( 3, 1 );
CClientDC dc( this );
CFont * pOldFont = dc.SelectObject( &_font );
COLORREF clrOldText = dc.SetTextColor( pPM->GetColor( COLOR_GRAYTEXT ) );
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(strCueBanner), strCueBanner.GetLength(), &rcCueBanner, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, 0 );
	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrOldText );
	dc.SelectObject( pOldFont );
}

bool CExtEditBase::SetCueBanner( __EXT_MFC_SAFE_LPCTSTR lpcText )
{
    ASSERT_VALID( this );
	if( lpcText == NULL )
		m_strCueBanner.Empty();
	else
		m_strCueBanner = lpcText;
	if( GetSafeHwnd() != NULL && GetWindowTextLength() > 0 && ::GetFocus() != m_hWnd )
		Invalidate();
    return true;
}

bool CExtEditBase::GetCueBanner( __EXT_MFC_SAFE_LPTSTR lpText, int cchText ) const
{
    ASSERT_VALID( this );
    if( lpText == NULL )
		return false;
	__EXT_MFC_STRNCPY( lpText, cchText, LPCTSTR(m_strCueBanner), m_strCueBanner.GetLength() );
    return true;
}

CExtSafeString CExtEditBase::GetCueBanner() const
{
    ASSERT_VALID( this );
	return m_strCueBanner;
}

void CExtEditBase::OnContextMenu(CWnd* pWnd,CPoint pos )
{
	if( ! m_bHandleCtxMenus )
	{
		CEdit::OnContextMenu( pWnd, pos );
		return;
	} // if( ! m_bHandleCtxMenus )
LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() );
	if( strProfileName == NULL )
	{
		CEdit::OnContextMenu( pWnd, pos );
		return;
	}
CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			GetParent()->GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	if( ! pPopup->CreatePopupMenu( GetSafeHwnd() ) )
	{
		ASSERT( FALSE );
		delete pPopup;
		CEdit::OnContextMenu( pWnd, pos );
		return;
	}
static struct
{
	UINT m_nCmdID;
	LPCTSTR m_sMenuText;
} arrCmds[] =
{
	{ ID_EDIT_UNDO, _T("&Undo") },
//	{ ID_EDIT_REDO, _T("&Redo") },
	{ ID_SEPARATOR, NULL },
	{ ID_EDIT_CUT, _T("Cu&t") },
	{ ID_EDIT_COPY, _T("&Copy") },
	{ ID_EDIT_PASTE, _T("&Paste") },
	{ ID_EDIT_CLEAR, _T("Cl&ear") },
	{ ID_SEPARATOR, NULL },
	{ ID_EDIT_SELECT_ALL, _T("Select &All") },
};
	for( int i = 0; i < sizeof(arrCmds)/sizeof(arrCmds[0]); i++ )
	{
		if( arrCmds[i].m_nCmdID == ID_SEPARATOR )
		{
			VERIFY( pPopup->ItemInsert( ID_SEPARATOR ) );
			continue;
		} // if( arrCmds[i].m_nCmdID == ID_SEPARATOR )
		CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr( strProfileName, arrCmds[i].m_nCmdID );
		if( pCmdItem == NULL )
			pCmdItem = g_CmdManager->CmdAllocPtr( strProfileName, arrCmds[i].m_nCmdID );
		ASSERT( pCmdItem != NULL );
		if( pCmdItem == NULL )
			continue;
		if( pCmdItem->m_sMenuText.IsEmpty() )
		{
			CExtSafeString sText;
			if( g_ResourceManager->LoadString( sText, arrCmds[i].m_nCmdID ) )
			{
				sText.Replace( _T("\t"), _T(" ") );
				sText.Replace( _T("\r"), _T("") );
				sText.TrimLeft();
				sText.TrimRight();
				if( ! sText.IsEmpty() )
				{
					INT nSep = INT( sText.ReverseFind('\n') );
					if( nSep < 0 )
					{
						pCmdItem->m_sMenuText = sText;
					} // if( nSep < 0 )
					else
					{
						int nLen = sText.GetLength();
						pCmdItem->m_sMenuText = sText.Right( nLen - nSep );
						pCmdItem->m_sMenuText.TrimLeft();
						pCmdItem->m_sMenuText.TrimRight();
					} // else from if( nSep < 0 )
				} // if( ! sText.IsEmpty() )
			}
			if( pCmdItem->m_sMenuText.IsEmpty() )
				pCmdItem->m_sMenuText = arrCmds[i].m_sMenuText;
		} // if( pCmdItem->m_sMenuText.IsEmpty() )
		pCmdItem->StateSetBasic();
		VERIFY( pPopup->ItemInsert( arrCmds[i].m_nCmdID ) );
	} // for( int i = 0; i < sizeof(arrCmds)/sizeof(arrCmds[0]); i++ )
CRect rcClient;
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	if( !rcClient.PtInRect( pos ) )
		pos = rcClient.CenterPoint();
	SetFocus();
	
HWND hWndOwn = m_hWnd;
	if(	! pPopup->TrackPopupMenu( TPMX_OWNERDRAW_FIXED|TPMX_DO_MESSAGE_LOOP, pos.x, pos.y, NULL, this, NULL, NULL, true )  )
	{
		//ASSERT( FALSE );
		//delete pPopup;
		if( ! ::IsWindow( hWndOwn ) )
			return;
		CEdit::OnContextMenu( pWnd, pos );
		return;
	}
	else
	{
		if( ! ::IsWindow( hWndOwn ) )
			return;
		VERIFY( RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME|RDW_ALLCHILDREN ) );
	}
}

__EXT_MFC_INT_PTR CExtEditBase::OnToolHitTest( CPoint point, TOOLINFO * pTI ) const
{
	__PROF_UIS_MANAGE_STATE;
	__EXT_MFC_IMPLEMENT_TT_REDIR_OnToolHitTest( CExtEditBase, CExtToolControlBar );
	return CEdit::OnToolHitTest( point, pTI );
}

COLORREF CExtEditBase::OnQueryBackColor() const
{ 
	ASSERT_VALID( this );
	return m_clrBack; 
}

COLORREF CExtEditBase::OnQueryTextColor() const
{ 
	ASSERT_VALID( this );
	return m_clrText; 
}

void CExtEditBase::InitToolTip()
{
	//if( OnAdvancedPopupMenuTipWndGet() != NULL )
	//	return;
	if( m_wndToolTip.m_hWnd == NULL )
	{
		m_wndToolTip.Create(this);
		m_wndToolTip.Activate(FALSE);
	}
}

int CExtEditBase::OnQueryMaxTipWidth( __EXT_MFC_SAFE_LPCTSTR lpszText )
{
	lpszText;
	return 250;
}

void CExtEditBase::ActivateTooltip(BOOL bActivate)
{
	m_bToolTipActive = bActivate ? true : false;
	if( OnAdvancedPopupMenuTipWndGet() != NULL )
		return;
	if( m_wndToolTip.GetToolCount() == 0 )
		return;
	m_wndToolTip.Activate(bActivate);
}

void CExtEditBase::SetTooltipText(
	CExtSafeString * spText,
	BOOL bActivate // = TRUE
	)
{
	SetTooltipText(
		__EXT_MFC_SAFE_LPCTSTR( ( spText != NULL && (! spText->IsEmpty() ) ) ? LPCTSTR(*spText) : LPCTSTR(_T("")) ),
		bActivate
		);
}

void CExtEditBase::SetTooltipText(
	CExtSafeString & sText,
	BOOL bActivate // = TRUE
	)
{
	SetTooltipText(
		__EXT_MFC_SAFE_LPCTSTR( ( ! sText.IsEmpty() ) ? LPCTSTR(sText) : LPCTSTR(_T("")) ),
		bActivate
		);
}

void CExtEditBase::SetTooltipText(
	__EXT_MFC_SAFE_LPCTSTR sText,
	BOOL bActivate // = TRUE
	)
{
LPCTSTR _sText = LPCTSTR(sText);
	if( _sText == NULL )
		_sText = _T("");
CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
	m_strToolTipText = _sText;
	if( pATTW != NULL )
	{
		//pATTW->SetText( _sText );
		m_bToolTipActive = bActivate ? true : false;
	} // if( pATTW != NULL )
	else
	{
		InitToolTip();
		if( m_wndToolTip.GetToolCount() == 0 )
		{
			CRect rectEdit; 
			GetClientRect( rectEdit );
			m_wndToolTip.AddTool( this, _sText, rectEdit, 1 );
		}
		CWnd::CancelToolTips();
		m_wndToolTip.UpdateTipText(sText,this,1);
		m_wndToolTip.SetMaxTipWidth( -1 );
		CExtSafeString strText( _sText );
		if( strText.Find( _T("\r") ) >= 0 )
			m_wndToolTip.SetMaxTipWidth( OnQueryMaxTipWidth( _sText ) );
		m_wndToolTip.Activate( bActivate );
	}
}

void CExtEditBase::SetTooltipText(
	int nId,
	BOOL bActivate // = TRUE
	)
{
CExtSafeString sText;
	g_ResourceManager->LoadString( sText, nId );
	if( ! sText.IsEmpty() )
		SetTooltipText( &sText, bActivate );
}

void CExtEditBase::SetBkColor( COLORREF clrBk )
{ 
	ASSERT_VALID( this );
	m_clrBack = clrBk; 
	if( GetSafeHwnd() != NULL )
		RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME |RDW_FRAME|RDW_ALLCHILDREN );
}

COLORREF CExtEditBase::GetBkColor() const
{ 
	ASSERT_VALID( this );
	return OnQueryBackColor(); 
}

void CExtEditBase::SetTextColor( COLORREF clrText )
{ 
	ASSERT_VALID( this );
	m_clrText = clrText; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

COLORREF CExtEditBase::GetTextColor() const
{ 
	ASSERT_VALID( this );
	return OnQueryTextColor(); 
}

LRESULT CExtEditBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		{ // block - clipboard commands
			bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
			bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
			bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
			if( ! bAlt )
			{
				if( INT(wParam) == VK_INSERT && (!bCtrl) && (!bShift) )
				{
					return Default();
					//return CEdit::WindowProc( message, wParam, lParam );
				}
				if(		bCtrl && (!bShift)
					&&	(	INT(wParam) == VK_INSERT 
						||	INT(wParam) == INT( _T('C') )
						)
					)
				{
					if( message == WM_KEYDOWN )
						SendMessage( WM_COPY, 0, 0 );
					return 0L;
				}
				
				if(		( bCtrl && (!bShift) && int(wParam) == int( _T('V') ) )
					||	( (!bCtrl) && bShift && int(wParam) == VK_INSERT )
					)
				{
					if( message == WM_KEYDOWN && ( GetStyle() & ES_READONLY ) == 0 )
					{
						//SendMessage( WM_PASTE, 0, 0 );
						Default();
					}
					return 0L;
				}
				if(		( bCtrl && (!bShift) && int(wParam) == int( _T('X') ) )
					||	( (!bCtrl) && bShift && int(wParam) == VK_DELETE )
					)
				{
					if( message == WM_KEYDOWN )
						SendMessage( ( ( GetStyle() & ES_READONLY ) == 0 ) ? WM_CUT : WM_COPY, 0, 0 );
					return 0L;
				}
			}
		} // block - clipboard commands
//		if( ( GetStyle() & ES_READONLY ) != 0 )
//		{
//			bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
//			if( ! bAlt )
//			{
//				bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
//				bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
//				if(		(	bCtrl
//						&&	(	wParam == WPARAM('X')
//							||	wParam == WPARAM('x')
//							||	wParam == WPARAM('V')
//							||	wParam == WPARAM('v')
//							||	wParam == WPARAM('Z')
//							||	wParam == WPARAM('z')
//							)
//						)
//					||	(	bShift
//						&&	(	wParam == VK_INSERT
//							||	wParam == VK_DELETE
//							)
//						)
//					||	(	( ! ( bCtrl || bShift ) )
//						&&	( wParam == VK_DELETE || wParam == VK_BACK )
//						)
//					)
//					return 0L;
//			}
//			else
//			{
//				if( wParam == VK_BACK )
//					return 0L;
//			}
//		}
		if( m_bHandleAltKeyCodes )
		{
			bool bKeyDown = ( message == WM_KEYDOWN || message == WM_SYSKEYDOWN ) ? true : false;
			switch( wParam )
			{
			case VK_MENU:
				if( bKeyDown )
				{
					if( ( GetStyle() & ES_READONLY ) != 0 )
					{
						m_bAltKeyCodeMode = false;
						if( g_hWndEditInAltKeyCodeMode == m_hWnd )
							g_hWndEditInAltKeyCodeMode = NULL;
						m_nTrackedAltKeyCode = 0L;
						break;
					}
					m_bAltKeyCodeMode = true;
					g_hWndEditInAltKeyCodeMode = m_hWnd;
					m_nTrackedAltKeyCode = 0L;
					return 0L;
				} // if( bKeyDown )
				else if( m_bAltKeyCodeMode )
				{
					m_bAltKeyCodeMode = false;
					if( g_hWndEditInAltKeyCodeMode == m_hWnd )
						g_hWndEditInAltKeyCodeMode = NULL;
					if( m_nTrackedAltKeyCode > 0L )
					{
						USES_CONVERSION;
						WCHAR wstr[2] = { WCHAR(m_nTrackedAltKeyCode), L'\0' };
						LPCTSTR str = W2CT( wstr );
						//ReplaceSel( str );
						m_nTrackedAltKeyCode = 0L;
						for( MSG _msg; ::PeekMessage( &_msg, m_hWnd, WM_CHAR, WM_CHAR, PM_REMOVE ); );
						SendMessage( WM_CHAR, WPARAM(str[0]), 1L );
					}
					return 0L;
				} // else if( m_bAltKeyCodeMode )
			break;
			case VK_INSERT:
			case VK_END:
			case VK_DOWN:
			case VK_NEXT:
			case VK_LEFT:
			case 0x0C: // VK_CLEAR
			case VK_RIGHT:
			case VK_HOME:
			case VK_UP:
			case VK_PRIOR:
				if( m_bAltKeyCodeMode )
				{
					if( ! CExtPopupMenuWnd::IsKeyPressed( VK_MENU ) )
					{
						m_bAltKeyCodeMode = false;
						if( g_hWndEditInAltKeyCodeMode == m_hWnd )
							g_hWndEditInAltKeyCodeMode = NULL;
						m_nTrackedAltKeyCode = 0L;
						break;
					}
					if( bKeyDown )
					{
						LONG nAdd = -1L;
						bool bIsExtendedKey = ( ( lParam & (1L<<24L) ) == 0 ) ? true : false;
						if( bIsExtendedKey )
						{
							switch( wParam )
							{
							case VK_INSERT:		nAdd = 0L; break;
							case VK_END:		nAdd = 1L; break;
							case VK_DOWN:		nAdd = 2L; break;
							case VK_NEXT:		nAdd = 3L; break;
							case VK_LEFT:		nAdd = 4L; break;
							case 0x0C:			nAdd = 5L; break; // VK_CLEAR
							case VK_RIGHT:		nAdd = 6L; break;
							case VK_HOME:		nAdd = 7L; break;
							case VK_UP:			nAdd = 8L; break;
							case VK_PRIOR:		nAdd = 9L; break;
							} // switch( wParam )
						} // if( bIsExtendedKey )
						if( nAdd >= 0L )
						{
							m_nTrackedAltKeyCode *= 10;
							ASSERT( 0L <= nAdd && nAdd <= 9L );
							m_nTrackedAltKeyCode += nAdd;
						}
					} // if( message == WM_KEYDOWN )
					return 0L;
				} // if( m_bAltKeyCodeMode )
			break;
			case VK_NUMPAD0:
			case VK_NUMPAD1:
			case VK_NUMPAD2:
			case VK_NUMPAD3:
			case VK_NUMPAD4:
			case VK_NUMPAD5:
			case VK_NUMPAD6:
			case VK_NUMPAD7:
			case VK_NUMPAD8:
			case VK_NUMPAD9:
				if( m_bAltKeyCodeMode )
				{
					if( ! CExtPopupMenuWnd::IsKeyPressed( VK_MENU ) )
					{
						m_bAltKeyCodeMode = false;
						if( g_hWndEditInAltKeyCodeMode == m_hWnd )
							g_hWndEditInAltKeyCodeMode = NULL;
						m_nTrackedAltKeyCode = 0L;
						break;
					}
					if( bKeyDown )
					{
						bool bIsExtendedKey = ( ( lParam & (1L<<24L) ) == 0 ) ? true : false;
						if( bIsExtendedKey )
						{
							m_nTrackedAltKeyCode *= 10;
							LONG nAdd = LONG(wParam) - LONG(VK_NUMPAD0);
							ASSERT( 0L <= nAdd && nAdd <= 9L );
							m_nTrackedAltKeyCode += nAdd;
						} // if( bIsExtendedKey )
					} // if( message == WM_KEYDOWN )
					return 0L;
				} // if( m_bAltKeyCodeMode )
			break;
			case WPARAM('0'):
			case WPARAM('1'):
			case WPARAM('2'):
			case WPARAM('3'):
			case WPARAM('4'):
			case WPARAM('5'):
			case WPARAM('6'):
			case WPARAM('7'):
			case WPARAM('8'):
			case WPARAM('9'):
				if( m_bAltKeyCodeMode )
				{
					if( ! CExtPopupMenuWnd::IsKeyPressed( VK_MENU ) )
					{
						m_bAltKeyCodeMode = false;
						if( g_hWndEditInAltKeyCodeMode == m_hWnd )
							g_hWndEditInAltKeyCodeMode = NULL;
						m_nTrackedAltKeyCode = 0L;
						break;
					}
					if( bKeyDown )
					{
						bool bIsExtendedKey = ( ( lParam & (1L<<24L) ) == 0 ) ? true : false;
						if( bIsExtendedKey )
						{
							m_nTrackedAltKeyCode *= 10;
							LONG nAdd = LONG(wParam) - LONG('0');
							ASSERT( 0L <= nAdd && nAdd <= 9L );
							m_nTrackedAltKeyCode += nAdd;
						} // if( bIsExtendedKey )
					} // if( message == WM_KEYDOWN )
					return 0L;
				} // if( m_bAltKeyCodeMode )
			break;
			default:
				m_bAltKeyCodeMode = false;
				if( g_hWndEditInAltKeyCodeMode == m_hWnd )
					g_hWndEditInAltKeyCodeMode = NULL;
				m_nTrackedAltKeyCode = 0L;
			break;
			} // switch( wParam )
		} // if( m_bHandleAltKeyCodes )
		else
		{
			m_bAltKeyCodeMode = false;
			if( g_hWndEditInAltKeyCodeMode == m_hWnd )
				g_hWndEditInAltKeyCodeMode = NULL;
			m_nTrackedAltKeyCode = 0L;
		} // else from if( m_bHandleAltKeyCodes )
	break;
	case WM_CANCELMODE:
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
	case WM_KILLFOCUS:
	case WM_DESTROY:
	case WM_SETTINGCHANGE:
	case WM_DISPLAYCHANGE:
	case WM_SHOWWINDOW:
	case WM_SIZE:
	case WM_WINDOWPOSCHANGING:
	case WM_WINDOWPOSCHANGED:
	case EM_SETSEL:
	case EM_REPLACESEL:
	case EM_UNDO:
	case EM_REDO:
		m_bAltKeyCodeMode = false;
		if( g_hWndEditInAltKeyCodeMode == m_hWnd )
			g_hWndEditInAltKeyCodeMode = NULL;
		m_nTrackedAltKeyCode = 0L;
	break;
	case WM_STYLECHANGED:
		if( ( GetStyle() & ES_READONLY ) != 0 )
		{
			m_bAltKeyCodeMode = false;
			if( g_hWndEditInAltKeyCodeMode == m_hWnd )
				g_hWndEditInAltKeyCodeMode = NULL;
			m_nTrackedAltKeyCode = 0L;
			break;
		}
	break;
	case WM_SYSCHAR:
	case WM_CHAR:
		if(		( GetStyle() & ES_READONLY ) != 0
			&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_CONTROL ) )
			)
			return 0L;
	break;
	case WM_PASTE:
		{
			m_bAltKeyCodeMode = false;
			if( g_hWndEditInAltKeyCodeMode == m_hWnd )
				g_hWndEditInAltKeyCodeMode = NULL;
			m_nTrackedAltKeyCode = 0L;
		}
	break;
	} // switch( message )
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
LRESULT lResult = CEdit::WindowProc( message, wParam, lParam );
	return lResult;
}

BOOL CExtEditBase::PreTranslateMessage( MSG * pMsg )
{
	if(		(! CExtPopupMenuWnd::IsMenuTracking() )
		&&	OnAdvancedPopupMenuTipWndGet() == NULL
		)
	{
		InitToolTip();
		m_wndToolTip.RelayEvent( pMsg );
	}
	return CEdit::PreTranslateMessage( pMsg );
}

BOOL CExtEditBase::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if(		m_bHandleCtxMenus
		&&	(	nCode == CN_UPDATE_COMMAND_UI
			||	nCode == CN_COMMAND
			)
		)
	{
		bool bReadOnly = ( ( GetStyle() & ES_READONLY ) != 0 ) ? true : false;
		DWORD nSelection = GetSel();
		if( nID == ID_EDIT_UNDO )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable( ( CanUndo() && (! bReadOnly ) ) ? TRUE : FALSE );
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				Undo();
			return TRUE;
		} // if( nID == ID_EDIT_UNDO )

//		if( nID == ID_EDIT_REDO )
//		{
//			if( nCode == CN_UPDATE_COMMAND_UI )
//			{
//				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
//				ASSERT( pCmdUI != NULL );
//				pCmdUI->Enable(
//					(BOOL) ( SendMessage( EM_CANREDO ) && (! bReadOnly )
//					);
//			} // if( nCode == CN_UPDATE_COMMAND_UI )
//			else
//				SendMessage( EM_REDO );
//			return TRUE;
//		} // if( nID == ID_EDIT_REDO )
		if( nID == ID_EDIT_CUT )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable( (LOWORD(nSelection)) != (HIWORD(nSelection)) && (! bReadOnly ) );
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SendMessage( WM_CUT );
			return TRUE;
		} // if( nID == ID_EDIT_CUT )
		if( nID == ID_EDIT_COPY )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable( (LOWORD(nSelection)) != (HIWORD(nSelection)) );
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SendMessage( WM_COPY );
			return TRUE;
		} // if( nID == ID_EDIT_COPY )
		if( nID == ID_EDIT_PASTE )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable(
						(	::IsClipboardFormatAvailable( CF_TEXT )
						||	( g_PaintManager.m_bIsWinNT4orLater && ::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
						)
					&&	(! bReadOnly )
					);
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SendMessage( WM_PASTE );
			return TRUE;
		} // if( nID == ID_EDIT_PASTE )
		if( nID == ID_EDIT_CLEAR )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable( (LOWORD(nSelection)) != (HIWORD(nSelection)) && (! bReadOnly ) );
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SendMessage( WM_CLEAR );
			return TRUE;
		} // if( nID == ID_EDIT_CLEAR )
		if( nID == ID_EDIT_SELECT_ALL )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				
				int nLength = GetWindowTextLength();
				pCmdUI->Enable(
						nLength > 0
					&&	(!(		(LOWORD(nSelection)) == 0 
							&&	(HIWORD(nSelection)) == nLength
						))
					);
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SetSel( 0, -1 );
			return TRUE;
		} // if( nID == ID_EDIT_SELECT_ALL )
	} // if( m_bHandleCtxMenus ...
	return CEdit::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

HBRUSH CExtEditBase::CtlColor( CDC* pDC, UINT nCtlColor )
{
	ASSERT_VALID( this );
	if(		nCtlColor == CTLCOLOR_EDIT 
		||	nCtlColor == CTLCOLOR_MSGBOX
		||	nCtlColor == CTLCOLOR_STATIC
		)
	{
		CExtPaintManager * pPM = PmBridge_GetPM();
		ASSERT_VALID( pPM );
		bool bReadOnly = ( GetStyle() & ES_READONLY ) != 0;
		bool bDisabled = OnQueryWindowEnabledState() ? false : true;
		COLORREF clrText = GetTextColor();
		if( clrText == COLORREF(-1L) )
			clrText = pPM->GetColor( bDisabled ? CExtPaintManager::CLR_TEXT_DISABLED : COLOR_WINDOWTEXT, this );
		COLORREF clrBk = GetBkColor();
		if( clrBk == COLORREF(-1L) )
			clrBk = pPM->GetColor( ( bReadOnly || bDisabled ) ? CExtPaintManager::CLR_3DFACE_OUT : COLOR_WINDOW, this );
		pDC->SetBkColor( clrBk );
		pDC->SetTextColor( clrText );
		if( m_clrBackPrev != clrBk )
		{
			if( m_brBack.GetSafeHandle() != NULL )
				m_brBack.DeleteObject();
			m_brBack.CreateSolidBrush( clrBk );
			m_clrBackPrev = clrBk;
		}
		return m_brBack;
	}
	else
		return (HBRUSH)Default();
}

void CExtEditBase::stat_DoPaintNc(
	bool bEnabled,
	bool bMouseOver,
	CWnd * pWnd,
	CExtPmBridge * pPmBridge,
	CDC * pDC
	)
{
	ASSERT_VALID( pWnd );
	ASSERT( pWnd->GetSafeHwnd() != NULL );
	ASSERT( pPmBridge != NULL );
	ASSERT_VALID( pDC );
	ASSERT( pDC->GetSafeHdc() != NULL );
CRect rcInBarWnd, rcInBarClient;
	pWnd->GetWindowRect( &rcInBarWnd );
	pWnd->GetClientRect( &rcInBarClient );
	pWnd->ClientToScreen( &rcInBarClient );
	if( rcInBarWnd == rcInBarClient )
		return;
CPoint ptDevOffset = -rcInBarWnd.TopLeft();
	rcInBarWnd.OffsetRect( ptDevOffset );
	rcInBarClient.OffsetRect( ptDevOffset );
const INT cx = ::GetSystemMetrics( SM_CXVSCROLL );
const INT cy = ::GetSystemMetrics( SM_CYHSCROLL );
DWORD dwStyle = pWnd->GetStyle();
bool bHasVerticalSB = ( (dwStyle&WS_VSCROLL) != 0 ) ? true : false;
bool bHasHorizontalSB = ( (dwStyle&WS_HSCROLL) != 0 ) ? true : false;
	if( bHasVerticalSB && bHasHorizontalSB )
	{
		INT nNcX = rcInBarWnd.right - rcInBarClient.right - cx;
		INT nNcY = rcInBarWnd.bottom - rcInBarClient.bottom - cy;
		pDC->FillSolidRect(
			rcInBarWnd.right - cx - nNcX, rcInBarWnd.bottom - cy - nNcY, cx, cy,
			pPmBridge->PmBridge_GetPM()->GetColor( COLOR_WINDOW, pWnd )
			);
	}
CRect rcExclude( rcInBarClient );
	if( bHasVerticalSB )
		rcExclude.right += cx;
	if( bHasHorizontalSB )
		rcExclude.bottom += cy;

#ifndef __EXT_MFC_NO_SPINWND
CWnd * pWndParent = pWnd->GetParent();
	ASSERT_VALID( pWndParent );
bool bBuddyWithSpin = false;
bool bSpinAlignRight = false;
bool bSpinAlignLeft = false;
CWnd * pWndTest = pWnd->GetWindow( GW_HWNDNEXT );
	if( pWndTest->GetSafeHwnd() != NULL )
	{
		ASSERT_VALID( pWndTest );
		static const TCHAR szSpin[] = UPDOWN_CLASS;
		TCHAR szCompare[ sizeof(szSpin)/sizeof(szSpin[0]) + 1 ] = _T("");
		::GetClassName( pWndTest->GetSafeHwnd(), szCompare, sizeof(szCompare)/sizeof(szCompare[0]) );
		if( ! lstrcmpi( szCompare, szSpin ) )
		{
			DWORD dwStyle = pWndTest->GetStyle();
			bSpinAlignRight = ( ( dwStyle & UDS_ALIGNRIGHT ) != 0 ) ? true : false;
			bSpinAlignLeft  = ( ( dwStyle & UDS_ALIGNLEFT ) != 0 ) ? true : false;
			if( bSpinAlignRight || bSpinAlignLeft )
				bBuddyWithSpin  = ( ( dwStyle & UDS_AUTOBUDDY ) != 0 || ((HWND)pWndTest->SendMessage(UDM_GETBUDDY)) == pWnd->m_hWnd ) ? true : false;
		}
	}
	if( bBuddyWithSpin )
	{
		CRect rcSpin;
		pWndTest->GetClientRect( &rcSpin );
		if( bSpinAlignRight )
		{
			rcInBarWnd.right += rcSpin.Width();
			rcExclude.right += rcSpin.Width();
		}
		else if( bSpinAlignLeft )
		{
			rcInBarWnd.left -= rcSpin.Width();
			rcExclude.left -= rcSpin.Width();
		}
	}
#endif // __EXT_MFC_NO_SPINWND

	pDC->ExcludeClipRect( &rcExclude );
	if( bHasVerticalSB || bHasHorizontalSB )
	{
		struct default_friendly_t : public CWnd
		{
			friend void CExtEditBase::stat_DoPaintNc( bool bEnabled, bool bMouseOver, CWnd * pWnd, CExtPmBridge * pPmBridge, CDC * pDC );
		};
		((default_friendly_t*)pWnd)->Default();
	}
bool bReadOnly = ( ( pWnd->GetStyle() & ES_READONLY ) != 0 ) ? true : false;
COLORREF clrSysBk = pPmBridge->PmBridge_GetPM()->GetColor( ( bReadOnly || (!bEnabled) ) ? CExtPaintManager::CLR_3DFACE_OUT : COLOR_WINDOW, pWnd );
	CExtEditBase::stat_DrawEditImpl( bEnabled, bMouseOver, pWnd, pPmBridge, rcInBarWnd, pDC );
	pDC->SelectClipRgn( NULL );

#ifndef __EXT_MFC_NO_SPINWND
	if( bBuddyWithSpin )
	{
		CRect rcSpinWnd, rcSpinClient;
		pWndTest->GetWindowRect( &rcSpinWnd );
		pWndTest->GetClientRect( &rcSpinClient );
		pWndTest->ClientToScreen( &rcSpinClient );
		CPoint ptDevOffset = -rcSpinWnd.TopLeft();
		rcSpinWnd.OffsetRect( ptDevOffset );
		rcSpinClient.OffsetRect( ptDevOffset );
		__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( pWndTest->GetSafeHwnd(), GWL_EXSTYLE );
		bool bRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;
		if( bRTL )
			rcSpinClient.OffsetRect( -3, 0 );
		CWindowDC dcSpin( pWndTest );
		dcSpin.ExcludeClipRect( &rcSpinClient );
		dcSpin.FillSolidRect( &rcSpinWnd, clrSysBk );
		if( bSpinAlignRight )
			rcSpinWnd.left -= rcInBarClient.Width();
		else if( bSpinAlignLeft )
			rcSpinWnd.right += rcInBarClient.Width();
		CExtEditBase::stat_DrawEditImpl( bEnabled, bMouseOver, pWnd, pPmBridge, rcSpinWnd, &dcSpin );
		dcSpin.SelectClipRgn( NULL );
	}
#endif // __EXT_MFC_NO_SPINWND
}

void CExtEditBase::stat_DrawEditImpl(
	bool bEnabled,
	bool bMouseOver,
	CWnd * pWnd,
	CExtPmBridge * pPmBridge,
	CRect rectClient,
	CDC * pDC
	)
{
	ASSERT_VALID( pWnd );
	ASSERT( pWnd->GetSafeHwnd() != NULL );
	ASSERT( pPmBridge != NULL );
	ASSERT_VALID( pDC );
	ASSERT( pDC->GetSafeHdc() != NULL );
bool bNeedReleaseDC = false;
	if( pDC == NULL )
	{
		pDC = pWnd->GetDC();
		bNeedReleaseDC = true;
	}
	ASSERT_VALID( pDC );
bool bFocus = false;
	if(	::GetFocus() == pWnd->m_hWnd && (! CExtPopupMenuWnd::IsMenuTracking() ) )
		bFocus = true;
bool bReadOnly = ( ( pWnd->GetStyle() & ES_READONLY ) != 0 ) ? true : false;
CExtPaintManager::PAINTCONTROLFRAMEDATA _pcfd( pWnd, rectClient, true, bMouseOver, bEnabled, bFocus, bReadOnly );
	pPmBridge->PmBridge_GetPM()->PaintControlFrame( *pDC, _pcfd );
	if( bNeedReleaseDC )
		pWnd->ReleaseDC(pDC);
}

void CExtEditBase::stat_PrintClient(
	bool bEnabled,
	bool bMouseOver,
	UINT message,
	LPARAM lParam,
	CWnd * pWnd,
	CExtPmBridge * pPmBridge,
	CDC * pDC
	)
{
	ASSERT_VALID( pWnd );
	ASSERT( pWnd->GetSafeHwnd() != NULL );
	ASSERT( pPmBridge != NULL );
	ASSERT_VALID( pDC );
	ASSERT( pDC->GetSafeHdc() != NULL );
CRect rcWnd;
	pWnd->GetWindowRect( &rcWnd );
	rcWnd.OffsetRect( -rcWnd.TopLeft() );
CExtMemoryDC dc( pDC, &rcWnd, CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FILL_BITS|CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY );
	if( (lParam&PRF_NONCLIENT) != 0 )
		CExtEditBase::stat_DoPaintNc( bEnabled, bMouseOver, pWnd, pPmBridge, pDC );
	if( (lParam&PRF_CHILDREN) != 0 )
		CExtPaintManager::stat_PrintChildren( pWnd->m_hWnd, message, dc.GetSafeHdc(), lParam, false );
}

/////////////////////////////////////////////////////////////////////////////
// CExtEdit

IMPLEMENT_DYNCREATE( CExtEdit, CExtEditBase );

CExtEdit::CExtEdit()
{
}

CExtEdit::~CExtEdit()
{
}

BEGIN_MESSAGE_MAP(CExtEdit, CExtED < CExtEditBase >)
	//{{AFX_MSG_MAP(CExtEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtIPAddressCtrl

CTypedPtrList < CPtrList, CExtIPAddressCtrl * > CExtIPAddressCtrl::g_listTargets;

IMPLEMENT_DYNCREATE( CExtIPAddressCtrl, CIPAddressCtrl );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtIPAddressCtrl );

CExtIPAddressCtrl::CExtIPAddressCtrl()
	: m_bDirectCreateCall( false )
	, m_bInitialized( false )
	, m_bMouseOver( false )
{
	for( INT nIndex = 0; nIndex < 4; nIndex++ )	
	{
		m_pWNDPROC[nIndex] = NULL;
		m_hWndHooked[nIndex] = NULL;
	}
	PmBridge_Install();
}

CExtIPAddressCtrl::~CExtIPAddressCtrl()
{
	PmBridge_Uninstall();
}

BEGIN_MESSAGE_MAP(CExtIPAddressCtrl, CIPAddressCtrl)
	//{{AFX_MSG_MAP(CExtIPAddressCtrl)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_NCPAINT()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtIPAddressCtrl message handlers

BOOL CExtIPAddressCtrl::OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message )
{
	if( message == WM_MOUSEMOVE )
	{
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		if( rcWnd.PtInRect( GetCurrentMessage()->pt ) )
		{
			SetTimer( __EXT_EDIT_UPDATE_TIMER, __EXT_EDIT_UPDATE_TIMER_PERIOD, NULL );
			SendMessage( WM_TIMER, __EXT_EDIT_UPDATE_TIMER, 0L );
		}
	}
	return CIPAddressCtrl::OnSetCursor(pWnd, nHitTest, message);
}

void CExtIPAddressCtrl::OnTimer( __EXT_MFC_UINT_PTR nIDEvent ) 
{
	if( nIDEvent != __EXT_EDIT_UPDATE_TIMER )
	{
		CIPAddressCtrl::OnTimer( nIDEvent );
		return;
	}
POINT pt;
	if( ! ::GetCursorPos(&pt) )
		return;
	if( IsWindowVisible() )
	{
		bool bOldMouseOver = m_bMouseOver;
		CRect rectItem;
		GetWindowRect( &rectItem );
		if( !rectItem.PtInRect(pt) )
		{
			KillTimer( __EXT_EDIT_UPDATE_TIMER );
			m_bMouseOver = false;
		}
		else 
		{
			HWND hWndFromPoint = ::WindowFromPoint( pt );
			if(	hWndFromPoint == m_hWnd )
				m_bMouseOver = true;
			else
			{
				for( HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD ); hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
				{
					if( hWnd != NULL && ::IsWindow( hWnd ) )
					{
						if( hWnd == hWndFromPoint )
						{
							m_bMouseOver = true;
							break;
						}
					}
				}
			}
		}
		if( bOldMouseOver != m_bMouseOver )
			_PostRedraw();
	} // if( IsWindowVisible() )
}

void CExtIPAddressCtrl::_PostRedraw()
{
	VERIFY(
		RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE | RDW_UPDATENOW
				| RDW_ERASE | RDW_ERASENOW
				| RDW_FRAME | RDW_ALLCHILDREN
			)
		);
}

void CExtIPAddressCtrl::_DrawEditImpl(
	CRect rectClient,
	CDC * pDC // = NULL
	)
{
bool bNeedReleaseDC = false;
	if( pDC == NULL )
	{
		pDC = GetDC();
		bNeedReleaseDC = true;
	}
	ASSERT_VALID( pDC );
bool bFocus = false;
	if( ! CExtPopupMenuWnd::IsMenuTracking() )
	{
		HWND hWndFocused = GetFocus()->GetSafeHwnd();
		if( hWndFocused == m_hWnd )
			bFocus = true;
		else
		{
			for( HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD ); hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
			{
				if( hWnd != NULL && ::IsWindow( hWnd ) )
				{
					if( hWnd == hWndFocused )
					{
						bFocus = true;
						break;
					}
				}
			}
		}
	}
	rectClient.InflateRect( 3, 3 );
bool bEnabled = IsWindowEnabled() ? true : false;
bool bReadOnly = ( ( GetStyle() & ES_READONLY ) != 0 ) ? true : false;
CExtPaintManager::PAINTCONTROLFRAMEDATA _pcfd( this, rectClient, true, m_bMouseOver, bEnabled, bFocus, bReadOnly );
	PmBridge_GetPM()->PaintControlFrame( *pDC, _pcfd );
	if( bNeedReleaseDC )
		ReleaseDC(pDC);
}

void CExtIPAddressCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CIPAddressCtrl::OnSetFocus(pOldWnd);
	_PostRedraw();
}

void CExtIPAddressCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CIPAddressCtrl::OnKillFocus(pNewWnd);
	_PostRedraw();
}

void CExtIPAddressCtrl::OnNcPaint() 
{
	ASSERT_VALID( this );
CWindowDC dc( this );
	_DoPaintNC( &dc );
}

void CExtIPAddressCtrl::_DoPaintNC( CDC * pDC )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );
	ASSERT( pDC->GetSafeHdc() != NULL );
	if( ! m_bInitialized )
		return;
CRect rcInBarWnd, rcInBarClient;
    GetWindowRect( &rcInBarWnd );
    GetClientRect( &rcInBarClient );
    ClientToScreen( &rcInBarClient );
    if( rcInBarWnd == rcInBarClient )
        return;
CPoint ptDevOffset = -rcInBarWnd.TopLeft();
    rcInBarWnd.OffsetRect( ptDevOffset );
    rcInBarClient.OffsetRect( ptDevOffset );
const INT cx = ::GetSystemMetrics( SM_CXVSCROLL );
const INT cy = ::GetSystemMetrics( SM_CYHSCROLL );
DWORD dwStyle = GetStyle();
bool bHasVerticalSB = ( ( dwStyle & WS_VSCROLL ) != 0 ) ? true : false;
bool bHasHorizontalSB = ( ( dwStyle & WS_HSCROLL ) != 0 ) ? true : false;
    if( bHasVerticalSB && bHasHorizontalSB )
    {
		INT nNcX = rcInBarWnd.right - rcInBarClient.right - cx;
		INT nNcY = rcInBarWnd.bottom - rcInBarClient.bottom - cy;
        pDC->FillSolidRect(
            rcInBarWnd.right - cx - nNcX, rcInBarWnd.bottom - cy - nNcY, cx, cy,
            PmBridge_GetPM()->GetColor( COLOR_WINDOW, this )
            );
    }
CRect rcExclude( rcInBarClient );
    if( bHasVerticalSB )
        rcExclude.right += cx;
    if( bHasHorizontalSB )
        rcExclude.bottom += cy;
	pDC->ExcludeClipRect( &rcExclude );
    if( bHasVerticalSB || bHasHorizontalSB )
		Default();
	rcInBarWnd.DeflateRect( 3, 3 );
	_DrawEditImpl( rcInBarWnd, pDC );
	pDC->SelectClipRgn( NULL );
}

LRESULT CExtIPAddressCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(		message == WM_PRINT 
		||	message == WM_PRINTCLIENT
		)
	{
		CDC * pDC = CDC::FromHandle( (HDC) wParam );
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		rcWnd.OffsetRect( -rcWnd.TopLeft() );
		CExtMemoryDC dc( pDC, &rcWnd );
		if( ( lParam & PRF_NONCLIENT ) != 0 )
			_DoPaintNC( &dc );
		if( ( lParam & PRF_CHILDREN ) != 0 )
			CExtPaintManager::stat_PrintChildren( m_hWnd, message, dc.GetSafeHdc(), lParam, false );
		return (!0);
	}
	return CIPAddressCtrl::WindowProc(message, wParam, lParam);
}

void CExtIPAddressCtrl::OnDestroy() 
{
POSITION pos = g_listTargets.Find( this );
	if( pos == NULL )
		return;
	g_listTargets.RemoveAt( pos );
	for( INT nIndexWnd = 0; nIndexWnd < 4; nIndexWnd++ )
	{
		if(		m_hWndHooked[ nIndexWnd ] != NULL
			&&	::IsWindow(m_hWndHooked[ nIndexWnd ])
			&&	m_pWNDPROC[ nIndexWnd ] != NULL
			)
		{
			::__EXT_MFC_SetWindowLong(
				m_hWndHooked[ nIndexWnd ],
				__EXT_MFC_GWL_WNDPROC,
#if defined(_WIN64)
				(__EXT_MFC_LONG_PTR)
#else
				(LONG)(__EXT_MFC_LONG_PTR)
#endif
					m_pWNDPROC[ nIndexWnd ]
				);
		}
	}
	CIPAddressCtrl::OnDestroy();
}

LRESULT CALLBACK CExtIPAddressCtrl::g_HookWndProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam )
{
	for( POSITION pos = g_listTargets.GetHeadPosition(); pos != NULL; )
	{
		CExtIPAddressCtrl * pWndTarget = g_listTargets.GetNext( pos );
		if( pWndTarget != NULL && pWndTarget->GetSafeHwnd() != NULL )
		{
			if( nMessage == WM_SETCURSOR )
 				::SendMessage( pWndTarget->m_hWnd, nMessage, wParam, lParam );
			else if( nMessage == WM_SETFOCUS || nMessage == WM_KILLFOCUS )
 				pWndTarget->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME|RDW_ALLCHILDREN );
			for( INT nIndexWnd = 0; nIndexWnd < 4; nIndexWnd++ )
			{
				HWND hWndHooked = pWndTarget->m_hWndHooked[ nIndexWnd ];
				WNDPROC pWNDPROC  = pWndTarget->m_pWNDPROC[ nIndexWnd ];
				if( hWndHooked != NULL && ::IsWindow(hWndHooked) && hWndHooked == hWnd )
				{
					ASSERT( pWNDPROC != NULL );
					return ::CallWindowProc( pWNDPROC, hWnd, nMessage, wParam, lParam );
				}
			}
		}
	} // for( POSITION pos = g_listTargets.GetHeadPosition(); pos != NULL; )
	return 0L;
}

void CExtIPAddressCtrl::PreSubclassWindow() 
{
	CIPAddressCtrl::PreSubclassWindow();
	if( m_bDirectCreateCall )
		return;
	if( ! _CreateHelper() )
	{
		ASSERT( FALSE );
		return;
	}
	m_bInitialized = true;
}

BOOL CExtIPAddressCtrl::Create(
	DWORD dwStyle, 
	const RECT & rect,
	CWnd * pParentWnd, 
	UINT nID
	)
{
	if( ! CIPAddressCtrl::Create( dwStyle, rect, pParentWnd, nID ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	return TRUE;
}

BOOL CExtIPAddressCtrl::Create(
	LPCTSTR lpszClassName, 
	LPCTSTR lpszWindowName, 
	DWORD dwStyle, 
	const RECT & rect, 
	CWnd * pParentWnd, 
	UINT nID, CCreateContext * pContext
	) 
{
	ASSERT_VALID( this );
	m_bDirectCreateCall = true;
	if( ! CWnd::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	if( ! _CreateHelper() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bInitialized = true;
	return TRUE;
}

bool CExtIPAddressCtrl::_CreateHelper()
{
	ASSERT_VALID( this );
	if( m_bInitialized )
		return true;
	if( m_hWnd == NULL || ( !::IsWindow( m_hWnd ) ) )
		return false;
	// hook all the child edit controls
POSITION pos = g_listTargets.Find( this );
	if( pos == NULL )
		g_listTargets.AddHead( this );
bool bInit = false;
INT nIndexWnd = 0;
	for( HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD ); ((hWnd != NULL) && (nIndexWnd < 4)); hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ), nIndexWnd++ )
	{
		m_hWndHooked[ nIndexWnd ] = hWnd;
		if( m_hWndHooked[ nIndexWnd ] == NULL || (!::IsWindow(m_hWndHooked[ nIndexWnd ])) )
		{
			ASSERT( FALSE );
			bInit = false;
			break;
		}
		bInit = true;

		m_pWNDPROC[ nIndexWnd ] = (WNDPROC)(__EXT_MFC_LONG_PTR)
			::__EXT_MFC_SetWindowLong(
				m_hWndHooked[ nIndexWnd ],
				__EXT_MFC_GWL_WNDPROC,
#if defined(_WIN64)
				(__EXT_MFC_LONG_PTR)
#else
				(LONG)(__EXT_MFC_LONG_PTR)
#endif
					g_HookWndProc
				);
		ASSERT( m_pWNDPROC[ nIndexWnd ] != NULL );
	}	
	if( ! bInit )
		return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditHotKeyBase window

IMPLEMENT_DYNCREATE( CExtEditHotKeyBase, CExtEditBase );

CExtEditHotKeyBase::CExtEditHotKeyBase()
	: m_bAllowTabKeyInput( false )
	, m_nSimpleKeyModifiers( (BYTE)0 )
{
	::memset( &m_dataAccel, 0, sizeof(ACCEL) );
}

CExtEditHotKeyBase::~CExtEditHotKeyBase()
{
}

BEGIN_MESSAGE_MAP( CExtEditHotKeyBase, CExtEditBase )
    //{{AFX_MSG_MAP(CExtEditHotKeyBase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtSafeString CExtEditHotKeyBase::stat_GetSingleKeyName(
	HKL hKeyboardLayout,
	WORD nKey,
	__EXT_MFC_SAFE_LPCTSTR strAppendIfNonEmpty, // = NULL
	bool bMakeOnlyFirstLettersUpper // = true
	)
{
UINT nScan = ::MapVirtualKeyEx( (UINT)nKey, 0, hKeyboardLayout );
	nScan &= 0x0FFFF;
	switch( nKey )
	{
	case VK_INSERT:
	case VK_DELETE:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_END:
	case VK_HOME:
	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_SELECT:
	case VK_PRINT:
	case VK_EXECUTE:
	case VK_SNAPSHOT:
	case VK_HELP:
		nScan |= 0x0300;
		break;
	} // switch( nKey )
LPARAM lParam = MAKELPARAM( 1, nScan );
const int nMaxKeyName = 256;
CExtSafeString strKeyText;
LPTSTR pBuffer = strKeyText.GetBuffer(nMaxKeyName);
	ASSERT( pBuffer != NULL );
	::memset( pBuffer, 0, nMaxKeyName*sizeof(TCHAR) );
	::GetKeyNameText( (LONG)lParam, pBuffer, nMaxKeyName );
	strKeyText.ReleaseBuffer();
	if( strKeyText.IsEmpty())
		return strKeyText;
	if( ! bMakeOnlyFirstLettersUpper )
	{
		if( strAppendIfNonEmpty != NULL )
			strKeyText += strAppendIfNonEmpty;
		return strKeyText;
	}
	strKeyText.MakeLower();
bool bPrevSpace = true;
	for( int i = 0; i < strKeyText.GetLength(); i++ )
	{
		TCHAR chr = strKeyText[i];
		if( _istspace(chr) )
		{
			bPrevSpace = true;
			continue;
		} // if( _istspace(chr) )
		if( _istalpha(chr) && bPrevSpace )
		{
			chr = (TCHAR)_totupper( chr );
			strKeyText.SetAt( i, chr );
		} // if( _istalpha(chr) && bPrevSpace )
		bPrevSpace = false;
	} // for( int i = 0; i < strKeyText.GetLength(); i++ )
	if( strAppendIfNonEmpty != NULL )
		strKeyText += strAppendIfNonEmpty;
	return strKeyText;
}

CExtSafeString CExtEditHotKeyBase::stat_FormatAccelText( const ACCEL & _dataAccel )
{
HKL hKeyboardLayout = ::GetKeyboardLayout( ( ::AfxGetThread() ) -> m_nThreadID );
static TCHAR strTextBetweenKeys[] = _T("+");
CExtSafeString strAccelText;
	if( _dataAccel.fVirt & FCONTROL )
		strAccelText += CExtEditHotKeyBase::stat_GetSingleKeyName( hKeyboardLayout, VK_CONTROL, strTextBetweenKeys );
	if( _dataAccel.fVirt & FSHIFT )
		strAccelText += CExtEditHotKeyBase::stat_GetSingleKeyName( hKeyboardLayout, VK_SHIFT, strTextBetweenKeys );
	if( _dataAccel.fVirt & FALT )
		strAccelText += CExtEditHotKeyBase::stat_GetSingleKeyName( hKeyboardLayout, VK_MENU, strTextBetweenKeys );
	if( _dataAccel.key == 0 )
		return strAccelText;
	if( _dataAccel.fVirt & FVIRTKEY )
		strAccelText += CExtEditHotKeyBase::stat_GetSingleKeyName( hKeyboardLayout, _dataAccel.key );
	else
		strAccelText += (TCHAR)_dataAccel.key;
	return strAccelText;
}

void CExtEditHotKeyBase::_SyncEditText()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
CExtSafeString strText = OnGetAccelText();
	SetRedraw( FALSE );
	CExtEditBase::SetSel( 0, -1 );
	CExtEditBase::ReplaceSel( strText );
	CExtEditBase::SetSel( -1, -1 );
	SetRedraw( TRUE );
	Invalidate();
}

bool CExtEditHotKeyBase::IsAccelDefined() const
{
	if( KeyGet() != 0 )
		return true;
	return false;
}

bool CExtEditHotKeyBase::IsEmpty() const
{
	ASSERT_VALID( this );
	if( KeyGet() == 0 && ModifiersGet() == 0 )
		return true;
	return false;
}

void CExtEditHotKeyBase::Empty()
{
	_Empty();
	_SyncEditText();
}

void CExtEditHotKeyBase::_Empty()
{
	ASSERT_VALID( this );
	::memset( &m_dataAccel, 0, sizeof(ACCEL) );
}

const ACCEL & CExtEditHotKeyBase::AccelGet() const
{
	ASSERT_VALID( this );
	return m_dataAccel;
}

void CExtEditHotKeyBase::AccelSet( const ACCEL & _dataAccel )
{
	ASSERT_VALID( this );
	__EXT_MFC_MEMCPY( &m_dataAccel, sizeof(ACCEL), &_dataAccel, sizeof(ACCEL) );
	_SyncEditText();
}

WORD CExtEditHotKeyBase::KeyGet() const
{
	ASSERT_VALID( this );
	return m_dataAccel.key;
}

void CExtEditHotKeyBase::KeySet( WORD nKey )
{
	ASSERT_VALID( this );
	if( m_dataAccel.key == nKey )
		return;
	m_dataAccel.key = nKey;
	_SyncEditText();
	OnAccelChanged();
	if( IsAccelDefined() )
		OnAccelDefined();
}

BYTE CExtEditHotKeyBase::ModifiersGet() const
{
	ASSERT_VALID( this );
	return m_dataAccel.fVirt;
}

void CExtEditHotKeyBase::ModifiersSet( BYTE nModifiers )
{
	ASSERT_VALID( this );
	if( m_dataAccel.fVirt == nModifiers )
		return;
	m_dataAccel.fVirt = nModifiers;
	_SyncEditText();
	OnAccelChanged();
	if( IsAccelDefined() )
		OnAccelDefined();
}

BYTE CExtEditHotKeyBase::SimpleKeyModifiersGet() const
{
	ASSERT_VALID( this );
	return m_nSimpleKeyModifiers;
}

void CExtEditHotKeyBase::SimpleKeyModifiersSet( 
	BYTE nModifiers 
	)
{
	ASSERT_VALID( this );
	m_nSimpleKeyModifiers = nModifiers;
	m_nSimpleKeyModifiers &= FCONTROL|FALT|FSHIFT;
}

bool CExtEditHotKeyBase::AllowTabKeyInputGet() const
{
	ASSERT_VALID( this );
	return m_bAllowTabKeyInput;
}

void CExtEditHotKeyBase::AllowTabKeyInputSet( 
	bool bAllowTabKeyInput 
	)
{
	ASSERT_VALID( this );
	m_bAllowTabKeyInput = bAllowTabKeyInput;
}

CExtSafeString CExtEditHotKeyBase::OnGetEmptyText()
{
	ASSERT_VALID( this );
	return CExtSafeString( _T("") );
}

CExtSafeString CExtEditHotKeyBase::OnGetAccelText()
{
	ASSERT_VALID( this );
	if( IsEmpty() ) 
		return OnGetEmptyText();
CExtSafeString strText = stat_FormatAccelText( m_dataAccel );
	return strText;
}

bool CExtEditHotKeyBase::OnTestEraseAccel( 
	const ACCEL & _dataAccel 
	)
{
	ASSERT_VALID( this );
	if(		(	_dataAccel.key == VK_BACK
			||	_dataAccel.key == VK_DELETE
			||	_dataAccel.key == VK_ESCAPE
			)
		&&	( _dataAccel.fVirt&(FCONTROL|FALT|FSHIFT) ) == 0
		)
		return true;
	return false;
}

bool CExtEditHotKeyBase::OnAccelChanging( const ACCEL & _dataAccelOld, const ACCEL & _dataAccelNew )
{
	ASSERT_VALID( this );
	_dataAccelOld;
	_dataAccelNew;
	return true; // allow any changes
}

void CExtEditHotKeyBase::OnAccelChanged()
{
	ASSERT_VALID( this );
}

void CExtEditHotKeyBase::OnAccelDefined()
{
	ASSERT_VALID( this );
}

bool CExtEditHotKeyBase::OnTestSimpleKey( WORD nKey  )
{
	ASSERT_VALID( this );
	if(		( 0x30 <= nKey && nKey <= 0x39 ) // Numbers
		||	( 0x41 <= nKey && nKey <= 0x5A ) // Letters
		||	nKey == VK_SPACE
		||	nKey == 0xC0 // '`'
		||	nKey == 0xBD // '-'
		||	nKey == 0xBB // '='
		||	nKey == 0xDC // '\\'
		||	nKey == 0xDB // '['
		||	nKey == 0xDD // ']'
		||	nKey == 0xBA // ';'
		||	nKey == 0xDE // '\''
		||	nKey == 0xBC // ','
		||	nKey == 0xBE // '.'
		||	nKey == 0xBF // '/'
		||	nKey == VK_LEFT
		||	nKey == VK_RIGHT
		||	nKey == VK_UP
		||	nKey == VK_DOWN
		||	nKey == VK_INSERT
		||	nKey == VK_DELETE
		||	nKey == VK_HOME
		||	nKey == VK_END
		||	nKey == VK_PRIOR
		||	nKey == VK_NEXT
		)
		return true;
	return false;
}

bool CExtEditHotKeyBase::OnTestDisabledKey( WORD nKey )
{
	ASSERT_VALID( this );
	if(		nKey == VK_NUMLOCK
		||	nKey == VK_PRINT
		||	nKey == VK_PAUSE
		)
		return true;
	return false;
}

BOOL CExtEditHotKeyBase::PreTranslateMessage( MSG * pMsg )
{
	ASSERT_VALID( this );
	if(		pMsg->message != WM_KEYDOWN
		&&	pMsg->message != WM_KEYUP
		&&	pMsg->message != WM_SYSKEYDOWN
		&&	pMsg->message != WM_SYSKEYUP
		)
		return CExtEditBase::PreTranslateMessage( pMsg );
	if( OnTestDisabledKey( (WORD)pMsg->wParam ) )
		return TRUE;
HWND hWndFocus = ::GetFocus();
	if( hWndFocus == NULL || hWndFocus != m_hWnd )
		return CExtEditBase::PreTranslateMessage( pMsg );
bool bDown = ( pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN )	? true : false;
	if( ! bDown )
		return TRUE;
bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
	if( pMsg->wParam == VK_TAB && (!AllowTabKeyInputGet()) )
	{
		if( bAlt || bCtrl )
			return TRUE;
		::SendMessage( ::GetParent(m_hWnd), WM_NEXTDLGCTL, bShift ? 1 : 0, 0 );
		return TRUE;
	} // if( pMsg->wParam == VK_TAB && (!AllowTabKeyInputGet()) )
ACCEL _dataAccelSave;
	__EXT_MFC_MEMCPY(
		&_dataAccelSave,
		sizeof(ACCEL),
		&m_dataAccel, 
		sizeof(ACCEL) 
		);
	_Empty();
	m_dataAccel.fVirt &= ~(FCONTROL|FALT|FSHIFT);
	if(		pMsg->wParam != VK_CONTROL
		&&	pMsg->wParam != VK_MENU
		&&	pMsg->wParam != VK_SHIFT
		)
	{
		m_dataAccel.fVirt |= FVIRTKEY;
		m_dataAccel.key = (WORD)pMsg->wParam;
	}
	else
		m_dataAccel.key = 0;
	if( bCtrl )
		m_dataAccel.fVirt |= FCONTROL;
	if( bAlt )
		m_dataAccel.fVirt |= FALT;
	if( bShift )
		m_dataAccel.fVirt |= FSHIFT;
	if(		( m_dataAccel.fVirt&(FCONTROL|FALT|FSHIFT) ) == 0
		&&	SimpleKeyModifiersGet() != 0
		&&	OnTestSimpleKey( m_dataAccel.key )
		)
		m_dataAccel.fVirt |= SimpleKeyModifiersGet();
	if( OnTestEraseAccel(m_dataAccel) )
		_Empty();
	if( ::memcmp( &_dataAccelSave, &m_dataAccel, sizeof(ACCEL) ) == 0 )
		return TRUE;
	if( ! OnAccelChanging( _dataAccelSave, m_dataAccel ) )
	{
		__EXT_MFC_MEMCPY( &m_dataAccel, sizeof(ACCEL), &_dataAccelSave, sizeof(ACCEL) );
		return TRUE;
	}
	_SyncEditText();
	OnAccelChanged();
	if( IsAccelDefined() )
		OnAccelDefined();
	return TRUE;
}

LRESULT CExtEditHotKeyBase::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	if( WM_KEYFIRST <= message && message <= WM_KEYLAST )
		return 0;
	if(		message == WM_COPY
		||	message == WM_CUT
		||	message == WM_PASTE
		||	message == WM_CLEAR
		||	message == EM_UNDO
		)
		return 0;
	if(		message == WM_RBUTTONUP
		||	message == WM_RBUTTONDOWN
		||	message == WM_RBUTTONDBLCLK
		)
	{
		if( ::GetFocus() != m_hWnd )
			SetFocus();
		return 0;
	}
	return CExtEditBase::WindowProc( message, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditHotKey

IMPLEMENT_DYNCREATE( CExtEditHotKey, CExtEditHotKeyBase );

CExtEditHotKey::CExtEditHotKey()
{
}

CExtEditHotKey::~CExtEditHotKey()
{
}

BEGIN_MESSAGE_MAP(CExtEditHotKey, CExtED < CExtEditHotKeyBase >)
	//{{AFX_MSG_MAP(CExtEditHotKey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void AFXAPI __EXT_MFC_DDX_HotKey( CDataExchange * pDX, INT nIDC, ACCEL & value )
{
HWND hWndCtrl = pDX->PrepareCtrl( nIDC );
	ASSERT( hWndCtrl != NULL );
	ASSERT( ::IsWindow( hWndCtrl ) );
CExtEditHotKey * pWnd = 
		DYNAMIC_DOWNCAST( CExtEditHotKey, CWnd::FromHandle( hWndCtrl ) );
	if( pWnd != NULL )
	{
		ASSERT_VALID( pWnd );
		if( pDX->m_bSaveAndValidate )
			value = pWnd->AccelGet();
		else
			pWnd->AccelSet( value );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditWithBehavior

IMPLEMENT_DYNAMIC( CExtEditWithBehavior, CExtEdit );

CExtEditWithBehavior::CExtEditWithBehavior()
	: m_nInternalFlags( __EISF_CLEAN )
	, m_bRichPainting( false )
	, m_clrRichTextNormal( COLORREF(-1L) )
	, m_clrRichTextReadOnly( COLORREF(-1L) )
	, m_clrRichTextDisabled( COLORREF(-1L) )
	, m_clrRichBackNormal( COLORREF(-1L) )
	, m_clrRichBackReadOnly( COLORREF(-1L) )
	, m_clrRichBackDisabled( COLORREF(-1L) )
	, m_bHandlingWmCut( false )
{
}

CExtEditWithBehavior::~CExtEditWithBehavior()
{
}

BEGIN_MESSAGE_MAP( CExtEditWithBehavior, CExtEdit )
	//{{AFX_MSG_MAP(CExtEditWithBehavior)
	ON_WM_KEYDOWN()
//	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
//	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_CUT, OnCut )
	ON_MESSAGE( WM_PASTE, OnPaste )
	ON_MESSAGE( WM_CLEAR, OnClear )
	ON_MESSAGE( WM_SETTEXT, OnSetText )
	ON_NOTIFY_REFLECT( EN_PROTECTED, OnProtected )
END_MESSAGE_MAP()

void CExtEditWithBehavior::PreSubclassWindow()
{
	CExtEdit::PreSubclassWindow();
	m_bRichMode = m_bHandlingWmCut = false;
CExtSafeString sClassName;
	::GetClassName( m_hWnd, LPTSTR( sClassName.GetBuffer( _MAX_PATH+1 ) ), _MAX_PATH );
	sClassName.ReleaseBuffer();
	sClassName.MakeLower();
	if( sClassName.Find( _T("rich") ) >= 0 )
	{
		m_bRichMode = true;
		m_bRichPainting = true;
		::SendMessage( m_hWnd, EM_SETEVENTMASK, 0, ENM_CHANGE|ENM_SELCHANGE|ENM_PROTECTED );
		::SendMessage( m_hWnd, EM_SETOPTIONS, WPARAM(ECOOP_AND), LPARAM(ECO_AUTOWORDSELECTION|ECO_AUTOHSCROLL /*|ECO_NOHIDESEL*/ ) );
static CExtNotImplRichEditOleCallBack g_stat_cb_not_impl;
		::SendMessage( m_hWnd, EM_SETOLECALLBACK, 0, (LPARAM)(&g_stat_cb_not_impl) );
		CHARFORMAT cfDefault;
		::memset( &cfDefault, 0, sizeof(CHARFORMAT) );
		cfDefault.cbSize = sizeof(CHARFORMAT);
		cfDefault.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_STRIKEOUT|CFM_FACE|CFM_SIZE|CFM_CHARSET|CFM_PROTECTED|CFM_COLOR;
		cfDefault.bCharSet = DEFAULT_CHARSET;
		cfDefault.dwEffects = CFE_PROTECTED; 
		//cfDefault.yHeight = 180;
		//cfDefault.crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
		CFont * pFont = GetFont();
		if( pFont->GetSafeHandle() == NULL )
		{
			CWnd * pWndParent = GetParent();
			if( pWndParent->GetSafeHwnd() != NULL )
				pFont = pWndParent->GetFont();
			if( pFont->GetSafeHandle() == NULL )
				pFont = &(PmBridge_GetPM()->m_FontNormal);
		}
		LOGFONT _lf; ::memset( &_lf, 0, sizeof(LOGFONT) );
		pFont->GetLogFont( &_lf );
#if (! defined _RICHEDIT_VER )
		USES_CONVERSION;
		strcpy( cfDefault.szFaceName, T2CA(_lf.lfFaceName) );
#else
#if ( _RICHEDIT_VER < 0x0200 )
USES_CONVERSION;
		strcpy( cfDefault.szFaceName, T2CA(_lf.lfFaceName) );
#else
		__EXT_MFC_STRCPY( cfDefault.szFaceName, sizeof(cfDefault.szFaceName)/sizeof(cfDefault.szFaceName[0]), _lf.lfFaceName );
#endif
#endif
		CWindowDC dcDesktop( NULL );
		cfDefault.yHeight = ::MulDiv( _lf.lfHeight, 72 * 20, dcDesktop.GetDeviceCaps( LOGPIXELSY ) );
		cfDefault.yHeight = abs( cfDefault.yHeight );
		cfDefault.bCharSet = DEFAULT_CHARSET;
		bool bEnabled = IsWindowEnabled() ? true : false, bReadOnly = ( ( GetStyle() & ES_READONLY ) != 0 ) ? true : false;
		cfDefault.crTextColor = OnQueryTextColor();
		if( cfDefault.crTextColor == COLORREF(-1L) )
		{
			cfDefault.crTextColor = bEnabled ? ( bReadOnly ? m_clrRichTextReadOnly : m_clrRichTextNormal ) : m_clrRichTextDisabled;
			if( cfDefault.crTextColor == COLORREF(-1L) )
				cfDefault.crTextColor = ::GetSysColor( bEnabled ? ( bReadOnly ? COLOR_3DSHADOW : COLOR_WINDOWTEXT ) : COLOR_GRAYTEXT );
		}
		COLORREF clrBack = OnQueryBackColor();
		if( clrBack == COLORREF(-1L) )
		{
			clrBack = bEnabled ? ( bReadOnly ? m_clrRichBackReadOnly : m_clrRichBackNormal ) : m_clrRichBackDisabled;
			if( clrBack == COLORREF(-1L) )
				clrBack = ::GetSysColor( bEnabled ? ( bReadOnly ? COLOR_3DFACE : COLOR_WINDOW ) : COLOR_3DFACE );
		}
		Rich_SetBackgroundColor( FALSE, clrBack );

		Rich_HideSelection( true, false );
		CHARRANGE crRestore;
		Rich_GetSel( crRestore );
		Rich_SetSel( 0, -1 ); 
		SendMessage( EM_SETCHARFORMAT, SCF_DEFAULT, LPARAM(&cfDefault) );
		SendMessage( EM_SETCHARFORMAT, SCF_SELECTION, LPARAM(&cfDefault) );
		Rich_SetSelectionColor( cfDefault.crTextColor );
		Rich_SetSel( crRestore );
		Rich_HideSelection( false, false );
		m_bRichPainting = false;
	}
}

void CExtEditWithBehavior::PostNcDestroy()
{
	m_bHandlingWmCut = false;
	CExtEdit::PreSubclassWindow();
}

LRESULT CExtEditWithBehavior::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( m_bRichMode && WM_KEYFIRST <= message && message <= WM_KEYLAST )
	{
		bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
		bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
		bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
		if( ! bAlt )
		{
			if(		bCtrl && ! bShift 
				&&	(	INT(wParam) == VK_INSERT 
				||	INT(wParam) == INT( _T('C') )
				)
				)
			{
				if( message == WM_KEYDOWN )
					SendMessage( WM_COPY, 0, 0 );
				return 0L;
			}
			
			if(		( bCtrl && (!bShift) && int(wParam) == int( _T('V') ) )
				||	( (!bCtrl) && bShift && int(wParam) == VK_INSERT )
				)
			{
				if( message == WM_KEYDOWN && ( GetStyle() & ES_READONLY ) == 0 )
					SendMessage( WM_PASTE, 0, 0 );
				return 0L;
			}
			if(		( bCtrl && (!bShift) && int(wParam) == int( _T('X') ) )
				||	( (!bCtrl) && bShift && int(wParam) == VK_DELETE )
				)
			{
				if( message == WM_KEYDOWN )
					SendMessage( ( ( GetStyle() & ES_READONLY ) == 0 ) ? WM_CUT : WM_COPY, 0, 0 );
				return 0L;
			}
		}
	}
	return CExtEdit::WindowProc( message, wParam, lParam );
}

void CExtEditWithBehavior::OnProtected( NMHDR * pNMHDR, LRESULT * pResult )
{
	ASSERT_VALID( this );
	if( ! m_bRichMode )
		return;
ENPROTECTED * pEP = (ENPROTECTED*)pNMHDR;
	(*pResult) = _Rich_OnProtected( pEP );
}

BOOL CExtEditWithBehavior::_Rich_OnProtected( ENPROTECTED * pEP )
{
	ASSERT_VALID( this );
	if( pEP->msg != EM_SETCHARFORMAT )
	{
		_Rich_OnDraw();
		//HWND hWndParent = ::GetParent( m_hWnd );
		//::SendMessage( hWndParent, WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), EN_CHANGE ), LPARAM(m_hWnd) );
	}
	return FALSE;
}

CExtSafeString CExtEditWithBehavior::TextGet() const
{
	ASSERT_VALID( this );
CExtSafeString strRetVal( _T("") );
	if( GetSafeHwnd() == NULL )
		return strRetVal;
	if( m_bRichMode )
		strRetVal = Rich_PlainText_Get();
	else
	{
		CString strText;
		GetWindowText( strText );
		strRetVal = LPCTSTR(strText);
	}
	return strRetVal;
}

CExtSafeString CExtEditWithBehavior::TextGetTrimmed() const
{
	ASSERT_VALID( this );
CExtSafeString strText = TextGet();
    strText.TrimLeft();
    strText.TrimRight();
    return CExtSafeString( strText );
}

void CExtEditWithBehavior::TextSet( __EXT_MFC_SAFE_LPCTSTR strText )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
LPCTSTR strTextToSet = (LPCTSTR(strText) != NULL) ? LPCTSTR(strText) : _T("");
	if( m_bRichMode )
	{
		ASSERT( ! m_bRichPainting );
		Rich_PlainText_Set( strTextToSet );
		_Rich_OnDraw();
	}
	else
		SetWindowText( strTextToSet );
}

void CExtEditWithBehavior::_Rich_OnDraw()
{
	ASSERT_VALID( this );
//	if( ! m_bRichMode )
//		return;
//	if( GetSafeHwnd() == NULL )
//		return;
//	if( m_bRichPainting )
//		return;
//	m_bRichPainting = true;
//	. . .
//	m_bRichPainting = false;
}

CExtSafeString CExtEditWithBehavior::TextGetValid() const
{
	ASSERT_VALID( this );
	return TextGet();
}

void CExtEditWithBehavior::_UpdateContent()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
CExtSafeString strTextValid = TextGetValid();
CExtSafeString strTextCurrent = TextGet();
	if( strTextValid != strTextCurrent )
	{
		//SetWindowText( LPCTSTR(strTextValid) );
		LPCTSTR strTextToSet = (LPCTSTR(strTextValid) != NULL) ? LPCTSTR(strTextValid) : _T("");
		if( m_bRichMode )
		{
			ASSERT( ! m_bRichPainting );
			Rich_PlainText_Set( strTextToSet );
			_Rich_OnDraw();
		}
		else
			SetWindowText( strTextToSet );
	}
}

bool CExtEditWithBehavior::Behavior_OnQueryCharEnabledTyping( __EXT_MFC_SAFE_TCHAR _tchr ) const
{
	ASSERT_VALID( this );
	_tchr;
	return true;
}

LRESULT CExtEditWithBehavior::OnCut( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
	if( m_bHandlingWmCut )
		return 0L;
	m_bHandlingWmCut = true;
HWND hWndOwn = m_hWnd;
int nStart, nEnd;
	GetSel( nStart, nEnd );
	if( nStart < nEnd )
	{
		SendMessage( WM_COPY );
		SendMessage( WM_KEYDOWN, VK_DELETE );
	}
	if(		( ! ::IsWindow( hWndOwn ) )
		||	LPVOID( CWnd::FromHandlePermanent( hWndOwn ) ) != LPVOID(this)
		)
		return 0L;
	m_bHandlingWmCut = false;
	return 0L;
}
	
LRESULT CExtEditWithBehavior::OnClear( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
int nStart, nEnd;
	GetSel(nStart, nEnd);
	if( nStart < nEnd )
		SendMessage( WM_KEYDOWN, VK_DELETE );
	return 0;
}

LRESULT CExtEditWithBehavior::OnPaste( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	if( ( GetStyle() & ES_READONLY ) != 0 )
		return 0;
	wParam;
	lParam;
int nStart, nEnd;
	GetSel(nStart, nEnd);
	Default();
CExtSafeString strTextValid = TextGetValid();
CExtSafeString strTextCurrent = TextGet();
	if( strTextValid != strTextCurrent )
	{
		//SetWindowText( LPCTSTR(strTextValid) );
		LPCTSTR strTextToSet = (LPCTSTR(strTextValid) != NULL) ? LPCTSTR(strTextValid) : _T("");
		if( m_bRichMode )
		{
			ASSERT( ! m_bRichPainting );
			Rich_PlainText_Set( strTextToSet );
			_Rich_OnDraw();
		}
		else
			SetWindowText( strTextToSet );

		SetSel( nStart, nEnd );
	}
	return 0;
}

LRESULT CExtEditWithBehavior::OnSetText( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
LRESULT nResult = Default();
CExtSafeString strText = TextGetValid();
	if( strText != LPCTSTR(lParam) )
	{
		//SetWindowText( strText );
		LPCTSTR strTextToSet = (LPCTSTR(strText) != NULL) ? LPCTSTR(strText) : _T("");
		if( m_bRichMode )
		{
			ASSERT( ! m_bRichPainting );
			Rich_PlainText_Set( strTextToSet );
			_Rich_OnDraw();
		}
		else
			SetWindowText( strTextToSet );
	}
	else if( m_bRichMode )
	{
			_Rich_OnDraw();
	}
	if( IsWindowVisible() )
		Invalidate();
	return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditWithBehavior::CBehaviorBase

CExtEditWithBehavior::CBehaviorBase::CBehaviorBase()
	: m_pEditWithBehaviorWnd( NULL )
	, m_nBehaviorFlags( 0 )
{
}

CExtEditWithBehavior::CBehaviorBase::~CBehaviorBase()
{
}

bool CExtEditWithBehavior::CBehaviorBase::_FlagsModify( UINT nAdd, UINT nRemove )
{
UINT nFlags = ( m_nBehaviorFlags & (~nRemove) ) | nAdd;
	if( m_nBehaviorFlags == nFlags )
		return false;
	m_nBehaviorFlags = nFlags;
	_Redraw();
	return true;
}

UINT CExtEditWithBehavior::CBehaviorBase::_FlagsGet() const
{
	return m_nBehaviorFlags;
}

void CExtEditWithBehavior::CBehaviorBase::Behavior_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	if( m_pEditWithBehaviorWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	m_pEditWithBehaviorWnd->OnChar( uChar, nRepCnt, nFlags );
	m_pEditWithBehaviorWnd->_Rich_OnDraw();
}

void CExtEditWithBehavior::CBehaviorBase::Behavior_OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	m_pEditWithBehaviorWnd->OnKeyDown( uChar, nRepCnt, nFlags );
//	if( uChar == VK_DELETE || uChar == VK_BACK )
//		m_pEditWithBehaviorWnd->_Rich_OnDraw();
}

void CExtEditWithBehavior::CBehaviorBase::Behavior_OnKillFocus( CWnd * pNewWnd ) 
{
	if( m_pEditWithBehaviorWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	m_pEditWithBehaviorWnd->OnKillFocus( pNewWnd );
}

LRESULT CExtEditWithBehavior::CBehaviorBase::Behavior_OnPaste( WPARAM wParam, LPARAM lParam )
{
	if( m_pEditWithBehaviorWnd->GetSafeHwnd() == NULL )
		return 0L;
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	if( ( m_pEditWithBehaviorWnd->GetStyle() & ES_READONLY ) != 0 )
		return 0;
LRESULT nRetVal = m_pEditWithBehaviorWnd->OnPaste( wParam, lParam );
//	m_pEditWithBehaviorWnd->_Rich_OnDraw();
	return nRetVal;
}

LRESULT CExtEditWithBehavior::CBehaviorBase::_Default()
{
	if( m_pEditWithBehaviorWnd->GetSafeHwnd() == NULL )
		return 0L;
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	return m_pEditWithBehaviorWnd->Default();
}

void CExtEditWithBehavior::CBehaviorBase::_Redraw()
{
	if( m_pEditWithBehaviorWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	m_pEditWithBehaviorWnd->_UpdateContent();
}

bool CExtEditWithBehavior::CBehaviorBase::Behavior_OnQueryCharEnabledTyping( __EXT_MFC_SAFE_TCHAR _tchr ) const
{
	if( m_pEditWithBehaviorWnd->GetSafeHwnd() == NULL )
		return true;
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	return m_pEditWithBehaviorWnd->Behavior_OnQueryCharEnabledTyping( _tchr );
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditWithBehavior::CBehaviorMasked

CExtEditWithBehavior::CBehaviorMasked::CBehaviorMasked(
	__EXT_MFC_SAFE_LPCTSTR strMask // = _T("")
	)
	: m_strMask( strMask )
	, m_bInsertMode( false )
	, m_bNullableMode( true )
	, m_bFieldSelectionMode( true )
	, m_bBack( false )
	, m_tchrNullChar( _T('_') )
{
// default mask symbol number
CharRule _rule1( __EXT_MFC_SAFE_TCHAR( _T('#') ), (CharRule::ValidationFunction)_istdigit );
	m_arrRules.Add( _rule1 );
// default mask symbol letter
CharRule _rule2( __EXT_MFC_SAFE_TCHAR( _T('@') ), (CharRule::ValidationFunction)_istalpha );
	m_arrRules.Add( _rule2 );
// default mask symbol number and letter
CharRule _rule3( __EXT_MFC_SAFE_TCHAR( _T('$') ), (CharRule::ValidationFunction)_istalnum );
	m_arrRules.Add( _rule3 );
}

__EXT_MFC_SAFE_LPCTSTR CExtEditWithBehavior::CBehaviorMasked::MaskGet() const
{
	return __EXT_MFC_SAFE_LPCTSTR(LPCTSTR(m_strMask));
}

void CExtEditWithBehavior::CBehaviorMasked::MaskSet( __EXT_MFC_SAFE_LPCTSTR strMask )
{
	if( m_strMask == LPCTSTR(strMask) )
		return;
	m_strMask = LPCTSTR(strMask);
	_ShowNullableMask();
	_Redraw();
}

CExtSafeString CExtEditWithBehavior::CBehaviorMasked::_GetNumericText() const
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
CExtSafeString strResult;
INT nIndex, nCount = INT(strText.GetLength());
	for( nIndex = 0; nIndex < nCount; nIndex++ )
	{
		__EXT_MFC_SAFE_TCHAR _tchr = strText[ nIndex ];
		if(_istdigit( _tchr ) )
		{
			TCHAR _tstr[2] = { _tchr, TCHAR(0) };
			strResult += _tstr;
		}
	}
	return strResult;
}

CExtSafeString CExtEditWithBehavior::CBehaviorMasked::Behavior_GetValidText(
	bool bSetSel // = false
	) const
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	bSetSel;
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
INT nMaskLen = INT( m_strMask.GetLength() );
	if( nMaskLen == 0 )
		return strText;
CExtSafeString strNewText;
bool bNullableMode = NullableModeGet();
__EXT_MFC_SAFE_TCHAR _tchrNullChar = NullCharGet();
INT nLength = INT(strText.GetLength()), nPos, nMaskPos;
	for( nPos = 0, nMaskPos = 0; nPos < nLength; nPos++, nMaskPos++ )
	{
		__EXT_MFC_SAFE_TCHAR _tchr = strText[nPos];
		__EXT_MFC_SAFE_TCHAR _tchrMask = static_cast < __EXT_MFC_SAFE_TCHAR > ( nMaskPos < nMaskLen ? m_strMask[ nMaskPos ] : 0 );
		if( ! TCHAR(_tchrMask) )
			break;
		INT nSymbolIndex, nSymbolCount = INT(m_arrRules.GetSize());
		for( nSymbolIndex = 0; nSymbolIndex < nSymbolCount; nSymbolIndex++ )
		{
			const CharRule & symbol = m_arrRules[ nSymbolIndex ];
			if( _tchrMask != symbol || ! symbol.Validate(_tchr) )
				continue;
			for( ; nMaskPos < nMaskLen; nMaskPos++ )
			{
				_tchrMask = m_strMask[ nMaskPos ];
				if( _tchrMask == symbol )
				{
					TCHAR _tstr[2] = { symbol.Convert(_tchr), TCHAR(0) };
					strNewText += _tstr;
					break;
				} 
				else
				{
					int iSymbol2 = 0;
					for( iSymbol2 = 0; iSymbol2 < nSymbolCount; iSymbol2++ )
					{
						if( _tchrMask == m_arrRules[ iSymbol2 ] )
						{
							TCHAR _tstr[2] = { symbol.Convert(_tchr), TCHAR(0) };
							strNewText += _tstr;
							break;
						}
					}
					if( iSymbol2 < nSymbolCount )
						break;
					TCHAR _tstr[2] = { TCHAR(_tchrMask), TCHAR(0) };
					strNewText += _tstr;
				}
			}
			break;
		}
		if( nSymbolIndex == nSymbolCount )
		{
			if( _tchr == _tchrMask )
			{
				for( nSymbolIndex = 0; nSymbolIndex < nSymbolCount; nSymbolIndex++ )
				{
					if( _tchrMask == m_arrRules[nSymbolIndex] )
						break;
				}
				if (nSymbolIndex == nSymbolCount)
				{
					TCHAR _tstr[2] = { TCHAR(_tchr), TCHAR(0) };
					strNewText += _tstr;
					continue;
				}
			}
			if( bNullableMode && _tchr == _tchrNullChar)
			{
				TCHAR _tstr[2] = { TCHAR(_tchr), TCHAR(0) };
				strNewText += _tstr;
				continue;
			}
			break;
		}
	}
	return strNewText;
}

void CExtEditWithBehavior::CBehaviorMasked::Behavior_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	if( m_strMask.IsEmpty() )
	{
		CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags );
		return;
	}

	if( ( m_pEditWithBehaviorWnd->GetStyle() & ES_READONLY ) != 0 )
		return;
	if( uChar == VK_BACK )
		return;
__EXT_MFC_SAFE_TCHAR _tchr = static_cast < TCHAR > ( uChar );
INT nMaskLen = INT(m_strMask.GetLength());
	if( nMaskLen == 0 )
	{
		if( Behavior_OnQueryCharEnabledTyping( _tchr ) )
			CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags );
		return;
	}
int nStart, nEnd;
	m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
INT nLength = INT(strText.GetLength());
INT _posNullChar = -1;
bool _nullCharBeforePos = _NullCharBefore( nStart );
bool bInsertMode = InsertModeGet();
bool bNullableMode = NullableModeGet();
bool _nullCharIs = _IsNullChar( _posNullChar );
	if(		nLength == nMaskLen
 		&&	(	( bInsertMode && ( bNullableMode && ! _nullCharIs ) )
			||	( bInsertMode && ! bNullableMode )
			)
		&&	nStart == nEnd
		)
	{
		OnCheckText( true );
		return;
	}
	if( ! ( _istprint( _tchr ) || IsCharAlphaNumeric( _tchr ) ) )
	{
		if( Behavior_OnQueryCharEnabledTyping( _tchr ) )
			CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags );
		return;
	}
	else
	{
		if( nStart >= m_strMask.GetLength() )
			return;
	}
__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask[nStart];
__EXT_MFC_SAFE_TCHAR chTextCurrentSymbols = _T('\0');
	if( /*( ! strText.IsEmpty() )*/ nStart < strText.GetLength() && bNullableMode )
		chTextCurrentSymbols = strText.GetAt( nStart );
__EXT_MFC_SAFE_TCHAR _tchrNullChar = NullCharGet();
INT nSymbolIndex, nSymbolCount = INT(m_arrRules.GetSize());
	for( nSymbolIndex = 0; nSymbolIndex < nSymbolCount; nSymbolIndex++ )
	{
		const CharRule & symbol = m_arrRules[nSymbolIndex];
		if( _tchrMask == symbol )
		{
			if( symbol.Validate(_tchr) && Behavior_OnQueryCharEnabledTyping(_tchr) )
			{
				if(		( ! bInsertMode || ( bNullableMode && ( chTextCurrentSymbols == _tchrNullChar ) ) )
					&&	nStart == nEnd
					)
					nEnd = ( nEnd == nLength ? nEnd : ( nStart + 1 ) );
				m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
				_SymbolInsert( CExtSafeString( TCHAR(symbol.Convert(_tchr)) ) );
				m_pEditWithBehaviorWnd->SetSel( nStart + 1, nStart + 1 );
				bool _inserMaskSymbol = _AddSymbolMaskToEnd();
				if(		( _posNullChar != -1 ) && ( _posNullChar < nStart )
					&&	bInsertMode
					&&	( chTextCurrentSymbols != _tchrNullChar )
					&&	! _nullCharBeforePos
					)
					m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
				if(		( bInsertMode && ! bNullableMode )
					||	bNullableMode
					)
					OnCheckText( true );
				if( ! _inserMaskSymbol )
					_GetFieldSelection( nStart );
			}
			return;
		}
	}
	if( _tchrMask == _tchr && Behavior_OnQueryCharEnabledTyping(_tchr) )
	{
		if( ! bInsertMode || ( bNullableMode && ( chTextCurrentSymbols == _tchrNullChar ) ) )
			nEnd = ( nEnd == nLength ? nEnd : ( nStart + 1 ) );
		m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
		_SymbolInsert( CExtSafeString( TCHAR(_tchr) ) );
		m_pEditWithBehaviorWnd->SetSel( nStart + 1, nStart + 1 );
		if(		( _posNullChar != -1 ) && ( _posNullChar < nStart ) 
			&&	bInsertMode
			&&	( chTextCurrentSymbols != _tchrNullChar )
			&&	! _nullCharBeforePos
			)
			m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
		if(		( bInsertMode && ! bNullableMode )
			||	bNullableMode
			)
			OnCheckText( true );
		_GetFieldSelection( nStart );
		return;
	}
CExtSafeString strSymbols;
	for( nSymbolIndex = 0; nSymbolIndex < nSymbolCount; nSymbolIndex++ )
		strSymbols += TCHAR(__EXT_MFC_SAFE_TCHAR(m_arrRules[nSymbolIndex]));
	for( nSymbolIndex = 0; nSymbolIndex < nSymbolCount; nSymbolIndex++ )
	{
		const CharRule & symbol = m_arrRules[nSymbolIndex];
		if( ! symbol.Validate( _tchr ) )
			continue;
		CExtSafeString strMaskPortion = m_strMask.Mid( nStart );
		INT nMaskPos = INT( strMaskPortion.FindOneOf( strSymbols ) );
		if( nMaskPos >= 0 && TCHAR(strMaskPortion[nMaskPos]) == TCHAR(__EXT_MFC_SAFE_TCHAR(symbol)) && Behavior_OnQueryCharEnabledTyping(_tchr) )
		{
			if( nStart != nEnd )
			{
				_SymbolInsert( CExtSafeString( TCHAR(symbol.Convert(_tchr)) ) );
				m_pEditWithBehaviorWnd->SetSel( nStart + nMaskPos + 1, nStart + nMaskPos + 1 );
				if( bNullableMode )
					OnCheckText( true );
				return;
			}
			m_pEditWithBehaviorWnd->SetSel( nStart, nStart + nMaskPos );
			m_pEditWithBehaviorWnd->ReplaceSel( strMaskPortion.Left( nMaskPos ), TRUE );
			Behavior_OnChar( uChar, nRepCnt, nFlags );
			return;
		}
	}
}

bool CExtEditWithBehavior::CBehaviorMasked::_AddSymbolMaskToEnd()
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
INT nMaskSymbolCount = _GetCountMaskSymbolsFromEnd();
	if( nMaskSymbolCount == 0 )
		return false;
int _nStart, _nEnd;
	m_pEditWithBehaviorWnd->GetSel( _nStart, _nEnd );
	ASSERT( _nStart == _nEnd );
CExtSafeString _str = m_pEditWithBehaviorWnd->TextGet();
INT nMaskLen = INT( m_strMask.GetLength() );
	if( ( nMaskLen - _nStart ) != nMaskSymbolCount )
		return false;
	for( INT i = _nStart; i < nMaskLen; i++ )
	{
		__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask.GetAt( i );
		TCHAR _tstr[2] = { TCHAR(_tchrMask), TCHAR(0) };
		_str += _tstr;
	}
	m_pEditWithBehaviorWnd->TextSet( _str );
	m_pEditWithBehaviorWnd->SetSel( _nStart, _nEnd );
	return true;
}

INT CExtEditWithBehavior::CBehaviorMasked::_GetCountMaskSymbolsFromEnd()
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
INT nMaskLen = INT( m_strMask.GetLength() );
INT nMaskSymbolCount = 0;
	if( nMaskLen == 0 )
		return nMaskSymbolCount;
INT i = nMaskLen - 1;
	for( ; i >= 0; i-- )
	{
		__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask.GetAt( i );

		if( m_arrRules.FindPos( _tchrMask ) < 0 )
			nMaskSymbolCount ++;
		else
			break;
	}
	return nMaskSymbolCount;
}

void CExtEditWithBehavior::CBehaviorMasked::Behavior_OnKeyDown(UINT uChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	if( m_strMask.IsEmpty() )
	{
		CBehaviorBase::Behavior_OnKeyDown( uChar, nRepCnt, nFlags );
		return;
	}

	switch( uChar )
	{
	case VK_BACK:
	{
		if( ( m_pEditWithBehaviorWnd->GetStyle() & ES_READONLY ) != 0 )
			return;
		m_bBack = true;
		int nStart, nEnd;
		m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
		CExtSafeString str = m_pEditWithBehaviorWnd->TextGet();
		INT nLength = INT(str.GetLength());
		if( nStart != nEnd )
			m_pEditWithBehaviorWnd->SendMessage( WM_KEYDOWN, VK_DELETE );
		else if( nEnd != 0 )
		{
			nEnd --;
			bool bJumpMode = false;
			INT nMaskLen = INT( m_strMask.GetLength() );
			if( nEnd < nMaskLen )
			{
				for( ; nEnd >= 0 ; )
				{
					m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
					__EXT_MFC_SAFE_TCHAR tchrMask = m_strMask[ nEnd ];
					__EXT_MFC_SAFE_TCHAR tchrSymbol = str.GetAt( nEnd );
					if(		( nEnd == ( nMaskLen - 1 ) )
						&&	tchrMask == tchrSymbol
						)
						return;
					bool bSymbolFind = false;
					for( INT nSymbolIndex = 0, nSymbolCount = INT(m_arrRules.GetSize()); nSymbolIndex < nSymbolCount; nSymbolIndex++ )
					{
						const CharRule & symbol = m_arrRules[nSymbolIndex];
						if( tchrMask == symbol )
						{
							bSymbolFind = true;
							break;
						}
					}
					if( bSymbolFind	)
					{
						if( ! bJumpMode )
							m_pEditWithBehaviorWnd->SendMessage( WM_KEYDOWN, VK_DELETE );
						break;
					}
					else
					{
						if( ( nEnd + 1 ) == nLength )
						{
							m_pEditWithBehaviorWnd->SendMessage( WM_KEYDOWN, VK_DELETE );
							break;
						}
						bJumpMode = true;
						nEnd --;
						continue;
					}
				} // for( ; nEnd > 0 ; )
				if( bJumpMode )
				{
					nEnd ++;
					m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
				}
			} // if( nEnd < nMaskLen )
		} // else if( nEnd != 0 )
	}
	return;
	case VK_DELETE:
	{
		if( ( m_pEditWithBehaviorWnd->GetStyle() & ES_READONLY ) != 0 )
			return;
		int nStart, nEnd;
		bool bNullableMode = NullableModeGet();
		CExtSafeString str = m_pEditWithBehaviorWnd->TextGet();
		INT nLength = INT(str.GetLength());
		m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
		if( nStart == nEnd )
		{
			bool bJumpMode = false;
			INT nMaskLen = INT( m_strMask.GetLength() );
			for( ; true ; )
			{
				if( nEnd < nMaskLen )
				{
					__EXT_MFC_SAFE_TCHAR tchrMask = m_strMask[ nEnd ];
					__EXT_MFC_SAFE_TCHAR tchrSymbol = str.GetAt( nEnd );
					if(		( nEnd == ( nMaskLen - 1 ) )
						&&	tchrMask == tchrSymbol
						)
						return;
					bool bSymbolFind = false;
					for( INT nSymbolIndex = 0, nSymbolCount = INT(m_arrRules.GetSize()); nSymbolIndex < nSymbolCount; nSymbolIndex++ )
					{
						const CharRule& symbol = m_arrRules[nSymbolIndex];

						if( tchrMask == symbol )
						{
							bSymbolFind = true;
							break;
						}
					}
					if( ! bSymbolFind && nLength == ( nEnd + 1 ) )
						break;
					if( ! bSymbolFind )
					{
						nEnd ++;
						m_pEditWithBehaviorWnd->SendMessage( WM_KEYDOWN, VK_RIGHT );
						bJumpMode = true;
						continue;
					}
					else
						break;
				}
				else
					break;
			} // for( ; true ; )
			if( bJumpMode )
				return;
			if( (! m_bBack) && bNullableMode )
			{
				if( nEnd == nMaskLen )
					return;
				__EXT_MFC_SAFE_TCHAR tchrNullChar = NullCharGet();
				CExtSafeString strText = TCHAR(tchrNullChar);
				m_pEditWithBehaviorWnd->SetSel( nEnd, ( nEnd + 1 ) );
				m_pEditWithBehaviorWnd->ReplaceSel( strText, TRUE );
				m_pEditWithBehaviorWnd->SetSel( ( nEnd + 1 ), ( nEnd + 1 ) );
				return;
			}
			else if( m_bBack && bNullableMode )
			{
				__EXT_MFC_SAFE_TCHAR tchrNullChar = NullCharGet();
				CExtSafeString strText = TCHAR(tchrNullChar);
				m_pEditWithBehaviorWnd->SetSel( nEnd, ( nEnd + 1 ) );
				m_pEditWithBehaviorWnd->ReplaceSel( strText, TRUE );
				m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
				m_bBack = false;
				return;
			}
			m_bBack = false;
		} // if( nStart == nEnd )
	}
	break;
	} // switch( uChar )

	switch( uChar )
	{
	case VK_BACK:
	case VK_DELETE:
		{
			if( ( m_pEditWithBehaviorWnd->GetStyle() & ES_READONLY ) != 0 )
				return;
			int _nStart, _nEnd;
			int nStart, nEnd;
			bool bNullableMode = NullableModeGet();
			__EXT_MFC_SAFE_TCHAR _tchrNullChar = NullCharGet();
			m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
			_DefineNewPosition( _nStart, _nEnd );
			if( nStart != nEnd && _nStart == _nEnd )
			{
				m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
				return;
			}
			CExtSafeString _strNew;
			_MaskSymbolDelete( _strNew );
			//m_pEditWithBehaviorWnd->TextSet( _strNew );
			if( _nStart == _nEnd )
				_strNew.Delete( _nStart, 1 );
			else
				_strNew.Delete( _nStart, ( _nEnd - _nStart ) );
			if( bNullableMode && m_strMask.GetLength() > 0 )
			{
				if( _nStart == _nEnd )
					_strNew.Insert( _nStart, _tchrNullChar );
				else
					for( int i = 0; i < ( _nEnd - _nStart ); i++ )
						_strNew.Insert( _nStart, _tchrNullChar );
			}
			_MaskSymbolInsert( _strNew );
			if( bNullableMode )
				OnCheckText( false );
			m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
		}
	break;
	default:
		CBehaviorBase::Behavior_OnKeyDown( uChar, nRepCnt, nFlags );
	break;
	} // switch( uChar )
}

void CExtEditWithBehavior::CBehaviorMasked::_GetMaskBasedGroupArrays(
	CExtSafeStringArray & arrMaskGroups,
	CExtSafeStringArray * p_arrEnteredValues, // = NULL
	bool bJoinSimilarRules, // = true
	bool bJoinDifferentRules, // = false
	bool bPutNullableCharIntoValueArray // = false
	)
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	arrMaskGroups.RemoveAll();
	if( p_arrEnteredValues != NULL )
		p_arrEnteredValues->RemoveAll();

// (###) ###-#### x####
//-------------------------- with bJoinSimilarRules == true
//  0    1   2     3
//  ###  ### ####  #### ---> save into arrMaskGroups
//-------------------------- with bJoinSimilarRules == false
//  0 1 2 3 4 5 6 7 ....
//  # # # # # # # # # # # # # # ---> save into arrMaskGroups
//

//==================================================================
// sample with different rules # and $, #-digit, $-alpha
//	#$-##-$$
//-------------------------- with bJoinSimilarRules == true and bJoinDifferentRules == true
//	#$-##-$$
//  0  1  2
//	#$ ## $$
//-------------------------- with bJoinSimilarRules == true and bJoinDifferentRules == false
//	#$-##-$$
//  0 1 2  3
//	# $ ## $$
//-------------------------- with bJoinSimilarRules == false and bJoinDifferentRules == true
//	#$-##-$$
//  0  1 2 3 4
//	#$ # # $ $
//-------------------------- with bJoinSimilarRules == false and bJoinDifferentRules == false
//	#$-##-$$
//  0 1 2 3 4 5
//	# $ # # $ $
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
INT nLenText = INT( strText.GetLength() );
INT nLenMask = INT( m_strMask.GetLength() );
INT i;
CExtSafeString strAsmMask, strAsmVal;
	for( i = 0; i < nLenMask; i++ )
	{
		__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask.GetAt(i);
		__EXT_MFC_SAFE_TCHAR _tchrText = ( p_arrEnteredValues != NULL && i < nLenText ) ? __EXT_MFC_SAFE_TCHAR(strText.GetAt(0)) : __EXT_MFC_SAFE_TCHAR(0);
		if( m_arrRules.FindPos( _tchrMask ) < 0 )
			continue;
		bool bAppendToCurrentAsm = false;
		if( bJoinDifferentRules )
			bAppendToCurrentAsm = true;
		else if( strAsmMask.IsEmpty() )
			bAppendToCurrentAsm = true;
		else if(
				bJoinSimilarRules
			&&	TCHAR( strAsmMask.GetAt( strAsmMask.GetLength() - 1 ) ) == TCHAR(_tchrMask)
			)
			bAppendToCurrentAsm = true;
		if( bAppendToCurrentAsm )
		{
			TCHAR _tstr[2] = { TCHAR(_tchrMask), TCHAR(0) };
			strAsmMask += _tstr;
			if( _tchrText != 0 )
			{
				TCHAR _tstr[2] = { TCHAR(_tchrText), TCHAR(0) };
				strAsmVal += _tstr;
			}
		}
		else
		{
			ASSERT( ! strAsmMask.IsEmpty() );
			arrMaskGroups.Add( strAsmMask );
			strAsmMask.Empty();
			if( p_arrEnteredValues != NULL )
			{
				p_arrEnteredValues->Add( strAsmVal );
				strAsmVal.Empty();
			}
		}
	} // for( i = 0; i < nLenMask; i++ )
	if( ! strAsmMask.IsEmpty() )
	{
			arrMaskGroups.Add( strAsmMask );
			strAsmMask.Empty();
			if( p_arrEnteredValues != NULL )
			{
				p_arrEnteredValues->Add( strAsmVal );
				strAsmVal.Empty();
			}
	} // if( ! strAsmMask.IsEmpty() )
	if( (!bPutNullableCharIntoValueArray) && p_arrEnteredValues != NULL )
	{
		__EXT_MFC_SAFE_TCHAR strWithNullableChar[ 2 ] = { NullCharGet(), _T('\0') };
		INT nCount = INT( p_arrEnteredValues->GetSize() );
		for( i = 0; i < nCount; i++ )
			p_arrEnteredValues->ElementAt( i ).Replace( LPCTSTR(strWithNullableChar), _T("") );
	} // if( (!bPutNullableCharIntoValueArray) && p_arrEnteredValues != NULL )
}

void CExtEditWithBehavior::CBehaviorMasked::OnCheckText(
	bool bAppendEndChars
	)
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	ASSERT( m_pEditWithBehaviorWnd->GetSafeHwnd() != NULL );
int nStart, nEnd;
	m_pEditWithBehaviorWnd->GetSel(nStart, nEnd);
INT nLenMask = INT( m_strMask.GetLength() );
CExtSafeString strTextInitial = m_pEditWithBehaviorWnd->TextGet();
INT nLenInitial = INT( strTextInitial.GetLength() );
CExtSafeString strText = strTextInitial;
CExtSafeString strEnd;
INT i = 0;
bool bNullableMode = NullableModeGet();
__EXT_MFC_SAFE_TCHAR _tchrNullChar = NullCharGet();
__EXT_MFC_SAFE_TCHAR _tchrText, _tchrMask;
	if( ( nLenInitial > nLenMask ) && bNullableMode )
	{
		for( i = 0; i < (nLenInitial - 1); i++ )
		{
			_tchrText = strText.GetAt(i);
			if( _tchrText == _tchrNullChar )
			{
				strText.Delete(i);
				break;
			}
		}
		i = 0;
	}
	for( ; ! strText.IsEmpty(); )
	{
		if(	bNullableMode && nLenMask <= i )
		{
			_tchrText = strText.GetAt(0);
			ASSERT( _tchrText == _tchrNullChar );
			break;
		}
		_tchrText = strText.GetAt(0);
		_tchrMask = m_strMask.GetAt(i);
		bool bRuleWasFound = false;
		if(		(	bNullableMode
				&&	_tchrText == _tchrNullChar
				&&	m_arrRules.FindPos( _tchrMask ) >= 0
				)
			||	m_arrRules.CheckRule( _tchrMask, _tchrText, &bRuleWasFound )
			)
		{
			TCHAR _tstr[2] = { TCHAR(_tchrText), TCHAR(0) };
			strEnd += _tstr;
			strText.Delete(0);
			i++;
			continue;
		}
		if( bRuleWasFound )
		{
			strText.Delete(0);
		}
		else
		{
			TCHAR _tstr[2] = { TCHAR(_tchrMask), TCHAR(0) };
			strEnd += _tstr;
			i++;
		}
	}
INT nResetSelPos = min( nStart, nEnd );
INT nLenComposed = INT( strEnd.GetLength() );
	ASSERT( nLenComposed <= nLenMask );
	if( bAppendEndChars )
	{
		if( nLenComposed < nLenMask )
		{
			bool bMoveSelToAppended = ( nEnd == nLenComposed ) ? true : false;
			for( i = nLenComposed; i < nLenMask; i ++ )
			{
				_tchrMask = m_strMask.GetAt(i);
				if( m_arrRules.FindPos( _tchrMask ) < 0 )
				{
					TCHAR _tstr[2] = { TCHAR(_tchrMask), TCHAR(0) };
					strEnd += _tstr;
					nLenComposed ++;
				}
				else
					break;
			}
			if( bMoveSelToAppended )
				nResetSelPos = i;
		}
	}
INT strLen = INT(strEnd.GetLength());
	if( bNullableMode  )
	{
		if( strLen < nLenMask )
		{
			nEnd = strLen;
			for( ; strLen < nLenMask; strLen ++ )
			{
				__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask.GetAt( strLen );
				if( m_arrRules.FindPos( _tchrMask ) >= 0 )
					_tchrMask = _tchrNullChar;
				TCHAR _tstr[2] = { TCHAR(_tchrMask), TCHAR(0) };
				strEnd += _tstr;
			}
		}
	}
	if( strEnd == strTextInitial )
		return;
	OnInputChar( strEnd );
	if( strEnd == strTextInitial )
		return;
	m_pEditWithBehaviorWnd->TextSet( strEnd );
	m_pEditWithBehaviorWnd->SetSel( nResetSelPos, nResetSelPos );
}


void CExtEditWithBehavior::CBehaviorMasked::OnInputChar( CExtSafeString & str )
{
	str;
}

void CExtEditWithBehavior::CBehaviorMasked::_DefineNewPosition( int & _nStart, int & _nEnd )
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
int nStart, nEnd;
	m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
	if( strText.IsEmpty() )
		return;
INT nMaskCountAfterStartPos = 0, nMaskCountBetweenStartPosEndPos = 0, i;
	for( i = 0; i < nStart; i++ )
	{
		__EXT_MFC_SAFE_TCHAR _tchrSymbol = strText.GetAt( i );
		__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask.GetAt( i );
		if( _tchrSymbol == _tchrMask )
			nMaskCountAfterStartPos ++;
	}
	_nStart = nStart - nMaskCountAfterStartPos;
	_nEnd = _nStart;
INT nTextLen = (INT)strText.GetLength();
//INT nMaskLen = (INT)m_strMask.GetLength();
	nStart = min( nStart, nTextLen );
	nEnd = min( nEnd, nTextLen );
	if( nStart != nEnd )
	{
		if( ! m_strMask.IsEmpty() )
		{
			for( i = nStart; i < nEnd; i++ )
			{
				__EXT_MFC_SAFE_TCHAR _tchrSymbol = strText.GetAt( i );
				__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask.GetAt( i );
				if( _tchrSymbol == _tchrMask )
					nMaskCountBetweenStartPosEndPos ++;
			}
		}
		_nEnd = nEnd - nMaskCountBetweenStartPosEndPos - nMaskCountAfterStartPos;
	}
}

void CExtEditWithBehavior::CBehaviorMasked::_MaskSymbolDelete( CExtSafeString & _str )
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
	if( strText.IsEmpty() )
		return;
INT nLenStr = INT(strText.GetLength()), nLenMask = INT(m_strMask.GetLength()), i;
	_str = _T("");
	if( nLenMask < nLenStr )
		return;
	for( i = 0; i < nLenStr; i++ )
	{
		__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask.GetAt( i );
		__EXT_MFC_SAFE_TCHAR _tchrSymbol = strText.GetAt( i );
		if( _tchrSymbol != _tchrMask )
		{
			TCHAR _tstr[2] = { TCHAR(_tchrSymbol), TCHAR(0) };
			_str += _tstr;
		}
	}
}

void CExtEditWithBehavior::CBehaviorMasked::_MaskSymbolInsert( CExtSafeString _str )
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	if( _str.IsEmpty() )
	{
		m_pEditWithBehaviorWnd->TextSet( _str );
		return;
	}
INT nMaskLen = INT( m_strMask.GetLength() );
CExtSafeString strText = _T("");
INT i = 0;
	if( nMaskLen > 0 )
	{
		for( ; ! _str.IsEmpty(); )
		{
			__EXT_MFC_SAFE_TCHAR _tchrMask = m_strMask.GetAt( i );
			__EXT_MFC_SAFE_TCHAR _tchrSymbol = _str.GetAt( 0 );
			if( m_arrRules.FindPos( _tchrMask ) >= 0 )
			{
				TCHAR _tstr[2] = { TCHAR(_tchrSymbol), TCHAR(0) };
				strText += _tstr;
				_str.Delete( 0 );
			}
			else
			{
				TCHAR _tstr[2] = { TCHAR(_tchrMask), TCHAR(0) };
				strText += _tstr;
			}
			i ++;
			if( i >= nMaskLen )
				break;
		}
	}
	m_pEditWithBehaviorWnd->TextSet( strText );
}

void CExtEditWithBehavior::CBehaviorMasked::_SymbolInsert( CExtSafeString _str )
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
bool bNullableMode = NullableModeGet();
bool bInsertMode = InsertModeGet();
int _nStart, _nEnd;
	_DefineNewPosition( _nStart, _nEnd );
CExtSafeString _strNew;
	_MaskSymbolDelete( _strNew );
INT nLenStr = INT(_strNew.GetLength());
__EXT_MFC_SAFE_TCHAR _tchrNullChar = NullCharGet();
INT nPosNullChar = -1;
bool bNullChar = _IsNullChar( nPosNullChar );
bool bDeleteNullCharSymbolAfterPos = false;
	if(		bNullableMode && bInsertMode
		&&	TCHAR(_strNew.GetAt( _nStart )) != TCHAR(_tchrNullChar)
		&&	bNullChar
		&&	_nEnd == _nStart
		)
	{
		INT i;
		for( i = _nStart; i < nLenStr; i++ )
		{
			__EXT_MFC_SAFE_TCHAR _tchrSymbol = _strNew.GetAt( i );
			if( _tchrSymbol == _tchrNullChar )
			{
				_strNew.Delete( i );
				bDeleteNullCharSymbolAfterPos = true;
				break;
			}
		}
		if( ! bDeleteNullCharSymbolAfterPos )
		{
			for( i = _nStart; i >= 0; i-- )
			{
				__EXT_MFC_SAFE_TCHAR _tchrSymbol = _strNew.GetAt( i );
				if( _tchrSymbol == _tchrNullChar )
				{
					_strNew.Delete( i );
					break;
				}
			}
		}
	}
	if( _nStart == _nEnd )
	{
		if( bDeleteNullCharSymbolAfterPos || ! bNullableMode )
			_strNew.Insert( _nStart, _str );
		else
			_strNew.Insert( _nStart - 1, _str );
	}
	else
	{
		_strNew.Delete( _nStart, ( _nEnd - _nStart ) );
		_strNew.Insert( _nStart, _str );
	}
	_MaskSymbolInsert( _strNew );
}

bool CExtEditWithBehavior::CBehaviorMasked::_NullCharBefore( int _nPos )
{
	ASSERT_VALID( m_pEditWithBehaviorWnd );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
INT nLenStr = INT(strText.GetLength()), i;
__EXT_MFC_SAFE_TCHAR _tchrNullChar = NullCharGet();
__EXT_MFC_SAFE_TCHAR _tchrSymbol;
	for( i = _nPos; i < nLenStr; i++ )
	{
		_tchrSymbol = strText.GetAt( i );
		if( _tchrSymbol == _tchrNullChar )
			return true;
	}
	return false;
}

CExtEditWithBehavior::CBehaviorMasked::CharRuleArray & CExtEditWithBehavior::CBehaviorMasked::GetSymbolArray()
{
	return m_arrRules;
}

void CExtEditWithBehavior::CBehaviorMasked::InsertModeSet( bool bInsertMode )
{
	m_bInsertMode = bInsertMode;
}

bool CExtEditWithBehavior::CBehaviorMasked::InsertModeGet() const
{
	return m_bInsertMode;
}

void CExtEditWithBehavior::CBehaviorMasked::NullableModeSet( bool bNullableMode )
{
	m_bNullableMode = bNullableMode;
}

bool CExtEditWithBehavior::CBehaviorMasked::NullableModeGet() const
{
	return m_bNullableMode;
}

void CExtEditWithBehavior::CBehaviorMasked::NullCharSet( __EXT_MFC_SAFE_TCHAR _tchrNullChar )
{
	m_tchrNullChar = _tchrNullChar;
	_Redraw();
}

void CExtEditWithBehavior::CBehaviorMasked::FieldSelectionModeSet( bool bFieldSelectionMode )
{
	m_bFieldSelectionMode = bFieldSelectionMode;
}

bool CExtEditWithBehavior::CBehaviorMasked::FieldSelectionModeGet() const
{
	return m_bFieldSelectionMode;
}

__EXT_MFC_SAFE_TCHAR CExtEditWithBehavior::CBehaviorMasked::NullCharGet() const
{
	return m_tchrNullChar;
}

bool CExtEditWithBehavior::CBehaviorMasked::_IsNullChar( int & _pos ) const
{
//	ASSERT_VALID( m_pEditWithBehaviorWnd );
CExtSafeString strTextInitial = m_pEditWithBehaviorWnd->TextGet();
INT nLength = INT(strTextInitial.GetLength()), i;
TCHAR tchrText;
TCHAR _tchrNullChar = TCHAR( NullCharGet() );
	for( i = 0; i < nLength; i++ )
	{
		tchrText = strTextInitial.GetAt( i );
		if( tchrText == _tchrNullChar )
		{
			_pos = i;
			return true;
		}
	}
	return false;
}

bool CExtEditWithBehavior::CBehaviorMasked::_GetFieldSelection( int _currentPosition )
{
bool bFieldSelectionMode = FieldSelectionModeGet();
	if( m_pEditWithBehaviorWnd->GetSafeHwnd() == NULL )
		return bFieldSelectionMode;
	ASSERT_VALID( m_pEditWithBehaviorWnd );
	if( ! bFieldSelectionMode )
		return false;
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
	if( strText.IsEmpty() )
		return false;
INT nLength = INT( strText.GetLength() );
bool bNullableMode = NullableModeGet();
TCHAR _tchrNullChar = TCHAR( NullCharGet() );
TCHAR _tchrMask = m_strMask.GetAt(_currentPosition);
CExtSafeString strSymbols;
INT nSymbolIndex, nSymbolCount = INT(m_arrRules.GetSize());
	for( nSymbolIndex = 0; nSymbolIndex < nSymbolCount; nSymbolIndex++ )
		strSymbols += TCHAR(__EXT_MFC_SAFE_TCHAR(m_arrRules[nSymbolIndex]));
	if( strSymbols.FindOneOf( & _tchrMask ) < 0 )
		return false;
INT _maskLen = m_strMask.GetLength();
	if( _maskLen == _currentPosition + 1 )
		return false;
	_tchrMask = m_strMask.GetAt(_currentPosition + 1);
	if( strSymbols.FindOneOf( & _tchrMask ) >= 0 )
		return false;
INT _startPosition = -1;
INT _endPosition = -1;
INT i = _currentPosition + 1;
	for( ; i < nLength; i++ )
	{
		_tchrMask = m_strMask.GetAt(i);
		if( strSymbols.FindOneOf( & _tchrMask ) >= 0 )
		{
			_startPosition = i;
			break;
		}
	}
	if( _startPosition < 0 )
		return false;
	for( i = _startPosition; i < nLength; i++ )
	{
		_tchrMask = m_strMask.GetAt(i);
		if( strSymbols.FindOneOf( & _tchrMask ) < 0 )
		{
			_endPosition = i;
			break;
		}
	}
	if( _endPosition < 0 )
		_endPosition = nLength;
	m_pEditWithBehaviorWnd->SetSel( _startPosition, _endPosition );
CExtSafeString strIns;
	for( i = _startPosition; i < _endPosition; i ++ )
		strIns += _tchrNullChar;
	if( ! strIns.IsEmpty() && bNullableMode )
	{
		m_pEditWithBehaviorWnd->ReplaceSel( strIns, TRUE );
		m_pEditWithBehaviorWnd->SetSel( _startPosition, _endPosition );
	}
	return true;
}

void CExtEditWithBehavior::CBehaviorMasked::_ShowNullableMask()
{
	if( m_pEditWithBehaviorWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( m_pEditWithBehaviorWnd );
bool bNullableMode = NullableModeGet();
	if( ! bNullableMode || m_pEditWithBehaviorWnd == NULL )
		return;
INT nMaskLength = INT( m_strMask.GetLength() );
TCHAR _tchrNullChar = TCHAR(NullCharGet());
CExtSafeString strText;
	for( INT i = 0; i < nMaskLength; i ++ )
	{
		TCHAR _tchrMask = m_strMask.GetAt( i );
		INT nSymbolIndex, nSymbolCount = INT(m_arrRules.GetSize());
		for( nSymbolIndex = 0; nSymbolIndex < nSymbolCount; nSymbolIndex++ )
		{
			const CharRule & symbol = m_arrRules[nSymbolIndex];
			if( _tchrMask == TCHAR(__EXT_MFC_SAFE_TCHAR(symbol)) )
				_tchrMask = _tchrNullChar;
		}
		strText += _tchrMask;
	}
	m_pEditWithBehaviorWnd->TextSet( strText );
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditWithBehavior::CBehaviorMasked::CharRule

CExtEditWithBehavior::CBehaviorMasked::CharRule::CharRule()
	: m_cSymbol( __EXT_MFC_SAFE_TCHAR(TCHAR(0)) )
	, m_fnValidation( NULL )
	, m_fnConversion( NULL )
	, m_clrSymbol( COLORREF(-1L) )
{
}	

CExtEditWithBehavior::CBehaviorMasked::CharRule::CharRule( __EXT_MFC_SAFE_TCHAR cSymbol, ValidationFunction fnValidation, ConversionFunction fnConversion /*= NULL*/ )
	: m_cSymbol( cSymbol )
	, m_fnValidation( fnValidation )
	, m_fnConversion( fnConversion )
	, m_clrSymbol( COLORREF(-1L) )
{
}	

CExtEditWithBehavior::CBehaviorMasked::CharRule::CharRule( COLORREF clrSymbol, __EXT_MFC_SAFE_TCHAR cSymbol, ValidationFunction fnValidation, ConversionFunction fnConversion /*= NULL*/ )
	: m_cSymbol( cSymbol )
	, m_fnValidation( fnValidation )
	, m_fnConversion( fnConversion )
	, m_clrSymbol( clrSymbol )
{
}	

CExtEditWithBehavior::CBehaviorMasked::CharRule::~CharRule()
{
}

bool CExtEditWithBehavior::CBehaviorMasked::CharRule::Validate( __EXT_MFC_SAFE_TCHAR _tchr ) const
{
	if( m_fnValidation != NULL )
		return ( m_fnValidation( _tchr ) != 0 ) ? true : false;
	return true;
}

__EXT_MFC_SAFE_TCHAR CExtEditWithBehavior::CBehaviorMasked::CharRule::Convert( __EXT_MFC_SAFE_TCHAR _tchr ) const
{
	if( m_fnConversion != NULL )
		return (TCHAR)m_fnConversion( _tchr );
	return _tchr;
}

void CExtEditWithBehavior::CBehaviorMasked::CharRule::Set(__EXT_MFC_SAFE_TCHAR cSymbol)
{
	m_cSymbol = cSymbol;
}

__EXT_MFC_SAFE_TCHAR CExtEditWithBehavior::CBehaviorMasked::CharRule::Get() const
{
	return m_cSymbol;
}

CExtEditWithBehavior::CBehaviorMasked::CharRule::operator __EXT_MFC_SAFE_TCHAR() const
{
	return m_cSymbol;
}

//void CExtEditWithBehavior::OnChange() 
//{
//}
//
//void CExtEditWithBehavior::OnUpdate() 
//{
//}

void CExtEditWithBehavior::CBehaviorMasked::CharRule::Serialize( CArchive & ar )
{
	if( ar.IsStoring() )
	{
		ar << DWORD(TCHAR(__EXT_MFC_SAFE_TCHAR(m_cSymbol)));
		ar << DWORD(m_clrSymbol);
	}
	else
	{
		DWORD dwTmp;
		ar >> dwTmp;
		m_cSymbol = (__EXT_MFC_SAFE_TCHAR)(TCHAR)(dwTmp);
		ar >> dwTmp;
		m_clrSymbol = (COLORREF)dwTmp;
	}
}

void CExtEditWithBehavior::CBehaviorMasked::CharRuleArray::Serialize( CArchive & ar )
{
DWORD dwIndex, dwCount;
	if( ar.IsStoring() )
	{
		dwCount = DWORD( GetSize() );
		ar << dwCount;
	}
	else
	{
		RemoveAll();
		ar >> dwCount;
		SetSize( INT(dwCount) );
	}
	for( dwIndex = 0; dwIndex < dwCount; dwIndex ++ )
	{
		CharRule & _cr = ElementAt( INT(dwIndex) );
		_cr.Serialize( ar );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditMasked

IMPLEMENT_DYNCREATE( CExtEditMasked, CExtEditWithBehavior );

CExtEditMasked::CExtEditMasked(
	__EXT_MFC_SAFE_LPCTSTR strMask // = _T("")
	)
	: CBehaviorMasked( strMask )
	, m_clrDefault( COLORREF(-1L) )
{
	m_pEditWithBehaviorWnd = this;
}

BEGIN_MESSAGE_MAP( CExtEditMasked, CExtEditWithBehavior )
	//{{AFX_MSG_MAP(CExtEditMasked)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtEditMasked::_Rich_OnDraw()
{
	ASSERT_VALID( this );
	if( ! m_bRichMode )
		return;
	if( GetSafeHwnd() == NULL )
		return;
	if( m_bRichPainting )
		return;
	m_bRichPainting = true;
CExtSafeString strText = TextGet();
CExtSafeString strMask = MaskGet();
INT nTextLen = INT( strText.GetLength() );
INT nMaskLen = INT( strMask.GetLength() );
INT nWalkLen = min( nTextLen, nMaskLen );
	if( nWalkLen > 0 )
	{
		COLORREF clrDefault = m_clrDefault;
		if( clrDefault == COLORREF(-1L) )
		{
			if( IsWindowEnabled() && ( GetStyle() & ES_READONLY ) == 0 )
				clrDefault = ::GetSysColor( COLOR_WINDOWTEXT );
			else
				clrDefault = ::GetSysColor( COLOR_3DSHADOW );
		}
		CHARFORMAT cfDefault;
		::memset( &cfDefault, 0, sizeof(CHARFORMAT) );
		cfDefault.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_STRIKEOUT|CFM_FACE|CFM_SIZE|CFM_CHARSET|CFM_PROTECTED|CFM_COLOR;
		cfDefault.bCharSet = DEFAULT_CHARSET;
		cfDefault.dwEffects = CFE_PROTECTED; 
		CFont * pFont = GetFont();
		if( pFont->GetSafeHandle() == NULL )
		{
			CWnd * pWndParent = GetParent();
			if( pWndParent->GetSafeHwnd() != NULL )
				pFont = pWndParent->GetFont();
			if( pFont->GetSafeHandle() == NULL )
				pFont = &(PmBridge_GetPM()->m_FontNormal);
		}
		LOGFONT _lf; ::memset( &_lf, 0, sizeof(LOGFONT) );
		pFont->GetLogFont( &_lf );
#if (! defined _RICHEDIT_VER )
		USES_CONVERSION;
		strcpy( cfDefault.szFaceName, T2CA(_lf.lfFaceName) );
#else
#if ( _RICHEDIT_VER < 0x0200 )
		USES_CONVERSION;
		strcpy( cfDefault.szFaceName, T2CA(_lf.lfFaceName) );
#else
		__EXT_MFC_STRCPY( cfDefault.szFaceName, sizeof(cfDefault.szFaceName)/sizeof(cfDefault.szFaceName[0]), _lf.lfFaceName );
#endif
#endif
		CWindowDC dcDesktop( NULL );
		cfDefault.yHeight = ::MulDiv( _lf.lfHeight, 72 * 20, dcDesktop.GetDeviceCaps( LOGPIXELSY ) );
		cfDefault.yHeight = abs( cfDefault.yHeight );
		bool bEnabled = IsWindowEnabled() ? true : false, bReadOnly = ( ( GetStyle() & ES_READONLY ) != 0 ) ? true : false;
		cfDefault.crTextColor = bEnabled ? ( bReadOnly ? m_clrRichTextReadOnly : m_clrRichTextNormal ) : m_clrRichTextDisabled;
		if( cfDefault.crTextColor == COLORREF(-1L) )
		{
			if( bEnabled )
			{
				if( bReadOnly )
					cfDefault.crTextColor = ::GetSysColor( COLOR_3DSHADOW );
				else
					cfDefault.crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
			}
			else
				cfDefault.crTextColor = ::GetSysColor( COLOR_GRAYTEXT );
		}
		COLORREF clrBack = OnQueryBackColor();
		if( clrBack == COLORREF(-1L) )
		{
			clrBack = bEnabled ? ( bReadOnly ? m_clrRichBackReadOnly : m_clrRichBackNormal ) : m_clrRichBackDisabled;
			if( clrBack == COLORREF(-1L) )
			{
				if( bEnabled )
				{
					if( bReadOnly )
						clrBack = ::GetSysColor( COLOR_3DFACE );
					else
						clrBack = ::GetSysColor( COLOR_WINDOW );
				}
				else
					clrBack = ::GetSysColor( COLOR_3DFACE );
			}
		}
		Rich_SetBackgroundColor( FALSE, clrBack );
		Rich_HideSelection( true, false );
		CHARRANGE crRestore;
		Rich_GetSel( crRestore );
		Rich_SetSel( 0, -1 ); 
		SendMessage( EM_SETCHARFORMAT, SCF_DEFAULT, LPARAM(&cfDefault) );
		SendMessage( EM_SETCHARFORMAT, SCF_SELECTION, LPARAM(&cfDefault) );
		Rich_SetSelectionColor( cfDefault.crTextColor );
		CharRuleArray & arrCharRules = GetSymbolArray();
		INT nWalkIndex;
		for( nWalkIndex = 0; nWalkIndex < nWalkLen; nWalkIndex ++ )
		{
			TCHAR tchrMask = strMask[ nWalkIndex ];
			INT nRuleNo = arrCharRules.FindPos( tchrMask );
			if( nRuleNo < 0 )
				continue;
			CharRule & theCharRule = arrCharRules.ElementAt( nRuleNo );
			if( theCharRule.m_clrSymbol == COLORREF(-1L) )
				continue;
			Rich_SetSel( nWalkIndex, ( nWalkIndex + 1 ) );
			Rich_SetSelectionColor( theCharRule.m_clrSymbol );
		}
		Rich_SetSel( crRestore );
		Rich_HideSelection( false, false );
	} // if( nWalkLen > 0 )
	m_bRichPainting = false;
}

CExtSafeString CExtEditMasked::TextGetValid() const
{
	ASSERT_VALID( this );
	return Behavior_GetValidText();
}

#ifndef _UNICODE
int CExtEditMasked::stat_IsCharDigit( UINT c ) { return _istdigit( (TCHAR)c ); } // similar to _istdigit()
int CExtEditMasked::stat_IsCharAlpha( UINT c ) { return IsCharAlpha( (TCHAR)c ); } // similar to _istalpha()
int CExtEditMasked::stat_IsCharAlphaOrDigit( UINT c ) { return IsCharAlphaNumeric( (TCHAR)c ); } // similar to _istalnum()
#else
int CExtEditMasked::stat_IsCharDigit( __EXT_MFC_SAFE_TCHAR c ) { return _istdigit( (TCHAR)c ); } // similar to _istdigit()
int CExtEditMasked::stat_IsCharAlpha( __EXT_MFC_SAFE_TCHAR c ) { return IsCharAlpha( (TCHAR)c ); } // similar to _istalpha()
int CExtEditMasked::stat_IsCharAlphaOrDigit( __EXT_MFC_SAFE_TCHAR c ) { return IsCharAlphaNumeric( (TCHAR)c ); } // similar to _istalnum()
#endif

void CExtEditMasked::OnChar( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	Behavior_OnChar( uChar, nRepCnt, nFlags );
//	_Rich_OnDraw();
}

void CExtEditMasked::OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	Behavior_OnKeyDown( uChar, nRepCnt, nFlags );
}

void CExtEditMasked::OnKillFocus( CWnd * pNewWnd )
{
	ASSERT_VALID( this );
	Behavior_OnKillFocus( pNewWnd );
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditSystemNumberCurrencyBase window

IMPLEMENT_DYNAMIC( CExtEditSystemNumberCurrencyBase, CExtEditWithBehavior );

CExtEditSystemNumberCurrencyBase::CExtEditSystemNumberCurrencyBase(
	INT nMaxWholeDigits, // = __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS
	INT nMaxFractionDigits // = __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS
	)
	: m_nMaxWholeDigits( nMaxWholeDigits >= 0 ? nMaxWholeDigits : (-nMaxWholeDigits) )
	, m_nMaxFractionDigits( nMaxFractionDigits )
	, m_nNegSignMatch( 0 )
	, m_strNegativeSign( _T("-") )
	, m_strDecimalPoint( _T('.') )
	, m_strGroupSeparator( _T(',') )
	, m_nNegativeFormatIndex( 0 )
	, m_nPositiveFormatIndex( 0 )
	, m_lfMin( __EXT_MFC_SYSTEM_NUMBER_MIN )
	, m_lfMax( __EXT_MFC_SYSTEM_NUMBER_MAX )
	, m_bAdjustingSeparators( false )
	, m_bBackspace( false )
	, m_nStartSel( -1 )
	, m_nEndSel( -1 )
	, m_clrNumber( COLORREF(-1L) )
	, m_clrFraction( COLORREF(-1L) )
	, m_clrGroupSeparator( COLORREF(-1L) )
	, m_clrDecimalSeparator( COLORREF(-1L) )
	, m_clrSign( COLORREF(-1L) )
	, m_clrBrackets( COLORREF(-1L) )
	, m_strCurrencySymbol( _T("$") )
	, m_bEmptyGroupSeparatorMode( false )
	, m_bUseScientificNumberMode( false )
	, m_strScientificSymbol( _T("e") )
{
	ASSERT( m_nMaxWholeDigits > 0 ); // must have at least 1 digit to the left of the decimal
	ASSERT( m_nMaxFractionDigits >= 0 ); // decimal places must be positive
	m_pEditWithBehaviorWnd = this;
	if( nMaxWholeDigits < 0 )
		CBehaviorBase::m_nBehaviorFlags |= __ESE_DISABLE_NEGATIVE;
}

CExtEditSystemNumberCurrencyBase::~CExtEditSystemNumberCurrencyBase()
{
}

BEGIN_MESSAGE_MAP( CExtEditSystemNumberCurrencyBase, CExtEditWithBehavior )
	//{{AFX_MSG_MAP(CExtEditWithBehavior)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_MESSAGE( WM_PASTE, OnPaste )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtEditSystemNumberCurrencyBase::CurrencySet( const CURRENCY & _cy )
{
	ASSERT_VALID( this );
long double lfVal = ((long double)_cy.int64) / ((long double)10000.0);
	LongDoubleSet( lfVal );
}

COleCurrency CExtEditSystemNumberCurrencyBase::CurrencyGet() const
{
	ASSERT_VALID( this );
long double lfVal = LongDoubleGet() * ((long double)10000.0);
COleCurrency _oleCurrency;
	_oleCurrency.m_cur.int64 = (LONGLONG)lfVal;
	return _oleCurrency;
}

void CExtEditSystemNumberCurrencyBase::LongDoubleSet( long double lfValue, bool bTrimTrailingZeros /*= true*/ )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	m_pEditWithBehaviorWnd->SetWindowText( _GetDoubleText( lfValue, bTrimTrailingZeros ) );
}

void CExtEditSystemNumberCurrencyBase::MaxWholeDigitsSet( INT nMaxWholeDigits )
{
	ASSERT_VALID( this );
	ASSERT( nMaxWholeDigits );
bool bAllowNegative = (nMaxWholeDigits >= 0);
	if( nMaxWholeDigits < 0 )
		nMaxWholeDigits = -nMaxWholeDigits;
	if( m_nMaxWholeDigits == nMaxWholeDigits && AllowNegativeGet() == bAllowNegative )
		return;
	m_nMaxWholeDigits = nMaxWholeDigits;
	if( ! _FlagsModify( bAllowNegative ? 0 : __ESE_DISABLE_NEGATIVE, bAllowNegative ? __ESE_DISABLE_NEGATIVE : 0 ) )
		_Redraw();
}

INT CExtEditSystemNumberCurrencyBase::MaxWholeDigitsGet() const
{
	ASSERT_VALID( this );
	return m_nMaxWholeDigits;
}

void CExtEditSystemNumberCurrencyBase::MaxFractionDigitsSet( INT nMaxFractionDigits )
{
	ASSERT_VALID( this );
	ASSERT( nMaxFractionDigits >= 0 );
	if( m_nMaxFractionDigits == nMaxFractionDigits )
		return;
	m_nMaxFractionDigits = nMaxFractionDigits;
	_Redraw();
}

INT CExtEditSystemNumberCurrencyBase::MaxFractionDigitsGet() const
{
	ASSERT_VALID( this );
	return m_nMaxFractionDigits;
}

void CExtEditSystemNumberCurrencyBase::AllowNegativeSet( bool bAllowNegative /*= true*/ )
{
	ASSERT_VALID( this );
	_FlagsModify( bAllowNegative ? 0 : __ESE_DISABLE_NEGATIVE, bAllowNegative ? __ESE_DISABLE_NEGATIVE : 0 );
}

bool CExtEditSystemNumberCurrencyBase::AllowNegativeGet() const
{
	ASSERT_VALID( this );
bool bAllowNegative = ( ( CBehaviorBase::m_nBehaviorFlags & __ESE_DISABLE_NEGATIVE ) != 0 ) ? false : true;
	return bAllowNegative;
}

void CExtEditSystemNumberCurrencyBase::RangeSet( long double lfMin, long double lfMax )
{
	ASSERT_VALID( this );
	ASSERT( lfMin <= lfMax );
	m_lfMin = lfMin;
	m_lfMax = lfMax;
	_AdjustWithinRange();
	_Redraw();
}

void CExtEditSystemNumberCurrencyBase::RangeGet( long double * p_lfMin, long double * p_lfMax ) const
{
	ASSERT_VALID( this );
	if( p_lfMin != NULL )
		(*p_lfMin) = m_lfMin;
	if( p_lfMax != NULL )
		(*p_lfMax) = m_lfMax;
}

void CExtEditSystemNumberCurrencyBase::NumberColorSet( COLORREF clrNumber /* = COLORREF(-1L) */ )
{
	ASSERT_VALID( this );
	if( m_clrNumber == clrNumber )
		return;
	m_clrNumber = clrNumber;
	_Rich_OnDraw();
}

COLORREF CExtEditSystemNumberCurrencyBase::NumberColorGet() const
{
	ASSERT_VALID( this );
	return m_clrNumber;
}

void CExtEditSystemNumberCurrencyBase::FractionColorSet( COLORREF clrFraction /* = COLORREF(-1L) */ )
{
	ASSERT_VALID( this );
	if( m_clrFraction == clrFraction )
		return;
	m_clrFraction = clrFraction;
	_Rich_OnDraw();
}

COLORREF CExtEditSystemNumberCurrencyBase::FractionColorGet() const
{
	ASSERT_VALID( this );
	return m_clrFraction;
}

void CExtEditSystemNumberCurrencyBase::GroupSeparatorColorSet( COLORREF clrGroupSeparator /* = COLORREF(-1L) */ )
{
	ASSERT_VALID( this );
	if( m_clrGroupSeparator == clrGroupSeparator )
		return;
	m_clrGroupSeparator = clrGroupSeparator;
	_Rich_OnDraw();
}

COLORREF CExtEditSystemNumberCurrencyBase::GroupSeparatorColorGet() const
{
	ASSERT_VALID( this );
	return m_clrGroupSeparator;
}

void CExtEditSystemNumberCurrencyBase::DecimalSeparatorColorSet( COLORREF clrDecimalSeparator /* = COLORREF(-1L) */ )
{
	ASSERT_VALID( this );
	if( m_clrDecimalSeparator == clrDecimalSeparator )
		return;
	m_clrDecimalSeparator = clrDecimalSeparator;
	_Rich_OnDraw();
}

COLORREF CExtEditSystemNumberCurrencyBase::DecimalSeparatorColorGet() const
{
	ASSERT_VALID( this );
	return m_clrDecimalSeparator;
}

void CExtEditSystemNumberCurrencyBase::SignColorSet( COLORREF clrSign /* = COLORREF(-1L) */ )
{
	ASSERT_VALID( this );
	if( m_clrSign == clrSign )
		return;
	m_clrSign = clrSign;
	_Rich_OnDraw();
}

COLORREF CExtEditSystemNumberCurrencyBase::SignColorGet() const
{
	ASSERT_VALID( this );
	return m_clrSign;
}

void CExtEditSystemNumberCurrencyBase::BracketsColorSet( COLORREF clrBrackets /* = COLORREF(-1L) */ )
{
	ASSERT_VALID( this );
	if( m_clrBrackets == clrBrackets )
		return;
	m_clrBrackets = clrBrackets;
	_Rich_OnDraw();
}

COLORREF CExtEditSystemNumberCurrencyBase::BracketsColorGet() const
{
	ASSERT_VALID( this );
	return m_clrBrackets;
}

bool CExtEditSystemNumberCurrencyBase::_IsValid() const
{
	ASSERT_VALID( this );
long double lfValue = LongDoubleGet();
bool bRetVal = ( m_lfMin <= lfValue && lfValue <= m_lfMax ) ? true : false;
	return bRetVal;
}

INT CExtEditSystemNumberCurrencyBase::_GetGroupSeparatorCount( const CExtSafeString & strText ) const
{
	ASSERT_VALID( this );
	if( m_bEmptyGroupSeparatorMode || m_strGroupSeparator.IsEmpty() )
		return 0;
INT nPos = 0, nSepCount = 0;
	for( ; true; )
	{
		nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
		if( nPos == -1 )
			break;
		nSepCount ++;
		nPos ++;
	}
	return nSepCount;
}

void CExtEditSystemNumberCurrencyBase::_AdjustSeparators( 
	INT nCurrentSeparatorCount, 
	bool bDecimalPoint // = false
	)
{
	ASSERT_VALID( this );
	nCurrentSeparatorCount; 
	bDecimalPoint;
	m_bAdjustingSeparators = true;
CExtSafeString strText = Behavior_GetValidText( true );
	m_bAdjustingSeparators = false;
}

void CExtEditSystemNumberCurrencyBase::stat_InsertZeros( CExtSafeString * pStrText, INT nPos, INT nCount )
{
	ASSERT( pStrText != NULL );
	if( nPos < 0 && nCount > 0 )
		nPos = INT( pStrText->GetLength() );
	for( INT iZero = 0; iZero < nCount; iZero++ )
		pStrText->Insert( nPos, _T('0') );
}

CExtSafeString CExtEditSystemNumberCurrencyBase::TextGetValid() const
{
	ASSERT_VALID( this );
	return Behavior_GetValidText();
}

void CExtEditSystemNumberCurrencyBase::OnChar( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	Behavior_OnChar( uChar, nRepCnt, nFlags );
//	_Rich_OnDraw();
}

void CExtEditSystemNumberCurrencyBase::OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	Behavior_OnKeyDown( uChar, nRepCnt, nFlags );
}

void CExtEditSystemNumberCurrencyBase::OnKillFocus( CWnd * pNewWnd )
{
	ASSERT_VALID( this );
	Behavior_OnKillFocus( pNewWnd );
}

LRESULT CExtEditSystemNumberCurrencyBase::OnPaste( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	if( ( GetStyle() & ES_READONLY ) != 0 )
		return 0;
LRESULT lResult = Behavior_OnPaste( wParam, lParam );
	return lResult;
}

void CExtEditSystemNumberCurrencyBase::_CheckFlags()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	// test whether value is empty
CExtSafeString strOriginalText = _GetNumericText( m_pEditWithBehaviorWnd->TextGet() );
CExtSafeString strText = strOriginalText;
INT nLen = INT( strText.GetLength() );
	// optionally remove any extra leading zeros but always leave one in front of the decimal point
	if( ( CBehaviorBase::m_nBehaviorFlags & __ESE_KF_REMOVE_EXTRA_LEADING_ZEROS ) != 0 && nLen > 0 )
	{
		bool bIsNegative = _IsNegative( strText );
		INT _nStart = 0, _nEnd = 0;
		_GetTextWithoutMarks( strText, _nStart, _nEnd );
		strText.TrimLeft( _T('0') );
		INT nDecimalSignPos = (INT)strText.Find( m_strDecimalPoint );
		if( strText.IsEmpty() || nDecimalSignPos == 0 )
			strText.Insert( 0, _T('0') );
			_GetTextWithMarks( strText, _nStart, _nEnd, bIsNegative );
	}
	else if( nLen == 0 && ( CBehaviorBase::m_nBehaviorFlags & __ESE_KF_DONT_PAD_WITH_ZEROS_IF_EMPTY ) != 0 )
			return;
	// test whether we need to pad the number with zeros after decimal point
	if( ( CBehaviorBase::m_nBehaviorFlags & __ESE_KF_PAD_WITH_ZEROS_AFTER_DECIMAL ) != 0 && m_nMaxFractionDigits > 0 )
	{
		bool bIsNegative = _IsNegative( strText );
		INT _nStart = 0, _nEnd = 0;
		_GetTextWithoutMarks( strText, _nStart, _nEnd );
		INT nDecimalSignPos = (INT)strText.Find( m_strDecimalPoint );
		INT nLen = INT( strText.GetLength() );
		if( nDecimalSignPos < 0 )
		{
			if( nLen == 0 )
			{
				strText = _T('0');
				nLen = (INT)strText.GetLength();
			}
			strText += m_strDecimalPoint;
			nDecimalSignPos = (INT)strText.Find( m_strDecimalPoint );
		}
		INT _nRightPartLen = (INT)strText.Mid( nDecimalSignPos + (INT)m_strDecimalPoint.GetLength() ).GetLength();
		if( _nRightPartLen > m_nMaxFractionDigits )
			strText.Delete( nDecimalSignPos + m_nMaxFractionDigits + m_strDecimalPoint.GetLength() );
		stat_InsertZeros( &strText, -1, m_nMaxFractionDigits - _nRightPartLen );
		_GetTextWithoutMarks( strText, _nStart, _nEnd );
		_GetTextWithMarks( strText, _nStart, _nEnd, bIsNegative );
	}
	// test whether we need to pad the number with zeros before the decimal point
	if( ( CBehaviorBase::m_nBehaviorFlags & __ESE_KF_PAD_WITH_ZEROS_BEFORE_DECIMAL ) != 0 && m_nMaxWholeDigits > 0 )
	{
		bool bIsNegative = _IsNegative( strText );
		INT _nStart = 0, _nEnd = 0;
		_GetTextWithoutMarks( strText, _nStart, _nEnd );
		INT nDecimalSignPos = (INT)strText.Find( m_strDecimalPoint );
		INT nLen = INT( strText.GetLength() );
		INT nZeroCount = 0, nZeroPos = -1;
		if( nDecimalSignPos < 0 )
		{
			nZeroCount = m_nMaxWholeDigits - nLen;
			nZeroPos = nLen;
		}
		else
		{
			nZeroCount = m_nMaxWholeDigits - nDecimalSignPos;
			nZeroPos = nDecimalSignPos;
		}
		stat_InsertZeros( &strText, nZeroPos, nZeroCount );
		_GetTextWithoutMarks( strText, _nStart, _nEnd );
		_GetTextWithMarks( strText, _nStart, _nEnd, bIsNegative );
	}
	if( strText != strOriginalText )
		m_pEditWithBehaviorWnd->SetWindowText( strText );
}

void CExtEditSystemNumberCurrencyBase::Behavior_OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags ) 
{
	ASSERT_VALID( this );
	switch( uChar )
	{
	case VK_DELETE:
	{
		if( ( GetStyle() & ES_READONLY ) != 0 )
			return;
		INT nStart = 0, nEnd = 0;
		m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
		CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
		if( _DeleteSymbols( strText, nStart, nEnd ) )
		{
			m_pEditWithBehaviorWnd->SetWindowText( strText );
			m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
		}
		else
			m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
//		_Rich_OnDraw();
		return;
	}
	case VK_BACK:
	{
		if( ( GetStyle() & ES_READONLY ) != 0 )
			return;
		CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
		if( strText.IsEmpty() )
			return;
		INT nStart = 0, nEnd = 0;
		m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
		if( _DeleteSymbols( strText, nStart, nEnd, true ) )
			m_pEditWithBehaviorWnd->SetWindowText( strText );
		m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
//		_Rich_OnDraw();
		return;
	}
	}
	//CExtEditWithBehavior::CBehaviorBase::Behavior_OnKeyDown( uChar, nRepCnt, nFlags );
	CBehaviorBase::Behavior_OnKeyDown( uChar, nRepCnt, nFlags );
}

void CExtEditSystemNumberCurrencyBase::Behavior_OnKillFocus( CWnd * pNewWnd )
{
	ASSERT_VALID( this );
	//CExtEditWithBehavior::CBehaviorBase::Behavior_OnKillFocus( pNewWnd );
	CBehaviorBase::Behavior_OnKillFocus( pNewWnd );
	_CheckFlags();
}

LRESULT CExtEditSystemNumberCurrencyBase::Behavior_OnPaste( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
	if( ( GetStyle() & ES_READONLY ) != 0 )
		return 0;
INT nStart = 0, nEnd = 0;
	m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
	// paste from clipboard
	if( OpenClipboard() )
	{
		CExtSafeString strClipboardText;
		bool bHaveClipboardText = false;
		if( g_PaintManager.m_bIsWinNT4orLater && ::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		{
			HGLOBAL h = ::GetClipboardData( CF_UNICODETEXT );
			if( h != NULL )
			{
				LPWSTR strUnicodeBuffer = (LPWSTR) ::GlobalLock( h );
				if( strUnicodeBuffer != NULL )
				{
					bHaveClipboardText = true;
					USES_CONVERSION;
					LPCTSTR strBuffer = W2CT(strUnicodeBuffer);
					strClipboardText = strBuffer;
					::GlobalUnlock( h );
				}
			}
		} // if( g_PaintManager.m_bIsWinNT4orLater && ::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		if( ( ! bHaveClipboardText ) && ::IsClipboardFormatAvailable( CF_TEXT ) )
		{
			HGLOBAL h = ::GetClipboardData( CF_TEXT );
			if( h != NULL )
			{
				LPSTR strBuffer = (LPSTR) ::GlobalLock( h );
				if( strBuffer != NULL )
				{
					bHaveClipboardText = true;
					strClipboardText = LPCTSTR(strBuffer);
					::GlobalUnlock( h );
				} // if( strBuffer != NULL )
			}
		} // if( ( ! bHaveClipboardText ) && ::IsClipboardFormatAvailable( CF_TEXT ) )
		if( bHaveClipboardText )
		{
			bool bNegative = _IsNegative( strText );
			_GetTextWithoutMarks( strText, nStart, nEnd );
			INT _nStart = 0, _nEnd = 0;
			INT nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
			INT nDecimalPointLen = INT( m_strDecimalPoint.GetLength() );
			if( nDecimalPointPos != -1 )
				_GetTextWithoutMarks( strClipboardText, _nStart, _nEnd, true );
			else
				_GetTextWithoutMarks( strClipboardText, _nStart, _nEnd );
			if( strText.IsEmpty() )
				_GetTextWithoutMarks( strClipboardText, _nStart, _nEnd, true );
			INT nClipboardTextLen = INT( strClipboardText.GetLength() );
			if( ( nEnd - nStart ) > 0 )
				strText.Delete( nStart, ( nEnd - nStart ) );
			nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
			bool bStartInFrontOfDecimalPoint = ( nStart <= nDecimalPointPos );
			strText.Insert( nStart, strClipboardText );
			nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
			if( nDecimalPointPos > m_nMaxWholeDigits )
			{
				strText.Delete( nDecimalPointPos, nDecimalPointLen );
				strText.Insert( m_nMaxWholeDigits, m_strDecimalPoint );
				nDecimalPointPos = m_nMaxWholeDigits;
			}
			if( nDecimalPointPos != -1 )
				nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
			INT nMaxLen = m_nMaxWholeDigits + m_nMaxFractionDigits + ( ( nDecimalPointPos != -1 ) ? nDecimalPointLen : 0 );
			nEnd = nStart + nClipboardTextLen;
			if(		bStartInFrontOfDecimalPoint 
				&&	nEnd > nDecimalPointPos
				)
				nEnd += nDecimalPointLen;
			if( nMaxLen < strText.GetLength() )
			{
				strText.Delete( nMaxLen, ( strText.GetLength() - nMaxLen ) );
				if( nDecimalPointPos != -1 )
				{
					nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
					// check if DecimalPoint deleted
					if( nDecimalPointPos == -1 )
					{
						nMaxLen -= nDecimalPointLen;
						strText.Delete( nMaxLen, nDecimalPointLen );
					}
				}
				if( nDecimalPointPos != -1 && nDecimalPointPos > m_nMaxWholeDigits )
				{
					strText.Delete( nDecimalPointPos, nDecimalPointLen );
					strText.Insert( m_nMaxWholeDigits, m_strDecimalPoint );
					if( nEnd >= m_nMaxWholeDigits )
						nEnd += nDecimalPointLen;
				}
			}
			_GetTextWithMarks( strText, nStart, nEnd, bNegative );
			m_pEditWithBehaviorWnd->TextSet( strText );
			m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
		} // if( bHaveClipboardText )
		::CloseClipboard();
	} // if( OpenClipboard() )
//	_Rich_OnDraw();
	return 0;
}

BOOL CExtEditSystemNumberCurrencyBase::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	ASSERT_VALID( this );
	if( message == WM_COMMAND )
	{
		INT nCode = HIWORD(wParam);
		switch( nCode )
		{
		case EN_CHANGE:
			_AdjustWithinRange();
		break;
		}
	}
	return CExtEditWithBehavior::OnChildNotify(message, wParam, lParam, pLResult);
}

LRESULT CExtEditSystemNumberCurrencyBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( m_bRichMode && message == WM_KEYDOWN )
	{
		switch( INT(wParam) )
		{
		case VK_TAB:
			{
				HWND hWndParent = GetParent()->GetSafeHwnd();
				if( hWndParent != NULL )
				{
					if( ( (::__EXT_MFC_GetWindowLong(hWndParent,GWL_STYLE)) & WS_POPUP ) == 0 )
					{
						hWndParent = ::GetParent(hWndParent);
						if( hWndParent != NULL )
						{
							bool bShift = CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT ) ? true : false;
							::SendMessage( hWndParent, WM_NEXTDLGCTL, bShift ? 1 : 0, 0 );
							Invalidate();
						}
					}
				}
				return 0L;
			}
		break;
		}
	}
	return CExtEditWithBehavior::WindowProc( message, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditSystemNumber window

IMPLEMENT_DYNCREATE( CExtEditSystemNumber, CExtEditSystemNumberCurrencyBase );

CExtEditSystemNumber::CExtEditSystemNumber(
	INT nMaxWholeDigits, // = __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS,
	INT nMaxFractionDigits, // = __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS,
	bool bUseScientificNumberMode // = false
	)
	: CExtEditSystemNumberCurrencyBase( nMaxWholeDigits, nMaxFractionDigits )
	, m_clrPower( COLORREF(-1L) )
	, m_clrScientificSign( COLORREF(-1L) )
{
	m_bUseScientificNumberMode = bUseScientificNumberMode;
	_Init();
}

CExtEditSystemNumber::~CExtEditSystemNumber()
{
}

BEGIN_MESSAGE_MAP( CExtEditSystemNumber, CExtEditSystemNumberCurrencyBase )
	//{{AFX_MSG_MAP(CExtEditSystemNumberCurrencyBase)
	//}}AFX_MSG_MAP
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

void CExtEditSystemNumber::_Init()
{
	ASSERT_VALID( this );
	// negative sign
	if( m_bUseScientificNumberMode )
		m_strNegativeSign = _T("-");
	else if( g_ResourceManager->GetLocaleInfo( LOCALE_SNEGATIVESIGN, NULL, 0 ) )
	{
		g_ResourceManager->GetLocaleInfo( LOCALE_SNEGATIVESIGN, m_strNegativeSign.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
		m_strNegativeSign.ReleaseBuffer();
	}
	// decimal point
	if( g_ResourceManager->GetLocaleInfo( LOCALE_SDECIMAL, NULL, 0 ) )
		g_ResourceManager->GetLocaleInfo( LOCALE_SDECIMAL, m_strDecimalPoint.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
	m_strDecimalPoint.ReleaseBuffer();
	// group separator
	if( m_bEmptyGroupSeparatorMode )
		m_strGroupSeparator.Empty();
	else
	{
		if( g_ResourceManager->GetLocaleInfo( LOCALE_STHOUSAND, NULL, 0 ) )
			g_ResourceManager->GetLocaleInfo( LOCALE_STHOUSAND, m_strGroupSeparator.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
		m_strGroupSeparator.ReleaseBuffer();
	}
	if( m_bUseScientificNumberMode )
		m_nNegativeFormatIndex = 1;
	else
	{
		TCHAR cNegNumFormatIndex = _T('0');
		if( g_ResourceManager->GetLocaleInfo( LOCALE_INEGNUMBER, NULL, 0 ) )
			g_ResourceManager->GetLocaleInfo( LOCALE_INEGNUMBER, &cNegNumFormatIndex, sizeof( cNegNumFormatIndex ) / sizeof(TCHAR) );
		m_nNegativeFormatIndex = _ttoi( & cNegNumFormatIndex );
	}
CExtSafeString strDigitGrouping = _T("0");
	if( g_ResourceManager->GetLocaleInfo( LOCALE_SGROUPING, NULL, 0 ) )
	{
		g_ResourceManager->GetLocaleInfo( LOCALE_SGROUPING, strDigitGrouping.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
		strDigitGrouping.ReleaseBuffer();
		INT nStrLen = INT( strDigitGrouping.GetLength() );
		CExtSafeString _strGroup = _T("");
		INT nGroup = 0;
		if( m_listDigitGrouping.GetCount() > 0 )
			m_listDigitGrouping.RemoveAll();
		for( INT i = 0; i < nStrLen; i++ )
		{
			TCHAR ch = strDigitGrouping[i];
			if( ch == _T(';') )
			{
				nGroup = _ttoi( _strGroup );
				m_listDigitGrouping.AddTail( nGroup );
				_strGroup = _T("");
				nGroup = 0;
			}
			else
				_strGroup = _strGroup + ch;
		}
		nGroup = _ttoi( _strGroup );
		m_listDigitGrouping.AddTail( nGroup );
	}
TCHAR cNoOfDigitsAfterDecimal = _T('0');
	if( g_ResourceManager->GetLocaleInfo( LOCALE_IDIGITS, NULL, 0 ) )
		g_ResourceManager->GetLocaleInfo( LOCALE_IDIGITS, &cNoOfDigitsAfterDecimal, sizeof( cNoOfDigitsAfterDecimal ) / sizeof(TCHAR) );
	m_nMaxFractionDigits = _ttoi( & cNoOfDigitsAfterDecimal );
	TCHAR cDisplayLeadingZeros = _T('0');
	if( g_ResourceManager->GetLocaleInfo( LOCALE_ILZERO, NULL, 0 ) )
		g_ResourceManager->GetLocaleInfo( LOCALE_ILZERO, &cDisplayLeadingZeros, sizeof( cDisplayLeadingZeros ) / sizeof(TCHAR) );
	m_nDisplayLeadingZeros = _ttoi( & cDisplayLeadingZeros );
TCHAR cDigitGrouping[20] = _T("");
	if( g_ResourceManager->GetLocaleInfo( LOCALE_SGROUPING, NULL, 0 ) )
		g_ResourceManager->GetLocaleInfo( LOCALE_SGROUPING, cDigitGrouping, sizeof( cDigitGrouping ) / sizeof( cDigitGrouping[0] ) );
	// ensure the separators are not the same
	if(		(! m_bEmptyGroupSeparatorMode )
		&&	(! m_strGroupSeparator.IsEmpty() )
		&&	m_strDecimalPoint == m_strGroupSeparator
		&&	(	m_strDecimalPoint == _T(".")
			||	m_strDecimalPoint == _T(",")
			)
		)
		m_strGroupSeparator = ( m_strDecimalPoint == _T(",") ? _T(".") : _T(",") );
	if(		(! m_bEmptyGroupSeparatorMode )
		&&	(! m_strGroupSeparator.IsEmpty() )
		&&	m_strDecimalPoint == m_strGroupSeparator
		&&	(	m_strDecimalPoint != _T(".")
			&&	m_strDecimalPoint != _T(",")
			)
		)
		m_strDecimalPoint == _T(".");
}

void CExtEditSystemNumber::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	ASSERT_VALID( this );
	CExtEditSystemNumberCurrencyBase::OnSettingChange( uFlags, lpszSection );
	if( LPCTSTR(lpszSection) != NULL && _tcscmp( LPCTSTR(lpszSection), _T("intl") ) == 0 )
	{
		long double lfSaved = LongDoubleGet();
		_Init();
		LongDoubleSet( lfSaved );
		SetSel( -1, -1 );
	}
}

CExtSafeString CExtEditSystemNumber::_GetNumericText(
	 const CExtSafeString & strText,
	 bool bUseMathSymbols // = false
	 ) const
{
	CString strRetVal = _T("");
	CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
	INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );
	if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
		strRetVal = _GetNumericTextBase( strText, bUseMathSymbols );
	else
	{
		CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		CExtSafeString _strFirstPart, _strSecondPart;
		_strFirstPart = _GetNumericTextBase( strFirstPart, bUseMathSymbols );
		_strSecondPart = _GetNumericTextBase( strSecondPart, bUseMathSymbols );
		strRetVal = _strFirstPart + _T("e") + _strSecondPart;
	}
	return strRetVal;
}

CExtSafeString CExtEditSystemNumber::_GetNumericTextBase(
	const CExtSafeString & strText,
	bool bUseMathSymbols // = false
	) const
{
	ASSERT_VALID( this );
CExtSafeString strNewText;
bool bIsNegative = _IsNegative(strText); // false;
bool bHasDecimalPoint = false;
INT nDecimalSignLen = INT( m_strDecimalPoint.GetLength() );
INT nDecimalSignPos = INT( strText.Find( m_strDecimalPoint ) );
INT nPos, nLen;
	for( nPos = 0, nLen = INT( strText.GetLength() ); nPos < nLen; nPos++ )
	{
		TCHAR c = strText[nPos];
		if( _istdigit(c) )
			strNewText += c;
		else if( nPos == nDecimalSignPos && !bHasDecimalPoint )
		{
			bHasDecimalPoint = true;
			strNewText += ( bUseMathSymbols ? _T(".") : m_strDecimalPoint );
			nPos += nDecimalSignLen - 1;
		}
	}
	// add negative sign to the front of the number
	if( bIsNegative )
	{
		CExtSafeString strNegativeSign;
		if( m_nNegativeFormatIndex == 1 || m_nNegativeFormatIndex == 3 )
			strNegativeSign = m_strNegativeSign;
		if( m_nNegativeFormatIndex == 2 )
			strNegativeSign = m_strNegativeSign + _T(" ");
		if( m_nNegativeFormatIndex == 4 )
			strNegativeSign = _T(" ") + m_strNegativeSign;

		if( bUseMathSymbols || m_bUseScientificNumberMode )
		{
			strNewText.Insert( 0, _T('-') );
		}
		else
		{
			INT nIndex = ( m_nNegativeFormatIndex == 1 || m_nNegativeFormatIndex == 2 ) ? 0 : strNewText.GetLength();

			if( m_nNegativeFormatIndex != 0 )
				strNewText.Insert( nIndex, bUseMathSymbols ? _T("-") : strNegativeSign );
			else
			{
				strNewText.Insert( strNewText.GetLength(), _T(')') );
				strNewText.Insert( 0, _T('(') );
			}
		}
	}
	return strNewText;
}

CExtSafeString CExtEditSystemNumber::_GetDoubleText( long double lfValue, bool bTrimTrailingZeros /*= true*/ ) const
{
	ASSERT_VALID( this );
CExtSafeString strText;
	strText.Format( _T("%lf"), lfValue );
	if( m_strDecimalPoint != _T(".") )
		strText.Replace( _T("."), m_strDecimalPoint );
	if( bTrimTrailingZeros )
	{
		strText.TrimRight( _T('0') );
		strText.TrimRight( m_strDecimalPoint );
	}
bool bNegative = ( strText.Find( _T("-") ) == 0 ) ? true : false;
	if( bNegative )
	{
		strText.Delete( 0 );
		CExtSafeString _strBefore = _T("");
		CExtSafeString _strAfter = _T("");
		switch( m_nNegativeFormatIndex )
		{
		case 0:
			_strBefore = _T("(");
			_strAfter = _T(")");
		break;
		case 1:
			_strBefore = m_strNegativeSign;
		break;
		case 2:
			_strBefore = m_strNegativeSign + _T(" ");
		break;
		case 3:
			_strAfter = m_strNegativeSign;
		break;
		case 4:
			_strAfter = _T(" ") + m_strNegativeSign;
		break;
		}
		INT nLen = INT( strText.GetLength() );
		if( ! _strAfter.IsEmpty() )
			strText.Insert( nLen, _strAfter );
		if( ! _strBefore.IsEmpty() )
			strText.Insert( 0, _strBefore );
	}
	return strText;
}

void CExtEditSystemNumber::IntSet( INT nVal )
{
	ASSERT_VALID( this );
CExtSafeString strText;
	strText.Format( _T("%d"), nVal );
bool bNegative = ( strText.Find( _T("-") ) == 0 ) ? true : false;
	if( bNegative )
	{
		strText.Delete( 0 );
		CExtSafeString _strBefore = _T("");
		CExtSafeString _strAfter = _T("");
		switch( m_nNegativeFormatIndex )
		{
		case 0:
			_strBefore = _T("(");
			_strAfter = _T(")");
		break;
		case 1:
			_strBefore = m_strNegativeSign;
		break;
		case 2:
			_strBefore = m_strNegativeSign + _T(" ");
		break;
		case 3:
			_strAfter = m_strNegativeSign;
		break;
		case 4:
			_strAfter = _T(" ") + m_strNegativeSign;
		break;
		}
		INT nLen = INT( strText.GetLength() );
		if( ! _strAfter.IsEmpty() )
			strText.Insert( nLen, _strAfter );
		if( ! _strBefore.IsEmpty() )
			strText.Insert( 0, _strBefore );
	}
	m_pEditWithBehaviorWnd->SetWindowText(strText);
}

long double CExtEditSystemNumber::LongDoubleGetMantissa() const
{
	CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
	long double lfVal = 0;
	CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
	INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );
	if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
		lfVal = LongDoubleGetBase( strText );
	else
	{
		CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		//CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		long double _lfFirstPart; //, _lfSecondPart;
		_lfFirstPart = LongDoubleGetBase( strFirstPart );
		//_lfSecondPart = LongDoubleGetBase( strSecondPart );
		//long double lfE = 10.0;
		//long double lfPow = powl( lfE, _lfSecondPart );
		//lfVal = _lfFirstPart * lfPow;
		lfVal = _lfFirstPart;
	}
	return lfVal;
}

long double CExtEditSystemNumber::LongDoubleGetPower() const
{
	CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
	long double lfVal = 0.0;
	CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
	INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );
	if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
		lfVal = 0.0;
	else
	{
		//CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		long double _lfSecondPart; // _lfFirstPart, 
		//_lfFirstPart = LongDoubleGetBase( strFirstPart );
		_lfSecondPart = LongDoubleGetBase( strSecondPart );
		//long double lfE = 10.0;
		//long double lfPow = powl( lfE, _lfSecondPart );
		//lfVal = _lfFirstPart * lfPow;
		lfVal = _lfSecondPart;
	}
	return lfVal;
}

long double CExtEditSystemNumber::LongDoubleGet() const
{
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
long double lfVal = 0;
CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );
	if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
		lfVal = LongDoubleGetBase( strText );
	else
	{
		CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		long double _lfFirstPart, _lfSecondPart;
		_lfFirstPart = LongDoubleGetBase( strFirstPart );
		_lfSecondPart = LongDoubleGetBase( strSecondPart );
		long double lfE = 10.0;
		long double lfPow = powl( lfE, _lfSecondPart );
		lfVal = _lfFirstPart * lfPow;
	}
	return lfVal;
}

void CExtEditSystemNumber::LongDoubleSetMantissa(
	long double lfValue,
	bool bTrimTrailingZeros // = true
	)
{
ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	long double lfPower = LongDoubleGetPower();
	CExtSafeString strMantissa = _GetDoubleText( lfValue, bTrimTrailingZeros );
	CExtSafeString strPower = _GetDoubleText( lfPower, bTrimTrailingZeros );
	CExtSafeString strNewNumber = strMantissa + _T("e") + strPower;
	m_pEditWithBehaviorWnd->SetWindowText( strNewNumber );
}

void CExtEditSystemNumber::LongDoubleSetPower(
	long double lfValue,
	bool bTrimTrailingZeros // = true
	)
{
ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	long double lfMantissa = LongDoubleGetMantissa();
	CExtSafeString strMantissa = _GetDoubleText( lfMantissa, bTrimTrailingZeros );
	CExtSafeString strPower = _GetDoubleText( lfValue, bTrimTrailingZeros );
	CExtSafeString strNewNumber = strMantissa + _T("e") + strPower;
	m_pEditWithBehaviorWnd->SetWindowText( strNewNumber );
}

long double CExtEditSystemNumber::LongDoubleGetBase( LPCTSTR strNumeric ) const
{
	ASSERT_VALID( this );
//	return _tcstod( _GetNumericText( m_pEditWithBehaviorWnd->TextGet(), true ), NULL );
//CExtSafeString str = _GetNumericText( m_pEditWithBehaviorWnd->TextGet() ); //, true 
CExtSafeString str = _GetNumericText( strNumeric, true );
	if(		str.IsEmpty()
		||	str == _T(".")
		||	str == _T("-")
		||	str == LPCTSTR(m_strDecimalPoint)
		||	( str.GetLength() == 1 && m_strDecimalPoint.GetLength() > 0 && str[0] == m_strDecimalPoint[0] )
		)
		return 0.0;
LCID _LCID = LANG_USER_DEFAULT;
	if( g_ResourceManager->IsCustomLangAllowed() )
		_LCID = g_ResourceManager->GetLangIdDesired();
DOUBLE lfTmp = 0.0;
USES_CONVERSION;
HRESULT hr = ::VarR8FromStr( (LPOLESTR) T2COLE( LPCTSTR(str) ), _LCID, 0, &lfTmp );
	ASSERT( SUCCEEDED( hr ) );
	hr;
long double lfVal = (long double)lfTmp;
	return lfVal;
}

INT CExtEditSystemNumber::IntGet() const
{
	ASSERT_VALID( this );
//	return (INT)_ttoi( _GetNumericText( m_pEditWithBehaviorWnd->TextGet(), true ) );
CExtSafeString str = _GetNumericText( m_pEditWithBehaviorWnd->TextGet() ); //, true 
	if( str.IsEmpty() )
		return 0;
LCID _LCID = LANG_USER_DEFAULT;
	if( g_ResourceManager->IsCustomLangAllowed() )
		_LCID = g_ResourceManager->GetLangIdDesired();
LONG nTmp = 0L;
USES_CONVERSION;
HRESULT hr = ::VarI4FromStr( (LPOLESTR) T2COLE( LPCTSTR(str) ), _LCID, 0, &nTmp );
	ASSERT( SUCCEEDED( hr ) );
	hr;
	return INT(nTmp);
}

CExtSafeString CExtEditSystemNumber::Behavior_GetValidText(
	bool bSetSel // = false
	) const
{
	ASSERT_VALID( this );
	CExtSafeString strNewText;
	CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
	CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
	INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );
	if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
	{
		INT nStart, nEnd;
		m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
		strNewText = Behavior_GetValidText_Base( strText, nStart, nEnd );
		if( bSetSel && ( ! m_bRichPainting ) )
		{
			if( strNewText != strText )
				m_pEditWithBehaviorWnd->SetWindowText( strNewText );
			m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
		}
	}
	else
	{
		INT nStart, nEnd;
		nStart = nEnd = 0;
		INT nStartPart, nEndPart;
		m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
		nStartPart = nStart, nEndPart = nEnd;
		bool bLeftSide = false;
		bool bRightSide = false;
		if( nStart <= nE_Pos && nEnd <= nE_Pos )
		{
			bLeftSide = true;
			nStartPart = nStart;
			nEndPart = nEnd;
		}
		if(		( nStart <= nE_Pos && nEnd > nE_Pos )
			||	( nStart > nE_Pos && nEnd <= nE_Pos )
			)
		{
			bLeftSide = true;
			bRightSide = true;
			nStartPart = nStart;
			nEndPart = nEnd;
		}
		if( nStart > nE_Pos && nEnd > nE_Pos )
		{
			bRightSide = true;
			nStartPart = nStart - nE_Pos - 1;
			nEndPart = nEnd - nE_Pos - 1;
		}
		CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		INT _nStart, _nEnd;
		if( bLeftSide )
		{
			_nStart = ( nStartPart <= nE_Pos ) ? nStartPart : nE_Pos;
			_nEnd = ( nEndPart <= nE_Pos ) ? nEndPart : nE_Pos;
		}
		strFirstPart = Behavior_GetValidText_Base( strFirstPart, _nStart, _nEnd );
		if( bLeftSide )
		{
			nStart = ( _nStart <= nE_Pos ) ? _nStart: nStart;
			nEnd = ( _nEnd <= nE_Pos ) ? _nEnd : nEnd;
		}
		if( bRightSide )
		{
			_nStart = _nEnd = 0;
			_nStart = ( nStartPart >= nE_Pos ) ? nStartPart : nE_Pos;
			_nEnd = ( nEndPart >= nE_Pos ) ? nEndPart : nE_Pos;
		}
		strSecondPart = Behavior_GetValidText_Base( strSecondPart, _nStart, _nEnd, true );
		if( bRightSide )
		{
			nStart = ( nStartPart >= nE_Pos ) ? (_nStart + nE_Pos + 1) : nStart;
			nEnd = ( nEndPart >= nE_Pos ) ? (_nEnd + nE_Pos + 1) : nEnd;
		}
		strNewText = strFirstPart + _T("e") + strSecondPart;
		if( bSetSel && ( ! m_bRichPainting ) )
		{
			if( strNewText != strText )
				m_pEditWithBehaviorWnd->SetWindowText( strNewText );
			m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
		}
	}
	return strNewText;
}

CExtSafeString CExtEditSystemNumber::Behavior_GetValidText_Base( 
	LPCTSTR strText,
	INT & nStartSel,
	INT & nEndSel,
	bool bPower // = false 
	) const
{
	ASSERT_VALID( this );
	CExtSafeString _strText = strText; //m_pEditWithBehaviorWnd->TextGet();
	if( _strText.IsEmpty() )
		return _T("");
	CExtSafeString strNewText = _strText;
	bool bIsNegative = _IsNegative( _strText );
	INT nStart, nEnd;
	nStart = nStartSel;
	nEnd = nEndSel;
	//m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
	_GetTextWithoutMarksBase( strNewText, nStart, nEnd );
	if( bPower )
		_GetTextWithMarksBase( strNewText, nStart, nEnd, false, bPower );
	else
		_GetTextWithMarks( strNewText, nStart, nEnd );
	// insert the negative sign if it is not present
	if( bIsNegative && AllowNegativeGet() )
	{
		INT nNegSignLen = INT( m_strNegativeSign.GetLength() ) + ( ( m_nNegativeFormatIndex == 2 || m_nNegativeFormatIndex == 4 ) ? 1: 0 );
		if( m_nNegativeFormatIndex == 0 )
			nNegSignLen = 1;
		CExtSafeString _strNegativeSign = ( (m_nNegativeFormatIndex == 4) ? _T(" "): _T("") ) + m_strNegativeSign + ( (m_nNegativeFormatIndex == 2) ? _T(" "): _T("") );
		if( m_nNegativeFormatIndex == 0 )
		{
			strNewText.Insert( strNewText.GetLength(), _T(")") );
			strNewText.Insert( 0, _T("(") );
			nStart += 1;
			nEnd += 1;
		}
		if( m_nNegativeFormatIndex == 1 || m_nNegativeFormatIndex == 2 )
			strNewText.Insert( 0, _strNegativeSign );
		if( m_nNegativeFormatIndex == 3 || m_nNegativeFormatIndex == 4 )
			strNewText.Insert( strNewText.GetLength(), _strNegativeSign );
		if( m_nNegativeFormatIndex == 1 || m_nNegativeFormatIndex == 2 )
		{
			nStart += nNegSignLen;
			nEnd += nNegSignLen;
		}
	}
// 	if( bSetSel && ( ! m_bRichPainting ) )
// 	{
// 		CExtSafeString strCurrentText = m_pEditWithBehaviorWnd->TextGet();
// 		if( strNewText != strCurrentText )
// 			m_pEditWithBehaviorWnd->SetWindowText( strNewText );
// 		m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
// 	}
	nStartSel = nStart;
	nEndSel = nEnd;
	return strNewText;
}

void CExtEditSystemNumber::Behavior_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	if( m_bUseScientificNumberMode )
		Behavior_Scientific_Mode_OnChar( uChar, nRepCnt, nFlags );
	else
	{
		CString strText;
		GetWindowText( strText );
		INT nStart, nEnd; // !!!
		GetSel( nStart, nEnd ); // !!!
		INT nStartOld, nEndOld; // !!!
		nStartOld = nStart;
		nEndOld = nEnd;
		CString strOldText = strText;
		Behavior_Base_Mode_OnChar( strText, nStart, nEnd, uChar ); // !!!
		if( strOldText != strText )
			m_pEditWithBehaviorWnd->TextSet( strText );
		//if( nStart != nStartOld || nEnd != nEndOld )
			m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
		if( uChar == 22 || uChar == 3 ) // ctrl-v, ctrl-c
			CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags ); // !!! 
	}
}

void CExtEditSystemNumber::Behavior_Scientific_Mode_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	if( m_pEditWithBehaviorWnd->GetStyle() & ES_READONLY )
		return;
	if( uChar < 8 && uChar != 3 ) // except ctr-c
		return;
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );
	if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
	{
		CString strText; // !!!
		GetWindowText( strText ); // !!!
		INT nStart, nEnd; // !!!
		GetSel( nStart, nEnd ); // !!!
		INT nStartOld, nEndOld; // !!!
		nStartOld = nStart;
		nEndOld = nEnd;
		CString strOldText = strText;
		Behavior_Base_Mode_OnChar( strText, nStart, nEnd, uChar ); // !!!
		if( strOldText != strText )
			m_pEditWithBehaviorWnd->TextSet( strText );
		if( nStart != nStartOld || nEnd != nEndOld )
			m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
	}
	else
	{
		TCHAR c = static_cast<TCHAR>( uChar );
		CExtSafeString strScientificSymbolLower, strScientificSymbolUpper;
		strScientificSymbolLower = strScientificSymbolUpper = m_strScientificSymbol;
		strScientificSymbolLower.MakeLower();
		strScientificSymbolUpper.MakeUpper();
		if( c == strScientificSymbolLower || c == strScientificSymbolUpper ) // ( c == _T('e') || c == _T('E') )
			return;
		INT nStart, nEnd;
		INT nStartPart, nEndPart;
		m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
		INT nStartOld, nEndOld; // !!!
		nStartOld = nStart;
		nEndOld = nEnd;
		nStartPart = nStart, nEndPart = nEnd;
		bool bLeftSide = false;
		bool bRightSide = false;
		bool bDeleteRightSide = false;
		if( nStart <= nE_Pos  && nEnd <= nE_Pos )
		{
			bLeftSide = true;
			nStartPart = nStart;
			nEndPart = nEnd;
		}
		if( nStart <= nE_Pos  && nEnd > nE_Pos )
		{
			bDeleteRightSide = true;
			bLeftSide = true;
			nStartPart = nStart;
			nEndPart = nE_Pos;
		}
		if( nStart > nE_Pos  && nEnd <= nE_Pos )
		{
			bDeleteRightSide = true;
			bLeftSide = true;
			nStartPart = nE_Pos;
			nEndPart = nEnd;
		}
		if( nStart > nE_Pos  && nEnd > nE_Pos )
		{
			bRightSide = true;
			nStartPart = nStart - nE_Pos - 1;
			nEndPart = nEnd - nE_Pos - 1;
		}
		CString strOldText = strText;
		CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		if( bLeftSide )
		{
			//m_pEditWithBehaviorWnd->TextSet( strFirstPart ); // !!!
			//m_pEditWithBehaviorWnd->SetSel( nStartPart, nEndPart ); // !!!
			Behavior_Base_Mode_OnChar( strFirstPart, nStartPart, nEndPart, uChar ); // !!!
 			//strFirstPart = m_pEditWithBehaviorWnd->TextGet(); // !!!
			//m_pEditWithBehaviorWnd->GetSel( nStart, nEnd ); // !!!
			nStart = nStartPart; // !!!
			nEnd = nEndPart; // !!!
		}
		else if( bRightSide )
		{
			//m_pEditWithBehaviorWnd->TextSet( strSecondPart ); // !!!
			//m_pEditWithBehaviorWnd->SetSel( nStartPart, nEndPart ); // !!!
			Behavior_Base_Mode_OnChar( strSecondPart, nStartPart, nEndPart, uChar, true ); // !!!
 			//strSecondPart = m_pEditWithBehaviorWnd->TextGet(); // !!!
			//m_pEditWithBehaviorWnd->GetSel( nStart, nEnd ); // !!!
			nStart = nStartPart; // !!!
			nEnd = nEndPart; // !!!
			nStart += nE_Pos + 1; // !!!
			nEnd += nE_Pos + 1; // !!!
		}
		strSecondPart = bDeleteRightSide ? _T("") : ( _T("e") + strSecondPart );
		CExtSafeString strRes = strFirstPart + strSecondPart;
		if( strOldText != strRes )
			m_pEditWithBehaviorWnd->TextSet( strRes );
		if( nStart != nStartOld || nEnd != nEndOld )
			m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
	}
	if( uChar == 22 || uChar == 3 ) // ctrl-v, ctrl-c
		CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags ); // !!! 
}

void CExtEditSystemNumber::Behavior_Base_Mode_OnChar( 
	CExtSafeString & strTextBase,
	INT & nStartSel,
	INT & nEndSel,
	UINT uChar,
	bool bPower // = false
	)
{
	ASSERT_VALID( this );
	if( m_pEditWithBehaviorWnd->GetStyle() & ES_READONLY )
		return;
	if( uChar == 3 ) // ctrl-c
		return;
TCHAR c = static_cast<TCHAR>( uChar );
INT nStart, nEnd; // !!!
	nStart = nStartSel, nEnd = nEndSel; // !!!
//	m_pEditWithBehaviorWnd->GetSel( nStart, nEnd ); // !!!
//CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet(); // !!!
CExtSafeString strText = strTextBase;
CExtSafeString strNumericText = _GetNumericText( strText );
INT nDecimalPos = INT( strText.Find( m_strDecimalPoint ) );
INT nNumericDecimalPos = INT( strNumericText.Find( m_strDecimalPoint ) );
INT nLen = INT( strText.GetLength() );
INT nNumericLen = INT( strNumericText.GetLength() );
INT nSepCount = _GetGroupSeparatorCount( strText );
bool bNeedAdjustment = false;
INT nNegSignLen = INT( m_strNegativeSign.GetLength() );
bool bIsNegative = false;
bool bIsDecimalPoint = false;
CExtSafeString strScientificSymbolLower, strScientificSymbolUpper;
	strScientificSymbolLower = strScientificSymbolUpper = m_strScientificSymbol;
	strScientificSymbolLower.MakeLower();
	strScientificSymbolUpper.MakeUpper();
	if(		( c == m_strNegativeSign[0/*nNegSignMatch*/] )
		||	(	m_nNegativeFormatIndex == 0 
			&&	c == _T('(') 
				)
		||	( c == _T('-') )
		)
		bIsNegative = true;
	// check if it's a negative sign
	if( bIsNegative && AllowNegativeGet() )
	{
		bool bNegative = _IsNegative( strText );
		CExtSafeString _strBefore = _T(""), _strAfter = _T("");
		switch( m_nNegativeFormatIndex )
		{
		case 0:
			_strBefore = _T("(");
			_strAfter = _T(")");
		break;
		case 1:
			_strBefore = m_strNegativeSign;
		break;
		case 2:
			_strBefore = m_strNegativeSign + _T(" ");
		break;
		case 3:
			_strAfter = m_strNegativeSign;
		break;
		case 4:
			_strAfter = _T(" ") + m_strNegativeSign;
		break;
		}
		if( m_bUseScientificNumberMode )
		{
			_strBefore = _T("-");
			_strAfter = _T("");
		}
		INT nBeforLen = INT( _strBefore.GetLength() );
		INT nAfterLen = INT( _strAfter.GetLength() );
		CExtSafeString strNewText = strText; // !!!
		//m_pEditWithBehaviorWnd->GetSel( nStart, nEnd ); // !!!
		nStart = nStartSel, nEnd = nEndSel; // !!!
		if( bNegative )
		{
			if( nStart < nBeforLen )
				nStart = nBeforLen;
			if( nEnd > ( nLen - nAfterLen ) )
				nEnd = ( nLen - nAfterLen );
		}
		_GetTextWithoutMarksBase( strNewText, nStart, nEnd );
		//_GetTextWithMarks( strNewText, nStart, nEnd, (! bNegative) );
		_GetTextWithMarksBase( strNewText, nStart, nEnd, (! bNegative), bPower );
		//CExtSafeString strCurrentText = m_pEditWithBehaviorWnd->TextGet(); // !!!
		CExtSafeString strCurrentText = strTextBase; // !!!
		if( strNewText != strCurrentText )
		{
			strTextBase = strNewText; // !!!
			//m_pEditWithBehaviorWnd->SetWindowText( strNewText ); // !!!
		}
		//m_pEditWithBehaviorWnd->SetSel( nStart, nEnd ); // !!!
		nStartSel = nStart, nEndSel = nEnd; // !!!
		return;
	}
	// test whether it is decimal point (only one is enabled)
	else if( c == m_strDecimalPoint[0] && m_nMaxFractionDigits > 0 )
	{
		if( ! bPower )
		{
			if( nDecimalPos >= 0 )
			{
				// test decimal point replacement
				if( nDecimalPos >= nStart && nDecimalPos < nEnd )
				{
					bNeedAdjustment = true;
					bIsDecimalPoint = true;
				}
				else
				{	// else set caret
					// m_pEditWithBehaviorWnd->SetSel( nDecimalPos + m_strDecimalPoint.GetLength(), nDecimalPos + m_strDecimalPoint.GetLength() ); // !!!
					nStartSel = nDecimalPos + m_strDecimalPoint.GetLength(); // !!!
					nEndSel = nDecimalPos + m_strDecimalPoint.GetLength(); // !!!
					return;
				}
			}
			else
			{
				bNeedAdjustment = true;
				bIsDecimalPoint = true;
			}
		}
		else
			return;
	}
	else if( _istdigit(c) )
	{
		// test we're on the right of the decimal point.
		if( nDecimalPos >= 0 && nDecimalPos < nStart )
		{
			bool bIsNegative = ( ! strNumericText.IsEmpty() && ( strNumericText.Find( m_strNegativeSign ) >= 0 ) && ( m_nNegativeFormatIndex == 3 || m_nNegativeFormatIndex == 4 ) );
			if( ! bIsNegative )
				bIsNegative = ( ! strNumericText.IsEmpty() && ( strNumericText.Find( _T('(') ) >= 0 ) && m_nNegativeFormatIndex == 0 );

			INT nNegLength = 0;
			if( m_nNegativeFormatIndex != 0 )
				nNegLength = nNegSignLen + ( ( m_nNegativeFormatIndex == 4 ) ? 1 : 0 );
			else
			{
				nNegLength = 1;
				if( nStart == nEnd && nStart == nLen )
					nStart = nEnd = nLen - 1;
			}
			if( bIsNegative )
			{
				INT nNegSignPos = -1;
				if( m_nNegativeFormatIndex == 3 )
					nNegSignPos = INT( strText.Find( m_strNegativeSign ) );
				if( m_nNegativeFormatIndex == 4 )
					nNegSignPos = INT( strText.Find( _T(' ') + m_strNegativeSign ) );
				if( m_nNegativeFormatIndex == 0 )
					nNegSignPos = INT( strText.Find( _T(')') ) );
				if( ( nStart > nNegSignPos ) && ( nStart <= nLen ) && nNegSignPos != -1 )
				{
					nStart = nNegSignPos;
					//m_pEditWithBehaviorWnd->SetSel( nStart, nStart ); // !!!
					nStartSel = nEndSel = nStart; // !!!
				}
			}
			INT nDecimalSeparatorLen = INT( m_strDecimalPoint.GetLength() );
			if( nDecimalPos <= nStart && nStart < ( nDecimalPos + nDecimalSeparatorLen ) )
			{
				INT nPrevStart = nStart;
				nStart = ( nDecimalPos + nDecimalSeparatorLen );
				if(		nEnd == nPrevStart 
					||	nEnd < nPrevStart 
					||	(	nEnd > nPrevStart 
						&&	nEnd < nStart
						)
					)
					nEnd = nStart;
				//m_pEditWithBehaviorWnd->SetSel( nStart, nEnd ); // !!!
				nStartSel = nStart; // !!!
				nEndSel = nEnd; // !!!
			}
			if( strNumericText.Mid( /*nDecimalPos */ nNumericDecimalPos + nDecimalSeparatorLen ).GetLength() == ( m_nMaxFractionDigits + ( bIsNegative ? nNegLength : 0 ) ) )
			{
				if( nStart <= ( nDecimalPos + m_nMaxFractionDigits + nDecimalSeparatorLen - 1 ) )
				{
					nEnd = ( ( ( nEnd == nLen ) || ( nEnd > ( nStart + 1 ) ) ) ? nEnd : (nStart + 1) );
					//m_pEditWithBehaviorWnd->SetSel( nStart, nEnd ); // !!!
// 					nStartSel = nStart; // !!!
// 					nEndSel = nEnd; // !!!
					//m_pEditWithBehaviorWnd->ReplaceSel( CExtSafeString(c), TRUE ); // !!!
					CExtSafeString _strText = strTextBase; // !!!
					INT _nStart = min( nStart, nEnd );
					INT _nEnd = max( nStart, nEnd );
					_strText.Delete( _nStart, (_nEnd - _nStart) ); // !!!
					_strText.Insert( _nStart, c ); // !!!
					strTextBase = _strText; // !!!
					nStartSel = nEndSel = _nStart + 1; // !!!
					//CExtSafeString strReplaceText = Behavior_GetValidText(); // !!!
					CExtSafeString strReplaceText = Behavior_GetValidText_Base( strTextBase, nStartSel, nEndSel, bPower ); // !!!
				}
				return;
			}
			else if( strNumericText.Mid( /*nDecimalPos */ nNumericDecimalPos + nDecimalSeparatorLen ).GetLength() > ( m_nMaxFractionDigits + ( bIsNegative ? nNegLength : 0 ) ) )
			{
				ASSERT(false);
				return;
			}
		}
		// we're on the left side of the decimal point
		else
		{
			bool bIsNegative = ( ! strNumericText.IsEmpty() && ( strNumericText.Find( m_strNegativeSign ) >= 0 ) && ( m_nNegativeFormatIndex == 1 || m_nNegativeFormatIndex == 2 ) );
			if( ! bIsNegative )
				bIsNegative = ( ! strNumericText.IsEmpty() && ( strNumericText.Find( _T('(') ) >= 0 ) && m_nNegativeFormatIndex == 0 );
			bool bIsNegativeRight = ( ! strNumericText.IsEmpty() && ( strNumericText.Find( m_strNegativeSign ) >= 0 ) && ( m_nNegativeFormatIndex == 3 || m_nNegativeFormatIndex == 4 ) );
			INT nNegLength = 0;
				nNegLength 
					= nNegSignLen
					+ (	
						(		m_nNegativeFormatIndex == 2 
							||	m_nNegativeFormatIndex == 4
							) ? 1 : 0 
						);

			if( m_nNegativeFormatIndex == 0 )
			{
				nNegLength = 1;
				if( nStart == nEnd && nStart == nLen )
					nStart = nEnd = nLen - 1;
			}
			if( bIsNegative )
			{
				if( ( nStart >= 0 ) && ( nStart < nNegLength ) )
				{
					nStart = nNegLength;
					if( nStart > nEnd )
						nEnd = nStart;
					//m_pEditWithBehaviorWnd->SetSel( nStart, nEnd ); // !!!
					nStartSel = nStart; // !!!
					nEndSel = nEnd; // !!!
				}
			}
			if( bIsNegativeRight || ( bIsNegative && m_nNegativeFormatIndex == 0 ) )
			{
				if( ( nEnd >= ( nLen - nNegLength ) ) && ( nEnd <= nLen ) )
				{
					nEnd = nLen - nNegLength;
					if( nStart > nEnd )
						nStart = nEnd;
					//m_pEditWithBehaviorWnd->SetSel( nStart, nEnd ); // !!!
					nStartSel = nStart; // !!!
					nEndSel = nEnd; // !!!
				}
				if( ( nStart >= ( nLen - nNegLength ) ) && ( nStart <= nLen ) )
				{
					nStart = nLen - nNegLength;
					//m_pEditWithBehaviorWnd->SetSel( nStart, nStart ); // !!!
					nStartSel = nStart; // !!!
					nEndSel = nStart; // !!!
				}
			}
			INT nGroupSeparatorLen = m_bEmptyGroupSeparatorMode ? 0 : INT( m_strGroupSeparator.GetLength() );
			INT nDecimalSignLen = INT( m_strDecimalPoint.GetLength() );
			// ensure we can still enter digits.
			INT nDecPos = m_nMaxWholeDigits + ( bIsNegative ? nNegLength : 0 ) + ( nSepCount * nGroupSeparatorLen );
			if( m_bUseScientificNumberMode && bPower && (nStart == nDecPos) )
				nDecPos++;
			if( nStart == nDecPos )
			{
				UINT nFlags = CBehaviorBase::m_nBehaviorFlags;
				if(  ( nFlags & __ESE_ADD_DECIMAL_AFTER_MAX_WHOLE_DIGITS ) != 0 && m_nMaxFractionDigits > 0 )
				{
					nEnd = ( ( ( nEnd == nLen ) || ( ( nEnd > nStart ) && ( ( nEnd - nStart ) > 1 ) ) ) ? nEnd : ( nStart + nDecimalSignLen + 1 ) );
					if( bIsNegativeRight || ( bIsNegative && m_nNegativeFormatIndex == 0 ) )
					{
						//m_pEditWithBehaviorWnd->SetSel( nStart, strText.GetLength() ); // !!!
						//m_pEditWithBehaviorWnd->ReplaceSel( _T(""), TRUE ); // !!!
						CExtSafeString _strText = strTextBase; // !!!
						INT _nStart = min( nStart, nEnd );
						//INT _nEnd = max( nStart, nEnd );
						_strText.Delete( _nStart, (strText.GetLength() - _nStart) ); // !!!
						strTextBase = _strText; // !!!
						nStartSel = nEndSel = _nStart; // !!!
					}
					//m_pEditWithBehaviorWnd->SetSel( nStart, nEnd ); // !!!
					//m_pEditWithBehaviorWnd->ReplaceSel( m_strDecimalPoint + c, TRUE ); // !!!
					CExtSafeString _strText = strTextBase; // !!!
					INT _nStart = min( nStart, nEnd );
					INT _nEnd = max( nStart, nEnd );
					_strText.Delete( _nStart, (_nEnd - _nStart) ); // !!!
					_strText.Insert( _nStart, (m_strDecimalPoint + c) ); // !!!
					strTextBase = _strText; // !!!
					nStartSel = nEndSel = _nStart + m_strDecimalPoint.GetLength() + 1; // !!!
					if( bIsNegativeRight || ( bIsNegative && m_nNegativeFormatIndex == 0 ) )
					{
						//CExtSafeString strChangedText = m_pEditWithBehaviorWnd->TextGet(); // !!!
						CExtSafeString strChangedText = strTextBase; // !!!
						CExtSafeString _strNegativeSign = ( m_nNegativeFormatIndex == 4 ? _T(" "): _T("") ) + m_strNegativeSign;
						nEnd = INT( strChangedText.GetLength() );
						//m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd ); // !!!
						nStartSel = nEndSel = nStart = nEnd; // !!!
						if( m_nNegativeFormatIndex != 0 )
						{
							//m_pEditWithBehaviorWnd->ReplaceSel( _strNegativeSign, TRUE ); // !!!
							CExtSafeString _strText = strTextBase; // !!!
							INT _nStart = min( nStart, nEnd );
							INT _nEnd = max( nStart, nEnd );
							_strText.Delete( _nStart, (_nEnd - _nStart) ); // !!!
							_strText.Insert( _nStart, _strNegativeSign ); // !!!
							strTextBase = _strText; // !!!
							//nStartSel = nEndSel = nStart + m_strDecimalPoint.GetLength() + 1; // !!!
							nEnd = nStart = _nStart + _strNegativeSign.GetLength(); // !!!
						}
						else
						{
							//m_pEditWithBehaviorWnd->ReplaceSel( _T(")"), TRUE ); // !!!
							CExtSafeString _strText = strTextBase; // !!!
							INT _nStart = min( nStart, nEnd );
							INT _nEnd = max( nStart, nEnd );
							_strText.Delete( _nStart, (_nEnd - _nStart) ); // !!!
							_strText.Insert( _nStart, _T(")") ); // !!!
							strTextBase = _strText; // !!!
							//nStartSel = nEndSel = nStart + m_strDecimalPoint.GetLength() + 1; // !!!
							nEnd = nStart = _nStart + 1; // !!!
						}
						//m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd ); // !!!
						nStartSel = nEndSel = nEnd; // !!!
					}
					//CExtSafeString strReplaceText = Behavior_GetValidText(true); // !!!
					_strText = Behavior_GetValidText_Base( strTextBase, nStartSel, nEndSel, bPower ); // !!!
					strTextBase = _strText; // !!!
				}
				return;
			}
			INT nMaxWholeDigits = m_nMaxWholeDigits;
			if( m_bUseScientificNumberMode && bPower )
				nMaxWholeDigits++;
			if(	strNumericText.Mid( 
					0, 
					(	( nNumericDecimalPos >= 0 )
						? nNumericDecimalPos
						: nNumericLen
							)
						).GetLength()
						== ( nMaxWholeDigits + ( bIsNegative ? nNegLength : 0 ) ) //  + ( ( nNumericDecimalPos >= 0 ) ? ( m_strDecimalPoint.GetLength() ) : 0 )
				)
			{
				{
					INT nPos = 0;
					if( (! m_bEmptyGroupSeparatorMode) && (! m_strGroupSeparator.IsEmpty() ) )
					{
						for( ; true; )
						{
							nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
							if( nPos == -1 )
								break;
							if( nPos <= nStart && nStart < ( nPos + nGroupSeparatorLen ) )
							{
								nStart = nPos + nGroupSeparatorLen;
								break;
							}
							nPos ++;
						}
					}
					INT _nNegativeSignLen = INT( m_strNegativeSign.GetLength() ) + ( (m_nNegativeFormatIndex == 2 || m_nNegativeFormatIndex == 4) ? 1: 0 );
					if( m_nNegativeFormatIndex == 0 )
						_nNegativeSignLen = 1;
					if( nStart < _nNegativeSignLen && bIsNegative )
						nStart = _nNegativeSignLen;
					nEnd = ( ( ( nEnd == nLen ) || ( nEnd > ( nStart + 1 ) ) ) ? nEnd : ( nStart + 1 ) );
					if( bIsNegativeRight )
					{
						INT nNegSignPos = INT( strText.Find( m_strNegativeSign ) );
						if( nEnd > nNegSignPos )
							nEnd = nNegSignPos;
					}
					//m_pEditWithBehaviorWnd->SetSel( nStart, nEnd ); // !!!
					//m_pEditWithBehaviorWnd->ReplaceSel( CExtSafeString(c), TRUE ); // !!!
					CExtSafeString _strText = strTextBase; // !!!
					INT _nStart = min( nStart, nEnd );
					INT _nEnd = max( nStart, nEnd );
					_strText.Delete( _nStart, (_nEnd - _nStart) ); // !!!
					_strText.Insert( _nStart, c ); // !!!
					strTextBase = _strText; // !!!
					nStartSel = nEndSel = _nStart + 1; // !!!
					//CExtSafeString strReplaceText = Behavior_GetValidText(true); // !!!
					_strText = Behavior_GetValidText_Base( strTextBase, nStartSel, nEndSel, bPower ); // !!!
					strTextBase = _strText; // !!!

				}
				return;
			}
			bNeedAdjustment = true;
		}
	}
	else if( m_bUseScientificNumberMode && ( c == strScientificSymbolLower || c == strScientificSymbolUpper ) ) // !!! ( c == _T('e') || c == _T('E') )
	{
// 		m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
// 		m_pEditWithBehaviorWnd->ReplaceSel( CExtSafeString(c), TRUE );
		bNeedAdjustment = false;
	}
	// test whether it is a non-printable character (i.e. backspace, ctrl+c )
	else if( ! ( _istprint( c ) || IsCharAlphaNumeric( c ) ) )
		bNeedAdjustment = true;
	else
		return;

// 	if( ! m_bBackspace ) // !!! must be moved to base method
// 	{
// 		//CExtEditSystemNumberCurrencyBase::CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags );
// 		CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags ); // !!! 
// 
// 		// if the decimal point was added/removed or a separator needs adding/removing, adjust the text
// 		if( bNeedAdjustment )
// 			_AdjustSeparators( nSepCount, bIsDecimalPoint ); // !!! change: add new CString argument
// 	}
// 	else
// 		m_bBackspace = false;

	if( ! m_bBackspace ) // !!! must be moved to base method
	{
		//CExtEditSystemNumberCurrencyBase::CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags );
		//CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags ); // !!! 

		CExtSafeString _strText = strTextBase; // !!!
		INT _nStart = min( nStart, nEnd );
		INT _nEnd = max( nStart, nEnd );
		_strText.Delete( _nStart, (_nEnd - _nStart) ); // !!!
		_strText.Insert( _nStart, c ); // !!!
		strTextBase = _strText; // !!!
		nStartSel = nEndSel = _nStart + 1; // !!!

		// if the decimal point was added/removed or a separator needs adding/removing, adjust the text
		if( bNeedAdjustment )
		{
			CExtSafeString _strText = Behavior_GetValidText_Base( strTextBase, nStartSel, nEndSel, bPower ); // !!!
			strTextBase = _strText; // !!!
		}
	}
	else
		m_bBackspace = false;
}

void CExtEditSystemNumber::Behavior_OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags ) 
{
	ASSERT_VALID( this );
	switch( uChar )
	{
	case VK_DELETE:
	{
		if( ( GetStyle() & ES_READONLY ) != 0 )
			return;
		CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
		CString strScientificSymbol = m_strScientificSymbol;
		strScientificSymbol.MakeLower();
		INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
		strScientificSymbol.MakeUpper();
		if( nE_Pos < 0 )
			nE_Pos = INT( strText.Find( strScientificSymbol ) );
		if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
		{
			INT nStart = 0, nEnd = 0;
			m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
			CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
			if( _DeleteSymbols( strText, nStart, nEnd ) )
			{
				m_pEditWithBehaviorWnd->SetWindowText( strText );
				m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
			}
			else
				m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
		}
		else
		{
			INT nStart, nEnd;
			INT nStartPart, nEndPart;
			m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
			nStartPart = nStart, nEndPart = nEnd;
			bool bLeftSide = false;
			bool bRightSide = false;
			bool bDeleteRightSide = false;
			bool bDel = false;
			if( nStart <= nE_Pos  && nEnd <= nE_Pos )
			{
				bLeftSide = true;
				nStartPart = nStart;
				nEndPart = nEnd;
				if( nStartPart == nEndPart && nStartPart == nE_Pos )
				{
					bDeleteRightSide = true;
					bDel = true;
				}
			}
			if( nStart <= nE_Pos  && nEnd > nE_Pos )
			{
				bDeleteRightSide = true;
				bLeftSide = true;
				nStartPart = nStart;
				nEndPart = nE_Pos;
			}
			if( nStart > nE_Pos  && nEnd <= nE_Pos )
			{
				bDeleteRightSide = true;
				bLeftSide = true;
				nStartPart = nE_Pos;
				nEndPart = nEnd;
			}
			if( nStart > nE_Pos  && nEnd > nE_Pos )
			{
				bRightSide = true;
				nStartPart = nStart - nE_Pos - 1;
				nEndPart = nEnd - nE_Pos - 1;
			}
			CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
			CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
			if( bLeftSide )
			{
				if( _DeleteSymbols( strFirstPart, nStartPart, nEndPart ) )
				{
					bDel = true;
					nStart = nEnd = nStartPart;
				}
				else
					nStart = nEnd = nEndPart;
			}
			else if( bRightSide )
			{
				if( _DeleteSymbols( strSecondPart, nStartPart, nEndPart, false, 1 ) )
				{
					bDel = true;
					nStart = nEnd = nStartPart;
				}
				else
					nStart = nEnd = nEndPart;
				nStart += nE_Pos + 1;
				nEnd += nE_Pos + 1;
			}
			strSecondPart = bDeleteRightSide ? _T("") : ( _T("e") + strSecondPart );
			CExtSafeString strRes = strFirstPart + strSecondPart;
			if( bDel )
				m_pEditWithBehaviorWnd->TextSet( strRes );
			m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
		}
		return;
	}
	case VK_BACK:
	{
		if( ( GetStyle() & ES_READONLY ) != 0 )
			return;
		CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
		CString strScientificSymbol = m_strScientificSymbol;
		strScientificSymbol.MakeLower();
		INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
		strScientificSymbol.MakeUpper();
		if( nE_Pos < 0 )
			nE_Pos = INT( strText.Find( strScientificSymbol ) );

		if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
		{
			CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
			if( strText.IsEmpty() )
				return;
			INT nStart = 0, nEnd = 0;
			m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
			if( _DeleteSymbols( strText, nStart, nEnd, true ) )
				m_pEditWithBehaviorWnd->SetWindowText( strText );
			m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
		}
		else
		{
			INT nStart, nEnd;
			INT nStartPart, nEndPart;
			m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
			nStartPart = nStart, nEndPart = nEnd;
			bool bLeftSide = false;
			bool bRightSide = false;
			bool bDeleteRightSide = false;
			bool bDel = false;
			if( nStart <= nE_Pos  && nEnd <= nE_Pos )
			{
				bLeftSide = true;
				nStartPart = nStart;
				nEndPart = nEnd;
			}
			if( nStart <= nE_Pos  && nEnd > nE_Pos )
			{
				bDeleteRightSide = true;
				bLeftSide = true;
				nStartPart = nStart;
				nEndPart = nE_Pos;
			}
			if( nStart > nE_Pos  && nEnd <= nE_Pos )
			{
				bDeleteRightSide = true;
				bLeftSide = true;
				nStartPart = nE_Pos;
				nEndPart = nEnd;
			}
			if( nStart > nE_Pos  && nEnd > nE_Pos )
			{
				nStartPart = nStart - nE_Pos - 1;
				nEndPart = nEnd - nE_Pos - 1;
				if( nStartPart == nEndPart && nStartPart == 0 )
				{
					bDeleteRightSide = true;
					m_bBackspace = true;
					bDel = true;
				}
				else
					bRightSide = true;
			}
			CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
			CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
			if( bLeftSide )
			{
				bDel = _DeleteSymbols( strFirstPart, nStartPart, nEndPart, true );
				nStart = nEnd = nStartPart;
			}
			else if( bRightSide )
			{
				bDel = _DeleteSymbols( strSecondPart, nStartPart, nEndPart, true, 1 );
				nStart = nEnd = nStartPart;
				nStart += nE_Pos + 1;
				nEnd += nE_Pos + 1;
			}
			strSecondPart = bDeleteRightSide ? _T("") : ( _T("e") + strSecondPart );
			CExtSafeString strRes = strFirstPart + strSecondPart;
			if( bDel )
				m_pEditWithBehaviorWnd->SetWindowText( strRes );
			m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
		}
		return;
	}
	}
	CExtEditSystemNumberCurrencyBase::Behavior_OnKeyDown( uChar, nRepCnt, nFlags );
}

bool CExtEditSystemNumber::_DeleteSymbols( 
	CExtSafeString & strText, 
	INT & nStart, 
	INT & nEnd, 
	bool bBackspace, // = false
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( ! m_bRichPainting );
INT nLen = INT( strText.GetLength() );
bool bNegative = _IsNegative( strText );
CExtSafeString strTextLen = strText;
INT nStLen = 0, nEndLen = 0;
	_GetTextWithoutMarksBase( strTextLen, nStLen, nEndLen );
INT nTextLen = INT( strTextLen.GetLength() );
INT nDecSepPosLen = INT( strTextLen.Find( m_strDecimalPoint ) );
INT nGroupSeparatorLen = m_bEmptyGroupSeparatorMode ? 0 : INT( m_strGroupSeparator.GetLength() );
INT nDecimalSeparatorLen = INT( m_strDecimalPoint.GetLength() );
INT nDecimalSeparatorPos = INT( strText.Find( m_strDecimalPoint ) );
INT _nPos = 0, _nGroupSignBeforeStart = 0, _nGroupSignCount = (INT)_GetGroupSeparatorCount( strText );
	if( nGroupSeparatorLen > 0 )
	{
		for( ; true; )
		{
			_nPos = (INT)strText.Find( m_strGroupSeparator, _nPos );
			if( _nPos < 0 )
				break;
			if( _nPos < nStart )
				_nGroupSignBeforeStart ++;
			_nPos ++;
		}
	}
	_nGroupSignBeforeStart = _nGroupSignCount - _nGroupSignBeforeStart;
	if( bBackspace )
	{
		if( nStart == 0 && nStart == nEnd )
			return false;
		INT nPos = 0;
		if( nStart >= 0 )
		{
			if(		(	nDecimalSeparatorPos < nStart
					||	(	nDecimalSeparatorPos == nStart
						&&	nStart != nEnd
						)
					)
				&&	nStart <= ( nDecimalSeparatorPos + nDecimalSeparatorLen )
				&&	nEnd <= ( nDecimalSeparatorPos + nDecimalSeparatorLen )
				&&	nDecSepPosLen == m_nMaxWholeDigits
				&&	(	nTextLen > ( nDecSepPosLen + nDecimalSeparatorLen )
					&&	nDecSepPosLen != -1
					)
				)
			{
				nStart = nDecimalSeparatorPos;
				nEnd = nDecimalSeparatorPos;
				m_bBackspace = true;
				return false;
			}
			if( nGroupSeparatorLen > 0 )
			{
				for( ; true; )
				{
					nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
					if( nPos == -1 )
						break;
					if(		(	nPos < nStart 
							||	(	nPos == nStart 
								&&	nStart != nEnd
								)
							)
						&&	nStart <= ( nPos + nGroupSeparatorLen )
						&&	nEnd <= ( nPos + nGroupSeparatorLen )
						)
					{
						nStart = nPos;
						nEnd = nPos;
						m_bBackspace = true;
						return false;
					}
					nPos ++;
				}
			}
		}
	}
	else
	{
		if( nStart == nLen )
			return false;
		INT nPos = 0;
		if( nStart < nLen )
		{
			if(		nDecimalSeparatorPos <= nStart
				&&	nStart < ( nDecimalSeparatorPos + nDecimalSeparatorLen )
				&&	nEnd <= ( nDecimalSeparatorPos + nDecimalSeparatorLen )
				&&	nDecSepPosLen == m_nMaxWholeDigits
				&&	(	nTextLen > ( nDecSepPosLen + nDecimalSeparatorLen )
					&&	nDecSepPosLen != -1
					)
				)
			{
				nStart = nDecimalSeparatorPos + nDecimalSeparatorLen;
				nEnd = nDecimalSeparatorPos + nDecimalSeparatorLen;
				return false;
			}
			if( nGroupSeparatorLen > 0 )
			{
				for( ; true; )
				{
					nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
					if( nPos == -1 )
						break;
					if(		nPos <= nStart 
						&&	nStart < ( nPos + nGroupSeparatorLen )
						&&	nEnd <= ( nPos + nGroupSeparatorLen )
						)
					{
						nStart = nPos + nGroupSeparatorLen;
						nEnd = nPos + nGroupSeparatorLen;
						return false;
					}
					nPos ++;
				}
			}
		}
	}
	if( nStart != nEnd )
	{
		if(		nDecimalSeparatorPos < nStart
			&&	nStart < ( nDecimalSeparatorPos + nDecimalSeparatorLen )
			)
		{
			nStart = nDecimalSeparatorPos + nDecimalSeparatorLen;
			if( nStart > nEnd )
				nEnd = nStart;
		}
		if(		nDecimalSeparatorPos < nEnd
			&&	nEnd <= ( nDecimalSeparatorPos + nDecimalSeparatorLen )
			)
			nEnd = nDecimalSeparatorPos;
		if( nGroupSeparatorLen > 0 )
		{
			INT nPos = 0;
			for( ; true; )
			{
				nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
				if( nPos == -1 )
					break;
				if(		nPos < nStart 
					&&	nStart < ( nPos + nGroupSeparatorLen )
					)
				{
					nStart = nPos + nGroupSeparatorLen;
					if( nStart >= nEnd )
					{
						nEnd = nStart;
						break;
					}
				}
				if(		nPos < nEnd 
					&&	nEnd < ( nPos + nGroupSeparatorLen )
					)
				{
					nEnd = nPos;
					if( nEnd < nStart )
						nStart = nEnd;
					if( nEnd >= nStart )
						break;
				}
				nPos ++;
			}
		}
	}

	{
		INT nNegSignLen = INT( m_strNegativeSign.GetLength() ) + ( ( m_nNegativeFormatIndex == 2 || m_nNegativeFormatIndex == 4 ) ? 1 : 0 );
		if( m_nNegativeFormatIndex == 0 )
			nNegSignLen = 1;
		bool bNegDel = false;
		if( bNegative )
		{
			if(		( nStart < nNegSignLen || ( nStart == nNegSignLen  && nStart == nEnd  && bBackspace ) ) 
				&&	( m_nNegativeFormatIndex == 1 || m_nNegativeFormatIndex == 2 || m_nNegativeFormatIndex == 0 ) 
				)
			{
				bNegative = false;
				bNegDel = true;
			}

			if(		(	(	nStart >= ( nLen - nNegSignLen )
						&&	! bBackspace
						)
					||	nEnd > ( nLen - nNegSignLen )
					||	(	nStart > ( nLen - nNegSignLen )
						&&	bBackspace
						)
					|| (	nStart == nLen
						&&	nStart == nEnd
						&&	bBackspace
						)
					)
				&&	(	m_nNegativeFormatIndex == 3
					||	m_nNegativeFormatIndex == 4
					||	m_nNegativeFormatIndex == 0
					)
				)
			{
				bNegative = false;
				bNegDel = true;
			}
		}

		_GetTextWithoutMarksBase( strText, nStart, nEnd );

		if( bBackspace )
		{
			bool bSelect = ( nStart != nEnd ) ? true : false;
			strText.Delete( ( nStart != nEnd ) ? nStart : ( nStart - 1 ), ( nStart != nEnd ) ? ( nEnd - nStart ) : ( bNegDel ? 0 : 1 ) );
			if( ! bSelect && ! bNegDel )
				nStart --;
			m_bBackspace = true;
		}
		else
		{
			INT _nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
			if( _nDecimalPointPos != -1 )
			{
				if(		nStart > _nDecimalPointPos 
					&&	nStart < ( _nDecimalPointPos + nDecimalSeparatorLen )
					)
					nStart = _nDecimalPointPos;
				if(		nEnd > _nDecimalPointPos 
					&&	nEnd < ( _nDecimalPointPos + nDecimalSeparatorLen )
					)
					nEnd = _nDecimalPointPos + nDecimalSeparatorLen;
			}
			strText.Delete( nStart, ( nStart != nEnd ) ? ( nEnd - nStart ) : ( bNegDel ? 0 : 1 ) );
		}
		_GetTextWithoutMarksBase( strText, nStart, nEnd );
		bool bPower = (lParam == 1) ? true : false;
		_GetTextWithMarksBase( strText, nStart, nEnd, bNegative, bPower );
		if( m_bBackspace && ( nStart - nGroupSeparatorLen ) >= 0 )
		{
			if( nGroupSeparatorLen > 0 )
			{
				INT nGroupSepPos = (INT)strText.Find( m_strGroupSeparator, ( nStart - nGroupSeparatorLen ) );
				if( nGroupSepPos >= 0 )
				{
					INT _nPos = 0, _nGroupSignBeforeStartAfterDelete = 0;
					bool _bChangePos = false;
					INT _nGroupSignCountAfterDelete = (INT)_GetGroupSeparatorCount( strText );
					for( ; true; )
					{
						_nPos = (INT)strText.Find( m_strGroupSeparator, _nPos );
						if( _nPos == -1 )
							break;
						if( _nPos < nStart )
							_nGroupSignBeforeStartAfterDelete ++;
						_nPos ++;
					}
					_nGroupSignBeforeStartAfterDelete = _nGroupSignCountAfterDelete - _nGroupSignBeforeStartAfterDelete;
					if( ( _nGroupSignBeforeStart - 1 ) == _nGroupSignBeforeStartAfterDelete )
						_bChangePos = true;
					if( nGroupSepPos == ( nStart - nGroupSeparatorLen ) && _bChangePos )
						nStart = nGroupSepPos;
				}
			}
		}
	}
	return true;
}

LRESULT CExtEditSystemNumber::Behavior_OnPaste( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
	if( ( GetStyle() & ES_READONLY ) != 0 )
		return 0;
INT nStart = 0, nEnd = 0;
	m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
	// paste from clipboard
	if( OpenClipboard() )
	{
		CExtSafeString strClipboardText;
		bool bHaveClipboardText = false;
		if( g_PaintManager.m_bIsWinNT4orLater && ::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		{
			HGLOBAL h = ::GetClipboardData( CF_UNICODETEXT );
			if( h != NULL )
			{
				LPWSTR strUnicodeBuffer = (LPWSTR) ::GlobalLock( h );
				if( strUnicodeBuffer != NULL )
				{
					bHaveClipboardText = true;
					USES_CONVERSION;
					LPCTSTR strBuffer = W2CT(strUnicodeBuffer);
					strClipboardText = strBuffer;
					::GlobalUnlock( h );
				}
			}
		} // if( g_PaintManager.m_bIsWinNT4orLater && ::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		if( ( ! bHaveClipboardText ) && ::IsClipboardFormatAvailable( CF_TEXT ) )
		{
			HGLOBAL h = ::GetClipboardData( CF_TEXT );
			if( h != NULL )
			{
				LPSTR strBuffer = (LPSTR) ::GlobalLock( h );
				if( strBuffer != NULL )
				{
					bHaveClipboardText = true;
					strClipboardText = LPCTSTR(strBuffer);
					::GlobalUnlock( h );
				} // if( strBuffer != NULL )
			}
		} // if( ( ! bHaveClipboardText ) && ::IsClipboardFormatAvailable( CF_TEXT ) )
		if( bHaveClipboardText )
		{
			CString strScientificSymbol = m_strScientificSymbol;
			strScientificSymbol.MakeLower();
			INT nE_PosCurrText = INT( strText.Find( strScientificSymbol ) );
			INT nE_PosClipboardText = INT( strClipboardText.Find( strScientificSymbol ) );
			strScientificSymbol.MakeUpper();
			if( nE_PosCurrText < 0 )
				nE_PosCurrText = INT( strText.Find( strScientificSymbol ) );
			if( nE_PosClipboardText < 0 )
				nE_PosClipboardText = INT( strClipboardText.Find( strScientificSymbol ) );

			if( ( nE_PosCurrText < 0 && nE_PosClipboardText < 0 ) || (! m_bUseScientificNumberMode) )
				_OnPaste( strText, strClipboardText, nStart, nEnd );
			else if( nE_PosCurrText >= 0 && nE_PosClipboardText < 0 )
			{
				bool bLeftSide = false;
				bool bRightSide = false;
				INT nStartPartLeft, nStartPartRight, nEndPartLeft, nEndPartRight;
				nStartPartLeft = nStartPartRight = nEndPartLeft = nEndPartRight = 0;
				INT _nStart = min( nStart, nEnd );
				if( _nStart <= nE_PosCurrText )
				{
					bLeftSide = true;
					nStartPartLeft = nStart;
					nEndPartLeft = nEnd;
					if( nStart < nEnd )
						nEndPartLeft = min( nEndPartLeft, nE_PosCurrText );
					else
						nStartPartLeft = min( nStartPartLeft, nE_PosCurrText );
				}
				if( _nStart > nE_PosCurrText )
				{
					bRightSide = true;
					nStartPartRight = nStart;
					nEndPartRight = nEnd;
					if( nStart < nEnd )
						nStartPartRight = max( nStartPartRight, nE_PosCurrText );
					else
						nEndPartRight = max( nEndPartRight, nE_PosCurrText );
					nStartPartRight -= nE_PosCurrText + 1;
					nEndPartRight -= nE_PosCurrText + 1;
				}
				CExtSafeString strFirstPart = strText.Mid( 0, nE_PosCurrText );
				CExtSafeString strSecondPart = strText.Mid( (nE_PosCurrText + 1), (strText.GetLength() - nE_PosCurrText) );
				if( bLeftSide )
					_OnPaste( strFirstPart, strClipboardText, nStartPartLeft, nEndPartLeft );
				else if( bRightSide )
					_OnPaste( strSecondPart, strClipboardText, nStartPartRight, nEndPartRight, true );
 				INT nFirstPartLen = strFirstPart.GetLength() + 1;
				if( bLeftSide )
				{
					nStart = nStartPartLeft;
					nEnd = nEndPartLeft;
				}
				if( bRightSide )
				{
					nStart = nStartPartRight + nFirstPartLen;
					nEnd = nEndPartRight + nFirstPartLen;
				}
				strText = strFirstPart + _T("e") + strSecondPart;
			}
			else if( nE_PosCurrText < 0 && nE_PosClipboardText >= 0 )
			{
				_OnPaste( strText, strClipboardText, nStart, nEnd, false, false );
			}
			else if( nE_PosCurrText >= 0 && nE_PosClipboardText >= 0 )
			{
				bool bLeftSide = false;
				bool bRightSide = false;
				INT nStartPartLeft, nStartPartRight, nEndPartLeft, nEndPartRight;
				nStartPartLeft = nStartPartRight = nEndPartLeft = nEndPartRight = 0;
				INT _nStart = min( nStart, nEnd );
				if( _nStart <= nE_PosCurrText )
				{
					bLeftSide = true;
					nStartPartLeft = nStart;
					nEndPartLeft = nEnd;
					if( nStart < nEnd )
						nEndPartLeft = min( nEndPartLeft, nE_PosCurrText );
					else
						nStartPartLeft = min( nStartPartLeft, nE_PosCurrText );
				}
				if( _nStart > nE_PosCurrText )
				{
					bRightSide = true;
					nStartPartRight = nStart;
					nEndPartRight = nEnd;
					if( nStart < nEnd )
						nStartPartRight = max( nStartPartRight, nE_PosCurrText );
					else
						nEndPartRight = max( nEndPartRight, nE_PosCurrText );
					nStartPartRight -= nE_PosCurrText + 1;
					nEndPartRight -= nE_PosCurrText + 1;
				}
				CExtSafeString strFirstPart = strText.Mid( 0, nE_PosCurrText );
				CExtSafeString strSecondPart = strText.Mid( (nE_PosCurrText + 1), (strText.GetLength() - nE_PosCurrText) );
				if( bLeftSide )
				{
					_OnPaste( strFirstPart, strClipboardText, nStartPartLeft, nEndPartLeft, false, false );
					strSecondPart = _T("");
				}
				else if( bRightSide )
					_OnPaste( strSecondPart, strClipboardText, nStartPartRight, nEndPartRight, true );
				INT nFirstPartLen = strFirstPart.GetLength() + 1;
				if( bLeftSide )
				{
					nStart = nStartPartLeft;
					nEnd = nEndPartLeft;
				}
				if( bRightSide )
				{
					nStart = nStartPartRight + nFirstPartLen;
					nEnd = nEndPartRight + nFirstPartLen;
				}
				strText = strFirstPart + _T("e") + strSecondPart;
			}
			m_pEditWithBehaviorWnd->TextSet( strText );
			m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
		} // if( bHaveClipboardText )
		::CloseClipboard();
	} // if( OpenClipboard() )
//	_Rich_OnDraw();
	return 0;
}

void CExtEditSystemNumber::_OnPaste( 
	CExtSafeString & strText,
	CExtSafeString strClipboardText,
	INT & nStart,
	INT & nEnd,
	bool bPower, // = false
	bool bDelScientificSignFromClipboard // = true
	) const
{
	bool bNegative = _IsNegative( strText );
	_GetTextWithoutMarks( strText, nStart, nEnd );
	INT _nStart = 0, _nEnd = 0;
	INT nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
	INT nDecimalPointLen = INT( m_strDecimalPoint.GetLength() );
	if( nDecimalPointPos != -1 || bPower )
		_GetTextWithoutMarks( strClipboardText, _nStart, _nEnd, true );
	else
		_GetTextWithoutMarks( strClipboardText, _nStart, _nEnd );
	if( strText.IsEmpty() )
		_GetTextWithoutMarks( strClipboardText, _nStart, _nEnd, true );
//	INT nClipboardTextLen = INT( strClipboardText.GetLength() );
	if( ( nEnd - nStart ) > 0 )
		strText.Delete( nStart, ( nEnd - nStart ) );
	nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
	bool bStartInFrontOfDecimalPoint = ( nStart <= nDecimalPointPos );
	strText.Insert( nStart, strClipboardText );
	nStart += strClipboardText.GetLength();
	nEnd = nStart;
//////////////////////////////////////////////////////////////////////////
	CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
	INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );

	if( (! m_bUseScientificNumberMode) || nE_Pos < 0 || bDelScientificSignFromClipboard )
	{
		_GetTextWithoutMarksBase( strText, nStart, nEnd );
		nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
		if( nDecimalPointPos > m_nMaxWholeDigits && (! bPower) )
		{
			strText.Delete( nDecimalPointPos, nDecimalPointLen );
			strText.Insert( m_nMaxWholeDigits, m_strDecimalPoint );
			nDecimalPointPos = m_nMaxWholeDigits;
		}
		if( nDecimalPointPos != -1 && (! bPower) )
			nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
		INT nMaxLen = m_nMaxWholeDigits + m_nMaxFractionDigits + ( ( nDecimalPointPos != -1 ) ? nDecimalPointLen : 0 );
		//nEnd = nStart + nClipboardTextLen;
		if(		bStartInFrontOfDecimalPoint 
			&&	nEnd > nDecimalPointPos
			)
			nEnd += nDecimalPointLen;
		if( nMaxLen < strText.GetLength() && (! bPower) )
		{
			strText.Delete( nMaxLen, ( strText.GetLength() - nMaxLen ) );
			if( nDecimalPointPos != -1 )
			{
				nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
				// check if DecimalPoint deleted
				if( nDecimalPointPos == -1 )
				{
					nMaxLen -= nDecimalPointLen;
					strText.Delete( nMaxLen, nDecimalPointLen );
				}
			}
			if( nDecimalPointPos != -1 && nDecimalPointPos > m_nMaxWholeDigits )
			{
				strText.Delete( nDecimalPointPos, nDecimalPointLen );
				strText.Insert( m_nMaxWholeDigits, m_strDecimalPoint );
				if( nEnd >= m_nMaxWholeDigits )
					nEnd += nDecimalPointLen;
			}
		}
		_GetTextWithMarksBase( strText, nStart, nEnd, bNegative, bPower );
	}
	else
	{
		INT nStartPartLeft, nStartPartRight, nEndPartLeft, nEndPartRight;
		nStartPartLeft = nStartPartRight = nEndPartLeft = nEndPartRight = 0;
		INT _nStart = min( nStart, nEnd );
		if( _nStart <= nE_Pos )
		{
			nStartPartLeft = nStart;
			nEndPartLeft = nEnd;
			if( nStart < nEnd )
				nEndPartLeft = min( nEndPartLeft, nE_Pos );
			else
				nStartPartLeft = min( nStartPartLeft, nE_Pos );
		}
		if( _nStart > nE_Pos )
		{
// 			nStartPartRight = nStart - nE_Pos - 1;
// 			nEndPartRight = nEnd - nE_Pos - 1;
// 			if( nStart < nEnd )
// 				nStartPartRight = max( nStartPartRight, nE_Pos );
// 			else
// 				nEndPartRight = max( nEndPartRight, nE_Pos );
//////////////////////////////////////////////////////////////////////////
			nStartPartRight = nStart;
			nEndPartRight = nEnd;
			if( nStart < nEnd )
				nStartPartRight = max( nStartPartRight, nE_Pos );
			else
				nEndPartRight = max( nEndPartRight, nE_Pos );
			nStartPartRight -= nE_Pos + 1;
			nEndPartRight -= nE_Pos + 1;

		}
		CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		if( ! strFirstPart.IsEmpty() )
		{
			nDecimalPointPos = INT( strFirstPart.Find( m_strDecimalPoint ) );
			if( nDecimalPointPos > m_nMaxWholeDigits )
			{
				strFirstPart.Delete( nDecimalPointPos, nDecimalPointLen );
				strFirstPart.Insert( m_nMaxWholeDigits, m_strDecimalPoint );
				nDecimalPointPos = m_nMaxWholeDigits;
			}
			if( nDecimalPointPos != -1 )
				nDecimalPointPos = INT( strFirstPart.Find( m_strDecimalPoint ) );
			INT nMaxLen = m_nMaxWholeDigits + m_nMaxFractionDigits + ( ( nDecimalPointPos != -1 ) ? nDecimalPointLen : 0 );
			//nEnd = nStart + nClipboardTextLen;
			nEndPartLeft = strFirstPart.GetLength();
			if(		bStartInFrontOfDecimalPoint
				&&	nEndPartLeft > nDecimalPointPos
				)
				nEndPartLeft += nDecimalPointLen;
			if( nMaxLen < strFirstPart.GetLength() )
			{
				strFirstPart.Delete( nMaxLen, ( strFirstPart.GetLength() - nMaxLen ) );
				if( nDecimalPointPos != -1 )
				{
					nDecimalPointPos = INT( strFirstPart.Find( m_strDecimalPoint ) );
					// check if DecimalPoint deleted
					if( nDecimalPointPos == -1 )
					{
						nMaxLen -= nDecimalPointLen;
						strFirstPart.Delete( nMaxLen, nDecimalPointLen );
					}
				}
				if( nDecimalPointPos != -1 && nDecimalPointPos > m_nMaxWholeDigits )
				{
					strFirstPart.Delete( nDecimalPointPos, nDecimalPointLen );
					strFirstPart.Insert( m_nMaxWholeDigits, m_strDecimalPoint );
					if( nEndPartLeft >= m_nMaxWholeDigits )
						nEndPartLeft += nDecimalPointLen;
				}
			}
			_GetTextWithMarksBase( strFirstPart, nStartPartLeft, nEndPartLeft, bNegative );
		}
		if( ! strSecondPart.IsEmpty() )
		{
			_GetTextWithoutMarksBase( strSecondPart, nStartPartRight, nEndPartRight, true );
			bool bNegative = _IsNegative( strSecondPart );
			_GetTextWithMarksBase( strSecondPart, nStartPartRight, nEndPartRight, bNegative, true );
			nStartPartRight += strFirstPart.GetLength() + 1;
			nEndPartRight += strFirstPart.GetLength() + 1;
		}
		if( nStart < nEnd )
		{
			nStart = min( nStartPartLeft, nStartPartRight );
			nEnd = max( nEndPartLeft, nEndPartRight );
		}
		else
		{
			nStart = max( nStartPartLeft, nStartPartRight );
			nEnd = min( nEndPartLeft, nEndPartRight );
		}
		nStart = nEnd = max( nStart, nEnd );
		strText = strFirstPart + m_strScientificSymbol + strSecondPart;
	}
}

void CExtEditSystemNumber::_GetTextWithoutMarks( 
	CExtSafeString & strText, 
	INT & nStart, 
	INT & nEnd, 
	bool bCutDecimalPoint // = false 
	) const
{
	ASSERT_VALID( this );
	CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
	INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );
	if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
		_GetTextWithoutMarksBase( strText, nStart, nEnd, bCutDecimalPoint );
	else
	{
		bool bLeftSide = false;
		bool bRightSide = false;
		INT nStartPartLeft, nStartPartRight, nEndPartLeft, nEndPartRight;
		nStartPartLeft = nStartPartRight = nEndPartLeft = nEndPartRight = 0;
		if( nStart <= nE_Pos  && nEnd <= nE_Pos )
		{
			bLeftSide = true;
			nStartPartLeft = nStart;
			nEndPartLeft = nEnd;
		}
		if( nStart <= nE_Pos  && nEnd > nE_Pos )
		{
			bLeftSide = true;
			bRightSide = true;
			nStartPartLeft = nStart;
			nEndPartLeft = nE_Pos;
			nStartPartRight = 0;
			nEndPartRight = nEnd - nE_Pos - 1;
		}
		if( nStart > nE_Pos  && nEnd <= nE_Pos )
		{
			bLeftSide = true;
			bRightSide = true;
			nStartPartLeft = nE_Pos;
			nEndPartLeft = nEnd;
			nStartPartRight = 0;
			nEndPartRight = nStart - nE_Pos - 1;
		}
		if( nStart > nE_Pos  && nEnd > nE_Pos )
		{
			bRightSide = true;
			nStartPartRight = nStart - nE_Pos - 1;
			nEndPartRight = nEnd - nE_Pos - 1;
		}
		CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		_GetTextWithoutMarksBase( strFirstPart, nStartPartLeft, nEndPartLeft, bCutDecimalPoint );
		_GetTextWithoutMarksBase( strSecondPart, nStartPartRight, nEndPartRight, true );
		if( bLeftSide && (! bRightSide) )
		{
			nStart = nStartPartLeft;
			nEnd = nEndPartLeft;
		}
		if( bRightSide && (! bLeftSide) )
		{
			nStart = nStartPartRight + nE_Pos + 1;
			nEnd = nEndPartRight + nE_Pos + 1;
		}
		if( bLeftSide && bRightSide )
		{
			if( nStart < nEnd )
			{
				nStart = nStartPartLeft;
				nEnd = nEndPartRight + nE_Pos + 1;
			}
			else
			{
				nStart = nStartPartRight + nE_Pos + 1;
				nEnd = nEndPartLeft;
			}
		}
		strText = strFirstPart + _T("e") + strSecondPart;
	}
}

void CExtEditSystemNumber::_GetTextWithoutMarksBase( 
	CExtSafeString & strText, 
	INT & nStart, 
	INT & nEnd, 
	bool bCutDecimalPoint // = false 
	) const
{
	ASSERT_VALID( this );
CExtSafeString strCurrentText;
INT nStartPos = nStart, nEndPos = nEnd;
INT nSignCountBeforeStartPos, nSignCountBeforeEndPos, nSignCount;
INT nStrLen = INT( strText.GetLength() );
INT nDecimalSeparatorLen = INT( m_strDecimalPoint.GetLength() );
INT nDecimalSeparatorPos = INT( strText.Find( m_strDecimalPoint ) );
	nSignCount = 0;
	if( nEndPos > strText.GetLength() )
		nEndPos = INT( strText.GetLength() );
	nSignCountBeforeStartPos = nSignCountBeforeEndPos = nSignCount = 0;
	for( INT i = 0; i < nStrLen; i++ )
	{
		TCHAR _tchr = strText[i];
		bool bUseSymbol = false;
		if( _T('0') <= _tchr && _tchr <= _T('9') ) //  || ( _tchr == m_strDecimalPoint && ! bCutDecimalPoint ) 
		{
			strCurrentText += _tchr;
			bUseSymbol = true;
		}
		if( i >= nDecimalSeparatorPos && i < ( nDecimalSeparatorPos + nDecimalSeparatorLen ) && ! bCutDecimalPoint && nDecimalSeparatorPos != -1 )
		{
			strCurrentText += _tchr;
			bUseSymbol = true;
		}
		if( ! bUseSymbol )
			nSignCount++;
		if( i == ( nStartPos - 1 ) )
			nSignCountBeforeStartPos = nSignCount;
		else if( i == ( nEndPos - 1 ) )
			nSignCountBeforeEndPos = nSignCount;
	}
	nStartPos -= nSignCountBeforeStartPos;
	nEndPos -= nSignCountBeforeEndPos;
	if( nStartPos < 0 )
		nStartPos = 0;
	if( nEndPos < 0 )
		nEndPos = 0;
	strText = strCurrentText;
	if( nStart != nEnd )
	{
		nStart = nStartPos;
		nEnd = nEndPos;
	}
	else
		nStart = nEnd = nStartPos;
}


void CExtEditSystemNumber::_GetTextWithMarks( 
	 CExtSafeString & strText, 
	 INT & nStart, 
	 INT & nEnd, 
	 bool bInsertNegSign  // = false
	 ) const
{
	ASSERT_VALID( this );
	if( strText.IsEmpty() )
		return;
	CString strScientificSymbol = m_strScientificSymbol;
	strScientificSymbol.MakeLower();
	INT nE_Pos = INT( strText.Find( strScientificSymbol ) );
	strScientificSymbol.MakeUpper();
	if( nE_Pos < 0 )
		nE_Pos = INT( strText.Find( strScientificSymbol ) );
	if( nE_Pos < 0 || (! m_bUseScientificNumberMode) )
		_GetTextWithMarksBase( strText, nStart, nEnd, bInsertNegSign );
	else
	{
		bool bLeftSide = false;
		bool bRightSide = false;
		INT nStartPartLeft, nStartPartRight, nEndPartLeft, nEndPartRight;
		nStartPartLeft = nStartPartRight = nEndPartLeft = nEndPartRight = 0;
		if( nStart <= nE_Pos  && nEnd <= nE_Pos )
		{
			bLeftSide = true;
			nStartPartLeft = nStart;
			nEndPartLeft = nEnd;
		}
		if( nStart <= nE_Pos  && nEnd > nE_Pos )
		{
			bLeftSide = true;
			bRightSide = true;
			nStartPartLeft = nStart;
			nEndPartLeft = nE_Pos;
			nStartPartRight = 0;
			nEndPartRight = nEnd - nE_Pos - 1;
		}
		if( nStart > nE_Pos  && nEnd <= nE_Pos )
		{
			bLeftSide = true;
			bRightSide = true;
			nStartPartLeft = nE_Pos;
			nEndPartLeft = nEnd;
			nStartPartRight = 0;
			nEndPartRight = nStart - nE_Pos - 1;
		}
		if( nStart > nE_Pos  && nEnd > nE_Pos )
		{
			bRightSide = true;
			nStartPartRight = nStart - nE_Pos - 1;
			nEndPartRight = nEnd - nE_Pos - 1;
		}
		CExtSafeString strFirstPart = strText.Mid( 0, nE_Pos );
		CExtSafeString strSecondPart = strText.Mid( (nE_Pos + 1), (strText.GetLength() - nE_Pos) );
		_GetTextWithMarksBase( strFirstPart, nStartPartLeft, nEndPartLeft, bInsertNegSign );
		_GetTextWithMarksBase( strSecondPart, nStartPartRight, nEndPartRight, bInsertNegSign, true );
		if( bLeftSide && (! bRightSide) )
		{
			nStart = nStartPartLeft;
			nEnd = nEndPartLeft;
		}
		if( bRightSide && (! bLeftSide) )
		{
			nStart = nStartPartRight + nE_Pos + 1;
			nEnd = nEndPartRight + nE_Pos + 1;
		}
		if( bLeftSide && bRightSide )
		{
			if( nStart < nEnd )
			{
				nStart = nStartPartLeft;
				nEnd = nEndPartRight + nE_Pos + 1;
			}
			else
			{
				nStart = nStartPartRight + nE_Pos + 1;
				nEnd = nEndPartLeft;
			}
		}
		strText = strFirstPart + _T("e") + strSecondPart;
	}
}

void CExtEditSystemNumber::_GetTextWithMarksBase( 
	CExtSafeString & strText, 
	INT & nStart, 
	INT & nEnd, 
	bool bInsertNegSign,  // = false
	bool bPower // = false
	) const
{
	ASSERT_VALID( this );
	//if( strText.IsEmpty() )
	//	return;
INT nTextLen = INT( strText.GetLength() );
CExtSafeString strTextChange;
INT nGroupSeparatorLen = m_bEmptyGroupSeparatorMode ? 0 : INT( m_strGroupSeparator.GetLength() );
INT nDecimalSeparatorLen = INT( m_strDecimalPoint.GetLength() );
INT nPositionDecimalPoint = INT( strText.Find( m_strDecimalPoint ) );
INT nMaxLen = m_nMaxWholeDigits + m_nMaxFractionDigits + ( ( nPositionDecimalPoint >= 0 ) ? nDecimalSeparatorLen : 0 );
	if( bPower )
		nMaxLen += nTextLen;
	if( nTextLen > nMaxLen )
	{
		if( nPositionDecimalPoint >= 0 )
		{
			CExtSafeString _strDecimalPoint = _T("");
			_strDecimalPoint = m_strDecimalPoint;
			strText.Replace( _strDecimalPoint, _T("") );
			if( nPositionDecimalPoint > m_nMaxWholeDigits )
				strText.Insert( m_nMaxWholeDigits, m_strDecimalPoint );
			else
				strText.Insert( nPositionDecimalPoint, m_strDecimalPoint );
		}
		nTextLen = INT( strText.GetLength() );
	}
INT nMaxWholeDigits = m_nMaxWholeDigits;
INT nMaxFractionDigits = m_nMaxFractionDigits;
	if( bPower )
		nMaxFractionDigits = nTextLen + 1;
	if( bPower /*&& nMaxWholeDigits < nTextLen && nPositionDecimalPoint < 0*/ )
		nMaxWholeDigits = nTextLen + 1;
INT nWholeNumber = ( ( nPositionDecimalPoint >= 0 ) ? ( ( nPositionDecimalPoint < nMaxWholeDigits ) ? nPositionDecimalPoint : nMaxWholeDigits ) : ( ( nMaxWholeDigits >= nTextLen ) ?  nTextLen : nMaxWholeDigits ) );
INT nDecimalPointLen = nDecimalSeparatorLen; // 1
INT nCorectTextLen = nWholeNumber + nDecimalPointLen + nMaxFractionDigits;
	nCorectTextLen = ( nCorectTextLen > nTextLen ) ? nTextLen : nCorectTextLen;
INT nCountDigitGroups = INT( m_listDigitGrouping.GetCount() );
CList < INT, INT > _listPos;
bool bLastZiro = ( m_listDigitGrouping.GetAt( m_listDigitGrouping.FindIndex( nCountDigitGroups - 1 ) ) == 0 ) ? true : false;
	ASSERT( nWholeNumber <= nMaxWholeDigits );
INT nPos = nWholeNumber;
INT i = 0;
INT nPreviousCount = -1;
	for( ; true; )
	{
		INT nCount = INT( m_listDigitGrouping.GetAt( m_listDigitGrouping.FindIndex(i) ) );
		if( nCount == 0 && i < ( nCountDigitGroups - 1 ) )
			break;
		if( nCount == 0 && i == ( nCountDigitGroups - 1 ) )
		{
			ASSERT( nPreviousCount != -1 );
			nCount = nPreviousCount;
		}
		nPos -= nCount;
		if( nPos > 0 )
			_listPos.AddHead( nPos - 1 );
		else
			break;
		if( i == ( nCountDigitGroups - 1 ) && ! bLastZiro )
			break;
		if( i < ( nCountDigitGroups - 1 ) )
			i ++;
		nPreviousCount = nCount;
	}
INT nSignCountBeforeStart = 0;
INT nSignCountBeforeEnd = 0;
bool bInsertedDecimalSignBeforeStart = false;
bool bInsertedDecimalSignBeforeEnd = false;
INT nCurDigitPos = 0;
INT nPosCount = INT( _listPos.GetCount() );
bool bCanPastGroupSep = ( nPosCount > 0 );
	for( i = 0; i < nCorectTextLen; i++ )
	{
		TCHAR _tchr = strText[i];
		strTextChange += _tchr;
		bool bInsertSign = false;
		INT nPosCur = 0;
		if( bCanPastGroupSep )
			nPosCur = _listPos.GetAt( _listPos.FindIndex( nCurDigitPos ) );
		if(		( i <= nWholeNumber )
			&&	( i != ( nCorectTextLen - 1 ) )
			&&	( i == nPosCur )
			&&	bCanPastGroupSep
			)
		{
			if( ! m_bEmptyGroupSeparatorMode )
				strTextChange += m_strGroupSeparator;
			bInsertSign = true;
			if( nCurDigitPos < ( nPosCount - 1 ) )
				nCurDigitPos ++;
			else
				bCanPastGroupSep = false;
		}
		if( i == ( nWholeNumber - 1 ) && ( nPositionDecimalPoint == -1 ) && ( i != nCorectTextLen - 1 ) )
		{
			strTextChange += m_strDecimalPoint;
			if( i < ( nStart - 1 ) )
				bInsertedDecimalSignBeforeStart  = true;
			if( i < ( nEnd - 1 ) )
				bInsertedDecimalSignBeforeEnd = true;
		}
		if( bInsertSign )
		{
			if( i < nStart )
				nSignCountBeforeStart ++;
			if( i < nEnd )
				nSignCountBeforeEnd ++;
		}
	}
	if( bInsertNegSign && AllowNegativeGet() )
	{
		INT nNegSignLen = INT( m_strNegativeSign.GetLength() ) + ( ( m_nNegativeFormatIndex == 2 || m_nNegativeFormatIndex == 4 ) ? 1: 0 );
		CExtSafeString _strNegativeSign = ( (m_nNegativeFormatIndex == 4) ? _T(" "): _T("") ) + m_strNegativeSign + ( (m_nNegativeFormatIndex == 2) ? _T(" "): _T("") );
		if( m_nNegativeFormatIndex == 0 )
		{
			strTextChange.Insert( strTextChange.GetLength(), _T(")") );
			strTextChange.Insert( 0, _T("(") );
			nStart += 1;
			nEnd += 1;
		}
		if( m_nNegativeFormatIndex == 1 || m_nNegativeFormatIndex == 2 )
			strTextChange.Insert( 0, _strNegativeSign );
		if( m_nNegativeFormatIndex == 3 || m_nNegativeFormatIndex == 4 )
			strTextChange.Insert( strTextChange.GetLength(), _strNegativeSign );
		if( m_nNegativeFormatIndex == 1 || m_nNegativeFormatIndex == 2 )
		{
			nStart += nNegSignLen;
			nEnd += nNegSignLen;
		}
	}
	strText = strTextChange;
	nStart += nGroupSeparatorLen * nSignCountBeforeStart + ( bInsertedDecimalSignBeforeStart ? nDecimalSeparatorLen : 0 );
	nEnd += nGroupSeparatorLen * nSignCountBeforeEnd + ( bInsertedDecimalSignBeforeEnd ? nDecimalSeparatorLen : 0 );
}

bool CExtEditSystemNumber::_IsNegative( __EXT_MFC_SAFE_LPCTSTR strText ) const
{
	ASSERT_VALID( this );
CExtSafeString _strText = strText;
int nNegSignPos = _strText.Find( m_strNegativeSign );
bool bRetVal = ( ( m_bUseScientificNumberMode ? ( nNegSignPos == 0 ) : ( nNegSignPos >= 0 ) ) ? true: false );
	if( m_nNegativeFormatIndex == 0 )
		bRetVal = ( ( _strText.Find( _T('(') ) >= 0 ) ? true: false );
	return bRetVal;
}

void CExtEditSystemNumber::_AdjustWithinRange()
{
	ASSERT_VALID( this );
	if( m_bRichPainting )
		return;
	if( _IsValid() )
		return;
	if( TextGet().IsEmpty() )
		SetWindowText( _T(" ") );
	else
		_Redraw();
long double dValue = LongDoubleGetMantissa();
    if( dValue < m_lfMin )
		LongDoubleSetMantissa( m_lfMin );
    else if( dValue > m_lfMax )
		LongDoubleSetMantissa( m_lfMax );
//CExtSafeString _strText = TextGet();
CExtSafeString _strText = _GetDoubleText( LongDoubleGetMantissa(), true );
bool bNegative = _IsNegative( _strText );
INT nNegSignPos = -1;
INT nLen = INT( _strText.GetLength() );
	if( bNegative )
	{
		if( m_bUseScientificNumberMode )
			nNegSignPos = INT( _strText.Find( _T("-") ) );
		else
		{
			switch( m_nNegativeFormatIndex )
			{
			case 0:
				nNegSignPos = INT( _strText.Find( _T(")") ) );
				break;
			case 3:
				nNegSignPos = INT( _strText.Find( m_strNegativeSign ) );
				break;
			case 4:
				nNegSignPos = INT( _strText.Find( _T(" ") + m_strNegativeSign ) );
				break;
			}
		}
	}
	_Rich_OnDraw();
	if( ! m_bRichPainting )
	{
		if( nNegSignPos >= 0 )
			SetSel( nNegSignPos, nNegSignPos );
		else
			SetSel( nLen, nLen );
	}
}

bool CExtEditSystemNumber::_IsValid() const
{
	ASSERT_VALID( this );
	long double lfValue = LongDoubleGetMantissa();
	bool bRetVal = ( m_lfMin <= lfValue && lfValue <= m_lfMax ) ? true : false;
	return bRetVal;
}

void CExtEditSystemNumber::_Rich_OnDraw()
{
	ASSERT_VALID( this );
	if( ! m_bRichMode )
		return;
	if( GetSafeHwnd() == NULL )
		return;
	if( m_bRichPainting )
		return;
	m_bRichPainting = true;
CHARRANGE crRestore;
	Rich_GetSel( crRestore );
CHARFORMAT cfDefault;
	::memset( &cfDefault, 0, sizeof(CHARFORMAT) );
	cfDefault.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_STRIKEOUT|CFM_FACE|CFM_SIZE|CFM_CHARSET|CFM_PROTECTED|CFM_COLOR;
	cfDefault.bCharSet = DEFAULT_CHARSET;
	cfDefault.dwEffects = CFE_PROTECTED; 
	//cfDefault.yHeight = 180;
	//cfDefault.crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
CFont * pFont = GetFont();
	if( pFont->GetSafeHandle() == NULL )
	{
		CWnd * pWndParent = GetParent();
		if( pWndParent->GetSafeHwnd() != NULL )
			pFont = pWndParent->GetFont();
		if( pFont->GetSafeHandle() == NULL )
			pFont = &(PmBridge_GetPM()->m_FontNormal);
	}
LOGFONT _lf; ::memset( &_lf, 0, sizeof(LOGFONT) );
	pFont->GetLogFont( &_lf );
#if (! defined _RICHEDIT_VER )
	USES_CONVERSION;
	strcpy( cfDefault.szFaceName, T2CA(_lf.lfFaceName) );
#else
#if ( _RICHEDIT_VER < 0x0200 )
	USES_CONVERSION;
	strcpy( cfDefault.szFaceName, T2CA(_lf.lfFaceName) );
#else
	__EXT_MFC_STRCPY( cfDefault.szFaceName, sizeof(cfDefault.szFaceName)/sizeof(cfDefault.szFaceName[0]), _lf.lfFaceName );
#endif
#endif
CWindowDC dcDesktop( NULL );
	cfDefault.yHeight = ::MulDiv( _lf.lfHeight, 72 * 20, dcDesktop.GetDeviceCaps( LOGPIXELSY ) );
	cfDefault.yHeight = abs( cfDefault.yHeight );
bool bEnabled = IsWindowEnabled() ? true : false, bReadOnly = ( ( GetStyle() & ES_READONLY ) != 0 );
	cfDefault.crTextColor = OnQueryTextColor();
	if( cfDefault.crTextColor == COLORREF(-1L) )
	{
		cfDefault.crTextColor = bEnabled ? ( bReadOnly ? m_clrRichTextReadOnly : m_clrRichTextNormal ) : m_clrRichTextDisabled;
		if( cfDefault.crTextColor == COLORREF(-1L) )
		{
			if( bEnabled )
			{
				if( bReadOnly )
					cfDefault.crTextColor = ::GetSysColor( COLOR_3DSHADOW );
				else
					cfDefault.crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
			}
			else
				cfDefault.crTextColor = ::GetSysColor( COLOR_GRAYTEXT );
		}
	}
COLORREF clrBack = OnQueryBackColor();
	if( clrBack == COLORREF(-1L) )
	{
		clrBack = bEnabled ? ( bReadOnly ? m_clrRichBackReadOnly : m_clrRichBackNormal ) : m_clrRichBackDisabled;
		if( clrBack == COLORREF(-1L) )
		{
			if( bEnabled )
			{
				if( bReadOnly )
					clrBack = ::GetSysColor( COLOR_3DFACE );
				else
					clrBack = ::GetSysColor( COLOR_WINDOW );
			}
			else
				clrBack = ::GetSysColor( COLOR_3DFACE );
		}
	}
	Rich_SetBackgroundColor( FALSE, clrBack );
	Rich_HideSelection( true, false );
	Rich_SetSel( 0, -1 ); 
	SendMessage( EM_SETCHARFORMAT, SCF_DEFAULT, LPARAM(&cfDefault) );
	SendMessage( EM_SETCHARFORMAT, SCF_SELECTION, LPARAM(&cfDefault) );
	Rich_SetSelectionColor( cfDefault.crTextColor );
INT nGroupSeparatorLen = m_bEmptyGroupSeparatorMode ? 0 : INT( m_strGroupSeparator.GetLength() );
INT nDecimalSignLen = INT( m_strDecimalPoint.GetLength() );
COLORREF clrSign = m_clrSign;
	if( clrSign == COLORREF(-1L) )
		clrSign = cfDefault.crTextColor;
	if(		m_clrNumber != COLORREF(-1L)
		||	m_clrFraction != COLORREF(-1L)
		||	m_clrGroupSeparator != COLORREF(-1L)
		||	m_clrDecimalSeparator != COLORREF(-1L)
		||	m_clrBrackets != COLORREF(-1L)
		||	m_clrPower != COLORREF(-1L)
		||	m_clrScientificSign != COLORREF(-1L)
		||	clrSign != COLORREF(-1L)
		)
	{
		CExtSafeString strText = TextGet();
		INT nTextLen = INT( strText.GetLength() );
		if( nTextLen > 0 )
		{
			bool bNegative = _IsNegative( strText );
			INT nPositionDecimalPoint = INT( strText.Find( m_strDecimalPoint ) );
			INT nPositionScientificSymbol = INT( strText.Find( m_strScientificSymbol ) );

			CExtSafeString _strBefore = _T(""), _strAfter = _T("");
			if( bNegative )
			{
				switch( m_nNegativeFormatIndex )
				{
				case 0:
					_strBefore = _T("(");
					_strAfter = _T(")");
				break;
				case 1:
					_strBefore = m_strNegativeSign;
				break;
				case 2:
					_strBefore = m_strNegativeSign + _T(" ");
				break;
				case 3:
					_strAfter = m_strNegativeSign;
				break;
				case 4:
					_strAfter = _T(" ") + m_strNegativeSign;
				break;
				}
			}

			INT nBeforLen = _strBefore.GetLength();
			INT nAfterLen = _strAfter.GetLength();
			// Whole number
			if( m_clrNumber != COLORREF(-1L) )
			{
				Rich_SetSel( nBeforLen, ( nPositionDecimalPoint != -1 ) ? nPositionDecimalPoint : ( nTextLen - nAfterLen ) );
				Rich_SetSelectionColor( m_clrNumber );
			}

			if( nPositionDecimalPoint != -1 )
			{
				if( m_clrDecimalSeparator != COLORREF(-1L) )
				{
					Rich_SetSel( nPositionDecimalPoint, ( nPositionDecimalPoint + nDecimalSignLen ) );
					Rich_SetSelectionColor( m_clrDecimalSeparator );
				}
				if( m_clrFraction != COLORREF(-1L) )
				{
					Rich_SetSel( nPositionDecimalPoint + nDecimalSignLen, nTextLen - nAfterLen );
					Rich_SetSelectionColor( m_clrFraction );
				}
			}

			if( bNegative && clrSign != COLORREF(-1L) )
			{
				if(		( _strAfter.IsEmpty() || _strBefore.IsEmpty() )
					&&	( ! _strAfter.IsEmpty() || !_strBefore.IsEmpty() )
					)
				{
					INT nNegSignPos = INT( strText.Find( m_strNegativeSign ) );
					INT nNegSignLen = INT( m_strNegativeSign.GetLength() );
					if( clrSign != COLORREF(-1L) )
					{
						Rich_SetSel( nNegSignPos, ( nNegSignPos + nNegSignLen ) );
						Rich_SetSelectionColor( clrSign );
					}
				}
				if( ! _strAfter.IsEmpty() && ! _strBefore.IsEmpty() )
				{
					if( m_clrBrackets != COLORREF(-1L) )
					{
						INT i, nPos = 0;
						CString _strBrackets = _T("()[]{}");
						INT nBrackLen = _strBrackets.GetLength();
						for( i = 0; i < nBrackLen; i++ )
						{
							nPos = 0;
							for( ; true; )
							{
								nPos = strText.Find( _strBrackets[i], nPos );
								if( nPos == -1 )
									break;
								Rich_SetSel( nPos, ( nPos + 1 ) );
								Rich_SetSelectionColor( m_clrBrackets );
								nPos ++;
							}
						}
					}
				}
			}

			if( nPositionScientificSymbol >= 0 )
			{
				Rich_SetSel( nPositionScientificSymbol, ( nPositionScientificSymbol + m_strScientificSymbol.GetLength() ) );
				if( m_clrScientificSign != COLORREF(-1L) )
					Rich_SetSelectionColor( m_clrScientificSign );
				else
					Rich_SetSelectionColor( cfDefault.crTextColor );
				INT nPositionPower = nPositionScientificSymbol + m_strScientificSymbol.GetLength();
				INT nPositionMinus = INT( strText.Find( _T("-"), ( nPositionScientificSymbol + m_strScientificSymbol.GetLength() ) ) );
				if( nPositionMinus >= 0 )
				{
					Rich_SetSel( nPositionMinus, ( nPositionMinus + 1 ) );
					if( m_clrSign != COLORREF(-1L) )
						Rich_SetSelectionColor( m_clrSign );
					else
						Rich_SetSelectionColor( cfDefault.crTextColor );
					nPositionPower += 1;
				}
				Rich_SetSel( nPositionPower, strText.GetLength() );
				if( m_clrPower != COLORREF(-1L) )
					Rich_SetSelectionColor( m_clrPower );
				else
					Rich_SetSelectionColor( cfDefault.crTextColor );
			}

			if( nGroupSeparatorLen > 0 && m_clrGroupSeparator != COLORREF(-1L) )
			{
				INT nPos = 0;
				for( ; true; )
				{
					nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
					if( nPos == -1 )
						break;
					Rich_SetSel( nPos, ( nPos + nGroupSeparatorLen ) );
					Rich_SetSelectionColor( m_clrGroupSeparator );
					nPos ++;
				}
			}
		} // if( nTextLen > 0 )
	}
	Rich_SetSel( crRestore );
	Rich_HideSelection( false, false );
	m_bRichPainting = false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditSystemCurrency window

IMPLEMENT_DYNCREATE( CExtEditSystemCurrency, CExtEditSystemNumberCurrencyBase );

CExtEditSystemCurrency::CExtEditSystemCurrency(
	INT nMaxWholeDigits, // = __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS
	INT nMaxFractionDigits // = __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS
	)
	: CExtEditSystemNumberCurrencyBase( nMaxWholeDigits, nMaxFractionDigits )
{
	_Init();
}

CExtEditSystemCurrency::~CExtEditSystemCurrency()
{
}

BEGIN_MESSAGE_MAP( CExtEditSystemCurrency, CExtEditSystemNumberCurrencyBase )
	//{{AFX_MSG_MAP(CExtEditSystemNumberCurrencyBase)
	//}}AFX_MSG_MAP
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

void CExtEditSystemCurrency::_Init()
{
	ASSERT_VALID( this );
	// negative sign
	if( g_ResourceManager->GetLocaleInfo( LOCALE_SNEGATIVESIGN, NULL, 0 ) )
	{
		g_ResourceManager->GetLocaleInfo( LOCALE_SNEGATIVESIGN, m_strNegativeSign.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
		m_strNegativeSign.ReleaseBuffer();
	}
	if( g_ResourceManager->GetLocaleInfo( LOCALE_SCURRENCY, NULL, 0 ) )
	{
		g_ResourceManager->GetLocaleInfo( LOCALE_SCURRENCY, m_strCurrencySymbol.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
		m_strCurrencySymbol.ReleaseBuffer();
	}
	// decimal point
	if( g_ResourceManager->GetLocaleInfo( LOCALE_SMONDECIMALSEP, NULL, 0 ) )
	{
		g_ResourceManager->GetLocaleInfo( LOCALE_SMONDECIMALSEP, m_strDecimalPoint.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
		m_strDecimalPoint.ReleaseBuffer();
	}
	// group separator
	if( m_bEmptyGroupSeparatorMode )
		m_strGroupSeparator.Empty();
	else
	{
		if( g_ResourceManager->GetLocaleInfo( LOCALE_SMONTHOUSANDSEP, NULL, 0 ) )
		{
			g_ResourceManager->GetLocaleInfo( LOCALE_SMONTHOUSANDSEP, m_strGroupSeparator.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
			m_strGroupSeparator.ReleaseBuffer();
		}
	}
TCHAR cPosCurFormatIndex = _T('0');
	if( g_ResourceManager->GetLocaleInfo( LOCALE_ICURRENCY, NULL, 0 ) )
		g_ResourceManager->GetLocaleInfo( LOCALE_ICURRENCY, &cPosCurFormatIndex, sizeof( cPosCurFormatIndex ) / sizeof(TCHAR) );
	m_nPositiveFormatIndex = _ttoi( & cPosCurFormatIndex );
CExtSafeString strNegCurFormatIndex = _T("0");
	if( g_ResourceManager->GetLocaleInfo( LOCALE_INEGCURR, NULL, 0 ) )
	{
		g_ResourceManager->GetLocaleInfo( LOCALE_INEGCURR, strNegCurFormatIndex.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
		strNegCurFormatIndex.ReleaseBuffer();
	}
CExtSafeString strDigitGrouping = _T("0");
	if( g_ResourceManager->GetLocaleInfo( LOCALE_SMONGROUPING, NULL, 0 ) )
	{
		g_ResourceManager->GetLocaleInfo( LOCALE_SMONGROUPING, strDigitGrouping.GetBuffer( _MAX_PATH + 1 ), _MAX_PATH );
		strDigitGrouping.ReleaseBuffer();
		INT nStrLen = INT( strDigitGrouping.GetLength() );
		CExtSafeString _strGroup = _T("");
		INT nGroup = 0;
		if( m_listDigitGrouping.GetCount() > 0 )
			m_listDigitGrouping.RemoveAll();
		for( INT i = 0; i < nStrLen; i++ )
		{
			TCHAR ch = strDigitGrouping[i];
			if( ch == _T(';') )
			{
				nGroup = _ttoi( _strGroup );
				m_listDigitGrouping.AddTail( nGroup );
				_strGroup = _T("");
				nGroup = 0;
			}
			else
				_strGroup = _strGroup + ch;
		}
		nGroup = _ttoi( _strGroup );
		m_listDigitGrouping.AddTail( nGroup );
	}
	m_nNegativeFormatIndex = _ttoi( strNegCurFormatIndex );
	TCHAR cNoOfDigitsAfterDecimal = _T('0');
	if( g_ResourceManager->GetLocaleInfo( LOCALE_ICURRDIGITS, NULL, 0 ) )
		g_ResourceManager->GetLocaleInfo( LOCALE_ICURRDIGITS, &cNoOfDigitsAfterDecimal, sizeof( cNoOfDigitsAfterDecimal ) / sizeof(TCHAR) );
	m_nMaxFractionDigits = _ttoi( & cNoOfDigitsAfterDecimal );
	TCHAR cDisplayLeadingZeros = _T('0');
	if( g_ResourceManager->GetLocaleInfo( LOCALE_ILZERO, NULL, 0 ) )
		g_ResourceManager->GetLocaleInfo( LOCALE_ILZERO, &cDisplayLeadingZeros, sizeof( cDisplayLeadingZeros ) / sizeof(TCHAR) );
	m_nDisplayLeadingZeros = _ttoi( & cDisplayLeadingZeros );
	TCHAR cDigitGrouping[20] = _T("");
	if( g_ResourceManager->GetLocaleInfo( LOCALE_SGROUPING, NULL, 0 ) )
		g_ResourceManager->GetLocaleInfo( LOCALE_SGROUPING, cDigitGrouping, sizeof( cDigitGrouping ) / sizeof( cDigitGrouping[0] ) );
	// ensure the separators are not the same
	if( (! m_bEmptyGroupSeparatorMode  ) && m_strDecimalPoint == m_strGroupSeparator )
		m_strGroupSeparator = ( m_strDecimalPoint == _T(",") ? _T(".") : _T(",") );
}

void CExtEditSystemCurrency::OnSettingChange( UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection )
{
	ASSERT_VALID( this );
	CExtEditSystemNumberCurrencyBase::OnSettingChange( uFlags, lpszSection );
	if( LPCTSTR(lpszSection) != NULL && _tcscmp( LPCTSTR(lpszSection), _T("intl") ) == 0 )
	{
		long double lfSaved = LongDoubleGet();
		_Init();
		LongDoubleSet( lfSaved );
		SetSel( -1, -1 );
	}
}

CExtSafeString CExtEditSystemCurrency::_GetNumericText(
	const CExtSafeString & strText, 
	bool bUseMathSymbols // = false
	) const
{
	ASSERT_VALID( this );
CExtSafeString strNewText;
bool bIsNegative = _IsNegative( strText );
bool bHasDecimalPoint = false;
INT nDecimalSignLen = INT( m_strDecimalPoint.GetLength() );
INT nDecimalSignPos = INT( strText.Find( m_strDecimalPoint ) );
INT nPos, nLen;
	for( nPos = 0, nLen = INT( strText.GetLength() ); nPos < nLen; nPos++ )
	{
		TCHAR c = strText[nPos];
		if( _istdigit(c) )
			strNewText += c;
		else if( nPos == nDecimalSignPos && !bHasDecimalPoint )
		{
			bHasDecimalPoint = true;
			strNewText += ( bUseMathSymbols ? _T(".") : m_strDecimalPoint );
			nPos += nDecimalSignLen - 1;
		}
	}
	if( bUseMathSymbols )
	{
		if( bIsNegative )
			strNewText.Insert( 0, _T("-") );
	}
	else
	{
		CExtSafeString _strBefore = _T("");
		CExtSafeString _strAfter = _T("");
		_GetSignStrings( _strBefore, _strAfter, bIsNegative && AllowNegativeGet() );
		if( ! _strAfter.IsEmpty() )
			strNewText.Insert( strNewText.GetLength(), _strAfter );
		if( ! _strBefore.IsEmpty() )
			strNewText.Insert( 0, _strBefore );
	}
	return strNewText;
}

CExtSafeString CExtEditSystemCurrency::_GetDoubleText( long double lfValue, bool bTrimTrailingZeros /*= true*/ ) const
{
	ASSERT_VALID( this );
CExtSafeString strText;
	strText.Format( _T("%lf"), lfValue );
	if( m_strDecimalPoint != _T(".") )
		strText.Replace( _T("."), m_strDecimalPoint );
	if( bTrimTrailingZeros )
	{
		strText.TrimRight( _T("0") );
		strText.TrimRight( m_strDecimalPoint );
	}
bool bNegative = ( strText.Find( _T("-") ) == 0 ) ? true : false;
	if( bNegative )
		strText.Delete( 0 );
CExtSafeString _strBefore = _T("");
CExtSafeString _strAfter = _T("");
	_GetSignStrings( _strBefore, _strAfter, bNegative && AllowNegativeGet() );
INT nLen = INT( strText.GetLength() );
	if( ! _strAfter.IsEmpty() )
		strText.Insert( nLen, _strAfter );
	if( ! _strBefore.IsEmpty() )
		strText.Insert( 0, _strBefore );
	return strText;
}

void CExtEditSystemCurrency::IntSet( INT nVal )
{
	ASSERT_VALID( this );
CExtSafeString strText;
	strText.Format( _T("%d"), nVal );
bool bNegative = ( strText.Find( _T("-") ) == 0 ) ? true : false;
	strText.Delete( 0 );
CExtSafeString _strBefore = _T("");
CExtSafeString _strAfter = _T("");
	_GetSignStrings( _strBefore, _strAfter, bNegative && AllowNegativeGet() );
INT nLen = INT( strText.GetLength() );
	if( ! _strAfter.IsEmpty() )
		strText.Insert( nLen, _strAfter );
	if( ! _strBefore.IsEmpty() )
		strText.Insert( 0, _strBefore );
	m_pEditWithBehaviorWnd->SetWindowText(strText);
}

long double CExtEditSystemCurrency::LongDoubleGet() const
{
	ASSERT_VALID( this );
//	return _tcstod( _GetNumericText( m_pEditWithBehaviorWnd->TextGet(), true ), NULL );
CExtSafeString str = _GetNumericText( m_pEditWithBehaviorWnd->TextGet(), true );
	if(		str.IsEmpty()
	   ||	str == _T(".")
	   ||	str == LPCTSTR(m_strDecimalPoint)
	   ||	( str.GetLength() == 1 && m_strDecimalPoint.GetLength() > 0 && str[0] == m_strDecimalPoint[0] )
	   )
	   return 0.0;
LCID _LCID = LANG_USER_DEFAULT;
	if( g_ResourceManager->IsCustomLangAllowed() )
		_LCID = g_ResourceManager->GetLangIdDesired();
COleCurrency _oleCurrency;
	VERIFY( _oleCurrency.ParseCurrency( LPCTSTR(str), 0, _LCID ) );
long double lfVal = ((long double)_oleCurrency.m_cur.int64) / ((long double)10000.0);
	return lfVal;
}

INT CExtEditSystemCurrency::IntGet() const
{
	ASSERT_VALID( this );
//	return (INT)_ttoi( _GetNumericText( m_pEditWithBehaviorWnd->TextGet() ) );
CExtSafeString str = _GetNumericText( m_pEditWithBehaviorWnd->TextGet(), true );
	if( str.IsEmpty() )
		return 0;
LCID _LCID = LANG_USER_DEFAULT;
	if( g_ResourceManager->IsCustomLangAllowed() )
		_LCID = g_ResourceManager->GetLangIdDesired();
COleCurrency _oleCurrency;
	VERIFY( _oleCurrency.ParseCurrency( LPCTSTR(str), 0, _LCID ) );
__int64 nVal = ((__int64)_oleCurrency.m_cur.int64) / ((__int64)10000);
	return INT(nVal);
}

CExtSafeString CExtEditSystemCurrency::Behavior_GetValidText(
	bool bSetSel // = false
	) const
{
	ASSERT_VALID( this );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
CExtSafeString strNewText = strText;
bool bIsNegative = _IsNegative( strText );
INT nStart, nEnd;
	m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
	_GetTextWithoutMarks( strNewText, nStart, nEnd );
	_GetTextWithMarks( strNewText, nStart, nEnd, ( bIsNegative && AllowNegativeGet() ) );
	if( bSetSel && ( ! m_bRichPainting ) )
	{
		CExtSafeString strCurrentText = m_pEditWithBehaviorWnd->TextGet();
		if( strNewText != strCurrentText )
			m_pEditWithBehaviorWnd->SetWindowText( strNewText );
		m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
	}
	return strNewText;
}

void CExtEditSystemCurrency::Behavior_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	if( m_pEditWithBehaviorWnd->GetStyle() & ES_READONLY )
		return;
TCHAR c = static_cast<TCHAR>( uChar );
INT nStart, nEnd;
	m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
CExtSafeString strText = m_pEditWithBehaviorWnd->TextGet();
CExtSafeString strNumericText = _GetNumericText( strText );
INT nDecimalPos = INT( strText.Find( m_strDecimalPoint ) );
INT nNumericDecimalPos = INT( strNumericText.Find( m_strDecimalPoint ) );
INT nLen = INT( strText.GetLength() );
INT nNumericLen = INT( strNumericText.GetLength() );
INT nSepCount = _GetGroupSeparatorCount( strText );
bool bNeedAdjustment = false;
bool bIsNegative = false;
bool bIsDecimalPoint = false;
	if(		( c == m_strNegativeSign[0] )
		||	(	(	m_nNegativeFormatIndex == 0
				||	m_nNegativeFormatIndex == 4
				||	m_nNegativeFormatIndex == 14
				||	m_nNegativeFormatIndex == 15
				)
			&&	c == _T('(') 
			)
		||	( c == _T('-') )
		)
		{
			bIsNegative = true;
		}
	// test whether it is a negative sign
	if( bIsNegative && AllowNegativeGet() )
	{
		bool bNegative = _IsNegative( strText );
		CExtSafeString _strBefore = _T(""), _strAfter = _T("");
		_GetSignStrings( _strBefore, _strAfter, bNegative );
		INT nBeforLen = INT( _strBefore.GetLength() );
		INT nAfterLen = INT( _strAfter.GetLength() );
		CExtSafeString strNewText = strText;
		m_pEditWithBehaviorWnd->GetSel( nStart, nEnd );
		if( nStart < nBeforLen )
			nStart = nBeforLen;
		if( nEnd > ( nLen - nAfterLen ) )
			nEnd = ( nLen - nAfterLen );
		_GetTextWithoutMarks( strNewText, nStart, nEnd );
		_GetTextWithMarks( strNewText, nStart, nEnd, ! bNegative );
		CExtSafeString strCurrentText = m_pEditWithBehaviorWnd->TextGet();
		if( strNewText != strCurrentText )
			m_pEditWithBehaviorWnd->SetWindowText( strNewText );
		m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
		return;
	}
	// test whether it is a decimal point (only one is enabled)
	else if( c == m_strDecimalPoint[0] && m_nMaxFractionDigits > 0 )
	{
		if( nDecimalPos >= 0 )
		{
			// test decimal point replacement
			if( nDecimalPos >= nStart && nDecimalPos < nEnd )
			{
				bNeedAdjustment = true;
				bIsDecimalPoint = true;
			}
			else
			{	// else set caret
					m_pEditWithBehaviorWnd->SetSel( nDecimalPos + m_strDecimalPoint.GetLength(), nDecimalPos + m_strDecimalPoint.GetLength() );
				//m_pEditWithBehaviorWnd->SetSel( nDecimalPos + 1, nDecimalPos + 1 );
				return;
			}
		}
		else
		{
			bNeedAdjustment = true;
			bIsDecimalPoint = true;
		}
	}
	// test if it's a digit
	else if( _istdigit(c) )
	{
		// test whether we're on the right of the decimal point
		if( nDecimalPos >= 0 && nDecimalPos < nStart )
		{
			bool bIsNegative = _IsNegative( strNumericText );
			CExtSafeString _strBeforeCurrency = _T("");
			CExtSafeString _strAfterCurrency = _T("");
			_GetSignStrings( _strBeforeCurrency, _strAfterCurrency, bIsNegative && AllowNegativeGet() );
			INT nNegLength = INT( _strAfterCurrency.GetLength() );
			if( ! _strAfterCurrency.IsEmpty() )
			{
				INT nNegSignPos = -1;
				if( ! _strAfterCurrency.IsEmpty() )
					nNegSignPos = INT( strText.Find( _strAfterCurrency ) );
				if( ( nStart > nNegSignPos ) && ( nStart <= nLen ) && nNegSignPos != -1 )
				{
					nStart = nNegSignPos;
					m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
				}
			}
			INT nDecimalSeparatorLen = INT( m_strDecimalPoint.GetLength() );
			if( nDecimalPos <= nStart && nStart < ( nDecimalPos + nDecimalSeparatorLen ) )
			{
				INT nPrevStart = nStart;
				nStart = ( nDecimalPos + nDecimalSeparatorLen );
				if(		nEnd == nPrevStart 
					||	nEnd < nPrevStart 
					||	(	nEnd > nPrevStart 
						&&	nEnd < nStart
						)
					)
					nEnd = nStart;
				m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
			}
			if( strNumericText.Mid( /*nDecimalPos */nNumericDecimalPos + nDecimalSeparatorLen ).GetLength() == ( m_nMaxFractionDigits + nNegLength ) )
			{
				if( nStart <= ( nDecimalPos + m_nMaxFractionDigits + nDecimalSeparatorLen - 1 ) )
				{
					//nEnd = ( ( ( nEnd == nLen ) || ( nEnd > nStart ) ) ? nEnd : (nStart + 1) );
					nEnd = ( ( ( nEnd == nLen ) || ( nEnd > ( nStart + 1 ) ) ) ? nEnd : (nStart + 1) );
					m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
					m_pEditWithBehaviorWnd->ReplaceSel( CExtSafeString(c), TRUE );
				}
				return;
			}
			else if( strNumericText.Mid( nNumericDecimalPos + nDecimalSeparatorLen ).GetLength() > ( m_nMaxFractionDigits + nNegLength ) )
			{
				ASSERT(false);
				return;
			}
		}
		// we're on the left side of the decimal point
		else
		{
			bool bIsNegative = _IsNegative( strNumericText );
			CExtSafeString _strBeforeCurrency = _T("");
			CExtSafeString _strAfterCurrency = _T("");
			_GetSignStrings( _strBeforeCurrency, _strAfterCurrency, bIsNegative && AllowNegativeGet() );
			INT nNegLength = INT( _strBeforeCurrency.GetLength() );
			INT nAfterSignsLength = INT( _strAfterCurrency.GetLength() );
			if( nNegLength > 0 )
			{
				if( ( nStart >= 0 ) && ( nStart < nNegLength ) )
				{
					nStart = nNegLength;
					if( nStart > nEnd )
						nEnd = nStart;
					m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
				}
			}
			if( nAfterSignsLength > 0 )
			{
				if( ( nEnd >= ( nLen - nAfterSignsLength ) ) && ( nEnd <= nLen ) )
				{
					nEnd = nLen - nAfterSignsLength;
					if( nStart > nEnd )
						nStart = nEnd;
					m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
				}
				if( ( nStart >= ( nLen - nAfterSignsLength ) ) && ( nStart <= nLen ) )
				{
					nStart = nLen - nAfterSignsLength;
					m_pEditWithBehaviorWnd->SetSel( nStart, nStart );
				}
			}
			INT nGroupSeparatorLen = m_bEmptyGroupSeparatorMode ? 0 : INT( m_strGroupSeparator.GetLength() );
			INT nDecimalSignLen = INT( m_strDecimalPoint.GetLength() );
			// ensure we can still enter digits
			INT nDecPos = m_nMaxWholeDigits + nNegLength + ( nSepCount * nGroupSeparatorLen );
			if( nStart == nDecPos )
			{
				UINT nFlags = CBehaviorBase::m_nBehaviorFlags;
				if(  nFlags & __ESE_ADD_DECIMAL_AFTER_MAX_WHOLE_DIGITS && m_nMaxFractionDigits > 0 )
				{
					nEnd = ( ( ( nEnd == nLen ) || ( ( nEnd > nStart ) && ( ( nEnd - nStart ) > 1 ) ) ) ? nEnd : ( nStart + nDecimalSignLen + 1 ) );
					
					if( _strAfterCurrency.GetLength() > 0 )
					{
						m_pEditWithBehaviorWnd->SetSel( nStart, strText.GetLength() );
						m_pEditWithBehaviorWnd->ReplaceSel( _T(""), TRUE );
					}
					m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
					m_pEditWithBehaviorWnd->ReplaceSel( m_strDecimalPoint + c, TRUE );
					if( _strAfterCurrency.GetLength() > 0 )
					{
						CExtSafeString strChangedText = m_pEditWithBehaviorWnd->TextGet();
						INT nEnd = INT( strChangedText.GetLength() );
						m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
						m_pEditWithBehaviorWnd->ReplaceSel( _strAfterCurrency, TRUE );
						m_pEditWithBehaviorWnd->SetSel( nEnd, nEnd );
					}
					CExtSafeString strReplaceText = Behavior_GetValidText( true );
				}
				return;
			}
			if(	strNumericText.Mid( 
					0, 
					(	( nNumericDecimalPos >= 0 )
						? nNumericDecimalPos
						: nNumericLen
							)
						).GetLength()
				== ( m_nMaxWholeDigits + nNegLength )
				)
			{
				if( nGroupSeparatorLen )
				{
					INT nPos = 0;
					for( ; true; )
					{
						nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
						if( nPos == -1 )
							break;
						if( nPos <= nStart && nStart < ( nPos + nGroupSeparatorLen ) )
						{
							nStart = nPos + nGroupSeparatorLen;
							break;
						}
						nPos ++;
					}
				}
				INT _nNegativeSignLen = nNegLength;
				if( nStart < _nNegativeSignLen )
					nStart = _nNegativeSignLen;
				nEnd = ( ( ( nEnd == nLen ) || ( nEnd > nStart ) ) ? nEnd : ( nStart + 1 ) );
				if( _strAfterCurrency.GetLength() > 0 )
				{
					INT nNegSignPos = INT( strText.Find( _strAfterCurrency ) );
					if( nEnd > nNegSignPos )
						nEnd = nNegSignPos;
				}
				if( ! m_bRichPainting )
					m_pEditWithBehaviorWnd->SetSel( nStart, nEnd );
				m_pEditWithBehaviorWnd->ReplaceSel( CExtSafeString(c), TRUE );
				CExtSafeString strReplaceText = Behavior_GetValidText(true);
				return;
			}
			bNeedAdjustment = true;
		}
	}
	// test whether it is a non-printable character (i.e. backspace, ctrl+c )
	else if( ! ( _istprint( c ) || IsCharAlphaNumeric( c ) ) )
		bNeedAdjustment = true;
	else
		return;
	if( ! m_bBackspace )
	{
		//CExtEditSystemNumberCurrencyBase::CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags );
		CBehaviorBase::Behavior_OnChar( uChar, nRepCnt, nFlags );

		// if the decimal point was added/removed or a separator needs adding/removing, adjust the text
		if( bNeedAdjustment )
			_AdjustSeparators( nSepCount, bIsDecimalPoint );
	}
	else
		m_bBackspace = false;
}

bool CExtEditSystemCurrency::_DeleteSymbols( 
	CExtSafeString & strText, 
	INT & nStart, 
	INT & nEnd, 
	bool bBackspace, // = false
	LPARAM lParam // = 0L
	)
{
	lParam;
	ASSERT_VALID( this );
	ASSERT( ! m_bRichPainting );
INT nLen = INT( strText.GetLength() );
bool bNegative = _IsNegative( strText );
CExtSafeString strTextLen = strText;
INT nStLen = 0, nEndLen = 0;
	_GetTextWithoutMarks( strTextLen, nStLen, nEndLen );
INT nTextLen = INT( strTextLen.GetLength() );
INT nDecSepPosLen = INT( strTextLen.Find( m_strDecimalPoint ) );
INT nGroupSeparatorLen = m_bEmptyGroupSeparatorMode ? 0 : INT( m_strGroupSeparator.GetLength() );
INT nDecimalSeparatorLen = INT( m_strDecimalPoint.GetLength() );
INT nDecimalSeparatorPos = INT( strText.Find( m_strDecimalPoint ) );
INT _nPos = 0, _nGroupSignBeforeStart = 0, _nGroupSignCount = (INT)_GetGroupSeparatorCount( strText );
	if( nGroupSeparatorLen > 0 )
	{
		for( ; true; )
		{
			_nPos = (INT)strText.Find( m_strGroupSeparator, _nPos );
			if( _nPos < 0 )
				break;
			if( _nPos < nStart )
				_nGroupSignBeforeStart ++;
			_nPos ++;
		}
	}
	_nGroupSignBeforeStart = _nGroupSignCount - _nGroupSignBeforeStart;
	if( bBackspace )
	{
		if( nStart == 0 && nStart == nEnd )
			return false;
		INT nPos = 0;
		if( nStart >= 0 )
		{
			if(		(	nDecimalSeparatorPos < nStart
					||	(	nDecimalSeparatorPos == nStart
						&&	nStart != nEnd
						)
					)
				&&	nStart <= ( nDecimalSeparatorPos + nDecimalSeparatorLen )
				&&	nEnd <= ( nDecimalSeparatorPos + nDecimalSeparatorLen )
				&&	nDecSepPosLen == m_nMaxWholeDigits
				&&	(	nTextLen > ( nDecSepPosLen + nDecimalSeparatorLen )
					&&	nDecSepPosLen != -1
					)
				)
			{
				nStart = nDecimalSeparatorPos;
				nEnd = nDecimalSeparatorPos;
				m_bBackspace = true;
				return false;
			}
			if( nGroupSeparatorLen > 0 )
			{
				for( ; true; )
				{
					nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
					if( nPos == -1 )
						break;
					if(		(	nPos < nStart 
							||	(	nPos == nStart 
								&&	nStart != nEnd
								)
							)
						&&	nStart <= ( nPos + nGroupSeparatorLen )
						&&	nEnd <= ( nPos + nGroupSeparatorLen )
						)
					{
						nStart = nPos;
						nEnd = nPos;
						m_bBackspace = true;
						return false;
					}

					nPos ++;
				}
			}
		}
	}
	else
	{
		if( nStart == nLen )
			return false;
		INT nPos = 0;
		if( nStart < nLen )
		{
			if(		nDecimalSeparatorPos <= nStart
				&&	nStart < ( nDecimalSeparatorPos + nDecimalSeparatorLen )
				&&	nEnd <= ( nDecimalSeparatorPos + nDecimalSeparatorLen )
				&&	nDecSepPosLen == m_nMaxWholeDigits
				&&	(	nTextLen > ( nDecSepPosLen + nDecimalSeparatorLen )
					&&	nDecSepPosLen != -1
					)
				)
			{
				nStart = nDecimalSeparatorPos + nDecimalSeparatorLen;
				nEnd = nDecimalSeparatorPos + nDecimalSeparatorLen;
				return false;
			}
			if( nGroupSeparatorLen > 0 )
			{
				for( ; true; )
				{
					nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
					if( nPos == -1 )
						break;
					if(		nPos <= nStart 
						&&	nStart < ( nPos + nGroupSeparatorLen )
						&&	nEnd <= ( nPos + nGroupSeparatorLen )
						)
					{
						nStart = nPos + nGroupSeparatorLen;
						nEnd = nPos + nGroupSeparatorLen;
						return false;
					}
					nPos ++;
				}
			}
		}
	}
	if( nStart != nEnd )
	{
		if(		nDecimalSeparatorPos < nStart
			&&	nStart < ( nDecimalSeparatorPos + nDecimalSeparatorLen )
			)
		{
			nStart = nDecimalSeparatorPos + nDecimalSeparatorLen;
			if( nStart > nEnd )
				nEnd = nStart;
		}
		if(		nDecimalSeparatorPos < nEnd
			&&	nEnd <= ( nDecimalSeparatorPos + nDecimalSeparatorLen )
			)
			nEnd = nDecimalSeparatorPos;
		if( nGroupSeparatorLen > 0 )
		{
			INT nPos = 0;
			for( ; true; )
			{
				nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
				if( nPos == -1 )
					break;
				if(		nPos < nStart 
					&&	nStart < ( nPos + nGroupSeparatorLen )
					)
				{
					nStart = nPos + nGroupSeparatorLen;
					if( nStart >= nEnd )
					{
						nEnd = nStart;
						break;
					}
				}
				if(		nPos < nEnd 
					&&	nEnd < ( nPos + nGroupSeparatorLen )
					)
				{
					nEnd = nPos;
					if( nEnd < nStart )
						nStart = nEnd;
					if( nEnd >= nStart )
						break;
				}
				nPos ++;
			}
		}
	}

	{
		CExtSafeString _strBefore = _T("");
		CExtSafeString _strAfter = _T("");
		_GetSignStrings( _strBefore, _strAfter, bNegative && AllowNegativeGet() );
		INT nBeforeLen = INT( _strBefore.GetLength() );
		INT nAfterLen = INT( _strAfter.GetLength() );
		bool bNegDel = false;
		if( bNegative )
		{
			if(		(	nStart < nBeforeLen
					||	(	nStart == nBeforeLen
						&&	nStart == nEnd  
						&&	bBackspace 
						) 
					) 
				&&	nBeforeLen > 0
				)
			{
				bNegative = false;
				bNegDel = true;
			}
			if(		(	( nStart >= ( nLen - nAfterLen ) && ! bBackspace ) 
					||	( nStart > ( nLen - nAfterLen ) && bBackspace ) 
					||	( nStart == nLen  && nStart == nEnd  && bBackspace ) 
					||	nEnd > ( nLen - nAfterLen )
					) 
				&&	nAfterLen > 0
				)
			{
				bNegative = false;
				bNegDel = true;
			}
		}
		else
		{
			if(		(	(	nStart < nBeforeLen
						&&	nEnd <= nBeforeLen
						)
					||	(	nStart == nBeforeLen
						&&	nStart == nEnd  
						&&	bBackspace 
						) 
					) 
				&&	nBeforeLen > 0
				)
			{
				nStart = nBeforeLen;
				return true;
			}
			if(		(	( nStart >= ( nLen - nAfterLen ) && ! bBackspace ) 
					||	( nStart > ( nLen - nAfterLen ) && bBackspace ) 
					||	( nStart == nLen  && nStart == nEnd  && bBackspace ) 
					) 
				&&	nAfterLen > 0
				)
			{
				nStart = nLen - nAfterLen;
				return true;
			}
			if(		nEnd > ( nLen - nAfterLen ) 
				&&	nAfterLen > 0
				)
				nEnd = nLen - nAfterLen;
		}
		_GetTextWithoutMarks( strText, nStart, nEnd );
		if( bBackspace )
		{
			bool bSelect = ( nStart != nEnd ) ? true : false;
			strText.Delete( ( nStart != nEnd ) ? nStart : ( nStart - 1 ), ( nStart != nEnd ) ? ( nEnd - nStart ) : ( bNegDel ? 0 : 1 ) );
			if( ! bSelect && ! bNegDel )
				nStart --;
			m_bBackspace = true;
		}
		else
		{
			INT _nDecimalPointPos = INT( strText.Find( m_strDecimalPoint ) );
			if( _nDecimalPointPos != -1 )
			{
				if(		nStart > _nDecimalPointPos 
					&&	nStart < ( _nDecimalPointPos + nDecimalSeparatorLen )
					)
					nStart = _nDecimalPointPos;
				if(		nEnd > _nDecimalPointPos 
					&&	nEnd < ( _nDecimalPointPos + nDecimalSeparatorLen )
					)
					nEnd = _nDecimalPointPos + nDecimalSeparatorLen;
			}
			strText.Delete( nStart, ( nStart != nEnd ) ? ( nEnd - nStart ) : ( bNegDel ? 0 : 1 ) );
		}
		_GetTextWithoutMarks( strText, nStart, nEnd );
		_GetTextWithMarks( strText, nStart, nEnd, bNegative );
		if( nGroupSeparatorLen > 0 && m_bBackspace && ( nStart - nGroupSeparatorLen ) >= 0 )
		{
			INT nGroupSepPos = (INT)strText.Find( m_strGroupSeparator, ( nStart - nGroupSeparatorLen ) );
			if( nGroupSepPos >= 0 )
			{
				INT _nPos = 0;
				INT _nGroupSignBeforeStartAfterDelete = 0;
				bool _bChangePos = false;
				INT _nGroupSignCountAfterDelete = (INT)_GetGroupSeparatorCount( strText );
				for( ; true; )
				{
					_nPos = (INT)strText.Find( m_strGroupSeparator, _nPos );
					if( _nPos < 0 )
						break;
					if( _nPos < nStart )
						_nGroupSignBeforeStartAfterDelete ++;
					_nPos ++;
				}
				_nGroupSignBeforeStartAfterDelete = _nGroupSignCountAfterDelete - _nGroupSignBeforeStartAfterDelete;
				if( ( _nGroupSignBeforeStart - 1 ) == _nGroupSignBeforeStartAfterDelete )
					_bChangePos = true;
				if( nGroupSepPos == ( nStart - nGroupSeparatorLen ) && _bChangePos )
					nStart = nGroupSepPos;
			}
		}
	}
	return true;
}

INT CExtEditSystemCurrency::_GetSignLengthBeforeCurrency( 
	bool bNegative // = false 
	) const
{
	ASSERT_VALID( this );
INT nCurSignLength = INT( m_strCurrencySymbol.GetLength() );
INT nNegSignLenBeforeCur = 0;
INT nPosSignLenBeforeCur = 0;
INT nSpecificSymbLength = 1; // Bracket or Space
	if( bNegative )
	{
		CExtSafeString strBefore = _T("");
		CExtSafeString strAfter = _T("");
		_GetSignStrings( strBefore, strAfter );
		nNegSignLenBeforeCur = INT( strBefore.GetLength() );
	}
	switch( m_nPositiveFormatIndex )
	{
	case 0:
		nPosSignLenBeforeCur = nCurSignLength;
	break;
	case 2:
		nPosSignLenBeforeCur = nCurSignLength + nSpecificSymbLength;
	break;
	}
	return bNegative ? nNegSignLenBeforeCur : nPosSignLenBeforeCur;
}

void CExtEditSystemCurrency::_GetSignStrings( 
	CExtSafeString & strBeforeCurrency, 
	CExtSafeString & strAfterCurrency,
	bool bNegative // = true
	) const
{
	ASSERT_VALID( this );
CExtSafeString strBefore = _T("");
CExtSafeString strAfter = _T("");
	if( bNegative )
	{
		switch( m_nNegativeFormatIndex )
		{
		case 0:
			strBefore = _T("(") + m_strCurrencySymbol;
			strAfter = _T(")");
		break;
		case 1:
			strBefore = m_strNegativeSign + m_strCurrencySymbol;
		break;
		case 2:
			strBefore = m_strCurrencySymbol + m_strNegativeSign;
		break;
		case 3:
			strBefore = m_strCurrencySymbol;
			strAfter = m_strNegativeSign;
		break;
		case 4:
			strBefore = _T("(");
			strAfter = m_strCurrencySymbol + _T(")");
		break;
		case 5:
			strBefore = m_strNegativeSign;
			strAfter = m_strCurrencySymbol;
		break;
		case 6:
			strAfter = m_strNegativeSign + m_strCurrencySymbol;
		break;
		case 7:
			strAfter = m_strCurrencySymbol + m_strNegativeSign;
		break;
		case 8:
			strBefore = m_strNegativeSign;
			strAfter = _T(" ") + m_strCurrencySymbol;
		break;
		case 9:
			strBefore = m_strNegativeSign + m_strCurrencySymbol + _T(" ");
		break;
		case 10:
			strAfter = _T(" ") + m_strCurrencySymbol + m_strNegativeSign;
		break;
		case 11:
			strBefore = m_strCurrencySymbol + _T(" ");
			strAfter = m_strNegativeSign;
		break;
		case 12:
			strBefore = m_strCurrencySymbol + _T(" ") + m_strNegativeSign;
		break;
		case 13:
			strAfter = m_strNegativeSign + _T(" ") + m_strCurrencySymbol;
		break;
		case 14:
			strBefore = _T("(") + m_strCurrencySymbol + _T(" ");
			strAfter = _T(")");
		break;
		case 15:
			strBefore = _T("(");
			strAfter = _T(" ") + m_strCurrencySymbol + _T(")");
		break;
		}
	}
	else
	{
		switch( m_nPositiveFormatIndex )
		{
		case 0:
			strBefore = m_strCurrencySymbol;
		break;
		case 1:
			strAfter = m_strCurrencySymbol;
		break;
		case 2:
			strBefore = m_strCurrencySymbol + _T(" ");
		break;
		case 3:
			strAfter = _T(" ") + m_strCurrencySymbol;
		break;
		}
	}
	strBeforeCurrency = strBefore;
	strAfterCurrency = strAfter;
}

void CExtEditSystemCurrency::_GetTextWithoutMarks( 
	CExtSafeString & strText, 
	INT & nStart, 
	INT & nEnd, 
	bool bCutDecimalPoint // = false 
	) const
{
	ASSERT_VALID( this );
CExtSafeString strCurrentText;
INT nStartPos = nStart, nEndPos = nEnd;
INT nSignCountBeforeStartPos ,nSignCountBeforeEndPos ,nSignCount;
bool bIsNegative = _IsNegative(strText);
INT nDecimalSeparatorLen = INT( m_strDecimalPoint.GetLength() );
INT nDecimalSeparatorPos = INT( strText.Find( m_strDecimalPoint ) );
CExtSafeString _strBefore = _T("");
CExtSafeString _strAfter = _T("");
	_GetSignStrings( _strBefore, _strAfter, bIsNegative );
INT nStrLen = INT( strText.GetLength() );
	if( nEndPos > INT( strText.GetLength() ) )
		nEndPos = INT( strText.GetLength() );
	nSignCountBeforeStartPos = nSignCountBeforeEndPos = nSignCount = 0;
	for( INT i = 0; i < nStrLen; i++ )
	{
		TCHAR _tchr = strText[i];
		bool bUseSymbol = false;
		if( _T('0') <= _tchr && _tchr <= _T('9') )
		{
			strCurrentText += _tchr;
			bUseSymbol = true;
		}
		if( i >= nDecimalSeparatorPos && i < ( nDecimalSeparatorPos + nDecimalSeparatorLen ) && ! bCutDecimalPoint && nDecimalSeparatorPos != -1 )
		{
			strCurrentText += _tchr;

			bUseSymbol = true;
		}
		if( ! bUseSymbol )
			nSignCount++;
		if( i == ( nStartPos - 1 ) )
			nSignCountBeforeStartPos = nSignCount;
		else if( i == ( nEndPos - 1 ) )
			nSignCountBeforeEndPos = nSignCount;
	}
	nStartPos -= nSignCountBeforeStartPos;
	nEndPos -= nSignCountBeforeEndPos;
	if( nStartPos < 0 )
		nStartPos = 0;
	if( nEndPos < 0 )
		nEndPos = 0;
	strText = strCurrentText;
	if( nStart != nEnd )
	{
		nStart = nStartPos;
		nEnd = nEndPos;
	}
	else
		nStart = nEnd = nStartPos;
}

void CExtEditSystemCurrency::_GetTextWithMarks( 
	CExtSafeString & strText, 
	INT & nStart, 
	INT & nEnd, 
	bool bInsertNegSign  // = false
	) const
{
	ASSERT_VALID( this );
	if( strText.IsEmpty() )
		return;
INT nTextLen = INT( strText.GetLength() );
CExtSafeString strTextChange;
INT nGroupSeparatorLen = m_bEmptyGroupSeparatorMode ? 0 : INT( m_strGroupSeparator.GetLength() );
INT nDecimalSeparatorLen = INT( m_strDecimalPoint.GetLength() );
INT nPositionDecimalPoint = INT( strText.Find( m_strDecimalPoint ) );
INT nMaxLen = m_nMaxWholeDigits + m_nMaxFractionDigits + ( ( nPositionDecimalPoint >= 0 ) ? nDecimalSeparatorLen : 0 );
bool bStartBehindEnd = ( nStart > INT( strText.GetLength() ) );
bool bEndBehindEnd = ( nEnd > INT( strText.GetLength() ) );
	if( nTextLen > nMaxLen )
	{
		if( nPositionDecimalPoint >= 0 )
		{
			CExtSafeString _strDecimalPoint = _T("");
			_strDecimalPoint = m_strDecimalPoint;
			strText.Replace( _strDecimalPoint, _T("") );

			if( nPositionDecimalPoint > m_nMaxWholeDigits )
			{
				strText.Insert( m_nMaxWholeDigits, m_strDecimalPoint );
			}
			else
				strText.Insert( nPositionDecimalPoint, m_strDecimalPoint );
		}
		nTextLen = INT( strText.GetLength() );
	}
INT nWholeNumber = ( ( nPositionDecimalPoint >= 0 ) ? ( ( nPositionDecimalPoint < m_nMaxWholeDigits ) ? nPositionDecimalPoint : m_nMaxWholeDigits ) : ( ( m_nMaxWholeDigits >= nTextLen ) ?  nTextLen : m_nMaxWholeDigits ) );
INT nDecimalPointLen = nDecimalSeparatorLen; // 1
INT nCorectTextLen = nWholeNumber + nDecimalPointLen + m_nMaxFractionDigits;
	nCorectTextLen = ( nCorectTextLen > nTextLen ) ? nTextLen : nCorectTextLen;
INT nCountDigitGroups = INT( m_listDigitGrouping.GetCount() );
CList < INT, INT > _listPos;
bool bLastZiro = ( m_listDigitGrouping.GetAt( m_listDigitGrouping.FindIndex( nCountDigitGroups - 1 ) ) == 0 ) ? true : false;
	ASSERT( nWholeNumber <= m_nMaxWholeDigits );
INT nPos = nWholeNumber;
INT i = 0;
INT nPreviousCount = -1;
	for( ; true; )
	{
		INT nCount = m_listDigitGrouping.GetAt(m_listDigitGrouping.FindIndex(i));
		if( nCount == 0 && i < ( nCountDigitGroups - 1 ) )
			break;
		if( nCount == 0 && i == ( nCountDigitGroups - 1 ) )
		{
			ASSERT( nPreviousCount != -1 );
			nCount = nPreviousCount;
		}
		nPos -= nCount;
		if( nPos > 0 )
			_listPos.AddHead( nPos - 1 );
		else
			break;
		if( i == ( nCountDigitGroups - 1 ) && ! bLastZiro )
			break;
		if( i < ( nCountDigitGroups - 1 ) )
			i ++;
		nPreviousCount = nCount;
	}
INT nSignCountBeforeStart = 0;
INT nSignCountBeforeEnd = 0;
bool bInsertedDecimalSignBeforeStart = false;
bool bInsertedDecimalSignBeforeEnd = false;
INT nCurDigitPos = 0;
INT nPosCount = INT( _listPos.GetCount() );
bool bCanPastGroupSep = ( nPosCount > 0 );
	for( i = 0; i < nCorectTextLen; i++ )
	{
		TCHAR _tchr = strText[i];
		strTextChange += _tchr;
		bool bInsertSign = false;
		INT nPosCur = 0;
		if( bCanPastGroupSep )
			nPosCur = _listPos.GetAt( _listPos.FindIndex( nCurDigitPos ) );
		if(		( i <= nWholeNumber )
			&&	( i != ( nCorectTextLen - 1 ) )
			&&	( i == nPosCur )
			&&	bCanPastGroupSep
			)
		{
			if( ! m_bEmptyGroupSeparatorMode )
				strTextChange += m_strGroupSeparator;
			bInsertSign = true;
			if( nCurDigitPos < ( nPosCount - 1 ) )
				nCurDigitPos ++;
			else
				bCanPastGroupSep = false;
		}
		if( i == ( nWholeNumber - 1 ) && ( nPositionDecimalPoint == -1 ) && ( i != nCorectTextLen - 1 ) )
		{
			strTextChange += m_strDecimalPoint;
			if( i < ( nStart - 1 ) )
				bInsertedDecimalSignBeforeStart  = true;
			if( i < ( nEnd - 1 ) )
				bInsertedDecimalSignBeforeEnd = true;
		}
		if( bInsertSign )
		{
			if( i < nStart )
				nSignCountBeforeStart ++;
			if( i < nEnd )
				nSignCountBeforeEnd ++;
		}
	}
CExtSafeString _strSignBefore = _T("");
CExtSafeString _strSignAfter = _T("");
	_GetSignStrings( _strSignBefore, _strSignAfter, bInsertNegSign && AllowNegativeGet() );
	if( _strSignAfter.GetLength() > 0 )
	{
		strTextChange.Insert( strTextChange.GetLength(), _strSignAfter );
		if( bStartBehindEnd )
			nStart += INT( _strSignAfter.GetLength() );
		if( bEndBehindEnd )
			nEnd += INT( _strSignAfter.GetLength() );
	}
	if( _strSignBefore.GetLength() > 0 )
	{
		strTextChange.Insert( 0, _strSignBefore );
		nStart += INT( _strSignBefore.GetLength() );
		nEnd += INT( _strSignBefore.GetLength() );
	}
	strText = strTextChange;
	nStart += nGroupSeparatorLen * nSignCountBeforeStart + ( bInsertedDecimalSignBeforeStart ? nDecimalSeparatorLen : 0 );
	nEnd += nGroupSeparatorLen * nSignCountBeforeEnd + ( bInsertedDecimalSignBeforeEnd ? nDecimalSeparatorLen : 0 );
}

bool CExtEditSystemCurrency::_IsNegative( __EXT_MFC_SAFE_LPCTSTR strText ) const
{
	ASSERT_VALID( this );
CExtSafeString _strText = strText;
bool bRetVal = ( ( _strText.Find( m_strNegativeSign ) >= 0 ) ? true: false );
	if(		m_nNegativeFormatIndex == 0 
		||	m_nNegativeFormatIndex == 4
		||	m_nNegativeFormatIndex == 14
		||	m_nNegativeFormatIndex == 15
		)
		bRetVal = ( ( _strText.Find( _T('(') ) >= 0 ) ? true: false );
	return bRetVal;
}

void CExtEditSystemCurrency::_AdjustWithinRange()
{
	ASSERT_VALID( this );
	if( _IsValid() )
		return;
	if( m_pEditWithBehaviorWnd->TextGet().IsEmpty() )
		m_pEditWithBehaviorWnd->SetWindowText( _T(" ") );
	else
		_Redraw();
long double dValue = LongDoubleGet();
    if( dValue < m_lfMin )
		LongDoubleSet( m_lfMin );
    else if( dValue > m_lfMax )
		LongDoubleSet( m_lfMax );
CExtSafeString _strText = m_pEditWithBehaviorWnd->TextGet();
bool bNegative = _IsNegative( _strText );
CExtSafeString _strBefore = _T("");
CExtSafeString _strAfter = _T("");
	_GetSignStrings( _strBefore, _strAfter, bNegative );
INT nNegSignPos = -1;
INT nLen = INT( _strText.GetLength() );
	if( ! _strAfter.IsEmpty() )
		nNegSignPos = INT( _strText.Find( _strAfter ) );
	if( ! m_bRichPainting )
	{
		if( nNegSignPos >= 0 )
			m_pEditWithBehaviorWnd->SetSel( nNegSignPos, nNegSignPos );
		else
			m_pEditWithBehaviorWnd->SetSel( nLen, nLen );
	}
}

void CExtEditSystemCurrency::_Rich_OnDraw()
{
	if( ! m_bRichMode )
		return;
	if( GetSafeHwnd() == NULL )
		return;
	if( m_bRichPainting )
		return;
	m_bRichPainting = true;
CHARRANGE crRestore;
	Rich_GetSel( crRestore );
CHARFORMAT cfDefault;
	::memset( &cfDefault, 0, sizeof(CHARFORMAT) );
	cfDefault.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_STRIKEOUT|CFM_FACE|CFM_SIZE|CFM_CHARSET|CFM_PROTECTED|CFM_COLOR;
	cfDefault.bCharSet = DEFAULT_CHARSET;
	cfDefault.dwEffects = CFE_PROTECTED; 
	//cfDefault.yHeight = 180;
	//cfDefault.crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
CFont * pFont = GetFont();
	if( pFont->GetSafeHandle() == NULL )
	{
		CWnd * pWndParent = GetParent();
		if( pWndParent->GetSafeHwnd() != NULL )
			pFont = pWndParent->GetFont();
		if( pFont->GetSafeHandle() == NULL )
			pFont = &(PmBridge_GetPM()->m_FontNormal);
	}
LOGFONT _lf; ::memset( &_lf, 0, sizeof(LOGFONT) );
	pFont->GetLogFont( &_lf );
#if (! defined _RICHEDIT_VER )
	USES_CONVERSION;
	strcpy( cfDefault.szFaceName, T2CA(_lf.lfFaceName) );
#else
#if ( _RICHEDIT_VER < 0x0200 )
	USES_CONVERSION;
	strcpy( cfDefault.szFaceName, T2CA(_lf.lfFaceName) );
#else
	__EXT_MFC_STRCPY( cfDefault.szFaceName, sizeof(cfDefault.szFaceName)/sizeof(cfDefault.szFaceName[0]), _lf.lfFaceName );
#endif
#endif
CWindowDC dcDesktop( NULL );
	cfDefault.yHeight = ::MulDiv( _lf.lfHeight, 72 * 20, dcDesktop.GetDeviceCaps( LOGPIXELSY ) );
	cfDefault.yHeight = abs( cfDefault.yHeight );
bool bEnabled = IsWindowEnabled() ? true : false, bReadOnly = ( ( GetStyle() & ES_READONLY ) != 0 ) ? true : false;
	cfDefault.crTextColor = OnQueryTextColor();
	if( cfDefault.crTextColor == COLORREF(-1L) )
	{
		cfDefault.crTextColor = bEnabled ? ( bReadOnly ? m_clrRichTextReadOnly : m_clrRichTextNormal ) : m_clrRichTextDisabled;
		if( cfDefault.crTextColor == COLORREF(-1L) )
		{
			if( bEnabled )
			{
				if( bReadOnly )
					cfDefault.crTextColor = ::GetSysColor( COLOR_3DSHADOW );
				else
					cfDefault.crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
			}
			else
				cfDefault.crTextColor = ::GetSysColor( COLOR_GRAYTEXT );
		}
	}
COLORREF clrBack = OnQueryBackColor();
	if( clrBack == COLORREF(-1L) )
	{
		clrBack = bEnabled ? ( bReadOnly ? m_clrRichBackReadOnly : m_clrRichBackNormal ) : m_clrRichBackDisabled;
		if( clrBack == COLORREF(-1L) )
		{
			if( bEnabled )
			{
				if( bReadOnly )
					clrBack = ::GetSysColor( COLOR_3DFACE );
				else
					clrBack = ::GetSysColor( COLOR_WINDOW );
			}
			else
				clrBack = ::GetSysColor( COLOR_3DFACE );
		}
	}
	Rich_SetBackgroundColor( FALSE, clrBack );
	Rich_HideSelection( true, false );
	Rich_SetSel( 0, -1 ); 
	SendMessage( EM_SETCHARFORMAT, SCF_DEFAULT, LPARAM(&cfDefault) );
	SendMessage( EM_SETCHARFORMAT, SCF_SELECTION, LPARAM(&cfDefault) );
	Rich_SetSelectionColor( cfDefault.crTextColor );
COLORREF clrSign = m_clrSign;
	if( clrSign == COLORREF(-1L) )
		clrSign = cfDefault.crTextColor;
COLORREF clrBrackets = m_clrBrackets;
	if( clrSign == COLORREF(-1L) )
		clrBrackets = cfDefault.crTextColor;
INT nGroupSeparatorLen = m_bEmptyGroupSeparatorMode ? 0 : INT( m_strGroupSeparator.GetLength() );
INT nDecimalSignLen = INT( m_strDecimalPoint.GetLength() );
	if(		m_clrNumber != COLORREF(-1L)
		||	m_clrFraction != COLORREF(-1L)
		||	m_clrGroupSeparator != COLORREF(-1L)
		||	m_clrDecimalSeparator != COLORREF(-1L)
		||	m_clrSign != COLORREF(-1L)
		||	m_clrBrackets != COLORREF(-1L)
		)
	{
		CExtSafeString strText = TextGet();
		INT nTextLen = INT( strText.GetLength() );
		if( nTextLen > 0 )
		{
			bool bNegative = _IsNegative( strText );
			INT nPositionDecimalPoint = INT( strText.Find( m_strDecimalPoint ) );
			CExtSafeString _strBefore = _T(""), _strAfter = _T("");
			_GetSignStrings( _strBefore, _strAfter, bNegative );
			INT nBeforLen = _strBefore.GetLength();
			INT nAfterLen = _strAfter.GetLength();
			// whole number
			if( m_clrNumber != COLORREF(-1L) )
			{
				Rich_SetSel( nBeforLen, ( nPositionDecimalPoint != -1 ) ? nPositionDecimalPoint : ( nTextLen - nAfterLen ) );
				Rich_SetSelectionColor( m_clrNumber );
			}
			if( nPositionDecimalPoint != -1 )
			{
				if( m_clrDecimalSeparator != COLORREF(-1L) )
				{
					Rich_SetSel( nPositionDecimalPoint, ( nPositionDecimalPoint + nDecimalSignLen ) );
					Rich_SetSelectionColor( m_clrDecimalSeparator );
				}
				if( m_clrFraction != COLORREF(-1L) )
				{
					Rich_SetSel( nPositionDecimalPoint + nDecimalSignLen, nTextLen - nAfterLen );
					Rich_SetSelectionColor( m_clrFraction );
				}
			}
			if( nGroupSeparatorLen > 0 && m_clrGroupSeparator != COLORREF(-1L) )
			{
				INT nPos = 0;
				for( ; true; )
				{
					nPos = INT( strText.Find( m_strGroupSeparator, nPos ) );
					if( nPos == -1 )
						break;
					Rich_SetSel( nPos, ( nPos + nGroupSeparatorLen ) );
					Rich_SetSelectionColor( m_clrGroupSeparator ); 
					nPos ++;
				}
			}
			INT nNegSignPos = INT( strText.Find( m_strNegativeSign ) );
			INT nNegSignLen = INT( m_strNegativeSign.GetLength() );
			if( nNegSignPos >= 0 )
			{
				if( m_clrSign != COLORREF(-1L) )
				{
					Rich_SetSel( nNegSignPos, nNegSignPos + nNegSignLen );
					Rich_SetSelectionColor( m_clrSign );
				}
			}
			if( m_clrBrackets != COLORREF(-1L) )
			{
				INT nPos = 0;
				CExtSafeString _strBrackets = _T("()[]{}");
				INT nBrackLen = INT( _strBrackets.GetLength() );
				for( INT i = 0; i < nBrackLen; i++ )
				{
					nPos = 0;
					for( ; true; )
					{
						nPos = INT( strText.Find( _strBrackets[i], nPos ) );
						if( nPos == -1 )
							break;
						Rich_SetSel( nPos, ( nPos + 1 ) );
						Rich_SetSelectionColor( m_clrBrackets ); 
						nPos ++;
					}
				}
			}
		} // if( nTextLen > 0 )
	}
	Rich_SetSel( crRestore );
	Rich_HideSelection( false, false );
	m_bRichPainting = false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtEditScientificNumber window

IMPLEMENT_DYNCREATE( CExtEditScientificNumber, CExtEditSystemNumber );

CExtEditScientificNumber::CExtEditScientificNumber(
	INT nMaxWholeDigits, // = __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS
	INT nMaxFractionDigits // = __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS
	)
{
	m_bUseScientificNumberMode = true;
	m_nNegativeFormatIndex = 1;
	::CExtEditSystemNumber( nMaxWholeDigits, nMaxFractionDigits, true );
}

BEGIN_MESSAGE_MAP( CExtEditScientificNumber, CExtEditSystemNumber )
//{{AFX_MSG_MAP(CExtEditSystemNumberCurrencyBase)
//}}AFX_MSG_MAP
__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

void CExtEditScientificNumber::PowerColorSet( COLORREF clrPower /* = COLORREF(-1L) */ )
{
	ASSERT_VALID( this );
	if( m_clrPower == clrPower )
		return;
	m_clrPower = clrPower;
	_Rich_OnDraw();
}

COLORREF CExtEditScientificNumber::PowerColorGet() const
{
	ASSERT_VALID( this );
	return m_clrPower;
}

void CExtEditScientificNumber::ScientificSignColorSet( COLORREF clrScientifcSign /* = COLORREF(-1L) */ )
{
	ASSERT_VALID( this );
	if( m_clrScientificSign == clrScientifcSign )
		return;
	m_clrScientificSign = clrScientifcSign;
	_Rich_OnDraw();
}

COLORREF CExtEditScientificNumber::ScientificSignColorGet() const
{
	ASSERT_VALID( this );
	return m_clrScientificSign;
}


