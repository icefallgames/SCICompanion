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

#if (!defined __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG)

#if (!defined __EXT_MDI_WINDOWS_LIST_DIALOG_H)
	#include <ExtMdiWindowsListDlg.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#include <Resources/Resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtMdiWindowsListDlg dialog

IMPLEMENT_DYNAMIC( CExtMdiWindowsListDlg, CExtResizableDialog )

CExtMdiWindowsListDlg::CExtMdiWindowsListDlg(
	CMDIFrameWnd * pMDIFrameWnd,
	CWnd * pParent // = NULL
	)
	: __BASEOF_CExtMdiWindowsListDlg__ ( IDD_EXT_MDI_WINDOWS_LIST, pParent )
{
	_Init( pMDIFrameWnd->GetSafeHwnd() );
}

CExtMdiWindowsListDlg::CExtMdiWindowsListDlg(
	HWND hMDIFrameWnd,
	CWnd * pParent // = NULL
	)
	: __BASEOF_CExtMdiWindowsListDlg__ ( IDD_EXT_MDI_WINDOWS_LIST, pParent )
{
	_Init( hMDIFrameWnd );
}

void CExtMdiWindowsListDlg::_Init( HWND hMDIFrameWnd )
{
	m_hIcon = NULL;
	m_hMDIFrameWnd = hMDIFrameWnd;
	ASSERT( m_hMDIFrameWnd != NULL && ::IsWindow(m_hMDIFrameWnd) );
	m_hWndMDIClient =
		CExtControlBar::stat_FindMdiClientHWND(
			m_hMDIFrameWnd
			);
	ASSERT( m_hWndMDIClient != NULL && ::IsWindow(m_hWndMDIClient) );
	m_hIcon = (HICON)(__EXT_MFC_DWORD_PTR)::SendMessage( m_hMDIFrameWnd, WM_GETICON, FALSE, 0 );
	if( m_hIcon == NULL ) 
	{
		m_hIcon = (HICON)(__EXT_MFC_DWORD_PTR)::SendMessage( m_hMDIFrameWnd, WM_GETICON, TRUE, 0 );
		if( m_hIcon == NULL ) 
		{
			m_hIcon = (HICON)(__EXT_MFC_DWORD_PTR)::__EXT_MFC_GetClassLong( m_hMDIFrameWnd, __EXT_MFC_GCL_HICONSM );
			if( m_hIcon == NULL )
				m_hIcon = ::AfxGetApp()->LoadStandardIcon( IDI_WINLOGO );
		} // if( m_hIcon == NULL ) 
	} // if( m_hIcon == NULL ) 
}

void CExtMdiWindowsListDlg::DoDataExchange(CDataExchange* pDX)
{
	__BASEOF_CExtMdiWindowsListDlg__ :: DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtMdiWindowsListDlg)
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_EXT_TILEVERT, m_btnTileVert);
	DDX_Control(pDX, IDC_EXT_TILEHORZ, m_btnTileHorz);
	DDX_Control(pDX, IDC_EXT_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_EXT_MINIMIZE, m_btnMinimize);
	DDX_Control(pDX, IDC_EXT_LISTBOX_WINDOWS, m_wndListWindows);
	DDX_Control(pDX, IDC_EXT_CLOSEWINDOWS, m_btnCloseWindows);
	DDX_Control(pDX, IDC_EXT_CASCADE, m_btnCascade);
	DDX_Control(pDX, IDC_EXT_ACTIVATE, m_btnActivate);
	DDX_Control(pDX, IDC_EXT_STATIC_LISTBOX_WINDOWS, m_labelListWindows);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CExtMdiWindowsListDlg, CExtResizableDialog )
	//{{AFX_MSG_MAP(CExtMdiWindowsListDlg)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_COMPAREITEM()
	ON_LBN_SELCHANGE(IDC_EXT_LISTBOX_WINDOWS, OnSelchangeListboxWindows)
	ON_LBN_DBLCLK(IDC_EXT_LISTBOX_WINDOWS, OnDblclkListboxWindows)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtMdiWindowsListDlg message handlers

