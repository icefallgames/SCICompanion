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

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif 

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __EXTDOCKBAR_H)
	#include "ExtDockBar.h"
#endif

#if( !defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
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

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE) || (!defined __EXT_MFC_NO_DATE_PICKER_POPUP) || (!defined __EXT_MFC_NO_UNDO_REDO_POPUP) || (!defined __EXT_MFC_NO_GRIDBASEWND)
	#if (!defined __EXT_POPUP_CTRL_MENU_H)
		#include <ExtPopupCtrlMenu.h>
	#endif
#endif

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	#if (!defined __EXT_RIBBON_BAR_H)
		#include <ExtRibbonBar.h>
	#endif // (!defined __EXT_RIBBON_BAR_H)
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif

#ifndef __EXT_MFC_NO_GRIDWND
	#if (!defined __EXT_GRIDWND_H)
		#include <ExtGridWnd.h>
	#endif 
#endif // __EXT_MFC_NO_GRIDWND

#include <Resources/Resource.h>

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER 9950

/////////////////////////////////////////////////////////////////////////////
// CExtToolControlBar::CExtToolControlBarCmdUI

class CExtToolControlBar::CExtToolControlBarCmdUI : public CCmdUI
{
	void _SetCheckImpl( int nCheck, bool bUpdateInCmdManager );
public:
	virtual void Enable( BOOL bOn );
	virtual void SetRadio( BOOL bOn )
	{
		_SetCheckImpl( bOn ? 1 : 0, false );
//		CExtToolControlBar * pToolBar = (CExtToolControlBar*)m_pOther;
//		ASSERT( pToolBar != NULL );
//		ASSERT_KINDOF( CExtToolControlBar, pToolBar );
//		ASSERT( m_nIndex < m_nIndexMax );
//		if( pToolBar->IsRightExpandButton(m_nIndex) )
//			return;
//		CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr(
//			g_CmdManager->ProfileNameFromWnd( pToolBar->GetSafeHwnd() ),
//			pToolBar->GetButtonID(m_nIndex)
//			);
//		if( pCmdItem != NULL )
//			pCmdItem->StateSetRadio( bOn ? true : false );
	}
	virtual void SetCheck( int nCheck )
	{
		_SetCheckImpl( nCheck, true );
	}
	virtual void SetText( LPCTSTR lpszText )
	{
		lpszText;
		// ignore it
//#if (!defined __EXT_MFC_NO_CUSTOMIZE)
//		CExtToolControlBar* pToolBar = (CExtToolControlBar*)m_pOther;
//		ASSERT(pToolBar != NULL);
//		ASSERT_KINDOF(CExtToolControlBar, pToolBar);
//		ASSERT(m_nIndex < m_nIndexMax);
//		if( pToolBar->IsRightExpandButton(m_nIndex) )
//			return;
//		CExtBarButton * pTBB = pToolBar->GetButton( m_nIndex );
//		ASSERT_VALID( pTBB );
//		CExtCustomizeCmdTreeNode * pNode = pTBB->GetCmdNode( false );
//		if( pNode == NULL )
//			return;
//		ASSERT_VALID( pNode );
//		if( lpszText == NULL )
//			lpszText = _T("");
//		LPCTSTR _strCurText = pNode->GetTextInToolbar( NULL, true );
//		if( _strCurText == NULL )
//			_strCurText = _T("");
//		if( _tcscmp( _strCurText, lpszText ) != 0 )
//		{
//			pNode->SetTextInToolbar( lpszText );
//			if( pToolBar->IsVisible() )
//			{
//				DWORD dwDisplayStyle = pNode->GetFlags()&__ECTN_DISPLAY_MASK;
//				if(		dwDisplayStyle == __ECTN_DISPLAY_TEXTONLY_ALWAYS
//					||	dwDisplayStyle == __ECTN_DISPLAY_TEXT_AND_IMAGE
//					)
//				{
//					CFrameWnd * pFrame = pToolBar->GetParentFrame();
//					if( pFrame != NULL )
//						pFrame->DelayRecalcLayout();
//				}
//			}
//		}
//#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	};
}; // class CExtToolControlBar::CExtToolControlBarCmdUI

void CExtToolControlBar::CExtToolControlBarCmdUI::_SetCheckImpl(
	int nCheck,
	bool bUpdateInCmdManager
	)
{
	bUpdateInCmdManager;
	ASSERT( nCheck >= 0 && nCheck <= 2 );
CExtToolControlBar * pToolBar = (CExtToolControlBar*)m_pOther;
	ASSERT( pToolBar != NULL );
	ASSERT_KINDOF( CExtToolControlBar, pToolBar );
	ASSERT( m_nIndex < m_nIndexMax ); 
	if( pToolBar->IsRightExpandButton( m_nIndex ) )
		return;
UINT nByttonStyle = pToolBar->GetButtonStyle( m_nIndex );
UINT nNewStyle = nByttonStyle & (~(TBBS_CHECKED | TBBS_INDETERMINATE));
	if( nCheck == 1 )
		nNewStyle |= TBBS_CHECKED;
	else if( nCheck == 2 )
		nNewStyle |= TBBS_INDETERMINATE;
	ASSERT( ( nNewStyle & TBBS_SEPARATOR ) == 0 );
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle | TBBS_CHECKBOX);
//	if( bUpdateInCmdManager )
//	{
//		CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr(
//			g_CmdManager->ProfileNameFromWnd( pToolBar->GetSafeHwnd() ),
//			pToolBar->GetButtonID(m_nIndex)
//			);
//		if( pCmdItem != NULL )
//		{
//			pCmdItem->StateSetCheck(
//				(nNewStyle &
//					(TBBS_CHECKED|TBBS_INDETERMINATE)
//					) ? true : false
//				);
//		}
//	}
}

void CExtToolControlBar::CExtToolControlBarCmdUI::Enable( BOOL bOn )
{
	CCmdUI::m_bEnableChanged = TRUE;
CExtToolControlBar * pToolBar = (CExtToolControlBar*)m_pOther;
	ASSERT( pToolBar != NULL );
	ASSERT_KINDOF( CExtToolControlBar, pToolBar );
	ASSERT( m_nIndex < m_nIndexMax );
	if( pToolBar->IsRightExpandButton( m_nIndex ) )
		return;
UINT nByttonStyle = pToolBar->GetButtonStyle( m_nIndex );
UINT nNewStyle = nByttonStyle & (~(TBBS_DISABLED));
	if( ! bOn )
		nNewStyle |= TBBS_DISABLED;
	ASSERT( ( nNewStyle & TBBS_SEPARATOR ) == 0 );
	pToolBar->SetButtonStyle( m_nIndex, nNewStyle );
//CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr(
//		g_CmdManager->ProfileNameFromWnd( pToolBar->GetSafeHwnd() ),															
//		pToolBar->GetButtonID(m_nIndex)
//		);
//	if( pCmdItem != NULL )
//		pCmdItem->StateEnable( bOn ? true : false );
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarButton

IMPLEMENT_DYNCREATE(CExtBarButton, CObject)

CExtBarButton::CExtBarButton(
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	)
	: CExtAnimationClient( pBar )
	, m_nStyle( nStyle )
	, m_pBar( pBar )
	, m_pParentButton(NULL )
	, m_nButtonCmdID( ID_SEPARATOR )
	, m_nEffectiveCmdID( ID_SEPARATOR )
	, m_pCtrl( NULL )
	, m_bCtrlVisibleVert( false )
	, m_bCtrlAutoDestroyed( false )
	, m_ActiveSize( 0, 0 )
	, m_ActiveRect( 0, 0, 0, 0 )
	, m_bWrapH( false )
	, m_bWrapV( false )
	, m_bWrapF( false )
	, m_bHover( false )
	, m_bVisible(true )
	, m_bVertDocked( false )
	, m_hMenu( NULL )
	, m_bPopupMenu( false )
	, m_bAutoDestroyMenu( false )
	, m_bAppendMdiWindowsMenu( false )
	, m_bSeparatedDropDown( false )
	, m_bAutoChangeID( false )
	, m_bDropDownHT( false )
	, m_bNoRotateVerticalLayout( false )
	, m_bPressedTracking( false )
	, m_bDrawBorder( true )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_pWndKeyTip( NULL )
	, m_pCmdNodeI( NULL )
	, m_pCmdNodeC( NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_nMaxButtonWidth( -1 )
	, m_nIconAlignment( -1 )
	, m_nTextAlignment( -1 )
	, m_rcIconMargins( -1, -1, -1, -1 )
	, m_rcTextMargins( -1, -1, -1, -1 )
{
	SetCmdID( nCmdID );
}

CExtBarButton::~CExtBarButton()
{
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS != NULL )
		pAcAS->AnimationSite_ClientRemove( this );
	_DestroyMenu();
	m_arrChildren.RemoveAll();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
HDWP hPassiveModeDWP = NULL;
	OnKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)
	CtrlSet( NULL, true ); // destroy attached window
}

void CExtBarButton::Show(
	bool bShow // = true
	)
{
	ASSERT_VALID( this );
	m_bVisible = bShow;
	_UpdateCtrl();
INT nIndex, nCount = ChildButtonGetCount();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtBarButton * pChildTBB = ChildButtonGetAt( nIndex );
		ASSERT_VALID( pChildTBB );
		pChildTBB->Show( bShow );
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
}

bool CExtBarButton::IsVisible() const
{
	ASSERT_VALID( this );
	return m_bVisible; // && ( (m_nStyle & TBBS_HIDDEN) == 0 );
}

bool CExtBarButton::IsChildButton(
	const CExtBarButton * pTestChildTBB,
	bool bTestOneLevelOnly // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTestChildTBB );
const CExtBarButton * pTBB = pTestChildTBB->ParentButtonGet();
	if( pTBB == NULL )
		return false;
	if( LPVOID(pTBB) == LPVOID(this) )
		return true;
	if( bTestOneLevelOnly )
		return false;
bool bRetVal = IsChildButton( pTBB );
	return bRetVal;
}

CExtBarButton * CExtBarButton::ParentButtonGet()
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( m_pParentButton != NULL )
	{
		ASSERT_VALID( m_pParentButton );
	}
#endif // _DEBUG
	return m_pParentButton;
}

const CExtBarButton * CExtBarButton::ParentButtonGet() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtBarButton * > ( this ) )
		-> ParentButtonGet();
}

void CExtBarButton::ParentButtonSet(
	CExtBarButton * pParentButton // = NULL
	)
{
	ASSERT_VALID( this );
	m_pParentButton = pParentButton;
#ifdef _DEBUG
	if( m_pParentButton != NULL )
	{
		ASSERT_VALID( m_pParentButton );
	}
#endif // _DEBUG
}

INT CExtBarButton::ChildButtonGetCount() const
{
	ASSERT_VALID( this );
INT nCount = INT( m_arrChildren.GetSize() );
	return nCount;
}

CExtBarButton * CExtBarButton::ChildButtonGetAt( INT nPos )
{
	ASSERT_VALID( this );
	if( nPos < 0 )
		return NULL;
INT nCount = ChildButtonGetCount();
	if( nPos >= nCount )
		return NULL;
CExtBarButton * pTBB = m_arrChildren.GetAt( nPos );
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		CExtBarButton * pParentTBB = pTBB->ParentButtonGet();
		ASSERT( pParentTBB == NULL || pParentTBB == this );
	}
#endif // _DEBUG
	return pTBB;
}

const CExtBarButton * CExtBarButton::ChildButtonGetAt( INT nPos ) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtBarButton * > ( this ) )
		-> ChildButtonGetAt( nPos );
}

bool CExtBarButton::ChildButtonSetAt(
	INT nPos,
	CExtBarButton * pTBB
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB->ParentButtonSet( NULL );
	if( nPos < 0 )
		return false;
INT nCount = ChildButtonGetCount();
	if( nPos >= nCount )
		return false;
	m_arrChildren.SetAt( nPos, pTBB );
	pTBB->ParentButtonSet( this );
	return true;
}

void CExtBarButton::ChildButtonInsertAt(
	INT nPos,
	CExtBarButton * pTBB
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB->ParentButtonSet( NULL );
INT nCount = ChildButtonGetCount();
	if( nPos < 0 || nPos > nCount )
		nPos = nCount;
	m_arrChildren.InsertAt( nPos, pTBB );
	pTBB->ParentButtonSet( this );
}

void CExtBarButton::ChildButtonAdd(
	CExtBarButton * pTBB
	)
{
	ASSERT_VALID( this );
	ChildButtonInsertAt( -1, pTBB );
}

bool CExtBarButton::ChildButtonRemove(
	CExtBarButton * pTBB,
	bool bDestroy // = false
	)
{
	ASSERT_VALID( this );
INT nPos = ChildButtonGetIndexOf( pTBB );
	if( nPos < 0 )
		return false;
	ChildButtonRemoveAt( nPos, 1, bDestroy );
	return true;
}

void CExtBarButton::ChildButtonRemoveAll(
	bool bDestroy // = false
	)
{
	ASSERT_VALID( this );
	ChildButtonRemoveAt( 0, -1, bDestroy );
}

INT CExtBarButton::ChildButtonRemoveAt(
	INT nPos,
	INT nCountToRemove, // = 1 // -1 remove up to end
	bool bDestroy // = false
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( nCountToRemove == 0 )
		return 0;
	if( nPos < 0 )
		return 0;
INT nCount = ChildButtonGetCount();
	if( nPos >= nCount )
		return 0;
	if( nCountToRemove < 0 )
		nCountToRemove = nCount;
	if( ( nPos + nCountToRemove ) > nCount )
		nCountToRemove = nCount - nPos;
	if( nCountToRemove == 0 )
		return 0;
	if( bDestroy )
	{
		INT nIndex;
		for( nIndex = 0; nIndex < nCountToRemove; nIndex ++ )
		{
			CExtBarButton * pTBB = m_arrChildren[ nPos + nIndex ];
			ASSERT_VALID( pTBB );
			pBar->AnimationSite_ClientRemove( pTBB );
			delete pTBB;
		} // for( nIndex = 0; nIndex < nCountToRemove; nIndex ++ )
	} // if( bDestroy )
	m_arrChildren.RemoveAt( nPos, nCountToRemove );
	return nCountToRemove;
}

INT CExtBarButton::ChildButtonGetIndexOf(
	const CExtBarButton * pTBB
	) const
{
	ASSERT_VALID( this );
	if( pTBB == NULL )
		return -1;
	ASSERT_VALID( pTBB );
INT nIndex, nCount = ChildButtonGetCount();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		const CExtBarButton * pChildTBB = ChildButtonGetAt( nIndex );
		ASSERT_VALID( pChildTBB );
		if( pChildTBB == pTBB )
			return nIndex;
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
	return -1;
}

CExtToolControlBar * CExtBarButton::GetSafeBar()
{
	if( this == NULL )
		return NULL;
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT( pBar != NULL );
	return pBar;
}

const CExtToolControlBar * CExtBarButton::GetSafeBar() const
{
	return 
		( const_cast < CExtBarButton * > ( this ) ) -> GetSafeBar();
}

CExtToolControlBar * CExtBarButton::GetBar()
{
	ASSERT_VALID( this );
	return m_pBar;
}

const CExtToolControlBar * CExtBarButton::GetBar() const
{
	ASSERT_VALID( this );
	return 
		( const_cast < CExtBarButton * > ( this ) ) -> GetBar();
}

void CExtBarButton::SetBar( CExtToolControlBar * pBar )
{
	ASSERT_VALID( this );
	m_pBar = pBar;
INT nChildIndex, nChildCount = ChildButtonGetCount();
	for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
	{
		CExtBarButton * pTBB = ChildButtonGetAt( nChildIndex );
		ASSERT_VALID( pTBB );
		pTBB->SetBar( pBar );
	} // for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
}

#if (!defined __EXT_MFC_NO_RIBBON_BAR)

bool CExtBarButton::IsRibbonPaintingMode() const
{
	ASSERT_VALID( this );
const CExtToolControlBar * pBar = GetBar();
	if( pBar == NULL )
		return false;
	if( ! pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) ) )
		return false;
	return true;
}

bool CExtBarButton::IsNoRibbonLayout() const
{
	ASSERT_VALID( this );
	return false;
}

CExtRibbonNode * CExtBarButton::Ribbon_GetNode(
	bool bInitial // = false
	)
{
	ASSERT_VALID( this );
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( bInitial );
	if( pNode == NULL )
		return NULL;
	ASSERT_VALID( pNode );
CExtRibbonNode * pRibbonNode = DYNAMIC_DOWNCAST( CExtRibbonNode, pNode );
	return pRibbonNode;
}

const CExtRibbonNode * CExtBarButton::Ribbon_GetNode(
	bool bInitial // = false
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtBarButton * > ( this ) )
		-> Ribbon_GetNode( bInitial );
}

CSize CExtBarButton::RibbonILV_CalcSize(
	CDC & dc,
	INT nILV // = -1 // -1 use current visual level
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	if( ! IsVisible() )
		return CSize( 0, 0 );
	if( (GetStyle()&TBBS_HIDDEN) != 0 )
		return CSize( 0, 0 );
CSize _size =
		GetBar() ->
		PmBridge_GetPM() ->
		Ribbon_CalcButtonSize( dc, nILV, this );
	return _size;
}

bool CExtBarButton::RibbonWrapFromILE( INT nILE ) const
{
	ASSERT_VALID( this );
	ASSERT(
			__EXT_RIBBON_ILE_MIN <= nILE
		&&	nILE <= (__EXT_RIBBON_ILE_MAX+1)
		);
const CExtRibbonNode * pRibbonNode = Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return false;
bool bRibbonWrap = pRibbonNode->RibbonWrapFromILE( nILE );
	return bRibbonWrap;
}

INT CExtBarButton::RibbonILV_fromILE(
	INT nILE,
	bool * p_bIsWrap // = NULL
	) const
{
	ASSERT_VALID( this );
	ASSERT(
			__EXT_RIBBON_ILE_MIN <= nILE
		&&	nILE <= (__EXT_RIBBON_ILE_MAX+1)
		);
const CExtRibbonNode * pRibbonNode = Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return __EXT_RIBBON_ILV_SIMPLE_MAX;
INT nILV = pRibbonNode->RibbonILV_fromILE( nILE, p_bIsWrap );
	return nILV;
}

INT CExtBarButton::RibbonILV_Get(
	INT nType // = 0 // -1 min, 0 current, 1 - max
	) const
{
	ASSERT_VALID( this );
const CExtRibbonNode * pRibbonNode = Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return __EXT_RIBBON_ILV_SIMPLE_MAX;
const CExtCustomizeCmdTreeNode * pParentNode = pRibbonNode->GetParentNode();
	if(		pParentNode != NULL
		&&	(	pParentNode->IsKindOf( RUNTIME_CLASS( CExtRibbonNodeRightButtonsCollection ) )
			||	pParentNode->IsKindOf( RUNTIME_CLASS( CExtRibbonNodeQuickAccessButtonsCollection ) )
			)
		)
		return __EXT_RIBBON_ILV_SIMPLE_MIN;
INT nILV = pRibbonNode->RibbonILV_Get( nType );
	return nILV;
}

void CExtBarButton::RibbonILV_Set(
	INT nILV,
	INT nType // = 0, // -1 min, 0 current, 1 - max
	)
{
	ASSERT_VALID( this );
CExtRibbonNode * pRibbonNode = Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return;
	pRibbonNode->RibbonILV_Set( nILV, nType );
}

INT CExtBarButton::RibbonILE_Get() const
{
	ASSERT_VALID( this );
const CExtRibbonNode * pRibbonNode = Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return __EXT_RIBBON_ILE_MAX;
INT nILE = pRibbonNode->RibbonILE_Get();
	return nILE;
}

bool CExtBarButton::RibbonILE_Set( INT nILE ) // returns flag indicating whether ILV changed
{
	ASSERT_VALID( this );
CExtRibbonNode * pRibbonNode = Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return false;
bool bRetVal = pRibbonNode->RibbonILE_Set( nILE );
	return bRetVal;
}

INT CExtBarButton::RibbonILE_GetCollapsed() const
{
	ASSERT_VALID( this );
const CExtRibbonNode * pRibbonNode = Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return __EXT_RIBBON_ILE_MAX;
INT nILE = pRibbonNode->RibbonILE_GetCollapsed();
	return nILE;
}

bool CExtBarButton::RibbonILE_SetCollapsed( INT nILE ) // returns flag indicating whether collapsed state changed
{
	ASSERT_VALID( this );
CExtRibbonNode * pRibbonNode = Ribbon_GetNode();
	if( pRibbonNode == NULL )
		return false;
	pRibbonNode->RibbonILE_SetCollapsed( nILE );
	return false;
}

CRect CExtBarButton::OnRibbonGetContentPadding() const
{
	ASSERT_VALID( this );
//	return CRect( 0, 0, 0, 0 );
CRect rcContentPadding = GetBar()->PmBridge_GetPM()->Ribbon_GetContentPadding( this );
	return rcContentPadding;
}

void CExtBarButton::OnRibbonAlignContent( CDC & dc )
{
	ASSERT_VALID( this );
INT nChildIndex, nChildCount = ChildButtonGetCount();
	for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
	{
		CExtBarButton * pTBB = ChildButtonGetAt( nChildIndex );
		ASSERT_VALID( pTBB );
		if( ! pTBB->IsVisible() )
			continue;
		if( ( pTBB->GetStyle() & TBBS_HIDDEN ) != 0 )
			continue;
		pTBB->OnRibbonAlignContent( dc );
	} // for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
}

INT CExtBarButton::OnRibbonGetSeparatorExtent( bool bHorz )
{
	ASSERT_VALID( this );
	bHorz;
	return 2;
}

void CExtBarButton::OnRibbonCalcMinMaxILE( INT & nMinILE, INT & nMaxILE )
{
	ASSERT_VALID( this );
bool bWrap = false;
INT nILV = 32761;
INT nILE = __EXT_RIBBON_ILE_MAX;
	for( ; true; nILE-- )
	{
		bool bLastWrap = false;
		INT nNextILV =
			RibbonILV_fromILE(
				nILE,
				&bLastWrap
				);
		if(		nNextILV != nILV
			||	( bWrap && ( ! bLastWrap ) )
			||	( ( ! bWrap ) && bLastWrap )
			)
		{
			nILV = nNextILV;
			bWrap = bLastWrap;
			if( nMinILE > nILE )
				nMinILE = nILE;
			if( nMaxILE < nILE )
				nMaxILE = nILE;
		}
		if( nILE == __EXT_RIBBON_ILE_MIN )
			break;
	} // for( ; true; nILE-- )
INT nIndex, nCount = ChildButtonGetCount();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtBarButton * pChildTBB = ChildButtonGetAt( nIndex );
		ASSERT_VALID( pChildTBB );
		pChildTBB->OnRibbonCalcMinMaxILE( nMinILE, nMaxILE );
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
}

#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtCustomizeCmdKeyTip * CExtBarButton::OnKeyTipGetInfo()
{
	ASSERT_VALID( this );
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		CExtCustomizeCmdKeyTip * pCmdKeyTip = pNode->CmdKeyTipGet();
		return pCmdKeyTip;
	}
	return NULL;
}

CExtSafeString CExtBarButton::OnKeyTipGetText(
	INT nIndent
	)
{
	ASSERT_VALID( this );
CExtSafeString strKeyTipText( _T("") );
CExtCustomizeCmdKeyTip * pCmdKeyTip = OnKeyTipGetInfo();
	if( pCmdKeyTip != NULL )
		strKeyTipText = pCmdKeyTip->GetTipText( nIndent );
	return strKeyTipText;
}

CWnd * CExtBarButton::OnKeyTipGetParentWnd()
{
	ASSERT_VALID( this );
CExtToolControlBar * pToolBar = GetBar();
	ASSERT_VALID( pToolBar );
	ASSERT( pToolBar->GetSafeHwnd() != NULL );
	if( pToolBar->m_pDockSite != NULL )
		return pToolBar->GetParentFrame();
CWnd * pWnd = pToolBar->GetParent();
	for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
	{
		if( ( pWnd->GetStyle() & WS_CHILD ) == 0 )
			return pWnd;
	}
	return NULL;
}

CPoint CExtBarButton::OnKeyTipGetGuideLines()
{
	ASSERT_VALID( this );
CExtToolControlBar * pToolBar = GetBar();
	ASSERT_VALID( pToolBar );
	ASSERT( pToolBar->GetSafeHwnd() != NULL );
CRect rcTBB = *this;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
CExtRibbonPage * pRibbonPage = DYNAMIC_DOWNCAST( CExtRibbonPage, pToolBar );
	if( pRibbonPage != NULL )
	{
		INT nILV = RibbonILV_Get();
		if( nILV < __EXT_RIBBON_ILV_SIMPLE_LARGE )
		{
			INT nTabLineHeight = pRibbonPage->RibbonLayout_GetTabLineHeight();
			INT nFrameCaptionHeight = pRibbonPage->RibbonLayout_GetFrameCaptionHeight();
			INT nHeightAtTheTop = nTabLineHeight + nFrameCaptionHeight;
			INT nBottomLineHeight = pRibbonPage->RibbonLayout_GetBottomLineHeight();
			CRect rcClient;
			pRibbonPage->GetClientRect( &rcClient );
			CExtPaintManager * pPM = pRibbonPage->PmBridge_GetPM();
			ASSERT_VALID( pPM );
			INT nCaptionHeight = pPM->Ribbon_GroupCaptionGetHeight( NULL );
			bool bCaptionAtTop = pRibbonPage->RibbonLayout_GroupCaptionIsTopAligned( NULL );
			if( bCaptionAtTop )
				rcClient.top += nCaptionHeight;
			else
				rcClient.bottom -= nCaptionHeight;
			CRect rcPageBk = rcClient;
			rcPageBk.top += nHeightAtTheTop;
			rcPageBk.bottom -= nBottomLineHeight;
			CPoint ptCenter = rcTBB.CenterPoint();
			if( rcPageBk.PtInRect( ptCenter ) )
			{
				pToolBar->ClientToScreen( &rcTBB );
				INT nHeight3 = rcPageBk.Height() / 3;
				if(		rcPageBk.top <= ptCenter.y
					&&	ptCenter.y < ( rcPageBk.top + nHeight3 )
					)
				{
					CPoint ptGuideLines(
						( nILV == __EXT_RIBBON_ILV_SIMPLE_NORMAL )
							? ( rcTBB.left + pPM->UiScalingDo( 18, CExtPaintManager::__EUIST_X ) )
							: ( rcTBB.left + rcTBB.Width() / 2 )
							,
						rcTBB.top
						);
					return ptGuideLines;
				}
				if(		( rcPageBk.top + nHeight3 ) <= ptCenter.y
					&&	ptCenter.y < ( rcPageBk.bottom - nHeight3 )
					)
				{
					CPoint ptGuideLines(
						( nILV == __EXT_RIBBON_ILV_SIMPLE_NORMAL )
							? ( rcTBB.left + pPM->UiScalingDo( 18, CExtPaintManager::__EUIST_X ) )
							: ( rcTBB.left + rcTBB.Width() / 2 )
							,
						rcTBB.top + rcTBB.Height() / 2
						);
					return ptGuideLines;
				}
					CPoint ptGuideLines(
						( nILV == __EXT_RIBBON_ILV_SIMPLE_NORMAL )
							? ( rcTBB.left + pPM->UiScalingDo( 18, CExtPaintManager::__EUIST_X ) )
							: ( rcTBB.left + rcTBB.Width() / 2 )
							,
						rcTBB.bottom 
						);
					return ptGuideLines;
			} // if( rcPageBk.PtInRect( ptCenter ) )
		} // if( nILV < __EXT_RIBBON_ILV_SIMPLE_LARGE )
	} // if( pRibbonPage != NULL )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	pToolBar->ClientToScreen( &rcTBB );
CPoint ptGuideLines(
		rcTBB.left + rcTBB.Width() / 2,
		rcTBB.bottom 
		);
	return ptGuideLines;
}

CExtPopupKeyTipWnd * CExtBarButton::OnKeyTipGetWnd()
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( m_pWndKeyTip != NULL )
	{
		ASSERT_VALID( m_pWndKeyTip );
	}
#endif // _DEBUG
	return m_pWndKeyTip;
}

void CExtBarButton::OnKeyTipTrackingQuery(
	bool bShow,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	HDWP & hPassiveModeDWP
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pToolBar = GetBar();
	ASSERT_VALID( pToolBar );
	if( bShow )
	{
		ASSERT_VALID( pKeyTipChain );
		if(		( ! IsVisible() )
			||	( GetStyle() & TBBS_HIDDEN ) != 0
			||	pToolBar->GetSafeHwnd() == NULL
			||	( ! pToolBar->IsVisible() )
			)
		{
			OnKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
			return;
		}
		INT nIndent = pKeyTipChain->KeyCodeGetCount();
		CExtSafeString strKeyTipText = OnKeyTipGetText( nIndent );
		if( strKeyTipText.IsEmpty() )
		{
			OnKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
			return;
		}
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
		CExtBarButton * pCheckTBB = ParentButtonGet();
		for( ; pCheckTBB != NULL; )
		{
			ASSERT_VALID( pCheckTBB );
			CExtBarButton * pParentTBB = pCheckTBB->ParentButtonGet();
			if(		pParentTBB == NULL
				&&	pCheckTBB->IsKindOf( RUNTIME_CLASS(CExtRibbonButtonGroup) )
				&&	((CExtRibbonButtonGroup*)pCheckTBB)->TopCollapsedStateGet()
				)
			{
				OnKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
				return;
			}
			pCheckTBB = pParentTBB;
		}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
		if( m_pWndKeyTip == NULL )
		{
			try
			{
				m_pWndKeyTip = new CExtPopupKeyTipWnd;
			}
			catch( CException * pException )
			{
				pException->Delete();
				return;
			}
		} // if( m_pWndKeyTip == NULL )
#ifdef _DEBUG
		else
		{
			ASSERT_VALID( m_pWndKeyTip );
		} // else from if( m_pWndKeyTip == NULL )
#endif // _DEBUG
		CWnd * pWndKeyTipParent = OnKeyTipGetParentWnd();
		ASSERT_VALID( pWndKeyTipParent );
		ASSERT( pWndKeyTipParent->GetSafeHwnd() != NULL );
		CPoint ptGuideLines = OnKeyTipGetGuideLines();
		if( ! m_pWndKeyTip->Create(
				pWndKeyTipParent,
				ptGuideLines,
				strKeyTipText,
				IsEnabled(),
				hPassiveModeDWP
				)
			)
		{
			OnKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
			return;
		}
		if( hPassiveModeDWP == NULL )
			m_pWndKeyTip->ShowWindow( SW_SHOWNOACTIVATE );
	} // if( bShow )
	else
	{
		if( m_pWndKeyTip != NULL )
		{
			ASSERT_VALID( m_pWndKeyTip );
			m_pWndKeyTip->Hide();
			delete m_pWndKeyTip;
			m_pWndKeyTip = NULL;
		} // if( m_pWndKeyTip != NULL )
	} // else from if( bShow )
}

void CExtBarButton::OnKeyTipTrackingQueryNested(
	bool bShow,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	bool bApplyToThisItem,
	HDWP & hPassiveModeDWP
	)
{
	ASSERT_VALID( this );
	if( bApplyToThisItem )
		OnKeyTipTrackingQuery( bShow, pKeyTipChain, hPassiveModeDWP );
INT nChildIndex, nChildCount = ChildButtonGetCount();
	for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
	{
		CExtBarButton * pTBB = ChildButtonGetAt( nChildIndex );
		ASSERT_VALID( pTBB );
		pTBB->OnKeyTipTrackingQueryNested( bShow, pKeyTipChain, true, hPassiveModeDWP );
	} // for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
}

bool CExtBarButton::OnKeyTipTranslate(
	DWORD dwKeyCode,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	bool bAutoInvokeAction,
	bool & bActionInvoked,
	bool & bContinueKeyTipMode
	)
{
	ASSERT_VALID( this );
	bActionInvoked = false;
CExtToolControlBar * pToolBar = GetBar();
	if( pToolBar->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( pToolBar );
	if( ! IsVisible() )
		return false;
	if( ( GetStyle() & TBBS_HIDDEN ) != 0 )
		return false;
	if( IsDisabled() )
		return false;
CExtCustomizeCmdKeyTip * pCmdKeyTip = OnKeyTipGetInfo();
	if( pCmdKeyTip == NULL )
		return false;
CExtPopupKeyTipWnd * pWndKeyTip = OnKeyTipGetWnd();
	if(		pWndKeyTip->GetSafeHwnd() == NULL
		||	( pWndKeyTip->GetStyle() & WS_VISIBLE ) == 0
		)
		return false;
	if( ! pCmdKeyTip->CompareWidthChain( *pKeyTipChain ) )
		return false;
INT nKeyCodeCountOwn = pCmdKeyTip->KeyCodeGetCount();
INT nKeyCodeCountChain = pKeyTipChain->KeyCodeGetCount();
	ASSERT( nKeyCodeCountOwn > nKeyCodeCountChain );
DWORD dwKeyCodeOwn = pCmdKeyTip->KeyCodeGetAt( nKeyCodeCountChain );
	if( dwKeyCodeOwn != dwKeyCode )
		return false;
	pKeyTipChain->KeyCodeAdd( dwKeyCode );
	nKeyCodeCountChain++;
	if( nKeyCodeCountOwn > nKeyCodeCountChain )
		return true;
	if( bAutoInvokeAction )
		bActionInvoked = OnKeyTipInvokeAction( bContinueKeyTipMode );
	return true;
}

void CExtBarButton::OnKeyTipDisplay(
	CExtCustomizeCmdKeyTip & keyTipChain
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pToolBar = GetBar();
	if( pToolBar->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pToolBar );
	if( ! IsVisible() )
		return;
	if( ( GetStyle() & TBBS_HIDDEN ) != 0 )
		return;
//	if( IsDisabled() )
//		return;
CExtCustomizeCmdKeyTip * pCmdKeyTip = OnKeyTipGetInfo();
	if( pCmdKeyTip == NULL )
		return;
CExtPopupKeyTipWnd * pWndKeyTip = OnKeyTipGetWnd();
	if(		pWndKeyTip->GetSafeHwnd() == NULL
		||	( pWndKeyTip->GetStyle() & WS_VISIBLE ) == 0
		)
		return;
	if( pCmdKeyTip->CompareWidthChain( keyTipChain ) )
		return;
	pWndKeyTip->ShowWindow( SW_HIDE );
}

bool CExtBarButton::OnKeyTipInvokeAction(
	bool & bContinueKeyTipMode
	)
{
	ASSERT_VALID( this );
	if( IsAbleToTrackMenu() )
	{
		bContinueKeyTipMode = true;
		CExtToolControlBar::g_bMenuTrackingExpanded = false;
		CRect rcTBB = *this;
		CPoint pt( rcTBB.right - 1, rcTBB.bottom - 1 );
		//CList < HWND, HWND > _listHwndTipsDelayedHide;
		CExtToolControlBar * pToolBar = GetBar();
		ASSERT( pToolBar->GetSafeHwnd() != NULL );
		ASSERT_VALID( pToolBar );
		/*
		int nBtnIdx, nCountOfButtons = pToolBar->GetButtonsCount();
		for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
		{
			CExtBarButton * pTBB = pToolBar->GetButton( nBtnIdx );
			ASSERT_VALID( pTBB );
			CExtPopupKeyTipWnd * pWndKeyTip = pTBB->OnKeyTipGetWnd();
			if(		pWndKeyTip->GetSafeHwnd() == NULL
			//	||	( pWndKeyTip->GetStyle() & WS_VISIBLE ) == 0
				)
				continue;
			//_listHwndTipsDelayedHide.AddTail( pWndKeyTip->m_hWnd );
			//pWndKeyTip->ShowWindow( SW_HIDE );
			pTBB->OnKeyTipTrackingQuery( false );
		}
		*/
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
		if(		IsKindOf( RUNTIME_CLASS(CExtRibbonButtonGroup) )
			&&	((CExtRibbonButtonGroup*)this)->TopCollapsedStateGet()
			)
		{
			int nBtnIdx, nCountOfButtons = pToolBar->GetButtonsCount();
			for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
			{
				CExtBarButton * pTBB = pToolBar->GetButton( nBtnIdx );
				ASSERT_VALID( pTBB );
				CExtPopupKeyTipWnd * pWndKeyTip = pTBB->OnKeyTipGetWnd();
				if( pWndKeyTip->GetSafeHwnd() == NULL )
					continue;
				HDWP hPassiveModeDWP = NULL;
				pTBB->OnKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
			}
		}
		else
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
		{
			CExtMenuControlBar * pMenuBar = DYNAMIC_DOWNCAST( CExtMenuControlBar, pToolBar );
			if( pMenuBar != NULL )
				pMenuBar->_CancelFlatTracking( FALSE );
			else
				pToolBar->OnFlatTrackingStop();
		}
		pToolBar->Invalidate();
		//pToolBar->UpdateWindow();
		CExtPaintManager::stat_PassPaintMessages();
		pToolBar->KeyTipsDisplayedSet( true );
		OnTrackPopup( pt, true, true );
		pToolBar->KeyTipsDisplayedSet( false );
		return true;
	}
	bContinueKeyTipMode = false;
CWnd * pCtrl = CtrlGet();
	if(		pCtrl != NULL
		&&	( pCtrl->GetStyle() & WS_VISIBLE ) != 0
		)
	{
		ASSERT_VALID( pCtrl );
		CExtToolControlBar * pBar = GetSafeBar();
		ASSERT_VALID( pBar );
		if( CWnd::GetCapture() == pBar )
			pBar->SendMessage( WM_CANCELMODE );
		pCtrl->SetFocus();
		return true;
	}
	OnDeliverCmd();
	return true;
}

#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtBarButton::OnFlatTrackingStart(
	HDWP & hPassiveModeDWP
	)
{
	ASSERT_VALID( this );
	hPassiveModeDWP;
CExtToolControlBar * pBar = GetSafeBar();
	if( pBar == NULL )
		return;
	ASSERT_VALID( pBar );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	OnKeyTipTrackingQuery( true, &(pBar->KeyTipChainGet()), hPassiveModeDWP );
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)
}

void CExtBarButton::OnFlatTrackingStop()
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
HDWP hPassiveModeDWP = NULL;
	OnKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)
}

bool CExtBarButton::OnQueryFlatTrackingEnabled() const
{
	ASSERT_VALID( this );
	if( ! IsVisible() )
		return false;
	if( ( GetStyle() & TBBS_HIDDEN ) != 0 )
		return false;
	if( IsSeparator() )
		return false;
	if( ChildButtonGetCount() > 0 )
		return false;
	return true;
}

CRect CExtBarButton::OnQueryFlatWalkingCalcRect() const
{
	ASSERT_VALID( this );
CRect rcOwn = *this;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( OnQueryFlatTrackingEnabled() )
	{
		const CExtBarButton * pParentTBB = ParentButtonGet();
		if(		pParentTBB != NULL
			&&	pParentTBB->ParentButtonGet() == NULL
			&&	pParentTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonGroup ) )
			&&	(! pParentTBB->IsKindOf( RUNTIME_CLASS( CExtRibbonButtonToolGroup ) ) )
			)
		{
			ASSERT_VALID( pParentTBB );
			INT nSiblingIndex, nSavedSiblingIndex = pParentTBB->ChildButtonGetIndexOf( this );
			INT nSiblingCount = pParentTBB->ChildButtonGetCount();
			ASSERT( 0 <= nSavedSiblingIndex && nSavedSiblingIndex < nSiblingCount );
			for( nSiblingIndex = nSavedSiblingIndex; nSiblingIndex < nSiblingCount; nSiblingIndex ++ )
			{
				const CExtBarButton * pTBB = pParentTBB->ChildButtonGetAt( nSiblingIndex );
				ASSERT_VALID( pTBB );
				if( ! pTBB->OnQueryFlatTrackingEnabled() )
					continue;
				if( pTBB->IsWrap( __EVT_FLOAT ) )
					break;
				CRect rcTBB = *pTBB;
				rcOwn.left = min( rcOwn.left, rcTBB.left );
				rcOwn.right = max( rcOwn.right, rcTBB.right );
			} // for( nSiblingIndex = nSavedSiblingIndex; nSiblingIndex < nSiblingCount; nSiblingIndex ++ )
			for( nSiblingIndex = nSavedSiblingIndex; nSiblingIndex > 0; nSiblingIndex -- )
			{
				const CExtBarButton * pTBB = pParentTBB->ChildButtonGetAt( nSiblingIndex - 1 );
				ASSERT_VALID( pTBB );
				if( ! pTBB->OnQueryFlatTrackingEnabled() )
					continue;
				if( pTBB->IsWrap( __EVT_FLOAT ) )
					break;
				CRect rcTBB = *pTBB;
				rcOwn.left = min( rcOwn.left, rcTBB.left );
				rcOwn.right = max( rcOwn.right, rcTBB.right );
			} // for( nSiblingIndex = nSavedSiblingIndex; nSiblingIndex > 0; nSiblingIndex -- )
		} // if( pParentTBB != NULL ...
	} // if( OnQueryFlatTrackingEnabled() )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	return rcOwn;
}

void CExtBarButton::_DestroyMenu()
{
	if( m_hMenu == NULL )
		return;
	if( m_bAutoDestroyMenu )
	{
		ASSERT( ::IsMenu(m_hMenu) );
		VERIFY( ::DestroyMenu(m_hMenu) );
	}
	m_hMenu = NULL;
}

bool CExtBarButton::IsSeparator() const
{
	ASSERT_VALID( this );
	bool bSeparator =
		(GetStyle() & TBBS_SEPARATOR) ?
			true : false;
	return bSeparator;
}

bool CExtBarButton::IsDisabled() const
{
	ASSERT_VALID( this );
bool bDisabled =
		(GetStyle() & TBBS_DISABLED) ?
			true : false;
	return bDisabled;
}

bool CExtBarButton::IsShowTipWhenDisabled() const
{
	ASSERT_VALID( this );
const CExtToolControlBar * pBar = GetBar();
	if(		pBar != NULL
		&&	( ! pBar->IsShowTipWhenDisabled( this ) )
		)
		return false;
	return true;
}

bool CExtBarButton::IsEnabled() const
{
	ASSERT_VALID( this );
	return !( IsDisabled() );
}

bool CExtBarButton::IsIndeterminate() const
{
	ASSERT_VALID( this );
bool bIndeterminate =
		(GetStyle() & TBBS_INDETERMINATE) ?
			true : false;
	return bIndeterminate;
}

bool CExtBarButton::IsPressed() const
{
	ASSERT_VALID( this );
bool bPressed =
		(GetStyle() & (TBBS_PRESSED|TBBS_CHECKED)) ?
			true : false;
	return bPressed;
}

bool CExtBarButton::IsHover() const
{
	ASSERT_VALID( this );
	return m_bHover;
}

void CExtBarButton::SetHover(
	bool bHover // = true
	)
{
//	TRACE2( ">>>>>>>>>>>>SetHover - %p, %s\n", this, bHover ? "on" : "--------OFF" );
	ASSERT_VALID( this );
bool bAnimate = false;
bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
	if( ! bAnimationLocked )
	{
		if( m_bHover != bHover )
		{
			bAnimate = true;
			AnimationClient_CacheGeneratorLock();
 	AnimationClient_StateGet( true ).Empty();
 	AnimationClient_StateGet( false ).Empty();
//			if( AnimationClient_StateGet(true).IsEmpty() )
 				AnimationClient_CacheNextStateMinInfo(
					false,
					bHover
						? __EAPT_BY_HOVERED_STATE_TURNED_ON
						: __EAPT_BY_HOVERED_STATE_TURNED_OFF
					);
			//AnimationClient_CacheGeneratorUnlock();
		}
	} // if( ! bAnimationLocked )
	m_bHover = bHover;
	OnUpdateParentStyle();
	if( bAnimate )
	{
			//AnimationClient_CacheGeneratorLock();
			AnimationClient_CacheNextStateMinInfo(
				true,
				bHover
					? __EAPT_BY_HOVERED_STATE_TURNED_ON
					: __EAPT_BY_HOVERED_STATE_TURNED_OFF
				);
			AnimationClient_CacheGeneratorUnlock();
	} // if( bAnimate )
	_UpdateCtrl();
}

CWnd * CExtBarButton::CtrlGet()
{
	ASSERT_VALID( this );
	return m_pCtrl;
}

const CWnd * CExtBarButton::CtrlGet() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtBarButton * > ( this ) ) ->
		CtrlGet();
}

void CExtBarButton::CtrlSet(
	CWnd * pCtrl,
	bool bCtrlAutoDestroyed
	)
{
	ASSERT_VALID( this );
	if( m_pCtrl != NULL )
	{
		CWnd * pOldCtrl = m_pCtrl;
		m_pCtrl = NULL;
		if( pOldCtrl->GetSafeHwnd() != NULL )
			pOldCtrl->DestroyWindow();
		if( ! m_bCtrlAutoDestroyed )
			delete pOldCtrl;
	}
	m_pCtrl = pCtrl;
	m_bCtrlAutoDestroyed = bCtrlAutoDestroyed;
#ifdef _DEBUG
	if( m_pCtrl != NULL )
	{
		ASSERT_VALID( m_pCtrl );
	}
#endif // _DEBUG
}

CWnd * CExtBarButton::CtrlDetach()
{
	ASSERT_VALID( this );
CWnd * pCtrl = m_pCtrl;
	m_pCtrl = NULL;
	return pCtrl;
}

UINT CExtBarButton::GetStyle() const
{
	ASSERT_VALID( this );
	return m_nStyle;
}

void CExtBarButton::SetStyle( UINT nStyle )
{
	ASSERT_VALID( this );
DWORD nOldStyle = m_nStyle;
DWORD nNewStyle = nStyle;

bool bOldPressed = ( (nOldStyle&TBBS_PRESSED) != 0 || IsPressed() ) ? true : false;
	m_nStyle = nNewStyle;
bool bNewPressed = ( (nNewStyle&TBBS_PRESSED) != 0 || IsPressed() ) ? true : false;
	m_nStyle = nOldStyle;

//	if( (nOldStyle&TBBS_PRESSED) != (nNewStyle&TBBS_PRESSED) /*&& (nNewStyle&TBBS_PRESSED) == 0*/ )
	if( bOldPressed != bNewPressed )
	{
		AnimationClient_CacheGeneratorLock();
		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				bNewPressed
					? __EAPT_BY_PRESSED_STATE_TURNED_ON
					: __EAPT_BY_PRESSED_STATE_TURNED_OFF
				);
//		AnimationClient_CacheGeneratorUnlock();
	}

	m_nStyle = nNewStyle;
	
	if( (m_nStyle & TBBS_PRESSED) == 0 && (! IsHover() ) )
		m_bDropDownHT = false;
	OnUpdateParentStyle();
	_UpdateCmdIdFromStyle();
	_UpdateCtrl();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	_UpdateCmdNodeFromStyle();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

//	if( (nOldStyle&TBBS_PRESSED) != (nNewStyle&TBBS_PRESSED) /*&& (nNewStyle&TBBS_PRESSED) == 0*/ )
	if( bOldPressed != bNewPressed )
	{
//		AnimationClient_CacheGeneratorLock();
		AnimationClient_CacheNextStateMinInfo(
			true,
			bNewPressed
				? __EAPT_BY_PRESSED_STATE_TURNED_ON
				: __EAPT_BY_PRESSED_STATE_TURNED_OFF
			);
		AnimationClient_CacheGeneratorUnlock();
// 		CWindowDC dc( NULL );
// 		AnimationClient_StateGet( false ).Draw(
// 			dc.m_hDC,
// 			100,
// 			0,
// 			AnimationClient_StateGet( false ).GetSize().cx,
// 			AnimationClient_StateGet( false ).GetSize().cy
// 			);
// 		AnimationClient_StateGet( true ).Draw(
// 			dc.m_hDC,
// 			0,
// 			0,
// 			AnimationClient_StateGet( true ).GetSize().cx,
// 			AnimationClient_StateGet( true ).GetSize().cy
// 			);
	}

}

void CExtBarButton::ModifyStyle(
	UINT nAdd,
	UINT nRemove // = 0
	)
{
	ASSERT_VALID( this );
DWORD nOldStyle = m_nStyle;
DWORD nNewStyle = m_nStyle;
	nNewStyle |= nAdd;
	nNewStyle &= ~nRemove;
	
bool bOldPressed = ( (nOldStyle&TBBS_PRESSED) != 0 || IsPressed() ) ? true : false;
	m_nStyle = nNewStyle;
bool bNewPressed = ( (nNewStyle&TBBS_PRESSED) != 0 || IsPressed() ) ? true : false;
	m_nStyle = nOldStyle;

//	if( (nOldStyle&TBBS_PRESSED) != (nNewStyle&TBBS_PRESSED) /*&& (nNewStyle&TBBS_PRESSED) == 0*/ )
	if( bOldPressed != bNewPressed )
	{
		AnimationClient_CacheGeneratorLock();
		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				bNewPressed
					? __EAPT_BY_PRESSED_STATE_TURNED_ON
					: __EAPT_BY_PRESSED_STATE_TURNED_OFF
				);
//		AnimationClient_CacheGeneratorUnlock();
	}
	
	m_nStyle = nNewStyle;
	if( (m_nStyle & TBBS_PRESSED) == 0 && (! IsHover() ) )
		m_bDropDownHT = false;
	OnUpdateParentStyle();
	_UpdateCmdIdFromStyle();
	_UpdateCtrl();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	_UpdateCmdNodeFromStyle();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

//	if( (nOldStyle&TBBS_PRESSED) != (nNewStyle&TBBS_PRESSED) /*&& (nNewStyle&TBBS_PRESSED) == 0*/ )
	if( bOldPressed != bNewPressed )
	{
//		AnimationClient_CacheGeneratorLock();
		AnimationClient_CacheNextStateMinInfo(
			true,
			bNewPressed
				? __EAPT_BY_PRESSED_STATE_TURNED_ON
				: __EAPT_BY_PRESSED_STATE_TURNED_OFF
			);
		AnimationClient_CacheGeneratorUnlock();
// 		CWindowDC dc( NULL );
// 		AnimationClient_StateGet( false ).Draw(
// 			dc.m_hDC,
// 			100,
// 			0,
// 			AnimationClient_StateGet( false ).GetSize().cx,
// 			AnimationClient_StateGet( false ).GetSize().cy
// 			);
// 		AnimationClient_StateGet( true ).Draw(
// 			dc.m_hDC,
// 			0,
// 			0,
// 			AnimationClient_StateGet( true ).GetSize().cx,
// 			AnimationClient_StateGet( true ).GetSize().cy
// 			);
	}

}

void CExtBarButton::OnUpdateParentStyle()
{
	ASSERT_VALID( this );
/*
	if( AnimationClient_CacheGeneratorIsLocked() )
		return;
CExtBarButton * pParentTBB = ParentButtonGet();
	if( pParentTBB == NULL )
		return;
	ASSERT_VALID( pParentTBB );

bool bOwnHover = ( IsEnabled() && ( IsHover() || IsPressed() ) ) ? true : false;
bool bParentHover = ( pParentTBB->IsHover() ) ? true : false;
// bool bOwnHover = m_bHover;
// bool bParentHover = pParentTBB->m_bHover;
	if( bParentHover != bOwnHover )
	{
		bool bResetParentState = true;
		if( ! bOwnHover )
		{
			CExtToolControlBar * pBar = GetBar();
			if( pBar )
			{
				ASSERT_VALID( pBar );
				INT nBtnIdxHover = pBar->_GetHoverButtonIndex();
				if( nBtnIdxHover >= 0 )
				{
					CExtBarButton * pHoverTBB = pBar->GetButton( nBtnIdxHover );
					ASSERT_VALID( pHoverTBB );
					if( pParentTBB->IsChildButton( pHoverTBB ) )
						bResetParentState = false;
				} // if( nBtnIdxHover >= 0 )
			} // if( pBar )
		} // if( ! bOwnHover )
		if( bResetParentState )
			pParentTBB->SetHover( bOwnHover );
		pParentTBB->RedrawButton( false );
	} // if( bParentHover != bOwnHover )
*/
}

bool CExtBarButton::OnInvalidateButton()
{
	ASSERT_VALID( this );
	return false;
}

void CExtBarButton::OnPressedTrackingStart()
{
	ASSERT_VALID( this );
	m_bPressedTracking = true;
}

void CExtBarButton::OnPressedTrackingStop()
{
	ASSERT_VALID( this );
	m_bPressedTracking = false;
}

bool CExtBarButton::IsPressedTracking() const
{
	ASSERT_VALID( this );
	return m_bPressedTracking;
}

void CExtBarButton::SetVertDocked( 
	bool bVertDocked 
	)
{
	ASSERT_VALID( this );
	m_bVertDocked = bVertDocked;
}

bool CExtBarButton::IsWrap(
	CExtBarButton::e_wrap_t _evt
	) const
{
	ASSERT_VALID( this );
	ASSERT( __EVT_MIN_VALUE <= _evt && _evt <= __EVT_MAX_VALUE );
	switch( _evt )
	{
	case __EVT_HORZ:
		return m_bWrapH;
	case __EVT_VERT:
		return m_bWrapV;
	default:
		ASSERT( _evt == __EVT_FLOAT );
		return m_bWrapF;
	} // switch( _evt )
}

void CExtBarButton::SetWrap(
	CExtBarButton::e_wrap_t _evt,
	bool bWrap // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( __EVT_MIN_VALUE <= _evt && _evt <= __EVT_MAX_VALUE );
	switch( _evt )
	{
	case __EVT_HORZ:
		m_bWrapH = bWrap;
		break;
	case __EVT_VERT:
		m_bWrapV = bWrap;
		break;
	default:
		ASSERT( _evt == __EVT_FLOAT );
		m_bWrapF = bWrap;
		break;
	} // switch( _evt )
}

bool CExtBarButton::IsLargeIcon() const
{
	ASSERT_VALID( this );
	if( GetBar() == NULL )
		return false;
	if( IsSeparator() )
		return false;
const CExtToolControlBar * pBar = GetBar();
	return pBar->IsLargeIcons();
}

bool CExtBarButton::IsDisplayScreenTip() const
{
	ASSERT_VALID( this );
	if( GetBar() == NULL )
		return false;
	if( IsSeparator() )
		return false;
const CExtToolControlBar * pBar = GetBar();
	return pBar->IsDisplayScreenTips();
}

bool CExtBarButton::IsDisplayShortcutKeysOnScreenTip() const
{
	ASSERT_VALID( this );
	if( GetBar() == NULL )
		return false;
	if( IsSeparator() )
		return false;
const CExtToolControlBar * pBar = GetBar();
	return pBar->IsDisplayShortcutKeysOnScreenTips();
}

UINT CExtBarButton::GetCmdID(
	bool bEffective // = true
	) const
{
	ASSERT_VALID( this );
/*
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
bool bUseNode = true;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( GetSeparatedDropDown() )
	{
		const CExtToolControlBar * pToolBar = GetBar();
		if( pToolBar != NULL )
		{
			ASSERT_VALID( pToolBar );
			if( pToolBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) ) )
				bUseNode = false;
		}
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( bUseNode )
	{
		const CExtCustomizeCmdTreeNode * pNode = GetCmdNode( true );
		if( pNode == NULL )
			pNode = GetCmdNode( false );
		if( pNode != NULL )
			return pNode->GetCmdID( bEffective );
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
*/
	return bEffective ? m_nEffectiveCmdID : m_nButtonCmdID;
}

void CExtBarButton::SetCmdID(
	UINT nCmdID,
	bool bEffectiveOnly, // = false
	bool bEffectiveRecalcLayout // = false
	)
{
	ASSERT_VALID( this );
/*
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
bool bUseNode = true;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( GetSeparatedDropDown() )
	{
		const CExtToolControlBar * pToolBar = GetBar();
		if( pToolBar != NULL )
		{
			ASSERT_VALID( pToolBar );
			if( pToolBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) ) )
				bUseNode = false;
		}
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( bUseNode )
	{
		CExtCustomizeCmdTreeNode * pNode = GetCmdNode( true );
		if( pNode == NULL )
			pNode = GetCmdNode( false );
		if( pNode != NULL )
			pNode->SetCmdID( nCmdID, bEffectiveOnly );
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
*/
bool bRecalcLayout = false;
	if( bEffectiveRecalcLayout && m_nEffectiveCmdID != nCmdID )
		bRecalcLayout = true; 
	m_nEffectiveCmdID = nCmdID;
	if( ! bEffectiveOnly )
	{
		m_nButtonCmdID = nCmdID;
		if( m_nButtonCmdID == ID_SEPARATOR )
		{
			m_nStyle |= TBBS_SEPARATOR;
			m_nStyle &= ~(TBBS_BUTTON);
		}
		else
		{
			m_nStyle |= TBBS_BUTTON;
			m_nStyle &= ~(TBBS_SEPARATOR);
		}
	} // if( !bEffectiveOnly )
	if( bRecalcLayout )
	{
		CExtToolControlBar * pBar = GetBar();
		if( pBar->GetSafeHwnd() != NULL )
		{
			pBar->Invalidate();
			CFrameWnd * pFrame = pBar->GetParentFrame();
			if( pFrame != NULL )
				pFrame->RecalcLayout();
			else
				pBar->_RecalcLayoutImpl();
		} // if( pBar->GetSafeHwnd() != NULL )
	} // if( bRecalcLayout )
}

bool CExtBarButton::CanBePressedInDisabledState()
{
	return IsAbleToTrackMenu();
}

bool CExtBarButton::IsPaintDropDown(
	bool bCustomizeMode // = false
	) const
{
	ASSERT_VALID( this );
const CExtToolControlBar * pBar = GetSafeBar();
	if( pBar == NULL )
		return false;
bool bPaintAsDropDown = 
		(		IsAbleToTrackMenu( bCustomizeMode )
			//&&(! pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
			&&	(! pBar->_IsSimplifiedDropDownButtons() )
			) ? true : false;
	return bPaintAsDropDown;
}

bool CExtBarButton::IsAbleToTrackMenu(
	bool bCustomizeMode // = false
	) const
{
	ASSERT_VALID( this );
	bCustomizeMode;
	if(		( const_cast < CExtBarButton * > ( this ) ) -> GetMenu() != NULL
		||	IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton))
		)
		return true;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
const CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if(		pNode != NULL
		&&	(	(pNode->GetFlags() & __ECTN_AUTO_POPUP) != 0
			||	pNode->GetNodeCount() > 0
			)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return false;
}

bool CExtBarButton::IsContainerOfPopupLikeMenu()
{
	ASSERT_VALID( this );
	ASSERT(
			( m_hMenu == NULL )
		||	( m_hMenu != NULL && ::IsMenu(m_hMenu) )
		);
	return m_bPopupMenu;
}

CExtSafeString CExtBarButton::GetText() const
{
	ASSERT_VALID( this );
CExtSafeString sText( _T("") );
	if( IsSeparator() )
		return sText;
const CExtToolControlBar * pBar = GetBar();
HWND hWndBar = pBar->GetSafeHwnd();
CExtCmdItem * pCmdItem = NULL;
	if( hWndBar != NULL )
		pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( hWndBar ),
				GetCmdID( true )
				);
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode =
		((CExtBarButton*)this)->GetCmdNode();
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return CExtSafeString( pNode->GetTextInToolbar( pCmdItem ) );
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( pCmdItem == NULL )
		return sText;
	if( ! pCmdItem->m_sToolbarText.IsEmpty() )
		sText = pCmdItem->m_sToolbarText;
int nTabChrPos = sText.Find( _T('\t') );
	if( nTabChrPos < 0 )
		return sText;
	return sText.Left( nTabChrPos );
}

bool CExtBarButton::GetSeparatedDropDown() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		if( ( pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN ) != 0 )
			return true;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_bSeparatedDropDown;
}

void CExtBarButton::SetSeparatedDropDown(
	bool bSeparatedDropDown // = true
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->ModifyFlags(
			bSeparatedDropDown ? __ECTN_TBB_SEPARATED_DROPDOWN : 0,
			bSeparatedDropDown ? 0 : __ECTN_TBB_SEPARATED_DROPDOWN
			);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_bSeparatedDropDown == bSeparatedDropDown )
		return;
	m_bSeparatedDropDown = bSeparatedDropDown;
	_UpdateCtrl();
}

bool CExtBarButton::GetAutoChangeID() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return (pNode->GetFlags()&__ECTN_TBB_AUTOCHANGE_ID) ? true : false;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_bAutoChangeID;
}
void CExtBarButton::SetAutoChangeID(
	bool bAutoChangeID // = true
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->ModifyFlags(
			bAutoChangeID ? __ECTN_TBB_AUTOCHANGE_ID : 0,
			bAutoChangeID ? 0 : __ECTN_TBB_AUTOCHANGE_ID
			);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_bAutoChangeID == bAutoChangeID )
		return;
	m_bAutoChangeID = bAutoChangeID;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	// reset text
	if( m_bAutoChangeID )
		SetCmdID( GetCmdID(true), true );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	_UpdateCtrl();
}

bool CExtBarButton::GetNoRotateVerticalLayout() const
{
	ASSERT_VALID( this );
const CExtToolControlBar * pToolBar = GetBar();
	if( pToolBar != NULL )
	{
		ASSERT_VALID( pToolBar );
		if( pToolBar->m_bPaletteMode )
			return true;
	}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return (pNode->GetFlags()&__ECTN_TBB_NO_ROTATE_VL) ? true : false;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_bNoRotateVerticalLayout;
}
void CExtBarButton::SetNoRotateVerticalLayout(
	bool bNoRotateVerticalLayout // = true
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->ModifyFlags(
			bNoRotateVerticalLayout ? __ECTN_TBB_NO_ROTATE_VL : 0,
			bNoRotateVerticalLayout ? 0 : __ECTN_TBB_NO_ROTATE_VL
			);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_bNoRotateVerticalLayout == bNoRotateVerticalLayout )
		return;
	m_bNoRotateVerticalLayout = bNoRotateVerticalLayout;
	_UpdateCtrl();
}

bool CExtBarButton::GetCtrlVisibleVertically() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return (pNode->GetFlags()&__ECTN_TBB_CTRL_VIS_VL) ? true : false;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_bCtrlVisibleVert;
}
void CExtBarButton::SetCtrlVisibleVertically( bool bVisible )
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->ModifyFlags(
			bVisible ? __ECTN_TBB_CTRL_VIS_VL : 0,
			bVisible ? 0 : __ECTN_TBB_CTRL_VIS_VL
			);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_bCtrlVisibleVert = bVisible;
	_UpdateCtrl();
}

BOOL CExtBarButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	ASSERT_VALID( this );
	ASSERT( pPopup != NULL );
	ASSERT( pPopup->GetSafeHwnd() == NULL );
CWnd * pCtrl = CtrlGet();
	if( pCtrl != NULL )
	{
		if( pCtrl->SendMessage(
				CExtToolControlBar::g_nMsgPutToPopupMenu,
				reinterpret_cast <WPARAM> ( pPopup ),
				0
				)
			)
			return TRUE;
	}
CExtToolControlBar * pBar = GetBar();
	if( IsAbleToTrackMenu() )
	{
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( pBar->GetSafeHwnd() ),
				GetCmdID( false )
				);
		ASSERT( pCmdItem != NULL );

//		CExtSafeString sText = pCmdItem->m_sMenuText;
//		if( sText.IsEmpty() )
//			sText = pCmdItem->m_sToolbarText;
		CExtSafeString sText = pCmdItem->m_sToolbarText;
		if( sText.IsEmpty() )
			sText = pCmdItem->m_sMenuText;

		const CExtCmdIcon & _icon = GetIcon();
		VERIFY( pPopup->ItemInsert( (UINT)CExtPopupMenuWnd::TYPE_POPUP, -1, LPCTSTR(sText), _icon ) );
		CExtPopupMenuWnd * pChildPopup = pPopup->ItemGetPopup( pPopup->ItemGetCount() - 1 );
		ASSERT( pChildPopup != NULL );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			VERIFY(
				pChildPopup->UpdateFromCmdTree(
					GetCmdTargetWnd()->GetSafeHwnd(),
					pNode,
					false
					)
				);
			CExtPopupMenuWnd::MENUITEMDATA & _mii = pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
			_mii.SetCmdNode( pNode );
			sText = pNode->GetTextInToolbar( pCmdItem, true );
			if( ! sText.IsEmpty() )
				_mii.SetText( LPCTSTR(sText) );
		} // if( pNode != NULL )
		else
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		{
			HMENU hMenu = GetMenu();
			ASSERT( hMenu != NULL );
			ASSERT( ::IsMenu(hMenu) );
			CMenu _menu;
			_menu.Attach( hMenu );
			pChildPopup->UpdateFromMenu(
				GetCmdTargetWnd()->GetSafeHwnd(),
				&_menu,
				IsContainerOfPopupLikeMenu(),
				false
				);
			_menu.Detach();
		} // else from if( pNode != NULL )
		if( (pPopup->TrackFlagsGet()&TPMX_PALETTE) != 0 )
			pPopup->ItemGetInfo(
				pPopup->ItemGetCount()-1
				).SetChildCombine();
	} // if( IsAbleToTrackMenu() )
	else
	{
		if( (pPopup->TrackFlagsGet()&TPMX_PALETTE) != 0 && (! IsSeparator() ) )
		{
			int nCheck = 0;
			if( (GetStyle()&TBBS_CHECKED) != 0 )
				nCheck = 1;
			const CExtCmdIcon & _icon = GetIcon();
			CExtSafeString strButtonText = GetText();
			HWND hWndCmdReceiver = pBar->GetOwner()->GetSafeHwnd();
			if( ! pPopup->ItemInsertCommand(
					GetCmdID(true),
					-1,
					strButtonText.IsEmpty() ? NULL : LPCTSTR(strButtonText),
					NULL,
					_icon,
					nCheck,
					hWndCmdReceiver
					)
				)
			{
				ASSERT( FALSE );
				return FALSE;
			}
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd( pBar->GetSafeHwnd() ),
					GetCmdID( false )
					);
			CExtPopupMenuWnd::MENUITEMDATA & mi = 
				pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
			mi.Enable( IsEnabled() );
			mi.SetNoCmdUI( true );
			if(		pCmdItem != NULL
				&&	(! pCmdItem->m_sTipTool.IsEmpty() )
				)
				mi.SetCustomTip( LPCTSTR(pCmdItem->m_sTipTool) );
		} // if( (pPopu p->TrackFlagsGet()&TPMX_PALETTE) != 0 && (! IsSeparator() ) )
		else
		{
			if( ! pPopup->ItemInsert( GetCmdID(true) ) )
			{
				ASSERT( FALSE );
				return FALSE;
			}
		} // else from if( (pPopup->TrackFlagsGet()&TPMX_PALETTE) != 0 && (! IsSeparator() ) )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			pPopup->ItemGetInfo(
				pPopup->ItemGetCount() - 1
				).SetCmdNode( pNode );
		} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	} // else from if( IsAbleToTrackMenu() )
	return TRUE;
}

CRect CExtBarButton::RectDropDown() const
{
	ASSERT_VALID( this );
CRect rcBtnDropDownArea = Rect();

BOOL bHorz = TRUE;
const CExtToolControlBar * pToolBar = GetBar();
	ASSERT_VALID( pToolBar );

	if( (!GetNoRotateVerticalLayout()) && pToolBar->IsDockedVertically() )
		bHorz = FALSE;

bool bBarIsRTL = false;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( ! pToolBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) ) )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	{
		bBarIsRTL = 
			( (pToolBar->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
				? true : false;
		if(		(! bBarIsRTL)
			&&	pToolBar->m_pDockBar == NULL
			)
			bBarIsRTL =
				(	(pToolBar->GetParent()->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
					? true : false;
	}
	if( bHorz )
	{
		if( bBarIsRTL )
			rcBtnDropDownArea.right = rcBtnDropDownArea.left + _CalcSplitButtonExtent( true );
		else
			rcBtnDropDownArea.left = rcBtnDropDownArea.right - _CalcSplitButtonExtent( true );
	} // if( bHorz )
	else
		rcBtnDropDownArea.top = rcBtnDropDownArea.bottom - _CalcSplitButtonExtent( false );
	return rcBtnDropDownArea;
}

CRect CExtBarButton::RectWithoutDropDown() const
{
	ASSERT_VALID( this );
CRect rc = Rect();
	if( GetSeparatedDropDown() )
	{
		CRect rcDD = RectDropDown();
		rc.right = rcDD.left;
	}
	return rc;
}

bool CExtBarButton::_UpdateCtrlIsEnabled()
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	if( pBar->GetSafeHwnd() == NULL )
		return false;
	if( ! pBar->_UpdateCtrlIsEnabled( this ) )
		return false;
	return true;
}

void CExtBarButton::_UpdateCtrl()
{
	ASSERT_VALID( this );
	if( ! _UpdateCtrlIsEnabled() )
		return;
CExtToolControlBar * pBar = GetBar();
	if( pBar->GetSafeHwnd() == NULL )
		return;
CWnd * pCtrl = CtrlGet();
	if( pCtrl == NULL )
		return;
bool bShow = 
		(	IsVisible()
		&&	( (! m_bVertDocked) || GetCtrlVisibleVertically() )
		&&	( m_nStyle & TBBS_HIDDEN ) == 0
		)
		? true : false;
	if( bShow )
	{
		ASSERT_VALID( pCtrl );
		CRect rcCtrl;
		pCtrl->GetWindowRect( &rcCtrl );
		pBar->ScreenToClient( &rcCtrl );
		CRect rcMove(
			m_ActiveRect.TopLeft(),
			rcCtrl.Size()
			);
		if( m_bVertDocked )
		{
			int nOffset = (m_ActiveRect.Width() - rcCtrl.Width()) / 2;
			rcMove.OffsetRect( nOffset, 0 );
		} // if( m_bVertDocked )
		else
		{
			int nOffset = (m_ActiveRect.Height() - rcCtrl.Height()) / 2;
			rcMove.OffsetRect( 0, nOffset );
		} // else from if( m_bVertDocked )
		if( rcCtrl != rcMove )
			pCtrl->MoveWindow( &rcMove, FALSE );
	} // if( bShow )
	pCtrl->ShowWindow( bShow ? SW_SHOW : SW_HIDE );
	pCtrl->EnableWindow( IsEnabled() );
	if( pCtrl->IsKindOf( RUNTIME_CLASS( CButton ) ) )
	{
		int nCheck = 0;
		if( IsPressed() )
			nCheck = 1;
		else if( IsIndeterminate() )
			nCheck = 2;
		((CButton *)(pCtrl))->SetCheck( nCheck );
	}
//	if( IsVisible() && m_bHover )
//		pCtrl->SetFocus();
}

void CExtBarButton::SetLayoutRect( CDC &dc, const RECT & rectButton )
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	SetRect( rectButton );
}

void CExtBarButton::SetRect( const RECT & rectButton )
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	GetBar()->AnimationSite_ClientProgressStop(
		this
		);
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
	AnimationClient_TargetRectSet( rectButton );
	m_ActiveRect = rectButton;
CWnd * pCtrl = CtrlGet();
	if( pCtrl != NULL )
		_UpdateCtrl();
}

CRect CExtBarButton::Rect() const
{
	ASSERT_VALID( this );
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
CRect rcBtn = m_ActiveRect;
	if(		GetBar() != NULL
		&&	pBar->PmBridge_GetPM()->GetCb2DbTransparentMode( (CObject *)(this) )
		&&	pBar->PmBridge_GetPM()->IsMenuMustCombineExcludeArea( this )
		&&	pBar->m_pDockSite != NULL
		&&	(! pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
		)
	{
		const CSize _sizeRoundedAreaMerics =
			pBar->PmBridge_GetPM()->FixedBar_GetRoundedAreaMerics();
		CRect rcClient;
		pBar->GetClientRect( &rcClient );
		CRect rcBtn2(rcBtn);
		if( pBar->IsDockedVertically() )
		{
			rcBtn2.OffsetRect(
				rcBtn.left - rcClient.left,
				rcClient.bottom - rcBtn.bottom
				);
			rcBtn2.top = rcBtn.top - _sizeRoundedAreaMerics.cy*2-1;
			rcBtn2.right = rcClient.right;
		} // if( pBar->IsDockedVertically() )
		else if( pBar->IsDockedHorizontally() )
		{
			rcBtn2.OffsetRect(
				rcClient.right - rcBtn.right,
				rcBtn.top - rcClient.top
				);
			rcBtn2.left = rcBtn.left - _sizeRoundedAreaMerics.cx*2-1;
			rcBtn2.bottom = rcClient.bottom;
		} // else if( pBar->IsDockedHorizontally() )
	}
	return rcBtn;
}

LONG CExtBarButton::_CalcSplitButtonExtent( bool bHorz ) const
{
	ASSERT_VALID( this );
	return GetSafeBar()->_CalcSplitButtonExtent( this, bHorz );
}

CSize CExtBarButton::CalculateLayout(
	CDC & dc,
	CSize sizePreCalc,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT_VALID( (&dc) );

	if( GetNoRotateVerticalLayout() )
		bHorz = TRUE;

CWnd * pCtrl = CtrlGet();
	if(		( pCtrl != NULL ) 
		&&	( (! m_bVertDocked ) || GetCtrlVisibleVertically() ) 
		)
	{
		ASSERT_VALID( pCtrl );
		CRect rcCtrl;
		pCtrl->GetWindowRect( &rcCtrl );
		m_ActiveSize.cx = rcCtrl.Width();
		m_ActiveSize.cy = rcCtrl.Height();
		if( m_ActiveSize.cy < sizePreCalc.cy )
			m_ActiveSize.cy = sizePreCalc.cy;
		return m_ActiveSize;
	}
	m_ActiveSize = sizePreCalc;

CExtToolControlBar * pBar = GetBar();
	if( IsSeparator() )
	{
		if( bHorz )
			m_ActiveSize.cx = __EXT_TB_BUTTON_SEPARATOR_WIDTH;
		else
			m_ActiveSize.cy = __EXT_TB_BUTTON_SEPARATOR_HEIGHT;
	}
	else
	{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if( GetCmdNode() == NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		{
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd( pBar->GetSafeHwnd() ),
					GetCmdID( true )
					);
			if( pCmdItem == NULL )
				return m_ActiveSize; // shutdown mode
		}

		// TODO
		bool bBold = false;

		CSize szButton( 0, 0 );

		INT nMaxButtonWidth = OnQueryMaxButtonWidth( bHorz ? TRUE : FALSE );
		CRect rcBorderSizes = OnQueryBorderSizes( bHorz ? TRUE : FALSE );
		INT nIconAlignment = OnQueryAlignmentIcon( bHorz ? TRUE : FALSE );
		CRect rcIconMargins = OnQueryMarginsIcon( bHorz ? TRUE : FALSE );
		CRect rcTextMargins = OnQueryMarginsText( bHorz ? TRUE : FALSE );

		CExtCmdIcon * pIcon = GetIconPtr();
		CSize szIcon( 0, 0 );
		if( pIcon != NULL && (! pIcon->IsEmpty() ) )
		{
			szIcon = pIcon->GetSize();
			if( IsLargeIcon() )
			{
				szIcon.cx *= 2;
				szIcon.cy *= 2;
			}

			szIcon.cx += bHorz 
				? ( rcIconMargins.left + rcIconMargins.right )
				: ( rcIconMargins.top + rcIconMargins.bottom );
			szIcon.cy += bHorz 
				? ( rcIconMargins.top + rcIconMargins.bottom )
				: ( rcIconMargins.left + rcIconMargins.right );
		}

		CExtSafeString sTitle = GetText();
		if( ! sTitle.IsEmpty() )
		{
			CFont * pFont = pBar->OnGetToolbarFont( ! bHorz, true, this );
			if(	pFont->GetSafeHandle() == NULL )
				pFont = bBold 
					? &(pBar->PmBridge_GetPM()->m_FontBold)
					: &(pBar->PmBridge_GetPM()->m_FontNormal);
			ASSERT( pFont != NULL );
			CFont * pOldFont = dc.SelectObject( pFont );
			CRect rcText( 0, 0, 0, 0 );
			UINT nDtMeasureFlags =
				DT_LEFT | DT_TOP | DT_CALCRECT;
			if( nMaxButtonWidth >= 0 )
			{
				rcText.right = nMaxButtonWidth;
				nDtMeasureFlags |= DT_WORDBREAK;
			}
			else
				nDtMeasureFlags |= DT_SINGLELINE;

			CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
			if( ! bHorz )
				eLO = CExtRichContentLayout::__ELOT_90_CW;
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, LPCTSTR(sTitle), rcText, nDtMeasureFlags, 0
				);
			if( bHorz )
			{
				rcText.InflateRect( rcTextMargins );
				if( ( nIconAlignment & CExtPaintManager::__ALIGN_HORIZ_MASK ) == CExtPaintManager::__ALIGN_HORIZ_CENTER )
				{
					if( ( nIconAlignment & CExtPaintManager::__ALIGN_VERT_MASK ) == CExtPaintManager::__ALIGN_VERT_CENTER )
					{
						szButton.cx = 0;
						szButton.cy = 0;
					}
					else
					{
						szButton.cx = max( rcText.Width(), szIcon.cx );
						szButton.cy = rcText.Height() + szIcon.cy;
					}
				}
				else
				{
					szButton.cx = rcText.Width() + szIcon.cx;
					szButton.cy = max( rcText.Height(), szIcon.cy );
				}
				if( pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
					szButton.cx += 10;
			}
			else
			{
				rcText.InflateRect(
					rcTextMargins.top,
					rcTextMargins.right,
					rcTextMargins.bottom,
					rcTextMargins.left
					);

				if( ( nIconAlignment & CExtPaintManager::__ALIGN_HORIZ_MASK ) == CExtPaintManager::__ALIGN_HORIZ_CENTER )
				{
					if( ( nIconAlignment & CExtPaintManager::__ALIGN_VERT_MASK ) == CExtPaintManager::__ALIGN_VERT_CENTER )
					{
						szButton.cx = 0;
						szButton.cy = 0;
					}
					else
					{
						szButton.cx = rcText.Width() + szIcon.cx;
						szButton.cy = max( rcText.Height(), szIcon.cy );
					}
				}
				else
				{
 					szButton.cx = max( rcText.Width(), szIcon.cx );
 					szButton.cy = rcText.Height() + szIcon.cy;
				}
			
				if( pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
					szButton.cy += 10;
			}
			
			dc.SelectObject( pOldFont );
			
		} // if( ! sTitle.IsEmpty() )
		else
			szButton = szIcon;

		szButton.cx += bHorz 
			? ( rcBorderSizes.left + rcBorderSizes.right )
			: ( rcBorderSizes.top + rcBorderSizes.bottom );
		szButton.cy += bHorz 
			? ( rcBorderSizes.top + rcBorderSizes.bottom )
			: ( rcBorderSizes.left + rcBorderSizes.right );

		if( m_ActiveSize.cx < szButton.cx )
			m_ActiveSize.cx = szButton.cx;
		if( m_ActiveSize.cy < szButton.cy )
			m_ActiveSize.cy = szButton.cy;

		pBar->PmBridge_GetPM()->Toolbar_AdjustButtonSize( this, m_ActiveSize );

	} // else from if( IsSeparator() )

	if(		IsAbleToTrackMenu()
		&&	( ! pBar->_IsSimplifiedDropDownButtons() )
		)
	{
		if( bHorz )
			m_ActiveSize.cx += _CalcSplitButtonExtent( true );
		else
			m_ActiveSize.cy += _CalcSplitButtonExtent( false );
	}

CSize _size = pBar->PmBridge_GetPM()->Toolbar_GetMinButtonSize( this );
	if( bHorz )
	{
		m_ActiveSize.cx += _size.cx;
		if( m_ActiveSize.cy < _size.cy )
			m_ActiveSize.cy = _size.cy;
	}
	else
	{
		if( m_ActiveSize.cx < _size.cx )
			m_ActiveSize.cx = _size.cx;
		m_ActiveSize.cy += _size.cy;
	}

	return m_ActiveSize;
}

CExtCmdIcon * CExtBarButton::GetIconPtr()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
CExtToolControlBar * pBar = GetBar();
	if( pBar->GetSafeHwnd() == NULL )
		return NULL;
__EXT_MFC_SAFE_LPCTSTR strProfile = g_CmdManager->ProfileNameFromWnd( pBar->GetSafeHwnd() );
	if( strProfile == NULL )
		return NULL;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtCustomizeCmdTreeNode * pOwnNode = GetCmdNode();
	if( pOwnNode != NULL )
	{
		ASSERT_VALID( pOwnNode );
		if( !CExtCmdManager::IsCommand( GetCmdID( true ) ) )
			return NULL;
		CExtCmdIcon * pIcon =
			pOwnNode->GetIconPtrInToolbar(
				g_CmdManager->CmdGetPtr(
					strProfile,
					GetCmdID( true )
					)
				);
		if( pIcon == NULL )
			return NULL;
		if( (pOwnNode->GetFlags()&__ECTN_TBB_AUTOCHANGE_ID) == 0 )
			return pIcon;
	} // if( pOwnNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
UINT nCmdID = GetCmdID( true );
	if( ! CExtCmdManager::IsCommand( nCmdID ) )
		return NULL;
	return
		g_CmdManager->CmdGetIconPtr(
			strProfile,
			nCmdID
			);
}

const CExtCmdIcon * CExtBarButton::GetIconPtr() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtBarButton * > ( this ) )
		-> GetIconPtr();
}

CExtCmdIcon & CExtBarButton::GetIcon()
{
	ASSERT_VALID( this );
CExtCmdIcon * pIcon = GetIconPtr();
	if( pIcon != NULL )
		return (*pIcon);
static CExtCmdIcon g_EmptyIcon;
	return g_EmptyIcon;
}


const CExtCmdIcon & CExtBarButton::GetIcon() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtBarButton * > ( this ) )
		-> GetIcon();
}

const CExtAnimationParameters *
	CExtBarButton::AnimationClient_OnQueryAnimationParameters(
		INT eAPT // __EAPT_*** animation type
		) const
{
	ASSERT_VALID( this );
const CExtAnimationParameters * pAnimationParameters =
		g_PaintManager->Animation_GetParameters(
			eAPT,
			(CObject*)this,
			this
			);
	return pAnimationParameters;
}

bool CExtBarButton::AnimationClient_CacheNextState(
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
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( AnimationClient_NonClientModeGet() )
		pBar->DoPaintNC( &dc );
	else
		pBar->DoEraseBk( &dc );
	PaintCompound( dc, true, false, false );
	return
		CExtAnimationClient::AnimationClient_CacheNextState(
			dc,
			rcAcAnimationTarget,
			bAnimate,
			eAPT
			);
}

bool CExtBarButton::AnimationClient_StatePaint(
	CDC & dc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( GetBar() );
	if( AnimationClient_SiteGet() == NULL )
		AnimationClient_SiteSet( GetBar() );
bool bRetVal = CExtAnimationClient::AnimationClient_StatePaint( dc );
	PaintChildren( dc, true );
	return bRetVal;
}

void CExtBarButton::AnimationClient_StateDoStep()
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->m_pDockBar == NULL )
	{
		CExtRibbonBar * pRibbonBar = DYNAMIC_DOWNCAST( CExtRibbonBar, pBar );
		if(		pRibbonBar != NULL
			&&	pRibbonBar->m_pExtNcFrameImpl != NULL
			&&	pRibbonBar->m_pExtNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement()
			)
		{
			CWnd * pWndParent = pBar->GetParent();
			if( pWndParent != NULL )
			{
				CRect _rectTest = *this;
				pBar->ClientToScreen( &_rectTest );
				pWndParent->ScreenToClient( &_rectTest );
				if(		_rectTest.bottom <= 0
					||	(	_rectTest.top <= 0
						&&	IsKindOf( RUNTIME_CLASS(CExtRibbonButtonFile) )
						)
					)
				{
					pBar->_InvalidateButton( pBar->_GetIndexOf( this ) );
					return;
				}
			} // if( pWndParent != NULL )
		} // if( m_pDockBar == NULL ...
	} // if( pBar->m_pDockBar == NULL )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	CExtAnimationClient::AnimationClient_StateDoStep();
}

bool CExtBarButton::IsHorzBarOrientation() const
{
	ASSERT_VALID( this );
const CExtToolControlBar * pBar = GetBar();
	if( pBar == NULL )
		return true;
	ASSERT_VALID( pBar );
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( pBar->IsKindOf( RUNTIME_CLASS(CExtRibbonPage) ) )
		return true;
#endif
DWORD dwBarStyle =
		( const_cast < CExtToolControlBar * > ( pBar ) )
		-> GetBarStyle();
	if( ( dwBarStyle & CBRS_ORIENT_HORZ ) != 0 )
		return true;
	if( pBar->m_bPaletteMode )
		return true;
	if( GetNoRotateVerticalLayout() )
		return true;
	return false;
}

bool CExtBarButton::IsPaintAble( CDC & dc ) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! IsVisible() )
		return false;
	if( ( GetStyle() & TBBS_HIDDEN ) != 0 )
		return false;
CRect rcTBB = *this;
	if( ! dc.RectVisible( &rcTBB ) )
		return false;
	return true;
}

void CExtBarButton::PaintParentChain(
	CDC & dc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtBarButton * pTBB = ParentButtonGet();
	if( pTBB == NULL )
		return;
CTypedPtrList < CPtrList, CExtBarButton * > _list;
	for( ; pTBB != NULL; pTBB = pTBB->ParentButtonGet() )
	{
		ASSERT_VALID( pTBB );
		_list.AddHead( pTBB );
	}
POSITION pos = _list.GetHeadPosition();
	for( ; pos != NULL; )
	{
		pTBB = _list.GetNext( pos );
		ASSERT_VALID( pTBB );
		pTBB->PaintCompound( dc, false, false, false );
	}
}

void CExtBarButton::PaintChildren(
	CDC & dc,
	bool bPaintOneNearestChildrenLevelOnly
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
INT nChildIndex, nChildCount = ChildButtonGetCount();
	for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
	{
		CExtBarButton * pTBB = ChildButtonGetAt( nChildIndex );
		ASSERT_VALID( pTBB );
		if( ! pTBB->IsVisible() )
			continue;
		if( ( pTBB->GetStyle() & TBBS_HIDDEN ) != 0 )
			continue;
		pTBB->PaintCompound( dc, false, ! bPaintOneNearestChildrenLevelOnly, false );
	} // for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
}

void CExtBarButton::Paint( // old version version (left for compatibility)
	CDC & dc,
	bool bHorz // unused, same as IsHorzBarOrientation()
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	bHorz;
	Paint( dc );
}

void CExtBarButton::Paint( // new simplified version (for simple toolbars)
	CDC & dc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	PaintCompound( dc, false, false, false );
}

void CExtBarButton::PaintCompound( // new fully featured version (for compound toolbars and rendering animation parts)
	CDC & dc,
	bool bPaintParentChain,
	bool bPaintChildren,
	bool bPaintOneNearestChildrenLevelOnly
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );

	if( ! IsPaintAble( dc ) )
		return;
	if( AnimationClient_StatePaint( dc ) )
		return;
	if( bPaintParentChain )
		PaintParentChain( dc );

CWnd * pCtrl = CtrlGet();
	if(		( pCtrl != NULL )
		&&	(	( ! m_bVertDocked )
			||	GetCtrlVisibleVertically()
			)
		)
		return;

//	ASSERT( !IsSeparator() );
CRect rcArea( m_ActiveRect );
	if(		rcArea.right <= rcArea.left
		||	rcArea.bottom <= rcArea.top
		)
		return;
	if(		(! IsVisible() )
		||	(GetStyle() & TBBS_HIDDEN) != 0
		||	(! dc.RectVisible(&m_ActiveRect) )
		)
		return;
bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
bool bPushed =
		( IsPressed() && (!bDockSiteCustomizeMode) )
			? true : false;
bool bEnabled =
		( IsDisabled() && (!bDockSiteCustomizeMode) )
			? false : true;
bool bHover =
		( IsHover() && (!bDockSiteCustomizeMode) )
			? true : false;
	if(		(! bDockSiteCustomizeMode )
		&&	(! GetBar()->IsForceHoverWhenMenuTracking() )
		)
	{
		if(		CExtToolControlBar::g_bMenuTracking
			||	CExtPopupMenuWnd::IsMenuTracking()
			)
			bHover = false;
		else if( !bHover )
			bHover = IsPressedTracking();
		//if( bPushed && bHover && IsPressedTracking() )
			//bHover = false;
	} // if( ! bDockSiteCustomizeMode ...

bool bIndeterminate =
		( IsIndeterminate() && (!bDockSiteCustomizeMode) )
			? true : false;
CExtSafeString sText = GetText();

CExtCmdIcon * pIcon = GetIconPtr();
CExtCmdIcon * pIconLarge = NULL;
	if(		pIcon != NULL
		&&	(! pIcon->IsEmpty() )
		&&	IsLargeIcon()
		)
	{
		CSize _sizeIcon = pIcon->GetSize();
		_sizeIcon.cx *= 2;
		_sizeIcon.cy *= 2;
		pIconLarge = new CExtCmdIcon;
		if(	pIconLarge->CreateScaledCopy(
				*pIcon,
				_sizeIcon
				)
			)
			pIcon = pIconLarge;
#ifdef _DEBUG
		else
		{
			ASSERT( FALSE );
		}
#endif // _DEBUG
	}
bool bPaintAsDropDown = IsPaintDropDown();
bool bHorz = IsHorzBarOrientation();
HFONT hFont =
	(HFONT) pBar
		-> OnGetToolbarFont( ! bHorz, false, this )
		-> GetSafeHandle();
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		this,
		bHorz,
		rcArea,
		sText,
		pIcon,
		true,
		bHover,
		bPushed,
		bIndeterminate,
		bEnabled,
		m_bDrawBorder,
		false,
		false,
		0,
		hFont,
		bPaintAsDropDown,
		0,
		(!bEnabled) || ( bEnabled && (!bHover) && (!bPushed) )
		);

	_ppbd.m_rcBorderSizes = OnQueryBorderSizes( bHorz );
	_ppbd.m_nIconAlignment = OnQueryAlignmentIcon( bHorz );
	_ppbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
	_ppbd.m_rcIconMargins = OnQueryMarginsIcon( bHorz );
	_ppbd.m_rcTextMargins = OnQueryMarginsText( bHorz );

	if( OnQueryMaxButtonWidth( bHorz ) >= 0 )
		_ppbd.m_bWordBreak = true;	

	if( GetSeparatedDropDown() )
	{
		_ppbd.m_bSeparatedDropDown = true;
		_ppbd.m_bHoverDropDown = m_bDropDownHT;
		if(		( m_bDropDownHT /*|| CExtToolControlBar::g_bMenuTracking*/ )
			&&	bPushed
			&&	( ! bDockSiteCustomizeMode )
			)
		{
			//_ppbd.m_bPushed = false;
			_ppbd.m_bPushedDropDown = true;
		}
	}

	_ppbd.m_clrCustomAccentEffectForIcon =
		OnQueryCustomAccentEffectForIcon( dc );

bool bChecked =
		( (GetStyle()&TBBS_CHECKED) != 0 ) 
			? true 
			: false;
	_ppbd.m_bChecked = bChecked;

	if(		CExtToolControlBar::g_bMenuTracking
		&&	pBar->_GetIndexOf(this) ==	pBar->m_nBtnIdxMenuTracking
		)
	{
		CExtPopupMenuWnd * pPopup = CExtPopupMenuWnd::GetTrackingMenu();
		if(		pPopup->GetSafeHwnd() != NULL
			&&	pPopup->m_eCombineAlign != CExtPopupMenuWnd::__CMBA_NONE
			&&	( pPopup->GetStyle() & WS_VISIBLE ) != 0
			)
		{
			_ppbd.m_bHover = _ppbd.m_bPushed
					= _ppbd.m_bHoverDropDown = _ppbd.m_bPushedDropDown
					= _ppbd.m_bChecked
					= _ppbd.m_bDrawBorder
					= false;
			_ppbd.m_bTransparentBackground = true;
		}
	}

	pBar->PmBridge_GetPM()->PaintPushButton( dc, _ppbd );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		&&	pSite->CustomizedNodeGet() != NULL
		&&	pSite->CustomizedNodeGet() == GetCmdNode( false )
		)
	{
		pBar->PmBridge_GetPM()->PaintDragSourceRect(
			dc,
			rcArea
			);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( pIconLarge != NULL )
		delete pIconLarge;

	if( bPaintChildren )
		PaintChildren( dc, bPaintOneNearestChildrenLevelOnly );
}

CRect CExtBarButton::OnQueryBorderSizes(
	bool bHorz // = true
	) const
{
	ASSERT_VALID( this );
	bHorz;
	return CRect( 1, 1, 1, 1 );
}

INT CExtBarButton::OnQueryAlignmentText(
	bool bHorz // = true
	) const
{
	ASSERT_VALID( this );
	bHorz;
	return 
		m_nTextAlignment >= 0 
			? m_nTextAlignment
			: CExtPaintManager::__ALIGN_HORIZ_CENTER | CExtPaintManager::__ALIGN_VERT_CENTER;
}

INT CExtBarButton::OnQueryAlignmentIcon(
	bool bHorz // = true
	) const
{
	ASSERT_VALID( this );
	bHorz;
CExtSafeString sText = GetText();
	return 
		m_nIconAlignment >= 0 
			? m_nIconAlignment
			: sText.IsEmpty() 
				? CExtPaintManager::__ALIGN_HORIZ_CENTER | CExtPaintManager::__ALIGN_VERT_CENTER
				: CExtPaintManager::__ALIGN_HORIZ_LEFT | CExtPaintManager::__ALIGN_VERT_CENTER;
}

CRect CExtBarButton::OnQueryMarginsText(
	bool bHorz // = true
	) const
{
	ASSERT_VALID( this );
	bHorz;
	if(		m_rcTextMargins.left >= 0
		&&	m_rcTextMargins.top >= 0
		&&	m_rcTextMargins.right >= 0
		&&	m_rcTextMargins.bottom >= 0
		)
		return m_rcTextMargins;
	return CRect( 2, 2, 2, 2 );
}

CRect CExtBarButton::OnQueryMarginsIcon(
	bool bHorz // = true
	) const
{
	ASSERT_VALID( this );
	bHorz;
	if(		m_rcIconMargins.left >= 0
		&&	m_rcIconMargins.top >= 0
		&&	m_rcIconMargins.right >= 0
		&&	m_rcIconMargins.bottom >= 0
		)
		return m_rcIconMargins;
	return CRect( 2, 2, 2, 2 );
}

INT CExtBarButton::OnQueryMaxButtonWidth(
	bool bHorz // = true
	) const
{
	ASSERT_VALID( this );
	bHorz;
	return m_nMaxButtonWidth;
}

void CExtBarButton::SetMaxButtonWidth( 
	INT nWidth 
	)
{
	ASSERT_VALID( this );
	m_nMaxButtonWidth = nWidth;
}

INT CExtBarButton::GetMaxButtonWidth() const
{
	ASSERT_VALID( this );
	return m_nMaxButtonWidth;
}

void CExtBarButton::SetTextAlignment( 
	INT nAlignment 
	)
{
	ASSERT_VALID( this );
	m_nTextAlignment = nAlignment;
}

INT CExtBarButton::GetTextAlignment() const
{
	ASSERT_VALID( this );
	return m_nTextAlignment;
}

void CExtBarButton::SetIconAlignment( 
	INT nAlignment 
	)
{
	ASSERT_VALID( this );
	m_nIconAlignment = nAlignment;
}

INT CExtBarButton::GetIconAlignment() const
{
	ASSERT_VALID( this );
	return OnQueryAlignmentIcon();
}

void CExtBarButton::SetTextMargins(
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
}

void CExtBarButton::SetTextMargins(
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

void CExtBarButton::SetTextMargins(
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

void CExtBarButton::SetIconMargins(
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
	ASSERT_VALID( this );
}

void CExtBarButton::SetIconMargins(
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

void CExtBarButton::SetIconMargins(
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

void CExtBarButton::GetTextMargins(
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

CRect CExtBarButton::GetTextMargins() const
{
	ASSERT_VALID( this );
	return m_rcTextMargins;
}

void CExtBarButton::GetIconMargins(
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

CRect CExtBarButton::GetIconMargins() const
{
	ASSERT_VALID( this );
	return m_rcIconMargins;
}

bool CExtBarButton::GetDrawBorder() const
{
	ASSERT_VALID( this );
	return m_bDrawBorder;
}

void CExtBarButton::SetDrawBorder( 
	bool bEnable // = true 
	)
{
	ASSERT_VALID( this );
	m_bDrawBorder = bEnable;
}

COLORREF CExtBarButton::OnQueryCustomAccentEffectForIcon(
	CDC & dc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtToolControlBar * pBar = GetBar();
	return pBar->OnQueryCustomAccentEffectForIcon( dc, this );
}

void CExtBarButton::OnUpdateCmdUI(
	CWnd * pTarget,
	BOOL bDisableIfNoHndler,
	int nIndex
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pTarget == NULL )
		pTarget = pBar->GetOwner();
	ASSERT_VALID( pTarget );
	if(		IsSeparator() // ignore separators
		||	(	IsAbleToTrackMenu() // ignore menu drop buttons
			//&&(pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)))
			&&	( pBar->_IsSimplifiedDropDownButtons() )
			)
		)
		return;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
	if( pSite != NULL )
	{
		CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
		if(		pNode != NULL
			//&&	(pNode->GetFlags() & __ECTN_AUTO_POPUP) != 0
			&&	(pNode->GetFlags() & __ECTN_DYNAMIC_POPUP) != 0
			)
			return;
		if( pSite->IsUserBarCommand( GetCmdID( false ) ) )
			return;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	// send the update notification
CExtToolControlBar::CExtToolControlBarCmdUI state;
	state.m_pOther = GetBar();
	state.m_nIndex = nIndex;
	state.m_nIndexMax = pBar->GetButtonsCount();
	state.m_nID = GetCmdID( true );
	state.DoUpdate( pTarget, bDisableIfNoHndler );
}

bool CExtBarButton::CanStartCommandDrag()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	if( IsSeparator() )
		return false;
CWnd * pCtrl = CtrlGet();
	if( pCtrl->GetSafeHwnd() != NULL )
		return false;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( GetCmdNode() == NULL )
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return true;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtBarButton::_UpdateCmdNodeFromStyle()
{
	ASSERT_VALID( this );
	if( GetBar() == NULL )
		return;
	ASSERT_VALID( GetBar() );
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode == NULL )
		return;
	ASSERT_VALID( pNode );
	if( GetStyle() & TBBS_HIDDEN )
		pNode->ModifyFlags( __ECTN_TBB_HIDDEN, 0 );
	else
		pNode->ModifyFlags( 0, __ECTN_TBB_HIDDEN );
}

bool CExtBarButton::IsCustomizeActiveItem()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
CExtToolControlBar * pBar = GetBar();
CExtCustomizeSite * pSite =
		pBar->GetCustomizeSite();
	if( pSite == NULL )
		return false;
	return pSite->IsActiveItem( this );
}

void CExtBarButton::SetCustomizeActiveItem()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	if( IsSeparator() )
		return;
CExtToolControlBar * pBar = GetBar();
CExtCustomizeSite * pSite =
		pBar->GetCustomizeSite();
	if( pSite == NULL )
		return;
	pSite->ActiveItemSet( this, pBar->GetSafeHwnd() );
	pBar->_GetIndexOf( this );
	RedrawButton();
}

int CExtBarButton::CalculateDropOverEffect(
	CPoint point,
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	bool bAllowTrackNodePopup // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( !_dragInfo.IsEmpty() );
	_dragInfo;
	if( IsSeparator() )
		return 0;
	if( !CanStartCommandDrag() )
		return 0;
	if( GetCmdNode() == NULL )
		return false;
//CPoint ptMid = Rect().CenterPoint();
CPoint ptMidL = Rect().TopLeft();
	ptMidL.x += Rect().Size().cx / 4;
	ptMidL.y += Rect().Size().cy / 4;
CPoint ptMidR( ptMidL );
	ptMidR.x += Rect().Size().cx / 2;
	ptMidR.y += Rect().Size().cy / 2;
int nRetVal = 0;
CExtToolControlBar * pBar = GetBar();
	if( (pBar->m_dwStyle & CBRS_ORIENT_HORZ) != 0 )
	{
		if( point.x <= ptMidL.x  )
			nRetVal = -1;
		else if( point.x >= ptMidR.x  )
			nRetVal = 1;
		else
			nRetVal = -2;
	} // if( (pBar->m_dwStyle & CBRS_ORIENT_HORZ) != 0 )
	else
	{
		if( point.y <= ptMidL.y  )
			nRetVal = -1;
		else if( point.y >= ptMidR.y  )
			nRetVal = 1;
		else
			nRetVal = -2;
	} // else from if( (pBar->m_dwStyle & CBRS_ORIENT_HORZ) != 0 )

	if( bAllowTrackNodePopup )
	{
		CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
		ASSERT( pSite != NULL );
		if( IsAbleToTrackMenu( true ) )
		{
			CExtCustomizeCmdTreeNode * pOwnNode = GetCmdNode();
			ASSERT_VALID( pOwnNode );
			CRect rcBtn = Rect();
			pBar->ClientToScreen( &rcBtn );
			pBar->ClientToScreen( &point );
			DWORD dwTrackFlags = OnGetTrackPopupFlags();
			pSite->DropTargetPopupTrack(
				pOwnNode,
				rcBtn,
				point,
				dwTrackFlags
				);
		} // if( IsAbleToTrackMenu() )
		else
			pSite->DropTargetPopupCancel();
	} // if( bAllowTrackNodePopup )
	else
	{
		CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
		if( pSite != NULL )
			pSite->DropTargetPopupCancel();
	} // else from if( bAllowTrackNodePopup )

	return nRetVal;
}

CExtCustomizeCmdTreeNode * CExtBarButton::GetCmdNode(
	bool bInitial // = false
	)
{
	ASSERT_VALID( this );
//	ASSERT_VALID( GetBar() );
//	if( IsSeparator() )
//	{
//		ASSERT( m_pCmdNodeI == NULL );
//		ASSERT( m_pCmdNodeC == NULL );
//		return NULL;
//	}
CExtCustomizeCmdTreeNode * pNode =
		bInitial ? m_pCmdNodeI : m_pCmdNodeC;
#ifdef _DEBUG
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
	}
#endif // _DEBUG
	return pNode;
}

const CExtCustomizeCmdTreeNode * CExtBarButton::GetCmdNode(
	bool bInitial // = false
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtBarButton * > ( this ) )
		-> GetCmdNode( bInitial );
}

void CExtBarButton::OnCustomizeUpdateProps(
	CExtCustomizeCmdTreeNode * pNode // = NULL // update from basic node
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	if( pNode == NULL )
		pNode = m_pCmdNodeI;
	ASSERT_VALID( pNode );
#ifdef _DEBUG
	if( m_pCmdNodeI != NULL )
	{
		ASSERT_VALID( m_pCmdNodeI );
	}
#endif // _DEBUG
//	ASSERT( !IsSeparator() );
	SetCmdID( pNode->GetCmdID(false), false ); // reset all IDs
	SetCmdID( pNode->GetCmdID(true), true ); // reset effective ID
DWORD dwNodeFlags = pNode->GetFlags();
	if( dwNodeFlags&__ECTN_TBB_APPEND_MDI_MENU )
		SetAppendMdiWindowsMenu( true );
	if( dwNodeFlags&__ECTN_TBB_SEPARATED_DROPDOWN )
		SetSeparatedDropDown( true );
	if( dwNodeFlags&__ECTN_TBB_AUTOCHANGE_ID )
		SetAutoChangeID( true );
	if( dwNodeFlags&__ECTN_TBB_NO_ROTATE_VL )
		SetNoRotateVerticalLayout( true );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( GetAutoChangeID() && m_pCmdNodeC != NULL )
	{
		ASSERT_VALID( m_pCmdNodeC );
		CExtToolControlBar * pBar = GetBar();
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd(
					pBar->GetSafeHwnd()
					),
				GetCmdID( true )
				);
		LPCTSTR strText =
			( pCmdItem != NULL )
				? ( pCmdItem->m_sToolbarText.IsEmpty()
						? ( pCmdItem->m_sMenuText.IsEmpty()
								? _T("")
								: pCmdItem->m_sMenuText
							)
						: pCmdItem->m_sToolbarText
					)
				: _T("")
				;
		m_pCmdNodeC->SetTextInToolbar( strText );
		m_pCmdNodeC->SetTextInMenu( strText );
	} // if( GetAutoChangeID() && m_pCmdNodeC != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

void CExtBarButton::SetBasicCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT_VALID( pNode );
//	ASSERT( !IsSeparator() );
	m_pCmdNodeI = pNode;
//	ASSERT( m_pCmdNodeI->GetCmdID(false) == GetCmdID(false) );
}

void CExtBarButton::SetCustomizedCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT_VALID( pNode );
#ifdef _DEBUG
	if( m_pCmdNodeI != NULL )
	{
		ASSERT_VALID( m_pCmdNodeI );
	}
#endif // _DEBUG
//	ASSERT( !IsSeparator() );
	m_pCmdNodeC = pNode;
	ASSERT( m_pCmdNodeC->GetCmdID(false) == GetCmdID(false) );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( GetAutoChangeID() )
	{
		CExtToolControlBar * pBar = GetBar();
		HWND hWnd = pBar->GetSafeHwnd();
		if( hWnd != NULL )
		{
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr( g_CmdManager->ProfileNameFromWnd( hWnd ), GetCmdID( true ) );
			LPCTSTR strText =
				( pCmdItem != NULL )
					? ( pCmdItem->m_sToolbarText.IsEmpty()
							? ( pCmdItem->m_sMenuText.IsEmpty()
									? _T("")
									: pCmdItem->m_sMenuText
								)
							: pCmdItem->m_sToolbarText
						)
					: _T("")
					;
			m_pCmdNodeC->SetTextInToolbar( strText );
			m_pCmdNodeC->SetTextInMenu( strText );
		} // if( hWnd != NULL )
	} // if( GetAutoChangeID() )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

CExtCustomizeCmdTreeNode * CExtBarButton::OnCustomizeNodeInit(
	CExtCustomizeSite * pSite,
	CExtCustomizeCmdTreeNode * pParentNode // toolbar node
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( pSite != NULL );
	ASSERT_VALID( pParentNode );
	if( IsSeparator() )
	{
		ASSERT( m_pCmdNodeI == NULL );
		ASSERT( m_pCmdNodeC == NULL );
		return NULL;
	}
	m_pCmdNodeI = m_pCmdNodeC = NULL;
	m_pCmdNodeI =
		pSite->OnCmdNodeCreate(
			GetCmdID( false ),
			GetCmdID( true )
			);
	pParentNode->InsertNode( pSite, m_pCmdNodeI );
CExtToolControlBar * pBar = GetBar();
INT nOwnIdx = pBar->_GetIndexOf( this );
	ASSERT( 0 <= nOwnIdx && nOwnIdx < pBar->GetButtonsCount() );
	if( nOwnIdx > 0 )
	{
		CExtBarButton * pPrevTBB =
			pBar->GetButton( nOwnIdx - 1 );
		if( pPrevTBB->IsSeparator() )
			m_pCmdNodeI->ModifyFlags( __ECTN_GROUP_START );
	} // __ECTN_GROUP_START
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( pBar->m_hWnd ),
			GetCmdID()
			);
	ASSERT( pCmdItem != NULL );
	m_pCmdNodeI->SetTextInToolbar( pCmdItem->m_sToolbarText );
	m_pCmdNodeI->SetTextInMenu( pCmdItem->m_sMenuText );
DWORD dwAddFlags = 0;
	if( IsAppendMdiWindowsMenu() )
		dwAddFlags |= __ECTN_TBB_APPEND_MDI_MENU;
	if( GetSeparatedDropDown() || pCmdItem->StateIsSeparatedDD() )
		dwAddFlags |= __ECTN_TBB_SEPARATED_DROPDOWN;
	if( GetAutoChangeID() || pCmdItem->StateIsAutochangeID() )
		dwAddFlags |= __ECTN_TBB_AUTOCHANGE_ID;
	if( GetNoRotateVerticalLayout() || pCmdItem->StateIsNoRotateVL() )
		dwAddFlags |= __ECTN_TBB_NO_ROTATE_VL;
	if( GetCtrlVisibleVertically() || pCmdItem->StateIsCtrlVisibleVertically() )
		dwAddFlags |= __ECTN_TBB_CTRL_VIS_VL;

	if( pCmdItem->StateIsCannotHide() )
		dwAddFlags |= __ECTN_TBB_CANNOT_HIDE;

	if( dwAddFlags != 0 )
		m_pCmdNodeI->ModifyFlags( dwAddFlags );

	m_pCmdNodeI->SetLParam( pCmdItem->m_nLParamUserData );

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	m_pCmdNodeI->TextFieldWidthSet( pCmdItem->m_nTextFieldWidth );
	m_pCmdNodeI->DropDownWidthSet( pCmdItem->m_nDropDownWidth );
	m_pCmdNodeI->DropDownHeightMaxSet( pCmdItem->m_nDropDownHeightMax );
	if( pCmdItem->StateIsCombo() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_COMBO|__ECTN_TBB_TEXT_FIELD, __ECTN_TBB_DATE );
	else if( pCmdItem->StateIsDate() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_DATE|__ECTN_TBB_TEXT_FIELD, __ECTN_TBB_COMBO );
	else if( pCmdItem->StateIsTextField() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_TEXT_FIELD, __ECTN_TBB_COMBO|__ECTN_TBB_DATE );
	else
		m_pCmdNodeI->ModifyFlags( 0, __ECTN_TBB_TEXT_FIELD|__ECTN_TBB_COMBO|__ECTN_TBB_DATE|__ECTN_TBB_RESIZABLE );

	if( pCmdItem->StateIsTextFieldNotEditable() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_TF_NE, 0 );
	else
		m_pCmdNodeI->ModifyFlags( 0, __ECTN_TBB_TF_NE );

	if(		( (m_pCmdNodeI->GetFlags() & (__ECTN_TBB_COMBO|__ECTN_TBB_DATE|__ECTN_TBB_TEXT_FIELD)) != 0 )
		&&	pCmdItem->StateIsResizable()
		)
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_RESIZABLE, 0 );
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

	if( pCmdItem->StateIsCheckBox() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_CHECK_BOX, 0 );
	else
		m_pCmdNodeI->ModifyFlags( 0, __ECTN_TBB_CHECK_BOX );

	if( pCmdItem->StateIsRadioBox() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_RADIO_BOX, 0 );
	else
		m_pCmdNodeI->ModifyFlags( 0, __ECTN_TBB_RADIO_BOX );

	if( pCmdItem->StateIsUndoRedo() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_UNDO_REDO, 0 );
	else
		m_pCmdNodeI->ModifyFlags( 0, __ECTN_TBB_UNDO_REDO );

	if( pCmdItem->StateIsColor() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_COLOR, 0 );
	else
		m_pCmdNodeI->ModifyFlags( 0, __ECTN_TBB_COLOR );
	
	if( pCmdItem->StateIsColorBtnDefault() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_MCB_DEFAULT, 0 );
	else
		m_pCmdNodeI->ModifyFlags( 0, __ECTN_TBB_MCB_DEFAULT );
	
	if( pCmdItem->StateIsColorBtnCustom() )
		m_pCmdNodeI->ModifyFlags( __ECTN_TBB_MCB_CUSTOM, 0 );
	else
		m_pCmdNodeI->ModifyFlags( 0, __ECTN_TBB_MCB_CUSTOM );
	
	if( GetMenu() != NULL )
	{
		CMenu * pMenu = CMenu::FromHandle( GetMenu() );
		ASSERT( pMenu != NULL );
		VERIFY(
			m_pCmdNodeI->LoadMenuTree(
				GetBar(),
				pBar->GetCustomizeSite(),
				pMenu,
				IsContainerOfPopupLikeMenu()
				)
			);
		SetMenu( NULL, true, true ); // no longer needed
	} // if( GetMenu() != NULL )
	return m_pCmdNodeI;
}

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
int CExtBarButton::GetInitialResizingStateH( // -1 - left side resizing, 1 - right side resizing, 0 - no resizing at specified point (in bar's client coord)
	CPoint point,
	int * p_nWidthMin, // = NULL
	int * p_nWidthMax // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	point;
	p_nWidthMin;
	p_nWidthMax;
	return 0;
}
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtBarButton::RedrawButton(
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
CExtToolControlBar * pBar = GetBar();
	if(		IsVisible()
		&&	(GetStyle() & TBBS_HIDDEN) == 0
		)
		pBar->_InvalidateButton( pBar->_GetIndexOf(this) );
	if( bUpdateNow )
		pBar->UpdateWindow();
}

void CExtBarButton::SetMenu(
	HMENU hMenu,
	bool bPopupMenu,
	bool bAutoDestroyMenu
	)
{
	ASSERT_VALID( this );
	_DestroyMenu();
	m_hMenu = hMenu;
	if( m_hMenu != NULL )
	{
		ASSERT( ::IsMenu(hMenu) );
		CExtToolControlBar * pBar = GetBar();
		VERIFY(
			g_CmdManager->UpdateFromMenu(
				g_CmdManager->ProfileNameFromWnd( pBar->GetSafeHwnd() ),
				hMenu
				)
			);
		m_bPopupMenu = bPopupMenu ? true : false;
		ModifyStyle(0,TBBS_DISABLED);
		m_bAutoDestroyMenu = bAutoDestroyMenu;
	}
}

UINT CExtBarButton::OnGetTrackPopupFlags()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
DWORD dwTrackFlags = 0;
CExtToolControlBar * pBar = GetBar();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( pBar->KeyTipsDisplayedGet() )
		dwTrackFlags |= TPMX_RIBBON_KEYTIPS;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( pBar->m_bPresubclassDialogMode )
	{
		if( pBar->IsDockedAtRight() )
			dwTrackFlags |= TPMX_RIGHTALIGN;
		else if( pBar->IsDockedAtLeft() )
			dwTrackFlags |= TPMX_LEFTALIGN;
		else if( pBar->IsDockedAtBottom() )
			dwTrackFlags |= TPMX_BOTTOMALIGN;
		else
			dwTrackFlags |= TPMX_TOPALIGN;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
		CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			CExtRibbonNode * pRibbonNode =
				DYNAMIC_DOWNCAST( CExtRibbonNode, pNode );
			if( pRibbonNode != NULL )
				dwTrackFlags |= pRibbonNode->m_nTpmxAdditionalFlags;
		} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
		return dwTrackFlags;
	} // if( pBar->m_bPresubclassDialogMode )

	switch( pBar->GetSafeDockBarDlgCtrlID() )
	{
	case AFX_IDW_DOCKBAR_TOP:
		dwTrackFlags |= TPMX_TOPALIGN;
	break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		dwTrackFlags |= TPMX_BOTTOMALIGN;
	break;
	case AFX_IDW_DOCKBAR_LEFT:
		dwTrackFlags |= TPMX_LEFTALIGN;
	break;
	case AFX_IDW_DOCKBAR_RIGHT:
		dwTrackFlags |= TPMX_RIGHTALIGN;
	break;
	default: // floating
		dwTrackFlags |= TPMX_TOPALIGN;
	break;
	} // switch( pBar->GetSafeDockBarDlgCtrlID() )

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		CExtRibbonNode * pRibbonNode =
			DYNAMIC_DOWNCAST( CExtRibbonNode, pNode );
		if( pRibbonNode != NULL )
			dwTrackFlags |= pRibbonNode->m_nTpmxAdditionalFlags;
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	return dwTrackFlags;
}

bool CExtBarButton::OnSetCursor( CPoint point )
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	point;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
CExtToolControlBar * pBar = GetBar();
CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
	if( pSite == NULL )
		return false;
	if( pSite->ActiveItemGet() != ((LPVOID)this) )
		return false;
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode == NULL )
		return false;
	ASSERT_VALID( pNode );
	if( (pNode->GetFlags() & __ECTN_TBB_RESIZABLE) == 0 )
		return false;
int nInitialResizingStateH =
		GetInitialResizingStateH( point );
	if( nInitialResizingStateH != 0 )
	{
		CExtLocalResourceHelper _LRH;
		CWinApp * pApp = ::AfxGetApp();
		ASSERT_VALID( pApp );
		HCURSOR hCursor = pApp->LoadCursor( MAKEINTRESOURCE( IDC_EXT_RESIZE_H1 ) );
		if( hCursor == NULL )
		{
			ASSERT( FALSE );
			hCursor = ::LoadCursor( NULL, IDC_SIZEWE );
		}
		ASSERT( hCursor != NULL );
		::SetCursor( hCursor );
		return true;
	} // if( nInitialResizingStateH != 0 )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return false;
}

CWnd * CExtBarButton::GetCmdTargetWnd()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
CExtToolControlBar * pBar = GetBar();
CWnd * pWnd = pBar->GetOwner();
	ASSERT_VALID( pWnd );
	return pWnd;
}

bool CExtBarButton::OnQueryHoverBasedMenuTracking() const
{
	ASSERT_VALID( this );
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
bool bEnabled = pBar->OnQueryHoverBasedMenuTracking( this );
	return bEnabled;
}

CExtPopupMenuWnd * CExtBarButton::OnCreatePopupMenuWnd()
{
	ASSERT_VALID( this );
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			pBar->GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	ASSERT_VALID( pPopup );
	return pPopup;
}

UINT CExtBarButton::OnTrackPopup(
	CPoint point,
	bool bSelectAny,
	bool bForceNoAnimation
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
bool bDockSiteCustomizeMode = pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return UINT(-1L);
	if( IsDisabled() && (! CanBePressedInDisabledState() ) )
		return UINT(-1L);
CWnd * pWnd = CtrlGet();
	if( pWnd != NULL )
	{
		LRESULT lResult = pWnd->SendMessage( CExtToolControlBar::g_nMsgContinueTrackMenu, WPARAM( this ), bSelectAny ? 1L : 0L );
		lResult;
		return UINT(-1L);
	} // if( pWnd != NULL )
	if( ! IsAbleToTrackMenu() )
		return UINT(-1L);
bool bPrevTBMT = CExtToolControlBar::g_bMenuTracking;
	if( CExtToolControlBar::g_bMenuTracking && pBar->_GetIndexOf(this) == pBar->m_nBtnIdxMenuTracking )
		return UINT(-1L);
	if( GetSeparatedDropDown() )
		m_bDropDownHT = true;
	CExtToolControlBar::_CloseTrackingMenus();
	if( pBar->IsFloating() )
	{
		pBar->ActivateTopParent();
		CFrameWnd * pFrame = pBar->GetDockingFrame();
		ASSERT_VALID( pFrame );
		pFrame->BringWindowToTop();
	}
CWnd * pWndCmdTarget = GetCmdTargetWnd();
	ASSERT_VALID( pWndCmdTarget );
CExtPopupMenuWnd * pPopup = OnCreatePopupMenuWnd();
	ASSERT_VALID( pPopup );
DWORD dwTrackFlags = OnGetTrackPopupFlags() | TPMX_OWNERDRAW_FIXED;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) ) )
		dwTrackFlags |= TPMX_COMBINE_NONE|TPMX_RIBBON_MODE|TPMX_NO_HIDE_RARELY;
	else
		dwTrackFlags |= TPMX_COMBINE_DEFAULT;
#else // (!defined __EXT_MFC_NO_RIBBON_BAR)
		dwTrackFlags |= TPMX_COMBINE_DEFAULT;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( bForceNoAnimation )
		dwTrackFlags |= TPMX_FORCE_NO_ANIMATION;
	if( bSelectAny )
		dwTrackFlags |= TPMX_SELECT_ANY;
	pPopup->TrackFlagsSet( dwTrackFlags );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		VERIFY( pPopup->UpdateFromCmdTree( pWndCmdTarget->GetSafeHwnd(), pNode ) );
	} // if( pNode != NULL )
	else
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	{
		HMENU hMenu = GetMenu();
		ASSERT( hMenu != NULL );
		ASSERT( ::IsMenu(hMenu) );
		CMenu _menu;
		_menu.Attach( hMenu );
		VERIFY( pPopup->UpdateFromMenu( pWndCmdTarget->GetSafeHwnd(), &_menu, IsContainerOfPopupLikeMenu() ) );
		_menu.Detach();
	} // else from if( pNode != NULL )
	if( IsAppendMdiWindowsMenu() && (! pBar->_IsDockSiteCustomizeMode() ) )
		pPopup->UpdateMdiWindowsMenu( pWndCmdTarget );
	CExtToolControlBar::g_bMenuTracking = bPrevTBMT;
CRect rcBtn = Rect();
	pBar->ClientToScreen( &rcBtn );
	pBar->ClientToScreen( &point );
	if( CExtToolControlBar::g_bMenuTrackingExpanded )
		dwTrackFlags |= TPMX_NO_HIDE_RARELY;
	pBar->_SwitchMenuTrackingIndex( pBar->_GetIndexOf( this ) );
	if(		GetAutoChangeID()
		&&	(! CExtControlBar::FindHelpMode(GetBar()) )
		&&	(! pBar->GetParent()->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) ) )
		)
	{
		HWND hWndBar = pBar->GetSafeHwnd();
		CExtToolControlBar::g_bMenuTracking = true;
		UINT nResultCmdID = 0;
		pPopup->m_hWndNotifyMenuClosed = pBar->GetSafeHwnd();
		if( GetSeparatedDropDown() )
			m_bDropDownHT = true;
		g_pTrackingMenuTBB = this;
		if( ! pPopup->TrackPopupMenu(
				dwTrackFlags|TPMX_NO_WM_COMMAND|TPMX_DO_MESSAGE_LOOP, point.x, point.y, &rcBtn, GetBar(),
				CExtToolControlBar::_CbPaintCombinedContent, &nResultCmdID, true
				)
			)
		{
			g_pTrackingMenuTBB = NULL;
			//delete pPopup;
			if( ! ::IsWindow( hWndBar ) )
				return UINT(-1L);
			CExtToolControlBar::_CloseTrackingMenus();
			return UINT(-1L);
		}
		g_pTrackingMenuTBB = NULL;
		if( ! ::IsWindow( hWndBar ) )
			return UINT(-1L);
		if( pBar->_GetIndexOf( this ) < 0 )
			return UINT(-1L);
		ModifyStyle( 0, TBBS_PRESSED );
		pBar->Invalidate();
		if( nResultCmdID == 0 )
			return UINT(-1L);
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		SetCmdID( nResultCmdID, true, false );
		CExtCustomizeCmdTreeNode * pNodeC = GetCmdNode( false );
		if( pNodeC != NULL )
		{
			ASSERT_VALID( pNodeC );
			CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr( g_CmdManager->ProfileNameFromWnd( hWndBar ), nResultCmdID );
			LPCTSTR strText =
				( pCmdItem != NULL )
					? ( pCmdItem->m_sToolbarText.IsEmpty()
							? ( pCmdItem->m_sMenuText.IsEmpty() ? _T("") : pCmdItem->m_sMenuText )
							: pCmdItem->m_sToolbarText
						)
					: _T("")
					;
			pNodeC->SetTextInToolbar( strText );
			pNodeC->SetTextInMenu( strText );
		} // if( pNodeC != NULL )
//		pBar->Invalidate();
		CFrameWnd * pFrame = pBar->GetParentFrame();
		if( pFrame != NULL )
			pFrame->RecalcLayout();
		else
			pBar->_RecalcLayoutImpl();
#else // (!defined __EXT_MFC_NO_CUSTOMIZE)
		SetCmdID( nResultCmdID, true, true );
#endif // else from (!defined __EXT_MFC_NO_CUSTOMIZE)
		OnDeliverCmd();
		if( ::IsWindow(hWndBar) )
			pBar->_UpdateButton( pBar->_GetIndexOf(this) );
	} // if( GetAutoChangeID() )
	else
	{
		pPopup->m_hWndNotifyMenuClosed = pBar->GetSafeHwnd();
		if( GetSeparatedDropDown() )
			m_bDropDownHT = true;
		g_pTrackingMenuTBB = this;
		if( ! pPopup->TrackPopupMenu(
				dwTrackFlags, point.x, point.y, &rcBtn, GetBar(),
				CExtToolControlBar::_CbPaintCombinedContent, NULL, true
				)
			)
		{
			g_pTrackingMenuTBB = NULL;
			//delete pPopup;
			CExtToolControlBar::_CloseTrackingMenus();
			return UINT(-1L);
		}
		CExtToolControlBar::g_bMenuTracking = true;
		pBar->_SwitchMenuTrackingIndex( pBar->_GetIndexOf( this ) );
	} // else from if( GetAutoChangeID() )
	return UINT(-1L);
}

void CExtBarButton::OnHover(
	CPoint point,
	bool bOn,
	bool & bSuspendTips
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	bSuspendTips = false;

//	TRACE2(
//		" >> OnHover(%s,%s)\n",
//		bOn ? " on  " : " off ",
//		CExtToolControlBar::g_bMenuTracking ? " on  " : " off "
//		);

CExtToolControlBar * pBar = GetBar();

bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;
	if(		(	pBar->m_pDockSite == NULL
			||	(! pBar->m_pDockSite->m_bHelpMode )
			)
		&&	(! CExtPopupMenuWnd::IsMenuTracking() )
		)
	{
		CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON _icn(
			NULL,
			this,
			bOn
				? CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON::__EICN_SET
				: CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON::__EICN_CANCEL
			);
		HWND hWndOwn = pBar->GetSafeHwnd();
		_icn.Notify();
		bSuspendTips = _icn.m_bSuspendTips;
		if( ! ::IsWindow( hWndOwn ) )
			return;
	}

	if( bOn )
	{
		CExtBarButton * pTBB = this;
		for( ; pTBB != NULL; pTBB = pTBB->ParentButtonGet() )
		{
			pTBB->SetHover( true );
			pTBB->RedrawButton( false );
		}
		if( ! CExtPopupMenuWnd::IsMenuTracking() )
		{
			m_bDropDownHT = false;
			if( GetSeparatedDropDown() )
			{
				CRect rcBtnDropDownArea = RectDropDown();
				if( rcBtnDropDownArea.PtInRect(point) )
					m_bDropDownHT = true;
			}
		}

///		if( CExtToolControlBar::g_bMenuTracking
///			&& CExtPopupMenuWnd::IsMenuTracking()
///			&& pBar->m_nBtnIdxHover != pBar->m_nBtnIdxMenuTracking
///			)
///			CExtPopupMenuWnd::CancelMenuTracking();
		if( CExtToolControlBar::g_bMenuTracking )
		{
			if( ! OnQueryHoverBasedMenuTracking() )
				return;
			OnTrackPopup( point, false, false );
		}
		else
		{
			CExtControlBar::stat_SetMessageString(
				GetCmdTargetWnd(),
				(UINT)(
					(	//	( ! pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
							( ! pBar->_IsSimplifiedDropDownButtons() )
						&&	( ! IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) )
						&&	CExtCmdManager::IsCommand( GetCmdID(false) )
					)
						? GetCmdID(true)
						: AFX_IDS_IDLEMESSAGE
					)
				);
		}
		CWnd * pCtrl = CtrlGet();
		if(		pCtrl == NULL
			||	( pCtrl->GetStyle() & WS_VISIBLE ) == 0
			)
		{
			CExtMouseCaptureSink * pMCS = (CExtMouseCaptureSink *)GetBar();
			if( pMCS != NULL )
				pMCS->SetCapture( pBar->GetSafeHwnd() );
		}
	} // if( bOn )
	else
	{
		if( ! CExtPopupMenuWnd::IsMenuTracking() )
			m_bDropDownHT = false;
		CExtControlBar::stat_SetMessageString( GetCmdTargetWnd() );
		if(		::GetCapture() == pBar->m_hWnd
			&&	pBar->_GetHoverButtonIndex() < 0
			)
			CExtMouseCaptureSink::ReleaseCapture();
	} // else from if( bOn )
}

void CExtBarButton::OnHovering(
	CPoint point,
	bool & bSuspendTips
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	point;
	bSuspendTips;
}

CExtBarButton * CExtBarButton::g_pTrackingMenuTBB = NULL;

void CExtBarButton::OnClick(
	CPoint point,
	bool bDown
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );

CExtToolControlBar * pBar = GetBar();
	if( pBar->_OnHookButtonClick( this, point, bDown ) )
		return;

bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;

bool bDropDownHT = false;
	if( GetSeparatedDropDown() )
	{
		CRect rcBtnDropDownArea = RectDropDown();
		if( rcBtnDropDownArea.PtInRect(point) )
			bDropDownHT = true;
	}

	if(		IsDisabled()
		&&	(	(! CanBePressedInDisabledState() )
			||	(! bDropDownHT )
			)
		)
		return;

	if( bDown )
	{
		bool bThisButtonTrackingMenu = ( g_pTrackingMenuTBB == this && CExtPopupMenuWnd::IsMenuTracking() ) ? true : false;
		CExtToolControlBar::_CloseTrackingMenus();

		bool bAllowTrackMenu = true;
		m_bDropDownHT = false;
		if( GetSeparatedDropDown() )
		{
			m_bDropDownHT = bDropDownHT;
			if( ! bDropDownHT )
				bAllowTrackMenu = false;
		}

		if( bAllowTrackMenu && IsAbleToTrackMenu() )
		{
			if(		CExtToolControlBar::g_bMenuTracking
				&&	pBar->_GetIndexOf(this) ==	pBar->m_nBtnIdxMenuTracking
				)
				return;
			CExtToolControlBar::g_bMenuTrackingExpanded = false;
			if( ! bThisButtonTrackingMenu )
				OnTrackPopup( point, false, false );
			else
			{
				CExtPopupMenuWnd::CancelMenuTracking();
				CExtBarButton::g_pTrackingMenuTBB = NULL;
			}
			return;
		}

		CExtControlBar::stat_SetMessageString(
			GetCmdTargetWnd(),
			(UINT)GetCmdID(false)
			);

		return;
	} // if( bDown )

	CExtControlBar::stat_SetMessageString( GetCmdTargetWnd() );

	if( ! m_bDropDownHT )
		bDropDownHT = false;
	m_bDropDownHT = false;

	if(		(IsAbleToTrackMenu() && (!GetSeparatedDropDown()))
		||	bDropDownHT
		)
		return;

CWnd * pCtrl = CtrlGet();
	if(		pCtrl != NULL
		&&	( pCtrl->GetStyle() & WS_VISIBLE ) != 0
		)
	{
		ASSERT_VALID( pCtrl );
		CExtToolControlBar * pBar = GetSafeBar();
		ASSERT_VALID( pBar );
		if( CWnd::GetCapture() == pBar )
			pBar->SendMessage( WM_CANCELMODE );
		//pCtrl->SetFocus();
		return;
	}

	OnDeliverCmd();
}

bool CExtBarButton::OnRClick(
	CPoint point,
	bool bDown
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonRClick( this, point, bDown ) )
		return true;
	return false;
}

bool CExtBarButton::OnDblClick(
	CPoint point
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	point;
	if( g_pTrackingMenuTBB == this )
	{
		CExtPopupMenuWnd::CancelMenuTracking();
		CExtBarButton::g_pTrackingMenuTBB = NULL;
	}
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonDblClick( this, point ) )
		return true;
	return false;
}

void CExtBarButton::OnDeliverCmd()
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonDeliverCmd( this ) )
		return;
INT nAcIndex, nAcCount = pBar->AnimationSite_ClientGetCount();
	for( nAcIndex = 0; nAcIndex < nAcCount; nAcIndex ++ )
	{
		CExtAnimationClient * pAC = pBar->AnimationSite_ClientGetAt( nAcIndex );
		pBar->AnimationSite_ClientProgressStop( pAC );
	}
UINT nCmdID_E = GetCmdID(false);
UINT nCmdID_I = GetCmdID(true);
HWND hWndBar = pBar->GetSafeHwnd();
	if( pBar->OnDeliverCmd( this ) )
		return;
	if( ! CExtCmdManager::IsCommand( nCmdID_E ) )
		return;
	if( hWndBar == NULL || (! ::IsWindow(hWndBar) ) )
		return;
CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr( g_CmdManager->ProfileNameFromWnd( hWndBar ), nCmdID_I );
	if( pCmdItem == NULL )
		return;
	// the command may be undelivered (i.e. pCmdItem->Deliver() will
	// return "false") because there is no command handler found for it
	pCmdItem->Deliver( pBar, true );
}

__EXT_MFC_INT_PTR CExtBarButton::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	point; // should be inside this button

	if(	! IsDisplayScreenTip() )
		return -1;

CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	m_bDropDownHT = false;
	if( GetSeparatedDropDown() )
	{
		CRect rcBtnDropDownArea = RectDropDown();
		if( rcBtnDropDownArea.PtInRect(point) )
			m_bDropDownHT = true;
	}
	pBar->m_bLastDropDownHT = m_bDropDownHT;
int nCmdID = (int)GetCmdID(true);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( true );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		CExtCustomizeCmdScreenTip * pCCST = pNode->CmdScreenTipFindForDisplaying( ! m_bDropDownHT );
		if( pCCST != NULL )
		{
			ASSERT_VALID( pCCST );
			if( ! pCCST->IsEmpty() )
			{
				if( pTI != NULL )
				{
					CRect rcArea = m_bDropDownHT ? RectDropDown() : RectWithoutDropDown();
					::CopyRect(
						&(pTI->rect),
						&rcArea
						);
					pTI->uId = (UINT)nCmdID;
					pTI->hwnd = GetSafeBar()->GetSafeHwnd();
					//pTI->lpszText = LPSTR_TEXTCALLBACK;
					pTI->lpszText = _tcsdup( _T("ScreenTip") );
				} // if( pTI != NULL )
				return (__EXT_MFC_INT_PTR)nCmdID;
			} // if( ! pCCST->IsEmpty() )
		} // if( pCCST != NULL )
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( GetSafeBar()->GetSafeHwnd() ),
			nCmdID
			);
	if( pCmdItem == NULL )
		return -1;

	if( pCmdItem->m_sTipTool.IsEmpty() )
		return -1;
	if( pTI != NULL )
	{
		CRect rcArea = Rect();
		::CopyRect(
			&(pTI->rect),
			&rcArea
			);
		CExtSafeString strTip = pCmdItem->m_sTipTool;
		if(		IsDisplayShortcutKeysOnScreenTip()
			&&	( ! pCmdItem->m_sAccelText.IsEmpty() )
			)
		{
			if( ! strTip.IsEmpty() )
				strTip += _T(" ");
			strTip += _T("(");
			strTip += pCmdItem->m_sAccelText;
			strTip += _T(")");
		}
		pTI->uId = (UINT)nCmdID;
		pTI->hwnd = GetSafeBar()->GetSafeHwnd();
		pTI->lpszText = (LPTSTR)
			::calloc(
				(strTip.GetLength() + 1),
				sizeof(TCHAR)
				);
		if( pTI->lpszText != NULL )
			__EXT_MFC_STRCPY(
				pTI->lpszText,
				strTip.GetLength() + 1,
				strTip
				);
		else
			pTI->lpszText = LPSTR_TEXTCALLBACK;
	} // if( pTI != NULL )

	return (__EXT_MFC_INT_PTR)nCmdID;
}

LRESULT CExtBarButton::OnHelpHitTest(
	CPoint point
	)
{
	ASSERT_VALID( this );
	point; // should be inside this button

	if( IsSeparator() )
		return -1;

	if( IsAbleToTrackMenu() )
	{
		CExtToolControlBar * pBar = GetBar();
		CFrameWnd * pDockSiteOrOther = pBar->m_pDockSite;
		if(		pDockSiteOrOther == NULL
			||	(! pDockSiteOrOther->m_bHelpMode )
			)
		{
			pDockSiteOrOther = pBar->GetParentFrame();
			if(		pDockSiteOrOther != NULL
				&&	pDockSiteOrOther->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) )
				)
				pDockSiteOrOther = pDockSiteOrOther->GetParentFrame();
			if(		pDockSiteOrOther == NULL
				||	(! pDockSiteOrOther->m_bHelpMode )
				)
				return -3L;
		}
		OnTrackPopup( point, false, false );
		return CExtToolControlBar::_ContinueHelpHitTestMode( GetBar() );
	} // if( IsAbleToTrackMenu() )

int nCmdID = (int)GetCmdID(false);

CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( GetSafeBar()->GetSafeHwnd() ),
			nCmdID
			);
	if( pCmdItem == NULL
		//|| pCmdItem->m_sTipTool.IsEmpty()
		)
		return -1;

	return nCmdID;
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarCheckBoxButton

IMPLEMENT_DYNCREATE(CExtBarCheckBoxButton, CExtBarButton)
		
CExtBarCheckBoxButton::CExtBarCheckBoxButton(
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	)
	: CExtBarButton( pBar, nCmdID, nStyle )
{
//	m_bAcEnabled = false;
}

CExtBarCheckBoxButton::~CExtBarCheckBoxButton()
{
}

INT CExtBarCheckBoxButton::_GetBoxState() const
{
	ASSERT_VALID( this );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
bool bPressedTracking =
		IsPressedTracking();
bool bEnabled =
		( IsDisabled() && (!bDockSiteCustomizeMode) )
			? false : true;
bool bHover =
		( IsHover() && (!bDockSiteCustomizeMode) )
			? true : false;
bool bSelected = false;
CExtMenuControlBar * pMenuBar = 
		DYNAMIC_DOWNCAST( CExtMenuControlBar, pBar );
	if( pMenuBar != NULL )
	{
		INT nIndex = pMenuBar->_FlatTrackingIndexGet();
		if(		nIndex >= 0 
			&&	nIndex == pMenuBar->_GetIndexOf( const_cast < CExtBarCheckBoxButton * > ( this ) ) 
			)
			bSelected = true;
	}
	if( bSelected )
		bHover = true;
	if(		(! bDockSiteCustomizeMode )
		&&	(! GetBar()->IsForceHoverWhenMenuTracking() )
		)
	{
		if(		CExtToolControlBar::g_bMenuTracking
			||	CExtPopupMenuWnd::IsMenuTracking()
			)
			bHover = false;
		else if( !bHover )
			bHover = IsPressedTracking();
	} // if( ! bDockSiteCustomizeMode ...
bool bChecked =
		( (GetStyle()&TBBS_CHECKED) != 0 ) 
			? true 
			: false;
bool bIndeterminate =
		( IsIndeterminate() && (!bDockSiteCustomizeMode) )
			? true : false;

INT nCheck = 
		bIndeterminate 
			? BST_INDETERMINATE 
			: bChecked 
				? BST_CHECKED
				: BST_UNCHECKED;

CExtPaintManager::eBoxState_t eState = 
		CExtPaintManager::BOX_UNCHECKED;
	switch( nCheck ) 
	{
	case BST_UNCHECKED:
		if( bEnabled )
		{
			if( bHover && !bPressedTracking )
				eState = CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED;
			else if( bHover && bPressedTracking )
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
			if( bHover && !bPressedTracking )
				eState = CExtPaintManager::BOX_MOUSE_HOVER_CHECKED;
			else if( bHover && bPressedTracking )
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
			if( bHover && !bPressedTracking )
				eState = CExtPaintManager::BOX_MOUSE_HOVER_INDETERMINATE;
			else if( bHover && bPressedTracking )
				eState = CExtPaintManager::BOX_LDOWN_INDETERMINATE;
			else
				eState = CExtPaintManager::BOX_INDETERMINATE;
		}
		else
			eState = CExtPaintManager::BOX_DISABLED_INDETERMINATE;
		break;
	} // switch( nCheck ) 

	return eState;
}

CSize CExtBarCheckBoxButton::_GetBoxSize(
	CDC & dc
	) const
{
	ASSERT_VALID( this );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
bool bHorz = IsHorzBarOrientation();
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd;
	_pcrbd.m_pHelperSrc = (CObject*)pBar; 
	_pcrbd.m_eState = (CExtPaintManager::eBoxState_t) _GetBoxState(); 
	_pcrbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
	_pcrbd.m_rcTextMargins = OnQueryMarginsText( bHorz );
CSize szBox = 
		pBar->PmBridge_GetPM()->GetCheckButtonBoxSize( dc, _pcrbd );
	return szBox;
}

CSize CExtBarCheckBoxButton::CalculateLayout(
	CDC & dc,
	CSize sizePreCalc,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
CSize szButton =
	CExtBarButton::CalculateLayout(
		dc,
		sizePreCalc,
		bHorz
		);
CSize szBox = _GetBoxSize( dc );
	if( bHorz )
	{
		szButton.cx += ( szBox.cx + 4 );
		szButton.cy = max( szBox.cy, szButton.cy );
	}	
	else
	{
		szButton.cy += ( szBox.cy + 4 );
		szButton.cx = max( szBox.cx, szButton.cx );
	}
	return szButton;
}

void CExtBarCheckBoxButton::_PaintBox(
	CDC & dc,
	CRect & rcBox 
	) const
{
	ASSERT_VALID( this );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
bool bHorz = IsHorzBarOrientation();
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd;
	_pcrbd.m_pHelperSrc = (CObject*)pBar; 
	_pcrbd.m_rcClient = rcBox; 
	_pcrbd.m_rcBox = rcBox;
	_pcrbd.m_eState = (CExtPaintManager::eBoxState_t) _GetBoxState(); 
	_pcrbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
	_pcrbd.m_rcTextMargins = OnQueryMarginsText( bHorz );
	pBar->PmBridge_GetPM()->PaintCheckButtonBox(
		dc,
		_pcrbd
		);
}

void CExtBarCheckBoxButton::PaintCompound(
	CDC & dc,
	bool bPaintParentChain,
	bool bPaintChildren,
	bool bPaintOneNearestChildrenLevelOnly
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( ! IsSeparator() );

	if( ! IsPaintAble( dc ) )
		return;

	if( AnimationClient_StatePaint( dc ) )
		return;
	if( bPaintParentChain )
		PaintParentChain( dc );

CWnd * pCtrl = CtrlGet();
	if(		( pCtrl != NULL )
		&&	(	( ! m_bVertDocked )
			||	GetCtrlVisibleVertically()
			)
		)
		return;

CRect rcArea( m_ActiveRect );
	if(		rcArea.right <= rcArea.left
		||	rcArea.bottom <= rcArea.top
		)
		return;
	if(		(! IsVisible() )
		||	(GetStyle() & TBBS_HIDDEN) != 0
		||	(! dc.RectVisible(&m_ActiveRect) )
		)
		return;

CRect rcAreaInit( rcArea );

CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );

bool bHorz = IsHorzBarOrientation();

CRect rcBox( 0,0,0,0 );
CSize szBox = _GetBoxSize( dc );
	rcBox.SetRect( 
		0, 0, 
		szBox.cx, szBox.cy 
		);
	rcBox.OffsetRect( 
		rcArea.TopLeft() 
		);
	if( bHorz )
	{
		rcBox.OffsetRect(
			0,
			(rcArea.Height() - szBox.cy) / 2
			);
		rcArea.left += (szBox.cx + 4);
	}
	else
	{
		rcBox.OffsetRect(
			(rcArea.Width() - szBox.cx) / 2,
			0
			);
		rcArea.top += (szBox.cy + 4);
	}
	_PaintBox(
		dc,
		rcBox
		);

bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
bool bPushed =
		( IsPressed() && (!bDockSiteCustomizeMode) )
			? true : false;
bool bEnabled =
		( IsDisabled() && (!bDockSiteCustomizeMode) )
			? false : true;
bool bHover =
		( IsHover() && (!bDockSiteCustomizeMode) )
			? true : false;
	if(		(! bDockSiteCustomizeMode )
		&&	(! GetBar()->IsForceHoverWhenMenuTracking() )
		)
	{
		if(		CExtToolControlBar::g_bMenuTracking
			||	CExtPopupMenuWnd::IsMenuTracking()
			)
			bHover = false;
		else if( !bHover )
			bHover = IsPressedTracking();
		//if( bPushed && bHover && IsPressedTracking() )
			//bHover = false;
	} // if( ! bDockSiteCustomizeMode ...
bool bIndeterminate =
		( IsIndeterminate() && (!bDockSiteCustomizeMode) )
			? true : false;
bool bSelected = false;
CExtMenuControlBar * pMenuBar = 
		DYNAMIC_DOWNCAST( CExtMenuControlBar, pBar );
	if( pMenuBar != NULL )
	{
		INT nIndex = pMenuBar->_FlatTrackingIndexGet();
		if(		nIndex >= 0 
			&&	nIndex == pMenuBar->_GetIndexOf( this ) 
			)
			bSelected = true;
	}

CExtCmdIcon * pIcon = GetIconPtr();
CExtCmdIcon * pIconLarge = NULL;
	if(		pIcon != NULL
		&&	(! pIcon->IsEmpty() )
		&&	IsLargeIcon()
		)
	{
		CSize _sizeIcon = pIcon->GetSize();
		_sizeIcon.cx *= 2;
		_sizeIcon.cy *= 2;
		pIconLarge = new CExtCmdIcon;
		if(	pIconLarge->CreateScaledCopy(
				*pIcon,
				_sizeIcon
				)
			)
			pIcon = pIconLarge;
#ifdef _DEBUG
		else
		{
			ASSERT( FALSE );
		}
#endif // _DEBUG
	}

CExtSafeString sText = GetText();

HFONT hFont =
	(HFONT) pBar
		-> OnGetToolbarFont( ! bHorz, false, this )
		-> GetSafeHandle();
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		this,
		bHorz,
		rcArea,
		sText,
		pIcon,
		true,		// bFlat	
		false,		// bHover,
		false,		// bPushed,
		bIndeterminate,
		bEnabled,
		false,		// bDrawBorder
		bSelected,	// bDrawFocusRect
		false,		// bDefaultPushButton
		0,
		hFont,
		(		IsAbleToTrackMenu()
			&&	(! pBar->_IsSimplifiedDropDownButtons() )
			) ? true : false,
		0,
		true // bTransparentBackground
		);

	_ppbd.m_rcBorderSizes = OnQueryBorderSizes( bHorz );
	_ppbd.m_nIconAlignment = OnQueryAlignmentIcon( bHorz );
	_ppbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
	_ppbd.m_rcIconMargins = OnQueryMarginsIcon( bHorz );
	_ppbd.m_rcTextMargins = OnQueryMarginsText( bHorz );

	if( GetSeparatedDropDown() )
	{
		_ppbd.m_bSeparatedDropDown = true;
		if(		m_bDropDownHT
			&&	bPushed
			&&	( ! bDockSiteCustomizeMode )
			)
			_ppbd.m_bPushedDropDown = true;
	}

	_ppbd.m_clrCustomAccentEffectForIcon =
		OnQueryCustomAccentEffectForIcon( dc );

	pBar->PmBridge_GetPM()->PaintPushButton( dc, _ppbd );

// CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd(
// 		this,
// 		(CExtPaintManager::eBoxState_t) _GetBoxState(),
// 		false,
// 		OnQueryMaxButtonWidth( bHorz ) >= 0 ? true : false,
// 		rcArea,
// 		(LPCTSTR) sText,
// 		bHover,		
// 		bPushed,
// 		bEnabled,
// 		bSelected,
// 		0,
// 		hFont,
// 		true
// 		);
// 	_pcrbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
// 	_pcrbd.m_rcTextMargins = OnQueryMarginsText( bHorz );
// 
// 	pBar->PmBridge_GetPM()->PaintCheckButton( dc, _pcrbd );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		&&	pSite->CustomizedNodeGet() != NULL
		&&	pSite->CustomizedNodeGet() == GetCmdNode( false )
		)
	{
		pBar->PmBridge_GetPM()->PaintDragSourceRect(
			dc,
			rcAreaInit
			);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( pIconLarge != NULL )
		delete pIconLarge;

	if( bPaintChildren )
		PaintChildren( dc, bPaintOneNearestChildrenLevelOnly );	
}

CExtCmdIcon * CExtBarCheckBoxButton::GetIconPtr()
{
	ASSERT_VALID( this );
	return NULL;
}

#if (!defined __EXT_MFC_NO_RIBBON_BAR)

bool CExtBarCheckBoxButton::IsNoRibbonLayout() const
{
	ASSERT_VALID( this );
	return true;
}

INT CExtBarCheckBoxButton::RibbonILV_Get(
	INT nType // = 0 // -1 min, 0 current, 1 - max
	) const
{
	ASSERT_VALID( this );
	nType;
	return __EXT_RIBBON_ILV_SIMPLE_NORMAL;
}

CSize CExtBarCheckBoxButton::RibbonILV_CalcSize(
	CDC & dc,
	INT nILV // = -1 // -1 use current visual level
	) const
{
	ASSERT_VALID( this );
	nILV;
	return
		( const_cast < CExtBarCheckBoxButton * > ( this ) )
		-> CalculateLayout( dc, CSize(0,0), IsHorzBarOrientation() );
}

#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

/////////////////////////////////////////////////////////////////////////////
// CExtBarRadioBoxButton

IMPLEMENT_DYNCREATE(CExtBarRadioBoxButton, CExtBarCheckBoxButton)

CExtBarRadioBoxButton::CExtBarRadioBoxButton(
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	)
	: CExtBarCheckBoxButton( pBar, nCmdID, nStyle )
{
}

CExtBarRadioBoxButton::~CExtBarRadioBoxButton()
{
}

INT CExtBarRadioBoxButton::_GetBoxState() const
{
	ASSERT_VALID( this );
	ASSERT( CtrlGet() == NULL );
	ASSERT( !IsSeparator() );
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
bool bPressedTracking =
		IsPressedTracking();
bool bEnabled =
		( IsDisabled() && (!bDockSiteCustomizeMode) )
			? false : true;
bool bHover =
		( IsHover() && (!bDockSiteCustomizeMode) )
			? true : false;
bool bSelected = false;
CExtMenuControlBar * pMenuBar = 
		DYNAMIC_DOWNCAST( CExtMenuControlBar, pBar );
	if( pMenuBar != NULL )
	{
		INT nIndex = pMenuBar->_FlatTrackingIndexGet();
		if(		nIndex >= 0 
			&&	nIndex == pMenuBar->_GetIndexOf( const_cast < CExtBarRadioBoxButton * > ( this ) ) 
			)
			bSelected = true;
	}
	if( bSelected )
		bHover = true;
	if(		(! bDockSiteCustomizeMode )
		&&	(! GetBar()->IsForceHoverWhenMenuTracking() )
		)
	{
		if(		CExtToolControlBar::g_bMenuTracking
			||	CExtPopupMenuWnd::IsMenuTracking()
			)
			bHover = false;
		else if( !bHover )
			bHover = IsPressedTracking();
	} // if( ! bDockSiteCustomizeMode ...
bool bChecked =
		( (GetStyle()&TBBS_CHECKED) != 0 ) 
			? true 
			: false;
bool bIndeterminate =
		( IsIndeterminate() && (!bDockSiteCustomizeMode) )
			? true : false;
INT nCheck = 
		(bChecked || bIndeterminate)
			? BST_CHECKED
			: BST_UNCHECKED;
CExtPaintManager::eBoxState_t eState = 
		CExtPaintManager::BOX_UNCHECKED;
	switch( nCheck ) 
	{
	case BST_UNCHECKED:
		if( bEnabled )
		{
			if( bHover && !bPressedTracking )
				eState = CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED;
			else if( bHover && bPressedTracking )
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
			if( bHover && !bPressedTracking )
				eState = CExtPaintManager::BOX_MOUSE_HOVER_CHECKED;
			else if( bHover && bPressedTracking )
				eState = CExtPaintManager::BOX_LDOWN_CHECKED;
			else
				eState = CExtPaintManager::BOX_CHECKED;
		}
		else
			eState = CExtPaintManager::BOX_DISABLED_CHECKED;
		break;
	} // switch( nCheck ) 

	return eState;
}

CSize CExtBarRadioBoxButton::_GetBoxSize(
	CDC & dc
	) const
{
	ASSERT_VALID( this );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
bool bHorz = IsHorzBarOrientation();
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd;
	_pcrbd.m_pHelperSrc = (CObject*)pBar; 
	_pcrbd.m_eState = (CExtPaintManager::eBoxState_t) _GetBoxState(); 
	_pcrbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
	_pcrbd.m_rcTextMargins = OnQueryMarginsText( bHorz );
CSize szBox = 
		pBar->PmBridge_GetPM()->GetRadioButtonBoxSize( dc, _pcrbd );
	return szBox;
}

void CExtBarRadioBoxButton::_PaintBox(
	CDC & dc,
	CRect & rcBox 
	) const
{
	ASSERT_VALID( this );
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
bool bHorz = IsHorzBarOrientation();
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd;
	_pcrbd.m_pHelperSrc = (CObject*)pBar; 
	_pcrbd.m_rcClient = rcBox; 
	_pcrbd.m_rcBox = rcBox;
	_pcrbd.m_eState = (CExtPaintManager::eBoxState_t) _GetBoxState(); 
	_pcrbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
	_pcrbd.m_rcTextMargins = OnQueryMarginsText( bHorz );
	pBar->PmBridge_GetPM()->PaintRadioButtonBox(
		dc,
		_pcrbd
		);
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarLabelButton

IMPLEMENT_DYNCREATE(CExtBarLabelButton, CExtBarButton)
		
CExtBarLabelButton::CExtBarLabelButton(
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	)
	: CExtBarButton( pBar, nCmdID, nStyle )
{
	m_bAcEnabled = false;
}

CExtBarLabelButton::~CExtBarLabelButton()
{
}

CExtCmdIcon * CExtBarLabelButton::GetIconPtr()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	return NULL;
}

void CExtBarLabelButton::OnUpdateCmdUI(
	CWnd * pTarget,
	BOOL bDisableIfNoHndler,
	int nIndex
	)
{
	ASSERT_VALID( this );
	pTarget;
	bDisableIfNoHndler;
	nIndex;
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
UINT nOldStyle = pBar->GetButtonStyle( nIndex );
UINT nNewStyle = nOldStyle & (~(TBBS_DISABLED));
	if( nOldStyle != nNewStyle )
		pBar->SetButtonStyle( nIndex, nNewStyle );
}

bool CExtBarLabelButton::IsHover() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	return false;
}

bool CExtBarLabelButton::IsPressed() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	return false;
}

bool CExtBarLabelButton::IsPressedTracking() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	return false;
}

bool CExtBarLabelButton::AnimationClient_OnQueryEnabledState(
	INT eAPT // __EAPT_*** animation type
	) const
{
	ASSERT_VALID( this );
	eAPT;
	return false;
}

void CExtBarLabelButton::SetHover(
	bool bHover // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	bHover;
}

void CExtBarLabelButton::OnHover(
	CPoint point,
	bool bOn,
	bool & bSuspendTips
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	point;
	bOn;
	bSuspendTips;
}

void CExtBarLabelButton::OnClick(
	CPoint point,
	bool bDown
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonClick( this, point, bDown ) )
		return;
}

BOOL CExtBarLabelButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	ASSERT_VALID( this );
	ASSERT( pPopup != NULL );
	ASSERT( pPopup->GetSafeHwnd() == NULL );
	pPopup;
	return TRUE;
}

bool CExtBarLabelButton::OnQueryFlatTrackingEnabled() const
{
	ASSERT_VALID( this );
	return false;
}

#if (!defined __EXT_MFC_NO_RIBBON_BAR)

bool CExtBarLabelButton::IsNoRibbonLayout() const
{
	ASSERT_VALID( this );
	return true;
}

INT CExtBarLabelButton::RibbonILV_Get(
	INT nType // = 0 // -1 min, 0 current, 1 - max
	) const
{
	ASSERT_VALID( this );
	nType;
	return __EXT_RIBBON_ILV_SIMPLE_NORMAL;
}

CSize CExtBarLabelButton::RibbonILV_CalcSize(
	CDC & dc,
	INT nILV // = -1 // -1 use current visual level
	) const
{
	ASSERT_VALID( this );
	nILV;
	return
		( const_cast < CExtBarLabelButton * > ( this ) )
		-> CalculateLayout( dc, CSize(0,0), IsHorzBarOrientation() );
}

#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

/////////////////////////////////////////////////////////////////////////////
// CExtBarColorButton

IMPLEMENT_DYNCREATE(CExtBarColorButton, CExtBarButton)

void CExtBarColorButton::OnSyncIcon()
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetSafeBar();
	if( pBar->GetSafeHwnd() == NULL )
		return;
LPCTSTR strCommandProfileName = g_CmdManager->ProfileNameFromWnd( pBar->GetSafeHwnd() );
	if( strCommandProfileName == NULL )
		return;
CExtCmdIcon icon;
	OnGenerateColorIcon( icon );
	g_CmdManager->CmdSetIcon( strCommandProfileName, GetCmdID(true), icon );
}

CSize CExtBarColorButton::CalculateLayout(
	CDC & dc,
	CSize sizePreCalc,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
CExtToolControlBar * pBar = GetBar();
	if(		pBar != NULL
		&&	pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) )
		)
	{
		CSize _size = RibbonILV_CalcSize( dc );
		return _size;
	}
#endif  // (!defined __EXT_MFC_NO_RIBBON_BAR)
	return CExtBarButton::CalculateLayout( dc, sizePreCalc, bHorz );
}


void CExtBarColorButton::OnGenerateColorIcon(
	CExtCmdIcon & icon
	)
{
	ASSERT_VALID( this );
	icon.Empty();

CExtToolControlBar * pBar = GetBar();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode(false);
	if(		pNode != NULL
		&&	pBar->GetSafeHwnd() != NULL
		)
	{
		ASSERT_VALID( GetBar() );
		ASSERT_VALID( pNode );
		CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
		if( pSite != NULL )
		{
			pSite->OnColorItemGenerateIcon( this, pNode, ColorGet(true), icon );
			if( ! icon.IsEmpty() )
				return;
		} // if( pSite != NULL )
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	
// 	switch( m_eDIT )
// 	{
// 	case __DIT_FRAME:
// 		icon.AssignFromHICON( CExtPaintManager::stat_GenerateColorIconFrame( ColorGet(true) ), false );
// 	break;
// 	case __DIT_CHAR:
// 	{
// 		LOGFONT lf;
// 		::memset( &lf, 0 , sizeof(LOGFONT) );
// 		pBar->PmBridge_GetPM()->m_FontBold.GetLogFont( &lf );
// 		__EXT_MFC_STRCPY( lf.lfFaceName, LF_FACESIZE, _T("Times New Roman") );
// 		lf.lfHeight = ::MulDiv( lf.lfHeight, 3, 2 );
// 		lf.lfWeight = 900;
// 		CFont fontForLetter;
// 		VERIFY( fontForLetter.CreateFontIndirect( &lf ) );
// 		ASSERT( fontForLetter.GetSafeHandle() != NULL );
// 		icon.AssignFromHICON( CExtPaintManager::stat_GenerateColorIconChar( ColorGet(true), (HFONT)fontForLetter.GetSafeHandle() ), false );
// 	}
// 	break;
// 			//IDB_EXT_TBB_FILL_BOX_TOOL
// 	//case __DIT_BOX:
// 	default:
// 		icon.AssignFromHICON( CExtPaintManager::stat_GenerateColorIconBox( ColorGet(true) ), false );
// 	break;
// 	} // switch( m_eDIT )

CExtPaintManager * pPM = pBar->PmBridge_GetPM();
COLORREF clr = ColorGet( true );
	if( clr == COLORREF(-1L) )
	{
		VERIFY( icon.m_bmpNormal.FromColor( RGB(0,0,0), CSize(16,16), BYTE(0) ) );
		return;
	}
CExtBarColorButton::e_def_icon_type_t eDIT = CExtBarColorButton::IconTypeGet();

	switch( eDIT )
	{
	case CExtBarColorButton::__DIT_FRAME:
		icon.AssignFromHICON( CExtPaintManager::stat_GenerateColorIconFrame( clr ), false );
	break;
	case CExtBarColorButton::__DIT_CHAR:
	{
		LOGFONT lf;
		::memset( &lf, 0 , sizeof(LOGFONT) );
		pPM->m_FontBold.GetLogFont( &lf );
		__EXT_MFC_STRCPY( lf.lfFaceName, LF_FACESIZE, _T("Times New Roman") );
		lf.lfHeight = ::MulDiv( lf.lfHeight, 3, 2 );
		lf.lfWeight = 900;
		CFont fontForLetter;
		VERIFY(
			fontForLetter.CreateFontIndirect( &lf )
			);
		ASSERT( fontForLetter.GetSafeHandle() != NULL );
		icon.AssignFromHICON( CExtPaintManager::stat_GenerateColorIconChar( clr, (HFONT)fontForLetter.GetSafeHandle() ), false );
	}
	break;
	case CExtBarColorButton::__DIT_FILL_2007:
	case CExtBarColorButton::__DIT_CHAR_2007:
	{
		icon.Empty();
		icon.m_dwFlags = __EXT_ICON_FLAGS_DEFAULT;
		if(		icon.m_bmpNormal.LoadBMP_Resource(
					( eDIT == CExtBarColorButton::__DIT_FILL_2007 )
						? ( MAKEINTRESOURCE( IDB_EXT_TBB_COLOR_TOOL_2007_FILL ) )
						: ( MAKEINTRESOURCE( IDB_EXT_TBB_COLOR_TOOL_2007_CHAR ) )
					)
			&&	icon.m_bmpNormal.Make32( false )
			)
		{
			icon.m_bmpNormal.AlphaColor( RGB(255,0,255), RGB(0,0,0), BYTE(0) );
			CRect rcFill( 1, 12, 14, 15 );
			INT nX, nY;
			for( nY = rcFill.top; nY <= rcFill.bottom; nY ++ )
			{
				for( nX = rcFill.left; nX <= rcFill.right; nX ++ )
					icon.m_bmpNormal.SetPixel( nX, nY, clr, 255 );
			}
		}
	}
	break;
	//case CExtBarColorButton::__DIT_BOX:
	default:
		icon.AssignFromHICON( CExtPaintManager::stat_GenerateColorIconBox( clr ), false );
	break;
	} // switch( switch( eDIT ) )
}

__EXT_MFC_SAFE_LPCTSTR CExtBarColorButton::BtnTextDefaultGet() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
const CExtToolControlBar * pBar = GetBar();
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if(		pNode != NULL
		&&	pBar->GetSafeHwnd() != NULL
		)
	{
		ASSERT_VALID( GetBar() );
		ASSERT_VALID( pNode );
		CExtCustomizeSite * pSite =
			((CExtToolControlBar *)GetBar())->GetCustomizeSite();
		if( pSite != NULL )
			pSite->OnColorItemGetBtnTextDefault(
				(CExtBarColorButton*)this,
				pNode,
				(CExtSafeString &)m_sBtnTextColorDefault
				);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return
		m_sBtnTextColorDefault.IsEmpty()
			? _T("")
			: m_sBtnTextColorDefault
			;
}

void CExtBarColorButton::BtnTextDefaultSet(
	__EXT_MFC_SAFE_LPCTSTR sText // = NULL
	)
{
	ASSERT_VALID( this );
	m_sBtnTextColorDefault =
		(sText == NULL)
			? _T("")
			: sText
			;
}

__EXT_MFC_SAFE_LPCTSTR CExtBarColorButton::BtnTextCustomGet() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
const CExtToolControlBar * pBar = GetBar();
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if(		pNode != NULL
		&&	pBar->GetSafeHwnd() != NULL
		)
	{
		ASSERT_VALID( GetBar() );
		ASSERT_VALID( pNode );
		CExtCustomizeSite * pSite =
			((CExtToolControlBar *)GetBar())->GetCustomizeSite();
		if( pSite != NULL )
			pSite->OnColorItemGetBtnTextCustom(
				(CExtBarColorButton*)this,
				pNode,
				(CExtSafeString &)m_sBtnTextColorCustom
				);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return
		m_sBtnTextColorCustom.IsEmpty()
			? _T("")
			: m_sBtnTextColorCustom
			;
}

void CExtBarColorButton::BtnTextCustomSet(
	__EXT_MFC_SAFE_LPCTSTR sText // = NULL
	)
{
	ASSERT_VALID( this );
	m_sBtnTextColorCustom =
		(sText == NULL)
			? _T("")
			: sText
			;
}

CExtCmdIcon * CExtBarColorButton::GetIconPtr()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	return CExtBarButton::GetIconPtr();
}

BOOL CExtBarColorButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( CtrlGet() == NULL );
	ASSERT( IsAbleToTrackMenu() );

CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( pBar->m_pDockSite != NULL )
	{
		ASSERT_VALID( pBar->m_pDockSite );
		if( !pPopup->ItemInsert( GetCmdID(true) ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}

		CExtPopupMenuWnd::MENUITEMDATA & mi = 
			pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
		VERIFY(
			mi.UpdateFromCmdTree(
				pBar->m_pDockSite->m_hWnd,
				GetCmdNode(false),
				GetCmdNode(false)->GetOwnIndex(),
				pPopup
				)
			);

		pPopup->ItemGetInfo(
			pPopup->ItemGetCount() - 1
			).SetCmdNode( GetCmdNode() );

		if( (pPopup->TrackFlagsGet()&TPMX_PALETTE) != 0 )
			mi.SetChildCombine();

		return TRUE;
	} // if( pBar->m_pDockSite != NULL )

#endif

CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( GetSafeBar()->GetSafeHwnd() ),
			GetCmdID(false)
			);
	
CExtCmdIcon icon;
CExtCmdIcon * pIcon = GetIconPtr();
	if(		pIcon != NULL 
	   &&	(!pIcon->IsEmpty())
	   )
	   icon = *pIcon;
CExtPopupColorMenuWnd * pColorPopup =
		STATIC_DOWNCAST(
			CExtPopupColorMenuWnd,
			CExtPopupMenuWnd::InstantiatePopupMenu(
				pBar->GetSafeHwnd(),
				RUNTIME_CLASS(CExtPopupColorMenuWnd),
				this
				)
			);
	VERIFY(
		pPopup->ItemInsertSpecPopup(
			pColorPopup,
			-1,
			pCmdItem->m_sMenuText,
			icon
			)
		);
	pColorPopup->m_lParamCookie = LParamCookieGet();
	pColorPopup->m_bEnableBtnColorDefault = IsEnabledBtnColorDefault();
	pColorPopup->m_bEnableBtnColorCustom = IsEnabledBtnColorCustom();
	pColorPopup->m_clrDefault = ColorGet( false );
	pColorPopup->m_clrInitial = ColorGet( true );
CExtSafeString _s( BtnTextDefaultGet() );
	if( !_s.IsEmpty() )
		pColorPopup->m_sBtnTextColorDefault = _s;
	_s = BtnTextCustomGet();
	if( !_s.IsEmpty() )
		pColorPopup->m_sBtnTextColorCustom = _s;
	
CExtPopupMenuWnd::MENUITEMDATA & mi = 
		pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
	mi.Enable( IsEnabled() ? true : false );

	if( (pPopup->TrackFlagsGet()&TPMX_PALETTE) != 0 )
		mi.SetChildCombine();

	return TRUE;
}

bool CExtBarColorButton::CanBePressedInDisabledState()
{
	return false;
}

bool CExtBarColorButton::IsAbleToTrackMenu(
	bool bCustomizeMode // = false
	) const
{
	ASSERT_VALID( this );
	if( bCustomizeMode )
		return false;
	return true;
}

UINT CExtBarColorButton::OnTrackPopup(
	CPoint point,
	bool bSelectAny,
	bool bForceNoAnimation
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( CtrlGet() == NULL );
	ASSERT( IsAbleToTrackMenu() );

CExtToolControlBar * pBar = GetBar();
bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return UINT(-1L);

	if( !IsEnabled() )
		return UINT(-1L);

bool bPrevTBMT = CExtToolControlBar::g_bMenuTracking;
	if(		CExtToolControlBar::g_bMenuTracking
		//&&	CExtPopupMenuWnd::IsMenuTracking()
		&&	pBar->_GetIndexOf(this) ==
			pBar->GetMenuTrackingButton()
		)
		return UINT(-1L);

	CExtToolControlBar::_CloseTrackingMenus();

	if( pBar->IsFloating() )
	{
		pBar->ActivateTopParent();
		CFrameWnd * pFrame =
			pBar->GetDockingFrame();
		ASSERT_VALID( pFrame );
		pFrame->BringWindowToTop();
	}

CWnd * pWndCmdTarget = GetCmdTargetWnd();
	ASSERT_VALID( pWndCmdTarget );

CExtPopupColorMenuWnd * pPopup =
		STATIC_DOWNCAST(
			CExtPopupColorMenuWnd,
			CExtPopupMenuWnd::InstantiatePopupMenu(
				pBar->GetSafeHwnd(),
				RUNTIME_CLASS(CExtPopupColorMenuWnd),
				this
				)
			);
	VERIFY(
		pPopup->CreatePopupMenu(
			GetCmdTargetWnd()->GetSafeHwnd()
			)
		);
	pPopup->m_lParamCookie = LParamCookieGet();
	pPopup->m_bEnableBtnColorDefault = IsEnabledBtnColorDefault();
	pPopup->m_bEnableBtnColorCustom = IsEnabledBtnColorCustom();
	pPopup->m_clrDefault = ColorGet( false );
	pPopup->m_clrInitial = ColorGet( true );
CExtSafeString _s( BtnTextDefaultGet() );
	if( !_s.IsEmpty() )
		pPopup->m_sBtnTextColorDefault = _s;
	_s = BtnTextCustomGet();
	if( !_s.IsEmpty() )
		pPopup->m_sBtnTextColorCustom = _s;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	pPopup->_CmdNodeSet( GetCmdNode(false) );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	CExtToolControlBar::g_bMenuTracking = bPrevTBMT;

CRect rcBtn = Rect();
	pBar->ClientToScreen( &rcBtn );
	pBar->ClientToScreen( &point );

DWORD dwTrackFlags = OnGetTrackPopupFlags() | TPMX_OWNERDRAW_FIXED;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) ) )
		dwTrackFlags |= TPMX_COMBINE_NONE|TPMX_RIBBON_MODE|TPMX_NO_HIDE_RARELY;
	else
		dwTrackFlags |= TPMX_COMBINE_DEFAULT;
#else // (!defined __EXT_MFC_NO_RIBBON_BAR)
		dwTrackFlags |= TPMX_COMBINE_DEFAULT;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	if( bSelectAny )
		dwTrackFlags |= TPMX_SELECT_ANY;
	if( CExtToolControlBar::g_bMenuTrackingExpanded )
		dwTrackFlags |= TPMX_NO_HIDE_RARELY;

	if( bForceNoAnimation )
		dwTrackFlags |= TPMX_FORCE_NO_ANIMATION;

	pBar->_SwitchMenuTrackingIndex(
		pBar->_GetIndexOf( this )
		);

UINT nResultCmdID = (UINT)-1L;
	pPopup->m_hWndNotifyMenuClosed = pBar->GetSafeHwnd();
	if( GetSeparatedDropDown() )
		m_bDropDownHT = true;
	g_pTrackingMenuTBB = this;
	if( ! pPopup->TrackPopupMenu(
			dwTrackFlags,
			point.x,
			point.y,
			&rcBtn,
			GetBar(),
			CExtToolControlBar::_CbPaintCombinedContent,
			&nResultCmdID,
			true
			)
		)
	{
		g_pTrackingMenuTBB = NULL;
		//delete pPopup;
		CExtToolControlBar::_CloseTrackingMenus();
		return UINT(-1L);
	}
	CExtToolControlBar::g_bMenuTracking = true;
	pBar->_SwitchMenuTrackingIndex(
		pBar->_GetIndexOf( this )
		);
	return UINT(-1L);
}

__EXT_MFC_INT_PTR CExtBarColorButton::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	return CExtBarButton::OnToolHitTest( point, pTI );
}

LRESULT CExtBarColorButton::OnHelpHitTest(
	CPoint point
	)
{
	ASSERT_VALID( this );
//	return
//		CExtBarButton::OnHelpHitTest( point );
	point;
	return GetCmdID();
}

void CExtBarColorButton::OnUpdateCmdUI(
	CWnd * pTarget,
	BOOL bDisableIfNoHndler,
	int nIndex
	)
{
	ASSERT_VALID( this );
	CExtBarButton::OnUpdateCmdUI(
		pTarget,
		bDisableIfNoHndler,
		nIndex
		);
}

bool CExtBarColorButton::CanStartCommandDrag()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( GetCmdNode() == NULL )
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return true;
}

LPARAM CExtBarColorButton::LParamCookieGet() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return pNode->GetLParam();
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_lParamCookie;
}

void CExtBarColorButton::LParamCookieSet(
	LPARAM lParamCookie // = 0L
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->SetLParam( lParamCookie );
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_lParamCookie = lParamCookie;
}

bool CExtBarColorButton::IsEnabledBtnColorDefault() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return ( (pNode->GetFlags() & __ECTN_TBB_MCB_DEFAULT) != 0 ) ? true : false;
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_bEnableBtnColorDefault;
}
void CExtBarColorButton::EnableBtnColorDefault(
	bool bEnable // = true
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode(false);
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->ModifyFlags(
			bEnable ? __ECTN_TBB_MCB_DEFAULT : 0,
			bEnable ? 0 : __ECTN_TBB_MCB_DEFAULT
			);
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_bEnableBtnColorDefault = bEnable;
}
bool CExtBarColorButton::IsEnabledBtnColorCustom() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return ( (pNode->GetFlags() & __ECTN_TBB_MCB_CUSTOM) != 0 ) ? true : false;
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_bEnableBtnColorCustom;
}
void CExtBarColorButton::EnableBtnColorCustom(
	bool bEnable // = true
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode(false);
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->ModifyFlags(
			bEnable ? __ECTN_TBB_MCB_CUSTOM: 0,
			bEnable ? 0 : __ECTN_TBB_MCB_CUSTOM
			);
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_bEnableBtnColorCustom = bEnable;
}

COLORREF CExtBarColorButton::ColorGet( bool bSelected ) const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
const CExtToolControlBar * pBar = GetBar();
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if(		pNode != NULL
		&&	pBar->GetSafeHwnd() != NULL
		)
	{
		ASSERT_VALID( GetBar() );
		ASSERT_VALID( pNode );
		CExtCustomizeSite * pSite =
			((CExtToolControlBar *)GetBar())->GetCustomizeSite();
		if( pSite != NULL )
		{
			COLORREF clr = bSelected ? m_clrSelected : m_clrDefault;
			if(	pSite->OnColorItemValueGet(
					(CExtBarColorButton*)this,
					pNode,
					clr,
					bSelected
					)
				)
				return clr;
		} // if( pSite != NULL )
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return bSelected ? m_clrSelected : m_clrDefault;
}

void CExtBarColorButton::ColorSet( COLORREF clr, bool bSelected )
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtToolControlBar * pBar = GetBar();
CExtCustomizeCmdTreeNode * pNode = GetCmdNode(false);
	if(		pNode != NULL
		&&	pBar->GetSafeHwnd() != NULL
		)
	{
		ASSERT_VALID( GetBar() );
		ASSERT_VALID( pNode );
		CExtCustomizeSite * pSite =
			pBar->GetCustomizeSite();
		if( pSite != NULL )
			pSite->OnColorItemValueSet(
				this,
				pNode,
				clr,
				bSelected
				);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( bSelected )
	{
		if( m_clrSelected != clr )
		{
			m_clrSelected = clr;
			OnSyncIcon();
		}
	}
	else
	{
		if( m_clrDefault != clr )
		{
			m_clrDefault = clr;
			OnSyncIcon();
		}
	}
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
int CExtBarColorButton::CalculateDropOverEffect(
	CPoint point,
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	bool bAllowTrackNodePopup // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( !_dragInfo.IsEmpty() );
	bAllowTrackNodePopup;
	return
		CExtBarButton::CalculateDropOverEffect(
			point,
			_dragInfo,
			false
			);
}
CExtCustomizeCmdTreeNode * CExtBarColorButton::GetCmdNode(
	bool bInitial // = false
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	return CExtBarButton::GetCmdNode( bInitial );
}
void CExtBarColorButton::SetBasicCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	CExtBarButton::SetBasicCmdNode( pNode );
}
void CExtBarColorButton::SetCustomizedCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	CExtBarButton::SetCustomizedCmdNode( pNode );
}

CExtCustomizeCmdTreeNode * CExtBarColorButton::OnCustomizeNodeInit(
	CExtCustomizeSite * pSite,
	CExtCustomizeCmdTreeNode * pParentNode // toolbar node
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
CExtCustomizeCmdTreeNode * pNode =
		CExtBarButton::OnCustomizeNodeInit( pSite, pParentNode );
	ASSERT_VALID( pNode );
	return pNode;
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtBarColorButton::OnDeliverCmd()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonDeliverCmd( this ) )
		return;
	if( pBar->OnDeliverCmd( this ) )
		return;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		CExtToolControlBar * pBar = GetBar();
		CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
		if(		pSite != NULL
			&&	pSite->OnColorItemChanged(
					this,
					pNode,
					true,
					ColorGet(true),
					LParamCookieGet()
					)
			)
			return;
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	GetCmdTargetWnd()->SendMessage(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
		(WPARAM)ColorGet(true),
		LParamCookieGet()
		);
}


/////////////////////////////////////////////////////////////////////////////
// CExtBarContentExpandButton

IMPLEMENT_DYNCREATE(CExtBarContentExpandButton, CExtBarButton)

CExtBarContentExpandButton::CExtBarContentExpandButton(
	CExtToolControlBar * pBar // = NULL
	)
	: CExtBarButton( pBar )
{
	SetCmdID( __ID_TOOLBAR_RIGHT_BUTTON__ );
	AnimationClient_EnabledSet( false );
}

CExtBarContentExpandButton::~CExtBarContentExpandButton()
{
}

CSize CExtBarContentExpandButton::CalculateLayout(
	CDC & dc,
	CSize sizePreCalc,
	BOOL bHorz
	)
{
	dc;
	ASSERT( CtrlGet() == NULL );
	ASSERT( ! IsSeparator() );
	ASSERT( GetCmdID(false) == __ID_TOOLBAR_RIGHT_BUTTON__ );
SIZE sizeReal = { 0, 0 };
CExtToolControlBar * pBar = GetBar();
	if( pBar->PmBridge_GetPM()->Toolbar_GetSizeOfCEB(
			sizeReal,
			sizePreCalc,
			dc,
			bHorz,
			this
			)
		)
	{
		m_ActiveSize = sizeReal;
		return m_ActiveSize;
	}
CSize _size(
			bHorz ? __RIGHT_BUTTON_HORZ_DX__ : sizePreCalc.cx,
			bHorz ? sizePreCalc.cy : __RIGHT_BUTTON_VERT_DY__
			);
	m_ActiveSize = _size;
	return m_ActiveSize;
}

void CExtBarContentExpandButton::PaintCompound(
	CDC & dc,
	bool bPaintParentChain,
	bool bPaintChildren,
	bool bPaintOneNearestChildrenLevelOnly
	)
{
	ASSERT( CtrlGet() == NULL );
	ASSERT_VALID( (&dc) );
	ASSERT( !  IsSeparator() );
	ASSERT( GetCmdID(false) == __ID_TOOLBAR_RIGHT_BUTTON__ );

	if( ! IsPaintAble( dc ) )
		return;
	if( AnimationClient_StatePaint( dc ) )
		return;
	if( bPaintParentChain )
		PaintParentChain( dc );

CRect rcArea( m_ActiveRect );
	if(		rcArea.right <= rcArea.left
		||	rcArea.bottom <= rcArea.top
		)
		return;

CExtToolControlBar * pBar = GetBar();
bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
bool bEnabled = 
		( (!IsDisabled()) || bDockSiteCustomizeMode )
			? true : false;
bool bBarIsCompletelyVisible =
		(GetButtons().GetSize() == 0) ? true : false;
bool bEmpty = 
		(	(!pBar->m_bRightButtonDisplayBarsList)
			&& bBarIsCompletelyVisible
		) ? true : false;
bool bPushed =
		( (!bEmpty) && bEnabled && IsPressed() && (!bDockSiteCustomizeMode) )
			? true : false;
bool bHover =
		(	(!bEmpty)
			&& bEnabled
			&& (!bDockSiteCustomizeMode)
			&& IsHover()
			&& !CExtToolControlBar::g_bMenuTracking
			&& !CExtPopupMenuWnd::IsMenuTracking()
		) ? true : false;
bool bHorz = IsHorzBarOrientation();
	pBar->PmBridge_GetPM()->PaintToolbarExpandButton(
		dc,
		rcArea,
		bHorz,
		bBarIsCompletelyVisible,
		bEnabled && !bEmpty,
		bPushed,
		bHover,
		this,
		0L,
		( (!bHover) && (!bPushed) )
		);

	if( bPaintChildren )
		PaintChildren( dc, bPaintOneNearestChildrenLevelOnly );
}

BOOL CExtBarContentExpandButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	pPopup;
	ASSERT( FALSE );
	return FALSE;
}

UINT CExtBarContentExpandButton::OnGetTrackPopupFlags()
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
DWORD dwTrackFlags = CExtBarButton::OnGetTrackPopupFlags();
	if( pBar->IsPaletteChevronMenu() )
		dwTrackFlags |= TPMX_PALETTE;
	else
		dwTrackFlags &= ~TPMX_PALETTE;
	dwTrackFlags &= ~(TPMX_COMBINE_MASK);
	if( pBar->PmBridge_GetPM()->IsToolBarChevronMenuCombined() )
		dwTrackFlags |= TPMX_COMBINE_DEFAULT;

	return dwTrackFlags;
}

UINT CExtBarContentExpandButton::OnTrackPopup(
	CPoint point,
	bool bSelectAny,
	bool bForceNoAnimation
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );

bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return UINT(-1L);

	if( !IsEnabled() )
		return UINT(-1L);

bool bPrevTBMT = CExtToolControlBar::g_bMenuTracking;

	if(		CExtToolControlBar::g_bMenuTracking
			//&&	CExtPopupMenuWnd::IsMenuTracking()
		&&	pBar->_GetIndexOf(this) ==
				pBar->m_nBtnIdxMenuTracking
		)
		return UINT(-1L);

bool bBarIsCompletelyVisible =
		(GetButtons().GetSize() == 0) ? true : false;
bool bEmpty = 
		(	!pBar->m_bRightButtonDisplayBarsList
			&& bBarIsCompletelyVisible
		) ? true : false;
	if( bEmpty )
		return UINT(-1L);

	CExtToolControlBar::_CloseTrackingMenus();

	CExtToolControlBar::g_bMenuTracking = bPrevTBMT;

	if( pBar->IsFloating() )
	{
		pBar->ActivateTopParent();
		CFrameWnd * pFrame =
			pBar->GetParentFrame();
		if( pFrame != NULL )
			pFrame->BringWindowToTop();
	}

HWND hWndTrack = GetCmdTargetWnd()->GetSafeHwnd();
	ASSERT(
		hWndTrack != NULL
		&& ::IsWindow(hWndTrack)
		);

DWORD dwTrackFlags =
		OnGetTrackPopupFlags()
		| TPMX_OWNERDRAW_FIXED
		;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) ) )
		dwTrackFlags |= TPMX_COMBINE_NONE|TPMX_RIBBON_MODE|TPMX_NO_HIDE_RARELY;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

CExtPopupMenuWnd * pPopup = OnCreatePopupMenuWnd();
	ASSERT_VALID( pPopup );

	VERIFY( pPopup->CreatePopupMenu(hWndTrack) );
	pPopup->TrackFlagsSet( dwTrackFlags );

CExtControlBar::POPUP_MENU_EVENT_DATA _pmed(
		CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CTXEXPBTN_TOP,
		pPopup,
		GetBar(),
		CPoint( -1, -1 ),
		WM_NCLBUTTONDOWN,
		this
		);

	if( !_pmed.NotifyTarget( false ) )
	{

		// append hidden buttons
		__EXT_MFC_INT_PTR nCount = GetButtons().GetSize();
		if( nCount != 0 )
		{
			ASSERT( nCount > 0 );
			int nCountBefore = pPopup->ItemGetCount();
			for( int i=0; i < nCount; i++ )
			{
				CExtBarButton * pTBB = GetButtons() [i];
				ASSERT( pTBB != NULL );
				if( i==0 && pTBB->IsSeparator() )
					continue;
				ASSERT( ! pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) );
				VERIFY( pTBB->PutToPopupMenu(pPopup) );
			} // for( int i=0; i< nCount; i++ )
			if( (dwTrackFlags&TPMX_PALETTE) != NULL )
			{
				pPopup->ItemRemoveAllSeparators();
				pPopup->ResetPaletteWrapping( true );
			}
			int nCountAfter = pPopup->ItemGetCount();
			ASSERT( nCountAfter >= nCountBefore );
			if(		nCountAfter != nCountBefore
				&&	(! (pBar->m_bPresubclassDialogMode) )
				&&	pBar->m_bRightButtonDisplayBarsList
				)
			{
				// append separator
				VERIFY(
					pPopup->ItemInsert(
						CExtPopupMenuWnd::TYPE_SEPARATOR
						)
					);
			}
		} // if( nCount != 0 )
		// append show/hide popup

		if(		(! (pBar->m_bPresubclassDialogMode) )
			&&	pBar->m_bRightButtonDisplayBarsList
			)
		{
			CFrameWnd * pFrame = pBar->_GetDockingFrameImpl();
			if( pFrame != NULL )
			{
				ASSERT_VALID( pFrame );
				CExtSafeString sShowHidePanels;
				if( ! g_ResourceManager->LoadString( sShowHidePanels, IDS_SHOW_HIDE_PANELS ) )
					sShowHidePanels = _T("&Show/hide panels");

				CExtControlBar::POPUP_MENU_EVENT_DATA _pmed(
					CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CTXEXPBTN_APPEND,
					pPopup,
					GetBar(),
					CPoint( -1, -1 ),
					WM_NCLBUTTONDOWN,
					this
					);
			
				if( ! _pmed.NotifyTarget( false ) )
				{
					VERIFY(
						pPopup->ItemInsert(
							(UINT)CExtPopupMenuWnd::TYPE_POPUP,
							-1,
							sShowHidePanels
							)
						);
					CExtPopupMenuWnd::MENUITEMDATA & mi =
						pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
					mi.SetChildCombine();
					mi.SetToolButton();
					mi.SetToolWrap();
					CExtPopupMenuWnd * pPopupBars = mi.GetPopup();
					ASSERT( pPopupBars != NULL );
					if( ! CExtDockBar::_ConstructDockSiteControlBarPopupMenu(
							pFrame,
							pPopupBars,
							CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CTXEXPBTN_BARS,
							GetBar(),
							WM_NCLBUTTONDOWN,
							this
							)
						)
					{
						VERIFY(
							pPopup->ItemRemove(
								pPopup->ItemGetCount() - 1
								)
							);
					}
				} // if( !_pmed.NotifyTarget( false ) )

				_pmed.NotifyTarget( true );
			
			} // if( pFrame != NULL )

		} // if( (!pBar->m_bPresubclassDialogMode) ...

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if(		pBar->m_bCustomizationAllowed
			&&	pBar->GetCustomizeSite() != NULL
			)
		{ // if customization allowed
			VERIFY(
				CExtDockBar::_AppendAddRemoveButtonsCustomizeMenu(
					pPopup,
					GetBar()
					)
				);
		} // if customization allowed
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	} // if( !_pmed.NotifyTarget( false ) )
	_pmed.NotifyTarget( true );

	
	if( pPopup->ItemGetCount() == 0 )
	{
		delete pPopup;
		return UINT(-1L);
	}

CRect rcBtn = Rect();
	pBar->ClientToScreen( &rcBtn );
	pBar->ClientToScreen( &point );
	if( bSelectAny )
		dwTrackFlags |= TPMX_SELECT_ANY;
	if( CExtToolControlBar::g_bMenuTrackingExpanded )
		dwTrackFlags |= TPMX_NO_HIDE_RARELY;
	if( bForceNoAnimation )
		dwTrackFlags |= TPMX_FORCE_NO_ANIMATION;

	pBar->_SwitchMenuTrackingIndex(
		pBar->_GetIndexOf( this )
		);

	pPopup->m_hWndNotifyMenuClosed = pBar->GetSafeHwnd();
	if( GetSeparatedDropDown() )
		m_bDropDownHT = true;
	g_pTrackingMenuTBB = this;
	if( ! pPopup->TrackPopupMenu(
			dwTrackFlags,
			point.x,
			point.y,
			&rcBtn,
			GetBar(),
			CExtToolControlBar::_CbPaintCombinedContent,
			NULL,
			true
			)
		)
	{
		g_pTrackingMenuTBB = NULL;
		//delete pPopup;
		CExtToolControlBar::_CloseTrackingMenus();
		return UINT(-1L);
	}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
	if( pSite != NULL )
		pSite->BarDefCmdTargetSet( GetBar() );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	CExtToolControlBar::g_bMenuTracking = true;
	pBar->_SwitchMenuTrackingIndex(
		pBar->_GetIndexOf( this )
		);
	return UINT(-1L);
}

void CExtBarContentExpandButton::OnHover(
	CPoint point,
	bool bOn,
	bool & bSuspendTips
	)
{
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	bSuspendTips = false;
bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;
	CExtBarButton::OnHover(
		point,
		bOn,
		bSuspendTips
		);
}

void CExtBarContentExpandButton::OnClick(
	CPoint point,
	bool bDown
	)
{
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonClick( this, point, bDown ) )
		return;

bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;

	if( !bDown )
		return;

	CExtToolControlBar::g_bMenuTrackingExpanded = false;

bool bThisButtonTrackingMenu = ( g_pTrackingMenuTBB == this ) ? true : false;
	if( ! bThisButtonTrackingMenu )
		OnTrackPopup( point, false, false );
	else
	{
		CExtPopupMenuWnd::CancelMenuTracking();
		CExtBarButton::g_pTrackingMenuTBB = NULL;
	}
}

__EXT_MFC_INT_PTR CExtBarContentExpandButton::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	point; // should be inside this button

	if( pTI != NULL )
	{
		CRect rcArea = Rect();
		::CopyRect(
			&(pTI->rect),
			&rcArea
			);

		__EXT_MFC_INT_PTR nCmdID = AFX_IDS_IDLEMESSAGE; //(int)pBar->GetDlgCtrlID();
		
		pTI->uId = (UINT)nCmdID;
		pTI->hwnd = GetSafeBar()->GetSafeHwnd();
		CExtSafeString sExpandTip;
		if( ! g_ResourceManager->LoadString( sExpandTip, IDS_CONTENT_EXPAND_TIP ) )
			sExpandTip = _T("Where are buttons?");

		pTI->lpszText = (LPTSTR)
			::calloc(
				(sExpandTip.GetLength() + 1),
				sizeof(TCHAR)
				);
		if( pTI->lpszText != NULL )
			__EXT_MFC_STRCPY(
				pTI->lpszText,
				sExpandTip.GetLength() + 1,
				sExpandTip
				);
		else
			pTI->lpszText = LPSTR_TEXTCALLBACK;

		return (__EXT_MFC_INT_PTR)nCmdID;
	} // if( pTI != NULL )
	
	return -1;
}

LRESULT CExtBarContentExpandButton::OnHelpHitTest(
	CPoint point
	)
{
	ASSERT_VALID( this );
	return
		CExtBarButton::OnHelpHitTest( point );
}

bool CExtBarContentExpandButton::CanStartCommandDrag()
{
	ASSERT_VALID( this );
	return false;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * CExtBarContentExpandButton::GetCmdNode(
	bool bInitial // = false
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	bInitial;
	ASSERT( m_pCmdNodeI == NULL );
	ASSERT( m_pCmdNodeC == NULL );
	return NULL;
}
void CExtBarContentExpandButton::SetBasicCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( m_pCmdNodeI == NULL );
	ASSERT( m_pCmdNodeC == NULL );
	ASSERT( FALSE ); // should be never called
	pNode;
}
void CExtBarContentExpandButton::SetCustomizedCmdNode(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( m_pCmdNodeI == NULL );
	ASSERT( m_pCmdNodeC == NULL );
	ASSERT( FALSE ); // should be never called
	pNode;
}
CExtCustomizeCmdTreeNode * CExtBarContentExpandButton::OnCustomizeNodeInit(
	CExtCustomizeSite * pSite,
	CExtCustomizeCmdTreeNode * pParentNode // toolbar node
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT( pSite != NULL );
	ASSERT_VALID( pParentNode );
	pSite;
	pParentNode;
	return NULL; // no customization support
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtBarSliderButton

IMPLEMENT_DYNCREATE(CExtBarSliderButton, CExtBarButton)

ULONG CExtBarSliderButton::ScrollLimitGet() const
{
	ASSERT_VALID( this );
ULONG nScrollPageSize = ScrollPageSizeGet();
ULONG nScrollTotalRange = ScrollTotalRangeGet();
ULONG nScrollLimit = nScrollTotalRange;
	if( nScrollLimit > nScrollPageSize )
	{
		const CExtToolControlBar * pBar = GetBar();
		bool bHorz = pBar->IsDockedVertically() ? false : true;
		INT nExtentBtn = bHorz
				? ScrollButtonExtentHorzGet()
				: ScrollButtonExtentVertGet();
		if( nExtentBtn > 0 )
			nScrollLimit -= nScrollPageSize;
	} // if( nScrollLimit > nScrollPageSize )
	else
		nScrollLimit = 0L;
	
	return nScrollLimit;
}

ULONG CExtBarSliderButton::ScrollTotalRangeGet() const
{
	ASSERT_VALID( this );
	return m_nScrollTotalRange;
}

ULONG CExtBarSliderButton::ScrollTotalRangeSet( ULONG nScrollTotalRange )
{
	ASSERT_VALID( this );
ULONG nScrollPosOld = ScrollPosGet();
	if( nScrollPosOld > nScrollTotalRange )
		ScrollPosSet( nScrollTotalRange );
ULONG nScrollTotalRangeOld = ScrollTotalRangeGet();
	m_nScrollTotalRange = nScrollTotalRange;
	ASSERT_VALID( this );
	RecalcSliderLayout();
	return nScrollTotalRangeOld;
}

ULONG CExtBarSliderButton::ScrollPosGet() const
{
	ASSERT_VALID( this );
	return m_nScrollPos;
}

ULONG CExtBarSliderButton::ScrollPosSet( ULONG nScrollPos )
{
	ASSERT_VALID( this );
ULONG nScrollPosOld = ScrollPosGet();
	m_nScrollPos = nScrollPos;
	ASSERT_VALID( this );
	RecalcSliderLayout();
	return nScrollPosOld;
}

ULONG CExtBarSliderButton::ScrollPageSizeGet() const
{
	ASSERT_VALID( this );
	return m_nScrollPageSize;
}

ULONG CExtBarSliderButton::ScrollPageSizeSet( ULONG nScrollPageSize )
{
	ASSERT_VALID( this );
ULONG nScrollPageSizeOld = ScrollPageSizeGet();
	m_nScrollPageSize = nScrollPageSize;
	ASSERT_VALID( this );
	RecalcSliderLayout();
	return nScrollPageSizeOld;
}

INT CExtBarSliderButton::ScrollButtonExtentHorzGet() const
{
	ASSERT_VALID( this );
	return m_nScrollButtonExtentH;
}

INT CExtBarSliderButton::ScrollButtonExtentHorzSet( INT nScrollButtonExtent )
{
	ASSERT_VALID( this );
INT nScrollButtonExtentOld = ScrollButtonExtentHorzGet();
	m_nScrollButtonExtentH = nScrollButtonExtent;
	ASSERT_VALID( this );
	RecalcSliderLayout();
	return nScrollButtonExtentOld;
}

INT CExtBarSliderButton::ScrollButtonExtentVertGet() const
{
	ASSERT_VALID( this );
	return m_nScrollButtonExtentV;
}

INT CExtBarSliderButton::ScrollButtonExtentVertSet( INT nScrollButtonExtent )
{
	ASSERT_VALID( this );
INT nScrollButtonExtentOld = ScrollButtonExtentVertGet();
	m_nScrollButtonExtentV = nScrollButtonExtent;
	ASSERT_VALID( this );
	RecalcSliderLayout();
	return nScrollButtonExtentOld;
}

INT CExtBarSliderButton::ScrollControlExtentHorzGet() const
{
	ASSERT_VALID( this );
	return m_nScrollControlExtentH;
}

INT CExtBarSliderButton::ScrollControlExtentHorzSet( INT nScrollControlExtent )
{
	ASSERT_VALID( this );
INT nScrollControlExtentOld = ScrollControlExtentHorzGet();
	m_nScrollControlExtentH = nScrollControlExtent;
	ASSERT_VALID( this );
	RecalcSliderLayout();
	return nScrollControlExtentOld;
}

INT CExtBarSliderButton::ScrollControlExtentVertGet() const
{
	ASSERT_VALID( this );
	return m_nScrollControlExtentV;
}

INT CExtBarSliderButton::ScrollControlExtentVertSet( INT nScrollControlExtent )
{
	ASSERT_VALID( this );
INT nScrollControlExtentOld = ScrollControlExtentVertGet();
	m_nScrollControlExtentV = nScrollControlExtent;
	ASSERT_VALID( this );
	RecalcSliderLayout();
	return nScrollControlExtentOld;
}

#ifdef _DEBUG
void CExtBarSliderButton::AssertValid() const
{
	CExtBarButton::AssertValid();
	ASSERT( m_nScrollPos <= m_nScrollTotalRange );
	ASSERT( m_nScrollButtonExtentH >= 0 );
	ASSERT( m_nScrollButtonExtentV >= 0 );
	ASSERT( m_nScrollControlExtentH >= 0 );
	ASSERT( m_nScrollControlExtentV >= 0 );
	ASSERT( m_pCtrl == NULL );
}
void CExtBarSliderButton::Dump(CDumpContext& dc) const
{
	CExtBarButton::Dump( dc );
}
#endif

void CExtBarSliderButton::SetMenu(
	HMENU hMenu,
	bool bPopupMenu,
	bool bAutoDestroyMenu
	)
{
	ASSERT_VALID( this );
	ASSERT( FALSE ); // should not be used with text-field button
	hMenu;
	bPopupMenu;
	bAutoDestroyMenu;
}

HMENU CExtBarSliderButton::GetMenu()
{
	ASSERT_VALID( this );
	return NULL;
}

//bool CExtBarSliderButton::IsLargeIcon() const
//{
//	ASSERT_VALID( this );
//	return false;
//}
//
//bool CExtBarSliderButton::IsDisplayScreenTip() const
//{
//	ASSERT_VALID( this );
//	return false;
//}
//
//bool CExtBarSliderButton::IsDisplayShortcutKeysOnScreenTip() const
//{
//	ASSERT_VALID( this );
//	return false;
//}

void CExtBarSliderButton::SetRect( const RECT & rectButton )
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	GetBar()->AnimationSite_ClientProgressStop(
		this
		);
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
	AnimationClient_TargetRectSet( rectButton );
	m_ActiveRect = rectButton;
	RecalcSliderLayout();
}

bool CExtBarSliderButton::CanBePressedInDisabledState()
{
	return false;
}

bool CExtBarSliderButton::IsAbleToTrackMenu(
	bool bCustomizeMode // = false
	) const
{
	ASSERT_VALID( this );
	bCustomizeMode;
	return false;
}

bool CExtBarSliderButton::IsContainerOfPopupLikeMenu()
{
	ASSERT_VALID( this );
	return false;
}

//CExtCmdIcon * CExtBarSliderButton::GetIconPtr()
//{
//	ASSERT_VALID( this );
//	return NULL; // no icon at all for text-fields
//}

CSize CExtBarSliderButton::CalculateLayout(
	CDC & dc,
	CSize sizePreCalc,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT_VALID( (&dc) );
	if( GetNoRotateVerticalLayout() )
		bHorz = TRUE;
	m_ActiveSize = sizePreCalc;
	ASSERT( ! IsSeparator() );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( GetCmdNode() == NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	{
		CExtToolControlBar * pBar = GetBar();
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( pBar->GetSafeHwnd() ),
				GetCmdID( true )
				);
		if( pCmdItem == NULL )
			return m_ActiveSize; // shutdown mode
	} // if( GetCmdNode() == NULL )
CSize _size( 0, 0 );
	// update size for icon
bool bIconEmpty = true;
CExtCmdIcon * pIcon = GetIconPtr();
	if( pIcon != NULL && (! pIcon->IsEmpty() ) )
	{
		bIconEmpty = false;
		_size = pIcon->GetSize();
		if( IsLargeIcon() )
		{
			_size.cx *= 2;
			_size.cy *= 2;
		}
	}
	// update size for text
bool bTextEmpty = true;
CExtSafeString sMeasureText = GetText();
	if( sMeasureText.IsEmpty() )
	{
		CSize sizeAlign = m_pBar->_GetDefButtonSize();
		if( bHorz )
		{
			sizeAlign.cy -= 7;
			_size.cy = max( _size.cy, sizeAlign.cy );
		} // if( bHorz )
		else
		{
			sizeAlign.cx -= 7;
			_size.cx = max( _size.cx, sizeAlign.cx );
		} // else from if( bHorz )
	} // if( sMeasureText.IsEmpty() )
	else
	{
		bTextEmpty = false;
// 		static TCHAR stat_strDummyAmpSeq[] = _T("\001\001");
// 		sMeasureText.Replace( _T("&&"), stat_strDummyAmpSeq );
// 		sMeasureText.Remove( _T('&') );
// 		sMeasureText.Replace( stat_strDummyAmpSeq, _T("&") );
		CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
		if( ! bHorz )
			eLO = CExtRichContentLayout::__ELOT_90_CW;
		CRect rcTextMeasure( 0, 0, 0, 0 );
		CExtRichContentLayout::stat_DrawText(
			CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
			dc.m_hDC, LPCTSTR(sMeasureText), &rcTextMeasure, DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT, 0
			);
		if( bHorz )
		{
			_size.cx +=
				rcTextMeasure.Width()
				+ __EXT_TB_BUTTON_TEXT_MARGIN*2;
			_size.cy =
				max(
					_size.cy,
					rcTextMeasure.Height()
					);
		} // if( bHorz )
		else
		{
			_size.cx =
				max(
					_size.cx,
					rcTextMeasure.Width()
					);
			_size.cy +=
				rcTextMeasure.Height()
				//+ __EXT_TB_BUTTON_TEXT_MARGIN*2
				+ __EXT_TB_BUTTON_TEXT_MARGIN
				;
		} // else from if( bHorz )
	} // else from if( sMeasureText.IsEmpty() )
	// update size for slider control
	if( bHorz )
	{
		INT nExtentCtrl = ScrollControlExtentHorzGet();
		ASSERT( nExtentCtrl >= 0 );
		_size.cx += nExtentCtrl;
		if( bTextEmpty && bIconEmpty )
			_size.cy += __EXT_TB_BUTTON_OUTER_MARGIN*2-2;
	} // if( bHorz )
	else
	{
		INT nExtentCtrl = ScrollControlExtentVertGet();
		ASSERT( nExtentCtrl >= 0 );
		_size.cy += nExtentCtrl;
		if( bTextEmpty && bIconEmpty )
			_size.cx += __EXT_TB_BUTTON_OUTER_MARGIN*2-2;
	} // else from if( bHorz )
	if( !( bTextEmpty && bIconEmpty ) )
	{
		_size.cx += __EXT_TB_BUTTON_OUTER_MARGIN*2;
		_size.cy += __EXT_TB_BUTTON_OUTER_MARGIN*2;
		if( m_ActiveSize.cx < _size.cx )
			m_ActiveSize.cx = _size.cx;
		if( m_ActiveSize.cy < _size.cy )
			m_ActiveSize.cy = _size.cy;
	} // if( !( bTextEmpty && bIconEmpty ) )
	m_ActiveSize = _size;
	return m_ActiveSize;
}

void CExtBarSliderButton::RecalcSliderLayout()
{
	ASSERT_VALID( this );

	GetBar()->AnimationSite_ClientProgressStop(
		this
		);
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();

CRect rcAll = Rect();
CExtToolControlBar * pBar = GetBar();
bool bHorz = pBar->IsDockedVertically() ? false : true;
	if( GetNoRotateVerticalLayout() )
		bHorz = true;
INT nExtentCtrl = bHorz
		? ScrollControlExtentHorzGet()
		: ScrollControlExtentVertGet();
	m_arrAreaRects[__ESA_ICON_AND_TEXT] = rcAll;
	m_arrAreaRects[__ESA_SLIDER] = rcAll;
	m_arrAreaRects[__ESA_ICON_AND_TEXT].DeflateRect(
		0,
		0,
		bHorz ? nExtentCtrl : 0,
		bHorz ? 0 : nExtentCtrl
		);
	m_arrAreaRects[__ESA_SLIDER].DeflateRect(
		bHorz ? m_arrAreaRects[__ESA_ICON_AND_TEXT].Width() : 0,
		bHorz ? 0 : m_arrAreaRects[__ESA_ICON_AND_TEXT].Height(),
		0,
		0
		);
INT nExtentBtn = bHorz
		? ScrollButtonExtentHorzGet()
		: ScrollButtonExtentVertGet();
	m_arrAreaRects[__ESA_SCROLLABLE] = m_arrAreaRects[__ESA_SLIDER];
	m_arrAreaRects[__ESA_LINE_UP] = m_arrAreaRects[__ESA_SLIDER];
	m_arrAreaRects[__ESA_LINE_DOWN] = m_arrAreaRects[__ESA_SLIDER];
	m_arrAreaRects[__ESA_THUMB].SetRect( 0, 0, 0, 0 );
	m_arrAreaRects[__ESA_PAGE_UP].SetRect( 0, 0, 0, 0 );
	m_arrAreaRects[__ESA_PAGE_DOWN].SetRect( 0, 0, 0, 0 );
ULONG nScrollPos = ScrollPosGet();
ULONG nScrollPageSize = ScrollPageSizeGet();
ULONG nScrollTotalRange = ScrollTotalRangeGet();
ULONG nScrollLimit = ScrollLimitGet();
ULONG nExtentMD = 0L;
INT nExtentThumb = 0;
	if( nScrollLimit > 0L )
	{
		if( nScrollPageSize == 0 )
		{
			nExtentThumb = nExtentBtn;
			if( nExtentThumb == 0 )
				nExtentThumb = 6;
		} // if( nScrollPageSize == 0 )
		else
		{
			if( nScrollTotalRange > nScrollPageSize )
			{
				nExtentMD = nScrollTotalRange/nScrollPageSize;
			} // if( nScrollTotalRange > nScrollPageSize )
		} // else from if( nScrollPageSize == 0 )
	} // if( nScrollLimit > 0L )
	if( bHorz )
	{
		m_arrAreaRects[__ESA_SCROLLABLE].DeflateRect( nExtentBtn, 0 );
		m_arrAreaRects[__ESA_LINE_UP].right = m_arrAreaRects[__ESA_LINE_UP].left + nExtentBtn;
		m_arrAreaRects[__ESA_LINE_DOWN].left = m_arrAreaRects[__ESA_LINE_DOWN].right - nExtentBtn;
		if( m_arrAreaRects[__ESA_SCROLLABLE].Width() > 5 )
		{
			if( nExtentThumb == 0 && nExtentMD != 0L )
				nExtentThumb =
					::MulDiv( m_arrAreaRects[__ESA_SCROLLABLE].Width(), nScrollPageSize, nScrollTotalRange );
			if( nExtentThumb != 0 && nExtentThumb < 6 )
				nExtentThumb = 6;
			if( nExtentThumb != 0 )
			{
				m_arrAreaRects[__ESA_THUMB] = m_arrAreaRects[__ESA_SCROLLABLE];
				m_arrAreaRects[__ESA_THUMB].right = m_arrAreaRects[__ESA_THUMB].left + nExtentThumb;
				if( nScrollLimit > 0 )
				{
					INT nRestSpace = m_arrAreaRects[__ESA_SCROLLABLE].Width() - m_arrAreaRects[__ESA_THUMB].Width();
					ASSERT( nRestSpace > 0 );
					INT nOffset = ::MulDiv( nRestSpace, nScrollPos, nScrollLimit );
					m_arrAreaRects[__ESA_THUMB].OffsetRect( nOffset, 0 );
					if( nScrollPos > 0 )
					{
						m_arrAreaRects[__ESA_PAGE_UP] = m_arrAreaRects[__ESA_SCROLLABLE];
						m_arrAreaRects[__ESA_PAGE_UP].right = m_arrAreaRects[__ESA_THUMB].left;
					}
					if( nScrollPos < nScrollLimit )
					{
						m_arrAreaRects[__ESA_PAGE_DOWN] = m_arrAreaRects[__ESA_SCROLLABLE];
						m_arrAreaRects[__ESA_PAGE_DOWN].left = m_arrAreaRects[__ESA_THUMB].right;
					}
				} // if( nScrollLimit > 0 )
			} // if( nExtentThumb != 0 )
		} // if( m_arrAreaRects[__ESA_SCROLLABLE].Width() > 5 )
	} // if( bHorz )
	else
	{
		m_arrAreaRects[__ESA_SCROLLABLE].DeflateRect( 0, nExtentBtn );
		m_arrAreaRects[__ESA_LINE_UP].bottom = m_arrAreaRects[__ESA_LINE_UP].top + nExtentBtn;
		m_arrAreaRects[__ESA_LINE_DOWN].top = m_arrAreaRects[__ESA_LINE_DOWN].bottom - nExtentBtn;
		if( m_arrAreaRects[__ESA_SCROLLABLE].Height() > 5 )
		{
			if( nExtentThumb == 0 && nExtentMD != 0L )
				nExtentThumb =
					::MulDiv( m_arrAreaRects[__ESA_SCROLLABLE].Height(), nScrollPageSize, nScrollTotalRange );
			if( nExtentThumb != 0 && nExtentThumb < 6 )
				nExtentThumb = 6;
			if( nExtentThumb != 0 )
			{
				m_arrAreaRects[__ESA_THUMB] = m_arrAreaRects[__ESA_SCROLLABLE];
				m_arrAreaRects[__ESA_THUMB].bottom = m_arrAreaRects[__ESA_THUMB].top + nExtentThumb;
				if( nScrollLimit > 0 )
				{
					INT nRestSpace = m_arrAreaRects[__ESA_SCROLLABLE].Height() - m_arrAreaRects[__ESA_THUMB].Height();
					ASSERT( nRestSpace > 0 );
					INT nOffset = ::MulDiv( nRestSpace, nScrollPos, nScrollLimit );
					m_arrAreaRects[__ESA_THUMB].OffsetRect( 0, nOffset );
					if( nScrollPos > 0 )
					{
						m_arrAreaRects[__ESA_PAGE_UP] = m_arrAreaRects[__ESA_SCROLLABLE];
						m_arrAreaRects[__ESA_PAGE_UP].bottom = m_arrAreaRects[__ESA_THUMB].top;
					}
					if( nScrollPos < nScrollLimit )
					{
						m_arrAreaRects[__ESA_PAGE_DOWN] = m_arrAreaRects[__ESA_SCROLLABLE];
						m_arrAreaRects[__ESA_PAGE_DOWN].top = m_arrAreaRects[__ESA_THUMB].bottom;
					}
				} // if( nScrollLimit > 0 )
			} // if( nExtentThumb != 0 )
		} // if( m_arrAreaRects[__ESA_SCROLLABLE].Height() > 5 )
	} // else from if( bHorz )
}

void CExtBarSliderButton::PaintCompound(
	CDC & dc,
	bool bPaintParentChain,
	bool bPaintChildren,
	bool bPaintOneNearestChildrenLevelOnly
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( ! IsSeparator() );

	if( ! IsPaintAble( dc ) )
		return;
	if( AnimationClient_StatePaint( dc ) )
		return;
	if( bPaintParentChain )
		PaintParentChain( dc );

	if(		( ! IsVisible() )
		||	( GetStyle() & TBBS_HIDDEN)  != 0
		||	( ! dc.RectVisible( &m_ActiveRect ) )
		)
		return;
CRect rcArea( m_ActiveRect );
bool bHorz = IsHorzBarOrientation();
INT nExtentCtrl = bHorz
		? ScrollControlExtentHorzGet()
		: ScrollControlExtentVertGet();
	rcArea.DeflateRect(
		0,
		0,
		bHorz ? nExtentCtrl : 0,
		bHorz ? 0 : nExtentCtrl
		);
	if(		rcArea.right > rcArea.left
		&&	rcArea.bottom > rcArea.top
		&&	dc.RectVisible( &rcArea )
		)
	{ // if icon/text area is not empty and visible
		CExtToolControlBar * pBar = GetBar();
		bool bDockSiteCustomizeMode =
			pBar->_IsDockSiteCustomizeMode();
		bool bPushed = false;
		//	( IsPressed() && (!bDockSiteCustomizeMode) )
		//		? true : false;
		bool bEnabled =
			( IsDisabled() && (!bDockSiteCustomizeMode) )
				? false : true;
		bool bHover = false;
		//	( IsHover() && (!bDockSiteCustomizeMode) )
		//		? true : false;
		//if( !bDockSiteCustomizeMode )
		//{
		//	if(		CExtToolControlBar::g_bMenuTracking
		//		||	CExtPopupMenuWnd::IsMenuTracking()
		//		)
		//		bHover = false;
		//	else if( !bHover )
		//		bHover = IsPressedTracking();
		//} // if( !bDockSiteCustomizeMode )
		bool bIndeterminate = false;
		//	( IsIndeterminate() && (!bDockSiteCustomizeMode) )
		//		? true : false;
		CExtSafeString sText = GetText();
		CExtCmdIcon * pIcon = GetIconPtr();
		CExtCmdIcon _iconLarge;
		if(		pIcon != NULL
			&&	(! pIcon->IsEmpty() )
			&&	IsLargeIcon()
			)
		{
			CSize _sizeIcon = pIcon->GetSize();
			_sizeIcon.cx *= 2;
			_sizeIcon.cy *= 2;
			if(	_iconLarge.CreateScaledCopy(
					*pIcon,
					_sizeIcon
					)
				)
				pIcon = &_iconLarge;
#ifdef _DEBUG
			else
			{
				ASSERT( FALSE );
			}
#endif // _DEBUG
		}
		HFONT hFont =
			(HFONT) GetBar()
				-> OnGetToolbarFont( ! bHorz, false, this )
				-> GetSafeHandle();
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
			this,
			bHorz,
			rcArea,
			sText,
			pIcon,
			true,
			bHover,
			bPushed,
			bIndeterminate,
			bEnabled,
			true,
			false,
			false,
			0,
			hFont, // NULL // (+ 2.24)
			(		IsAbleToTrackMenu()
				//&&( ! pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
				&&	( ! pBar->_IsSimplifiedDropDownButtons() )
				)	? true : false,
			0,
			(!bEnabled) || ( bEnabled && (!bHover) && (!bPushed) )
			);

		_ppbd.m_rcBorderSizes = OnQueryBorderSizes( bHorz );
		_ppbd.m_nIconAlignment = OnQueryAlignmentIcon( bHorz );
		_ppbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
		_ppbd.m_rcIconMargins = OnQueryMarginsIcon( bHorz );
		_ppbd.m_rcTextMargins = OnQueryMarginsText( bHorz );

		//if( GetSeparatedDropDown() )
		//{
		//	_ppbd.m_bSeparatedDropDown = true;
		//	if(		( m_bDropDownHT /*|| CExtToolControlBar::g_bMenuTracking*/ )
		//		&&	bPushed
		//		&&	( ! bDockSiteCustomizeMode )
		//		)
		//	{
		//		//_ppbd.m_bPushed = false;
		//		_ppbd.m_bPushedDropDown = true;
		//	}
		//}

		bool bChecked =
			( (GetStyle()&TBBS_CHECKED) != 0 ) 
				? true 
				: false;
		_ppbd.m_bChecked = bChecked;

		pBar->PmBridge_GetPM()->PaintPushButton( dc, _ppbd );
	} // if icon/text area is not empty and visible

	
	if(		m_arrAreaRects[__ESA_SLIDER].right > m_arrAreaRects[__ESA_SLIDER].left
		&&	m_arrAreaRects[__ESA_SLIDER].bottom > m_arrAreaRects[__ESA_SLIDER].top
		&&	dc.RectVisible( &m_arrAreaRects[__ESA_SLIDER] )
		)
	{ // if slider area is not empty and visible
		for(	int nRectNo = __ESA_PAINT_RANGE_FIRST;
				nRectNo <= __ESA_PAINT_RANGE_LAST;
				nRectNo++
				)
		{ // walk and paint all areas
			if(		m_arrAreaRects[nRectNo].right <= m_arrAreaRects[nRectNo].left
				||	m_arrAreaRects[nRectNo].bottom <= m_arrAreaRects[nRectNo].top
				||	( ! dc.RectVisible( &m_arrAreaRects[nRectNo] ) )
				)
				continue;
			OnPaintScrollArea(
				dc,
				(CExtBarSliderButton::e_SliderArea_t)nRectNo,
				bHorz
				);
		} // walk and paint all areas
	} // if slider area is not empty and visible

	rcArea = m_ActiveRect;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtToolControlBar * pBar = GetBar();
	CExtCustomizeSite * pSite = pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		&&	pSite->CustomizedNodeGet() != NULL
		&&	pSite->CustomizedNodeGet() == GetCmdNode( false )
		)
	{
		pBar->PmBridge_GetPM()->PaintDragSourceRect(
			dc,
			rcArea
			);
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( bPaintChildren )
		PaintChildren( dc, bPaintOneNearestChildrenLevelOnly );
}

void CExtBarSliderButton::OnPaintScrollArea(
	CDC & dc,
	CExtBarSliderButton::e_SliderArea_t eSA,
	bool bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	ASSERT( 0 <= eSA && eSA < __ESA_COUNT );
CRect rcArea = GetAreaRect( eSA );
	if(		rcArea.right < rcArea.left
		||	rcArea.bottom < rcArea.top
		||	( ! dc.RectVisible(&rcArea) )
		)
		return;
bool bFlatButton = false;
bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
bool bEnabled =
		(	IsDisabled()
		&& (!bDockSiteCustomizeMode)
		)
		? false : true;
	if( bEnabled )
	{
		ULONG nScrollPageSize = ScrollPageSizeGet();
		ULONG nScrollTotalRange = ScrollTotalRangeGet();
		if(		nScrollPageSize >= nScrollTotalRange
			||	nScrollTotalRange == 0L
			)
			bEnabled = false;
	} // if( bEnabled )
bool bHover =
		(	IsHover()
		&&	bEnabled
		&&	(!bDockSiteCustomizeMode)
		)
		? true : false;
bool bPushed =
		(	GetPressedArea() == eSA
		&&	bEnabled
		&&	(!bDockSiteCustomizeMode)
		)
		? true : false;
	if( !bDockSiteCustomizeMode )
	{
		if(		CExtToolControlBar::g_bMenuTracking
			||	CExtPopupMenuWnd::IsMenuTracking()
			)
			bHover = false;
		else if( !bHover )
			bHover = IsPressedTracking();
	} // if( !bDockSiteCustomizeMode )
CExtPaintManager::glyph_t * pBtnGlyph = NULL;
bool bPaintAsButton = false;
INT nExtentBtn = bHorz
		? ScrollButtonExtentHorzGet()
		: ScrollButtonExtentVertGet();
	switch( eSA )
	{
	case __ESA_LINE_UP:
		bPaintAsButton = true;
		pBtnGlyph = bHorz
			? CExtPaintManager::g_DockingCaptionGlyphs[
				CExtPaintManager::__DCBT_ARROW_LEFT ]
			: CExtPaintManager::g_DockingCaptionGlyphs[
				CExtPaintManager::__DCBT_ARROW_UP ];
		bFlatButton = bPushed;
//		bPushed = false;
	break;
	case __ESA_LINE_DOWN:
		bPaintAsButton = true;
		pBtnGlyph = bHorz
			? CExtPaintManager::g_DockingCaptionGlyphs[
				CExtPaintManager::__DCBT_ARROW_RIGHT ]
			: CExtPaintManager::g_DockingCaptionGlyphs[
				CExtPaintManager::__DCBT_ARROW_DOWN ];
		bFlatButton = bPushed;
//		bPushed = false;
	break;
	case __ESA_THUMB:
		bPaintAsButton = true;
		//bFlatButton = bPushed;
		bPushed = false;
	break;
	case __ESA_SCROLLABLE:
		if( nExtentBtn != 0 )
		{
			dc.FillRect(
				rcArea,
				(bHover && bEnabled)
					? ( & pBar->PmBridge_GetPM()->m_brushLightestDefault )
					: ( & pBar->PmBridge_GetPM()->m_brushLightDefault )
				);
			rcArea.InflateRect(
				bHorz ? 1 : 0,
				bHorz ? 0 : 1
				);
		} // if( nExtentBtn != 0 )
		else
		{
			rcArea.DeflateRect(
				bHorz ? 0 : (rcArea.Width()/2),
				bHorz ? (rcArea.Height()/2) : 0
				);
			rcArea.InflateRect(
				bHorz ? 0 : 1,
				bHorz ? 1 : 0
				);
			if( bEnabled && bHover )
			{
				dc.FillSolidRect( &rcArea, pBar->PmBridge_GetPM()->GetColor( COLOR_3DDKSHADOW, this ) );
				rcArea.DeflateRect( 1, 1 );
				dc.FillRect( &rcArea, &pBar->PmBridge_GetPM()->m_brushLightestDefault );
			} // if( bEnabled && bHover )
			else
			{
				dc.FillRect( &rcArea, &pBar->PmBridge_GetPM()->m_brushDarkestDefault );
				rcArea.DeflateRect( 1, 1 );
				dc.FillRect( &rcArea, &pBar->PmBridge_GetPM()->m_brushLighterDefault );
			} // else from if( bEnabled && bHover )
		} // else from if( nExtentBtn != 0 )
		return;
	case __ESA_PAGE_UP:
	case __ESA_PAGE_DOWN:
		if( nExtentBtn != 0 )
		{
			if( bHover || bPushed )
				dc.FillRect(
					rcArea,
					bPushed
						? &pBar->PmBridge_GetPM()->m_brushDarkestDefault
						: &pBar->PmBridge_GetPM()->m_brushLightestDefault
					);
		} // if( nExtentBtn != 0 )
		return;
	} // switch( eSA )
	if( bPaintAsButton )
	{
		CExtPaintManager * pPM = pBar->PmBridge_GetPM();
		ASSERT_VALID( pPM );

#ifndef __EXT_MFC_NO_GRIDWND
		COLORREF clrFace = pPM->GetColor( COLOR_3DFACE );
		COLORREF clrLight = pPM->GetColor( COLOR_3DHIGHLIGHT );
		COLORREF clrShadow = pPM->GetColor( COLOR_3DSHADOW );
		COLORREF clrWindow = pPM->GetColor( COLOR_WINDOW );
		COLORREF clrText = pPM->GetColor( COLOR_BTNTEXT );
		if( ! pPM->Grid_PaintButton(
				dc,
				rcArea,
				INT(CExtGridCell::__EBTT_ELLIPSIS),
				bPushed,
				bHover,
				false, // focused
				bEnabled,
				clrFace,
				clrLight,
				clrShadow,
				clrWindow,
				clrText,
				__EGCPF_NO_BUTTON_ELLIPSIS_DOTS,
				pBar, // (CObject*) this
				0L
				)
			)
#endif // __EXT_MFC_NO_GRIDWND
		{
			bool bIndeterminate =
				( IsIndeterminate() && (!bDockSiteCustomizeMode) )
					? true : false;
			CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
				this,
				bHorz,
				rcArea,
				_T(""),
				NULL,
				bFlatButton,
				bHover,
				bPushed,
				bIndeterminate,
				bEnabled,
				!bFlatButton,
				false,
				false,
				0,
				NULL,
				false,
				0,
				false
				);

			_ppbd.m_rcBorderSizes = OnQueryBorderSizes( bHorz );
			_ppbd.m_nIconAlignment = OnQueryAlignmentIcon( bHorz );
			_ppbd.m_nTextAlignment = OnQueryAlignmentText( bHorz );
			_ppbd.m_rcIconMargins = OnQueryMarginsIcon( bHorz );

			_ppbd.m_rcTextMargins = OnQueryMarginsText( bHorz );

			bool bChecked =
				( (GetStyle()&TBBS_CHECKED) != 0 ) 
					? true 
					: false;
			_ppbd.m_bChecked = bChecked;
			
			_ppbd.m_lParam = eSA; // flag, which specifies what the part of the slider we are drawing
			pPM->PaintPushButton( dc, _ppbd );
		}
	
		if( pBtnGlyph != NULL )
		{
			if( bPushed )
				rcArea.OffsetRect(
					pBar->PmBridge_GetPM()->GetPushedOffset()
					);
			COLORREF ColorValues[] =
			{
				RGB(0,0,0),
				pBar->PmBridge_GetPM()->GetColor(
					bEnabled
						? (
							(bHover || bPushed)
								? CExtPaintManager::CLR_TEXT_IN
								: CExtPaintManager::CLR_TEXT_OUT
							)
						: CExtPaintManager::CLR_TEXT_DISABLED
						,
					this
					)
			};
			pBar->PmBridge_GetPM()->PaintGlyphCentered(
				dc,
				rcArea,
				*pBtnGlyph,
				ColorValues
				);
		} // if( pBtnGlyph != NULL )
		return;
	} // if( bPaintAsButton )
}

CExtBarSliderButton::e_SliderArea_t
	CExtBarSliderButton::GetTrackedArea() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	return m_eAreaTracked;
}

CExtBarSliderButton::e_SliderArea_t
	CExtBarSliderButton::GetPressedArea() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	return m_eAreaPressed;
}

CRect CExtBarSliderButton::GetAreaRect(
	CExtBarSliderButton::e_SliderArea_t eSA
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	ASSERT( 0 <= eSA && eSA < __ESA_COUNT );
	return m_arrAreaRects[eSA];
}

UINT CExtBarSliderButton::OnTrackPopup(
	CPoint point,
	bool bSelectAny,
	bool bForceNoAnimation
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	point;
	bSelectAny;
	bForceNoAnimation;
	return UINT(-1L);
}

void CExtBarSliderButton::OnHover(
	CPoint point,
	bool bOn,
	bool & bSuspendTips
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	bSuspendTips = false;
bool bDockSiteCustomizeMode =
		m_pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;
	CExtBarButton::OnHover(
		point,
		bOn,
		bSuspendTips
		);
}

void CExtBarSliderButton::OnClick(
	CPoint point,
	bool bDown
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonClick( this, point, bDown ) )
		return;
bool bDockSiteCustomizeMode = pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;
	CExtBarButton::OnClick( point, bDown );
	if( pBar->m_pDockSite != NULL )
	{
		if( pBar->m_pDockSite->m_bHelpMode )
			return;
	} // if( m_pBar->m_pDockSite != NULL )

	m_eAreaTracked = m_eAreaPressed = __ESA_UNKNOWN_OR_NOWHERE;
	if( !bDown )
	{
		RedrawButton( true );
		return;
	}
ULONG nScrollLimit = ScrollLimitGet();
	if( nScrollLimit == 0L )
	{
		RedrawButton( true );
		return;
	}
ULONG nScrollPos = ScrollPosGet();
ULONG nScrollPosStart = nScrollPos;
	for(	int nRectNo = __ESA_CLICKABLE_RANGE_FIRST;
			nRectNo <= __ESA_CLICKABLE_RANGE_LAST;
			nRectNo++
			)
	{
		if(		m_arrAreaRects[nRectNo].right <= m_arrAreaRects[nRectNo].left
			||	m_arrAreaRects[nRectNo].bottom <= m_arrAreaRects[nRectNo].top
			)
			continue;
		if( m_arrAreaRects[nRectNo].PtInRect(point) )
		{
			m_eAreaTracked = (e_SliderArea_t)nRectNo;
			break;
		}
	}
	RedrawButton( true );
	if( m_eAreaTracked == __ESA_UNKNOWN_OR_NOWHERE )
		return;
bool bHorz = pBar->IsDockedVertically() ? false : true;
	if( GetNoRotateVerticalLayout() )
		bHorz = true;
e_SliderArea_t eSA = m_eAreaTracked;
CRect rcArea = GetAreaRect( eSA );
const UINT nTimerID = 401;
const UINT nTimerEllapse = 50;
HWND hWndOwnBar = m_pBar->GetSafeHwnd();
	ASSERT( hWndOwnBar != NULL );
bool bStopFlag = false;
CPoint ptCursor( point );
ULONG nStepSize = 0L;
bool bUpStep = false;
	switch( eSA )
	{
	case __ESA_LINE_UP:
		bUpStep = true;
	case __ESA_LINE_DOWN:
		nStepSize = 1L;
		break;
	case __ESA_PAGE_UP:
		bUpStep = true;
	case __ESA_PAGE_DOWN:
		nStepSize = ScrollPageSizeGet();
		if( nStepSize == 0L )
			nStepSize = 1L;
		break;
	case __ESA_THUMB:
		m_eAreaPressed = eSA;
		RedrawButton();
		break;
	} // switch( eSA )
	if( nStepSize > nScrollLimit )
		nStepSize = nScrollLimit;
	::SetCapture( hWndOwnBar );
	if( nStepSize != 0L )
		::SetTimer( hWndOwnBar, nTimerID, nTimerEllapse, NULL );
	for( MSG msg; ::IsWindow( hWndOwnBar ) && (!bStopFlag); )
	{
		if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if( ! ::IsWindow( hWndOwnBar ) )
				break;
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )

		bool bAnalyzeThumb = false;
		switch( msg.message )
		{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != hWndOwnBar )
				bStopFlag = true;
		break;
		case WM_MOUSEMOVE:
			if( eSA == __ESA_THUMB )
			{
				if(		( ! CExtPopupMenuWnd::IsKeyPressed(VK_LBUTTON) )
					||	CExtPopupMenuWnd::IsKeyPressed(VK_MBUTTON)
					||	CExtPopupMenuWnd::IsKeyPressed(VK_RBUTTON)
					||	CExtPopupMenuWnd::IsMenuTracking()
					)
				{
					bStopFlag = true;
					break;
				}
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				bAnalyzeThumb = true;
				::GetCursorPos( &ptCursor );
				::ScreenToClient( hWndOwnBar, &ptCursor );
				break;
			} // if( eSA == __ESA_THUMB )
			if( nStepSize == 0 )
				break;
		case WM_TIMER:
			if(		( ! CExtPopupMenuWnd::IsKeyPressed(VK_LBUTTON) )
				||	CExtPopupMenuWnd::IsKeyPressed(VK_MBUTTON)
				||	CExtPopupMenuWnd::IsKeyPressed(VK_RBUTTON)
				||	CExtPopupMenuWnd::IsMenuTracking()
				)
			{
				bStopFlag = true;
				break;
			}
			if( msg.message == WM_TIMER )
			{
				if( msg.wParam != nTimerID )
					break;
			} // if( msg.message == WM_TIMER )
			ASSERT( nStepSize != 0L );
			PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
			::GetCursorPos( &ptCursor );
			::ScreenToClient( hWndOwnBar, &ptCursor );
			if( ! rcArea.PtInRect(ptCursor) )
			{
				if( m_eAreaPressed != __ESA_UNKNOWN_OR_NOWHERE )
				{
					m_eAreaPressed = __ESA_UNKNOWN_OR_NOWHERE;
					RedrawButton( true );
				}
				continue;
			}
			if( bUpStep )
			{
				if( nScrollPos == 0L )
				{
					nScrollPos = ScrollPosGet();
					nScrollLimit = ScrollLimitGet();
					break;
				}
				if( nScrollPos < nStepSize )
					nScrollPos = 0L;
				else
					nScrollPos -= nStepSize;
			} // if( bUpStep )
			else
			{
				if( nScrollPos >= nScrollLimit )
				{
					nScrollPos = ScrollPosGet();
					nScrollLimit = ScrollLimitGet();
					break;
				}
				if( nStepSize > (nScrollLimit-nScrollPos) )
					nScrollPos = nScrollLimit;
				else
					nScrollPos += nStepSize;
				ASSERT( nScrollPos <= nScrollLimit );
			} // else from if( bUpStep )
			if( ScrollPosGet() != nScrollPos )
			{
				ScrollPosSet( nScrollPos );
				nScrollPos = ScrollPosGet();
				nScrollLimit = ScrollLimitGet();
				RecalcSliderLayout();
				RedrawButton( true );
			}
			rcArea = GetAreaRect( eSA );
			m_eAreaPressed = eSA;
		break;
		default:
			if(		( ! CExtPopupMenuWnd::IsKeyPressed(VK_LBUTTON) )
				||	CExtPopupMenuWnd::IsKeyPressed(VK_MBUTTON)
				||	CExtPopupMenuWnd::IsKeyPressed(VK_RBUTTON)
				||	CExtPopupMenuWnd::IsMenuTracking()
				)
				bStopFlag = true;
		break;
		} // switch( msg.message )
		if( bStopFlag || nScrollLimit == 0L )
			break;
		if( bAnalyzeThumb )
		{
			LONG nPixelOffset = bHorz
				? (ptCursor.x - point.x)
				: (ptCursor.y - point.y);
			LONG nPixelExtent = bHorz
				? (m_arrAreaRects[__ESA_SCROLLABLE].Width() - m_arrAreaRects[__ESA_THUMB].Width())
				: (m_arrAreaRects[__ESA_SCROLLABLE].Height() - m_arrAreaRects[__ESA_THUMB].Height());
			if( nPixelExtent <= 0 )
			{
				bStopFlag = true;
				break;
			} // if( nPixelExtent <= 0 )
			if( abs(nPixelOffset) > nPixelExtent )
				nPixelOffset =
					(nPixelOffset < 0)
						? (-nPixelExtent)
						: nPixelExtent;
			ULONG nShift =
				( nPixelExtent == 0 || nPixelOffset == 0 )
					? 0
					: ::MulDiv( nScrollLimit, abs(nPixelOffset), nPixelExtent );
			nScrollPos = nScrollPosStart;
			if( nPixelOffset < 0 )
			{
				if( nShift >= nScrollPos )
					nScrollPos = 0;
				else
					nScrollPos -= nShift;
			} // if( nPixelOffset < 0 )
			else
			{
				if( nScrollPos > nScrollLimit )
				{
					nScrollPos = ScrollPosGet();
					nScrollLimit = ScrollLimitGet();
					break;
				}
				if( nShift > (nScrollLimit-nScrollPos) )
					nScrollPos = nScrollLimit;
				else
					nScrollPos += nShift;
				ASSERT( nScrollPos <= nScrollLimit );
			} // else from if( nPixelOffset < 0 )
			if( ScrollPosGet() != nScrollPos )
			{
				ScrollPosSet( nScrollPos );
				nScrollPos = ScrollPosGet();
				nScrollLimit = ScrollLimitGet();
				RecalcSliderLayout();
				RedrawButton( true );
			} 
			rcArea = GetAreaRect( eSA );
			m_eAreaPressed = eSA;
			continue;
		} // if( bAnalyzeThumb )
		if( !AfxGetThread()->PumpMessage() )
			break;
	} // for( MSG msg; ::IsWindow( hWndOwnBar ) && (!bStopFlag); )
	if( ! ::IsWindow( hWndOwnBar ) )
		return;
	if( nStepSize != 0L )
		::KillTimer( hWndOwnBar, nTimerID );
	m_eAreaTracked = m_eAreaPressed = __ESA_UNKNOWN_OR_NOWHERE;
	RedrawButton( true );
	if( ::GetCapture() == hWndOwnBar )
		::ReleaseCapture();
}

void CExtBarSliderButton::OnDeliverCmd()
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonDeliverCmd( this ) )
		return;
	//CExtBarButton::OnDeliverCmd();
	if( pBar->OnDeliverCmd( this ) )
		return;
}

__EXT_MFC_INT_PTR CExtBarSliderButton::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	return CExtBarButton::OnToolHitTest( point, pTI );
}

LRESULT CExtBarSliderButton::OnHelpHitTest(
	CPoint point
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	return CExtBarButton::OnHelpHitTest( point );
}

void CExtBarSliderButton::OnUpdateCmdUI(
	CWnd * pTarget,
	BOOL bDisableIfNoHndler,
	int nIndex
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	CExtBarButton::OnUpdateCmdUI(
		pTarget,
		bDisableIfNoHndler,
		nIndex
		);
}

bool CExtBarSliderButton::OnSetCursor( CPoint point )
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	return CExtBarButton::OnSetCursor( point );
}

/////////////////////////////////////////////////////////////////////////////
// CExtToolControlBar

const UINT CExtToolControlBar::g_nMsgTrackButtonMenu =
	::RegisterWindowMessage(
		_T("CExtToolControlBar::g_nMsgTrackButtonMenu")
		);
const UINT CExtToolControlBar::g_nMsgContinueTrackMenu =
	::RegisterWindowMessage(
		_T("CExtToolControlBar::g_nMsgContinueTrackMenu")
		);

IMPLEMENT_DYNCREATE( CExtToolControlBar, CExtControlBar )

CExtBarButton * CExtToolControlBar::_GetButtonPtr(int nIndex) const
{
__EXT_MFC_INT_PTR nButtonsCount = m_buttons.GetSize();
	if( !( nIndex >= 0 && nIndex < nButtonsCount ) )
		return NULL;
CExtBarButton * pTBB =
		m_buttons[nIndex];
	ASSERT_VALID( pTBB );
	return pTBB;
}

void CExtToolControlBar::_RemoveAllButtonsImpl()
{
INT nAcIndex, nAcCount = AnimationSite_ClientGetCount();
	for( nAcIndex = 0; nAcIndex < nAcCount; nAcIndex ++ )
	{
		CExtAnimationClient * pAC = AnimationSite_ClientGetAt( nAcIndex );
		AnimationSite_ClientProgressStop( pAC );
	}
	if( GetMenuTrackingButton() != NULL )
	{
		_CloseTrackingMenus();
		CExtPopupMenuWnd::CancelMenuTracking();
	}
	if( m_pRightBtn != NULL )
		m_pRightBtn->GetButtons().RemoveAll();
CList < CExtBarButton *, CExtBarButton * > _listButtonsToDelete;
	for( INT nBtnIdx = 0; nBtnIdx < m_buttons.GetSize(); nBtnIdx++ )
	{
		CExtBarButton * pTBB = m_buttons[nBtnIdx];
		ASSERT( pTBB != NULL );
		AnimationSite_ClientRemove( pTBB );
		_listButtonsToDelete.AddTail( pTBB );
	} // for( INT nBtnIdx = 0; nBtnIdx < m_buttons.GetSize(); nBtnIdx++ )
	m_buttons.RemoveAll();
POSITION pos = _listButtonsToDelete.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtBarButton * pTBB = _listButtonsToDelete.GetNext( pos );
		ASSERT( pTBB != NULL );
		delete pTBB;
	}
	_listButtonsToDelete.RemoveAll();
	m_pRightBtn = NULL;
	m_nBtnIdxCapture = -1;      // nothing captured
	m_nBtnIdxHover = -1;
	m_nBtnIdxMenuTracking = -1;
}

int CExtToolControlBar::_GetButtonsCountImpl() const
{
int nCountOfButtons = (int)m_buttons.GetSize();
	return nCountOfButtons;
}

void CExtToolControlBar::_RedrawOnPosChanged()
{
	ASSERT_VALID( this );
	Invalidate();
	UpdateWindow();
}

void CExtToolControlBar::_SetCursor(
	const CPoint & point
	)
{
	ASSERT_VALID( this );
int nBtnIdx = _HitTestImpl(point);
	if( nBtnIdx >= 0 )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		if( pTBB->OnSetCursor(point) )
			return;
	} // if( nBtnIdx >= 0 )
	CExtControlBar::_SetCursor( point );
}

bool CExtToolControlBar::g_bMenuTracking = false;
bool CExtToolControlBar::g_bMenuTrackingExpanded = false;
bool CExtToolControlBar::g_bToolbarLargeIcons = false;
bool CExtToolControlBar::g_bToolbarScreenTips = true;
bool CExtToolControlBar::g_bToolbarShortcutKeysOnScreenTips = false;
bool CExtToolControlBar::g_bEnableDblClickToggleDocking = false;

CExtToolControlBar::CExtToolControlBar()
{
	m_bGripperStaticallyAtTop = false;

	m_bForceBalloonGradientInDialogs = false;
	m_bForceNoBalloonWhenRedockable = false;

	m_pRightBtn = NULL;
	m_bRightButtonDisplayBarsList = true;
	m_bPaletteMode = false;
	m_bHelperTempFullRowMode = false;
	m_bInsideCalcLayout = false;

	m_bFixedMode = true;
	m_bReposSingleChildMode = false;
	m_nBtnIdxCapture = -1;
	m_nBtnIdxHover = -1;
	m_nBtnIdxMenuTracking = -1;
	m_nUpdateCtrlLockCount = 0;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_bCustomizationAllowed = true;
	m_pCustomizeSite = NULL;
	m_bAutoShowOnCustomizationStart = false;
	m_bAutoHideOnCustomizationEnd = false;
	m_bVisibleInCustomizeListBox = true;
	m_pDragSourceTBB = NULL;
	m_pDragTargetTBB = NULL;
	m_nDragTargetShift = 0;
	m_bHelperSeparatorAction = false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	m_cxLeftBorder		= 1;
	m_cxRightBorder		= 1;
	m_cyTopBorder		= 0;
	m_cyBottomBorder	= 0;
	m_nGripWidthAtLeft	= 8;
	m_nGripHeightAtTop	= 10;

	m_bHelperFlatTrackingCalcEnabledLeftRight = false;
	m_bHelperFlatTrackingCalcEnabledUpDown = false;
	m_bHelperFlatTrackingCalcEnabledTab = false;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_bKeyTipsDisplayed = false;
	m_bHelperKeyTipsSupported = false;
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)

	m_bLastDropDownHT = false;
	m_ptLastClientHover.x = m_ptLastClientHover.y = -32767;

	m_bEnableDblClickToggleDocking = g_bEnableDblClickToggleDocking;
}

bool CExtToolControlBar::_AdjustBGInfo()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() != NULL )
	{
		CRect rcBorders( 0, 0, 0, 0 );
		if( PmBridge_GetPM()->Toolbar_GetBGInfo(
				rcBorders,
				m_nGripWidthAtLeft,
				m_nGripHeightAtTop,
				this
				)
			)
		{
			DWORD dwBarStyle = GetBarStyle();
			if( (dwBarStyle&CBRS_GRIPPER) == 0 )
			{
				m_nGripWidthAtLeft = 0;
				m_nGripHeightAtTop = 0;
			} // if( (GetBarStyle()&CBRS_GRIPPER) != 0 )
			m_cxLeftBorder		= rcBorders.left;
			m_cxRightBorder		= rcBorders.right;
			m_cyTopBorder		= rcBorders.top;
			m_cyBottomBorder	= rcBorders.bottom;
			CRect rcGrip( 0, 0, 0, 0 );
			if( (dwBarStyle&CBRS_FLOATING) == 0 )
			{
				GetWindowRect( &rcGrip );
				rcGrip.OffsetRect( -rcGrip.TopLeft() );
				rcGrip.DeflateRect(
					rcBorders.left,
					rcBorders.top,
					rcBorders.right,
					rcBorders.bottom
					);
				if( (dwBarStyle&CBRS_ORIENT_HORZ) != 0 )
				{
					rcGrip.right = rcGrip.left;
					rcGrip.left -= m_nGripWidthAtLeft;
				}
				else
				{
					rcGrip.bottom = rcGrip.top;
					rcGrip.top -= m_nGripHeightAtTop;
				}
			} // if( (dwBarStyle&CBRS_FLOATING) == 0 )
			_RectGripSet( rcGrip );
			return true;
		}
	} // if( GetSafeHwnd() != NULL )
	m_cxLeftBorder		= 1;
	m_cxRightBorder		= 1;
	m_cyTopBorder		= 0;
	m_cyBottomBorder	= 0;
	m_nGripWidthAtLeft	= 8;
	m_nGripHeightAtTop	= 10;
	return false;
}

CExtToolControlBar::~CExtToolControlBar()
{
	_RemoveAllButtonsImpl();

//CSingleLock _slCsCB( &g_csCB );
//	_slCsCB.Lock();
__EXT_MFC_INT_PTR nCountOfBars = g_AllBars.GetSize();
	for( __EXT_MFC_INT_PTR i = 0; i<nCountOfBars; i++ )
	{
		CExtControlBar * pBar = g_AllBars[i];
		ASSERT( pBar != NULL );
		if( pBar == this )
		{
			g_AllBars.RemoveAt( i );
			break;
		}
	}
//	_slCsCB.Unlock();
}

bool CExtToolControlBar::_OnHookButtonClick( CExtBarButton * pTBB, CPoint point, bool bDown )
	{ ASSERT_VALID( this ); ASSERT_VALID( pTBB ); pTBB; point; bDown; return false; }
bool CExtToolControlBar::_OnHookButtonRClick( CExtBarButton * pTBB, CPoint point, bool bDown )
	{ ASSERT_VALID( this ); ASSERT_VALID( pTBB ); pTBB; point; bDown; return false; }
bool CExtToolControlBar::_OnHookButtonDblClick( CExtBarButton * pTBB, CPoint point )
	{ ASSERT_VALID( this ); ASSERT_VALID( pTBB ); pTBB; point; return false; }
bool CExtToolControlBar::_OnHookButtonDeliverCmd( CExtBarButton * pTBB )
	{ ASSERT_VALID( this ); ASSERT_VALID( pTBB ); pTBB; return false; }

BOOL CExtToolControlBar::LoadToolBar(
	__EXT_MFC_SAFE_LPCTSTR  lpszResourceName,
	COLORREF clrTransparent // = RGB(192,192,192)
	)
{
LPUINT pCmdArray = NULL;
INT nCmdCount = 0;
	if( !g_CmdManager->UpdateFromToolBar(
			g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
			lpszResourceName,
			&pCmdArray,
			&nCmdCount,
			false,
			true,
			clrTransparent
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT( pCmdArray != NULL && nCmdCount > 0 );
BOOL bRetVal =  SetButtons( pCmdArray, nCmdCount );
	ASSERT( bRetVal );
	delete [] pCmdArray;
	return bRetVal;
}

bool CExtToolControlBar::_IsShowContentWhenDragging() const
{
//BOOL bDragShowContent = FALSE;
//    ::SystemParametersInfo(
//		SPI_GETDRAGFULLWINDOWS,
//		0,
//        &bDragShowContent,
//		0
//		);
//	return bDragShowContent ? true : false;

//	if( IsKindOf(RUNTIME_CLASS(CExtToolControlBar)) )
//		return true;
//	return false;

//	if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
//		return false;

	return true;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtToolControlBar::KeyTipsDisplayedGet() const
{
	ASSERT_VALID( this );
	return m_bKeyTipsDisplayed;
}

void CExtToolControlBar::KeyTipsDisplayedSet(
	bool bKeyTipsDisplayed // = true
	)
{
	ASSERT_VALID( this );
	if( ! m_bHelperKeyTipsSupported )
		m_bKeyTipsDisplayed = false;
	else
		m_bKeyTipsDisplayed = bKeyTipsDisplayed;
}

CExtCustomizeCmdKeyTip & CExtToolControlBar::KeyTipChainGet()
{
	ASSERT_VALID( this );
	return m_keyTipChain;
}

const CExtCustomizeCmdKeyTip & CExtToolControlBar::KeyTipChainGet() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtToolControlBar * > ( this ) )
		-> KeyTipChainGet();
}

void CExtToolControlBar::KeyTipChainSet(
	const CExtCustomizeCmdKeyTip & keyTipChain
	)
{
	ASSERT_VALID( this );
	m_keyTipChain = keyTipChain;
}

void CExtToolControlBar::KeyTipChainEmpty()
{
	ASSERT_VALID( this );
	KeyTipChainGet().KeyCodeRemoveAll();
}

INT CExtToolControlBar::KeyTipTranslate(
	DWORD dwKeyCode,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	bool bAutoInvokeAction,
	bool & bActionInvoked,
	bool & bContinueKeyTipMode
	)
{
	ASSERT_VALID( this );
	bActionInvoked = false;
int nBtnIdx, nCountOfButtons = _GetButtonsCountImpl();
	for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		if(	pTBB->OnKeyTipTranslate(
				dwKeyCode,
				pKeyTipChain,
				bAutoInvokeAction,
				bActionInvoked,
				bContinueKeyTipMode
				)
			)
			return nBtnIdx;
	}
	return -1;
}

void CExtToolControlBar::KeyTipDisplay(
	CExtCustomizeCmdKeyTip & keyTipChain
	)
{
int nBtnIdx, nCountOfButtons = _GetButtonsCountImpl();
	for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		pTBB->OnKeyTipDisplay( keyTipChain );
	}
}

#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtToolControlBar::OnFlatTrackingStart(
	HDWP & hPassiveModeDWP
	)
{
	ASSERT_VALID( this );
	hPassiveModeDWP;
MSG _msg;
	if(		::PeekMessage( &_msg, NULL, WM_ACTIVATEAPP, WM_ACTIVATEAPP, PM_NOREMOVE )
		&&	_msg.wParam == 0
		)
		return;
	if( ! CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( m_hWnd ) )
		return;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	KeyTipsDisplayedSet( true );
	KeyTipChainEmpty();
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)
bool bHaveDWP = false;
	if(		hPassiveModeDWP == NULL
//		&&	g_PaintManager.m_bIsWinVistaOrLater
//		&&	g_PaintManager.m_DWM.IsCompositionEnabled()
		)
	{
		bHaveDWP = true;
		hPassiveModeDWP = ::BeginDeferWindowPos( 64 );
	}
int nBtnIdx, nCountOfButtons = _GetButtonsCountImpl();
	for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		pTBB->OnFlatTrackingStart( hPassiveModeDWP );
	}
	if( bHaveDWP && hPassiveModeDWP != NULL )
	{
		::EndDeferWindowPos( hPassiveModeDWP );
		hPassiveModeDWP = NULL;
		CExtPaintManager::stat_PassPaintMessages();
		CExtPopupMenuTipWnd::UpdateDelayedLayeredBehaviorAll();
	}
	CExtPopupMenuTipWnd::UpdateDelayedLayeredBehaviorAll();
}

void CExtToolControlBar::OnFlatTrackingStop()
{
	ASSERT_VALID( this );
int nBtnIdx, nCountOfButtons = _GetButtonsCountImpl();
	for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		pTBB->OnFlatTrackingStop();
	}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	KeyTipsDisplayedSet( false );
	KeyTipChainEmpty();
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)
}

BOOL CExtToolControlBar::RemoveButton(
	int nPos,
	BOOL bDoRecalcLayout // = TRUE
	)
{
	if( nPos < 0 )
	{
		ASSERT( FALSE );
		return FALSE;
	}
int nCountOfButtons = _GetButtonsCountImpl();
	if( nPos >= nCountOfButtons )
	{
		ASSERT( FALSE );
		return FALSE;
	}
CExtBarButton * pTBB =
		_GetButtonPtr( nPos );
	ASSERT_VALID( pTBB );
	if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_buttons.RemoveAt( nPos );
INT nChildIndex, nChildCount = pTBB->ChildButtonGetCount();
	for( nChildIndex = 0; nChildIndex < nChildCount; nChildIndex ++ )
		RemoveButton( _GetIndexOf( pTBB->ChildButtonGetAt( nChildIndex ) ), FALSE );
	AnimationSite_ClientRemove( pTBB );
	delete pTBB;
	m_nBtnIdxCapture = -1;      // nothing captured
	m_nBtnIdxHover = -1;
	m_nBtnIdxMenuTracking = -1;
	if( bDoRecalcLayout )
		_RecalcLayoutImpl();
	return TRUE;
}

bool CExtToolControlBar::IsShowTipWhenDisabled( const CExtBarButton * pTBB ) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB;
	return true;
}

BOOL CExtToolControlBar::InsertSpecButton(
	int nPos, // -1 - append
	CExtBarButton * pButton,
	BOOL bDoRecalcLayout // = TRUE
	)
{
int nCountOfButtons = _GetButtonsCountImpl();
	if( nPos < 0 )
		nPos = nCountOfButtons;
	if( nPos > nCountOfButtons )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	if( nCountOfButtons > 0
		&& nPos == nCountOfButtons
		)
	{
		CExtBarButton * pTBB =
			_GetButtonPtr( nCountOfButtons - 1 );
		ASSERT_VALID( pTBB );
		if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) )
			nPos--;
	}
	ASSERT_VALID( pButton );
	ASSERT( pButton->GetSafeBar() != NULL );
	ASSERT( pButton->GetSafeBar() == this );
	if( _GetIndexOf(pButton) >= 0 )
	{
		ASSERT( FALSE ); // already inserted
		return FALSE;
	}
	m_buttons.InsertAt( nPos, pButton );
	ASSERT( _GetIndexOf(pButton) >= 0 );
	if( bDoRecalcLayout )
		_RecalcLayoutImpl();
	return TRUE;
}

BOOL CExtToolControlBar::InsertButton(
	int nPos, // = -1, // append
	UINT nCmdID, // = ID_SEPARATOR
	BOOL bDoRecalcLayout // = TRUE
	)
{
int nCountOfButtons = _GetButtonsCountImpl();
	if( nPos < 0 )
		nPos = nCountOfButtons;
	if( nPos > nCountOfButtons )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	if( nCountOfButtons > 0
		&& nPos == nCountOfButtons
		)
	{
		CExtBarButton * pTBB =
			_GetButtonPtr( nCountOfButtons - 1 );
		ASSERT_VALID( pTBB );
		if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) )
			nPos--;
	}
	try
	{
		CExtBarButton * pTBB = OnCreateBarCommandBtn( nCmdID );
		ASSERT_VALID( pTBB );
		m_buttons.InsertAt( nPos, pTBB );
		if( bDoRecalcLayout )
			_RecalcLayoutImpl();
	} // try
//	catch( std::exception * pXept )
//	{
//		delete pXept;
//		ASSERT( FALSE );
//		return FALSE;
//	} // catch( std::exception * pXept )
	catch( CException * pXept )
	{
		pXept->Delete();
		ASSERT( FALSE );
		return FALSE;
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
		return FALSE;
	} // catch( ... )
	return TRUE;
}

CExtBarButton * CExtToolControlBar::OnCreateBarCommandBtn(
	UINT nCmdID,
	UINT nStyle // = 0
	)
{
	ASSERT_VALID( this );
CExtBarButton * pTBB = new CExtBarButton( this, nCmdID, nStyle );
	ASSERT_VALID( pTBB );
	return pTBB;
}

CExtBarContentExpandButton * CExtToolControlBar::OnCreateBarRightBtn()
{
	ASSERT_VALID( this );
	if( m_bPaletteMode )
		return NULL;
CExtBarContentExpandButton * pRightBtn =
		new CExtBarContentExpandButton( this );
	ASSERT_VALID( pRightBtn );
	return pRightBtn;
}

COLORREF CExtToolControlBar::OnQueryCustomAccentEffectForIcon(
	CDC & dc,
	CExtBarButton * pTBB
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	dc;
	pTBB;
	return COLORREF(-1L);
}

BOOL CExtToolControlBar::SetButtons(
	const UINT * lpIDArray, // = NULL
	int nIDCount // = 0
	)
{
	ASSERT_VALID( this );
	ASSERT(
		lpIDArray == NULL
		|| nIDCount == 0
		|| AfxIsValidAddress(
			lpIDArray,
			sizeof(UINT) * nIDCount,
			FALSE
			)
		);
	_RemoveAllButtonsImpl();
	if( lpIDArray == NULL
		|| nIDCount == 0
		)
		return TRUE;
	try
	{
		bool bRevertRTL = OnQueryRevertRTL();
		for( int i = 0; i < nIDCount; i++ )
		{
			CExtBarButton * pTBB = OnCreateBarCommandBtn( *lpIDArray++, 0 );
			ASSERT_VALID( pTBB );
			if( bRevertRTL )
				m_buttons.InsertAt( 0, pTBB );
			else
				m_buttons.Add( pTBB );
		} // for( int i = 0; i < nIDCount; i++ )
		ASSERT( m_pRightBtn == NULL );
		m_pRightBtn = OnCreateBarRightBtn();
		if( m_pRightBtn != NULL )
		{
			ASSERT_VALID( m_pRightBtn );
			ASSERT_KINDOF( CExtBarContentExpandButton, m_pRightBtn );
			m_buttons.Add( m_pRightBtn );
		} // if( m_pRightBtn != NULL )
	} // try
	catch( CException * pXept )
	{
		pXept->Delete();
		ASSERT( FALSE );
		return FALSE;
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
		return FALSE;
	} // catch( ... )
	return TRUE;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
BOOL CExtToolControlBar::SetButtons(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	SetButtons();
	if( pNode == NULL )
		return TRUE;
	ASSERT_VALID( pNode );
CExtCustomizeSite * pSite = GetCustomizeSite();
	if( pSite == NULL )
	{
		ASSERT( FALSE );
		return FALSE;
	}
bool bRevertRTL = OnQueryRevertRTL();
int nCount = pNode->GetNodeCount();
	for( int i = 0; i < nCount; i++ )
	{
		CExtCustomizeCmdTreeNode * pNodeBtn =
			pNode->ElementAt( i );
		ASSERT_VALID( pNodeBtn );
		if( pNodeBtn->GetFlags() & __ECTN_GROUP_START )
		{
			if( i == 0 )
			{
				ASSERT( FALSE );
				continue;
			}
			// separator
			VERIFY(
				InsertButton(
					bRevertRTL ? 0 : -1,
					ID_SEPARATOR,
					FALSE
					)
				);
		}
		ASSERT( CExtCmdManager::IsCommand(pNodeBtn->GetCmdID(false)) || pNodeBtn->GetCmdID(false) == UINT(-1) );
		ASSERT( CExtCmdManager::IsCommand(pNodeBtn->GetCmdID(true)) || pNodeBtn->GetCmdID(true) == UINT(-1) );
		CExtBarButton * pTBB =
			pSite->OnCreateToolbarButton(
				this,
				NULL,
				pNodeBtn
				);
		if( pTBB == NULL )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		ASSERT_VALID( pTBB );
		if( bRevertRTL )
			m_buttons.InsertAt( 0, pTBB );
		else
			m_buttons.Add( pTBB );
	} // for( int i = 0; i < nCount; i++ )
	ASSERT( m_pRightBtn == NULL );
	m_pRightBtn = OnCreateBarRightBtn();
	if( m_pRightBtn != NULL )
	{
		ASSERT_VALID( m_pRightBtn );
		ASSERT_KINDOF( CExtBarContentExpandButton, m_pRightBtn );
		m_buttons.Add( m_pRightBtn );
	} // if( m_pRightBtn != NULL )
	return TRUE;
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtToolControlBar::IsLargeIcons() const
{
	ASSERT_VALID( this );
	return g_bToolbarLargeIcons;
}

bool CExtToolControlBar::IsDisplayScreenTips() const
{
	ASSERT_VALID( this );
	return g_bToolbarScreenTips;
}

bool CExtToolControlBar::IsDisplayShortcutKeysOnScreenTips() const
{
	ASSERT_VALID( this );
	return g_bToolbarShortcutKeysOnScreenTips;
}

LRESULT CExtToolControlBar::DoHelpHitTest( CPoint ptHelpHitTest )
{
	ASSERT_VALID( this );

LRESULT nHelpHit =
		((CExtToolControlBar*)this)->
			HitTest(ptHelpHitTest);
	if( nHelpHit >= 0 )
	{
		CExtBarButton * pTBB =
			_GetButtonPtr( (int)nHelpHit );
		ASSERT_VALID( pTBB );
		if( pTBB != NULL )
		{
			nHelpHit = pTBB->OnHelpHitTest( ptHelpHitTest );
			if( nHelpHit == -1 || nHelpHit == -2 )
				return -2;
			return nHelpHit;
		}
	} // if( nHelpHit >= 0 )
	return CExtControlBar::DoHelpHitTest( ptHelpHitTest );
}

/////////////////////////////////////////////////////////////////////////////
// CExtToolControlBar attribute access

int CExtToolControlBar::CommandToIndex(UINT nIDFind) const
{
	ASSERT_VALID(this);
int nBtnIdx, nCountOfButtons = _GetButtonsCountImpl();
	for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		if( pTBB->GetCmdID(false) == nIDFind )
			return nBtnIdx;
	}
	return -1;
}

UINT CExtToolControlBar::GetButtonID(int nIndex) const
{
//	ASSERT_VALID(this);
CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	if( pTBB == NULL )
		return ID_SEPARATOR;
	ASSERT_VALID( pTBB );
	return pTBB->GetCmdID(false);
}

void CExtToolControlBar::GetButtonRect(int nIndex, LPRECT lpRect) const
{
//	ASSERT_VALID(this);
	ASSERT(AfxIsValidAddress(lpRect, sizeof(RECT)));
CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	if( pTBB == NULL )
	{
		::memset( lpRect, 0, sizeof(RECT) );
		return;
	}
	ASSERT_VALID( pTBB );
	*lpRect = *pTBB;
}

UINT CExtToolControlBar::GetButtonStyle(int nIndex) const
{
CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	if( pTBB == NULL )
		return 0;
	ASSERT_VALID( pTBB );
	return pTBB->GetStyle();
}

void CExtToolControlBar::SetButtonStyle(int nIndex, UINT nStyle)
{
CExtBarButton * pTBB = _GetButtonPtr(nIndex);
	if( pTBB == NULL )
		return;
	ASSERT_VALID( pTBB );
UINT nOldStyle = pTBB->GetStyle();
	if( nOldStyle != nStyle )
	{
		pTBB->SetStyle( nStyle );
		_InvalidateButton( nIndex );
//		UpdateWindow();
	}
}

CWnd * CExtToolControlBar::GetButtonCtrl(
	int nIndex
	)
{
CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	ASSERT( pTBB != NULL );
	if( pTBB == NULL )
		return NULL;
	ASSERT_VALID( pTBB );
CWnd * pCtrl = pTBB->CtrlGet();
	return pCtrl;
}

void CExtToolControlBar::SetButtonCtrlVisibleVertically(
	int nIndex,
	bool bVisible // = true
	)
{
CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	ASSERT( pTBB != NULL );
	if( pTBB == NULL )
		return;
	pTBB->SetCtrlVisibleVertically( bVisible );
}

bool CExtToolControlBar::GetButtonCtrlVisibleVertically(
	int nIndex
	) const
{
CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	ASSERT( pTBB != NULL );
	if( pTBB == NULL )
		return false;
	return pTBB->GetCtrlVisibleVertically();
}


bool CExtToolControlBar::SetButtonCtrl(
	int nIndex,
	CWnd * pCtrl, // = NULL
	bool bCtrlAutoDestroyed // = TRUE
	)
{
CExtBarButton * pTBB = _GetButtonPtr(nIndex);
	ASSERT( pTBB != NULL );
	if( pTBB == NULL )
		return FALSE;
	ASSERT_VALID( pTBB );
	if( pCtrl != NULL )
	{
		ASSERT_VALID( pCtrl );
		DWORD dwCtrlStyle = pCtrl->GetStyle();
		ASSERT( (dwCtrlStyle&WS_CHILD) != 0 );
		if( (dwCtrlStyle&WS_CHILD) == 0 )
			return FALSE;
		ASSERT( IsChild( pCtrl ) );
		if( !IsChild( pCtrl ) )
			return FALSE;
		UINT nDlgCtrlID = (UINT)pCtrl->GetDlgCtrlID();
		ASSERT( nDlgCtrlID == pTBB->GetCmdID(false) );
		if( nDlgCtrlID != pTBB->GetCmdID(false) )
			return FALSE;
	}
	pTBB->CtrlSet( pCtrl, bCtrlAutoDestroyed );
	_RecalcLayoutImpl();
	return TRUE;
}

int CExtToolControlBar::GetButtonByAccessKey(__EXT_MFC_SAFE_TCHAR vkTCHAR)
{
int nCountOfButtons = _GetButtonsCountImpl();
	ASSERT( nCountOfButtons >= 0 );
	if( nCountOfButtons == 0 )
		return -1;
	for( int nBtnIdx=0; nBtnIdx<nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr(nBtnIdx);
		ASSERT_VALID( pTBB );
		if(		pTBB->IsSeparator()
			||	(! pTBB->IsVisible() )
			||	( pTBB->GetStyle() & TBBS_HIDDEN )
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton))
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiRightButton))
			)
			continue;
//		ASSERT( CExtCmdManager::IsCommand(pTBB->GetCmdID(false)) );
//		ASSERT( CExtCmdManager::IsCommand(pTBB->GetCmdID(true)) );
		if( ! CExtCmdManager::IsCommand(pTBB->GetCmdID(false)) )
			continue;
		if( ! CExtCmdManager::IsCommand(pTBB->GetCmdID(true)) )
			continue;

		CExtSafeString strButtonText( pTBB->GetText() );
		if( strButtonText.IsEmpty() )
			continue;

		int nTextLen = strButtonText.GetLength();
		ASSERT( nTextLen > 0 );
		int nAmdIndex = strButtonText.Find(_T('&'));
		if( nAmdIndex < 0 )
			continue;
		if( nAmdIndex == nTextLen - 1 ) // !?!?!
			continue;
		TCHAR vk = strButtonText[nAmdIndex+1];
		TCHAR szChar[2] = { vk, _T('\0') };
		::CharUpper( szChar );
		vk = szChar[0];

		if( vkTCHAR == ((TCHAR)vk) )
		{
			if(		( !pTBB->IsVisible() )
				||	(pTBB->GetStyle() & TBBS_HIDDEN) != 0
				)
			{
				if( m_pRightBtn != NULL )
					return _GetIndexOf( m_pRightBtn );
				continue;
			}
			return nBtnIdx;
		} // if( vkTCHAR == ((TCHAR)vk) )
	} // for( int nBtnIdx=0; nBtnIdx<nCountOfButtons; nBtnIdx++ )
	return -1;
}

HMENU CExtToolControlBar::GetButtonMenu(
	int nIndex
	)
{
CExtBarButton * pTBB = _GetButtonPtr(nIndex);
	ASSERT( pTBB != NULL );
	if( pTBB == NULL )
		return NULL;
	ASSERT_VALID( pTBB );
	return ((HMENU)(*pTBB));
}

bool CExtToolControlBar::MarkButtonAsMdiWindowsMenu(
	int nIndex,
	bool bAppendMdiWindowsMenu // = TRUE
	)
{
CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	ASSERT( pTBB != NULL );
	if( pTBB == NULL )
		return false;
	ASSERT_VALID( pTBB );
	pTBB->SetAppendMdiWindowsMenu(
		bAppendMdiWindowsMenu
		);
	return true;
}

bool CExtToolControlBar::SetButtonMenu(
	int nIndex,
	HMENU hMenu, // = NULL
	bool bPopupMenu, // = TRUE
	bool bAutoDestroyMenu, // = TRUE
	bool bDoRecalcLayout // = TRUE
	)
{
CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	ASSERT( pTBB != NULL );
	if( pTBB == NULL )
		return false;
	ASSERT_VALID( pTBB );
	pTBB->SetMenu(
		hMenu,
		bPopupMenu,
		bAutoDestroyMenu
		);
	if( bDoRecalcLayout )
		_RecalcLayoutImpl();
	return true;
}

int CExtToolControlBar::GetVisibleButton(
	int nBtnIdx,
	BOOL bNext,
	BOOL bPassDisabled // = TRUE
	)
{
int nCountOfButtons = _GetButtonsCountImpl();
	ASSERT( nCountOfButtons >= 0 );
	if( nCountOfButtons == 0 )
		return -1;
	if( nCountOfButtons == 1 )
		return 0;
	if( bNext )
	{
		int iNewButton = nBtnIdx + 1;
		if( iNewButton >= nCountOfButtons )
			iNewButton = 0;
		while( iNewButton != nBtnIdx )
		{
			CExtBarButton * pTBB =
				_GetButtonPtr(iNewButton);
			ASSERT_VALID( pTBB );
			if(		pTBB->OnQueryFlatTrackingEnabled()
				&&	(	(!bPassDisabled)
						|| (bPassDisabled && (!pTBB->IsDisabled()))
					)
				)
			{
				return iNewButton;
			}
			iNewButton++;
			if( iNewButton >= nCountOfButtons )
				iNewButton = 0;
		}
		return iNewButton;
	} // if( bNext )
	else
	{
		int iNewButton =
			nBtnIdx - 1;
		if( iNewButton < 0 )
			iNewButton = nCountOfButtons - 1;
		while( iNewButton != nBtnIdx )
		{
			CExtBarButton * pTBB =
				_GetButtonPtr(iNewButton);
			ASSERT_VALID( pTBB );
			if(		pTBB->OnQueryFlatTrackingEnabled()
				&&	(	(!bPassDisabled)
						|| (bPassDisabled && (!pTBB->IsDisabled()))
					)
				)
				return iNewButton;
			iNewButton--;
			if( iNewButton < 0 )
				iNewButton = nCountOfButtons - 1;
		}
		return iNewButton;
	} // else from if( bNext )
}

INT CExtToolControlBar::OnCalcFlatTrackingIndex(
	/*__EXT_MFC_SAFE_TCHAR*/ INT vkTCHAR,
	INT nStartSearchIndex
	)
{
	ASSERT_VALID( this );
	if(		(! m_bHelperFlatTrackingCalcEnabledLeftRight )
		&&	(! m_bHelperFlatTrackingCalcEnabledUpDown )
		)
		return -1;
	if(		vkTCHAR != VK_LEFT
		&&	vkTCHAR != VK_RIGHT
		&&	vkTCHAR != VK_UP
		&&	vkTCHAR != VK_DOWN
		&&	vkTCHAR != VK_TAB
		)
		return -1;
bool bShift = false;
CTypedPtrArray < CPtrArray, CExtBarButton * > arrTabOrder;
	if( vkTCHAR == VK_TAB )
	{
		bShift = CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT ) ? true : false;
		if( ! OnCalcFlatTrackingTabOrder( arrTabOrder ) )
		{
			if( bShift )
				vkTCHAR = VK_LEFT;
			else
				vkTCHAR = VK_RIGHT;
		} // if( ! OnCalcFlatTrackingTabOrder( arrTabOrder ) )
	} // if( _vkTCHAR == VK_TAB )
INT nButtonIndex, nCountOfButtons = GetButtonsCount();
	if( nCountOfButtons == 0 )
		return -1;
CExtBarButton * pStartTBB = NULL;
	if( 0 <= nStartSearchIndex && nStartSearchIndex < nCountOfButtons )
	{
		pStartTBB = GetButton( nStartSearchIndex );
		ASSERT_VALID( pStartTBB );
		if( ! pStartTBB->OnQueryFlatTrackingEnabled() )
		{
			pStartTBB = NULL;
			nStartSearchIndex = -1;
		}
	} // if( 0 <= nStartSearchIndex && nStartSearchIndex < nCountOfButtons )
	if( nStartSearchIndex < 0 || pStartTBB == NULL )
	{
		CPoint ptMinLT( 32767, 32767 );
		for( nButtonIndex = 0; nButtonIndex < nCountOfButtons; nButtonIndex++ )
		{
			CExtBarButton * pTBB = GetButton( nButtonIndex );
			ASSERT_VALID( pTBB );
			if( ! pTBB->OnQueryFlatTrackingEnabled() )
				continue;
			CRect rcTBB = *pTBB;
			if(		rcTBB.left < ptMinLT.x
				||	( rcTBB.left == ptMinLT.x && rcTBB.top < ptMinLT.y )
				)
			{
				nStartSearchIndex = nButtonIndex;
				ptMinLT.x = min( ptMinLT.x, rcTBB.left );
				ptMinLT.y = min( ptMinLT.y, rcTBB.top );
			}
		} // for( nButtonIndex = 0; nButtonIndex < nCountOfButtons; nButtonIndex++ )
		return nStartSearchIndex;
	} // if( nStartSearchIndex < 0 || pStartTBB == NULL )
	if( vkTCHAR == VK_TAB )
	{
		INT nIndex, nFound = -1, nCount = INT( arrTabOrder.GetSize() );
		if( nCount == 0 )
			return nStartSearchIndex;
		for( nIndex = 0; nIndex < nCount; nIndex ++ )
		{
			CExtBarButton * pTBB = arrTabOrder[ nIndex ];
			ASSERT_VALID( pTBB );
			if( pTBB == pStartTBB )
			{
				nFound = nIndex;
				break;
			}
		} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
		if( nFound < 0 )
			return nStartSearchIndex;
		if( bShift )
		{
			if( nFound == 0 )
				nFound = nCount - 1;
			else
				nFound --;
		} // if( bShift )
		else
		{
			if( nFound == ( nCount - 1 ) )
				nFound = 0;
			else
				nFound ++;
		} // else from if( bShift )
		CExtBarButton * pTBB = arrTabOrder[ nFound ];
		ASSERT_VALID( pTBB );
		nStartSearchIndex = _GetIndexOf( pTBB );
		return nStartSearchIndex;
	} // if( vkTCHAR == VK_TAB )
CRect rcNextTBB( 0, 0, 0, 0 );
	switch( TCHAR(vkTCHAR) )
	{
	case VK_LEFT:
		if( ! m_bHelperFlatTrackingCalcEnabledLeftRight )
			return nStartSearchIndex;
		rcNextTBB.SetRect( -32767, -32767, -32767,  32767 );
		break;
	case VK_RIGHT:
		if( ! m_bHelperFlatTrackingCalcEnabledLeftRight )
			return nStartSearchIndex;
		rcNextTBB.SetRect(  32767, -32767,  32767,  32767 );
		break;
	case VK_UP:
		if( ! m_bHelperFlatTrackingCalcEnabledUpDown )
			return nStartSearchIndex;
		rcNextTBB.SetRect( -32767, -32767, -32767, -32767 );
		break;
	case VK_DOWN:
		if( ! m_bHelperFlatTrackingCalcEnabledUpDown )
			return nStartSearchIndex;
		rcNextTBB.SetRect( -32767,  32767, -32767,  32767 );
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		return nStartSearchIndex;
#endif // _DEBUG
	} // switch( TCHAR(vkTCHAR) )
CRuntimeClass * pStartRTC = pStartTBB->GetRuntimeClass();
CExtBarButton * pNextsParentTBB = pStartTBB->ParentButtonGet();
//CRect rcStartTBB = *pStartTBB;
CRect rcStartTBB = pStartTBB->OnQueryFlatWalkingCalcRect();
CPoint ptStartCP = rcStartTBB.CenterPoint(), ptNextCP( 32767, 32767 );
INT nNextDist = 32767, nPassIndex;
INT nSavedStartSearchIndex = nStartSearchIndex;
	for( nPassIndex = 0; nPassIndex < 3; nPassIndex ++ )
	{
		for( nButtonIndex = 0; nButtonIndex < nCountOfButtons; nButtonIndex++ )
		{
			CExtBarButton * pTBB = GetButton( nButtonIndex );
			ASSERT_VALID( pTBB );
			if(		(! pTBB->OnQueryFlatTrackingEnabled() )
				||	pTBB == pStartTBB
				||	nButtonIndex == nStartSearchIndex
				)
				continue;
			CExtBarButton * pParentTBB = pTBB->ParentButtonGet();
			//CRect rcTBB = *pTBB;
			CRect rcTBB = pTBB->OnQueryFlatWalkingCalcRect();
			CPoint ptCP = rcTBB.CenterPoint();
			INT nX2 = ptCP.x - ptStartCP.x;
			INT nY2 = ptCP.y - ptStartCP.y;
			INT nCurrDist = nX2*nX2 + nY2*nY2;
			bool bHorzIntersection = false, bVertIntersection = false, bRtcIntersection = false;
//				if(		( rcTBB.top >= rcStartTBB.top && rcTBB.bottom >= rcStartTBB.bottom )
//					||	( rcTBB.top <= rcStartTBB.top && rcTBB.bottom <= rcStartTBB.bottom )
//					)
//					bHorzIntersection = true;
//				if(		( rcTBB.left >= rcStartTBB.left && rcTBB.right >= rcStartTBB.right )
//					||	( rcTBB.left <= rcStartTBB.left && rcTBB.right <= rcStartTBB.right )
//					)
//					bVertIntersection = true;
	//			if( rcTBB.top == rcStartTBB.top && rcTBB.bottom == rcStartTBB.bottom )
	//				bHorzIntersection = true;
	//			if( rcTBB.left == rcStartTBB.left && rcTBB.right == rcStartTBB.right )
	//				bVertIntersection = true;
			if( OnCalcFlatTrackingIndexCheckIntersectionEnabled( pTBB, vkTCHAR ) )
			{
				if(		( rcTBB.top < ptStartCP.y && ptStartCP.y < rcTBB.bottom )
					&&	( rcStartTBB.top < ptCP.y && ptCP.y < rcStartTBB.bottom )
					)
					bHorzIntersection = true;
			}
			if( OnCalcFlatTrackingIndexCheckIntersectionEnabled( pTBB, vkTCHAR ) )
			{
				if(		( rcTBB.left < ptStartCP.y && ptStartCP.y < rcTBB.right )
					&&	( rcStartTBB.left < ptCP.y && ptCP.y < rcStartTBB.right )
					)
					bVertIntersection = true;
			}
			CRuntimeClass * pCurrRTC = pTBB->GetRuntimeClass();
			if(		pStartRTC->IsDerivedFrom( pCurrRTC )
				&&	pCurrRTC->IsDerivedFrom( pStartRTC )
				)
				bRtcIntersection = true;
			if( nPassIndex == 0 )
			{
				if( ! bRtcIntersection )
					continue;
			}
			switch( TCHAR(vkTCHAR) )
			{
			case VK_LEFT:
				if( nPassIndex <= 1 )
				{
					if( ! bHorzIntersection )
						continue;
				}
				if( rcTBB.right > rcStartTBB.left )
					continue;
				if( rcTBB.right < rcNextTBB.right )
				{
					if( pNextsParentTBB != pParentTBB )
						continue;
					if( nCurrDist >= nNextDist )
						continue;
				}
				if( rcTBB.right == rcNextTBB.right )
				{
					INT nDistCurr = INT( abs( ( ptStartCP.y - ptCP.y ) ) );
					INT nDistPrev = INT( abs( ( ptStartCP.y - rcNextTBB.CenterPoint().y ) ) );
					if( nDistCurr > nDistPrev )
						continue;
				}
				if( nCurrDist >= nNextDist )
					continue;
				pNextsParentTBB = pParentTBB;
				ptNextCP = ptCP;
				nNextDist = nCurrDist;
				rcNextTBB = rcTBB;
				nStartSearchIndex = nButtonIndex;
				break;
			case VK_RIGHT:
				if( nPassIndex <= 1 )
				{
					if( ! bHorzIntersection )
						continue;
				}
				if( rcTBB.left < rcStartTBB.right )
					continue;
				if( rcTBB.left > rcNextTBB.left )
				{
					if( pNextsParentTBB != pParentTBB )
						continue;
					if( nCurrDist >= nNextDist )
						continue;
				}
				else if( rcTBB.left == rcNextTBB.left )
				{
					INT nDistCurr = INT( abs( ( ptStartCP.y - ptCP.y ) ) );
					INT nDistPrev = INT( abs( ( ptStartCP.y - rcNextTBB.CenterPoint().y ) ) );
					if( nDistCurr > nDistPrev )
						continue;
				}
				if( nCurrDist >= nNextDist )
					continue;
				pNextsParentTBB = pParentTBB;
				ptNextCP = ptCP;
				nNextDist = nCurrDist;
				rcNextTBB = rcTBB;
				nStartSearchIndex = nButtonIndex;
				break;
			case VK_UP:
				if( nPassIndex <= 1 )
				{
					if( ! bVertIntersection )
						continue;
				}
				if( rcTBB.bottom > rcStartTBB.top )
					continue;
				if( rcTBB.bottom < rcNextTBB.bottom )
				{
					if( pNextsParentTBB != pParentTBB )
						continue;
					if( nCurrDist >= nNextDist )
						continue;
				}
				if( rcTBB.bottom == rcNextTBB.bottom )
				{
					INT nDistCurr = INT( abs( ( ptStartCP.y - ptCP.y ) ) );
					INT nDistPrev = INT( abs( ( ptStartCP.y - rcNextTBB.CenterPoint().y ) ) );
					if( nDistCurr > nDistPrev )
						continue;
				}
				if( nCurrDist >= nNextDist )
					continue;
				pNextsParentTBB = pParentTBB;
				ptNextCP = ptCP;
				nNextDist = nCurrDist;
				rcNextTBB = rcTBB;
				nStartSearchIndex = nButtonIndex;
				break;
			case VK_DOWN:
				if( nPassIndex <= 1 )
				{
					if( ! bVertIntersection )
						continue;
				}
				if( rcTBB.top < rcStartTBB.bottom )
					continue;
				if( rcTBB.top > rcNextTBB.top )
				{
					if( pNextsParentTBB != pParentTBB )
						continue;
					if( nCurrDist >= nNextDist )
						continue;
				}
				else if( rcTBB.top == rcNextTBB.top )
				{
					INT nDistCurr = INT( abs( ( ptStartCP.y - ptCP.y ) ) );
					INT nDistPrev = INT( abs( ( ptStartCP.y - rcNextTBB.CenterPoint().y ) ) );
					if( nDistCurr > nDistPrev )
						continue;
				}
				if( nCurrDist >= nNextDist )
					continue;
				pNextsParentTBB = pParentTBB;
				ptNextCP = ptCP;
				nNextDist = nCurrDist;
				rcNextTBB = rcTBB;
				nStartSearchIndex = nButtonIndex;
				break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
				return nStartSearchIndex;
#endif // _DEBUG
			} // switch( TCHAR(vkTCHAR) )
		} // for( nButtonIndex = 0; nButtonIndex < nCountOfButtons; nButtonIndex++ )
		if( nSavedStartSearchIndex != nStartSearchIndex )
		{
			if( nStartSearchIndex >= 0 )
			{
				if( ! OnCalcFlatTrackingIndexCheckPass(
						vkTCHAR,
						nPassIndex,
						pStartTBB,
						GetButton( nStartSearchIndex )
						)
					)
				{
					nStartSearchIndex = -1;
					continue;
				}
			} // if( nStartSearchIndex >= 0 )
//			break;
		} // if( nSavedStartSearchIndex != nStartSearchIndex )
	} // for( nPassIndex = 0; nPassIndex < 3; nPassIndex ++ )
//	if( nStartSearchIndex < 0 )
//		return nSavedStartSearchIndex;
	return nStartSearchIndex;
}

bool CExtToolControlBar::OnCalcFlatTrackingIndexCheckPass(
	/*__EXT_MFC_SAFE_TCHAR*/ INT vkTCHAR,
	INT nPassIndex,
	const CExtBarButton * pPrevTBB,
	const CExtBarButton * pNextTBB
	) const
{
	ASSERT_VALID( this );
	vkTCHAR;
	nPassIndex;
	pPrevTBB;
	pNextTBB;
	return true;
}

bool CExtToolControlBar::OnCalcFlatTrackingTabOrder(
	CTypedPtrArray < CPtrArray, CExtBarButton * > & arrTabOrder
	)
{
	ASSERT_VALID( this );
	arrTabOrder.RemoveAll();
INT nIndex, nCount = GetButtonsCount();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtBarButton * pTBB = GetButton( nIndex );
		ASSERT_VALID( pTBB );
		if( ! pTBB->OnQueryFlatTrackingEnabled() )
			continue;
		arrTabOrder.Add( pTBB );
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
	return true;
}

bool CExtToolControlBar::OnCalcFlatTrackingIndexCheckIntersectionEnabled(
	CExtBarButton * pTBB,
	/*__EXT_MFC_SAFE_TCHAR*/ INT vkTCHAR
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB;
	vkTCHAR;
	return false;
}

BOOL CExtToolControlBar::TrackButtonMenu(
	int nIndex,
	BOOL bSelectAny, // = FALSE
	BOOL bDelayedTracking, // = FALSE
	BOOL bNoAnimation // = FALSE
	)
{
bool bDockSiteCustomizeMode =
		_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return FALSE;

	if(		g_bMenuTracking
		&&	m_nBtnIdxMenuTracking == nIndex
		)
		return TRUE;

	if( !SafeDisplayBar() )
		return FALSE;

CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	ASSERT( pTBB != NULL );
	if( pTBB == NULL )
		return FALSE;
	ASSERT_VALID( pTBB );
	if( pTBB->GetStyle() & (TBBS_HIDDEN|TBBS_DISABLED) )
		return FALSE;
	if( ! pTBB->IsAbleToTrackMenu() )
		return FALSE;
CPoint point( -32767, -32767 );
	if(		(! IsKindOf( RUNTIME_CLASS( CExtMenuControlBar ) ) )
		||	(! ((CExtMenuControlBar*)this)->m_bUseFastKeyboardMenuTracking )
		)
	{
		if( ! ::GetCursorPos(&point) )
			return FALSE;
		ScreenToClient( &point );
	}
	if( bDelayedTracking )
		PostMessage(
			g_nMsgTrackButtonMenu,
			(WPARAM)nIndex,
			(LPARAM)bNoAnimation
			);
	else
		pTBB->OnTrackPopup(
			point,
			bSelectAny ? true : false,
			bNoAnimation ? true : false
			);
	return TRUE;
}

CSize CExtToolControlBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	if(	(nLength == -1)
		&& !(dwMode & (LM_MRUWIDTH|LM_COMMIT))
		&&  (dwMode & (LM_HORZDOCK|LM_VERTDOCK))
		)
		return
			CalcFixedLayout(
				dwMode & LM_STRETCH,
				dwMode & LM_HORZDOCK
				);
	ASSERT(
		(dwMode&(LM_HORZ|LM_HORZDOCK))
		||
		(!(dwMode&LM_HORZDOCK))
		);
	return _CalcLayout( dwMode, nLength );
}

CSize CExtToolControlBar::CalcFixedLayout(
	BOOL bStretch,
	BOOL bHorz
	)
{
DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;
	ASSERT(
		(dwMode&(LM_HORZ|LM_HORZDOCK))
		||
		(!(dwMode&LM_HORZDOCK))
		);
	return _CalcLayout( dwMode );
}

CSize CExtToolControlBar::_CalcLayout(
	DWORD dwMode,
	int nLength
	)
{
	ASSERT_VALID(this);
	if( ! m_bInsideCalcLayout )
		m_bHelperTempFullRowMode = false;

//	if(		m_pDockSite->GetSafeHwnd() != NULL
//		&&	(! IsFloating() )
//		&&	OnQueryMultiRowLayout()
//		)
//	{
//		WINDOWPLACEMENT _wp;
//		::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
//		_wp.length = sizeof(WINDOWPLACEMENT);
//		if(		m_pDockSite->GetWindowPlacement( &_wp )
//			&&	_wp.showCmd == SW_SHOWMINIMIZED
//			)
//		{
//			CRect rcWnd;
//			GetWindowRect( &rcWnd );
//			return rcWnd.Size();
//		}
//	}
	
CSize sizeDefButton = _GetDefButtonSize();
CSize sizeCalcLayout = sizeDefButton;
	if( _GetButtonsCountImpl() > 0 )
	{
		if( m_pRightBtn != NULL )
			m_pRightBtn->ClearContent( false );
		BOOL bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;
		INT nLengthToSizeTB =
			(dwMode & LM_HORZ) ? 32767 : 0;
		BOOL bVertSizeTB = FALSE;
		if( bDynamic )
		{
			if( dwMode & LM_MRUWIDTH )
				nLengthToSizeTB = m_nMRUWidth;
			else if( dwMode & LM_HORZDOCK )
				nLengthToSizeTB = 32767;
			else if( dwMode & LM_VERTDOCK )
				nLengthToSizeTB = 0;
			else if( nLength != -1 )
			{
				CRect rcInside( 0, 0, 0, 0 );
				_CalcInsideRect(
					rcInside,
					(dwMode & LM_HORZ)
					);
				bVertSizeTB = (dwMode & LM_LENGTHY);
				nLengthToSizeTB =
					nLength +
					( bVertSizeTB ?
						rcInside.Height() : rcInside.Width()
					);
			}
			else if( (m_dwStyle & CBRS_FLOATING) != 0 )
					nLengthToSizeTB = m_nMRUWidth;
		} // if( bDynamic )

		bool bMultiRowLayout =
			OnQueryMultiRowLayout();
		if(		bMultiRowLayout
			&&	((dwMode & LM_HORZ) == 0)
			&&	(!m_bPaletteMode)
			)
		{
			CRect rcDockBarClient;
			if( m_pDockBar != NULL )
			{
				ASSERT_VALID( m_pDockBar );
				ASSERT_VALID( m_pDockSite );
				//m_pDockSite->GetClientRect( &rcDockBarClient );
				rcDockBarClient = ((CExtDockBar*)m_pDockBar)->m_rectLayout;
			}
			else
				GetParent()->GetClientRect( &rcDockBarClient );
			nLength = nLengthToSizeTB = rcDockBarClient.Height();
			bVertSizeTB = TRUE;
		}

		_SizeToolBar( nLengthToSizeTB, bVertSizeTB );
		sizeCalcLayout =
			_CalcSize(
				( (dwMode & LM_HORZ) == 0 ) ? TRUE : FALSE
				);
		if(		bDynamic
			&&	bMultiRowLayout
			&&	(!m_bPaletteMode)
			)
		{
			CSize sizeDiff(
				__EXT_TB_BUTTON_OUTER_MARGIN*2,
				__EXT_TB_BUTTON_OUTER_MARGIN*2
				);
			if(		m_pRightBtn != NULL
				&&	m_pRightBtn->IsVisible()
				)
			{
				CSize sizeRb = m_pRightBtn->Size();
				sizeDiff +=
					CSize(
						((dwMode&LM_HORZ) == 0) ? 0 : sizeRb.cx,
						((dwMode&LM_HORZ) == 0) ? sizeRb.cy : 0
						);
			}
			CRect rcDockBarClient;
			if( m_pDockBar != NULL )
			{
				ASSERT_VALID( m_pDockBar );
				ASSERT_VALID( m_pDockSite );
				//m_pDockSite->GetClientRect( &rcDockBarClient );
				rcDockBarClient = ((CExtDockBar*)m_pDockBar)->m_rectLayout;
			}
			else
				GetParent()->GetClientRect( &rcDockBarClient );
			CSize _sizeDockBarClient = rcDockBarClient.Size();
			INT nNewLengthToSizeTB = nLengthToSizeTB;
			if( (dwMode & LM_HORZ) == 0 )
			{
				if( (sizeCalcLayout.cy+sizeDiff.cy) > _sizeDockBarClient.cy )
					nNewLengthToSizeTB = _sizeDockBarClient.cy-sizeDiff.cy;
			} // if( (dwMode & LM_HORZ) == 0 )
			else
			{
				if( (sizeCalcLayout.cx+sizeDiff.cx) > _sizeDockBarClient.cx )
					nNewLengthToSizeTB = _sizeDockBarClient.cx-sizeDiff.cx;
			} // else from if( (dwMode & LM_HORZ) == 0 )
			if( nNewLengthToSizeTB < 1 )
				nNewLengthToSizeTB = 1;
			if( nNewLengthToSizeTB != nLengthToSizeTB )
			{
				if( ! m_bInsideCalcLayout )
				{
					m_bHelperTempFullRowMode = true;
					m_bInsideCalcLayout = true;
					sizeCalcLayout = _CalcLayout( dwMode, nNewLengthToSizeTB );
					if( nLengthToSizeTB != 32767 )
						_RecalcPositionsImpl();
					m_bInsideCalcLayout = false;
					return sizeCalcLayout;
				}
			}
			if( nLengthToSizeTB != 32767 )
				_RecalcPositionsImpl();
		}
		if(		m_pRightBtn != NULL
			&&	m_pRightBtn->GetButtons().GetSize() == 0
			)
		{
			m_pRightBtn->ClearContent();
			sizeCalcLayout = _CalcSize( (dwMode & LM_HORZ) == 0 );
		}
		if( dwMode & LM_COMMIT )
			if( (m_dwStyle & (CBRS_FLOATING|CBRS_SIZE_DYNAMIC)) != 0 )
				if( dwMode & LM_HORZ )
					m_nMRUWidth = sizeCalcLayout.cx;
		CRect rcInside( 0, 0, 0, 0 );
		_CalcInsideRect(
			rcInside,
			(dwMode & LM_HORZ)
			);
		sizeCalcLayout.cy -= rcInside.Height();
		sizeCalcLayout.cx -= rcInside.Width();
		CSize sizeCalcFixedLayout =
			CControlBar::CalcFixedLayout(
				dwMode & LM_STRETCH,
				(dwMode & LM_HORZ)
				);
		sizeCalcLayout.cx =
			max( sizeCalcLayout.cx, sizeCalcFixedLayout.cx );
		sizeCalcLayout.cy =
			max( sizeCalcLayout.cy, sizeCalcFixedLayout.cy );
		if( IsFloating() && IsBarWithGripper() )
		{
			CRect rcGrip = _RectGripGet();
			int nCyGripper = min( rcGrip.Width(), rcGrip.Height() );
			sizeCalcLayout.cy += nCyGripper;
		}
	} // if( _GetButtonsCountImpl() > 0 )

	if( IsDockedHorizontally() )
	{
		m_sizeDockedH = sizeCalcLayout;
		m_nMinHW = sizeCalcLayout.cy;
	}
	else if( IsDockedVertically() )
	{
		m_sizeDockedV = sizeCalcLayout;
		m_nMinVH = sizeCalcLayout.cx;
	}
	else if( IsFloating() )
	{
		if( sizeCalcLayout.cx < sizeDefButton.cx * 2 ) // (+ 2.23 )
			sizeCalcLayout.cx = sizeDefButton.cx * 2;
		m_sizeFloated = sizeCalcLayout;
	}

	if( m_pDockSite == NULL )
	{ // specific for dialog mode
		if( IsDockedHorizontally() )
			sizeCalcLayout.cy += 4;
		else
			sizeCalcLayout.cx += 4;
	} // specific for dialog mode

	return sizeCalcLayout;
}

CSize CExtToolControlBar::_CalcSize( BOOL bVerticallyDocked )
{
int nCountOfButtons = _GetButtonsCountImpl();
	if( nCountOfButtons == 0 )
		return _GetDefButtonSize();
CFont * pFont =
		OnGetToolbarFont(
			( bVerticallyDocked && (!m_bPaletteMode) )
				? true : false,
			true,
			this
			);
	if( pFont->GetSafeHandle() == NULL )
		pFont = (&( PmBridge_GetPM()->m_FontNormal ));
CClientDC dc( this );
CFont * pOldFont = dc.SelectObject( pFont );
	ASSERT( pOldFont != NULL );
bool bMultiRowLayout = OnQueryMultiRowLayout();
CExtBarButton::e_wrap_t _evt = GetBtnWrapCode();
CSize sizeDefButton = _GetDefButtonSize();
CSize sizeCalc(0,0); // ( sizeDefButton );
CPoint ptCurrent( 0, 0 );
int nRowExtent = 0;
	if( bMultiRowLayout && bVerticallyDocked && (!m_bPaletteMode) )
		nRowExtent = sizeDefButton.cx;
	else
		nRowExtent = sizeDefButton.cy;
INT nRowDistance = PmBridge_GetPM()->FixedBar_GetRowDistance( this ); // (+ v.2.22)
INT nWrapCount = 0;
	for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		
		if( pTBB->GetStyle() & TBBS_HIDDEN )
			continue;
		if(		pTBB->IsSeparator()
			&&	nBtnIdx < (nCountOfButtons-1)
			)
		{
			CExtBarButton * pNextTBB = _GetButtonPtr( nBtnIdx+1 );
			ASSERT_VALID( pNextTBB );
			if( pNextTBB->GetStyle() & TBBS_HIDDEN )
				continue;
		}
		
		CSize sizeTBB(
			pTBB->CalculateLayout(
				dc,
				sizeDefButton,
				(!bVerticallyDocked) || m_bPaletteMode
				)
			);
		if(		nBtnIdx == (nCountOfButtons-1)
			&&	m_pRightBtn != NULL
			&&	(IsFloating() || bMultiRowLayout)
			)
		{
			ASSERT_VALID( m_pRightBtn );
			ASSERT( m_pRightBtn == pTBB );
			if( bMultiRowLayout )
			{
				if( bVerticallyDocked )
					sizeCalc.cy += sizeTBB.cy;
				else
					sizeCalc.cx += sizeTBB.cx;
				if( nWrapCount > 0 && (! m_bPaletteMode) )
					m_bHelperTempFullRowMode = true;
			}
			break;
		}
		if( bMultiRowLayout && bVerticallyDocked && (!m_bPaletteMode) )
		{
			if(		( ptCurrent.y == 0 || pTBB->IsWrap(_evt) )
				&&	pTBB->IsSeparator()
				)
				sizeTBB = CSize( 0, 0 );
			nRowExtent = max( nRowExtent, sizeTBB.cx );
			sizeCalc.cx =
				max(
					ptCurrent.x + sizeTBB.cx,
					sizeCalc.cx
					);
			sizeCalc.cy =
				max(
					ptCurrent.y + sizeTBB.cy,
					sizeCalc.cy
					);
			ptCurrent.y += sizeTBB.cy;
			if( pTBB->IsWrap(_evt) )
			{
				ptCurrent.y = 0;
				ptCurrent.x += nRowExtent + nRowDistance;
				nRowExtent = sizeDefButton.cx;
				nWrapCount++;
			}
		} // if( bMultiRowLayout && bVerticallyDocked && (!m_bPaletteMode) )
		else
		{
			if(		( (!bVerticallyDocked) || m_bPaletteMode )
				&&	( ptCurrent.x == 0 || pTBB->IsWrap(_evt) )
				&&	pTBB->IsSeparator()
				)
				sizeTBB = CSize( 0, 0 );
			nRowExtent = max( nRowExtent, sizeTBB.cy );
			sizeCalc.cx =
				max(
					ptCurrent.x + sizeTBB.cx,
					sizeCalc.cx
					);
			sizeCalc.cy =
				max(
					ptCurrent.y + sizeTBB.cy,
					sizeCalc.cy
					);
			if(		bVerticallyDocked
					&&	(!m_bPaletteMode)
				)
			{
				ptCurrent.x = 0;
				ptCurrent.y += sizeTBB.cy;
			} // if( bVerticallyDocked && (!m_bPaletteMode) ...
			else
			{
				ptCurrent.x += sizeTBB.cx;
				if( pTBB->IsWrap(_evt) )
				{
					ptCurrent.x = 0;
					ptCurrent.y += nRowExtent + nRowDistance;
					nRowExtent = sizeDefButton.cy;
					nWrapCount++;
				}
			} // else from if( bVerticallyDocked && (!m_bPaletteMode) ...
		} // else from if( bMultiRowLayout && bVerticallyDocked && (!m_bPaletteMode) )
	} // for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	dc.SelectObject( pOldFont );

	if( sizeCalc.cx < sizeDefButton.cx )
		sizeCalc.cx = sizeDefButton.cx;
	if( sizeCalc.cy < sizeDefButton.cy )
		sizeCalc.cy = sizeDefButton.cy;
bool bFloating = IsFloating();
	if( ! bVerticallyDocked )
	{
//		sizeCalc.cx += m_cxLeftBorder + m_cxRightBorder;
		sizeCalc.cx += nRowDistance/2; // (+ v.2.22)
//		if(		(! bFloating )
//			&&	(GetBarStyle()&CBRS_GRIPPER) != 0
//			)
//		{
//			INT nGripWidth = _GripperWidthAtLeftGet();
//			if( nGripWidth >= 0 )
//				sizeCalc.cx += nGripWidth;
//		}
	}
	else
	{
//		sizeCalc.cx += 1;
//		sizeCalc.cy += m_cyTopBorder + m_cyBottomBorder;
		sizeCalc.cy += nRowDistance; // (+ v.2.22)
//		if(		(! bFloating )
//			&&	(GetBarStyle()&CBRS_GRIPPER) != 0
//			)
//		{
//			INT nGripHeight = _GripperHeightAtTopGet();
//			if( nGripHeight >= 0 )
//				sizeCalc.cy += nGripHeight;
//		}
	}
	if( bFloating ) // (+ v.2.22)
	{
		sizeCalc.cx += 1;
		sizeCalc.cy += 1;
	}

//	if( m_bPaletteMode && m_bPresubclassDialogMode )
//	{
//		CRect rcTest( 0, 0, 1, 1 );
//		CSize _sizeInitial = rcTest.Size();
//		CalcWindowRect( &rcTest );
//		CSize _sizeUpdated = rcTest.Size();
//		ASSERT(
//				_sizeUpdated.cx >= _sizeInitial.cx
//			&&	_sizeUpdated.cy >= _sizeInitial.cy
//			);
//		sizeCalc += _sizeUpdated - _sizeInitial;
//		sizeCalc.cx += m_cxLeftBorder + m_cxRightBorder;
//		sizeCalc.cy += m_cyTopBorder + m_cyBottomBorder;
//	}

	return sizeCalc;
}

void CExtToolControlBar::_SizeToolBar(int nLength, BOOL bVert)
{
	if( m_bPaletteMode )
		return;
int nCountOfButtons = _GetButtonsCountImpl();
	if( nCountOfButtons == 0 )
		return;
bool bMultiRowLayout = OnQueryMultiRowLayout();
	if( !(		IsFloating()
			||	bMultiRowLayout
			)
		)
	{
		_WrapToolBarH(
			bVert ? 32767 : nLength,
			bVert ? nLength : 32767
			);
		return;
	}
CSize sizeDefButton = _GetDefButtonSize();
	if( bMultiRowLayout )
	{
		if( bVert )
		{
			int nLengthCurr = nLength - sizeDefButton.cy;
			if( nLengthCurr < sizeDefButton.cy )
				nLengthCurr = sizeDefButton.cy;
			for( ; true;  )
			{
				_WrapToolBarV( nLengthCurr );
				CSize _size = _CalcSize( TRUE );
				if( _size.cy <= nLength )
					break;
				nLengthCurr -= sizeDefButton.cy / 2;
				if( nLengthCurr <= 0 )
					break;
			}
		} // if( bVert )
		else
			_WrapToolBarH( nLength );
		return;
	} // if( bMultiRowLayout )
INT nMinExtent = sizeDefButton.cx / 2;
	ASSERT( nMinExtent > 0 );
	if( bVert )
	{
		CExtPaintManager::monitor_parms_t _mp;
		CExtPaintManager::stat_GetMonitorParms( _mp, this );
		INT nCalcExtentMax = _mp.m_rcWorkArea.Width() + nMinExtent;
		// NOTES:
		// 1) bug was found by Eugene Pl.
		// 2) there is no reason to make toolbar wider
		//    then nCalcExtentMax value
		// 3) loop is locked when toolbar button icons are higher
		//    then 16px and user drags top/bottom border of a
		//    floating toolbar with 1-row layout (and attempting
		//    to decrease toolbar height)
		CSize _size( 32767, 32767 );
		for(	INT nCalcExtent = nMinExtent;
				nLength < _size.cy;
				nCalcExtent += nMinExtent
				)
		{
			_WrapToolBarH( nCalcExtent );
			_size = _CalcSize( FALSE );
			if( nCalcExtent > nCalcExtentMax )
				return;
		}
		_WrapToolBarH( _size.cx );
		return;
	}
INT nCountOfRowsDesired = _WrapToolBarH( nLength );
INT nCountOfRowsCurrent = _WrapToolBarH( nMinExtent );
	if( nCountOfRowsCurrent == nCountOfRowsDesired
		|| nMinExtent >= nLength
		)
	{
		_WrapToolBarH( _CalcSize(FALSE).cx );
		return;
	}
	while( nMinExtent < nLength )
	{
		INT nCurrentExtent = (nMinExtent + nLength) / 2;
		nCountOfRowsCurrent = _WrapToolBarH( nCurrentExtent );
		if( nCountOfRowsCurrent == nCountOfRowsDesired )
		{
			nLength = nCurrentExtent;
			continue;
		}
		if( nMinExtent == nCurrentExtent )
		{
			_WrapToolBarH( nLength );
			break;
		}
		nMinExtent = nCurrentExtent;
	}
	_WrapToolBarH( _CalcSize(FALSE).cx );
}

void CExtToolControlBar::_DraggingUpdateState(
	const CPoint & point,
	bool bForceFloatMode
	)
{
	CExtControlBar::_DraggingUpdateState( point, bForceFloatMode );
//	if( IsFloating() )
//		return;
//	if(		_IsDockSiteCustomizeMode()
//		&&	(! IsSingleOnRow() )
//		)
//		MakeSingleOnRow();
}

int CExtToolControlBar::_WrapToolBarV(
	int nHeight
	)
{
	ASSERT( !m_bPaletteMode );
//	ASSERT( !IsFloating() );

CFont * pFont = OnGetToolbarFont( true, true, this );
	if( pFont->GetSafeHandle() == NULL )
		pFont = (&( PmBridge_GetPM()->m_FontNormal ));
CClientDC dc( this );
CFont * pOldFont = dc.SelectObject( pFont );
	ASSERT( pOldFont != NULL );
CSize sizeDefButton = _GetDefButtonSize();
CRect rcClient;
	GetClientRect( &rcClient );
CPoint ptCurrent( rcClient.right, rcClient.top );
	if( m_pRightBtn != NULL )
		nHeight -=
			m_pRightBtn->CalculateLayout(
				dc, sizeDefButton, FALSE ).cy;
CExtBarButton::e_wrap_t _evt = GetBtnWrapCode();
int nCountOfColumns = 1;
int nCountOfButtons = _GetButtonsCountImpl();
CExtBarButton * pPrevButton = NULL;
INT nRowDistance = PmBridge_GetPM()->FixedBar_GetRowDistance( this );
	for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		pTBB->SetWrap( _evt, false );
		if( nBtnIdx == (nCountOfButtons-1)
			&& m_pRightBtn != NULL
			)
		{
			ASSERT_VALID( m_pRightBtn );
			ASSERT( m_pRightBtn == pTBB );
			break;
		}
		
		if( pTBB->GetStyle() & TBBS_HIDDEN )
			continue;
		if(		pTBB->IsSeparator()
			&&	nBtnIdx < (nCountOfButtons-1)
			)
		{
			CExtBarButton * pNextTBB = _GetButtonPtr( nBtnIdx+1 );
			ASSERT_VALID( pNextTBB );
			if( pNextTBB->GetStyle() & TBBS_HIDDEN )
				continue;
		}
		
		CSize sizeTBB =
			pTBB->CalculateLayout(
				dc,
				sizeDefButton,
				FALSE
				);
		if( ptCurrent.y == 0 && pTBB->IsSeparator() )
			sizeTBB = CSize(0, 0);
		if(	pPrevButton != NULL
			&&	( (ptCurrent.y + sizeTBB.cy) > nHeight )
			&&	(! pTBB->IsSeparator() )
			)
		{
			ASSERT_VALID( pPrevButton );
			pPrevButton->SetWrap( _evt );
			ptCurrent.x -=
				sizeTBB.cx
				+ nRowDistance;
			ptCurrent.y = 0;
			nCountOfColumns++;
		}
		ptCurrent.y += sizeTBB.cy;
		pPrevButton = pTBB;
	} // for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	dc.SelectObject( pOldFont );
	return nCountOfColumns;
}

int CExtToolControlBar::_WrapToolBarH(int nWidth, int nHeight /*= 32767*/)
{
bool bVerticallyDocked =
		((m_dwStyle & CBRS_ORIENT_HORZ) == 0)
			? true : false;
bool bFloatLikeMode =
		(	IsFloating()
		||	nHeight == 32767 // (+ v.2.22)
		||	OnQueryMultiRowLayout()
		) ? true : false;
CFont * pFont =
		OnGetToolbarFont(
			( bVerticallyDocked && (!m_bPaletteMode) )
				? true : false,
			true,
			this
			);
	if( pFont->GetSafeHandle() == NULL )
		pFont = (&( PmBridge_GetPM()->m_FontNormal ));
CClientDC dc( this );
CFont * pOldFont = dc.SelectObject( pFont );
	ASSERT( pOldFont != NULL );
CSize sizeDefButton = _GetDefButtonSize();
CRect rcClient;
	GetClientRect( &rcClient );
CPoint ptCurrent( 0, rcClient.top );
	if(		(!bFloatLikeMode)
		&&	(!bVerticallyDocked)
		&&	m_pRightBtn != NULL
		&&	(!m_bPaletteMode)
		)
		nWidth -=
			m_pRightBtn->CalculateLayout(
				dc, sizeDefButton, TRUE ).cx;
CExtBarButton::e_wrap_t _evt = GetBtnWrapCode();
int nCountOfRows = 1;
int nCountOfButtons = _GetButtonsCountImpl();
CExtBarButton * pPrevButton = NULL;
INT nRowDistance = PmBridge_GetPM()->FixedBar_GetRowDistance( this );
	for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		if( ! m_bPaletteMode )
		{
			pTBB->SetWrap( _evt, false );
		}
		if( nBtnIdx == (nCountOfButtons-1)
			&& m_pRightBtn != NULL
			)
		{
			ASSERT_VALID( m_pRightBtn );
			ASSERT( m_pRightBtn == pTBB );
			break;
		}

		if( pTBB->GetStyle() & TBBS_HIDDEN )
			continue;
		if(		pTBB->IsSeparator()
			&&	nBtnIdx < (nCountOfButtons-1)
			)
		{
			CExtBarButton * pNextTBB = _GetButtonPtr( nBtnIdx+1 );
			ASSERT_VALID( pNextTBB );
			if( pNextTBB->GetStyle() & TBBS_HIDDEN )
				continue;
		}
		
		CSize sizeTBB =
			pTBB->CalculateLayout(
				dc,
				sizeDefButton,
				(!bVerticallyDocked)
					|| m_bPaletteMode
				);
		if( ptCurrent.x == 0 && pTBB->IsSeparator() )
			sizeTBB = CSize(0, 0);
		if(		(	m_bPaletteMode
				&&	pPrevButton != NULL
				&&	pPrevButton->IsWrap( _evt )
				)
			||	(	(! m_bPaletteMode)
				&&	bFloatLikeMode
				&&	pPrevButton != NULL
				&&	(bFloatLikeMode || ptCurrent.y + sizeTBB.cy < nHeight)
				&&	ptCurrent.x + sizeTBB.cx > nWidth
				&&	(! pTBB->IsSeparator())
				)
			)
		{
			ASSERT_VALID( pPrevButton );
			if( ! m_bPaletteMode )
				pPrevButton->SetWrap( _evt );
			ptCurrent.x = 0;
			ptCurrent.y +=
				sizeTBB.cy
				+ nRowDistance;
			nCountOfRows++;
		}
		ptCurrent.x += sizeTBB.cx;
		pPrevButton = pTBB;
	} // for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	dc.SelectObject( pOldFont );
	return nCountOfRows;
}

CSize CExtToolControlBar::_CalcDesiredMinOuterSize( BOOL bHorz ) const
{
	ASSERT_VALID( this );
	if( m_bPaletteMode )
		return
			( const_cast < CExtToolControlBar * > ( this ) )
				-> CalcFixedLayout( FALSE, bHorz );
CSize sizeDefButton = _GetDefButtonSize();
CSize _size( 0, 0 );
	if( m_pRightBtn != NULL ) 
	{
		CFont * pFont =
			( const_cast < CExtToolControlBar * > ( this ) )
				-> OnGetToolbarFont(
					( (!bHorz) && (!m_bPaletteMode) )
						? true : false,
					true,
					const_cast < CExtToolControlBar * > ( this )
					);
		if( pFont->GetSafeHandle() == NULL )
			pFont = (&( PmBridge_GetPM()->m_FontNormal ));
		CClientDC dc( ( const_cast < CExtToolControlBar * > ( this ) ) );
		CFont * pOldFont = dc.SelectObject( pFont );
		CSize sizeTBBRight =
			m_pRightBtn->CalculateLayout(
				dc,
				sizeDefButton,
				bHorz //|| m_bPaletteMode
				);
		_size += sizeTBBRight;
		dc.SelectObject( pOldFont );
	}
//	if( IsFloating() )
//		return _size;

int nCountOfButtons = GetButtonsCount();
	if( nCountOfButtons > 0 )
	{
		CSize _sizeAdjust( 0, 0 );
		CClientDC dc( ( const_cast < CExtToolControlBar * > ( this ) ) );
		CFont * pFont =
			( const_cast < CExtToolControlBar * > ( this ) )
				-> OnGetToolbarFont(
					( (!bHorz) && (!m_bPaletteMode) )
						? true : false,
					true,
					const_cast < CExtToolControlBar * > ( this )
					);
		if( pFont->GetSafeHandle() == NULL )
			pFont = (&( PmBridge_GetPM()->m_FontNormal ));
		CFont * pOldFont = dc.SelectObject( pFont );
		for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
		{
			CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
			ASSERT_VALID( pTBB );
			
			if( pTBB->IsSeparator() )
				continue;
			if( pTBB->GetStyle() & TBBS_HIDDEN )
				continue;

			if(		nBtnIdx == (nCountOfButtons-1)
				&&	m_pRightBtn != NULL
				)
			{
				ASSERT_VALID( m_pRightBtn );
				ASSERT( m_pRightBtn == pTBB );
				break;
			}
			CSize sizeTBB(
				pTBB->CalculateLayout(
					dc,
					sizeDefButton,
					bHorz
					)
				);
			if( bHorz )
			{
				if( _sizeAdjust.cx > 0 )
					_sizeAdjust.cx = min( _sizeAdjust.cx, sizeTBB.cx );
				else
					_sizeAdjust = sizeTBB;
			}
			else
			{
				if( _sizeAdjust.cy > 0 )
					_sizeAdjust.cy = min( _sizeAdjust.cy, sizeTBB.cy );
				else
					_sizeAdjust = sizeTBB;
			}
		} // for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
		if( bHorz )
		{
			if( _sizeAdjust.cx > 0 )
				_size.cx += _sizeAdjust.cx;
		}
		else
		{
			if( _sizeAdjust.cy > 0 )
				_size.cy += _sizeAdjust.cy;
		}
		dc.SelectObject( pOldFont );
	} // if( nCountOfButtons > 0 )

CRect rcClient, rcWindow;
	GetClientRect( &rcClient );
	GetWindowRect( &rcWindow );
	if( bHorz )
		_size.cx += rcWindow.Width() - rcClient.Width();
	else
		_size.cy += rcWindow.Height() - rcClient.Height();

	return _size;
}

bool CExtToolControlBar::_GetFullRowMode() const
{
	if( m_bHelperTempFullRowMode )
		return true;
	return CExtControlBar::_GetFullRowMode();
}

bool CExtToolControlBar::OnQueryMultiRowLayout() const
{
	ASSERT_VALID( this );
	if(		m_bPresubclassDialogMode
		||	m_pDockSite == NULL
		||	m_bPaletteMode
		)
		return false;
	if( IsFloating() )
		return false;
	if( ! _IsDockSiteCustomizeMode() )
		return false;
	return true;
}

CExtControlBar::InternalAffixmentData *
	CExtToolControlBar::_AffixmentGetOuter()
{
	ASSERT_VALID( this );
	return &m_AffixmentData;
}

void CExtToolControlBar::_AffixmentSetOuter(
	CExtControlBar::InternalAffixmentData * pAffixmentData
	)
{
	ASSERT_VALID( this );
	ASSERT( pAffixmentData != NULL );
	m_AffixmentData = *pAffixmentData;
}

bool CExtToolControlBar::_AffixmentIsAlignedHW() const
{
	ASSERT_VALID( this );
	if( m_bPaletteMode )
		return false;
	return true;
}

LRESULT CExtToolControlBar::_ContinueHelpHitTestMode(
	CExtToolControlBar * pBar
	)
{
	ASSERT_VALID( pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL );
CFrameWnd * pDockSiteOrOther = pBar->m_pDockSite;
	if(		pDockSiteOrOther == NULL
		||	(! pDockSiteOrOther->m_bHelpMode )
		)
	{
		pDockSiteOrOther = pBar->GetParentFrame();
		if(		pDockSiteOrOther != NULL
			&&	pDockSiteOrOther->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) )
			)
			pDockSiteOrOther = pDockSiteOrOther->GetParentFrame();
		if(		pDockSiteOrOther == NULL
			||	(! pDockSiteOrOther->m_bHelpMode )
			)
			return -3L;
	}
static bool g_bContinuedMode = false;
	if( g_bContinuedMode )
		return -3L;
HWND hWndDockSite = pDockSiteOrOther->m_hWnd;
HWND hWndBar = pBar->m_hWnd;
MSG msg;
	g_bContinuedMode = true;
LRESULT lResult = -3L;
	for( ; true; )
	{
		if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if(!(	::IsWindow( hWndDockSite )
				&&	::IsWindow( hWndBar )
				&&	pDockSiteOrOther->m_bHelpMode
				))
				break;
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		
		if( msg.hwnd != NULL )
		{
			if( msg.message == WM_LBUTTONDOWN )
			{
				CWnd * pWndPerm = CWnd::FromHandlePermanent(msg.hwnd);
				if( pWndPerm != NULL )
				{
					CExtControlBar * pTestBar =
						DYNAMIC_DOWNCAST( CExtControlBar, pWndPerm );
					if( pTestBar != NULL )
					{
						lResult = pTestBar->DoHelpHitTest( CPoint(msg.lParam) );
						if( lResult > 0 )
						{
							PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
							break;
						}
						lResult = -3L;
					} // if( pTestBar != NULL )
				} // if( pWndPerm != NULL )
			} // if( msg.message == WM_LBUTTONDOWN )
			else if( msg.message == WM_MOUSEMOVE )
			{
				CWnd * pWndPerm = CWnd::FromHandlePermanent(msg.hwnd);
				if( pWndPerm != NULL )
				{
					CExtControlBar * pTestBar =
						DYNAMIC_DOWNCAST( CExtControlBar, pWndPerm );
					if( pTestBar != NULL )
					{
						CPoint pt( (DWORD)msg.lParam );
						if( pTestBar->_OnMouseMoveMsg((UINT)msg.wParam,pt) )
						{
							PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
							continue;
						} // if( pTestBar->_OnMouseMoveMsg((UINT)msg.wParam,pt) )
					} // if( pTestBar != NULL )
				} // if( pWndPerm != NULL )
			} // else if( msg.message == WM_MOUSEMOVE )
			else if( msg.message == WM_COMMAND )
			{
				if( ((HIWORD(msg.wParam))==0) || ((HIWORD(msg.wParam))==1) )
				{
					lResult = (LRESULT)( LOWORD(msg.wParam) );
					PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
					break;
				} // if( ((HIWORD(msg.wParam))==0) || ((HIWORD(msg.wParam))==1) )
			} // else if( msg.message == WM_COMMAND )
			else if( msg.message == WM_SYSCOMMAND )
			{
				switch( msg.wParam )
				{
				case SC_SIZE:
				case SC_MOVE:
				case SC_MINIMIZE:
				case SC_MAXIMIZE:
				case SC_NEXTWINDOW:
				case SC_PREVWINDOW:
				case SC_CLOSE:
				case SC_RESTORE:
				case SC_TASKLIST:
					lResult =
						HID_BASE_COMMAND +
							ID_COMMAND_FROM_SC( msg.wParam );
					PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
					break;
				default:
					lResult = (LRESULT)( msg.wParam );
					PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
					break;
				}
				break;
			} // else if( msg.message == WM_SYSCOMMAND )
		} // if( msg.hwnd != NULL )

		if( !AfxGetThread()->PumpMessage() )
			break;
	} // for( LRESULT lResult = -3L; true; )
	g_bContinuedMode = false;
	return lResult;
}

void CExtToolControlBar::GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle) const
{
	ASSERT_VALID(this);

CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	if( pTBB == NULL )
	{
		ASSERT( FALSE );
		nID = 0;
		nStyle = 0;
		return;
	}
	ASSERT_VALID( pTBB );
	nID = pTBB->GetCmdID(false);
	nStyle = pTBB->GetStyle();
}

void CExtToolControlBar::SetButtonInfo(int nIndex, UINT nID, UINT nStyle)
{
	ASSERT_VALID(this);

CExtBarButton * pTBB =
		_GetButtonPtr(nIndex);
	if( pTBB == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT_VALID( pTBB );
	pTBB->SetCmdID( nID );
	pTBB->SetStyle( nStyle );
	_InvalidateButton( nIndex );
	//UpdateWindow();
}

CFont * CExtToolControlBar::OnGetToolbarFont(
	bool bVert,
	bool bMeasureFont,
	CObject * pQuerySrc
	)
{
	ASSERT_VALID(this);
	pQuerySrc; bVert;
	if( ! bMeasureFont )
		return NULL;
HFONT hFont = PmBridge_GetPM()->Toolbar_GetFont( ! bVert, this );
	if( hFont != NULL )
		return CFont::FromHandle( hFont );
	ASSERT( PmBridge_GetPM()->m_FontNormal.GetSafeHandle() != NULL );
	return &(PmBridge_GetPM()->m_FontNormal);
}

bool CExtToolControlBar::_DoFtLayoutQuery(
	FtLayoutQuery * pFtLayoutQuery,
	CDC * pToolBarOutputDC
	) const
{
	ASSERT_VALID( this );
	ASSERT( pFtLayoutQuery != NULL || pToolBarOutputDC != NULL );
bool bPaintRowBkMode = PmBridge_GetPM()->FixedBar_IsPaintRowBkMode( this );
	if( ! bPaintRowBkMode )
		return false;

#ifdef _DEBUG
	if( pFtLayoutQuery != NULL )
	{
		ASSERT( pFtLayoutQuery->m_pToolBar == this );
		ASSERT_VALID( pFtLayoutQuery->m_pMiniFrame );
		ASSERT( pFtLayoutQuery->m_arrRowBtnIndices.GetSize() == 0 );
		ASSERT( pFtLayoutQuery->m_arrRowRects.GetSize() == 0 );
	} // if( pFtLayoutQuery != NULL )
	if( pToolBarOutputDC != NULL )
	{
		ASSERT_VALID( pToolBarOutputDC );
		ASSERT( pToolBarOutputDC->GetSafeHdc() != NULL );
	} // if( pToolBarOutputDC != NULL )
#endif // _DEBUG
CExtBarButton::e_wrap_t _evt = GetBtnWrapCode();
int nCountOfButtons = _GetButtonsCountImpl();
CRect rcClient;
	GetClientRect( &rcClient );
CRect rcRowBk( rcClient );
	rcRowBk.bottom = rcRowBk.top;
	int nBtnIdx = 0;
	for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		
		if(		(! pTBB->IsVisible() )
			||	(pTBB->GetStyle() & TBBS_HIDDEN) != 0
			)
			continue;
		if(		pTBB->IsSeparator()
			&&	nBtnIdx < (nCountOfButtons-1)
			)
		{
			CExtBarButton * pNextTBB = _GetButtonPtr( nBtnIdx+1 );
			ASSERT_VALID( pNextTBB );
			if( pNextTBB->GetStyle() & TBBS_HIDDEN )
				continue;
		}
		
		if( !pTBB->IsSeparator() )
		{
			CRect rcTBB = *pTBB;
			rcRowBk.top = min( rcRowBk.top, rcTBB.top );
			rcRowBk.bottom = max( rcRowBk.bottom, rcTBB.bottom );
		}
		if( pTBB->IsWrap(_evt) && ( m_dwStyle & CBRS_ORIENT_HORZ ) )
		{
			if( ! rcRowBk.IsRectEmpty() )
			{
				if( pFtLayoutQuery != NULL )
				{
					pFtLayoutQuery->m_arrRowBtnIndices.Add( nBtnIdx );
					pFtLayoutQuery->m_arrRowRects.Add( rcRowBk );
				} // pFtLayoutQuery
				if( pToolBarOutputDC != NULL )
				{
					PmBridge_GetPM()->PaintFloatToolbarRowBk(
						*pToolBarOutputDC,
						this,
						nBtnIdx,
						rcRowBk
						);
				} // if( pToolBarOutputDC != NULL )
			} // if( ! rcRowBk.IsRectEmpty() )
			rcRowBk.top = rcRowBk.bottom + 1;
		}
	} // for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	if( ! rcRowBk.IsRectEmpty() )
	{
		if( pFtLayoutQuery != NULL )
		{
			pFtLayoutQuery->m_arrRowBtnIndices.Add( nBtnIdx );
			pFtLayoutQuery->m_arrRowRects.Add( rcRowBk );
		} // pFtLayoutQuery
		if( pToolBarOutputDC != NULL )
		{
			PmBridge_GetPM()->PaintFloatToolbarRowBk(
				*pToolBarOutputDC,
				this,
				nBtnIdx,
				rcRowBk
				);
		} // if( pToolBarOutputDC != NULL )
	} // if( ! rcRowBk.IsRectEmpty() )
	return true;
}

void CExtToolControlBar::DoPaint( CDC * pDC )
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	CExtPaintManager::stat_ExcludeChildAreas(
		*pDC,
		*this
		);
CRect rcClient;
	GetClientRect( &rcClient );
CExtMemoryDC dc( pDC, &rcClient );
//CExtMemoryDC dc( pDC );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( dc.GetSafeHdc() != NULL )
		pDC = &dc;
	DoEraseBk( pDC );
	pDC->SetTextColor(
		PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_TEXT_OUT, this )
		);
	pDC->SetBkMode( TRANSPARENT );
CFont * pFont =
		OnGetToolbarFont(
			( ( ( m_dwStyle & CBRS_ORIENT_HORZ ) == 0 ) && (!m_bPaletteMode) )
				? true : false,
			false,
			this
			);
	if( pFont->GetSafeHandle() == NULL )
		pFont = (&( PmBridge_GetPM()->m_FontNormal ));
CFont * pOldFont =
		pDC->SelectObject( pFont );
	//ASSERT( pOldFont != NULL );
int nCountOfButtons = _GetButtonsCountImpl();
bool bMultiRowLayout = OnQueryMultiRowLayout();
CExtBarButton::e_wrap_t _evt = GetBtnWrapCode();

INT nRowDistance = PmBridge_GetPM()->FixedBar_GetRowDistance( this );
	 // pre-fill row backgrounds
	_DoFtLayoutQuery( NULL, pDC );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtBarButton * pCustomizeActiveTBB = NULL;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		
		if(		(! pTBB->IsVisible() )
			||	(pTBB->GetStyle() & TBBS_HIDDEN) != 0
			)
			continue;
		if(		pTBB->IsSeparator()
			&&	nBtnIdx < (nCountOfButtons-1)
			)
		{
			CExtBarButton * pNextTBB = _GetButtonPtr( nBtnIdx+1 );
			ASSERT_VALID( pNextTBB );
			if( pNextTBB->GetStyle() & TBBS_HIDDEN )
				continue;
		}
		
		CRect rcTBB = *pTBB;
		if( !pDC->RectVisible( &rcTBB ) )
			continue;
		if( !pTBB->IsSeparator() )
		{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			if(		GetCustomizeSite() != NULL
				&&	pTBB->IsCustomizeActiveItem()
				)
			{
				ASSERT( pCustomizeActiveTBB == NULL );
				pCustomizeActiveTBB = pTBB;
			} // if( GetCustomizeSite() != NULL ...
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			pTBB->Paint( *pDC );
			continue;
		} // if( !pTBB->IsSeparator() )
		bool bSeparatorHorz =
			(		( m_dwStyle & CBRS_ORIENT_HORZ ) != 0
				||	m_bPaletteMode
				)
				? true : false;
		bool bSeparatorRotate = false;
		CRect rectSeparator( rcTBB );
		if( (!bSeparatorHorz) && (!bMultiRowLayout) )
		{
			bSeparatorHorz = true;
			rcTBB.OffsetRect( 0, -rcTBB.Height() );
		}
		if( pTBB->IsWrap(_evt) )
		{
			if( bSeparatorHorz )
			{
				rectSeparator.left = rcClient.left;
				rectSeparator.right = rcClient.right;
				rectSeparator.top = rcTBB.bottom;
				rectSeparator.bottom =
					rectSeparator.top
					+ nRowDistance;
				bSeparatorRotate = true;
			} // if( bSeparatorHorz )
			else
			{
				rectSeparator.top = rcClient.top;
				rectSeparator.bottom = rcClient.bottom;
				rectSeparator.left =
					rcTBB.left
					- nRowDistance;
				rectSeparator.right =
					rectSeparator.left
					+ nRowDistance;
				bSeparatorRotate = true;
			} // else from if( bSeparatorHorz )
		} // if( pTBB->IsWrap(_evt) )
		if( nRowDistance == 0 && bSeparatorRotate )
			continue;
		if( bSeparatorRotate )
			bSeparatorHorz = !bSeparatorHorz;
		rectSeparator.DeflateRect(
			bSeparatorHorz ? 0 : 2,
			bSeparatorHorz ? 2 : 0
			);
		if(		bMultiRowLayout
			&&	m_pRightBtn != NULL
			&&	m_pRightBtn->IsVisible()
			&&	bSeparatorRotate
			)
		{
			CRect rcRightBtn = *m_pRightBtn;
			if( bSeparatorHorz )
			{
				if( ( m_dwStyle & CBRS_ORIENT_HORZ ) == 0 )
				{
					if( rectSeparator.bottom > (rcRightBtn.top-__EXT_TB_BUTTON_OUTER_MARGIN) )
						rectSeparator.bottom = (rcRightBtn.top-__EXT_TB_BUTTON_OUTER_MARGIN);
				}
			} // if( bSeparatorHorz )
			else
			{
				if( ( m_dwStyle & CBRS_ORIENT_HORZ ) != 0 )
				{
					if( rectSeparator.right > (rcRightBtn.left-__EXT_TB_BUTTON_OUTER_MARGIN) )
						rectSeparator.right = (rcRightBtn.left-__EXT_TB_BUTTON_OUTER_MARGIN);
				}
			} // else from if( bSeparatorHorz )
		}
		PmBridge_GetPM()->PaintSeparator(
			*pDC,
			rectSeparator,
			bSeparatorHorz,
			true,
			this
			);
		// DEBUG SEPARATOR AREA PAINTING:
		// pDC->FillSolidRect( &rectSeparator, RGB(0,255,0) );
	} // for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_pDragTargetTBB != NULL && (abs(m_nDragTargetShift)) == 1 )
	{
		ASSERT_VALID( m_pDragTargetTBB );
		CRect rcItem = *m_pDragTargetTBB;
		PmBridge_GetPM()->PaintDropDivider(
			*pDC,
			rcItem,
			(m_nDragTargetShift < 0) ? true : false,
			( ( m_dwStyle & CBRS_ORIENT_HORZ ) != 0 ) ? true : false
			);
	} // if( m_pDragTargetTBB != NULL && (abs(m_nDragTargetShift)) == 1 )
	if(		m_pDragSourceTBB != NULL
		||	pCustomizeActiveTBB != NULL
		)
	{
		CRect rcItem =
			(pCustomizeActiveTBB != NULL)
			? (*pCustomizeActiveTBB)
			: (*m_pDragSourceTBB)
			;
		PmBridge_GetPM()->PaintDragSourceRect(
			*pDC,
			rcItem
			);
	} // if( m_pDragSourceTBB != NULL ...
CExtCustomizeSite * pSite = GetCustomizeSite();
	if( pSite != NULL )
	{
		CExtCustomizeCmdTreeNode * pPopupDropTargetNode =
			pSite->GetPopupDropTargetNode();
		if( pPopupDropTargetNode != NULL )
		{
			ASSERT_VALID( pPopupDropTargetNode );
			for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
			{
				CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
				ASSERT_VALID( pTBB );
				if(		(! pTBB->IsVisible() )
					||	(pTBB->GetStyle() & TBBS_HIDDEN) != 0
					)
					continue;
				if( pTBB->IsSeparator() )
					continue;
				CRect rcTBB = *pTBB;
				if( !pDC->RectVisible( &rcTBB ) )
					continue;
				CExtCustomizeCmdTreeNode * pNode =
					pTBB->GetCmdNode();
				if( ((LPVOID)pNode) != ((LPVOID)pPopupDropTargetNode) )
					continue;
				PmBridge_GetPM()->PaintDragSourceRect(
					*pDC,
					rcTBB
					);
				break;
			} // for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
		} // if( pPopupDropTargetNode != NULL )
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	pDC->SelectObject( pOldFont );
	PmBridge_GetPM()->OnPaintSessionComplete( this );
}

void CExtToolControlBar::_InvalidateButton( int nIndex )
{
CExtBarButton * pTBB = _GetButtonPtr( nIndex );
	if( pTBB == NULL )
		return;
	ASSERT_VALID( pTBB );
CRect rect = pTBB->Rect();
	if( rect.IsRectEmpty() )
		return;
	if( pTBB->OnInvalidateButton() )
		return;
	if(		pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton))
		&&	PmBridge_GetPM()->GetCb2DbTransparentMode(this)
		)
	{
		const CSize _sizeRoundedAreaMerics = PmBridge_GetPM()->FixedBar_GetRoundedAreaMerics();
		rect.InflateRect( _sizeRoundedAreaMerics.cx + 1, _sizeRoundedAreaMerics.cy + 1 );
	}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if(		pTBB == m_pDragSourceTBB
		||	pTBB == m_pDragTargetTBB
		)
	{
		CSize _sizeDDM = PmBridge_GetPM()->GetDropDividerMerics();
		rect.InflateRect( _sizeDDM );
		rect.InflateRect( 1, 1 );
	} // if( pTBB == m_pDragSourceTBB )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	rect.InflateRect( 1, 1 );
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if(		m_pDockBar == NULL
		&&	IsKindOf( RUNTIME_CLASS(CExtRibbonBar) )
		&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl != NULL
		&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement()
		)
	{
		CWnd * pWndParent = GetParent();
		if( pWndParent != NULL )
		{
			CRect _rectTest = rect;
			ClientToScreen( &_rectTest );
			pWndParent->ScreenToClient( &_rectTest );
			if(		_rectTest.bottom <= 0
				||	(	_rectTest.top <= 0
					&&	pTBB->IsKindOf( RUNTIME_CLASS(CExtRibbonButtonFile) )
					)
				)
				pWndParent->SendMessage( WM_NCPAINT );
		} // if( pWndParent != NULL )
	} // if( m_pDockBar == NULL ...
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	InvalidateRect( &rect );
}

bool CExtToolControlBar::OnDeliverCmd( CExtBarButton * pTBB )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB;
	return false;
}

void CExtToolControlBar::AnimationSite_OnProgressShutdownTimer( UINT nTimerID )
{
	ASSERT_VALID( this);
HWND hWndOwn = GetSafeHwnd();
	CExtControlBar::AnimationSite_OnProgressShutdownTimer( nTimerID );
	if( hWndOwn != NULL && ::IsWindow( hWndOwn ) )
	{
		bool bUpdateHB = true;
		if( ! ( m_pDockSite->GetSafeHwnd() != NULL && m_pDockSite->IsWindowEnabled() ) )
			bUpdateHB = false;
		else if( ! IsWindowEnabled() )
			bUpdateHB = false;
		else
		{
			CWnd * pWndOwner = GetOwner();
			if( ! ( pWndOwner->GetSafeHwnd() != NULL && pWndOwner->IsWindowEnabled() ) )
				bUpdateHB = false;
		}
		if( bUpdateHB )
			_UpdateHoverButton( CPoint(-1,-1), false );
	}
}

__EXT_MFC_INT_PTR CExtToolControlBar::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	) const
{
	ASSERT_VALID( this );

	if(		(! IsDisplayScreenTips() )
		||	CExtToolControlBar::g_bMenuTracking
		||	CExtPopupMenuWnd::IsMenuTracking()
		||	OnAdvancedPopupMenuTipWndGet() != NULL
		||	m_bSuspendTips
		)
		return -1;

__EXT_MFC_INT_PTR nToolTipHit = (__EXT_MFC_INT_PTR)
		((CExtToolControlBar*)this)->
			HitTest(point);
	if( nToolTipHit >= 0 )
	{
		CExtBarButton * pTBB =
			_GetButtonPtr( (int)nToolTipHit );
		ASSERT_VALID( pTBB );
		if( pTBB != NULL )
		{
			nToolTipHit = pTBB->OnToolHitTest( point, pTI );
			if( nToolTipHit != -1 )
				return nToolTipHit;
		}
	} // if( nToolTipHit >= 0 )

	nToolTipHit =
		CExtControlBar::OnToolHitTest(
			point,
			pTI
			);
	return nToolTipHit;
}

bool CExtToolControlBar::OnQueryHoverBasedMenuTracking(
	const CExtBarButton * pTBB
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	pTBB;
	return true;
}

int CExtToolControlBar::_HitTestImpl(
	CPoint point,
	UINT nButtonStyleInclude, // = 0, // button must have style
	UINT nButtonStyleExclude, // = 0  // button must have not style
	bool bAllowFindNearest // = false // if did not hit - find nearest
	) const
{
	ASSERT_VALID( this );
	if( m_hWnd == NULL || (! ::IsWindow(m_hWnd) )  )
		return -1; // nowhere
	if( (!bAllowFindNearest) && ::GetCapture() == m_hWnd )
	{
		CPoint ptScreen( point );
		ClientToScreen( &ptScreen );
		HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
		if( hWndFromPoint != m_hWnd )
		{
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
			if(		m_pDockBar == NULL
				&&	IsKindOf( RUNTIME_CLASS(CExtRibbonBar) )
				&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl != NULL
				&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement()
				&&	hWndFromPoint == ::GetParent( m_hWnd )
				)
			{
			}
			else
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
				return -1; // nowhere
		}
	}
int nCountOfButtons = _GetButtonsCountImpl();
int nNearestIdx = -1;
int nNearestDistance = 32767;
bool bNearestRowMatch = false;
	for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB =
			_GetButtonPtr(nBtnIdx);
		ASSERT_VALID( pTBB );
		
		if(		(! pTBB->IsVisible() )
			||	(pTBB->GetStyle() & TBBS_HIDDEN) != 0
			)
			continue;
		if(		pTBB->IsSeparator()
			&&	nBtnIdx < (nCountOfButtons-1)
			)
		{
			CExtBarButton * pNextTBB = _GetButtonPtr( nBtnIdx+1 );
			ASSERT_VALID( pNextTBB );
			if( pNextTBB->GetStyle() & TBBS_HIDDEN )
				continue;
		}
		
		if( (pTBB->GetStyle() & nButtonStyleInclude)
				!= nButtonStyleInclude
			)
			continue;
		if( (pTBB->GetStyle() & nButtonStyleExclude)
				!= 0
			)
			continue;
		CRect rect = *pTBB;
		if( rect.PtInRect(point) )
			return nBtnIdx;
		if( !bAllowFindNearest )
			continue;
		CPoint ptCenter = rect.CenterPoint();
		double lfDX = double( point.x - ptCenter.x );
		double lfDY = double( point.y - ptCenter.y );
		int nDistance = (int)
			::sqrt( lfDX*lfDX + lfDY*lfDY );
		bool bRowMatch = false;
		if( (m_dwStyle & CBRS_ORIENT_HORZ) != 0 )
		{
			bRowMatch =
				(rect.top <= point.y && point.y <= rect.bottom)
					? true : false;
		} // if( (m_dwStyle & CBRS_ORIENT_HORZ) != 0 )
		else
		{
			bRowMatch =
				(rect.left <= point.x && point.x <= rect.right)
					? true : false;
		} // if( (m_dwStyle & CBRS_ORIENT_HORZ) != 0 )
		if( (!bNearestRowMatch) && bRowMatch )
		{
			nNearestIdx = nBtnIdx;
			nNearestDistance = nDistance;
			bNearestRowMatch = true;
		} // if( (!bNearestRowMatch) && bRowMatch )
		else if(
				bNearestRowMatch == bRowMatch
			&&	nNearestDistance > nDistance
			)
		{
			nNearestIdx = nBtnIdx;
			nNearestDistance = nDistance;
		}
	} // for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	if( bAllowFindNearest && nNearestIdx >= 0 )
		return nNearestIdx;
	return -1; // nowhere
}

int CExtToolControlBar::HitTest(
	CPoint point // in window relative coords
	) const
{
	return
		_HitTestImpl(
			point,
			0,
			TBBS_SEPARATOR
			);
}

/////////////////////////////////////////////////////////////////////////////
// CExtToolControlBar message handlers

BEGIN_MESSAGE_MAP(CExtToolControlBar, CExtControlBar)
	//{{AFX_MSG_MAP(CExtToolControlBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_NCCALCSIZE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE( CExtToolControlBar::g_nMsgTrackButtonMenu, OnTrackButtonMenu )
	ON_REGISTERED_MESSAGE( CExtPopupMenuWnd::g_nMsgNotifyMenuClosed, OnMenuClosed )
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

LRESULT CExtToolControlBar::OnMenuClosed( WPARAM wParam, LPARAM lParam )
{
	wParam;
	lParam;
	_UpdateHoverButton( CPoint(-1,-1), false );
	return 0;
}

LRESULT CExtToolControlBar::OnTrackButtonMenu(WPARAM wParam,LPARAM lParam)
{
	if( !(	::IsWindowEnabled(
				m_bPresubclassDialogMode
					? GetOwner()->GetSafeHwnd()
					: _GetDockingFrameImpl()->GetSafeHwnd()
				)
			)
		) 
		return 0;
int nCountOfButtons = GetButtonsCount();
	ASSERT( nCountOfButtons >= 0 );
	if( nCountOfButtons == 0 )
		return 0;
int nIndex = int(wParam);
	if( nIndex < 0 || nIndex>=nCountOfButtons )
	{
		_CloseTrackingMenus();
		return 0;
	}
CExtBarButton * pTBB = _GetButtonPtr(nIndex);
	ASSERT_VALID( pTBB );
BOOL bDroppedMode =
		CExtPopupMenuWnd::IsMenuTracking();
	if( ! pTBB->IsAbleToTrackMenu() )
		bDroppedMode = FALSE;
	if( bDroppedMode )
	{
		CExtToolControlBar::TrackButtonMenu(
			nIndex,
			true,
			false,
			(lParam != 0) ? true : false
			);
		return 0;
	}
	_CloseTrackingMenus();
	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();

	return 0;
}

void CExtToolControlBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_bRowResizing || m_bRowRecalcing || m_bDragging )
	{
		CExtControlBar::OnLButtonDown(nFlags, point);
		return;
	}
	
	_ActivateOnClick();

	m_nBtnIdxCapture = HitTest( point );
	if( m_nBtnIdxCapture < 0 ) // nothing hit
	{
		CPoint ptScreen = point;
		ClientToScreen( &ptScreen );
		bool bOwnMessage = true;
		HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
		if( hWndFromPoint != m_hWnd )
		{
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
			if(		m_pDockBar == NULL
				&&	IsKindOf( RUNTIME_CLASS(CExtRibbonBar) )
				&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl != NULL
				&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement()
				&&	hWndFromPoint == ::GetParent( m_hWnd )
				)
			{
			}
			else
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
				bOwnMessage = false; // nowhere
		}
		if( bOwnMessage )
			CExtControlBar::OnLButtonDown(nFlags, point);
		else
			SendMessage( WM_CANCELMODE );
		return;
	}

CExtBarButton * pTBB =
		_GetButtonPtr(m_nBtnIdxCapture);
	ASSERT_VALID( pTBB );
	ASSERT( !(pTBB->IsSeparator()) );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_bCustomizationAllowed )
	{
		CExtCustomizeSite * pSite = GetCustomizeSite();
		if( pSite != NULL )
		{
			CExtCustomizeCmdTreeNode * pNodeC =
				pTBB->GetCmdNode( false );
			if( pNodeC != NULL )
			{
				ASSERT_VALID( pNodeC );
				bool bCustomizeMode = pSite->IsCustomizeMode();
				bool bAlt =
					( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 )
						? true : false;
				if( bAlt || bCustomizeMode )
				{
					int nBtnDragSrc = m_nBtnIdxCapture;
					m_nBtnIdxCapture = -1;
					if( !pTBB->CanStartCommandDrag() )
						return;
					if( bCustomizeMode )
					{
	#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
						int nWidthMin = 0, nWidthMax = 0;
						int nInitialResizingStateH =
							pTBB->GetInitialResizingStateH(
								point,
								&nWidthMin,
								&nWidthMax
								);
						if(		pTBB->IsCustomizeActiveItem()
							&&	nInitialResizingStateH != 0
							)
						{ // if can resize button horizontally
							bool bCtrl =
								( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 )
									? true : false;
							bool bShift =
								( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 )
									? true : false;
							if( !( bAlt || bCtrl || bShift ) )
							{
								CRect rcItem = pTBB->Rect();
								if( !pSite->OnCmdNodeResizeH(
										nInitialResizingStateH,
										m_hWnd,
										pTBB,
										pNodeC,
										nWidthMin,
										nWidthMax,
										rcItem
										)
									)
									return; // resizing is canceled
								CFrameWnd * pFrame = GetParentFrame();
								if( pFrame != NULL )
								{
									pFrame->RecalcLayout();
									pSite->ActiveItemSet( pTBB, GetSafeHwnd() );
								}
								return;
							} // if( !( bAlt || bCtrl || bShift ) )
						} // if can resize button horizontally
	#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
						pTBB->SetCustomizeActiveItem();
					} // if( bCustomizeMode )
	//				CExtCmdItem * pCmdItem =
	//					g_CmdManager->CmdGetPtr(
	//						g_CmdManager->ProfileNameFromWnd( m_hWnd ),
	//						pTBB->GetCmdID()
	//						);
	//				if( pCmdItem == NULL )
	//				{
	//					ASSERT( FALSE );
	//					return;
	//				}
					CExtCustomizeCmdTreeNode * pNodeI =
						pTBB->GetCmdNode( true );
					SendMessage( WM_CANCELMODE );
					CRect rcItem( 0, 0, 1, 1 );

					CExtCustomizeCmdTreeNode * pPopupDropTargetNode =
						pSite->GetPopupDropTargetNode();
					if(		bCustomizeMode
						&&	pTBB->IsAbleToTrackMenu( true )
						&&	pPopupDropTargetNode != pNodeC
						)
					{
	//					pSite->DropTargetPopupCancel();
	//					pSite->DropTargetPopupCancel();
						pSite->DropTargetPopupCancelEx( -2 );
						CRect rcBtn = *pTBB;
						ClientToScreen( &rcBtn );
						CPoint ptTrack( point );
						ClientToScreen( &ptTrack );
						DWORD dwTrackFlags = pTBB->OnGetTrackPopupFlags();
						pSite->DropTargetPopupTrack(
							pNodeC,
							rcBtn,
							ptTrack,
							dwTrackFlags
							);
					} // if( bCustomizeMode && pTBB->IsAbleToTrackMenu() ...
					else
						pSite->DropTargetPopupCancel();

					m_pDragSourceTBB = pTBB;
					m_pDragTargetTBB = NULL;
					m_nDragTargetShift = 0;
					m_bHelperSeparatorAction = false;
					_InvalidateButton( nBtnDragSrc );
					//UpdateWindow();
					if( CExtControlBar::stat_DoDragDetect( m_hWnd, point ) )
						pSite->DoDragCmdNode( this, pNodeI, pNodeC, rcItem );
					m_pDragSourceTBB = NULL;
					m_pDragTargetTBB = NULL;
					m_nDragTargetShift = 0;
					m_bHelperSeparatorAction = false;
					return;
				} // if( bAlt || bCustomizeMode )
			} // if( pNodeC != NULL )
		} // if( pSite != NULL )
	} // if( m_bCustomizationAllowed )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	// update the button before checking for disabled status
	_UpdateButton(m_nBtnIdxCapture);

	if(		m_nBtnIdxMenuTracking == m_nBtnIdxCapture
		||	(	pTBB->IsDisabled()
			&&	( ! pTBB->CanBePressedInDisabledState() )
			)
		)
	{
		m_nBtnIdxCapture = -1;
		return;     // don't press it
	} // if( m_nBtnIdxMenuTracking == m_nBtnIdxCapture ...

	pTBB->ModifyStyle( TBBS_PRESSED );
	pTBB->OnPressedTrackingStart();
	_InvalidateButton( m_nBtnIdxCapture );
//	UpdateWindow(); // immediate feedback

HWND hWndCtrlTBB = pTBB->CtrlGet()->GetSafeHwnd();
HWND hWndOwn = m_hWnd;
	pTBB->OnClick( point, true );
	if( ! ::IsWindow(hWndOwn) )
		return;
	if( _GetIndexOf( pTBB ) < 0 )
		pTBB = NULL;
#ifdef _DEBUG
	else
	{
		ASSERT_VALID( pTBB );
	}
#endif // _DEBUG

	if( ! g_bMenuTracking )
	{
		if(		hWndCtrlTBB == NULL
			||	(	::IsWindow(hWndCtrlTBB)
				&&	( (::__EXT_MFC_GetWindowLong(hWndCtrlTBB,GWL_STYLE)) & WS_VISIBLE ) == 0
				)
			)
			CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
		else
			m_nBtnIdxCapture = -1;
	} // if( ! g_bMenuTracking )
	else
		m_nBtnIdxCapture = -1;
}

bool CExtToolControlBar::_IsSimplifiedDropDownButtons() const
{
	ASSERT_VALID( this );
	return false;
}

bool CExtToolControlBar::_OnMouseMoveMsg(UINT nFlags, CPoint point)
{
__PROF_UIS_MANAGE_STATE;
	if(		CExtPopupMenuWnd::IsMenuTracking()
		&&	(! CExtToolControlBar::g_bMenuTracking )
		)
		return true;
	if(		IsFloating()
		&&	CExtMiniDockFrameWnd::g_bAutoMakeTopmostOnMouseEvent
		)
	{
		CFrameWnd * pParentFrame = GetParentFrame();
		if( pParentFrame != NULL )
			pParentFrame->BringWindowToTop();
	}
	if( m_bRowResizing || m_bRowRecalcing || m_bDragging )
		return CExtControlBar::_OnMouseMoveMsg(nFlags, point);;
	if( m_nBtnIdxCapture >= 0 )
	{
		CExtBarButton * pTBB =
			_GetButtonPtr( m_nBtnIdxCapture );
		ASSERT_VALID( pTBB );
		ASSERT( ! pTBB->IsSeparator() );
//		UINT nNewStyle = ( pTBB->GetStyle() & (~TBBS_PRESSED) );
		UINT nNewStyle = pTBB->GetStyle();
		int nBtnIdxCapture = m_nBtnIdxCapture;
		if( CExtMouseCaptureSink::GetCapture() != m_hWnd )
		{
			if( HitTest(point) != m_nBtnIdxCapture )
				nNewStyle &= (~TBBS_PRESSED);
			m_nBtnIdxCapture = -1; // lost capture
		}
		else
		{
			// should be pressed if still hitting the captured button
			if( HitTest(point) == m_nBtnIdxCapture )
				nNewStyle |= TBBS_PRESSED;
		}
//		if( (nNewStyle & TBBS_PRESSED) == 0 )
//			pTBB->OnPressedTrackingStop();
//		else
//			pTBB->OnPressedTrackingStart();
		SetButtonStyle( nBtnIdxCapture, nNewStyle );
		UpdateWindow(); // immediate feedback
		return true;
	} // if( m_nBtnIdxCapture >= 0 )

bool bHoverChanged = _UpdateHoverButton();

	if(		CExtMouseCaptureSink::GetCapture() == GetSafeHwnd()
		&&	m_nBtnIdxHover < 0
		&&	m_nBtnIdxMenuTracking < 0
		&&	m_nBtnIdxCapture < 0
		)
	{
		CPoint ptScreen;
		if( ! ::GetCursorPos(&ptScreen) )
			return false;
		bool bOwnMessage = true;
		HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
		if( hWndFromPoint != m_hWnd )
		{
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
			if(		m_pDockBar == NULL
				&&	IsKindOf( RUNTIME_CLASS(CExtRibbonBar) )
				&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl != NULL
				&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement()
				&&	hWndFromPoint == ::GetParent( m_hWnd )
				)
			{
			}
			else
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
				bOwnMessage = false; // nowhere
		}
		if( bOwnMessage )
			CExtMouseCaptureSink::ReleaseCapture();
	}

	if( (! CExtPopupMenuWnd::IsMenuTracking() ) && m_nBtnIdxHover >= 0 )
	{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeSite * pSite =
			GetCustomizeSite();
		if(		pSite == NULL
			||	(!	pSite->IsCustomizeMode() )
			)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		{
			CExtBarButton * pTBB =
				_GetButtonPtr( m_nBtnIdxHover );
			ASSERT_VALID( pTBB );
			CExtPopupMenuTipWnd * pATTW =
				OnAdvancedPopupMenuTipWndGet( pTBB );
			if( pATTW != NULL )
			{
				ASSERT( ! pTBB->IsSeparator() );
				ASSERT_VALID( pTBB );
				TOOLINFO _ti;
				::memset( &_ti, 0, sizeof(AFX_OLDTOOLINFO) );
				_ti.cbSize = sizeof( TOOLINFO );
				_ti.hinst = ::AfxGetInstanceHandle();
				if( pTBB->OnToolHitTest( point, &_ti ) > 0 )
				{
					if(		_ti.lpszText != NULL
						&&	_ti.lpszText != LPSTR_TEXTCALLBACK
						&&	_tcslen( _ti.lpszText ) > 0
						)
					{
						CRect rcArea = _ti.rect;
						ClientToScreen( &rcArea );
						if( pATTW->GetSafeHwnd() != NULL )
						{
							CRect rcTest = pATTW->GetExcludeArea();
							if( rcTest != rcArea )
								bHoverChanged = true;
						}
						OnAdvancedPopupMenuTipWndDisplay(
							*pATTW,
							rcArea,
							_ti.lpszText,
							pTBB
							);
					}
				}
				if(		_ti.lpszText != NULL
					&&	_ti.lpszText != LPSTR_TEXTCALLBACK
					)
					::free( _ti.lpszText );
			} // if( pATTW != NULL )
		}
	} // if( (! CExtPopupMenuWnd::IsMenuTracking() ) && m_nBtnIdxHover >= 0 )
	if( bHoverChanged )
		return true;

	return CExtControlBar::_OnMouseMoveMsg(nFlags,point);
}

bool CExtToolControlBar::IsForceHoverWhenMenuTracking() const
{
	ASSERT_VALID( this );
	return false;
}

void CExtToolControlBar::_CheckHitTestablePoint(
	CPoint & point
	)
{
	ASSERT_VALID( this );
CPoint ptScreen = point;
	if( point.x < 0 || point.y < 0 )
	{
		if( ! ::GetCursorPos( &ptScreen ) )
		{
			point.x = point.y = 32767;
			return;
		}
		point = ptScreen;
		ScreenToClient( &point );
	}
	else
		ClientToScreen( &ptScreen );
}

bool CExtToolControlBar::_UpdateHoverButton(
	CPoint point, // = CPoint(-1,-1) // default is use ::GetCursorPos()
	bool bEnableUpdateWindow // = true
	)
{
	ASSERT_VALID( this );
	_CheckHitTestablePoint( point );
bool bHoverChanged = false;
INT nBtnIdxHoverNew = -1;
INT nIndex, nCount = GetButtonsCount();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtBarButton * pTBB = GetButton( nIndex );
		ASSERT_VALID( pTBB );
		if(		( ! pTBB->IsVisible() )
			||	( pTBB->GetStyle() & TBBS_HIDDEN ) != 0
			)
			continue;
		bool bHover = pTBB->IsHover();
		CRect rcTBB = *pTBB;
		if(		(	pTBB->IsDisabled()
				&&	(! pTBB->IsShowTipWhenDisabled() )
				)
			||	pTBB->IsSeparator()
			||	( ! rcTBB.PtInRect( point ) )
			)
		{
			if( bHover )
			{
				bHoverChanged = true;
				pTBB->SetHover( false );
				_InvalidateButton( nIndex );
			}
			continue;
		}
		if(		pTBB->ChildButtonGetCount() > 0
			&&	nBtnIdxHoverNew >= 0
			)
			continue;
		nBtnIdxHoverNew = nIndex;
		if( bHover )
			continue;
		bHoverChanged = true;
		pTBB->SetHover( true );
		_InvalidateButton( nIndex );
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )

bool bDropDownHT = false;
	if( m_nBtnIdxHover >= 0 )
	{
		CExtBarButton * pTBB =
			_GetButtonPtr( m_nBtnIdxHover );
		ASSERT_VALID( pTBB );
		if( pTBB->GetSeparatedDropDown() )
		{
			CRect rcBtnDropDownArea = pTBB->RectDropDown();
			if( rcBtnDropDownArea.PtInRect( point ) )
				bDropDownHT = true;
		} // if( pTBB->GetSeparatedDropDown() )
	} // if( m_nBtnIdxHover >= 0 )

	if(		m_nBtnIdxHover != nBtnIdxHoverNew
		||	m_bLastDropDownHT != bDropDownHT
		)
	{
		m_bLastDropDownHT = bDropDownHT;
		if(		m_nBtnIdxHover >= 0
			&&	m_nBtnIdxHover != nBtnIdxHoverNew
			)
		{
			CExtBarButton * pTBB =
				_GetButtonPtr( m_nBtnIdxHover );
			ASSERT_VALID( pTBB );
			pTBB->OnHover(
				point,
				false,
				m_bSuspendTips
				);
		} // if( m_nBtnIdxHover >= 0 ...
		bHoverChanged = true;
//		if(		CExtControlBar::FindHelpMode( this )
//			&&	nBtnIdxHoverNew >= 0
//			)
//		{
//			CExtBarButton * pTBB = _GetButtonPtr(nBtnIdxHoverNew);
//			if(		pTBB->IsAbleToTrackMenu()
//				&&	pTBB->OnHelpHitTest(point) == -3
//				)
//				m_nBtnIdxHover = nBtnIdxHoverNew;
//		}
//		else
//			m_nBtnIdxHover = nBtnIdxHoverNew;
		m_nBtnIdxHover = nBtnIdxHoverNew;
		if( m_nBtnIdxHover >= 0 )
		{
			if( m_ptLastClientHover == point )
			{
				m_nBtnIdxHover = -1;
			}
		}
		if( m_nBtnIdxHover >= 0 )
		{
			CExtBarButton * pTBB =
				_GetButtonPtr( m_nBtnIdxHover );
			ASSERT_VALID( pTBB );
			bool bEnableHoverOnNewBtn = true;
			if(		pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiRightButton))
				||	pTBB->IsDisabled()
				)
			{
				if( g_bMenuTracking )
					bEnableHoverOnNewBtn = false;
			}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			if( bEnableHoverOnNewBtn )
			{
				CExtCustomizeSite * pSite =
					GetCustomizeSite();
				if(		pSite != NULL
					&&	(	pSite->IsCustomizeMode()
						||	pSite->GetPopupDropTargetNode() != NULL
						||	pSite->DraggedNodeGet() != NULL
						)
					)
					bEnableHoverOnNewBtn = false;
			} // if( bEnableHoverOnNewBtn )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			if( bEnableHoverOnNewBtn )
			{
				m_ptLastClientHover = point;
				pTBB->OnHover(
					point,
					true,
					m_bSuspendTips
					);
				pTBB->RedrawButton( false );
			}
		} // if( m_nBtnIdxHover >= 0 )
	} // if( m_nBtnIdxHover != nBtnIdxHoverNew )
	else if( m_nBtnIdxHover >= 0 )
	{
		CExtBarButton * pTBB = _GetButtonPtr( m_nBtnIdxHover );
		ASSERT_VALID( pTBB );
		if( pTBB->IsKindOf( RUNTIME_CLASS( CExtBarMdiRightButton ) ) )
		{
			m_ptLastClientHover = point;
			pTBB->OnHover(
				point,
				true,
				m_bSuspendTips
				);
			pTBB->RedrawButton( false );
		} // if( pTBB->IsKindOf( RUNTIME_CLASS( CExtBarMdiRightButton ) ) )
	} // else if( m_nBtnIdxHover >= 0 )

	if( m_nBtnIdxHover >= 0 )
	{
		m_ptLastClientHover = point;
		CExtBarButton * pTBB = _GetButtonPtr( m_nBtnIdxHover );
		ASSERT_VALID( pTBB );
		pTBB->OnHovering(
			point,
			m_bSuspendTips
			);
	}

	if( bHoverChanged && bEnableUpdateWindow )
		UpdateWindow();
CPoint ptScreen = point;
	ClientToScreen( &ptScreen );
bool bOwnMessage = true;
HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
	if( hWndFromPoint != m_hWnd )
	{
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
		if(		m_pDockBar == NULL
			&&	IsKindOf( RUNTIME_CLASS(CExtRibbonBar) )
			&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl != NULL
			&&	((CExtRibbonBar*)this)->m_pExtNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement()
			&&	hWndFromPoint == ::GetParent( m_hWnd )
			)
		{
		}
		else
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
			bOwnMessage = false; // nowhere
	}
	if(		( ! bOwnMessage )
		&&	::GetCapture() == m_hWnd
		)
		::ReleaseCapture();

	m_nBtnIdxHover = nBtnIdxHoverNew;
	SetTimer( __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER, 100, NULL );

// 	if( m_nBtnIdxHover >= 0 )
// 	{
// 		SetTimer( __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER, 100, NULL );
// 		SendMessage( WM_TIMER, __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER );
// 	}
//	else
//		KillTimer( __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER );

	return bHoverChanged;
}

void CExtToolControlBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(		( m_nBtnIdxCapture < 0 && m_nBtnIdxHover < 0 )
		||	m_bRowResizing
		||	m_bRowRecalcing
		||	m_bDragging
		)
	{
		CExtControlBar::OnLButtonUp(nFlags, point);
		return;     // not captured
	}
	_CheckHitTestablePoint( point );
int nBtnIdxCapture = m_nBtnIdxCapture;
HWND hWndOwn = m_hWnd;
	if( nBtnIdxCapture >= 0 )
	{
		CExtBarButton * pTempTBB = _GetButtonPtr( nBtnIdxCapture );
		ASSERT_VALID( pTempTBB );
		ASSERT( !(pTempTBB->IsSeparator()) );
		if( pTempTBB->IsPressedTracking() )
		{
			pTempTBB->OnPressedTrackingStop();
			_InvalidateButton( m_nBtnIdxCapture );
		}
		m_nBtnIdxCapture = -1;
		CExtBarButton * pCaptureTBB = NULL;
		UINT nNewStyle = 0;
		if( nBtnIdxCapture == HitTest(point) )
		{
			pCaptureTBB = pTempTBB;
			nNewStyle = ( pCaptureTBB->GetStyle() & ~TBBS_PRESSED );
			// give button a chance to update
			_UpdateButton( nBtnIdxCapture );
			// then check for disabled state
			if( !(pCaptureTBB->IsDisabled()) )
			{
				if( pCaptureTBB->GetStyle() & TBBS_CHECKBOX )
				{
					// auto check: three state => down
					if( nNewStyle & TBBS_INDETERMINATE )
						nNewStyle &= ~TBBS_INDETERMINATE;
					nNewStyle ^= TBBS_CHECKED;
				}
			} // if( !(pCaptureTBB->IsDisabled()) )
		} // if( nBtnIdxCapture == HitTest(point) )
		CExtMouseCaptureSink::ReleaseCapture();

		if( pCaptureTBB != NULL )
		{
			if( _GetIndexOf( pCaptureTBB ) != nBtnIdxCapture )
				nBtnIdxCapture = -1;
			else
			{
				pCaptureTBB->OnClick( point, false );
				if( ! ::IsWindow( hWndOwn ) )
					return;
				if( _GetIndexOf( pCaptureTBB ) != nBtnIdxCapture )
					return;
			} // else from if( _GetIndexOf(pCaptureTBB) != nBtnIdxCapture )
		}

		if( nBtnIdxCapture >= 0 )
		{
			_InvalidateButton( nBtnIdxCapture );
			if( (!g_bMenuTracking)  )
			{
				SetButtonStyle( nBtnIdxCapture, nNewStyle );
				_UpdateButton( nBtnIdxCapture );
			} // if( (!g_bMenuTracking) && nBtnIdxCapture >= 0 )
		}
	} // if( nBtnIdxCapture >= 0 )

// INT nCountOfButtons = _GetButtonsCountImpl();
// 	for( INT nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
// 	{
// 		CExtBarButton * pTBB =
// 			_GetButtonPtr( nBtnIdx );
// 		ASSERT_VALID( pTBB );
// 		pTBB->m_bHover = false;
// 	} // for( INT nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	_UpdateHoverButton( CPoint(-1,-1), false );
}

void CExtToolControlBar::OnRButtonUp(UINT nFlags, CPoint point)
{
	ASSERT_VALID( this );

INT nButton = HitTest( point );
	if( nButton < 0 )
	{
		CExtControlBar::OnRButtonUp( nFlags,  point );
		return;
	}
CExtBarButton * pTBB = _GetButtonPtr( nButton );
	ASSERT_VALID( pTBB );
	if(		pTBB->IsSeparator()
		||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton))
		||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
		||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiRightButton))
		)
	{
		CExtControlBar::OnRButtonUp( nFlags,  point );
		return;
	}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( _IsDockSiteCustomizeMode() )
	{
		CExtCustomizeSite * pSite = GetCustomizeSite();
		if( pSite != NULL )
		{
			ASSERT( pSite->IsCustomizeMode() );
			CExtCustomizeCmdTreeNode * pNodeC = pTBB->GetCmdNode( false );
			if( pNodeC == NULL )
				return;
			ASSERT_VALID( pNodeC );
			CExtCustomizeCmdTreeNode * pNodeI = pTBB->GetCmdNode( true );
#ifdef _DEBUG
			if( pNodeI != NULL )
			{
				ASSERT_VALID( pNodeI );
			}
#endif // _DEBUG
			pSite->DropTargetPopupCancel();
			pTBB->SetCustomizeActiveItem();
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd(m_hWnd),
					pTBB->GetCmdID( false )
					);
			ASSERT( pCmdItem != NULL );
			if( !pSite->CustomizedNodeSet( pNodeC ) )
				return;
			Invalidate();
			//UpdateWindow();
			bool bCanAddRemoveSeparator = false;
			if( OnQueryRevertRTL() )
			{
				int nButtonsCount = GetButtonsCount();
				if(		nButton < (nButtonsCount-1)
					&&	( ! _GetButtonPtr( nButton + 1 ) ->
								IsKindOf(
									RUNTIME_CLASS(CExtBarMdiDocButton)
									)
						)
					)
					bCanAddRemoveSeparator = true;
			} // if( OnQueryRevertRTL() )
			else
			{
				if(		nButton > 0
					&&	( ! _GetButtonPtr( nButton - 1 ) ->
								IsKindOf(
									RUNTIME_CLASS(CExtBarMdiDocButton)
									)
						)
					)
					bCanAddRemoveSeparator = true;
			} // else from if( OnQueryRevertRTL() )
			bool bDelete = false;
			if( ! pSite->OnCustomizeTreeNode(
					pNodeI,
					pNodeC,
					pCmdItem,
					bDelete,
					bCanAddRemoveSeparator,
					this
					)
				)
			{
				VERIFY( pSite->CustomizedNodeSet( NULL ) );
				Invalidate();
				//UpdateWindow();
				return;
			}
			VERIFY( pSite->CustomizedNodeSet( NULL ) );
			if( bDelete )
			{
				pNodeC->RemoveSelf( pCmdItem );
				RemoveButton( nButton );
				if(		nButton > 0
					&&	_GetButtonPtr(nButton-1)->IsSeparator()
					)
					RemoveButton( nButton-1 );
				if(		nButton < (GetButtonsCount()-1)
					&&	_GetButtonPtr(nButton)->IsSeparator()
					)
				{
					RemoveButton( nButton );
					CExtBarButton * pTBB = _GetButtonPtr(nButton);
					ASSERT_VALID( pTBB );
					CExtCustomizeCmdTreeNode * pNodeNext = pTBB->GetCmdNode( false );
					ASSERT_VALID( pNodeNext );
					pNodeNext->ModifyFlags( 0, __ECTN_GROUP_START );
				}
			} // if( bDelete )
			else
			{
				if( pNodeC->GetFlags() & __ECTN_GROUP_START )
				{
					if(		nButton > 0
						&&	(! _GetButtonPtr(nButton-1)->IsSeparator() )
						)
						InsertButton( nButton, ID_SEPARATOR, FALSE );
				} // if( pNodeC->GetFlags() & __ECTN_GROUP_START )
				else
				{
					if(		nButton > 0
						&&	_GetButtonPtr(nButton-1)->IsSeparator()
						)
						RemoveButton( nButton-1, FALSE );
				} // else from if( pNodeC->GetFlags() & __ECTN_GROUP_START )
			} // else from if( bDelete )
			if( IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
				((CExtMenuControlBar*)this)->UpdateMenuBar();
			if( m_pDockSite == NULL )
			{
				_RecalcPositionsImpl();
				Invalidate();
				//UpdateWindow();
				return;
			} // if( m_pDockSite == NULL )
			ASSERT_VALID( m_pDockSite );
			if( IsFloating() )
			{
				_RecalcLayoutImpl();
				Invalidate();
				//UpdateWindow();
			}
			else
				m_pDockSite->RecalcLayout();
			return;
		} // if( pSite != NULL )
	} // if( _IsDockSiteCustomizeMode() )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	
	if( m_nBtnIdxHover >= 0 )
		_UpdateHoverButton( CPoint(LONG_MAX,LONG_MAX), true ); // cancel hover

	if( pTBB->OnRClick( point, false ) )
		return;

	CExtControlBar::OnRButtonUp( nFlags, point );
}

void CExtToolControlBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if( m_bRowResizing || m_bRowRecalcing || m_bDragging )
	{
		CExtControlBar::OnLButtonDblClk(nFlags, point);
		return;
	}
int nBtnIdx = _HitTestImpl(point);
	if( nBtnIdx >= 0 )
	{
		CExtBarButton* pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		if( pTBB->OnDblClick( point ) )
			return;
		if( ! IsFloating() )
		{
			if( pTBB->IsSeparator() )
			{
				if( ! _FloatStateChangingIsEnabled( this ) )
					return;
				ToggleDocking();
				return;
			}
		}
		WPARAM wParam = WPARAM(nFlags);
		LPARAM lParam = MAKELPARAM( point.x, point.y );
		SendMessage( WM_LBUTTONUP, wParam, lParam );
		SendMessage( WM_LBUTTONDOWN, wParam, lParam );
		return;
	} // if( nBtnIdx >= 0 )
	if( m_bEnableDblClickToggleDocking || IsFloating() )
		CExtControlBar::OnLButtonDblClk(nFlags,point);
}

void CExtToolControlBar::OnCancelMode()
{
	CExtControlBar::OnCancelMode();
bool bUpdateState = false;
	if( m_nBtnIdxCapture >= 0 )
	{
		CExtBarButton* pTBB =
			_GetButtonPtr(m_nBtnIdxCapture);
		ASSERT_VALID( pTBB );
		ASSERT( !(pTBB->IsSeparator()) );

		if( m_nBtnIdxMenuTracking != m_nBtnIdxCapture )
		{
			UINT nNewStyle = (pTBB->GetStyle() & ~TBBS_PRESSED);
			SetButtonStyle(m_nBtnIdxCapture, nNewStyle);
		}
		pTBB->OnPressedTrackingStop();

		m_nBtnIdxCapture = -1;
		bUpdateState = true;
	}
	if( m_nBtnIdxHover >= 0 )
		_UpdateHoverButton( CPoint(-1,-1), false );

	if( bUpdateState )
	{
		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
			CExtMouseCaptureSink::ReleaseCapture();
		Invalidate();
//		UpdateWindow();
	}
}

void CExtToolControlBar::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection)
{
	CExtControlBar::OnSettingChange(uFlags,lpszSection);

	if( m_bPresubclassDialogMode )
	{
		_RecalcLayoutImpl();
		return;
	}

CFrameWnd* pFrame = GetParentFrame();
	ASSERT_VALID(pFrame);
	pFrame->DelayRecalcLayout();
}

LONG CExtToolControlBar::_CalcSplitButtonExtent( const CExtBarButton * pTBB, bool bHorz ) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	return PmBridge_GetPM()->_CalcSplitButtonExtent( pTBB, bHorz );
}

bool CExtToolControlBar::IsRightExpandButton( int nBtnIdx )
{
	ASSERT_VALID( this );
CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
	if( pTBB == NULL )
		return false;
	ASSERT_VALID( pTBB );
	if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) )
		return true;
	return false;
}

void CExtToolControlBar::_UpdateCtrlLock()
{
	ASSERT_VALID( this );
	ASSERT( m_nUpdateCtrlLockCount >= 0 );
	m_nUpdateCtrlLockCount ++;
	ASSERT( m_nUpdateCtrlLockCount >= 0 );
//	if( m_nUpdateCtrlLockCount == 1 )
//		SetRedraw( FALSE );
}

void CExtToolControlBar::_UpdateCtrlUnlock()
{
	ASSERT_VALID( this );
	ASSERT( m_nUpdateCtrlLockCount >= 0 );
	m_nUpdateCtrlLockCount --;
	ASSERT( m_nUpdateCtrlLockCount >= 0 );
//	if( m_nUpdateCtrlLockCount == 0 )
//		SetRedraw( TRUE );
}

bool CExtToolControlBar::_UpdateCtrlIsLocked() const
{
	ASSERT_VALID( this );
	ASSERT( m_nUpdateCtrlLockCount >= 0 );
	return ( m_nUpdateCtrlLockCount == 0 ) ? false : true;
}

bool CExtToolControlBar::_UpdateCtrlIsEnabled(
	CExtBarButton * pTBB
	)
{
	ASSERT_VALID( this );
	pTBB;
	return _UpdateCtrlIsLocked() ? false : true;
}

void CExtToolControlBar::_UpdateCtrlAll()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
INT nBtnIdx, nCountOfButtons = _GetButtonsCountImpl();
	for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		pTBB->_UpdateCtrl();
	}
}

void CExtToolControlBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
CWnd * pTargetUpdateWnd = pTarget;
	if( ! m_bPresubclassDialogMode )
	{
		CExtControlBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

		//if( CExtPopupMenuWnd::IsMenuTracking() )
		//	return;
		if( _DraggingGetBar() != NULL )
			return;
	} // if( ! m_bPresubclassDialogMode )
	else
	{
		if( pTargetUpdateWnd != NULL 
			&& pTargetUpdateWnd->IsKindOf( RUNTIME_CLASS(CFrameWnd) )
			)
			return;
		//ASSERT( pTargetUpdateWnd == NULL );
//		if( pTargetUpdateWnd == NULL )
			pTargetUpdateWnd = GetOwner();
		if( pTargetUpdateWnd == NULL )
			pTargetUpdateWnd = GetParent();
		ASSERT( pTargetUpdateWnd != NULL );
	}

	if( pTargetUpdateWnd == NULL )
		return;

	ASSERT_VALID( pTargetUpdateWnd );

	if(		( GetStyle() & WS_VISIBLE ) == 0
		&&	( ! m_bUseInvisibleCmdUI )
		)
		return;

int nCountOfButtons = _GetButtonsCountImpl();
	for( int nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		pTBB->OnUpdateCmdUI(
			pTargetUpdateWnd,
			bDisableIfNoHndler,
			nBtnIdx
			);
	}

	// update the dialog controls added to the toolbar
	UpdateDialogControls(pTargetUpdateWnd, bDisableIfNoHndler);
}

void CExtToolControlBar::_UpdateButton( int nIndex )
{
CWnd * pTarget = GetOwner();
	if( pTarget == NULL && ( ! m_bPresubclassDialogMode ) )
		pTarget = GetParentFrame();
	if( pTarget == NULL )
		return;
BOOL bDisableIfNoHandler = TRUE;
	if( pTarget->IsKindOf( RUNTIME_CLASS(CFrameWnd) ) )
		bDisableIfNoHandler = ((CFrameWnd *)pTarget)->m_bAutoMenuEnable;
CExtBarButton * pTBB = _GetButtonPtr( nIndex );
	if( pTBB == NULL )
		return;
	ASSERT_VALID( pTBB );
	pTBB->OnUpdateCmdUI( pTarget, bDisableIfNoHandler, nIndex );
}

/////////////////////////////////////////////////////////////////////////////
// CExtToolControlBar diagnostics

#ifdef _DEBUG
void CExtToolControlBar::AssertValid() const
{
	CExtControlBar::AssertValid();
}

void CExtToolControlBar::Dump(CDumpContext& dc) const
{
	CExtControlBar::Dump(dc);
}
#endif

CSize CExtToolControlBar::_GetDefButtonSize() const
{
	ASSERT_VALID( this );
	return CSize( 23, 22 );
}

int CExtToolControlBar::_GetIndexOf( CExtBarButton * pTBB )
{
	if( pTBB == NULL )
		return -1;
	int nCountOfButtons = (int)m_buttons.GetSize();
	for( int nBtnIdx=0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		ASSERT( m_buttons[nBtnIdx] != NULL );
		if( m_buttons[nBtnIdx] == pTBB )
			return nBtnIdx;
	}
	return -1;
}

// input CRect should be client rectangle size
void CExtToolControlBar::_CalcInsideRect(CRect& rect, BOOL bHorz) const
{
	ASSERT_VALID(this);
	if( ! (((CExtToolControlBar*)this)->_AdjustBGInfo()) )
	{
		CControlBar::CalcInsideRect(rect,bHorz);
		return;
	}
DWORD dwStyle = m_dwStyle;
	if( (dwStyle&CBRS_BORDER_LEFT) != 0 )
		rect.left += afxData.cxBorder2;
	if( (dwStyle&CBRS_BORDER_TOP) != 0 )
		rect.top += afxData.cyBorder2;
	if( (dwStyle&CBRS_BORDER_RIGHT) != 0 )
		rect.right -= afxData.cxBorder2;
	if( (dwStyle&CBRS_BORDER_BOTTOM) != 0 )
		rect.bottom -= afxData.cyBorder2;
	rect.left += m_cxLeftBorder;
	rect.top += m_cyTopBorder;
	rect.right -= m_cxRightBorder;
	rect.bottom -= m_cyBottomBorder;
//	if( (m_dwStyle&(CBRS_GRIPPER|CBRS_FLOATING)) == CBRS_GRIPPER )
//	{
//		if( bHorz )
//		{
//			INT nGripWidth = _GripperWidthAtLeftGet();
//			if( nGripWidth >= 0 )
//				rect.left += nGripWidth;
//		} // if( bHorz )
//		else
//		{
//			INT nGripHeight = _GripperHeightAtTopGet();
//			if( nGripHeight >= 0 )
//				rect.top += nGripHeight;
//		} // else from if( bHorz )
//	} // if( (m_dwStyle&(CBRS_GRIPPER|CBRS_FLOATING)) == CBRS_GRIPPER )
}

bool CExtToolControlBar::OnQueryShiftRTL() const
{
	ASSERT_VALID( this );
	return false;
}

bool CExtToolControlBar::OnQueryRevertRTL() const
{
	ASSERT_VALID( this );
	return false;
}

void CExtToolControlBar::_RecalcPositionsImpl()
{
	if( GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID(this);
int nCountOfButtons = _GetButtonsCountImpl();
	if( nCountOfButtons == 0 )
		return;
CExtBarButton::e_wrap_t _evt = GetBtnWrapCode();
BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE;
BOOL bFloating = IsFloating();
bool bMultiRowLayout = OnQueryMultiRowLayout();
CRect rcInner;
	GetClientRect( &rcInner );
CExtPaintManager::e_paint_manager_name_t ePMN =
		PmBridge_GetPM()->OnQueryPaintManagerName();
	if( ! bFloating )
	{
		if( bMultiRowLayout )
		{
// 			if( bHorz )
// 				rcInner.OffsetRect( 0, 2 /*- PmBridge_GetPM()->GetPushedOffset().cy*/ );
// 			else
// 				rcInner.OffsetRect( -2, 0 );
			UINT nMetric = 0;
			if( bHorz )
				rcInner.OffsetRect( 0, 2 - nMetric );
			else
				rcInner.OffsetRect( -2 + nMetric, 0 );
		}
	} // if( ! bFloating )
CPoint ptLimitTL = rcInner.TopLeft();
CPoint ptLimitBR = rcInner.BottomRight();

CFont * pFont =
		OnGetToolbarFont(
			( ( ( m_dwStyle & CBRS_ORIENT_HORZ ) == 0 ) && (!m_bPaletteMode) )
				? true : false,
			false,
			this
			);
	if( pFont->GetSafeHandle() == NULL )
		pFont = (&( PmBridge_GetPM()->m_FontNormal ));
CClientDC dc(this);
CFont* pOldFont = dc.SelectObject( pFont );
	ASSERT( pOldFont != NULL );

CSize sizeDefButton = _GetDefButtonSize();
int nReviewCount = nCountOfButtons;
CSize sizeTBBRight( 0, 0 );
	if( m_pRightBtn != NULL )
	{
		nReviewCount--;
		ASSERT_VALID( m_pRightBtn );
		ASSERT( m_buttons.GetSize() > 0 );
		ASSERT( m_buttons[nReviewCount] == m_pRightBtn );
		m_pRightBtn->GetButtons().RemoveAll();
		m_pRightBtn->ClearContent();
		sizeTBBRight =
			m_pRightBtn->CalculateLayout(
				dc,
				sizeDefButton,
				bHorz || m_bPaletteMode
				);
		if( bFloating || m_bPaletteMode )
		{
			m_pRightBtn->Show( false );
			m_pRightBtn->SetRect(
				CRect( ptLimitTL, sizeTBBRight )
				);
			if( nReviewCount == 0 )
				return;
		} // if( bFloating || m_bPaletteMode )
		else
		{
			CRect rcCalcRightBtn(
				bHorz ? (ptLimitBR.x - sizeTBBRight.cx) : ptLimitTL.x,
				bHorz ? ptLimitTL.y : (ptLimitBR.y - sizeTBBRight.cy),
				ptLimitBR.x,
				ptLimitBR.y
				);
			m_pRightBtn->SetRect( rcCalcRightBtn );
			m_pRightBtn->Show( true );
			const CSize _sizeRoundedAreaMerics =
				PmBridge_GetPM()->FixedBar_GetRoundedAreaMerics();
			if( bHorz )
				rcInner.right -= sizeTBBRight.cx + _sizeRoundedAreaMerics.cx;
			else
				rcInner.bottom -= sizeTBBRight.cy + _sizeRoundedAreaMerics.cy;
		} // else from if( bFloating || m_bPaletteMode )
	} // if( m_pRightBtn != NULL )

CArray < CRect, CRect >	arrBtnRects;
CArray < BOOL, BOOL >	arrBtnVisibility;
CArray < BOOL, BOOL >	arrBtnSeparators;
//CArray < BOOL, BOOL >	arrBtnEnabled;
	arrBtnRects.SetSize( nReviewCount );
	arrBtnVisibility.SetSize( nReviewCount );
	arrBtnSeparators.SetSize( nReviewCount );
//	arrBtnEnabled.SetSize( nReviewCount );
CSize sizeLastWrappedRow( 0, 0 );

CPoint ptBtnPosCurr( ptLimitTL );
INT nRowExtent = sizeDefButton.cy;
	if( bMultiRowLayout && (!bHorz) )
	{
		ptBtnPosCurr.x = ptLimitBR.x;
		nRowExtent = sizeDefButton.cx;
	}
BOOL bSeparatorPrev = FALSE, bPostResyncAllowed = FALSE;
INT nRowDistance = PmBridge_GetPM()->FixedBar_GetRowDistance( this );
int nBtnIdx = 0;
	for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		BOOL bVisibleTBB =
			( pTBB->GetStyle() & TBBS_HIDDEN ) ? FALSE : TRUE;
		if(		pTBB->IsSeparator()
			&&	nBtnIdx < (nReviewCount-1)
			)
		{
			CExtBarButton * pNextTBB = _GetButtonPtr( nBtnIdx+1 );
			ASSERT_VALID( pNextTBB );
			if( pNextTBB->GetStyle() & TBBS_HIDDEN )
				bVisibleTBB = FALSE;
		}
		arrBtnVisibility.SetAt( nBtnIdx, bVisibleTBB );
		if( ! bVisibleTBB )
			continue;
		pTBB->SetVertDocked( (bHorz || m_bPaletteMode) ? false : true );
//		arrBtnEnabled.SetAt( nBtnIdx, pTBB->IsEnabled() );
		BOOL bSeparator = pTBB->IsSeparator();
		arrBtnSeparators.SetAt( nBtnIdx, bSeparator );
		if( (bSeparatorPrev && bSeparator) || (bSeparator && nBtnIdx == 0) )
		{ // remove double separators
			arrBtnVisibility.SetAt( nBtnIdx, FALSE );
			continue;
		} // remove double separators

		CSize sizeTBB =
			pTBB->CalculateLayout(
				dc,
				sizeDefButton,
				bHorz || m_bPaletteMode
				);
		
		CRect rcTBB( ptBtnPosCurr, sizeTBB );
		if( bMultiRowLayout && (!bHorz) )
			nRowExtent = max( nRowExtent, sizeTBB.cx );
		else
			nRowExtent = max( nRowExtent, sizeTBB.cy );

		if(		(	bFloating
				||	bMultiRowLayout
				||	m_bPaletteMode
				)
			&&	pTBB->IsWrap( _evt )
			)
		{
			if( bMultiRowLayout && (!bHorz) )
			{
				ptBtnPosCurr.y = ptLimitTL.y;
				ptBtnPosCurr.x -=
					nRowExtent
					+ nRowDistance;
				sizeLastWrappedRow = CSize( 0, 0 );
				nRowExtent = sizeDefButton.cx;
			} // if( bMultiRowLayout && (!bHorz) )
			else
			{
				ptBtnPosCurr.x = ptLimitTL.x;
				ptBtnPosCurr.y +=
					nRowExtent // sizeDefButton.cy (+ v.2.22)
					+ nRowDistance;
				sizeLastWrappedRow = CSize( 0, 0 );
				nRowExtent = sizeDefButton.cy;
			} // else from if( bMultiRowLayout && (!bHorz) )
		} // if( (bFloating || ...
		else
		{
			bool bResyncVisibleTBB = false;
			if( bHorz || m_bPaletteMode )
			{
				ptBtnPosCurr.x += sizeTBB.cx;
				if(		(!bMultiRowLayout)
					&&	ptBtnPosCurr.x > rcInner.right
					)
					bResyncVisibleTBB = true;
			} // if( bHorz || m_bPaletteMode )
			else
			{
				ptBtnPosCurr.y += sizeTBB.cy;
				if(		(!bMultiRowLayout)
					&&	ptBtnPosCurr.y > rcInner.bottom
					)
					bResyncVisibleTBB = true;
			} // else from if( bHorz || m_bPaletteMode )
			if( bResyncVisibleTBB )
			{
				int nSpaceToFind =
					( bHorz || m_bPaletteMode )
						? ptBtnPosCurr.x - rcInner.right
						: ptBtnPosCurr.y - rcInner.bottom;
				int nBtnWithNearestWidth = nBtnIdx;
				int nNearestMetric =
					( bHorz || m_bPaletteMode )
						? rcTBB.Width() : rcTBB.Height();
				int nNearestDiff = nSpaceToFind - nNearestMetric;
				// hide nearest by size reviewed visible
				int nBtnIdx2 = 0;
				for( nBtnIdx2 = nBtnIdx-1; nBtnIdx2 >= 0; nBtnIdx2-- )
				{
					if( ! arrBtnVisibility[nBtnIdx2] )
						continue;
					if( arrBtnSeparators[nBtnIdx2] )
						continue;

#ifdef _DEBUG
					CExtBarButton * pTBB2 =
						_GetButtonPtr( nBtnIdx2 );
					ASSERT( (pTBB2->GetStyle() & TBBS_HIDDEN) == 0 );
#endif // _DEBUG

					int nMetric =
						( bHorz || m_bPaletteMode )
							? arrBtnRects[nBtnIdx2].Width()
							: arrBtnRects[nBtnIdx2].Height();
					//if( nMetric <= nNearestMetric ) // <<-- widest search algorithm
					//	continue;
					if( nMetric > nSpaceToFind )
						continue;
					int nDiff = nSpaceToFind - nMetric;
//					if( ! arrBtnEnabled[nBtnIdx2] )
//					{ // hide disabled buttons first
//						nNearestDiff = nDiff;
//						nNearestMetric = nMetric;
//						nBtnWithNearestWidth = nBtnIdx2;
//						break;
//					} // hide disabled buttons first
					if( nDiff >= nNearestDiff )
						continue;
					nNearestDiff = nDiff;
					nNearestMetric = nMetric;
					nBtnWithNearestWidth = nBtnIdx2;
				} // for( nBtnIdx2 = nBtnIdx-1; nBtnIdx2 >= 0; nBtnIdx2-- )
				if( nBtnWithNearestWidth < nBtnIdx )
				{
					ASSERT( bVisibleTBB );
					arrBtnVisibility.SetAt( nBtnWithNearestWidth, FALSE );
					if(		(! m_bPaletteMode )
						&&	(! bMultiRowLayout )
						&&	(! IsFloating() )
						)
						bPostResyncAllowed = TRUE;
					if( m_pRightBtn!=NULL && !arrBtnSeparators[nBtnWithNearestWidth] )
					{
						CExtBarButton * pTBB2 =
							_GetButtonPtr( nBtnWithNearestWidth );
						ASSERT_VALID( pTBB2 );
						m_pRightBtn->GetButtons().Add( pTBB2 );
					}
					if( nBtnWithNearestWidth > 0 && nBtnWithNearestWidth < nReviewCount-1 )
					{ // remove double separators
						if(		arrBtnSeparators[nBtnWithNearestWidth-1]
							&&	arrBtnSeparators[nBtnWithNearestWidth+1]
							)
						{
							arrBtnVisibility.SetAt( nBtnWithNearestWidth-1, FALSE );
							nNearestMetric +=
								( bHorz || m_bPaletteMode )
									? arrBtnRects[nBtnWithNearestWidth-1].Width()
									: arrBtnRects[nBtnWithNearestWidth-1].Height();
						}
					} // remove double separators
					rcTBB.OffsetRect(
						( bHorz || m_bPaletteMode ) ? -nNearestMetric : 0,
						( bHorz || m_bPaletteMode ) ? 0 : -nNearestMetric
						);
					( ( bHorz || m_bPaletteMode ) ? ptBtnPosCurr.x : ptBtnPosCurr.y) -= nNearestMetric;
					for( nBtnIdx2 = nBtnWithNearestWidth+1; nBtnIdx2 < nBtnIdx; nBtnIdx2++ )
					{
						if( !arrBtnVisibility[nBtnIdx2] )
							continue;
						CRect rcTBB2 = arrBtnRects[nBtnIdx2];
						rcTBB2.OffsetRect(
							( bHorz || m_bPaletteMode ) ? -nNearestMetric : 0,
							( bHorz || m_bPaletteMode ) ? 0 : -nNearestMetric
							);
						arrBtnRects.SetAt( nBtnIdx2, rcTBB2 );
					} // for( nBtnIdx2 = nBtnWithNearestWidth+1; nBtnIdx2 < nBtnIdx; nBtnIdx2++ )
				} // if( nBtnWithNearestWidth < nBtnIdx )
				else
				{
					ASSERT( nBtnWithNearestWidth == nBtnIdx );
					bVisibleTBB = FALSE;
					if(		(! m_bPaletteMode )
						&&	(! bMultiRowLayout )
						&&	(! IsFloating() )
						)
						bPostResyncAllowed = TRUE;
					( (bHorz || m_bPaletteMode ) ? ptBtnPosCurr.x : ptBtnPosCurr.y) -=
						( (bHorz || m_bPaletteMode ) ? sizeTBB.cx : sizeTBB.cy);
				} // else from if( nBtnWithNearestWidth < nBtnIdx )
			} // if( bResyncVisibleTBB )
		} // else from if( (bFloating || ...

		arrBtnVisibility.SetAt( nBtnIdx, bVisibleTBB );
		if( bMultiRowLayout && (!bHorz) )
		{
			CRect rcTBB2( rcTBB );
			rcTBB2.OffsetRect( -rcTBB.Width(), 0 );
			arrBtnRects.SetAt( nBtnIdx, rcTBB2 );
		}
		else
		{
			arrBtnRects.SetAt( nBtnIdx, rcTBB );
		}
		if( bVisibleTBB )
			bSeparatorPrev = bSeparator;

		if(		m_pRightBtn != NULL
			&&	(!bMultiRowLayout)
			&&	(!bVisibleTBB)
			&&	(!bSeparator)
			)
			m_pRightBtn->GetButtons().Add( pTBB );
	} // for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )

	dc.SelectObject( pOldFont );

	if( bPostResyncAllowed )
	{
		ASSERT( ! IsFloating() );
		ASSERT( ! m_bPaletteMode );
		ASSERT( ! bMultiRowLayout );
		INT nVisIdx0 = -1, nVisIdx1 = -1;
		for( int nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
		{
			if( ! arrBtnVisibility[nBtnIdx] )
				continue;
			CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
			ASSERT_VALID( pTBB );
			if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) )
				break;
			if( nVisIdx0 < 0 )
				nVisIdx0 = nBtnIdx;
			nVisIdx1 = nBtnIdx;
		} // for( int nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
		if( nVisIdx0 >= 0 )
		{
			ASSERT( nVisIdx1 >= nVisIdx0 );
			INT nShiftMetric = 0;
			for( ; nVisIdx0 <= nVisIdx1; nVisIdx0++ )
			{
				if( ! arrBtnVisibility[nVisIdx0] )
					break;
				CExtBarButton * pTBB = _GetButtonPtr( nVisIdx0 );
				ASSERT_VALID( pTBB );
				ASSERT( ! pTBB->IsKindOf( RUNTIME_CLASS( CExtBarContentExpandButton ) ) );
				if( ! pTBB->IsSeparator() )
					break;
				arrBtnVisibility[nVisIdx0] = FALSE;
				if( bHorz )
					nShiftMetric -= arrBtnRects[nVisIdx0].Width();
				else
					nShiftMetric -= arrBtnRects[nVisIdx0].Height();
			}
			for( ; nVisIdx1 >= nVisIdx0; nVisIdx1-- )
			{
				if( ! arrBtnVisibility[nVisIdx1] )
					break;
				CExtBarButton * pTBB = _GetButtonPtr( nVisIdx1 );
				ASSERT_VALID( pTBB );
				ASSERT( ! pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) );
				if( ! pTBB->IsSeparator() )
					break;
				arrBtnVisibility[nVisIdx1] = FALSE;
			}
			if( nShiftMetric != 0 )
			{
				for( int nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
				{
					if( ! arrBtnVisibility[nBtnIdx] )
						continue;
					CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
					ASSERT_VALID( pTBB );
					if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) )
						break;
					arrBtnRects.ElementAt( nBtnIdx ).OffsetRect(
						bHorz ? nShiftMetric : 0,
						bHorz ? 0 : nShiftMetric
						);
				} // for( int nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
			} // if( nShiftMetric != 0 )
		} // if( nVisIdx0 >= 0 )
	} // if( bPostResyncAllowed )

INT nRowStart = 0;
INT nSizeRow = 0;
INT nSubRowCount = 0;
bool bReAlignHorz =
		(	bHorz
		||	bFloating
		||	m_bPaletteMode
		) ? true : false;
	if( bMultiRowLayout && (!bHorz) )
		bReAlignHorz = false;
bool bShiftRTL = OnQueryShiftRTL();
	if( ! bReAlignHorz )
		bShiftRTL = false;
	if( bShiftRTL )
	{ // do content shift to right
		CRect rcAlign = rcInner;
		INT nAdditionalShift = 0;
		for( int nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
		{
			CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
			ASSERT_VALID( pTBB );
			BOOL bVis = arrBtnVisibility[nBtnIdx];
			if( ! bVis )
				continue;
			CSize sizeTBB = *pTBB;
			if(		pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
				&&	(! pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiRightButton)) )
				)
			{
				nSizeRow = 0;
				nRowStart = nBtnIdx + 1;
				//Align.left += sizeTBB.cx;
				nAdditionalShift = - sizeTBB.cx;
				if( nAdditionalShift == 0 )
					nAdditionalShift = - 20;
				continue;
			}
			nSizeRow += sizeTBB.cx;
			bool bWrap =
				(	(	bFloating
						||	bMultiRowLayout
						||	m_bPaletteMode
						)
						&&	pTBB->IsWrap(_evt)
					)
				? true : false;
			if(		(! bWrap)
				&&	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiRightButton))
				)
				bWrap = true;
			if(		bWrap
				||	nBtnIdx == (nReviewCount-1)
				)
			{
				INT nShift =
					rcAlign.Width()
					- nSizeRow
					+ nAdditionalShift
					- 7
					;
				for( INT nBtnIdx2 = nRowStart; nBtnIdx2 <= nBtnIdx; nBtnIdx2++ )
				{
					CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx2 );
					ASSERT_VALID( pTBB );
					BOOL bVis2 = arrBtnVisibility[nBtnIdx2];
					if( ! bVis2 )
						continue;
					if(		pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
						&&	(! pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiRightButton)) )
						)
						continue;
					CRect rcBtn = arrBtnRects[nBtnIdx2];
					rcBtn.OffsetRect( nShift, 0 );
//					if( bFloating )
//						rcBtn.OffsetRect( -3, 0 );
					arrBtnRects.SetAt( nBtnIdx2, rcBtn );
				}
				nRowStart = nBtnIdx + 1;
				nSizeRow = 0;
				nSubRowCount++;
				rcAlign = rcInner;
				nAdditionalShift = 0;
			}
		} // for( int nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
		// reset vars
		nRowStart = 0;
		nSizeRow = 0;
		nSubRowCount = 0;
	}  // do content shift to right

	for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		CSize sizeTBB = *pTBB;
		INT nSizeBtn = bReAlignHorz ? sizeTBB.cy : sizeTBB.cx;
		nSizeRow = max( nSizeRow, nSizeBtn );
		if(		(	(	bFloating
					||	bMultiRowLayout
					||	m_bPaletteMode
					)
					&&	pTBB->IsWrap(_evt)
				)
			|| nBtnIdx == ( nReviewCount - 1 )
			)
		{
			for( INT nBtnIdx2 = nRowStart; nBtnIdx2 <= nBtnIdx; nBtnIdx2++ )
			{
				CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx2 );
				ASSERT_VALID( pTBB );
				BOOL bVis = arrBtnVisibility[nBtnIdx2];
				pTBB->Show( bVis ? true : false );
				if( ! bVis )
					continue;
				CRect rcBtn = arrBtnRects[nBtnIdx2];
				if( pTBB->IsSeparator() )
				{
					if( bReAlignHorz )
						rcBtn.bottom = rcBtn.top + nSizeRow;
					else
						rcBtn.left = rcBtn.right - nSizeRow;
				} // if( pTBB->IsSeparator() )
				else
				{
					if( bReAlignHorz )
						rcBtn.OffsetRect(
							0,
							( nSizeRow - rcBtn.Height() ) / 2
							);
					else 
						rcBtn.OffsetRect(
							- ( nSizeRow - rcBtn.Width() ) / 2,
							0
							);
				} // else from if( pTBB->IsSeparator() )
				if(		bFloating
//				||	bMultiRowLayout
					)
					//rcBtn.OffsetRect( 0, 1 );
					rcBtn.OffsetRect( 2, 1 );
				pTBB->SetRect( rcBtn );
			}
			nRowStart = nBtnIdx + 1;
			nSizeRow = 0;
			nSubRowCount++;
		}
	} // for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )

	if( nSubRowCount == 1 && (!m_bPaletteMode) && (!bFloating) && (!bMultiRowLayout) )
	{
		CRect rcBarClient;
		GetClientRect( &rcBarClient );
		nSizeRow = bHorz
			? rcBarClient.Height()
			: rcBarClient.Width()
			;
		for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
		{
			CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
			ASSERT_VALID( pTBB );
			BOOL bVis = arrBtnVisibility[nBtnIdx];
			if( ! bVis )
				continue;
			CRect rcBtn( pTBB->Rect() );
//			if( pTBB->IsSeparator() )
//			{
//				if( bHorz )
//				{
//					rcBtn.top = m_cyTopBorder;
//					rcBtn.bottom = m_cyBottomBorder + nSizeRow;
//				} // if( bHorz )
//				else
//				{
//					rcBtn.left = m_cxLeftBorder;
//					rcBtn.right = m_cxRightBorder + nSizeRow;
//				} // else from if( bHorz )
//			} // if( pTBB->IsSeparator() )
//			else
			{
				rcBtn.OffsetRect(
					bHorz
						? 0
						: ( ( nSizeRow - rcBtn.Width() ) / 2  - rcBtn.left)
					,
					bHorz
						? ( ( nSizeRow - rcBtn.Height() ) / 2 - rcBtn.top) 
						: 0
					);
			} // else from if( pTBB->IsSeparator() )
			pTBB->SetRect( rcBtn );
		} // for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
	} // if( nSubRowCount == 1 && (!m_bPaletteMode) && (!bFloating) && (!bMultiRowLayout) )
	else if( ! m_bPaletteMode )
	{ // just re-align separators in each row
		INT nRowMinLoc = 0, nRowMaxLoc = 0;
		INT nRowStart = 0;
		for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
		{
			CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
			ASSERT_VALID( pTBB );
			if(		pTBB->IsVisible()
				&&	(pTBB->GetStyle() & TBBS_HIDDEN) == 0
				)
			{
				CRect rcTBB = *pTBB;
				if( nBtnIdx == nRowStart )
				{
					if( bHorz )
					{
						nRowMinLoc = rcTBB.top;
						nRowMaxLoc = rcTBB.bottom;
					} // if( bHorz )
					else
					{
						nRowMinLoc = rcTBB.left;
						nRowMaxLoc = rcTBB.right;
					} // else from if( bHorz )
				} // if( nBtnIdx == nRowStart )
				else
				{
					if( bHorz )
					{
						nRowMinLoc = min( nRowMinLoc, rcTBB.top );
						nRowMaxLoc = max( nRowMaxLoc, rcTBB.bottom );
					} // if( bHorz )
					else
					{
						nRowMinLoc = min( nRowMinLoc, rcTBB.left );
						nRowMaxLoc = max( nRowMaxLoc, rcTBB.right );
					} // else from if( bHorz )
				} // else from if( nBtnIdx == nRowStart )
			} // if( pTBB->IsVisible() ...
			if(		(! pTBB->IsWrap(_evt) )
				&&	nBtnIdx != (nReviewCount-1)
				)
				continue;
			for( INT nBtnIdx2 = nRowStart; nBtnIdx2 < nBtnIdx; nBtnIdx2++ )
			{
				CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx2 );
				ASSERT_VALID( pTBB );
				if(		(! pTBB->IsVisible() )
					||	(pTBB->GetStyle() & TBBS_HIDDEN) != 0
					||	(! pTBB->IsSeparator() )
					)
					continue;
				CRect rcTBB = *pTBB;

				if( bHorz )
				{
					rcTBB.top = nRowMinLoc;
					rcTBB.bottom = nRowMaxLoc;
//					rcTBB.InflateRect(
//						0,
//						bDockerTrasparentMode ? 2 : 1
//						);
				} // if( bHorz )
				else
				{
					rcTBB.left = nRowMinLoc;
					rcTBB.right = nRowMaxLoc;
//					rcTBB.InflateRect(
//						bDockerTrasparentMode ? 2 : 1,
//						0,
//						bDockerTrasparentMode ? 3 : 2,
//						0
//						);
				} // else from if( bHorz )
				pTBB->SetRect( rcTBB );
			} // for( INT nBtnIdx2 = nRowStart; nBtnIdx2 < nBtnIdx; nBtnIdx2++ )
			nRowStart = nBtnIdx + 1;
		} // for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
	} // just re-align separators in each row

	// remove TBBS_HIDDEN in right menu
	if( (!m_bPaletteMode) && m_pRightBtn != NULL )
	{
		int nMenuCount = (int)m_pRightBtn->GetButtons().GetSize();
		for( int nMenuIdx = 0; nMenuIdx < nMenuCount; )
		{
			CExtBarButton * pTBB = 
				m_pRightBtn->GetButtons().
					GetAt( nMenuIdx );
			ASSERT_VALID( pTBB );
			if( pTBB->GetStyle() & TBBS_HIDDEN )
			{
				m_pRightBtn->GetButtons().RemoveAt( nMenuIdx );
				nMenuCount--;
			} // if( pTBB->GetStyle() & TBBS_HIDDEN )
			else
				nMenuIdx++;
		} // for( int nMenuIdx = 0; nMenuIdx < nMenuCount; )
	} // if( (!m_bPaletteMode) && m_pRightBtn != NULL )
	
	if(	m_pRightBtn != NULL && m_bPresubclassDialogMode )
	{
		if( m_pRightBtn->GetButtons().GetSize() == 0 )
			m_pRightBtn->ModifyStyle( TBBS_DISABLED, 0 );
		else
			m_pRightBtn->ModifyStyle( 0, TBBS_DISABLED );
	}

	// hide first separator(s)
	if(	! m_bPaletteMode )
	{
		INT nWalkCount = _GetButtonsCountImpl();
		for( nBtnIdx = 0; nBtnIdx < nWalkCount; nBtnIdx++ )
		{
			CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
			ASSERT_VALID( pTBB );
			if(		pTBB->IsVisible()
				&&	( pTBB->GetStyle() & TBBS_HIDDEN ) == 0
				)
			{
				if( pTBB->IsSeparator() )
				{
					pTBB->ModifyStyle( TBBS_HIDDEN, 0 );
					pTBB->Show( false );
					continue;
				}
				else
					break;
			}
		} // for( nBtnIdx = 0; nBtnIdx < nWalkCount; nBtnIdx++ )
	}

	// insert separators into right button
	if(	!(		bFloating
			||	bMultiRowLayout
			||	m_bPaletteMode
			||	bMultiRowLayout
			||	m_pRightBtn == NULL
			||	(	m_pRightBtn != NULL
					&& m_pRightBtn->GetButtons().GetSize() == 0
				)
			)
		)
	{
		int nHiddenCount = (int)m_pRightBtn->GetButtons().GetSize();
		if( nHiddenCount >= 2 )
		{
			int nEndMeasure = nHiddenCount-1;
			for( int iHidden = 0; iHidden < nEndMeasure; iHidden++ )
			{
				CExtBarButton * pTbbHidden0 =
					m_pRightBtn->GetButtons().GetAt(iHidden);
				ASSERT( pTbbHidden0 != NULL );
				ASSERT( ! pTbbHidden0->IsSeparator() );
				ASSERT( (! pTbbHidden0->IsVisible()) && (pTbbHidden0->GetStyle() & TBBS_HIDDEN) == 0 );
				CExtBarButton * pTbbHidden1 =
					m_pRightBtn->GetButtons().GetAt(iHidden+1);
				ASSERT( pTbbHidden1 != NULL );
				ASSERT( ! pTbbHidden1->IsSeparator() );
				ASSERT( (! pTbbHidden1->IsVisible()) && (pTbbHidden1->GetStyle() & TBBS_HIDDEN) == 0 );
				CExtBarButton * pTbbSeparatorToInsert = NULL;
				int nIdx0=-1,nIdx1=-1;
				for( nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
				{
					CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
					ASSERT_VALID( pTBB );
					if( pTBB == pTbbHidden0 )
					{
						nIdx0 = nBtnIdx;
						ASSERT( nIdx1 < 0 );
						continue;
					}
					if( nIdx0 >= 0
						&& pTBB->IsSeparator()
						&& pTbbSeparatorToInsert == NULL
						)
						pTbbSeparatorToInsert = pTBB;
					if( pTBB == pTbbHidden1 )
					{
						nIdx1 = nBtnIdx;
						ASSERT( nIdx0 >= 0 && nIdx0 < nIdx1 );
						break;
					}
				}
				ASSERT(
					nIdx0 < nIdx1
					&&
					nIdx0 >= 0 && nIdx0 < nCountOfButtons
					&& 
					nIdx1 >= 0 && nIdx1 < nCountOfButtons
					);
				if( (nIdx0+1) == nIdx1 )
					continue;
				if( pTbbSeparatorToInsert != NULL )
				{
					nEndMeasure++;
					iHidden++;
					m_pRightBtn->GetButtons().InsertAt(
						iHidden,
						pTbbSeparatorToInsert
						);
				}
			} // for( int iHidden = 0; iHidden < nEndMeasure; iHidden++ )
		} // if( nHiddenCount >= 2 )
	}

	// offset buttons according to the theme NC metrics
	if( ! bFloating )
	{
		CSize _sizeButtonOffset( 0, 0 );
		if( m_bPaletteMode )
		{
			if( bHorz )
				_sizeButtonOffset.cy = 2;
			else
				_sizeButtonOffset.cx = 2;
		} // if( m_bPaletteMode )
		_sizeButtonOffset += PmBridge_GetPM()->GetToolBarButtonOffset( bHorz );
		if( _sizeButtonOffset.cx != 0 || _sizeButtonOffset.cy != 0 )
		{
			for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
			{
				CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
				ASSERT_VALID( pTBB );
				if(		pTBB->IsVisible()
					&&	(pTBB->GetStyle() & TBBS_HIDDEN) == 0
					)
				{
					CRect rcTBB = *pTBB;
					rcTBB.OffsetRect( _sizeButtonOffset );
					pTBB->SetRect( rcTBB );
				}
			} // for( nBtnIdx = 0; nBtnIdx < nReviewCount; nBtnIdx++ )
		} // if( _sizeButtonOffset.cx != 0 || _sizeButtonOffset.cy != 0 )
	} // if( ! bFloating )
}

DWORD CExtToolControlBar::RecalcDelayShow(AFX_SIZEPARENTPARAMS* lpLayout)
{
DWORD dwRes = CControlBar::RecalcDelayShow( lpLayout );
	if( (!IsFloating()) && IsVisible() )
		_RecalcPositionsImpl();
	return dwRes;
}

void CExtToolControlBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	ASSERT_VALID( this );
	if( _AdjustBGInfo() )
	{
//		INT nGripWidthAtLeft = 0, nGripHeightAtTop = 0;
//		if( (GetBarStyle()&CBRS_GRIPPER) != 0 )
//		{
//			nGripWidthAtLeft =
//				PmBridge_GetPM()->Bar_GripperWidthAtLeftGet( this );
//			nGripHeightAtTop =
//				PmBridge_GetPM()->Bar_GripperHeightAtTopGet( this );
//			if( nGripWidthAtLeft < 0 )
//				nGripWidthAtLeft = 0;
//			if( nGripHeightAtTop < 0 )
//				nGripHeightAtTop = 0;
//		} // if( (GetBarStyle()&CBRS_GRIPPER) != 0 )
//		_GripperWidthAtLeftSet( nGripWidthAtLeft );
//		_GripperHeightAtTopSet( nGripHeightAtTop );
		lpncsp->rgrc[0].left += m_cxLeftBorder;
		lpncsp->rgrc[0].top += m_cyTopBorder;
		lpncsp->rgrc[0].right -= m_cxRightBorder;
		lpncsp->rgrc[0].bottom -= m_cyBottomBorder;
		return;
	}
	CExtControlBar::OnNcCalcSize(bCalcValidRects,lpncsp);
}

void CExtToolControlBar::OnSize(UINT nType, int cx, int cy) 
{
	CExtControlBar::OnSize(nType, cx, cy);
	if( _DraggingGetBar() != this )
		_RecalcLayoutImpl();
	else
		_RecalcPositionsImpl();
}

void CExtToolControlBar::_RecalcLayoutImpl()
{
	if( GetSafeHwnd() == NULL )
		return;
	CExtControlBar::_RecalcLayoutImpl();
	_RecalcPositionsImpl();
	Invalidate();
	if( m_bPresubclassDialogMode )
		UpdateWindow();
}

void CExtToolControlBar::_RibbonPageRslaResetStateData()
{
	ASSERT_VALID( this );
}

LRESULT CExtToolControlBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TIMER && wParam == 0x444 )
	{
		if(		CExtPopupMenuWnd::IsMenuTracking()
			||	g_bMenuTracking
			||	_DraggingGetBar() != NULL
			)
			return 0;
		KillTimer( 0x444 );
		CFrameWnd * pFrame = GetParentFrame();
		if( pFrame != NULL )
		{
			ASSERT_VALID( pFrame );
			pFrame->RecalcLayout();
			if( pFrame->IsKindOf( RUNTIME_CLASS(CMiniDockFrameWnd) ) )
				pFrame->SendMessage( WM_NCPAINT );
		} // if( pFrame != NULL )
		return 0;
	} // if( message == WM_TIMER && wParam == 0x444 )
	if( message == WM_WINDOWPOSCHANGED )
	{
		_RecalcPositionsImpl();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeSite * pSite = GetCustomizeSite();
		if( pSite != NULL && pSite->IsCustomizeMode() )
			pSite->ActiveItemSet();
#endif //(!defined __EXT_MFC_NO_CUSTOMIZE)
//		CExtControlBar * pDraggedBar = _DraggingGetBar();
//		if(		pDraggedBar == NULL
//			||	pDraggedBar != this
//			)
//			_RecalcLayoutImpl();
	} // if( message == WM_WINDOWPOSCHANGED )

	switch( message )
	{
	case WM_WINDOWPOSCHANGED:
	case WM_SIZE:
		_RedrawOnPosChanged();
	break;
	case WM_DESTROY:
	case WM_NCDESTROY:
		m_ptLastClientHover.x = m_ptLastClientHover.y = -32767;
	break;
	} // switch( message )

	return CExtControlBar::WindowProc(message,wParam,lParam);
}

#if _MFC_VER < 0x700
void CExtToolControlBar::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtToolControlBar::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CExtControlBar::OnActivateApp( bActive, hTask );
}

void CExtToolControlBar::OnTimer(__EXT_MFC_UINT_PTR nIDEvent) 
{
	switch( nIDEvent )
	{
		case __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER:
		{
			bool bStateClear = false;
			INT nIndex, nCount = GetButtonsCount();
			for( nIndex = 0; nIndex < nCount; nIndex ++ )
			{
				CExtBarButton * pTBB = GetButton( nIndex );
				ASSERT_VALID( pTBB );
				if(		( ! pTBB->IsVisible() )
					||	( pTBB->GetStyle() & TBBS_HIDDEN ) != 0
					)
					continue;
				bool bHover = pTBB->IsHover();
				if( bHover && m_nBtnIdxHover != nIndex )
				{
					bool bCancelHover = true;
					CExtBarButton * pParentTBB = GetButton( m_nBtnIdxHover );
					if( pParentTBB != NULL )
					{
						for( pParentTBB = pParentTBB->ParentButtonGet(); pParentTBB != NULL; pParentTBB = pParentTBB->ParentButtonGet() )
						{
							if( LPVOID(pParentTBB) == LPVOID(pTBB) )
							{
								bCancelHover = false;
								break;
							}
						}
					}
					if( bCancelHover )
					{
						pTBB->SetHover( false );
						_InvalidateButton( nIndex );
						bStateClear = true;
						break;
					}
				}
			}
			if( bStateClear )
			{
				KillTimer( __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER );
				return;
			}
			bool bActiveHover =
				(	( m_nBtnIdxHover >= 0 )
				&&	CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( m_hWnd )
				) ? true : false;
			if( bActiveHover )
			{
				CPoint point;
				if( ::GetCursorPos( &point ) )
				{
					if( ::WindowFromPoint( point ) != m_hWnd )
						bActiveHover = false;
				}
				else
					bActiveHover = false;
			}
			if(		bActiveHover
				&&	m_pDockSite->GetSafeHwnd() != NULL
				&&	( ! m_pDockSite->IsWindowEnabled() )
				)
				bActiveHover = false;
			if( ! bActiveHover )
			{
				_UpdateHoverButton( CPoint(32767,32767), false );
				KillTimer( __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER );
				CExtPopupMenuTipWnd * pATTW =
					OnAdvancedPopupMenuTipWndGet();
				if(		pATTW != NULL
					&&	pATTW->GetLastParentHWND() == m_hWnd
					)
					pATTW->Hide();
				CWnd::CancelToolTips();
			}
			else
			{
				if( m_nBtnIdxHover >= 0 )
				{
					CPoint pt;
					if( ::GetCursorPos( &pt ) )
					{
						ScreenToClient( &pt );
						INT nHT = HitTest( pt );
						if( nHT != m_nBtnIdxHover )
						{
							KillTimer( __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER );
							if( ! CExtPopupMenuWnd::IsMenuTracking() )
								_UpdateHoverButton( pt, false );
						}
					}
				}
			}
		}
		return; // case __EXT_MFC_ID_TOOLBAR_HOVER_PROTECTION_TIMER
	} // switch( nIDEvent )
	CExtControlBar::OnTimer(nIDEvent);
}

bool CExtToolControlBar::IsPaletteChevronMenu() const
{
	ASSERT_VALID( this );
//	if( m_bPresubclassDialogMode )
//		return false;
	return true;
}

void CExtToolControlBar::_SwitchMenuTrackingIndex(
	int iNewMenuTrackingIndex  // = -1
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	if( m_nBtnIdxMenuTracking == iNewMenuTrackingIndex )
		return;
	if( m_nBtnIdxMenuTracking >= 0 )
	{
		CExtBarButton * pTBB =
			_GetButtonPtr( m_nBtnIdxMenuTracking );
		ASSERT_VALID( pTBB );
		ASSERT( !(pTBB->IsSeparator()) );
		UINT nNewStyle =
			pTBB->GetStyle()
			&
			~(TBBS_PRESSED|TBBS_CHECKED|TBBS_INDETERMINATE);
		pTBB->SetStyle( nNewStyle );
		pTBB->OnPressedTrackingStop();
		_InvalidateButton( m_nBtnIdxMenuTracking );
		m_nBtnIdxMenuTracking = -1;
	} // if( m_nBtnIdxMenuTracking >= 0 )

	if( iNewMenuTrackingIndex >= 0 )
	{
		m_nBtnIdxMenuTracking = iNewMenuTrackingIndex;
		CExtBarButton * pTBB =
			_GetButtonPtr( m_nBtnIdxMenuTracking );
		ASSERT_VALID( pTBB );
		ASSERT( !(pTBB->IsSeparator()) );
		UINT nNewStyle =
			pTBB->GetStyle()
			&
			~(TBBS_CHECKED|TBBS_INDETERMINATE);
		nNewStyle |= TBBS_PRESSED;
		pTBB->SetStyle( nNewStyle );
		pTBB->OnPressedTrackingStart();
		_InvalidateButton( m_nBtnIdxMenuTracking );
	} // if( iNewMenuTrackingIndex >= 0 )

//	UpdateWindow();
}

CExtToolControlBar * CExtToolControlBar::_GetMenuTrackingBar()
{
//CSingleLock _slCsCB( &g_csCB );
//	_slCsCB.Lock();
CExtToolControlBar * pRetVal = NULL;
INT iBar;
	for( iBar = 0; iBar < g_AllBars.GetSize(); ++iBar )
	{
		CExtControlBar * pBar = g_AllBars[iBar];
		ASSERT_VALID( pBar );
		if( pBar->GetSafeHwnd() == NULL )
			continue;
		CExtToolControlBar * pToolControlBar =
			DYNAMIC_DOWNCAST(CExtToolControlBar,pBar);
		if( pToolControlBar == NULL )
			continue;
		if( pToolControlBar->m_nBtnIdxMenuTracking >= 0 )
		{
			pRetVal = pToolControlBar;
			break;
		}
	} // for( iBar = 0; iBar < g_AllBars.GetSize(); ++iBar )
//	_slCsCB.Unlock();
	return pRetVal; // fixed by Mr. Jay Horak
}

void CExtToolControlBar::_CloseTrackingMenus()
{
	__PROF_UIS_MANAGE_STATE;

//	TRACE1(
//		" >> _CloseTrackingMenus()(%s)\n",
//		g_bMenuTracking ? " on  " : " off "
//		);

	g_bMenuTracking = false;

//CSingleLock _slCsCB( &g_csCB );
//	_slCsCB.Lock();
	for( INT iBar=0; iBar<g_AllBars.GetSize(); ++iBar )
	{
		CExtControlBar * pBar = g_AllBars[iBar];
		ASSERT( pBar != NULL );
		if( CWnd::FromHandlePermanent(pBar->m_hWnd) != pBar )
			continue;
		ASSERT_VALID( pBar );
		if( pBar->GetSafeHwnd() == NULL )
			continue;
		CExtToolControlBar * pToolControlBar =
			DYNAMIC_DOWNCAST(CExtToolControlBar,pBar);
		if( pToolControlBar == NULL )
			continue;
		pToolControlBar->_SwitchMenuTrackingIndex();
	}
//	_slCsCB.Unlock();

//	CExtPopupMenuWnd::CancelMenuTracking();
//	CWnd::CancelToolTips();

	if(		(! CExtPopupMenuSite::g_DefPopupMenuSite.IsEmpty() )
		&&	(! CExtPopupMenuSite::g_DefPopupMenuSite.IsShutdownMode() )
		)
	{
		CExtPopupMenuWnd * pTopMenu =
			CExtPopupMenuSite::g_DefPopupMenuSite.
				GetInstance();
		if( pTopMenu->GetSafeHwnd() != NULL )
			pTopMenu->PostMessage( WM_CANCELMODE );
	}
	CWnd::CancelToolTips();

// 	TO FIX: can be tracked other menu
//	ASSERT( !CExtPopupMenuWnd::IsMenuTracking() );
}

void CExtToolControlBar::_CloseCustomMenusAll()
{
static bool g_bClosingCustomMenu = false;
	if( g_bClosingCustomMenu )
		return;
	g_bClosingCustomMenu = true;
	for( INT iBar=0; iBar<g_AllBars.GetSize(); ++iBar )
	{
		CExtControlBar * pBar = g_AllBars[iBar];
		ASSERT( pBar != NULL );
		if( CWnd::FromHandlePermanent(pBar->m_hWnd) != pBar )
			continue;
		ASSERT_VALID( pBar );
		if( pBar->GetSafeHwnd() == NULL )
			continue;
		CExtToolControlBar * pToolControlBar =
			DYNAMIC_DOWNCAST(CExtToolControlBar,pBar);
		if( pToolControlBar == NULL )
			continue;
		pToolControlBar->_CloseCustomMenus();
	}
	g_bClosingCustomMenu = false;
}

void CExtToolControlBar::_CloseCustomMenus()
{
}

CExtToolControlBar * CExtToolControlBar::_GetMenuTrackingToolControlBar()
{
	__PROF_UIS_MANAGE_STATE;
	for( INT iBar=0; iBar<g_AllBars.GetSize(); ++iBar )
	{
		CExtControlBar * pBar = g_AllBars[iBar];
		ASSERT( pBar != NULL );
		if( CWnd::FromHandlePermanent(pBar->m_hWnd) != pBar )
			continue;
		ASSERT_VALID( pBar );
		if( pBar->GetSafeHwnd() == NULL )
			continue;
		CExtToolControlBar * pToolControlBar =
			DYNAMIC_DOWNCAST(CExtToolControlBar,pBar);
		if( pToolControlBar == NULL )
			continue;
		if( pToolControlBar->GetMenuTrackingButton() >= 0 )
			return pToolControlBar;
	}
	return NULL;
}

CExtMenuControlBar * CExtToolControlBar::_GetMenuTrackingMenuControlBar()
{
	__PROF_UIS_MANAGE_STATE;
CExtToolControlBar * pToolControlBar = _GetMenuTrackingToolControlBar();
	if( pToolControlBar == NULL )
		return NULL;
CExtMenuControlBar * pMenuControlBar = DYNAMIC_DOWNCAST( CExtMenuControlBar, pToolControlBar );
	return pMenuControlBar;
}

void CExtToolControlBar::_CbPaintCombinedContent(
	LPVOID pCookie,
	CDC & dc,
	const CWnd & refWndMenu,
	const CRect & rcExcludeArea, // in screen coords
	int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
	)
{
	CExtToolControlBar::g_bMenuTracking = true;
	pCookie;
	dc;
	refWndMenu;
	rcExcludeArea;
	eCombineAlign;
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( refWndMenu.GetSafeHwnd() != NULL );
CExtToolControlBar * pBar = (CExtToolControlBar *)pCookie;
	ASSERT_VALID( pBar );
	ASSERT( pBar->IsKindOf(RUNTIME_CLASS(CExtToolControlBar)) );
	ASSERT( eCombineAlign != CExtPopupMenuWnd::__CMBA_NONE );

	if( rcExcludeArea.IsRectEmpty() )
		return;
	if( pBar->m_nBtnIdxMenuTracking < 0 )
		return;
	ASSERT(
		pBar->m_nBtnIdxMenuTracking <
			pBar->GetButtonsCount()
		);

CExtBarButton * pTBB =
		pBar->_GetButtonPtr(
			pBar->m_nBtnIdxMenuTracking
			);
	ASSERT_VALID( pTBB );

	if(		pBar->PmBridge_GetPM()->GetCb2DbTransparentMode(pTBB)
		&&	pTBB->IsKindOf( RUNTIME_CLASS(CExtBarContentExpandButton) )
		&&	(! pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
		&&	(		(! pBar->m_bPresubclassDialogMode )
				||	pBar->m_bForceBalloonGradientInDialogs
			)
		&&	::GetDeviceCaps( dc.m_hDC, BITSPIXEL ) > 8
		)
		return;

UINT nStyle = pTBB->GetStyle();
bool bHover = pTBB->IsHover();
	pTBB->AnimationClient_CacheGeneratorLock();
	pTBB->SetHover( false );
	pTBB->ModifyStyle(
		0,
		TBBS_PRESSED|TBBS_CHECKED|TBBS_CHECKBOX
		);
CRect rcClientBar,rcClientExcludeArea;
	pBar->GetClientRect( &rcClientBar );
	pBar->ClientToScreen( &rcClientBar );
	refWndMenu.GetClientRect( &rcClientExcludeArea );
	refWndMenu.ClientToScreen( &rcClientExcludeArea );
CPoint ptOffset =
		rcClientBar.TopLeft()
		- rcClientExcludeArea.TopLeft()
		;
CPoint ptViewportOrg = dc.GetViewportOrg();
	dc.SetViewportOrg( ptOffset );
	CExtPopupMenuWnd::g_pCombinedHelper = pTBB;
	pBar->PmBridge_GetPM()->PaintBarButtonCombinedArea( dc, pTBB );
	pTBB->Paint( dc );
	CExtPopupMenuWnd::g_pCombinedHelper = NULL;

	dc.SetViewportOrg( ptViewportOrg );
	pTBB->SetHover( bHover );
	pTBB->SetStyle( nStyle );
	pTBB->AnimationClient_CacheGeneratorUnlock();
}

CExtBarButton * CExtToolControlBar::GetButton( int nIndex )
{
	return _GetButtonPtr( nIndex );
}

CExtBarContentExpandButton * CExtToolControlBar::GetRightButton()
{
	return m_pRightBtn;
}

void CExtToolControlBar::OnCaptureChanged(CWnd *pWnd) 
{
	if(		pWnd != this
//		&&	pWnd != NULL
		)
	{
		if( m_nBtnIdxCapture >= 0 || m_nBtnIdxHover >= 0 )
		{
			SendMessage( WM_CANCELMODE );
			m_nBtnIdxCapture = m_nBtnIdxHover = -1;
		}
	}

	CExtControlBar::OnCaptureChanged(pWnd);
}

BOOL CExtToolControlBar::InitContentExpandButton()
{
	if( m_pRightBtn != NULL )
		return TRUE;
	m_pRightBtn = OnCreateBarRightBtn();
	if( m_pRightBtn == NULL )
		return FALSE;
	ASSERT_VALID( m_pRightBtn );
	ASSERT_KINDOF( CExtBarContentExpandButton, m_pRightBtn );
	m_buttons.Add( m_pRightBtn );
	return TRUE;
}

bool CExtToolControlBar::_CanDockToInnerCircles() const
{
	return false;
}

bool CExtToolControlBar::_CanDockToTabbedContainers(
	CExtControlBar * pDestBar
	) const
{
	pDestBar;
	return false;
}

void CExtToolControlBar::ToggleDocking()
{
	ASSERT_VALID( this );
	if( m_pDockSite == NULL || m_pDockBar == NULL )
		return;
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
#ifdef _DEBUG
	if( !IsFloating() )
	{
		ASSERT_KINDOF( CExtDockBar, m_pDockBar );
	}
#endif // _DEBUG
	ASSERT( m_pDockContext != NULL );
	
CFrameWnd * pParentFrame = GetParentFrame();
	ASSERT_VALID( pParentFrame );
	if( pParentFrame != m_pDockSite )
		pParentFrame->ShowWindow( SW_HIDE );
	m_pDockContext->ToggleDocking();

CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
	pFrame->DelayRecalcLayout();
	_RecalcNcArea();
}


CExtBarButton::e_wrap_t CExtToolControlBar::GetBtnWrapCode() const
{
	ASSERT_VALID( this );
	if(		m_bPresubclassDialogMode
		||	GetSafeHwnd() == NULL
		||	IsFloating()
		)
		return CExtBarButton::__EVT_FLOAT;
	if( IsDockedVertically() )
		return CExtBarButton::__EVT_VERT;
	return CExtBarButton::__EVT_HORZ;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtToolControlBar::_IsDockSiteCustomizeMode() const
{
	ASSERT_VALID( this );
	if( ! m_bCustomizationAllowed )
		return false;
CExtCustomizeSite * pSite = GetCustomizeSite();
	if( pSite == NULL )
//		return CExtControlBar::_IsDockSiteCustomizeMode();
		return false;
	return pSite->IsCustomizeMode();
}

CExtCustomizeSite * CExtToolControlBar::GetCustomizeSite()
{
	ASSERT_VALID( this );
	return m_pCustomizeSite;
}

CExtCustomizeSite * CExtToolControlBar::GetCustomizeSite() const
{
	ASSERT_VALID( this );
	return ((CExtToolControlBar*)this)->GetCustomizeSite();
}

bool CExtToolControlBar::OnCustomizeRegisterBar(
	CExtCustomizeSite * pCustomizeSite
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pCustomizeSite == NULL ); // should called only once
	ASSERT( pCustomizeSite != NULL );
	m_pCustomizeSite = pCustomizeSite;
	if(		(! m_bCustomizationAllowed )
		||	m_bPaletteMode
		)
		return false;
	return true;
}

void CExtToolControlBar::OnCustomizeGetBarInfo(
	CExtCustomizeSite::CToolBarCustomizeInfo * pInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pCustomizeSite != NULL );
	ASSERT( pInfo != NULL );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );

UINT nBarID = (UINT)GetDlgCtrlID();
CExtCmdItem * pCmd =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd(m_hWnd),
			nBarID
			);
	if( pCmd != NULL )
		pInfo->m_strDisplayName =
			pCmd->OnGetToolBarCustomizeName();
	if( pInfo->m_strDisplayName.IsEmpty() )
	{

		if( ! g_ResourceManager->LoadString( pInfo->m_strDisplayName, nBarID ) )
		{
			int nTextLen = GetWindowTextLength();
			if( nTextLen > 0 )
			{
				GetWindowText( pInfo->m_strDisplayName.GetBuffer(nTextLen+2), nTextLen+1 );
				pInfo->m_strDisplayName.ReleaseBuffer();
			} // if( nTextLen > 0 )
			if( pInfo->m_strDisplayName.IsEmpty() )
				pInfo->m_strDisplayName.Format(
					_T("Toolbar #%08X"),
					nBarID
					);
		}
	} // if( pInfo->m_strDisplayName.IsEmpty() )
	if( pInfo->m_pHelperBarButtonsI != NULL )
	{
		CExtCustomizeSite * pCustomizeSite = GetCustomizeSite();
		ASSERT( pCustomizeSite != NULL );
		ASSERT_VALID( pInfo->m_pHelperBarButtonsI );
		ASSERT( pInfo->m_pHelperBarButtonsI->GetCmdID() == ((UINT)GetDlgCtrlID()) );
		ASSERT( pInfo->m_pHelperBarButtonsI->GetNodeCount() == 0 );
		pInfo->m_pHelperBarButtonsI->SetTextUser( pInfo->m_strDisplayName );
		bool bRevertRTL = OnQueryRevertRTL();
		INT nCountOfButtons = _GetButtonsCountImpl();
		for( INT nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
		{
			CExtBarButton * pTBB =
				_GetButtonPtr(
					bRevertRTL
						? (nCountOfButtons - nBtnIdx - 1)
						: nBtnIdx
					);
			ASSERT_VALID( pTBB );
			pTBB->OnCustomizeNodeInit(
				pCustomizeSite,
				pInfo->m_pHelperBarButtonsI
				);
		} // for( INT nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	} // if( pInfo->m_pHelperBarButtonsI != NULL )
}

void CExtToolControlBar::OnCustomizeUpdateInitialBarState(
	CExtCustomizeCmdTreeNode * pParentNode,
	bool bRecalcLayout
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pParentNode );
	ASSERT( m_pCustomizeSite != NULL );
INT nNodeCount = pParentNode->GetNodeCount();
INT nCountOfButtons = _GetButtonsCountImpl();
	ASSERT( nNodeCount <= nCountOfButtons );
bool bRevertRTL = OnQueryRevertRTL();
INT nNodePos = bRevertRTL ? (pParentNode->GetNodeCount()-1) : 0;
	for( INT nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	{
		CExtBarButton * pTBB =
			_GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		CExtCustomizeCmdTreeNode * pCmdNodeI =
			pTBB->GetCmdNode( true );
		if( pCmdNodeI == NULL )
			continue;
		ASSERT( !pTBB->IsSeparator() );
		ASSERT_VALID( pCmdNodeI );
		CExtCustomizeCmdTreeNode * pCmdNodeC =
			pParentNode->ElementAt( nNodePos );
		if( bRevertRTL )
			nNodePos --;
		else
			nNodePos ++;
		ASSERT_VALID( pCmdNodeC );
		// re-init spec buttons
		pTBB =
			m_pCustomizeSite->OnCreateToolbarButton(
				this,
				pCmdNodeI,
				pCmdNodeI
				);
		ASSERT_VALID( pTBB );
		VERIFY(
			InsertSpecButton(
				nBtnIdx,
				pTBB,
				FALSE
				)
			);
		pTBB->SetBasicCmdNode( pCmdNodeI );
		pTBB->SetCustomizedCmdNode( pCmdNodeC );
		pTBB->OnCustomizeUpdateProps();
		RemoveButton( nBtnIdx + 1, FALSE );
	} // for( INT nBtnIdx = 0; nBtnIdx < nCountOfButtons; nBtnIdx++ )
	if( bRecalcLayout )
		_RecalcLayoutImpl();
}

void CExtToolControlBar::OnCustomizeModeEnter()
{
	ASSERT_VALID( this );
	if(		(! m_bCustomizationAllowed )
		||	m_pDockSite == NULL
		||	m_pDockBar == NULL
		||	(! m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockOuterBar)) )
		)
		return;
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );

CExtCustomizeSite * pSite = GetCustomizeSite();
	if( pSite != NULL )
		pSite->RegisterCommandDropTarget( this, this );

	if(		(! IsDocked() )
		||	_GetFullRowMode()
		)
		return;
INT nCountOfButtons = _GetButtonsCountImpl();
	if( nCountOfButtons == 0 )
		return;
	if( m_pRightBtn == NULL )
		return;
	if( m_pRightBtn->GetButtons().GetSize() == 0 )
		return;
INT nOwnIdxInDocker = m_pDockBar->FindBar(this);
	ASSERT( 0 < nOwnIdxInDocker && nOwnIdxInDocker < (m_pDockBar->m_arrBars.GetSize()-1) );
	if( m_pDockBar->m_arrBars[nOwnIdxInDocker+1] != NULL )
		m_pDockBar->m_arrBars.InsertAt( nOwnIdxInDocker+1, (CControlBar *)NULL );
	if( m_pDockBar->m_arrBars[nOwnIdxInDocker-1] != NULL )
		m_pDockBar->m_arrBars.InsertAt( nOwnIdxInDocker, (CControlBar *)NULL );
	Invalidate();
}

void CExtToolControlBar::OnCustomizeModeLeave()
{
	ASSERT_VALID( this );
//	if( ! m_bCustomizationAllowed )
//		return;
}

void CExtToolControlBar::OnCustomizeListBoxUpdate( CExtCustomizeToolbarsListBox * pWndList )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWndList );
	if( ! m_bCustomizationAllowed )
		return;
	CExtControlBar::OnCustomizeListBoxUpdate( pWndList );
}

DROPEFFECT CExtToolControlBar::OnCustomizeTargetOver(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CPoint point,
	DWORD dwKeyState
	)
{
	ASSERT_VALID( this );
	ASSERT( !_dragInfo.IsEmpty() );
	_dragInfo;
	if( ! m_bCustomizationAllowed )
		return DROPEFFECT_NONE;
	if(		m_pDockBar != NULL
		&&	IsFloating()
		)
		GetParentFrame()->SetWindowPos(
			&CWnd::wndTop, 0, 0, 0, 0,
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
			);
int nCountTest = _GetButtonsCountImpl();
int nCountReal = nCountTest;
	if( m_pRightBtn != NULL )
	{
		nCountTest--;
		ASSERT( nCountTest >= 0 );
	}
	if(		nCountTest > 0
		&&	_GetButtonPtr(0)->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
		)
	{
		nCountTest--;
		ASSERT( nCountTest >= 0 );
	}
	if(		nCountReal > 1
		&&	_GetButtonPtr(
				( m_pRightBtn != NULL )
					? (nCountReal-2)
					: (nCountReal-1)
				)->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
		)
	{
		nCountTest--;
		ASSERT( nCountTest >= 0 );
	}
	if( nCountTest == 0 )
	{
		if( (dwKeyState & MK_CONTROL) != NULL )
			return DROPEFFECT_COPY;
		return DROPEFFECT_MOVE;
	}

	if( m_pDragTargetTBB != NULL )
		_InvalidateButton( _GetIndexOf(m_pDragTargetTBB) );
int nHitIdx = _HitTestImpl( point, 0, 0, true );
	if( nHitIdx < 0 )
	{
		m_pDragTargetTBB = NULL;
		m_nDragTargetShift = 0;
		UpdateWindow();
		return DROPEFFECT_NONE;
	}
CExtBarButton * pHitTBB = _GetButtonPtr( nHitIdx );
	ASSERT_VALID( pHitTBB );
	if( pHitTBB->IsSeparator() )
	{
		nHitIdx--;
		if( nHitIdx < 0 )
		{
			m_pDragTargetTBB = NULL;
			m_nDragTargetShift = 0;
			UpdateWindow();
			return DROPEFFECT_NONE;
		}
		pHitTBB = _GetButtonPtr( nHitIdx );
		ASSERT_VALID( pHitTBB );
		if( pHitTBB->IsSeparator() )
		{
			ASSERT( FALSE ); // double  separators?!
			m_pDragTargetTBB = NULL;
			m_nDragTargetShift = 0;
			UpdateWindow();
			return DROPEFFECT_NONE;
		} // if( pHitTBB->IsSeparator() )
	} // if( pHitTBB->IsSeparator() )
	m_nDragTargetShift =
		pHitTBB->CalculateDropOverEffect( point, _dragInfo );
	if( m_nDragTargetShift == 0 )
	{
		m_pDragTargetTBB = NULL;
		m_nDragTargetShift = 0;
		UpdateWindow();
		return DROPEFFECT_NONE;
	} // if( m_nDragTargetShift == 0 )
	if( m_pDragTargetTBB != pHitTBB )
	{
		m_pDragTargetTBB = pHitTBB;
		_InvalidateButton( _GetIndexOf(m_pDragTargetTBB) );
	}
	UpdateWindow();
	if( (dwKeyState & MK_CONTROL) != NULL )
		return DROPEFFECT_COPY;
	return DROPEFFECT_MOVE;
}

void CExtToolControlBar::OnCustomizeTargetLeave()
{
	ASSERT_VALID( this );
	if( ! m_bCustomizationAllowed )
		return;
	if( m_pDragTargetTBB == NULL )
		return;
	_InvalidateButton( _GetIndexOf(m_pDragTargetTBB) );
	m_pDragTargetTBB = NULL;
	m_nDragTargetShift = 0;
	UpdateWindow();
}

bool CExtToolControlBar::OnCustomizeTargetDrop(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CPoint point,
	DROPEFFECT de
	)
{
	// returns true if drop with any effect
	// has been performed
	ASSERT_VALID( this );
	ASSERT( !_dragInfo.IsEmpty() );
	_dragInfo;
	point;
	if( ! m_bCustomizationAllowed )
		return false;
	ASSERT(
			de == DROPEFFECT_NONE
		||	de == DROPEFFECT_MOVE
		||	de == DROPEFFECT_COPY
		);
	if( de == DROPEFFECT_NONE )
		return false;
	if( m_pDragTargetTBB == NULL )
	{
		CExtCustomizeSite * pSite =
			GetCustomizeSite();
		ASSERT( pSite != NULL );
		int nInsertPos = 0;
		if(		GetButtonsCount() > 0
			&&	_GetButtonPtr(0)->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
			)
			nInsertPos = 1;
		pSite->OnInsertToolbarCmdNode(
			_dragInfo,
			this,
			nInsertPos,
			true
			);
		if( m_pDockBar != NULL )
		{
			Invalidate();
			GetParentFrame()->RecalcLayout();
		}
		return true;
	} // if( m_pDragTargetTBB == NULL )

	ASSERT_VALID( m_pDragTargetTBB );
	
int nBtn = _GetIndexOf(m_pDragTargetTBB);
	ASSERT( 0 <= nBtn && nBtn < _GetButtonsCountImpl() );
	_InvalidateButton( nBtn );

bool bSeparatorAdd = false;
bool bSeparatorRemove = false;
	if( de == DROPEFFECT_MOVE )
	{
		if( m_pDragSourceTBB == m_pDragTargetTBB )
		{
			if( m_nDragTargetShift == -2 )
				return false; // no reinsert
			if( m_nDragTargetShift < 0 )
			{ // if remove separator
				if(		nBtn > 0
					&&	_GetButtonPtr(nBtn-1)->IsSeparator()
					)
					bSeparatorRemove = true;
				else
					return false; // no reinsert
			} // if remove separator
			else
			{ // if add separator
				if(	(	nBtn > 1
					||	(	nBtn == 1
						&&	(! _GetButtonPtr(0)->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton)) )
						)
					)
					&&	(! _GetButtonPtr(nBtn-1)->IsSeparator() )
					)
					bSeparatorAdd = true;
				else
					return false; // no reinsert
			} // if add separator
		} // if( m_pDragSourceTBB == m_pDragTargetTBB )
		else if(
			m_pDragSourceTBB != NULL
			&&	nBtn > 0
			&&	m_nDragTargetShift < 0
			&&	_GetIndexOf(m_pDragSourceTBB) == (nBtn-1)
			&&	(!(	nBtn == 2
				&&	_GetButtonPtr(0)->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
				))
			)
		{
			if(	(	nBtn > 1
				&&	(! _GetButtonPtr(nBtn-2)->IsSeparator() )
				)
				||	nBtn <= 1
				)
			{ // avoid unwanted separator insert
				bSeparatorAdd = true;
				nBtn--;
			} // avoid unwanted separator insert
		}
	} // if( de == DROPEFFECT_MOVE )

	if( bSeparatorRemove )
	{
		CExtBarButton * pModifyTBB = _GetButtonPtr(nBtn);
		ASSERT_VALID( pModifyTBB );
		CExtCustomizeCmdTreeNode * pNode = pModifyTBB->GetCmdNode();
		ASSERT_VALID( pNode );
		pNode->ModifyFlags( 0, __ECTN_GROUP_START );
		RemoveButton(nBtn-1);
		UpdateWindow();
		if(		m_pDockBar != NULL
			//&&	IsFloating()
			)
		{
			Invalidate();
			GetParentFrame()->RecalcLayout();
		}
		m_bHelperSeparatorAction = true;
	} // if( bSeparatorRemove )
	else if( bSeparatorAdd )
	{
		CExtBarButton * pModifyTBB = _GetButtonPtr(nBtn);
		ASSERT_VALID( pModifyTBB );
		CExtCustomizeCmdTreeNode * pNode = pModifyTBB->GetCmdNode();
		ASSERT_VALID( pNode );
		pNode->ModifyFlags( __ECTN_GROUP_START );
		InsertButton(nBtn);
		UpdateWindow();
		if(		m_pDockBar != NULL
			//&&	IsFloating()
			)
		{
			Invalidate();
			GetParentFrame()->RecalcLayout();
		}
		m_bHelperSeparatorAction = true;
	} // else if( bSeparatorAdd )
	else
	{ // if command drop
		int nInsertPos = (m_nDragTargetShift < 0) ? nBtn : (nBtn+1);
		m_pDragTargetTBB = NULL;
		m_nDragTargetShift = 0;
		UpdateWindow();
		//InsertButton( nInsertPos, pCmdItem->m_nCmdID );
		CExtCustomizeSite * pSite =
			GetCustomizeSite();
		ASSERT( pSite != NULL );
		pSite->OnInsertToolbarCmdNode(
			_dragInfo,
			this,
			nInsertPos,
			true
			);
		if(		m_pDockBar != NULL
			//&&	IsFloating()
			)
		{
			Invalidate();
			GetParentFrame()->RecalcLayout();
		}
	} // if command drop
	return true;
}

void CExtToolControlBar::OnCustomizeSourceDragComplete(
	DROPEFFECT de,
	bool bCanceled,
	bool * p_bNoResetActiveItem
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDragSourceTBB );
	ASSERT( p_bNoResetActiveItem != NULL );
	ASSERT(
			de == DROPEFFECT_NONE
		||	de == DROPEFFECT_MOVE
		||	de == DROPEFFECT_COPY
		);
	if( ! m_bCustomizationAllowed )
		return;
int nBtnSrc = _GetIndexOf(m_pDragSourceTBB);
	if( (!bCanceled) && (!m_bHelperSeparatorAction) )
	{
		CExtBarButton * pHtCurrTBB = NULL;
		if( GetButtonsCount() > 0 )
		{
			CPoint point( 0, 0 );
			::GetCursorPos( &point );
			ScreenToClient( &point );
			INT nButton = HitTest( point );
			if( nButton >= 0 )
				pHtCurrTBB = _GetButtonPtr( nButton );
		} // if( GetButtonsCount() > 0 )
		if( de == DROPEFFECT_NONE || de == DROPEFFECT_MOVE )
		{
			if(		m_pDragSourceTBB == m_pDragTargetTBB
				||	m_pDragSourceTBB == pHtCurrTBB
				)
			{ // if keep source button active
				m_pDragSourceTBB = NULL;
				if( _IsDockSiteCustomizeMode() )
					*p_bNoResetActiveItem = true;
			} // if keep source button active
			else
			{ // if remove source button
				m_pDragSourceTBB = NULL;
				CExtCustomizeSite * pSite = GetCustomizeSite();
				ASSERT( pSite != NULL );
				CExtCustomizeCmdTreeNode * pNode =
					pSite->DraggedNodeGet();
				ASSERT_VALID( pNode );
				ASSERT( ((LPVOID)pNode) == ((LPVOID)_GetButtonPtr(nBtnSrc)->GetCmdNode(false)) );
				if( pSite->GetPopupDropTargetNode() == pNode )
				{
					pSite->DropTargetPopupCancel();
					ASSERT( pSite->GetPopupDropTargetNode() != pNode );
				}
				RemoveButton( nBtnSrc );
				pSite->DraggedNodeSet( NULL );
				CExtCmdItem * pCmdItem =
					g_CmdManager->CmdGetPtr(
						g_CmdManager->ProfileNameFromWnd(m_hWnd),
						pNode->GetCmdID( false )
						);
				ASSERT( pCmdItem != NULL );
				ASSERT_VALID( pNode->GetParentNode() );
				int nOwnNodeIndex = pNode->GetOwnIndex();
				int nSiblingCount = pNode->GetParentNode()->GetNodeCount();
				bool bRemovePrevSeparator =
					(pNode->GetFlags() & __ECTN_GROUP_START)
						? true : false;
				bool bRemoveNextSeparator = false;
				if(		bRemovePrevSeparator
					&&	nOwnNodeIndex < (nSiblingCount-1)
					)
				{
					ASSERT( nOwnNodeIndex > 0 );
					ASSERT( nBtnSrc > 0 );
					if( (pNode->GetParentNode()->ElementAt(nOwnNodeIndex+1)
							->GetFlags()&__ECTN_GROUP_START) == 0
						)
					{
						pNode->GetParentNode()->ElementAt(nOwnNodeIndex+1)
							->ModifyFlags( __ECTN_GROUP_START );
						bRemovePrevSeparator = false;
					}
				}
				if(		nOwnNodeIndex == 0
					&&	nSiblingCount > 1
					&&	(pNode->GetParentNode()->ElementAt(1)
							->GetFlags()&__ECTN_GROUP_START) != 0
					)
				{
					pNode->GetParentNode()->ElementAt(1)->ModifyFlags( 0, __ECTN_GROUP_START );
					bRemoveNextSeparator = true;
				}
				pNode->RemoveSelf( pCmdItem );
				bool bRevertRTL = OnQueryRevertRTL();
				if( bRevertRTL )
				{
					if( bRemovePrevSeparator )
					{
						ASSERT( _GetButtonPtr(nBtnSrc)->IsSeparator() );
						RemoveButton( nBtnSrc );
					}
					if( bRemoveNextSeparator )
					{
						ASSERT( _GetButtonPtr(nBtnSrc-1)->IsSeparator() );
						RemoveButton( nBtnSrc-1 );
					}
				} // if( bRevertRTL )
				else
				{
					if( bRemoveNextSeparator )
					{
						ASSERT( _GetButtonPtr(nBtnSrc)->IsSeparator() );
						RemoveButton( nBtnSrc );
					}
					if( bRemovePrevSeparator )
					{
						ASSERT( _GetButtonPtr(nBtnSrc-1)->IsSeparator() );
						RemoveButton( nBtnSrc-1 );
					}
				} // else from if( bRevertRTL )
				if( m_pDockBar != NULL )
				{
					CFrameWnd * pRecalcFrame = GetParentFrame();
					ASSERT_VALID( pRecalcFrame );
					pRecalcFrame->RecalcLayout();
					if( IsFloating() )
						pRecalcFrame->SendMessage( WM_NCPAINT );
					Invalidate();
				} // if( m_pDockBar != NULL )
			} // if remove source button
		}  // if( de == DROPEFFECT_NONE || de == DROPEFFECT_MOVE )
		else
		{
			_InvalidateButton( nBtnSrc );
			if( m_pDockBar != NULL )
			{
				CFrameWnd * pRecalcFrame = GetParentFrame();
				ASSERT_VALID( pRecalcFrame );
				pRecalcFrame->RecalcLayout();
				if( IsFloating() )
					pRecalcFrame->SendMessage( WM_NCPAINT );
			} // if( m_pDockBar != NULL )
		}
	} // if( (!bCanceled) && (!m_bHelperSeparatorAction) )
	else
		_InvalidateButton( nBtnSrc );
	m_pDragSourceTBB = NULL;
	m_pDragTargetTBB = NULL;
	m_nDragTargetShift = 0;
	m_bHelperSeparatorAction = false;
	UpdateWindow();
}

INT CExtToolControlBar::OnCustomizeCalcCmdNodePos(
	INT nButtonPos,
	bool * p_bReinitGroupStart // = NULL
	)
{
	ASSERT_VALID( this );
	if( p_bReinitGroupStart != NULL )
		(*p_bReinitGroupStart) = false;
	if( ! m_bCustomizationAllowed )
		return 0;
INT nButtonsCount = _GetButtonsCountImpl();
	ASSERT( nButtonPos <= nButtonsCount );
	if( nButtonsCount == 0 )
		return 0;
INT nCmdPos = 0;
bool bRevertRTL = OnQueryRevertRTL();
	if( bRevertRTL )
	{
		for( INT nBtnIdx = nButtonsCount-1; nBtnIdx > nButtonPos; nBtnIdx--  )
		{
			CExtBarButton * pTBB =
				_GetButtonPtr( nBtnIdx );
			ASSERT_VALID( pTBB );
			if( pTBB->GetCmdNode() == NULL )
				continue;
			ASSERT( ! pTBB->IsSeparator() );
			ASSERT_VALID( pTBB->GetCmdNode() );
			nCmdPos ++;
		} // for( INT nBtnIdx = nButtonsCount-1; nBtnIdx > nButtonPos; nBtnIdx--  )
		ASSERT( nCmdPos <= nButtonsCount );
		if( p_bReinitGroupStart != NULL && nButtonPos < nButtonsCount-1 )
		{
			CExtBarButton * pTBB = _GetButtonPtr( nButtonPos + 1 );
			ASSERT_VALID( pTBB );
			if( pTBB->IsSeparator() )
				(*p_bReinitGroupStart) = true;
		} // if( p_bReinitGroupStart != NULL && nButtonPos > 0 )
	} // if( bRevertRTL )
	else
	{
		for( INT nBtnIdx = 0; nBtnIdx < nButtonPos; nBtnIdx++  )
		{
			CExtBarButton * pTBB =
				_GetButtonPtr( nBtnIdx );
			ASSERT_VALID( pTBB );
			if( pTBB->GetCmdNode() == NULL )
				continue;
			ASSERT( ! pTBB->IsSeparator() );
			ASSERT_VALID( pTBB->GetCmdNode() );
			nCmdPos ++;
		} // for( INT nBtnIdx = 0; nBtnIdx < nButtonPos; nBtnIdx++  )
		ASSERT( nCmdPos <= nButtonsCount );
		if( p_bReinitGroupStart != NULL && nButtonPos > 0 )
		{
			CExtBarButton * pTBB = _GetButtonPtr( nButtonPos - 1 );
			ASSERT_VALID( pTBB );
			if( pTBB->IsSeparator() )
				(*p_bReinitGroupStart) = true;
		} // if( p_bReinitGroupStart != NULL && nButtonPos > 0 )
	} // else from if( bRevertRTL )
	return nCmdPos;
}

bool CExtToolControlBar::OnCustomizeCanShowHideButtons()
{
	ASSERT_VALID( this );
	if(		( ! m_bCustomizationAllowed )
		||	m_bPaletteMode
		||	IsKindOf(RUNTIME_CLASS(CExtPanelControlBar))
		||	IsKindOf(RUNTIME_CLASS(CExtMenuControlBar))
		||	(! IsVisible() )
		)
		return false;
INT nButtonsCount = _GetButtonsCountImpl();
	if( nButtonsCount == 0 )
		return false;
	for( INT nBtnIdx = 0; nBtnIdx < nButtonsCount; nBtnIdx++  )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		if(		pTBB->IsSeparator()
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton))
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiRightButton))
			||	pTBB->GetCmdNode() == NULL
			)
			continue;
		ASSERT_VALID( pTBB->GetCmdNode() );
		if(	(pTBB->GetCmdNode()->GetFlags()&__ECTN_TBB_CANNOT_HIDE) != 0 )
			continue;
		return true;
	} // for( INT nBtnIdx = 0; nBtnIdx < nButtonPos; nBtnIdx++  )
	return false;
}

void CExtToolControlBar::OnCustomizeInitShowHideButtonMenu(
	CExtPopupMenuWnd * pFolder
	)
{
	ASSERT_VALID( this );
	ASSERT( OnCustomizeCanShowHideButtons() );
	ASSERT_VALID( pFolder );
	ASSERT( IsVisible() );

	if( ! m_bCustomizationAllowed )
		return;
INT nButtonsCount = _GetButtonsCountImpl();
	ASSERT( nButtonsCount > 0 );
INT nInsertPos = 0;
	for( INT nBtnIdx = 0; nBtnIdx < nButtonsCount; nBtnIdx++  )
	{
		CExtBarButton * pTBB = _GetButtonPtr( nBtnIdx );
		ASSERT_VALID( pTBB );
		if(		pTBB->IsSeparator()
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton))
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiDocButton))
			||	pTBB->IsKindOf(RUNTIME_CLASS(CExtBarMdiRightButton))
			||	pTBB->GetCmdNode() == NULL
			)
			continue;
		ASSERT_VALID( pTBB->GetCmdNode() );
		if(	(pTBB->GetCmdNode()->GetFlags()&__ECTN_TBB_CANNOT_HIDE) != 0 )
			continue;
		VERIFY(
			pFolder->ItemInsert(
				pTBB->GetCmdID(false),
				nInsertPos
				)
			);
		CExtPopupMenuWnd::MENUITEMDATA & mi =
			pFolder->ItemGetInfo( nInsertPos );
		mi.SetForceEnabled();
		mi.SetExtraMark(
			(CExtPopupMenuWnd::MENUITEMDATA::pCbItemClick)
				stat_CbXtraMarkState
			);
		mi.LParamSet( (LPARAM)pTBB );
		pFolder->ItemSetDisplayed( nInsertPos, true );
		mi.SetExtraChecked(
			( (pTBB->GetStyle() & TBBS_HIDDEN) != 0 )
				? false : true
			);
		nInsertPos++;
	} // for( INT nBtnIdx = 0; nBtnIdx < nButtonPos; nBtnIdx++  )
}

bool CExtToolControlBar::stat_CbXtraMarkState(
	CExtPopupMenuWnd * pPopup,
	LPVOID pItemData
	)
{
	ASSERT_VALID( pPopup );
	pPopup;
	ASSERT( pItemData != NULL );
CExtPopupMenuWnd::MENUITEMDATA & mi =
		*((CExtPopupMenuWnd::MENUITEMDATA *)pItemData);
	ASSERT( mi.IsExtraMark() );
	mi.SetExtraChecked( !mi.IsExtraChecked() );
CExtBarButton * pTBB = (CExtBarButton*)mi.LParamGet();
	ASSERT_VALID( pTBB );
CExtToolControlBar * pBar = pTBB->GetBar();
	ASSERT_VALID( pBar );
#ifdef _DEBUG
INT nButtonIndex = pBar->_GetIndexOf( pTBB );
	ASSERT( nButtonIndex >= 0 );
#endif // _DEBUG
bool bShow = mi.IsExtraChecked();
	pTBB->ModifyStyle(
		bShow ? 0 : TBBS_HIDDEN,
		bShow ? TBBS_HIDDEN : 0
		);
	pTBB->Show( bShow );
	pBar->_RecalcPositionsImpl();
	pBar->Invalidate();
	pBar->UpdateWindow();
	if( pBar->m_bPresubclassDialogMode )
		return true; // not important
	if(		pBar->m_pDockSite != NULL
		&&	pBar->IsFloating()
		)
	{
		CFrameWnd * pFrame = pBar->GetParentFrame();
		ASSERT_VALID( pFrame );
		ASSERT_KINDOF( CMiniDockFrameWnd, pFrame );
	}
	pBar->SetTimer( 0x444, 1, NULL );
	return true; // not important
}

bool CExtToolControlBar::stat_CbCmdDeliver(
	CExtPopupMenuWnd * pPopup,
	LPVOID pItemData
	)
{
	ASSERT_VALID( pPopup );
	pPopup;
	ASSERT( pItemData != NULL );
CExtPopupMenuWnd::MENUITEMDATA & mi =
		*((CExtPopupMenuWnd::MENUITEMDATA *)pItemData);
	if( mi.IsExtraMark() )
		return false;
	if( mi.IsSeparator() )
		return false;
	if( mi.IsPopup() )
		return false;
	if( mi.GetCmdID() != ID_EXT_RESET_TOOLBAR )
		return false;
	ASSERT( pPopup->GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite(
			pPopup->GetSafeHwnd()
			);
	if( pSite == NULL )
		return false;
CExtToolControlBar * pBar = (CExtToolControlBar *)
		mi.LParamGet();
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtToolControlBar, pBar );
	ASSERT( pSite->BarIsRegistered( pBar ) );
	pSite->BarDefCmdTargetSet( pBar );
	return false;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtToolControlBar::FtLayoutQuery::FtLayoutQuery()
	: m_pToolBar( NULL )
	, m_pMiniFrame( NULL )
	, m_hWndFor( NULL )
{
}

CExtToolControlBar::FtLayoutQuery::~FtLayoutQuery()
{
	Empty();
}

void CExtToolControlBar::FtLayoutQuery::Empty()
{
	m_arrRowRects.RemoveAll();
	m_arrRowBtnIndices.RemoveAll();
	m_pToolBar = NULL;
	m_pMiniFrame = NULL;
	m_hWndFor = NULL;
}

bool CExtToolControlBar::FtLayoutQuery::Query( HWND hWndFor )
{
	Empty();
	if( hWndFor == NULL || (! ::IsWindow( hWndFor ) ) )
		return false;
	m_hWndFor = hWndFor;
HWND hWnd = hWndFor;
	for( ; hWnd != NULL; hWnd = ::GetParent(hWnd) )
	{
		if( m_pMiniFrame != NULL && hWndFor == m_pMiniFrame->m_hWnd )
			break;
		__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetClassLong( hWnd, GWL_STYLE );
		if( (dwStyle&WS_CHILD) == 0 && m_pMiniFrame == NULL )
			break;
		CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
		if( pWnd == NULL )
			continue;
		if( m_pMiniFrame == NULL )
		{
			CFrameWnd * pFrame = pWnd->GetParentFrame();
			if( pFrame == NULL )
				break;
			m_pMiniFrame =
				DYNAMIC_DOWNCAST(
					CExtMiniDockFrameWnd,
					pFrame
					);
			if( m_pMiniFrame == NULL )
				break;
		} // if( m_pMiniFrame == NULL )
		m_pToolBar =
			DYNAMIC_DOWNCAST(
				CExtToolControlBar,
				pWnd
				);
		if( m_pToolBar == NULL )
			continue;
		if( m_pToolBar->m_hWnd == m_hWndFor )
			break; // toolbar performs self-painting
		return m_pToolBar->_DoFtLayoutQuery( this, NULL );
	} // for( ; hWnd != NULL && ::IsWindow(hWnd); hWnd = ::GetParent(hWnd) )
	return false;
}

void CExtToolControlBar::FtLayoutQuery::ReMapRectsToTargetClient()
{
	if(		m_hWndFor == NULL
		||	(! ::IsWindow( m_hWndFor ) )
		||	(m_pToolBar->GetSafeHwnd() == NULL )
		)
		return;
	ASSERT_VALID( m_pToolBar );
INT nIdx, nCount = (INT)m_arrRowRects.GetSize();
	ASSERT( m_arrRowBtnIndices.GetSize() == nCount );
	for( nIdx = 0; nIdx < nCount; nIdx++ )
	{
		CRect & rcRowBk = m_arrRowRects.ElementAt( nIdx );
		m_pToolBar->ClientToScreen( &rcRowBk );
		LPPOINT pPoints = (LPPOINT)&rcRowBk;
		::ScreenToClient( m_hWndFor, pPoints );
		pPoints++;
		::ScreenToClient( m_hWndFor, pPoints );
	} // for( nIdx = 0; nIdx < nCount; nIdx++ )
}

void CExtToolControlBar::FtLayoutQuery::ReMapRectsToTargetWindow()
{
	if(		m_hWndFor == NULL
		||	(! ::IsWindow( m_hWndFor ) )
		||	(m_pToolBar->GetSafeHwnd() == NULL )
		)
		return;
	ASSERT_VALID( m_pToolBar );
INT nIdx, nCount = (INT)m_arrRowRects.GetSize();
	ASSERT( m_arrRowBtnIndices.GetSize() == nCount );
CRect rcTargetWnd;
	::GetWindowRect( m_hWndFor, &rcTargetWnd );
	for( nIdx = 0; nIdx < nCount; nIdx++ )
	{
		CRect & rcRowBk = m_arrRowRects.ElementAt( nIdx );
		m_pToolBar->ClientToScreen( &rcRowBk );
		rcRowBk.OffsetRect( -rcTargetWnd.left, -rcTargetWnd.top );
	} // for( nIdx = 0; nIdx < nCount; nIdx++ )
}

void CExtToolControlBar::FtLayoutQuery::DrawRects( CDC & dc )
{
	if(		m_hWndFor == NULL
		||	(! ::IsWindow( m_hWndFor ) )
		||	(m_pToolBar->GetSafeHwnd() == NULL )
		)
		return;
	ASSERT_VALID( m_pToolBar );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
INT nIdx, nCount = (INT)m_arrRowRects.GetSize();
	ASSERT( m_arrRowBtnIndices.GetSize() == nCount );
	for( nIdx = 0; nIdx < nCount; nIdx++ )
	{
		CRect & rcRowBk = m_arrRowRects.ElementAt( nIdx );
		INT nBtnIdx = m_arrRowBtnIndices[ nIdx ];
		if( dc.RectVisible( &rcRowBk ) )
			m_pToolBar->PmBridge_GetPM()->PaintFloatToolbarRowBk(
				dc,
				m_pToolBar,
				nBtnIdx,
				rcRowBk
				);
	} // for( nIdx = 0; nIdx < nCount; nIdx++ )
}

/////////////////////////////////////////////////////////////////////////////
// CExtPanelControlBar

IMPLEMENT_DYNCREATE(CExtPanelControlBar, CExtToolControlBar)

CExtPanelControlBar::CExtPanelControlBar()
	: m_rcMargins( 5, 5, 5, 5 )
{
	m_bPaletteMode = true;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_bVisibleInCustomizeListBox = false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

CExtPanelControlBar::~CExtPanelControlBar()
{
}

BEGIN_MESSAGE_MAP(CExtPanelControlBar, CExtToolControlBar)
	//{{AFX_MSG_MAP(CExtPanelControlBar)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtToolControlBar message handlers

CSize CExtPanelControlBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	ASSERT_VALID( this );

	if(	(nLength == -1)
		&& !(dwMode & (LM_MRUWIDTH|LM_COMMIT))
		&&  (dwMode & (LM_HORZDOCK|LM_VERTDOCK))
		)
		return
			CalcFixedLayout(
				dwMode & LM_STRETCH,
				dwMode & LM_HORZDOCK
				);
	ASSERT(
		(dwMode&(LM_HORZ|LM_HORZDOCK))
		||
		(!(dwMode&LM_HORZDOCK))
		);
CSize sizeCalcLayout = _CalcLayout( dwMode, nLength );
	return sizeCalcLayout;
}

CSize CExtPanelControlBar::CalcFixedLayout(
	BOOL bStretch,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );

DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;
	ASSERT(
		(dwMode&(LM_HORZ|LM_HORZDOCK))
		||
		(!(dwMode&LM_HORZDOCK))
		);
CSize sizeCalcLayout = _CalcLayout( dwMode );
	_RecalcNcArea();
	return sizeCalcLayout;
}

CSize CExtPanelControlBar::_CalcLayout(
	DWORD dwMode,
	int nLength // = -1
	)
{
	ASSERT_VALID( this );
	dwMode;
	nLength;

CSize sizeCalcLayout( 0, 0 );
	if( !IsFloating() )
	{
		sizeCalcLayout += CSize( 2, 2 );
		bool bGripperAtTop = false;
		if( IsBarWithGripper( &bGripperAtTop ) )
		{
			CSize sizeGrip = _RectGripGet().Size();
			if( bGripperAtTop )
				sizeCalcLayout.cy += sizeGrip.cy;
			else
				sizeCalcLayout.cx += sizeGrip.cx;
		}
	}

	if( IsDockedHorizontally() )
	{
		sizeCalcLayout += _QueryFixedBarNewSize( __FSQ_DOCKED_H );
		SetInitDesiredSizeHorizontal( sizeCalcLayout );
		m_nMinHW = sizeCalcLayout.cx;
	}
	else if( IsDockedVertically() )
	{
		sizeCalcLayout += _QueryFixedBarNewSize( __FSQ_DOCKED_V );
		SetInitDesiredSizeVertical( sizeCalcLayout );
		m_nMinVH = sizeCalcLayout.cy;
	}
	else if( IsFloating() )
	{
		sizeCalcLayout += _QueryFixedBarNewSize( __FSQ_FLOATING );
		SetInitDesiredSizeFloating( sizeCalcLayout );
	}
	else
	{
		sizeCalcLayout += CSize( 20, 20 );
	}

	if( m_pDockSite == NULL )
	{ // specific for dialog mode
		if( IsDockedHorizontally() )
			sizeCalcLayout.cy += 4;
		else
			sizeCalcLayout.cx += 4;
	} // specific for dialog mode

	return sizeCalcLayout;
}

INT CExtPanelControlBar::_CalcDesiredMinHW() const
{
	ASSERT_VALID( this );

CSize sizeCalcLayout = _QueryFixedBarNewSize( __FSQ_DOCKED_H );
	return sizeCalcLayout.cx;
}

CSize CExtPanelControlBar::_CalcDesiredMinOuterSize( BOOL bHorz ) const
{
	ASSERT_VALID( this );

CSize sizeCalcLayout =
		_QueryFixedBarNewSize(
			bHorz ? __FSQ_DOCKED_H : __FSQ_DOCKED_V
			);
	return sizeCalcLayout;
}

INT CExtPanelControlBar::_CalcDesiredMinVH() const
{
	ASSERT_VALID( this );

CSize sizeCalcLayout = _QueryFixedBarNewSize( __FSQ_DOCKED_V );
	return sizeCalcLayout.cy;
}

CSize CExtPanelControlBar::_CalcDesiredMinFloatedSize() const
{
	ASSERT_VALID( this );

CSize sizeCalcLayout = _QueryFixedBarNewSize( __FSQ_FLOATING );
	return sizeCalcLayout;
}

CSize CExtPanelControlBar::_QueryFixedBarNewSize(
	CExtPanelControlBar::e_fixed_bar_size_query_t eWhichSizeNeedToKnow
	) const
{
CSize sizeResult( OnQueryFixedBarNewSize(eWhichSizeNeedToKnow) );
//	if( eWhichSizeNeedToKnow != __FSQ_FLOATING )
//	{
//		CRect rcClient, rcWnd;
//		GetClientRect( &rcClient );
//		GetWindowRect( &rcWnd );
//		CSize sizeClient( rcClient.Size() );
//		CSize sizeWnd( rcWnd.Size() );
//		sizeResult += sizeWnd;
//		sizeResult -= sizeClient;
//	}
	return sizeResult;
}

CSize CExtPanelControlBar::OnQueryFixedBarNewSize(
	CExtPanelControlBar::e_fixed_bar_size_query_t eWhichSizeNeedToKnow
	) const
{
	ASSERT_VALID( this );
	eWhichSizeNeedToKnow;

//CSize sizeResult;
//	switch( eWhichSizeNeedToKnow )
//	{
//	case __FSQ_DOCKED_H:
//		sizeResult = CSize( 20, 20 );
//	break;
//	case __FSQ_DOCKED_V:
//		sizeResult = CSize( 20, 20 );
//	break;
//	default:
//		ASSERT( eWhichSizeNeedToKnow == __FSQ_FLOATING );
//		sizeResult = CSize( 20, 20 );
//	break;
//	}
//	return sizeResult;

CSize sizeResult( _GetChildSize() );
CRect rcMargins = OnQueryFixedBarChildWindowMargins();
	sizeResult.cx += rcMargins.left + rcMargins.right;
	sizeResult.cy += rcMargins.top + rcMargins.bottom;
	return sizeResult;
}

CRect CExtPanelControlBar::OnQueryFixedBarChildWindowMargins() const
{
	ASSERT_VALID( this );

	return m_rcMargins;
}

CSize CExtPanelControlBar::_GetChildSize() const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return CSize( 0, 0 );
CWnd * pWndChild = GetWindow( GW_CHILD );
	if( pWndChild->GetSafeHwnd() == NULL )
		return CSize( 0, 0 );
CRect rcChild;
	pWndChild->GetWindowRect( &rcChild );
CSize sizeChild( rcChild.Size() );
	return sizeChild;
}

void CExtPanelControlBar::_RepositionChild(
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );

CWnd * pWndChild = GetWindow( GW_CHILD );
	if( pWndChild == NULL )
		return;
CRect rcMargins = OnQueryFixedBarChildWindowMargins();
CRect rcRequired, rcReal;
	GetClientRect( &rcRequired );
	rcRequired.DeflateRect( &rcMargins );
	pWndChild->GetWindowRect( &rcReal );
	ScreenToClient( &rcReal );
//	if( rcReal == rcRequired )
	if( rcReal.TopLeft() == rcRequired.TopLeft() )
		return;
	rcReal.OffsetRect( - rcReal.TopLeft() + rcRequired.TopLeft() );
	pWndChild->MoveWindow(
		&rcReal, // &rcRequired,
		bRedraw ? TRUE : FALSE
		);
}

void CExtPanelControlBar::OnSize(UINT nType, int cx, int cy) 
{
	ASSERT_VALID( this );

	CExtToolControlBar::OnSize(nType, cx, cy);
	_RepositionChild();	
}

void CExtPanelControlBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	ASSERT_VALID( this );

	CExtToolControlBar::OnWindowPosChanged(lpwndpos);
	_RepositionChild();
}

void CExtPanelControlBar::OnSetFocus(CWnd* pOldWnd) 
{
	ASSERT_VALID( this );

	CExtToolControlBar::OnSetFocus(pOldWnd);
CWnd * pWndChild = GetWindow( GW_CHILD );
	if( pWndChild == NULL )
		return;	
	pWndChild->SetFocus();
}

void CExtPanelControlBar::OnPaint() 
{
	ASSERT_VALID( this );

CPaintDC dcPaint( this );
CExtMemoryDC dc( &dcPaint );

CRect rcClient;
	GetClientRect( &rcClient );

CWnd * pWndChild = GetWindow( GW_CHILD );
	if( pWndChild == NULL )
	{ // if no child - fill entire client area
		if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
			||	(! PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			)
			dc.FillSolidRect(
				&rcClient,
				PmBridge_GetPM()->GetColor(
					CExtPaintManager::CLR_3DFACE_OUT, this
					)
				);
	} // if no child - fill entire client area
	else
	{ // if has child - fill margins around child
//		CRect rcMargins = OnQueryFixedBarChildWindowMargins();
//		CRect rcExclude( rcClient );
//		rcExclude.DeflateRect( &rcMargins );
//		dc.ExcludeClipRect( &rcExclude );
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			m_hWnd
			);
		if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
			||	(! PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			)
			dc.FillSolidRect(
				&rcClient,
				PmBridge_GetPM()->GetColor(
					CExtPaintManager::CLR_3DFACE_OUT, this
					)
				);
		dc.SelectClipRgn( NULL );
	} // if has child - fill margins around child
}

bool CExtPanelControlBar::_AffixmentIsAlignedHW() const
{
	ASSERT_VALID( this );
	return false;
}

LRESULT CExtPanelControlBar::DoHelpHitTest( CPoint ptHelpHitTest )
{
	ASSERT_VALID( this );
	return CExtControlBar::DoHelpHitTest( ptHelpHitTest );
}

bool CExtPanelControlBar::OnQueryMultiRowLayout() const
{
	ASSERT_VALID( this );
	return false;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtCustomizeSite * CExtPanelControlBar::GetCustomizeSite()
{
	ASSERT_VALID( this );
	return NULL;
}

bool CExtPanelControlBar::OnCustomizeRegisterBar(
	CExtCustomizeSite * pCustomizeSite
	)
{
	ASSERT_VALID( this );
	ASSERT( pCustomizeSite != NULL );
	pCustomizeSite;
	return false;
}

void CExtPanelControlBar::OnCustomizeModeEnter()
{
	ASSERT_VALID( this );
}

void CExtPanelControlBar::OnCustomizeModeLeave()
{
	ASSERT_VALID( this );
}

void CExtPanelControlBar::OnCustomizeListBoxUpdate( CExtCustomizeToolbarsListBox * pWndList )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWndList );
	CExtToolControlBar::OnCustomizeListBoxUpdate( pWndList );
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP

/////////////////////////////////////////////////////////////////////////////
// CExtBarUndoRedoButton

IMPLEMENT_DYNCREATE(CExtBarUndoRedoButton, CExtBarButton)

CExtBarUndoRedoButton::CExtBarUndoRedoButton(
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	)
	: CExtBarButton( pBar, nCmdID, nStyle )
	, m_lParamCookie( 0L )
	, m_dwListBoxStyles( WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_MULTIPLESEL|LBS_NOINTEGRALHEIGHT|LBS_HASSTRINGS|LBS_OWNERDRAWVARIABLE )
	, m_nDropDownWidth( -1 ) // (-1) - auto calc
	, m_nDropDownHeightMax( 250 )
{
}

CExtBarUndoRedoButton::~CExtBarUndoRedoButton()
{
}

LPARAM CExtBarUndoRedoButton::LParamCookieGet() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return pNode->GetLParam();
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_lParamCookie;
}

void CExtBarUndoRedoButton::LParamCookieSet(
	LPARAM lParamCookie // = 0L
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarButton*)this)->GetCmdNode(false);
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->SetLParam( lParamCookie );
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_lParamCookie = lParamCookie;
}

bool CExtBarUndoRedoButton::CanBePressedInDisabledState()
{
	return false;
}

bool CExtBarUndoRedoButton::IsAbleToTrackMenu(
	bool bCustomizeMode // = false
	) const
{
	ASSERT_VALID( this );
	if( bCustomizeMode )
		return false;
	return true;
}

LRESULT CExtBarUndoRedoButton::OnHelpHitTest(
	CPoint point
	)
{
	ASSERT_VALID( this );
//	return
//		CExtBarButton::OnHelpHitTest( point );
	point;
	return GetCmdID();
}

CExtPopupMenuWnd * CExtBarUndoRedoButton::OnCreateDropPopup(
	bool bContentExpand // true - inserting to content expand menu, false - tracking dropped popup
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	bContentExpand;

	CWnd * pWndCmdTarget = GetCmdTargetWnd();
	ASSERT_VALID( pWndCmdTarget );

CExtPopupUndoRedoMenuWnd * pPopup =
		STATIC_DOWNCAST(
			CExtPopupUndoRedoMenuWnd,
			CExtPopupMenuWnd::InstantiatePopupMenu(
				GetBar()->GetSafeHwnd(),
				RUNTIME_CLASS(CExtPopupUndoRedoMenuWnd),
				this
				)
			);
	pPopup->m_lParamCookie = LParamCookieGet();

	pPopup->m_pCbListBoxSelection = 
		(CExtPopupInplaceListBox::pCbListBoxSelection)
			stat_CbListBoxSelection;
	pPopup->m_pSelectionCookie = (LPVOID)this;
	
	pPopup->m_pCbInitListBoxContent =
		(CExtPopupInplaceListBox::pCbInitListBoxContent)
			stat_CbInitListBoxContent;
	pPopup->m_pInitListBoxCookie = (LPVOID)this;

	pPopup->m_pCbListBoxItemDraw =
		(CExtPopupInplaceListBox::pCbListBoxItemDraw)
			stat_CbListBoxItemDraw;
	pPopup->m_pCbListBoxItemMeasure =
		(CExtPopupInplaceListBox::pCbListBoxItemMeasure)
			stat_CbListBoxItemMeasure;
	pPopup->m_pListBoxItemCookie = (LPVOID)this;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite == NULL
		||	(!	pSite->OnPopupListBoxGetStyles(
					this,
					GetCmdNode( false ),
					pPopup->m_dwListBoxStyles
					)
			)
		)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	pPopup->m_dwListBoxStyles = m_dwListBoxStyles;

	pPopup->m_pCbFormatCaption =
		(CExtPopupUndoRedoMenuWnd::pCbFormatCaption)
			stat_CbFormatCaption;
	pPopup->m_pFormatCaptionCookie = (LPVOID)this;

CSize sizeMeasure = OnPopupListBoxMeasureTrackSize();
	pPopup->m_sizeChildControl = sizeMeasure;
	
	VERIFY( 
		pPopup->CreatePopupMenu( pWndCmdTarget->GetSafeHwnd() ) 
		);
	
	return pPopup;
}

UINT CExtBarUndoRedoButton::OnTrackPopup(
	CPoint point,
	bool bSelectAny,
	bool bForceNoAnimation
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	ASSERT( CtrlGet() == NULL );
	ASSERT( IsAbleToTrackMenu() );

bool bDockSiteCustomizeMode =
		pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return UINT(-1L);

	if( !IsEnabled() )
		return UINT(-1L);

bool bPrevTBMT = CExtToolControlBar::g_bMenuTracking;
	if( CExtToolControlBar::g_bMenuTracking
		//&& CExtPopupMenuWnd::IsMenuTracking()
		&& pBar->_GetIndexOf(this) ==
			pBar->GetMenuTrackingButton()
		)
		return UINT(-1L);

	CExtToolControlBar::_CloseTrackingMenus();

	if( pBar->IsFloating() )
	{
		pBar->ActivateTopParent();
		CFrameWnd * pFrame =
			pBar->GetDockingFrame();
		ASSERT_VALID( pFrame );
		pFrame->BringWindowToTop();
	}

CExtPopupMenuWnd * pPopup = OnCreateDropPopup( false );
	if( pPopup == NULL )
	{
		ASSERT( FALSE );
		return UINT(-1L);
	}
	ASSERT_VALID( pPopup );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	pPopup->_CmdNodeSet( GetCmdNode(false) );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	CExtToolControlBar::g_bMenuTracking = bPrevTBMT;

CRect rcBtn = Rect();
	pBar->ClientToScreen( &rcBtn );
	pBar->ClientToScreen( &point );

DWORD dwTrackFlags =
		OnGetTrackPopupFlags()
			| TPMX_OWNERDRAW_FIXED
			;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( pBar->IsKindOf( RUNTIME_CLASS( CExtRibbonPage ) ) )
		dwTrackFlags |= TPMX_COMBINE_NONE|TPMX_RIBBON_MODE|TPMX_NO_HIDE_RARELY;
	else
		dwTrackFlags |= TPMX_COMBINE_DEFAULT;
#else // (!defined __EXT_MFC_NO_RIBBON_BAR)
		dwTrackFlags |= TPMX_COMBINE_DEFAULT;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	if( bSelectAny )
		dwTrackFlags |= TPMX_SELECT_ANY;
	if( CExtToolControlBar::g_bMenuTrackingExpanded )
		dwTrackFlags |= TPMX_NO_HIDE_RARELY;
	if( bForceNoAnimation )
		dwTrackFlags |= TPMX_FORCE_NO_ANIMATION;

	pBar->_SwitchMenuTrackingIndex(
		pBar->_GetIndexOf( this )
		);

UINT nResultCmdID = (UINT)-1L;
	pPopup->m_hWndNotifyMenuClosed = pBar->GetSafeHwnd();
	if( GetSeparatedDropDown() )
		m_bDropDownHT = true;
	g_pTrackingMenuTBB = this;
	if( ! pPopup->TrackPopupMenu(
			dwTrackFlags,
			point.x,
			point.y,
			&rcBtn,
			GetBar(),
			CExtToolControlBar::_CbPaintCombinedContent,
			&nResultCmdID,
			true
			)
		)
	{
		g_pTrackingMenuTBB = NULL;
		//delete pPopup;
		CExtToolControlBar::_CloseTrackingMenus();
		return UINT(-1L);
	}
	CExtToolControlBar::g_bMenuTracking = true;
	pBar->_SwitchMenuTrackingIndex(
		pBar->_GetIndexOf( this )
		);

	return UINT(-1L);
}

BOOL CExtBarUndoRedoButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( !pPopup->ItemInsert( GetCmdID(true) ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
CExtPopupMenuWnd::MENUITEMDATA & mi = 
		pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
	ASSERT_VALID( pBar->m_pDockSite );
	VERIFY(
		mi.UpdateFromCmdTree(
			pBar->m_pDockSite->m_hWnd,
			GetCmdNode(false),
			GetCmdNode(false)->GetOwnIndex(),
			pPopup
			)
		);
	mi.SetCmdNode( GetCmdNode() );
	mi.SetChildCombine();

#else

	CExtPopupMenuWnd * pSub = OnCreateDropPopup( true );
	if( pSub == NULL )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT_VALID( pSub );
	CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( m_pBar->GetSafeHwnd() ),
			GetCmdID( false )
			);
	ASSERT( pCmdItem != NULL );
	CExtSafeString sText = pCmdItem->m_sMenuText;
	if( sText.IsEmpty() )
		sText = pCmdItem->m_sToolbarText;
// added in 2.55
	CExtCmdIcon _emptyIcon, * pCmdIcon = GetIconPtr();
// commented in 2.55
// CExtCmdIcon _emptyIcon, * pCmdIcon =
// 		g_CmdManager->CmdGetIconPtr(
// 			g_CmdManager->ProfileNameFromWnd( m_pBar->GetSafeHwnd() ),
// 			GetCmdID( false )
// 			);
CExtCmdIcon & _cmdIcon =
		( pCmdIcon != NULL )
			? (*pCmdIcon)
			: _emptyIcon
			;
	if( ! pPopup->ItemInsertSpecPopup( pSub, -1, sText, _cmdIcon ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	CExtPopupMenuWnd::MENUITEMDATA & mi =
		pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
	
	mi.Enable( 
		IsEnabled() ? true : false 
		);

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return TRUE;
}

bool CExtBarUndoRedoButton::stat_CbFormatCaption(
	CExtSafeString & strCaption,
	CExtPopupUndoRedoMenuWnd * pUndoRedoPopupMenuWnd,
	CExtBarUndoRedoButton * pUndoRedoTBB
	)
{
	ASSERT_VALID( pUndoRedoTBB );
	ASSERT_KINDOF( CExtBarUndoRedoButton, pUndoRedoTBB );
	return
		pUndoRedoTBB->OnPopupUndoRedoFormatCaption(
			strCaption,
			pUndoRedoPopupMenuWnd
			);
}

bool CExtBarUndoRedoButton::stat_CbInitListBoxContent(
	CExtPopupInplaceListBox & wndListBox,
	CExtBarUndoRedoButton * pUndoRedoTBB
	)
{
	ASSERT_VALID( pUndoRedoTBB );
	ASSERT_KINDOF( CExtBarUndoRedoButton, pUndoRedoTBB );
	return
		pUndoRedoTBB->OnPopupListBoxInitContent(
			wndListBox
			);
}

bool CExtBarUndoRedoButton::stat_CbListBoxSelection(
	CExtPopupInplaceListBox & wndListBox,
	CExtBarUndoRedoButton * pUndoRedoTBB,
	int eSAT // CExtPopupInplaceListBox::e_sel_action_t
	)
{
	ASSERT_VALID( pUndoRedoTBB );
	ASSERT_KINDOF( CExtBarUndoRedoButton, pUndoRedoTBB );
	switch( ((CExtPopupInplaceListBox::e_sel_action_t)eSAT) )
	{
	case CExtPopupInplaceListBox::__SAT_SELCHANGE:
		return
			pUndoRedoTBB->OnPopupListBoxSelChange(
				wndListBox
				);
	case CExtPopupInplaceListBox::__SAT_SELENDOK:
		return
			pUndoRedoTBB->OnPopupListBoxSelEndOK(
				wndListBox
				);
	case CExtPopupInplaceListBox::__SAT_SELENDCANCEL:
		return
			pUndoRedoTBB->OnPopupListBoxSelEndCancel(
				wndListBox
				);
	case CExtPopupInplaceListBox::__SAT_CLOSE:
		return
			pUndoRedoTBB->OnPopupListBoxClose(
				wndListBox
				);
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( ((CExtPopupInplaceListBox::e_sel_action_t)eSAT) )
	return false;
}

bool CExtBarUndoRedoButton::stat_CbListBoxItemDraw(
	CExtPopupInplaceListBox & wndListBox,
	CExtBarUndoRedoButton * pUndoRedoTBB,
	LPDRAWITEMSTRUCT pDIS
	)
{
	ASSERT_VALID( pUndoRedoTBB );
	ASSERT_KINDOF( CExtBarUndoRedoButton, pUndoRedoTBB );
	ASSERT( pDIS != NULL && pDIS->hDC != NULL );
	return
		pUndoRedoTBB->OnPopupListBoxItemDraw(
			wndListBox,
			pDIS
			);
}

bool CExtBarUndoRedoButton::stat_CbListBoxItemMeasure(
	CExtPopupInplaceListBox & wndListBox,
	CExtBarUndoRedoButton * pUndoRedoTBB,
	LPMEASUREITEMSTRUCT pMIS
	)
{
	ASSERT_VALID( pUndoRedoTBB );
	ASSERT_KINDOF( CExtBarUndoRedoButton, pUndoRedoTBB );
	ASSERT( pMIS != NULL );
	return
		pUndoRedoTBB->OnPopupListBoxItemMeasure(
			wndListBox,
			pMIS
			);
}

CSize CExtBarUndoRedoButton::OnPopupListBoxMeasureTrackSize() const
{
	ASSERT_VALID( this );
const CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
	{
		CSize _size =
			pSite->OnPopupListBoxMeasureTrackSize(
				this,
				((CExtBarButton*)this)->GetCmdNode( false )
				);
		if( _size.cx > 0 && _size.cy > 0 )
			return _size;
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

int nCount = (int)m_arrLbItems.GetSize();
	if(		(m_dwListBoxStyles & LBS_HASSTRINGS) == 0
		||	nCount == 0
		)
	{
		CSize sizeCalc( 150, 100 );
		if( m_nDropDownWidth > 0 )
			sizeCalc.cx = m_nDropDownWidth;
		if( m_nDropDownHeightMax > 0 )
			sizeCalc.cy = m_nDropDownHeightMax;
//		if( m_bComboField )
//			sizeCalc.cx +=
//				//::GetSystemMetrics( SM_CXSIZEFRAME )
//				+ GetBar()->PmBridge_GetPM()->GetDropDownButtonWidth( this )
//				;
		return sizeCalc;
	}

CWindowDC dc( NULL );
CSize sizeCalc( 0, 0 );
	for( int i = 0; i < nCount; i++ )
	{
		const CExtSafeString & sItem =
			((CExtSafeStringArray*)&m_arrLbItems)->ElementAt( i );
		CExtSafeString sMeasure(
			sItem.IsEmpty() ? _T("AaWwPpQqRrWwZz") : sItem
			);
		CRect rcMeasure =
			CExtPaintManager::stat_CalcTextDimension(
				dc,
				pBar->PmBridge_GetPM()->m_FontNormal,
				sMeasure
				);
		if( m_dwListBoxStyles & (LBS_OWNERDRAWVARIABLE|LBS_OWNERDRAWFIXED) )
			rcMeasure.InflateRect( OnPopupListBoxCalcItemExtraSizes() );
		CSize sizeMeasure = rcMeasure.Size();
		if( m_nDropDownWidth == -1 )
			sizeCalc.cx = max( sizeCalc.cx, sizeMeasure.cx );
		sizeCalc.cy += sizeMeasure.cy;
	} // for( int i = 0; i < nCount; i++ )

	if( m_nDropDownWidth == -1 )
		sizeCalc.cx += ::GetSystemMetrics( SM_CXSIZEFRAME )*2;
	sizeCalc.cy += 2;
	ASSERT( m_nDropDownHeightMax > 0 );
INT nDD = GetBar()->PmBridge_GetPM()->GetDropDownButtonWidth( (CObject*)this );
	if( sizeCalc.cy > m_nDropDownHeightMax )
	{
		sizeCalc.cy = m_nDropDownHeightMax;
		if( m_nDropDownWidth == -1 )
			sizeCalc.cx +=
				::GetSystemMetrics( SM_CXSIZEFRAME )
				+ nDD
				;
	} // if( sizeCalc.cy > m_nDropDownHeightMax )
	else if( m_dwListBoxStyles & LBS_DISABLENOSCROLL )
	{
		if( m_nDropDownWidth == -1 )
			sizeCalc.cx +=
				::GetSystemMetrics( SM_CXSIZEFRAME )
				+ nDD
				;
	} // else if( m_dwListBoxStyles & LBS_DISABLENOSCROLL )

	if( m_nDropDownWidth > 0 )
		sizeCalc.cx = m_nDropDownWidth;

	return sizeCalc;
}

bool CExtBarUndoRedoButton::OnPopupListBoxInitContent(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxInitContent(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) != 0 )
	{
		int nCount = (int)m_arrLbItems.GetSize();
		for( int i = 0; i < nCount; i++ )
		{
			const CExtSafeString & sItem = m_arrLbItems.ElementAt( i );
			wndListBox.AddString(
				sItem.IsEmpty() ? _T("") : sItem
				);
		} // for( int i = 0; i < nCount; i++ )
		if( nCount > 0 )
			wndListBox.SetSel( 0, TRUE );
	} // if( (wndListBox.GetStyle() & LBS_HASSTRINGS) != 0 )

	return true;
}

bool CExtBarUndoRedoButton::OnPopupListBoxSelChange(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	wndListBox;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxSelChange(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return false;
}

bool CExtBarUndoRedoButton::OnPopupListBoxSelEndOK(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	wndListBox;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
HWND hWndThis = wndListBox.m_hWnd;
	ASSERT( hWndThis != NULL && ::IsWindow(hWndThis) );
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxSelEndOK(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
	if( ! ::IsWindow( hWndThis ) )
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return false;
}

bool CExtBarUndoRedoButton::OnPopupListBoxSelEndCancel(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	wndListBox;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxSelEndCancel(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return false;
}

bool CExtBarUndoRedoButton::OnPopupListBoxClose(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	wndListBox;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxClose(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return false;
}

bool CExtBarUndoRedoButton::OnPopupListBoxItemDraw(
	CExtPopupInplaceListBox & wndListBox,
	LPDRAWITEMSTRUCT pDIS
	)
{
	ASSERT_VALID( this );
	ASSERT( pDIS != NULL && pDIS->hDC != NULL );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxItemDraw(
				this,
				GetCmdNode( false ),
				wndListBox,
				pDIS
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) == 0 )
		return false;
	if( ((INT)pDIS->itemID) < 0 || ((INT)pDIS->itemID) >= wndListBox.GetCount() )
		return true;
CDC dc;
	dc.Attach( pDIS->hDC );

CExtToolControlBar * pBar = GetBar();
	if( pDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT) )
	{
		CRect rcErase( pDIS->rcItem );
		if( INT(pDIS->itemID) == (wndListBox.GetCount()-1) )
		{
			CRect rcClient;
			wndListBox.GetClientRect( &rcClient );
			if( rcErase.bottom < rcClient.bottom )
				rcErase.bottom = rcClient.bottom;
		}
		dc.FillSolidRect( 
			rcErase, 
			pBar->PmBridge_GetPM()->GetColor( COLOR_WINDOW )
			);
		dc.FillSolidRect(
			&pDIS->rcItem,
			pBar->PmBridge_GetPM()->GetColor(
				( pDIS->itemState & ODS_SELECTED )
					? COLOR_HIGHLIGHT
					: COLOR_WINDOW
					,
				this
				)
			);
	}

	if( INT(pDIS->itemID) >= 0 )
	{
		CString sText;
		wndListBox.GetText( (INT)pDIS->itemID, sText );
		if( !sText.IsEmpty() )
		{
			COLORREF clrTextOld =
				dc.SetTextColor(
					pBar->PmBridge_GetPM()->GetColor(
						( pDIS->itemState & ODS_SELECTED )
							? COLOR_HIGHLIGHTTEXT
							: COLOR_BTNTEXT
							,
						this
						)
					);
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CRect rcText( pDIS->rcItem );
			rcText.DeflateRect( OnPopupListBoxCalcItemExtraSizes() );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(sText),
				&rcText,
				DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX, 0
				);
			dc.SetBkMode( nOldBkMode );
			dc.SetTextColor( clrTextOld );
		} // if( !sText.IsEmpty() )
	} // if( INT(pDIS->itemID) >= 0 )

	dc.Detach();
	return true;
}

bool CExtBarUndoRedoButton::OnPopupListBoxItemMeasure(
	CExtPopupInplaceListBox & wndListBox,
	LPMEASUREITEMSTRUCT pMIS
	)
{
	ASSERT_VALID( this );
	ASSERT( pMIS != NULL );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxItemMeasure(
				this,
				GetCmdNode( false ),
				wndListBox,
				pMIS
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) == 0 )
		return false;
	pMIS->itemWidth = pMIS->itemHeight = 10;
	if( ((INT)pMIS->itemID) < 0 || ((INT)pMIS->itemID) >= wndListBox.GetCount() )
		return true;
CExtSafeString sMeasure;
	wndListBox.GetText( (INT)pMIS->itemID, *((CString*)&sMeasure) );
	if( sMeasure.IsEmpty() )
		return true;
CExtToolControlBar * pBar = GetBar();
CWindowDC dc( NULL );
CRect rcMeasure =
		CExtPaintManager::stat_CalcTextDimension(
			dc,
			pBar->PmBridge_GetPM()->m_FontNormal,
			sMeasure
			);
	rcMeasure.InflateRect( OnPopupListBoxCalcItemExtraSizes() );
	pMIS->itemWidth = rcMeasure.Width();
	pMIS->itemHeight = rcMeasure.Height();
	return true;
}

CSize CExtBarUndoRedoButton::OnPopupListBoxCalcItemExtraSizes() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
	{
		CSize _size =
			pSite->OnPopupListBoxCalcItemExtraSizes(
				this,
				((CExtBarButton*)this)->GetCmdNode( false )
				);
		if( _size.cx > 0 && _size.cy > 0 ) 
			return _size;
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return CSize( 2, 1 );
}

bool CExtBarUndoRedoButton::OnPopupUndoRedoFormatCaption(
	CExtSafeString & strCaption,
	CExtPopupUndoRedoMenuWnd * pUndoRedoPopupMenuWnd
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	strCaption;
	pUndoRedoPopupMenuWnd;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupUndoRedoFormatCaption(
				this,
				GetCmdNode( false ),
				pUndoRedoPopupMenuWnd, 
				strCaption
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return false;
}

#endif // __EXT_MFC_NO_UNDO_REDO_POPUP


