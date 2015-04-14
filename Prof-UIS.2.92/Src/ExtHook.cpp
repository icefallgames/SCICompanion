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

#if (!defined __EXT_HOOK_H)
	#include "ExtHook.h"
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef
	CMap < HWND, HWND,
		CExtHookSink::HookChains_t *, CExtHookSink::HookChains_t * >
	HookChainsMap_t;

static HookChainsMap_t g_HookChainsMap;

struct __PROF_UIS_API CExtHookSink::HookSinkArray_t :
	public CArray< CExtHookSink *, CExtHookSink * >
{
	INT Find( const CExtHookSink * pHookSink ) const
	{
		ASSERT( pHookSink != NULL );
		for( INT nSinkIdx = 0; nSinkIdx < GetSize(); nSinkIdx++ )
		{
			const CExtHookSink * pHookSinkExamine =
				GetAt( nSinkIdx );
			ASSERT( pHookSink != NULL );
			if( pHookSinkExamine == pHookSink )
				return nSinkIdx;
		}
		return -1;
	}
	void AddHead( CExtHookSink * pHookSink )
	{
		ASSERT( pHookSink != NULL );
		InsertAt( 0, pHookSink );
	}
	void AddTail( CExtHookSink * pHookSink )
	{
		ASSERT( pHookSink != NULL );
		InsertAt( GetSize(), pHookSink );
	}
}; // struct HookSinkArray_t

struct __PROF_UIS_API CExtHookSink::HookChains_t
{
	bool m_bEatNcDestroy:1;
	HookSinkArray_t m_HookSinkArray;
	HWND m_hWndHooked;
	WNDPROC m_pWNDPROC;

	static LRESULT CALLBACK g_HookWndProc(
		HWND hWnd,
		UINT nMessage,
		WPARAM wParam,
		LPARAM lParam
		);

	HookChains_t(
		HWND hWndHooked
		)
		: m_hWndHooked( hWndHooked )
		, m_bEatNcDestroy( false )
	{
		ASSERT( m_hWndHooked != NULL );
		ASSERT( ::IsWindow(m_hWndHooked) );
		m_pWNDPROC = (WNDPROC)(__EXT_MFC_LONG_PTR)
			::__EXT_MFC_SetWindowLong(
				m_hWndHooked,
				__EXT_MFC_GWL_WNDPROC,
#if defined(_WIN64)
				(__EXT_MFC_LONG_PTR)
#else
				(LONG)(__EXT_MFC_LONG_PTR)
#endif
					g_HookWndProc
				);
//		ASSERT( m_pWNDPROC != NULL );
	};

	~HookChains_t()
	{
		DestroyChains( false );
	};

	void DestroyChains( bool bDelete )
	{
		for( int nSinkIdx=0; nSinkIdx < m_HookSinkArray.GetSize(); nSinkIdx++ )
		{
			CExtHookSink * pHookSink =
				m_HookSinkArray[ nSinkIdx ];
			ASSERT( pHookSink != NULL );
			if( pHookSink->IsAutoDeleteHookWndSink() )
				delete pHookSink;
		} // for( int nSinkIdx=0; nSinkIdx < m_HookSinkArray.GetSize(); nSinkIdx++ )
		m_HookSinkArray.RemoveAll();

		ASSERT( m_hWndHooked != NULL );
		ASSERT( ::IsWindow(m_hWndHooked) );
		if( m_pWNDPROC != NULL )
			::__EXT_MFC_SetWindowLong(
				m_hWndHooked,
				__EXT_MFC_GWL_WNDPROC,
#if defined(_WIN64)
				(__EXT_MFC_LONG_PTR)
#else
				(LONG)(__EXT_MFC_LONG_PTR)
#endif
					m_pWNDPROC
				);

		POSITION posChains = g_HookChainsMap.GetStartPosition();
		for( ; posChains != NULL; )
		{
			CExtHookSink::HookChains_t * pHookChains = NULL;
			HWND hWndHooked = NULL;
			g_HookChainsMap.GetNextAssoc(
				posChains, hWndHooked, pHookChains );
			ASSERT( hWndHooked != NULL );
			ASSERT( pHookChains != NULL );
			if( pHookChains == this )
			{
				g_HookChainsMap.RemoveKey( hWndHooked );
				break;
			}
		}

		if( bDelete )
			delete this;
	};

	LRESULT HookChainsWindowProc(
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		)
	{
		__PROF_UIS_MANAGE_STATE;
		ASSERT( m_hWndHooked != NULL );
		ASSERT( ::IsWindow(m_hWndHooked) );
		ASSERT( m_pWNDPROC != NULL );
//		CWnd * pWndPermanent = NULL;
//		{ // BLOCK: state managing
//			__PROF_UIS_MANAGE_STATE;
//			pWndPermanent = 
//				CWnd::FromHandlePermanent( m_hWndHooked );
//		} // BLOCK: state managing
		bool bEatNcDestroy = m_bEatNcDestroy;
		int nSinkIdx, nSinkCount = (int)m_HookSinkArray.GetSize();
		for( nSinkIdx = 0; nSinkIdx < nSinkCount; nSinkIdx++ )
		{
			CExtHookSink * pHookSink =
				m_HookSinkArray[ nSinkIdx ];
			ASSERT( pHookSink != NULL );
			if( nMessage == WM_NCDESTROY ) 
			{
				if(		(! bEatNcDestroy )
					&&	pHookSink->m_bEatNcDestroy
					)
					bEatNcDestroy = true;
				if(! bEatNcDestroy )
					pHookSink->OnHookWndNcDestroy();
				continue;
			} // if( nMessage == WM_NCDESTROY ) 
			LRESULT lResult = 0;
			if(	pHookSink->OnHookWndMsg(
					lResult,
					m_hWndHooked,
					nMessage,
					wParam,
					lParam
					)
				)
				return lResult;
		} // for( nSinkIdx = 0; nSinkIdx < nSinkCount; nSinkIdx++ )
		WNDPROC pWNDPROC = m_pWNDPROC;
		HWND hWndHooked = m_hWndHooked;
		if( nMessage == WM_NCDESTROY ) 
		{
			DestroyChains( true );
//			if( pWndPermanent != NULL )
//			{
//				__PROF_UIS_MANAGE_STATE;
//				CWnd * pWndExamine =
//					CWnd::FromHandlePermanent( hWndHooked );
//				if( pWndExamine != pWndPermanent )
//					return 0L;
//			} // if( pWndPermanent != NULL )
			if( bEatNcDestroy )
				return 0L;
		} // if( nMessage == WM_NCDESTROY ) 
		LRESULT lResult =
			::CallWindowProc(
				pWNDPROC,
				hWndHooked,
				nMessage,
				wParam,
				lParam
				);
		if( ::IsWindow( hWndHooked ) )
		{
			CExtHookSink::HookChains_t * pHC = NULL;
			if(		g_HookChainsMap.Lookup( hWndHooked, pHC )
				&&	pHC == this
				)
			{
				nSinkCount = (int)m_HookSinkArray.GetSize();
				for( nSinkIdx = 0; nSinkIdx < nSinkCount; nSinkIdx++ )
				{
					CExtHookSink * pHookSink =
						m_HookSinkArray[ nSinkIdx ];
					if( pHookSink == NULL )
						continue;
					pHookSink->OnPostHookWndMsg(
						lResult,
						m_hWndHooked,
						nMessage,
						wParam,
						lParam
						);
					if( ! ::IsWindow( hWndHooked ) )
						break;
					pHC = NULL;
					if( ! g_HookChainsMap.Lookup( hWndHooked, pHC ) )
						break;
					if( pHC != this )
						break;
				} // for( nSinkIdx = 0; nSinkIdx < nSinkCount; nSinkIdx++ )
			}
		} // if( ::IsWindow( hWndHooked ) )
		return lResult;
	};

}; // struct CExtHookSink::HookChains_t