BOOL CExtMdiWindowsListDlg::OnInitDialog() 
{
	if( ! __BASEOF_CExtMdiWindowsListDlg__ :: OnInitDialog() )
		return FALSE;
	if( m_hIcon != NULL )
	{
		SetIcon( m_hIcon, TRUE );		// Set big icon
		SetIcon( m_hIcon, FALSE );	// Set small icon
	} // if( m_hIcon != NULL )
	AddAnchor(IDC_EXT_LISTBOX_WINDOWS, __RDA_LT, __RDA_RB);
	AddAnchor(IDC_EXT_ACTIVATE, __RDA_RT);
	AddAnchor(IDOK, __RDA_RT);
	AddAnchor(IDC_EXT_SAVE, __RDA_RT);
	AddAnchor(IDC_EXT_CLOSEWINDOWS, __RDA_RT);
	AddAnchor(IDC_EXT_CASCADE, __RDA_RT);
	AddAnchor(IDC_EXT_TILEHORZ, __RDA_RT);
	AddAnchor(IDC_EXT_TILEVERT, __RDA_RT);
	AddAnchor(IDC_EXT_MINIMIZE, __RDA_RT);
	_FillMDIWindowList();
	_SelectMDIActive();
	OnUpdateButtons();
	return TRUE;
}

void CExtMdiWindowsListDlg::OnUpdateButtons()
{
	int nCount = m_wndListWindows.GetSelCount();
	
	m_btnActivate.EnableWindow(nCount == 1);
	m_btnSave.EnableWindow(nCount > 0);
	m_btnCloseWindows.EnableWindow(nCount > 0);
	m_btnCascade.EnableWindow(nCount > 1);
	m_btnTileVert.EnableWindow(nCount > 1);
	m_btnTileHorz.EnableWindow(nCount > 1);
	m_btnMinimize.EnableWindow(nCount > 0);
}

//////////////////////////////////////////////////////////////////////////
// Draws listbox item
//////////////////////////////////////////////////////////////////////////

void CExtMdiWindowsListDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS) 
{
	if (nIDCtl == IDC_EXT_LISTBOX_WINDOWS)
	{
		if (lpDIS->itemID == LB_ERR)
			return;

		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		CRect rcItem = lpDIS->rcItem;
	
		COLORREF clrBackground = PmBridge_GetPM()->GetColor( COLOR_WINDOW, this );
		COLORREF clrText = PmBridge_GetPM()->GetColor( COLOR_WINDOWTEXT, this );
		
		if( lpDIS->itemState & ODS_SELECTED )
		{ 
			clrText = PmBridge_GetPM()->GetColor( COLOR_HIGHLIGHTTEXT, this ); 
			clrBackground = PmBridge_GetPM()->GetColor( COLOR_HIGHLIGHT, this ); 
		}

		if( lpDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT) )
		{
			CRect rcErase( rcItem );
			if( INT(lpDIS->itemID) == (m_wndListWindows.GetCount()-1) )
			{
				CRect rcClient;
				GetClientRect( &rcClient );
				if( rcErase.bottom < rcClient.bottom )
					rcErase.bottom = rcClient.bottom;
			}
			pDC->FillSolidRect( 
				rcErase, 
				PmBridge_GetPM()->GetColor( COLOR_WINDOW, this )
				);
		}

		CSize szIcon(
			::GetSystemMetrics(SM_CXSMICON),
			::GetSystemMetrics(SM_CYSMICON)
			);
		
		CRect rcText( rcItem );
		rcText.left += szIcon.cx + 3;
		
		if( lpDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT) )
			pDC->FillSolidRect( rcText, clrBackground ); 
		
		if( lpDIS->itemAction & ODA_FOCUS )
		{
			COLORREF clrTextOld = 
				pDC->SetTextColor( RGB(255,255,255) );
			COLORREF clrBkOld =
				pDC->SetBkColor( RGB(0,0,0) );
			pDC->DrawFocusRect( &rcText );
			pDC->SetBkColor( clrBkOld );
			pDC->SetTextColor( clrTextOld );
		}
		
		int	nOldBkMode = pDC->SetBkMode( TRANSPARENT ); 
		COLORREF clrOldText = pDC->SetTextColor( clrText ); 
		
		if( INT(lpDIS->itemID) >= 0 )
		{
			CString sText;
			m_wndListWindows.GetText( INT(lpDIS->itemID), sText );
			if( !sText.IsEmpty() )
			{
				rcItem.left += szIcon.cx + 4;
				CExtRichContentLayout::stat_DrawText(
					pDC->m_hDC,
					LPCTSTR(sText), -1, 
					&rcItem,
					DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE, 0
					);
				rcItem.left = lpDIS->rcItem.left;
			} // if( !sText.IsEmpty() )
		} // if( INT(lpDIS->itemID) >= 0 )
		
		HICON hIcon =  (HICON)(__EXT_MFC_DWORD_PTR)::SendMessage( (HWND)lpDIS->itemData, WM_GETICON, FALSE, 0 );
		if (hIcon == NULL) 
			hIcon = (HICON)(__EXT_MFC_DWORD_PTR)::__EXT_MFC_GetClassLong((HWND)lpDIS->itemData, __EXT_MFC_GCL_HICONSM);
		if (hIcon == NULL) 
			hIcon = AfxGetApp()->LoadStandardIcon(IDI_WINLOGO);

		CRect rcIcon(
			rcItem.left, 
			rcItem.top, 
			rcItem.left + szIcon.cx + 3, 
			rcItem.top + szIcon.cy
			);
		pDC->FillSolidRect( 
			rcIcon, 
			PmBridge_GetPM()->GetColor( COLOR_WINDOW, this ) 
			); 
		
		::DrawIconEx(
			pDC->m_hDC, 
			rcItem.left + 2, 
			rcItem.top, 
			hIcon,
			szIcon.cx, 
			szIcon.cy, 
			0, 
			NULL, 
			DI_NORMAL
			);
		
		pDC->SetTextColor( clrOldText );
		pDC->SetBkMode( nOldBkMode );

		return;
	} // if (nIDCtl == IDC_EXT_LISTBOX_WINDOWS)
	__BASEOF_CExtMdiWindowsListDlg__ :: OnDrawItem(nIDCtl, lpDIS);
}

void CExtMdiWindowsListDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMIS)
{
	if( nIDCtl == IDC_EXT_LISTBOX_WINDOWS )
	{
		if( lpMIS->itemID == LB_ERR )
			return;

		return;
	} // if( nIDCtl == IDC_EXT_LISTBOX_WINDOWS )
	__BASEOF_CExtMdiWindowsListDlg__ :: OnMeasureItem(nIDCtl, lpMIS);
}

int CExtMdiWindowsListDlg::OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCIS)
{
	if (nIDCtl == IDC_EXT_LISTBOX_WINDOWS)
	{
		ASSERT( lpCIS->CtlType == ODT_LISTBOX );
		if(		lpCIS->itemID1 == LB_ERR 
			||	lpCIS->itemID2 == LB_ERR 
			)
			return -1;
		
		CString strItem1, strItem2;
		m_wndListWindows.GetText( lpCIS->itemID1, strItem1 );
		m_wndListWindows.GetText( lpCIS->itemID2, strItem2 );

		return strItem1.Collate( strItem2 );
	} // if (nIDCtl == IDC_EXT_LISTBOX_WINDOWS)
	return __BASEOF_CExtMdiWindowsListDlg__ :: OnCompareItem(nIDCtl, lpCIS);
}

void CExtMdiWindowsListDlg::OnSelchangeListboxWindows() 
{
	OnUpdateButtons();
}