LRESULT CALLBACK CExtHookSink::HookChains_t::g_HookWndProc(
	HWND hWnd,
	UINT nMessage,
	WPARAM wParam,
	LPARAM lParam
	)
{

LRESULT lResult = 0;

MSG & refMsgMfcCurr = AfxGetThreadState()->m_lastSentMsg;
MSG msgMfcSaved( refMsgMfcCurr );
	refMsgMfcCurr.hwnd    = hWnd;
	refMsgMfcCurr.message = nMessage;
	refMsgMfcCurr.wParam  = wParam;
	refMsgMfcCurr.lParam  = lParam;

CExtHookSink::HookChains_t * pHookChains = NULL;
	if( g_HookChainsMap.Lookup( hWnd, pHookChains ) )
	{
		ASSERT( pHookChains != NULL );
		ASSERT( pHookChains->m_hWndHooked == hWnd );
		lResult =
			pHookChains->HookChainsWindowProc(
				nMessage,
				wParam,
				lParam
				);  
		if( nMessage == WM_NCDESTROY || ( ! ::IsWindow( hWnd ) ) ) 
			g_HookChainsMap.RemoveKey( hWnd );
	} // if( g_HookChainsMap.Lookup( hWnd, pHookChains ) )

	refMsgMfcCurr = msgMfcSaved;
	
	return lResult;
}

CExtHookSink::CExtHookSink(
	bool bEnableDetailedWndHooks // = true
	)
	: m_bEnableDetailedWndHooks( bEnableDetailedWndHooks )
	, m_bEatNcDestroy( false )
{
}

CExtHookSink::~CExtHookSink()
{
POSITION posChains = g_HookChainsMap.GetStartPosition();
	for( ; posChains != NULL; )
	{
		CExtHookSink::HookChains_t * pHookChains = NULL;
		HWND hWndHooked = NULL;
		g_HookChainsMap.GetNextAssoc(
			posChains, hWndHooked, pHookChains );
		ASSERT( hWndHooked != NULL );
		ASSERT( pHookChains != NULL );
//		__try
//		{
			INT nOwnIdx = pHookChains->m_HookSinkArray.Find( this );
			if( nOwnIdx < 0 )
				continue;
			pHookChains->m_HookSinkArray.RemoveAt( nOwnIdx );
//		}
//		__except( EXCEPTION_EXECUTE_HANDLER )
//		{
//		}
	} // for( ; pos != NULL; )
}

LRESULT CExtHookSink::OnHookWndMsgNextProcInvoke(
	UINT nMessage,
	WPARAM wParam,
	LPARAM lParam
	)
{
MSG & msgCurrMfc = AfxGetThreadState()->m_lastSentMsg;
	ASSERT( msgCurrMfc.hwnd != NULL );
	ASSERT( ::IsWindow( msgCurrMfc.hwnd ) );

CExtHookSink::HookChains_t * pHookChains = NULL;
	VERIFY(
		g_HookChainsMap.Lookup(
			msgCurrMfc.hwnd,
			pHookChains
			)
		);
	ASSERT( pHookChains != NULL );
	ASSERT( pHookChains->m_hWndHooked == msgCurrMfc.hwnd );
	ASSERT( pHookChains->m_pWNDPROC != NULL );

	return
		::CallWindowProc(
			pHookChains->m_pWNDPROC,
			msgCurrMfc.hwnd,
			nMessage,
			wParam,
			lParam
			);
}

LRESULT CExtHookSink::OnHookWndMsgNextProcCurrent(
	WPARAM wParam,
	LPARAM lParam
	)
{
MSG & msgCurrMfc = AfxGetThreadState()->m_lastSentMsg;
	ASSERT( msgCurrMfc.hwnd != NULL );
	ASSERT( ::IsWindow( msgCurrMfc.hwnd ) );

CExtHookSink::HookChains_t * pHookChains = NULL;
	VERIFY(
		g_HookChainsMap.Lookup(
			msgCurrMfc.hwnd,
			pHookChains
			)
		);
	ASSERT( pHookChains != NULL );
	ASSERT( pHookChains->m_hWndHooked == msgCurrMfc.hwnd );
	ASSERT( pHookChains->m_pWNDPROC != NULL );

	return
		::CallWindowProc(
			pHookChains->m_pWNDPROC,
			msgCurrMfc.hwnd,
			msgCurrMfc.message,
			wParam,
			lParam
			);
}

LRESULT CExtHookSink::OnHookWndMsgDefault()
{
MSG & msgCurrMfc = AfxGetThreadState()->m_lastSentMsg;
	ASSERT( msgCurrMfc.hwnd != NULL );
	ASSERT( ::IsWindow( msgCurrMfc.hwnd ) );

CExtHookSink::HookChains_t * pHookChains = NULL;
	VERIFY(
		g_HookChainsMap.Lookup(
			msgCurrMfc.hwnd,
			pHookChains
			)
		);
	ASSERT( pHookChains != NULL );
	ASSERT( pHookChains->m_hWndHooked == msgCurrMfc.hwnd );
	ASSERT( pHookChains->m_pWNDPROC != NULL );

	return
		::CallWindowProc(
			pHookChains->m_pWNDPROC,
			msgCurrMfc.hwnd,
			msgCurrMfc.message,
			msgCurrMfc.wParam,
			msgCurrMfc.lParam
			);
}

bool CExtHookSink::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	lResult;
	hWndHooked;
	nMessage;
	wParam;
	lParam;

	if( ! m_bEnableDetailedWndHooks )
		return false;

	switch( nMessage )
	{
	case WM_COMMAND:
		return
			OnHookCmdMsg(
				lResult,
				hWndHooked,
				HIWORD(wParam),
				LOWORD(wParam),
				(HWND)lParam
				);
	case WM_NOTIFY:
		return
			OnHookNotifyMsg(
				lResult,
				hWndHooked,
				(INT)wParam,
				(LPNMHDR)lParam
				);
	case WM_PAINT:
		return
			OnHookPaintMsg(
				lResult,
				hWndHooked,
				(HDC)wParam
				);
	case WM_ERASEBKGND:
		return
			OnHookEraseBackgroundMsg(
				lResult,
				hWndHooked,
				(HDC)wParam
				);
	case WM_PRINT:
	case WM_PRINTCLIENT:
		return
			OnHookPrintMsg(
				lResult,
				hWndHooked,
				(HDC)wParam
				);
	case WM_NCPAINT:
		return
			OnHookNcPaintMsg(
				lResult,
				hWndHooked,
				(HRGN)wParam
				);
	} // switch( nMessage )

	return false;
}

void CExtHookSink::OnPostHookWndMsg(
	LRESULT lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM wParam,
	LPARAM lParam
	)
{
	lResult;
	hWndHooked;
	nMessage;
	wParam;
	lParam;
}

bool CExtHookSink::OnHookCmdMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	WORD wNotifyCode,
	WORD wID,
	HWND hWndCtrl
	)
{
	lResult;
	hWndHooked;
	wNotifyCode;
	wID;
	hWndCtrl;
	
	return false;
}

bool CExtHookSink::OnHookNotifyMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	INT nIdCtrl,
	LPNMHDR lpnmhdr
	)
{
	lResult;
	hWndHooked;
	nIdCtrl;
	lpnmhdr;
	
	return false;
}

bool CExtHookSink::OnHookPaintMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	HDC hDC
	)
{
	lResult;
	hWndHooked;
	hDC;
	
	return false;
}

bool CExtHookSink::OnHookEraseBackgroundMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	HDC hDC
	)
{
	lResult;
	hWndHooked;
	hDC;
	
	return false;
}

bool CExtHookSink::OnHookPrintMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	HDC hDC
	)
{
	lResult;
	hWndHooked;
	hDC;
	
	return false;
}

bool CExtHookSink::OnHookNcPaintMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	HRGN hRgnUpdate
	)
{
	lResult;
	hWndHooked;
	hRgnUpdate;
	
	return false;
}

void CExtHookSink::OnHookWndNcDestroy()
{
}

void CExtHookSink::OnHookWndAttach( HWND hWnd )
{
	ASSERT( hWnd != NULL );
	hWnd;
}

void CExtHookSink::OnHookWndDetach( HWND hWnd )
{
	ASSERT( hWnd != NULL );
	hWnd;
}

bool CExtHookSink::IsAutoDeleteHookWndSink()
{
	return false;
}

bool CExtHookSink::SetupHookWndSink(
	HWND hWnd,
	bool bRemove, // = false
	bool bAddToHead // = false
	)
{
	ASSERT( hWnd != NULL );
	if( hWnd == NULL )
		return false;

	ASSERT( bRemove || (!bRemove && ::IsWindow(hWnd)) );
	if( (!bRemove) && (!::IsWindow(hWnd) ) )
		return false;

CExtHookSink::HookChains_t * pHookChains = NULL;
	if( !g_HookChainsMap.Lookup( hWnd, pHookChains ) )
	{
		ASSERT( pHookChains == NULL );
	}
	else
	{
		ASSERT( pHookChains != NULL );
	}
	
	if( bRemove )
	{
		if( pHookChains == NULL )
			return true;
		INT pos =
			pHookChains->m_HookSinkArray.Find( this );
		if( pos < 0 )
			return true;

		OnHookWndDetach( hWnd );

		pHookChains->m_HookSinkArray.RemoveAt( pos );
		if( IsAutoDeleteHookWndSink() )
			delete this;
		
if( pHookChains->m_HookSinkArray.GetSize() == 0 )
	pHookChains->DestroyChains( true );

		return true;
	} // if( bRemove )
	
	if( pHookChains == NULL )
	{
		pHookChains =
			new CExtHookSink::HookChains_t( hWnd );
		g_HookChainsMap.SetAt( hWnd, pHookChains );
	} // if( pHookChains == NULL )
	else
	{
		INT pos =
			pHookChains->m_HookSinkArray.Find( this );
		if( pos >= 0 )
			return true;
	} // else from if( pHookChains == NULL )

	if( bAddToHead )
		pHookChains->m_HookSinkArray.AddHead( this );
	else
		pHookChains->m_HookSinkArray.AddTail( this );

	OnHookWndAttach( hWnd );

	return true;
}

ULONG CExtHookSink::SetupHookWndSinkToChilds(
	HWND hWnd,
	UINT * pDlgCtrlIDs, // = NULL
	ULONG nCountOfDlgCtrlIDs, // = 0
	bool bDeep // = false
	)
{
	ASSERT( hWnd != NULL );
	if( hWnd == NULL )
		return 0;

	ASSERT( ::IsWindow(hWnd) );
	if( !::IsWindow(hWnd) )
		return 0;
ULONG nCountOfHooks = 0;
	hWnd = ::GetWindow( hWnd, GW_CHILD );
	for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
	{
		ASSERT(
			(nCountOfDlgCtrlIDs == 0 && pDlgCtrlIDs == NULL)
			|| (nCountOfDlgCtrlIDs > 0 && pDlgCtrlIDs != NULL)
			);
		bool bSetupHook = true;
		if( nCountOfDlgCtrlIDs > 0 && pDlgCtrlIDs != NULL )
		{
			bSetupHook = false;
			UINT nDlgCtrlID = ::GetDlgCtrlID( hWnd );
			for( ULONG i=0; i<nCountOfDlgCtrlIDs; i++ )
			{
				if( pDlgCtrlIDs[i] == nDlgCtrlID )
				{
					bSetupHook = true;
					break;
				}
			} // for( ULONG i=0; i<nCountOfDlgCtrlIDs; i++ )
		} // if( nCountOfDlgCtrlIDs > 0 && pDlgCtrlIDs != NULL )
		if( bSetupHook )
		{
			if(	SetupHookWndSink( hWnd ) )
				nCountOfHooks++;
			else
			{
				ASSERT( FALSE );
			}
		} // if( bSetupHook )
		if( bDeep )
			nCountOfHooks +=
				SetupHookWndSinkToChilds(
					hWnd,
					pDlgCtrlIDs,
					nCountOfDlgCtrlIDs,
					bDeep
					);
	} // for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
	return nCountOfHooks;
}

void CExtHookSink::RemoveAllWndHooks()
{
HookedWndList_t _list;
	GetHookedWindows( _list );
POSITION pos = _list.GetHeadPosition();
	for( int nHwndIdx = 0; nHwndIdx < _list.GetCount(); nHwndIdx++ )
	{
		HWND hWndHooked = _list.GetNext( pos );
		VERIFY( SetupHookWndSink( hWndHooked, true ) );
	} // for( int nHwndIdx = 0; nHwndIdx < _list.GetCount(); nHwndIdx++ )
}