void CExtMdiWindowsListDlg::_FillMDIWindowList()
{
	m_wndListWindows.SetRedraw( FALSE );
	m_wndListWindows.ResetContent();
	
//	CMDIChildWnd * pActiveChild = (CMDIChildWnd*)m_pMDIFrameWnd->MDIGetActive();
//	if( pActiveChild != NULL ) 
//	{
//		CMDIChildWnd * pChildFrame = (CMDIChildWnd*)pActiveChild->GetWindow(GW_HWNDLAST);
//		while( pChildFrame != NULL ) 
//		{
//			HWND hChildFrame = pChildFrame->GetSafeHwnd();
//			CString strWindowText;
//			pChildFrame->GetWindowText( strWindowText );
//			if( strWindowText.IsEmpty() )
//			{
//				CWnd* pMDIChildWnd = pChildFrame->GetWindow(GW_CHILD);
//				while( pMDIChildWnd )
//				{
//					if( pMDIChildWnd->IsKindOf(RUNTIME_CLASS(CView)) ) 
//					{
//						CDocument* pDocument = ((CView*)pMDIChildWnd)->GetDocument();
//						if (pDocument == NULL)
//							strWindowText = _T("");
//						strWindowText = pDocument->GetTitle();
//						pChildFrame->SetWindowText( strWindowText );
//					} // if( pMDIChildWnd->IsKindOf(RUNTIME_CLASS(CView)) ) 
//					pMDIChildWnd = pMDIChildWnd->GetWindow(GW_HWNDNEXT);
//				} // while( pMDIChildWnd )
//			} // if( strWindowText.IsEmpty() )
//			int iIndex = m_wndListWindows.AddString(strWindowText);
//			m_wndListWindows.SetItemData(iIndex, (DWORD)hChildFrame);
//			pChildFrame = (CMDIChildWnd*)pChildFrame->GetWindow(GW_HWNDPREV);
//		} // while( pChildFrame != NULL ) 
//	} // if( pActiveChild != NULL ) 

BOOL bMax = FALSE;
HWND hWndMdiActive = (HWND)
		::SendMessage(
			m_hWndMDIClient,
			WM_MDIGETACTIVE,
			0,
			(LPARAM)&bMax
			);
	if( hWndMdiActive != NULL ) 
	{
		HWND hWndMdiChild =
			::GetWindow(
				hWndMdiActive,
				GW_HWNDLAST
				);
		while( hWndMdiChild != NULL ) 
		{
			if( ::IsWindowVisible( hWndMdiChild ) )
			{
				HWND hChildFrame = hWndMdiChild;
				CString strWindowText;
				CWnd::FromHandle(hWndMdiChild)->GetWindowText( strWindowText );
				if( strWindowText.IsEmpty() )
				{
					HWND hChild = ::GetWindow( hWndMdiChild, GW_CHILD );
					while( hChild != NULL )
					{
						CWnd * pWndPermanent =
							CWnd::FromHandlePermanent( hChild );
						if( pWndPermanent != NULL )
						{
							CView * pView =
								DYNAMIC_DOWNCAST( CView, pWndPermanent );
							if( pView != NULL ) 
							{
								CDocument * pDocument = pView->GetDocument();
								if( pDocument == NULL )
									strWindowText = _T("");
								strWindowText = pDocument->GetTitle();
								::SetWindowText( hWndMdiChild, strWindowText );
							} // if( pView != NULL ) 
						} // if( pWndPermanent != NULL )
						hChild = ::GetWindow( hChild, GW_HWNDNEXT );
					} // while( hChild )
				} // if( strWindowText.IsEmpty() )
				int iIndex = m_wndListWindows.AddString(strWindowText);
				m_wndListWindows.SetItemData( iIndex, (__EXT_MFC_LONG_PTR)hChildFrame );
			}
			hWndMdiChild = ::GetWindow( hWndMdiChild, GW_HWNDPREV );
		} // while( hWndMdiChild != NULL ) 
	} // if( hWndMdiActive != NULL ) 

	m_wndListWindows.SetRedraw( TRUE );
	m_wndListWindows.Invalidate();
}