void CExtHookSink::GetHookedWindows( HookedWndList_t & _list )
{
	_list.RemoveAll();
POSITION posChains = g_HookChainsMap.GetStartPosition();
	for( ; posChains != NULL; )
	{
		CExtHookSink::HookChains_t * pHookChains = NULL;
		HWND hWndHooked = NULL;
		g_HookChainsMap.GetNextAssoc(
			posChains, hWndHooked, pHookChains );
		ASSERT( hWndHooked != NULL );
		ASSERT( pHookChains != NULL );
		ASSERT( pHookChains->m_hWndHooked == hWndHooked );
		if( pHookChains->m_HookSinkArray.Find(this) < 0 )
			continue;
		ASSERT( _list.Find(pHookChains->m_hWndHooked) == NULL );
		_list.AddTail( pHookChains->m_hWndHooked );
	} // for( ; pos != NULL; )
}

bool CExtHookSink::IsHookedWindow( HWND hWnd )
{
CExtHookSink::HookChains_t * pHookChains = NULL;
	if( !g_HookChainsMap.Lookup( hWnd, pHookChains ) )
		return false;
	ASSERT( pHookChains != NULL );
	ASSERT( pHookChains->m_hWndHooked == hWnd );
	if( pHookChains->m_HookSinkArray.Find(this) >= 0 )
		return true;
	return false;
}

CExtHookSpy::HookSpyItem_t CExtHookSpy::g_arr[ __EHS_IDX_COUNT ] =
{
	__EHSEF_MOUSE_ALL,
	__EHSEF_KEYBOARD,
	__EHSEF_GET_MSG,
	__EHSEF_SYS_MSG,
	__EHSEF_WND_PROC_IN,
	__EHSEF_WND_PROC_OUT,
};

CExtHookSpy::HookSpyItem_t::HookSpyItem_t( DWORD dwEHSEF )
	: m_hHook( NULL )
	, m_bListChanged( false )
	, m_dwEHSEF( dwEHSEF )
{
}

bool CExtHookSpy::HookSpyItem_t::IsRegistered( CExtHookSpy * pHS, DWORD dwEHSEF )
{
	dwEHSEF;
bool bVal = false;
bool bRetVal = m_map.Lookup( pHS, bVal ) ? true : false;

/*
#if (defined _DEBUG)
DWORD dwEffectiveEHSEF = dwEHSEF & m_dwEHSEF;
DWORD dwCurrentEHSEF = pHS->HookSpyGetEHSEF();
DWORD dwCheckEHSEF = dwCurrentEHSEF & __EHSEF_BASIC_HOOK_FILTERING;
	if( bRetVal )
	{
		ASSERT( ( dwCheckEHSEF & dwEffectiveEHSEF ) != 0 );
		ASSERT( m_list.Find( pHS ) != NULL );
	}
	else
	{
		ASSERT( ( dwCheckEHSEF & dwEffectiveEHSEF ) == 0 );
		ASSERT( m_list.Find( pHS ) == NULL );
	}
#endif // (defined _DEBUG)
*/

	return bRetVal;
}

void CExtHookSpy::HookSpyItem_t::Register( CExtHookSpy * pHS, DWORD dwEHSEF, bool bAddToHead )
{
	if( pHS == NULL )
		return;
DWORD dwEffectiveEHSEF = dwEHSEF & m_dwEHSEF;
	if( dwEffectiveEHSEF == 0 )
		return;
bool bRegistered = IsRegistered( pHS, m_dwEHSEF );
	pHS->HookSpyModifyEHSEF( 0, dwEffectiveEHSEF );
	if( bRegistered )
		return;
INT nPrevRegisteredCount = INT( m_list.GetCount() );
	ASSERT( nPrevRegisteredCount >= 0 );
	if( bAddToHead )
		m_list.AddHead( pHS );
	else
		m_list.AddTail( pHS );
	m_map.SetAt( pHS, false );
	m_bListChanged = true;
	ASSERT( (nPrevRegisteredCount+1) == INT( m_list.GetCount() ) );
	ASSERT( (nPrevRegisteredCount+1) == INT( m_map.GetCount() ) );
	if( nPrevRegisteredCount == 0 )
		Hook( true );
}

void CExtHookSpy::HookSpyItem_t::Unregister( CExtHookSpy * pHS, DWORD dwEHSEF )
{
	if( pHS == NULL || ( m_dwEHSEF & dwEHSEF ) == 0 )
		return;
#if (defined _DEBUG)
bool bRegistered = IsRegistered( pHS, m_dwEHSEF );
#endif // (defined _DEBUG)
	if( ( pHS->HookSpyGetEHSEF() & m_dwEHSEF ) == 0 )
	{
		ASSERT( ! bRegistered );
		return;
	}
	ASSERT( bRegistered );
DWORD dwEffectiveEHSEF = dwEHSEF & m_dwEHSEF;
	pHS->HookSpyModifyEHSEF( dwEffectiveEHSEF, 0 );
	if( ( pHS->HookSpyGetEHSEF() & m_dwEHSEF ) != 0 )
		return;
POSITION pos = m_list.Find( pHS );
	ASSERT( pos != NULL );
	m_list.RemoveAt( pos );
	m_map.RemoveKey( pHS );
	m_bListChanged = true;
INT nRegisteredCount = INT( m_list.GetCount() );
	ASSERT( nRegisteredCount >= 0 );
	ASSERT( nRegisteredCount == INT( m_map.GetCount() ) );
	if( nRegisteredCount == 0 )
		Hook( false );
}

void CExtHookSpy::HookSpyItem_t::Hook( bool bHook )
{
	if( bHook )
	{
		if( m_hHook != NULL )
			return;
		if( ( m_dwEHSEF & __EHSEF_MOUSE_ALL ) != 0 )
		{
			m_hHook =
				::SetWindowsHookEx(
					WH_MOUSE,
					SHS_HookMouseProc, 
					0,
					::GetCurrentThreadId()
					);
//			ASSERT( m_hHook != NULL );
		}
		else
		if( ( m_dwEHSEF & __EHSEF_KEYBOARD ) != 0 )
		{
			m_hHook =
				::SetWindowsHookEx(
					WH_KEYBOARD,
					SHS_HookKeyboardProc, 
					0,
					::GetCurrentThreadId()
					);
//			ASSERT( m_hHook != NULL );
		}
		else
		if( ( m_dwEHSEF & __EHSEF_GET_MSG ) != 0 )
		{
			m_hHook =
				::SetWindowsHookEx(
					WH_GETMESSAGE,
					SHS_HookGetMsgProc, 
					0,
					::GetCurrentThreadId()
					);
//			ASSERT( m_hHook != NULL );
		}
		else
		if( ( m_dwEHSEF & __EHSEF_SYS_MSG ) != 0 )
		{
			m_hHook =
				::SetWindowsHookEx(
					WH_SYSMSGFILTER,
					SHS_HookSysMsgProc, 
					0,
					::GetCurrentThreadId()
					);
//			ASSERT( m_hHook != NULL );
		}
		else
		if( ( m_dwEHSEF & __EHSEF_WND_PROC_IN ) != 0 )
		{
			m_hHook =
				::SetWindowsHookEx(
					WH_CALLWNDPROC,
					SHS_HookCallWindowProcIn, 
					0,
					::GetCurrentThreadId()
					);
//			ASSERT( m_hHook != NULL );
		}
		else
		if( ( m_dwEHSEF & __EHSEF_WND_PROC_OUT ) != 0 )
		{
			m_hHook =
				::SetWindowsHookEx(
					WH_CALLWNDPROCRET,
					SHS_HookCallWindowProcOut, 
					0,
					::GetCurrentThreadId()
					);
//			ASSERT( m_hHook != NULL );
		}
#if (defined _DEBUG)
		else
		{
			ASSERT( FALSE );
		}
#endif // (defined _DEBUG)
	} // if( bHook )
	else
	{
		if( m_hHook == NULL )
			return;
		::UnhookWindowsHookEx( m_hHook );
		m_hHook = NULL;
	} // else from if( bHook )
}

CExtHookSpy::CExtHookSpy(
	DWORD dwEHSEF // = 0 // auto-register flags, zero - unregister
	)
	: m_bHookConvertLocationsToClient( false )
	, m_dwEHSEF( 0 )
{
	if( dwEHSEF != 0 )
		HookSpyRegister( dwEHSEF );
}

CExtHookSpy::~CExtHookSpy()
{
	HookSpyUnregister();
}

LRESULT CALLBACK CExtHookSpy::HookSpyItem_t::SHS_HookMouseProc(
	int nCode,      // hook code
	WPARAM wParam,  // message identifier
	LPARAM lParam   // mouse coordinates
	)
{
__PROF_UIS_MANAGE_STATE;
MOUSEHOOKSTRUCT * lpMS = (MOUSEHOOKSTRUCT*)lParam;
	ASSERT( lpMS != NULL );
	if( nCode == HC_ACTION )
	{
		POSITION pos = g_arr[__EHS_IDX_MOUSE].m_list.GetHeadPosition();
		for( g_arr[__EHS_IDX_MOUSE].m_bListChanged = false; pos != NULL; )
		{
			CExtHookSpy * pHS = g_arr[__EHS_IDX_MOUSE].m_list.GetNext( pos );
			ASSERT( pHS != NULL );
			DWORD dwEHSEF = pHS->HookSpyGetEHSEF();
			if( (  dwEHSEF & __EHSEF_MOUSE_ALL ) == 0 )
				continue;
			bool bNC = false;
			switch( wParam )
			{
			case WM_NCMOUSEMOVE:
				bNC = true;
			case WM_MOUSEMOVE:
				if( (  dwEHSEF & ( bNC ? __EHSEF_MOUSE_MOVE_NC : __EHSEF_MOUSE_MOVE ) ) == 0 )
					continue;
				else
				{
					CPoint point = lpMS->pt; // screen coordinates
					if( pHS->m_bHookConvertLocationsToClient && (! bNC ) )
						::ScreenToClient( lpMS->hwnd, &point );
					if(	pHS->
							HSLL_OnMouseMove(
								lpMS->hwnd,
								0, // wParam,
								point
								)
						)
						return 1; // eat!
				}
			break;

			case WM_MOUSEWHEEL:
				if( (  dwEHSEF & __EHSEF_MOUSE_WHEEL ) == 0 )
					continue;
				else
					if(	pHS->
							HSLL_OnMouseWheel(
								wParam,
								lParam
								)
						)
						return 1; // eat!
			break;

			case WM_NCLBUTTONDBLCLK:
			case WM_NCRBUTTONDBLCLK:
			case WM_NCMBUTTONDBLCLK:

			case WM_NCLBUTTONDOWN:
			case WM_NCRBUTTONDOWN:
			case WM_NCMBUTTONDOWN:

			case WM_NCLBUTTONUP:
			case WM_NCRBUTTONUP:
			case WM_NCMBUTTONUP:
				bNC = true;

			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
				if( (  dwEHSEF & ( bNC ? __EHSEF_MOUSE_CLICK_NC : __EHSEF_MOUSE_CLICK ) ) == 0 )
					continue;
				else
				{
					CPoint point = lpMS->pt; // screen coordinates
					if( pHS->m_bHookConvertLocationsToClient && (! bNC ) )
						::ScreenToClient( lpMS->hwnd, &point );
					if(	pHS->
							HSLL_OnMouseClick(
								lpMS->hwnd,
								UINT(wParam),
								0,
								point
								)
						)
						return 1; // eat!
				}
			break;
			} // switch( wParam )
			if( g_arr[__EHS_IDX_MOUSE].m_bListChanged )
			{
				g_arr[__EHS_IDX_MOUSE].m_bListChanged = false;
				pos = g_arr[__EHS_IDX_MOUSE].m_list.GetHeadPosition();
			} // if( g_arr[__EHS_IDX_MOUSE].m_bListChanged )
		} // for( g_arr[__EHS_IDX_MOUSE].m_bListChanged = false; pos != NULL; )
		g_arr[__EHS_IDX_MOUSE].m_bListChanged = false;
	} // if( nCode == HC_ACTION )
	return ::CallNextHookEx( g_arr[__EHS_IDX_MOUSE].m_hHook, nCode, wParam, lParam );
}

LRESULT CALLBACK CExtHookSpy::HookSpyItem_t::SHS_HookKeyboardProc(
	int nCode,      // hook code
	WPARAM wParam,  // virtual-key code
	LPARAM lParam   // keystroke-message information
	)
{
__PROF_UIS_MANAGE_STATE;
	if( nCode == HC_ACTION )
	{
		POSITION pos = g_arr[__EHS_IDX_KEYBOARD].m_list.GetHeadPosition();
		for( g_arr[__EHS_IDX_KEYBOARD].m_bListChanged = false; pos != NULL; )
		{
			CExtHookSpy * pHS = g_arr[__EHS_IDX_KEYBOARD].m_list.GetNext( pos );
			ASSERT( pHS != NULL );
			DWORD dwEHSEF = pHS->HookSpyGetEHSEF();
			if( (  dwEHSEF & __EHSEF_KEYBOARD ) == 0 )
				continue;
			if(	pHS->
					HSLL_OnKey(
						UINT(wParam),
						LOWORD(lParam),
						HIWORD(lParam)
						)
				)
				return 1; // eat!
			if( g_arr[__EHS_IDX_KEYBOARD].m_bListChanged )
			{
				g_arr[__EHS_IDX_KEYBOARD].m_bListChanged = false;
				pos = g_arr[__EHS_IDX_KEYBOARD].m_list.GetHeadPosition();
			} // if( g_arr[__EHS_IDX_KEYBOARD].m_bListChanged )
		} // for( g_arr[__EHS_IDX_KEYBOARD].m_bListChanged = false; pos != NULL; )
		g_arr[__EHS_IDX_KEYBOARD].m_bListChanged = false;
	} // if( nCode == HC_ACTION )
	return ::CallNextHookEx( g_arr[__EHS_IDX_KEYBOARD].m_hHook, nCode, wParam, lParam );
}