void CExtMdiWindowsListDlg::_SelectMDIActive()
{
	int nCount = m_wndListWindows.GetCount();

	if(		nCount != LB_ERR 
		&&	nCount > 0
		)
	{
		m_wndListWindows.SelItemRange(FALSE, 0, nCount-1);
		
		BOOL bMax = FALSE;
		HWND hWnd = (HWND)
			::SendMessage(
				m_hWndMDIClient,
				WM_MDIGETACTIVE,
				0,
				(LPARAM)&bMax
				);
		for( int nItem = 0; nItem < nCount; nItem++ ) 
		{
			if( hWnd == (HWND)m_wndListWindows.GetItemData(nItem) ) 
			{
				m_wndListWindows.SetSel( nItem );
				return;
			} // if( hWnd == (HWND)m_wndListWindows.GetItemData(nItem) ) 
		} // for(int nItem = 0; nItem < nCount; nItem++) 
	}
}

void CExtMdiWindowsListDlg::OnDblclkListboxWindows() 
{
	this->SendMessage( WM_COMMAND, IDC_EXT_ACTIVATE );
}

BOOL CExtMdiWindowsListDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if( nCode == CN_COMMAND )
	{
		bool bNeedToUpdateButtons = false;
		int nCount = m_wndListWindows.GetCount();
		HWND hWnd = (HWND)m_wndListWindows.GetItemData( 
			m_wndListWindows.GetCurSel() 
			);
		WINDOWPLACEMENT	wp;

		if(hWnd != (HWND)LB_ERR)
		{
			switch(nID) 
			{
			case IDC_EXT_TILEVERT:
				if( nCount != LB_ERR && nCount > 0 )
				{
					for(int nItem = nCount-1; nItem >= 0; nItem-- )	
					{
						HWND hWnd=(HWND)m_wndListWindows.GetItemData(nItem);
						if( m_wndListWindows.GetSel(nItem) > 0 )
						{
							//::ShowWindow( hWnd, SW_RESTORE );
							::SendMessage( hWnd, WM_SYSCOMMAND, SC_RESTORE, 0L );
						}
						else
						{
							//::ShowWindow( hWnd, SW_MINIMIZE );
							::SendMessage( hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0L );
						}
					} // for(int nItem = nCount-1; nItem >= 0; nItem-- )	
					::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0);	
				} // if( nCount != LB_ERR && nCount > 0 )
				bNeedToUpdateButtons = true;
				break;
				
			case IDC_EXT_TILEHORZ:
				if( nCount != LB_ERR && nCount > 0 )
				{
					for(int nItem = nCount-1; nItem >= 0; nItem-- )	
					{
						HWND hWnd=(HWND)m_wndListWindows.GetItemData(nItem);
						if( m_wndListWindows.GetSel(nItem) > 0 )
						{
							//::ShowWindow( hWnd, SW_RESTORE );
							::SendMessage( hWnd, WM_SYSCOMMAND, SC_RESTORE, 0L );
						}
						else
						{
							//::ShowWindow( hWnd, SW_MINIMIZE );
							::SendMessage( hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0L );
						}
					} // for(int nItem = nCount-1; nItem >= 0; nItem-- )	
					::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);	
				} // if( nCount != LB_ERR && nCount > 0 )
				bNeedToUpdateButtons = true;
				break;
				
			case IDC_EXT_SAVE:
				if( nCount != LB_ERR && nCount > 0 )
				{
					for( int nItem = 0; nItem < nCount; nItem++ ) 
					{
						if( m_wndListWindows.GetSel(nItem) > 0 ) 
						{
							HWND hWnd = (HWND)m_wndListWindows.GetItemData(nItem);
							if( hWnd != NULL ) 
							{
								CMDIChildWnd* pMDIChildWnd = (CMDIChildWnd*)CWnd::FromHandle(hWnd);
								if (!pMDIChildWnd)
									continue;
								CDocument *pDocument = pMDIChildWnd->GetActiveDocument();
								if(pDocument != NULL)
									pDocument->SaveModified();
							} // if( hWnd != NULL )
						} // if( m_wndListWindows.GetSel(nItem) > 0 ) 
					} // for( int nItem = 0; nItem < nCount; nItem++ ) 
				} // if( nCount != LB_ERR && nCount > 0 )
				_FillMDIWindowList();
				_SelectMDIActive();
				bNeedToUpdateButtons = true;
				break;
				
			case IDC_EXT_MINIMIZE:
				if( nCount != LB_ERR && nCount > 0 )
				{
					for(int nItem = nCount-1; nItem >= 0; nItem-- )	
					{
						HWND hWnd=(HWND)m_wndListWindows.GetItemData(nItem);
						if( m_wndListWindows.GetSel(nItem) > 0 )
						{
							//::ShowWindow( hWnd, SW_MINIMIZE );
							::SendMessage( hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0L );
						}
					} // for(int nItem = nCount-1; nItem >= 0; nItem-- )
					::SendMessage(m_hWndMDIClient, WM_MDICASCADE, 0, 0);	
				} // if( nCount != LB_ERR && nCount > 0 )
				bNeedToUpdateButtons = true;
				break;
				
			case IDC_EXT_ACTIVATE:
				::memset( &wp, 0, sizeof(WINDOWPLACEMENT) );
				wp.length = sizeof(WINDOWPLACEMENT);
				::GetWindowPlacement(hWnd, &wp);
				if( wp.showCmd == SW_SHOWMINIMIZED )
				{
					//::ShowWindow( hWnd, SW_RESTORE );
					::SendMessage( hWnd, WM_SYSCOMMAND, SC_RESTORE, 0L );
				}
				::SendMessage( m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)hWnd, 0 );
				__BASEOF_CExtMdiWindowsListDlg__ :: OnOK();
				break;
				
			case IDC_EXT_CLOSEWINDOWS:
				if( nCount != LB_ERR && nCount > 0 )
				{
					for( int nItem = 0; nItem < nCount; nItem++ ) 
					{
						if( m_wndListWindows.GetSel(nItem) > 0 ) 
						{
							HWND hWnd = (HWND)m_wndListWindows.GetItemData(nItem);
							if( hWnd != NULL )
								::PostMessage(hWnd, WM_CLOSE, 0,0);
						} // if( m_wndListWindows.GetSel(nItem) > 0 ) 
					} // for( int nItem = 0; nItem < nCount; nItem++ ) 
				} // if( nCount != LB_ERR && nCount > 0 )
				CExtPopupMenuWnd::PassMsgLoop( false );
				_FillMDIWindowList();
				_SelectMDIActive();
				bNeedToUpdateButtons = true;
				break;
				
			case IDC_EXT_CASCADE:
				if( nCount != LB_ERR && nCount > 0 )
				{
					for(int nItem = nCount-1; nItem >= 0; nItem-- )	
					{
						HWND hWnd=(HWND)m_wndListWindows.GetItemData(nItem);
						if( m_wndListWindows.GetSel(nItem) > 0 )
						{
							//::ShowWindow( hWnd, SW_RESTORE );
							::SendMessage( hWnd, WM_SYSCOMMAND, SC_RESTORE, 0L );
						}
						else
						{
							//::ShowWindow( hWnd, SW_MINIMIZE );
							::SendMessage( hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0L );
						}
					} // for(int nItem = nCount-1; nItem >= 0; nItem-- )	
					::SendMessage(m_hWndMDIClient, WM_MDICASCADE, 0, 0);	
				} // if( nCount != LB_ERR && nCount > 0 )
				bNeedToUpdateButtons = true;
				break;
			}
			if( bNeedToUpdateButtons )
				OnUpdateButtons();
		} // if(hWnd != (HWND)LB_ERR)
	} // if( nCode == CN_COMMAND )
	return __BASEOF_CExtMdiWindowsListDlg__ :: OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

#endif // (!defined __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG)