LRESULT CALLBACK CExtHookSpy::HookSpyItem_t::SHS_HookGetMsgProc(
	int nCode,
	WPARAM wParam,
	LPARAM lParam
	)
{
__PROF_UIS_MANAGE_STATE;
POSITION pos = g_arr[__EHS_IDX_GET_MSG].m_list.GetHeadPosition();
	for( g_arr[__EHS_IDX_GET_MSG].m_bListChanged = false; pos != NULL; )
	{
		CExtHookSpy * pHS = g_arr[__EHS_IDX_GET_MSG].m_list.GetNext( pos );
		ASSERT( pHS != NULL );
		DWORD dwEHSEF = pHS->HookSpyGetEHSEF();
		if( (  dwEHSEF & __EHSEF_GET_MSG ) == 0 )
			continue;
		LRESULT lResult = 0;
		bool bCallNextHook = true;
		if(	pHS->
				HSLL_OnGetMsgProc(
					bCallNextHook,
					lResult,
					nCode,
					wParam,
					(MSG*)lParam
					)
			)
		{
			if( bCallNextHook )
				return ::CallNextHookEx( g_arr[__EHS_IDX_GET_MSG].m_hHook, nCode, wParam, lParam );
			else
				return lResult; // eat!
		}
		if( g_arr[__EHS_IDX_GET_MSG].m_bListChanged )
		{
			g_arr[__EHS_IDX_GET_MSG].m_bListChanged = false;
			pos = g_arr[__EHS_IDX_GET_MSG].m_list.GetHeadPosition();
		} // if( g_arr[__EHS_IDX_GET_MSG].m_bListChanged )
	} // for( g_arr[__EHS_IDX_GET_MSG].m_bListChanged = false; pos != NULL; )
	g_arr[__EHS_IDX_GET_MSG].m_bListChanged = false;
	return ::CallNextHookEx( g_arr[__EHS_IDX_GET_MSG].m_hHook, nCode, wParam, lParam );
}

LRESULT CALLBACK CExtHookSpy::HookSpyItem_t::SHS_HookSysMsgProc(
	int nCode,
	WPARAM wParam,
	LPARAM lParam
	)
{
__PROF_UIS_MANAGE_STATE;
POSITION pos = g_arr[__EHS_IDX_SYS_MSG].m_list.GetHeadPosition();
	for( g_arr[__EHS_IDX_SYS_MSG].m_bListChanged = false; pos != NULL; )
	{
		CExtHookSpy * pHS = g_arr[__EHS_IDX_SYS_MSG].m_list.GetNext( pos );
		ASSERT( pHS != NULL );
		DWORD dwEHSEF = pHS->HookSpyGetEHSEF();
		if( (  dwEHSEF & __EHSEF_SYS_MSG ) == 0 )
			continue;
		LRESULT lResult = 0;
		bool bCallNextHook = true;
		if(	pHS->
				HSLL_OnSysMsgProc(
					bCallNextHook,
					lResult,
					nCode,
					wParam,
					lParam
					)
			)
		{
			if( bCallNextHook )
				return ::CallNextHookEx( g_arr[__EHS_IDX_SYS_MSG].m_hHook, nCode, wParam, lParam );
			else
				return lResult; // eat!
		}
		if( g_arr[__EHS_IDX_SYS_MSG].m_bListChanged )
		{
			g_arr[__EHS_IDX_SYS_MSG].m_bListChanged = false;
			pos = g_arr[__EHS_IDX_SYS_MSG].m_list.GetHeadPosition();
		} // if( g_arr[__EHS_IDX_SYS_MSG].m_bListChanged )
	} // for( g_arr[__EHS_IDX_SYS_MSG].m_bListChanged = false; pos != NULL; )
	g_arr[__EHS_IDX_SYS_MSG].m_bListChanged = false;
	return ::CallNextHookEx( g_arr[__EHS_IDX_SYS_MSG].m_hHook, nCode, wParam, lParam );
}

LRESULT CALLBACK CExtHookSpy::HookSpyItem_t::SHS_HookCallWindowProcIn(
	int nCode,
	WPARAM wParam,
	LPARAM lParam
	)
{
__PROF_UIS_MANAGE_STATE;
POSITION pos = g_arr[__EHS_IDX_WND_PROC_IN].m_list.GetHeadPosition();
	for( g_arr[__EHS_IDX_WND_PROC_IN].m_bListChanged = false; pos != NULL; )
	{
		CExtHookSpy * pHS = g_arr[__EHS_IDX_WND_PROC_IN].m_list.GetNext( pos );
		ASSERT( pHS != NULL );
		DWORD dwEHSEF = pHS->HookSpyGetEHSEF();
		if( (  dwEHSEF & __EHSEF_WND_PROC_IN ) == 0 )
			continue;
		LRESULT lResult = 0;
		bool bCallNextHook = true;
		if(	pHS->
				HSLL_OnCallWindowProcIn(
					bCallNextHook,
					lResult,
					nCode,
					( wParam != 0 ) ? true : false,
					(PCWPSTRUCT)lParam
					)
			)
		{
			if( bCallNextHook )
				return ::CallNextHookEx( g_arr[__EHS_IDX_WND_PROC_IN].m_hHook, nCode, wParam, lParam );
			else
				return lResult; // eat!
		}
		if( g_arr[__EHS_IDX_WND_PROC_IN].m_bListChanged )
		{
			g_arr[__EHS_IDX_WND_PROC_IN].m_bListChanged = false;
			pos = g_arr[__EHS_IDX_WND_PROC_IN].m_list.GetHeadPosition();
		} // if( g_arr[__EHS_IDX_WND_PROC_IN].m_bListChanged )
	} // for( g_arr[__EHS_IDX_WND_PROC_IN].m_bListChanged = false; pos != NULL; )
	g_arr[__EHS_IDX_WND_PROC_IN].m_bListChanged = false;
	return ::CallNextHookEx( g_arr[__EHS_IDX_WND_PROC_IN].m_hHook, nCode, wParam, lParam );
}

LRESULT CALLBACK CExtHookSpy::HookSpyItem_t::SHS_HookCallWindowProcOut(
	int nCode,
	WPARAM wParam,
	LPARAM lParam
	)
{
__PROF_UIS_MANAGE_STATE;
POSITION pos = g_arr[__EHS_IDX_WND_PROC_OUT].m_list.GetHeadPosition();
	for( g_arr[__EHS_IDX_WND_PROC_OUT].m_bListChanged = false; pos != NULL; )
	{
		CExtHookSpy * pHS = g_arr[__EHS_IDX_WND_PROC_OUT].m_list.GetNext( pos );
		ASSERT( pHS != NULL );
		DWORD dwEHSEF = pHS->HookSpyGetEHSEF();
		if( (  dwEHSEF & __EHSEF_WND_PROC_OUT ) == 0 )
			continue;
		LRESULT lResult = 0;
		bool bCallNextHook = true;
		if(	pHS->
				HSLL_OnCallWindowProcOut(
					bCallNextHook,
					lResult,
					nCode,
					( wParam != 0 ) ? true : false,
					(PCWPRETSTRUCT)lParam
					)
			)
		{
			if( bCallNextHook )
				return ::CallNextHookEx( g_arr[__EHS_IDX_WND_PROC_OUT].m_hHook, nCode, wParam, lParam );
			else
				return lResult; // eat!
		}
		if( g_arr[__EHS_IDX_WND_PROC_OUT].m_bListChanged )
		{
			g_arr[__EHS_IDX_WND_PROC_OUT].m_bListChanged = false;
			pos = g_arr[__EHS_IDX_WND_PROC_OUT].m_list.GetHeadPosition();
		} // if( g_arr[__EHS_IDX_WND_PROC_OUT].m_bListChanged )
	} // for( g_arr[__EHS_IDX_WND_PROC_OUT].m_bListChanged = false; pos != NULL; )
	g_arr[__EHS_IDX_WND_PROC_OUT].m_bListChanged = false;
	return ::CallNextHookEx( g_arr[__EHS_IDX_WND_PROC_OUT].m_hHook, nCode, wParam, lParam );
}

bool CExtHookSpy::HSLL_OnMouseWheel(
	WPARAM wParam,
	LPARAM lParam
	)
{
	wParam;
	if( ! g_PaintManager.m_bIsWin2000orLater )
		return false;
struct __SAME_AS_MOUSEHOOKSTRUCTEX
{
	MOUSEHOOKSTRUCT mhs;
	DWORD mouseData;
};
__SAME_AS_MOUSEHOOKSTRUCTEX * pMHEX =
		reinterpret_cast
			< __SAME_AS_MOUSEHOOKSTRUCTEX * >
				( lParam );
	ASSERT( pMHEX != NULL );

DWORD dwWheelDeltaAndZeroFlags =
		DWORD( pMHEX->mouseData ) & 0xFFFF0000;
	if( dwWheelDeltaAndZeroFlags == 0 )
		return 1;
int yAmount =
		( int(short(dwWheelDeltaAndZeroFlags>>16)) > 0 )
			? (-1) : 1;
int nMouseWheelScrollLines =
		(int)g_PaintManager.GetMouseWheelScrollLines();
	if( nMouseWheelScrollLines > 2 )
		nMouseWheelScrollLines--; // offset is 1 less
	yAmount *= nMouseWheelScrollLines;

MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = pMHEX->mhs.hwnd;
	_msg.wParam = MAKEWPARAM( 0, yAmount );
	_msg.lParam = MAKELPARAM(pMHEX->mhs.pt.x,pMHEX->mhs.pt.y);
	_msg.message = WM_MOUSEWHEEL;
//	TRACE1( "wheel_msg = %x\n", _msg.message );
DWORD dwEHSEF = HookSpyGetEHSEF();
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION_LL ) != 0 )
	{
		if( HSLL_PreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION ) != 0 )
	{
		if( OnHookSpyPreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( OnHookSpyMouseWheelMsg( &_msg ) )
		return true;
	if( ( dwEHSEF & __EHSEF_POST_PROCESSING ) != 0 )
	{
		if( OnHookSpyPostProcessMessage( &_msg ) )
			return true;
	}
	return false;
}

bool CExtHookSpy::HSLL_OnMouseMove(
	HWND hWnd,
	UINT nFlags,
	CPoint point
	)
{
	if(		hWnd == NULL
		||	(! ::IsWindow(hWnd) )
		||	(! ::GetCursorPos(&point) ) 
		)
		return false;
	::ScreenToClient( hWnd, &point );
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = hWnd;
	_msg.wParam = WPARAM(nFlags);
	_msg.lParam = MAKELPARAM( point.x, point.y );
	_msg.message = WM_MOUSEMOVE;
	_msg.pt = point;
DWORD dwEHSEF = HookSpyGetEHSEF();
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION_LL ) != 0 )
	{
		if( HSLL_PreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION ) != 0 )
	{
		if( OnHookSpyPreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( OnHookSpyMouseMoveMsg( &_msg ) )
		return true;
	if( ! HookSpyIsRegistered() )
		return false;
	if( ( dwEHSEF & __EHSEF_POST_PROCESSING ) != 0 )
	{
		if( OnHookSpyPostProcessMessage( &_msg ) )
			return true;
	}
	return false;
}

bool CExtHookSpy::HSLL_OnMouseClick(
	HWND hWnd,
	UINT nMessage,
	UINT nFlags,
	CPoint point
	)
{
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = hWnd;
	_msg.wParam = WPARAM(nFlags);
	_msg.lParam = MAKELPARAM(point.x,point.y);
	_msg.message = nMessage;
::ScreenToClient( hWnd, &point );
	_msg.pt = point;
DWORD dwEHSEF = HookSpyGetEHSEF();
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION_LL ) != 0 )
	{
		if( HSLL_PreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION ) != 0 )
	{
		if( OnHookSpyPreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( OnHookSpyMouseClickMsg( &_msg ) )
		return true;
	if( ! HookSpyIsRegistered() )
		return false;
	if( ( dwEHSEF & __EHSEF_POST_PROCESSING ) != 0 )
	{
		if( OnHookSpyPostProcessMessage( &_msg ) )
			return true;
	}
	return false;
}

bool CExtHookSpy::HSLL_OnKey(
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags
	)
{
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = ::GetFocus();
	_msg.wParam = WPARAM(nChar);
	_msg.lParam = MAKELPARAM(nRepCnt,nFlags);
	_msg.message =
		( ( nFlags & (KF_UP) ) != 0 )
			? WM_KEYUP
			: WM_KEYDOWN
			;
//	TRACE3(
//		"key_msg = %x (%s), flags = 0x%04X\n",
//		_msg.message,
//		(_msg.message == WM_KEYUP) ? "WM_KEYUP" : "WM_KEYDOWN",
//		nFlags
//		);
DWORD dwEHSEF = HookSpyGetEHSEF();
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION_LL ) != 0 )
	{
		if( HSLL_PreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION ) != 0 )
	{
		if( OnHookSpyPreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( OnHookSpyKeyMsg( &_msg ) )
		return true;
	if( ! HookSpyIsRegistered() )
		return false;
	if( ( dwEHSEF & __EHSEF_POST_PROCESSING ) != 0 )
	{
		if( OnHookSpyPostProcessMessage( &_msg ) )
			return true;
	}
	return false;
}

bool CExtHookSpy::HSLL_OnGetMsgProc(
	bool & bCallNextHook,
	LRESULT & lResult,
	int nCode,
	WPARAM wParam,
	MSG * pMSG
	)
{
	lResult;
	if( nCode != HC_ACTION )
		return false;
	if( wParam != PM_REMOVE )
		return false;
DWORD dwEHSEF = HookSpyGetEHSEF();
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION_LL ) != 0 )
	{
		if( HSLL_PreTranslateMessage( pMSG ) )
		{
			bCallNextHook = false;
			return true;
		}
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION ) != 0 )
	{
		if( OnHookSpyPreTranslateMessage( pMSG ) )
		{
			bCallNextHook = false;
			return true;
		}
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( OnHookSpyGetMsgProc( pMSG ) )
	{
		bCallNextHook = false;
		return true;
	}
	if( ! HookSpyIsRegistered() )
		return false;
	if( ( dwEHSEF & __EHSEF_POST_PROCESSING ) != 0 )
	{
		if( OnHookSpyPostProcessMessage( pMSG ) )
		{
			bCallNextHook = false;
			return true;
		}
	}
	return false;
}

bool CExtHookSpy::HSLL_OnSysMsgProc(
	bool & bCallNextHook,
	LRESULT & lResult,
	int nCode,
	WPARAM wParam,
	LPARAM lParam
	)
{
//	lResult;
//	if( nCode != HC_ACTION )
//		return false;
	bCallNextHook;
	lResult;
	nCode;
	wParam;
	lParam;
	return false;
}

bool CExtHookSpy::HSLL_OnCallWindowProcIn(
	bool & bCallNextHook,
	LRESULT & lResult,
	int nCode,
	bool bMessageSentByCurrentThread,
	PCWPSTRUCT pMessageData
	)
{
	bCallNextHook;
	lResult;
	bMessageSentByCurrentThread;
	if( nCode != HC_ACTION )
		return false;
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = pMessageData->hwnd;
	_msg.wParam = pMessageData->wParam;
	_msg.lParam = pMessageData->lParam;
	_msg.message = pMessageData->message;
DWORD dwEHSEF = HookSpyGetEHSEF();
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION_LL ) != 0 )
	{
		if( HSLL_PreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION ) != 0 )
	{
		if( OnHookSpyPreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( OnHookSpyCallWindowProcIn( &_msg ) )
		return true;
	if( ! HookSpyIsRegistered() )
		return false;
	if( ( dwEHSEF & __EHSEF_POST_PROCESSING ) != 0 )
	{
		if( OnHookSpyPostProcessMessage( &_msg ) )
			return true;
	}
	return false;
}

bool CExtHookSpy::HSLL_OnCallWindowProcOut(
	bool & bCallNextHook,
	LRESULT & lResult,
	int nCode,
	bool bMessageSentByCurrentThread,
	PCWPRETSTRUCT pMessageData
	)
{
	bCallNextHook;
	lResult;
	bMessageSentByCurrentThread;
	if( nCode != HC_ACTION )
		return false;
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = pMessageData->hwnd;
	_msg.wParam = pMessageData->wParam;
	_msg.lParam = pMessageData->lParam;
	_msg.message = pMessageData->message;
DWORD dwEHSEF = HookSpyGetEHSEF();
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION_LL ) != 0 )
	{
		if( HSLL_PreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( ( dwEHSEF & __EHSEF_PRE_TRANSLATION ) != 0 )
	{
		if( OnHookSpyPreTranslateMessage( &_msg ) )
			return true;
		if( ! HookSpyIsRegistered() )
			return false;
	}
	if( OnHookSpyCallWindowProcOut( &_msg ) )
		return true;
	if( ! HookSpyIsRegistered() )
		return false;
	if( ( dwEHSEF & __EHSEF_POST_PROCESSING ) != 0 )
	{
		if( OnHookSpyPostProcessMessage( &_msg ) )
			return true;
	}
	return false;
}

bool CExtHookSpy::HSLL_PreTranslateMessage(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

bool CExtHookSpy::OnHookSpyPreTranslateMessage(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

bool CExtHookSpy::OnHookSpyPostProcessMessage(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

bool CExtHookSpy::OnHookSpyMouseWheelMsg(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

bool CExtHookSpy::OnHookSpyMouseMoveMsg(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

bool CExtHookSpy::OnHookSpyMouseClickMsg(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

bool CExtHookSpy::OnHookSpyKeyMsg(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

bool CExtHookSpy::OnHookSpyGetMsgProc(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

//bool CExtHookSpy::OnHookSpySysMsgProc(
//	MSG * pMSG
//	)
//{
//	ASSERT( pMSG != NULL );
//	pMSG;
//	return false;
//}

bool CExtHookSpy::OnHookSpyCallWindowProcIn(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

bool CExtHookSpy::OnHookSpyCallWindowProcOut(
	MSG * pMSG
	)
{
	ASSERT( pMSG != NULL );
	pMSG;
	return false;
}

void CExtHookSpy::HookSpyRegister(
	DWORD dwEHSEF, // = __EHSEF_ALL
	bool bAddToHead // = true
	)
{
	HookSpyRegister( this, dwEHSEF, bAddToHead );
}

void CExtHookSpy::HookSpyRegister(
	CExtHookSpy * pHS,
	DWORD dwEHSEF, // = __EHSEF_ALL
	bool bAddToHead // = true
	)
{
__PROF_UIS_MANAGE_STATE;
	if( pHS == NULL )
		return;
	if( dwEHSEF == 0 )
	{
		HookSpyUnregister( pHS );
		return;
	}
INT nIndex, nCount = __EHS_IDX_COUNT;
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
		g_arr[ nIndex ].Register( pHS, dwEHSEF, bAddToHead );
	if( ( pHS->HookSpyGetEHSEF() & __EHSEF_BASIC_HOOK_FILTERING ) != 0 )
		pHS->HookSpyModifyEHSEF( 0, dwEHSEF&__EHSEF_ADDITIONAL_PROCESSING );
}

void CExtHookSpy::HookSpyUnregister(
	DWORD dwEHSEF // = __EHSEF_ALL
	)
{
	HookSpyUnregister( this, dwEHSEF );
}

void CExtHookSpy::HookSpyUnregister(
	CExtHookSpy * pHS,
	DWORD dwEHSEF // = __EHSEF_ALL
	)
{
__PROF_UIS_MANAGE_STATE;
	if( pHS == NULL || dwEHSEF == 0 )
		return;
INT nIndex, nCount = __EHS_IDX_COUNT;
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
		g_arr[ nIndex ].Unregister( pHS, dwEHSEF );
	if( ( pHS->HookSpyGetEHSEF() & __EHSEF_BASIC_HOOK_FILTERING ) == 0 )
		pHS->HookSpyModifyEHSEF( dwEHSEF&__EHSEF_ADDITIONAL_PROCESSING, 0 );
}

bool CExtHookSpy::HookSpyIsRegistered(
	const CExtHookSpy * pHS,
	DWORD dwEHSEF // = __EHSEF_ALL
	)
{
__PROF_UIS_MANAGE_STATE;
	if( pHS == NULL || dwEHSEF == 0 )
		return false;
INT nIndex, nCount = __EHS_IDX_COUNT;
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		if( g_arr[ nIndex ].IsRegistered( ((CExtHookSpy*)pHS), dwEHSEF ) )
			return true;
	}
	return false;
}

