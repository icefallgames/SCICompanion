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

#if (!defined __EXT_TOOLCONTROLBAR_H)
#define __EXT_TOOLCONTROLBAR_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_CONTROLBAR_H)
	#include <ExtControlBar.h>
#endif

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif // __EXT_CMD_ICON_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CExtBarButton;
class CExtToolControlBar;
class CExtMenuControlBar;

class CExtCmdItem;
class CExtPopupInplaceListBox;
#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP
	class CExtPopupUndoRedoMenuWnd;
#endif // __EXT_MFC_NO_UNDO_REDO_POPUP

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

	class CExtCustomizeCmdKeyTip;
	class CExtPopupKeyTipWnd;

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////

#define __EXT_MFC_IMPLEMENT_TT_REDIR_OnToolHitTest_EX( __FROM__, __TO__, __TT_MEMBER__, __POINT_PARM__, __TOOLINFO_PTR__, __DEPTH__ ) \
	if( __TOOLINFO_PTR__ != NULL && GetSafeHwnd() != NULL ) \
	{ \
		HWND __local_hWndParent__ = ::GetParent( m_hWnd ); \
		int __local_nParentDepth__ = 1; \
		for( ; \
				( __local_hWndParent__ != NULL ) \
			&&	( __local_nParentDepth__ < __DEPTH__ ); \
			__local_nParentDepth__ ++ \
			) \
			__local_hWndParent__ = ::GetParent( __local_hWndParent__ ); \
		if( __local_hWndParent__ != NULL ) \
		{ \
			CWnd * __local_pWndPermanent__ = \
				CWnd::FromHandlePermanent( \
					__local_hWndParent__ \
					); \
			if( __local_pWndPermanent__ != NULL ) \
			{ \
				__TO__ * __local_pTypedParentWnd__ = \
					DYNAMIC_DOWNCAST( \
						__TO__, \
						__local_pWndPermanent__ \
						); \
				if( __local_pTypedParentWnd__ != NULL ) \
				{ \
					CPoint __local_pointForTypedParentWnd__ = __POINT_PARM__ ; \
					ClientToScreen( \
						& __local_pointForTypedParentWnd__ \
						); \
					__local_pTypedParentWnd__ -> ScreenToClient( \
						& __local_pointForTypedParentWnd__ \
						); \
					CRect __local_rcSaved__ = \
						__TOOLINFO_PTR__ -> rect; \
					ClientToScreen( \
						&( __TOOLINFO_PTR__ -> rect ) \
						); \
					__local_pTypedParentWnd__ -> \
						ScreenToClient( \
							&( __TOOLINFO_PTR__ -> rect ) \
							); \
					__TOOLINFO_PTR__->hwnd = \
						__local_pTypedParentWnd__ -> m_hWnd; \
					struct __local_friendy_typed_parent__ \
						: public __TO__ \
					{ \
						friend class __FROM__ ; \
					}; \
					__EXT_MFC_INT_PTR __local_nRetVal__ = \
						(	( __local_friendy_typed_parent__* ) \
								__local_pTypedParentWnd__ \
						) -> \
							OnToolHitTest( \
								__local_pointForTypedParentWnd__, \
								__TOOLINFO_PTR__ \
								); \
					::CopyRect( \
						&( __TOOLINFO_PTR__ -> rect ), \
						& __local_rcSaved__ \
						); \
					__TOOLINFO_PTR__ -> hwnd = m_hWnd; \
					if(		( __TOOLINFO_PTR__ -> lpszText ) != NULL \
						&&	( __TOOLINFO_PTR__ -> lpszText ) != LPSTR_TEXTCALLBACK \
						) \
					{ \
						if(		__local_nRetVal__ != -1 \
							&&	(	  __TT_MEMBER__ .GetSafeHwnd() == NULL \
								||	( __TT_MEMBER__ .GetStyle() & WS_VISIBLE ) == 0 \
								) \
							) \
						{ \
							__FROM__ * pNonConstThis = \
								const_cast < __FROM__ * > ( this ); \
							pNonConstThis -> SetTooltipText( \
								__EXT_MFC_SAFE_LPCTSTR( \
									LPCTSTR( __TOOLINFO_PTR__ -> lpszText ) \
									) \
								); \
							pNonConstThis -> ActivateTooltip(); \
						} \
						::free( __TOOLINFO_PTR__ -> lpszText ); \
						__TOOLINFO_PTR__ -> lpszText = NULL; \
					} \
					return -1; \
				} \
			} \
		} \
	}

#define __EXT_MFC_IMPLEMENT_TT_REDIR_OnToolHitTest( __FROM__, __TO__ ) \
	__EXT_MFC_IMPLEMENT_TT_REDIR_OnToolHitTest_EX( __FROM__, __TO__, m_wndToolTip, point, pTI, 1 )

/////////////////////////////////////////////////////////////////////////////
// CExtBarButton

#if (!defined __EXT_MFC_NO_RIBBON_BAR)

	// ribbon bar's visual informativeness level (simple case like buttons)
	#define __EXT_RIBBON_ILV_SIMPLE_SMALL			0
	#define __EXT_RIBBON_ILV_SIMPLE_NORMAL			1
	#define __EXT_RIBBON_ILV_SIMPLE_LARGE			2
	#define __EXT_RIBBON_ILV_SIMPLE_MIN				0
	#define __EXT_RIBBON_ILV_SIMPLE_MAX				2

	// ribbon bar's effective informativeness level
	#define __EXT_RIBBON_ILE_MIN					0
	#define __EXT_RIBBON_ILE_MAX					30

	#define __EXT_RIBBON_BIT_COUNT_ILE	5
	#define __EXT_RIBBON_BIT_COUNT_ILV	(32-__EXT_RIBBON_BIT_COUNT_ILE-1)
	#define __EXT_RIBBON_BIT_MASK_ILE	((1<<__EXT_RIBBON_BIT_COUNT_ILE)-1)
	#define __EXT_RIBBON_BIT_MASK_ILV	((1<<__EXT_RIBBON_BIT_COUNT_ILV)-1)

	#define __EXT_RIBBON_MAKE_RULE_ARRAY_ENTRY( __ILE__, __ILV__, __TOOL_WRAP__ ) \
		DWORD( \
				( DWORD(__ILE__) & __EXT_RIBBON_BIT_MASK_ILE ) \
			|	( ( DWORD(__ILV__) & __EXT_RIBBON_BIT_MASK_ILV ) << __EXT_RIBBON_BIT_COUNT_ILE ) \
			|	( DWORD( (__TOOL_WRAP__) ? 1 : 0 ) << 15 ) \
			)

	#define __EXT_RIBBON_ILE_FROM_ENTRY( __ENTRY__ ) \
		DWORD( DWORD(__ENTRY__) & __EXT_RIBBON_BIT_MASK_ILE )

	#define __EXT_RIBBON_ILV_FROM_ENTRY( __ENTRY__ ) \
		DWORD( (DWORD(__ENTRY__) >> __EXT_RIBBON_BIT_COUNT_ILE ) & __EXT_RIBBON_BIT_MASK_ILV )

	#define __EXT_RIBBON_TOOL_WRAP_FROM_ENTRY( __ENTRY__ ) \
		( ( ( (DWORD(__ENTRY__) >> 15 ) & 0x01 ) != 0 ) ? true : false )

	class CExtRibbonNode;

#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

class __PROF_UIS_API CExtBarButton
	: public CObject
	, public CExtAnimationClient
{
public:
	DECLARE_DYNCREATE( CExtBarButton );

protected:
	CExtToolControlBar * m_pBar;
	CExtBarButton * m_pParentButton;
	CArray < CExtBarButton *, CExtBarButton * > m_arrChildren;
	UINT	m_nButtonCmdID; // Command ID or ID_SEPARATOR
	UINT	m_nEffectiveCmdID;
	UINT	m_nStyle; // button styles
	
	CSize	m_ActiveSize;
	CRect	m_ActiveRect;
	
	bool	m_bWrapH:1, m_bWrapV:1, m_bWrapF:1,
			m_bVisible:1,
			m_bHover:1,
			m_bCtrlAutoDestroyed:1,
			m_bVertDocked:1,
			m_bPopupMenu:1,
			m_bAutoDestroyMenu:1,
			m_bAppendMdiWindowsMenu:1,
			m_bSeparatedDropDown:1,
			m_bAutoChangeID:1,
			m_bDropDownHT:1,
			m_bNoRotateVerticalLayout:1,
			m_bCtrlVisibleVert:1,
			m_bPressedTracking:1,
			m_bDrawBorder:1;

	INT m_nIconAlignment;
	INT m_nTextAlignment;

	CRect m_rcIconMargins;
	CRect m_rcTextMargins;
	
	INT m_nMaxButtonWidth;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtCustomizeCmdTreeNode * m_pCmdNodeI, * m_pCmdNodeC;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	CWnd *	m_pCtrl;
	HMENU m_hMenu;

	virtual bool _UpdateCtrlIsEnabled();
	virtual void _UpdateCtrl();
	void _UpdateCmdIdFromStyle()
	{
		if( m_nStyle & TBBS_SEPARATOR )
		{
			m_nStyle &= ~(TBBS_BUTTON);
			m_nButtonCmdID = m_nEffectiveCmdID = ID_SEPARATOR;
		}
		else
		{
			m_nStyle |= TBBS_BUTTON;
		}
	}

	void _DestroyMenu();

public:
	CExtBarButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	virtual ~CExtBarButton();

	CExtToolControlBar * GetSafeBar();
	const CExtToolControlBar * GetSafeBar() const;
	virtual CExtToolControlBar * GetBar();
	const CExtToolControlBar * GetBar() const;
	virtual void SetBar( CExtToolControlBar * pBar );

	virtual bool IsLargeIcon() const;
	virtual bool IsDisplayScreenTip() const;
	virtual bool IsDisplayShortcutKeysOnScreenTip() const;

	virtual UINT GetCmdID(
		bool bEffective = true
		) const;
	virtual void SetCmdID(
		UINT nCmdID,
		bool bEffectiveOnly = false,
		bool bEffectiveRecalcLayout = false
		);

	virtual bool GetSeparatedDropDown() const;
	virtual void SetSeparatedDropDown(
		bool bSeparatedDropDown = true
		);
	virtual bool GetAutoChangeID() const;
	virtual void SetAutoChangeID(
		bool bAutoChangeID = true
		);
	virtual bool GetNoRotateVerticalLayout() const;
	virtual void SetNoRotateVerticalLayout(
		bool bNoRotateVerticalLayout = true
		);
	virtual bool GetCtrlVisibleVertically() const;
	virtual void SetCtrlVisibleVertically( bool bVisible );

	virtual CWnd * CtrlGet();
	const CWnd * CtrlGet() const;
	virtual void CtrlSet(
		CWnd * pCtrl,
		bool bCtrlAutoDestroyed
		);
	virtual CWnd * CtrlDetach();

	void SetMaxButtonWidth( 
		INT nWidth 
		);
	INT GetMaxButtonWidth() const;

	void SetTextAlignment( 
		INT nAlignment 
		);
	INT GetTextAlignment() const;

	void SetIconAlignment( 
		INT nAlignment 
		);
	INT GetIconAlignment() const;

	void SetTextMargins(
		INT nLeft,
		INT nTop,
		INT nRight,
		INT nBottom
		);
	void SetTextMargins(
		const RECT & rcMargins
		);
	void SetTextMargins(
		LPCRECT lprcMargins
		);
	void SetIconMargins(
		INT nLeft,
		INT nTop,
		INT nRight,
		INT nBottom
		);
	void SetIconMargins(
		const RECT & rcMargins
		);
	void SetIconMargins(
		LPCRECT lprcMargins
		);

	void GetTextMargins(
		INT & nLeft,
		INT & nTop,
		INT & nRight,
		INT & nBottom
		) const;
	CRect GetTextMargins() const;
	void GetIconMargins(
		INT & nLeft,
		INT & nTop,
		INT & nRight,
		INT & nBottom
		) const;
	CRect GetIconMargins() const;

	bool GetDrawBorder() const;
	void SetDrawBorder( 
		bool bEnable = true 
		);

	virtual UINT GetStyle() const;
	virtual void SetStyle( UINT nStyle );
	virtual void ModifyStyle(
		UINT nAdd,
		UINT nRemove = 0
		);
	virtual void OnUpdateParentStyle();

	virtual bool OnInvalidateButton();

	virtual void OnPressedTrackingStart();
	virtual void OnPressedTrackingStop();
	virtual bool IsPressedTracking() const;
	
	virtual void SetVertDocked( bool bVertDocked );

	enum e_wrap_t
	{
		__EVT_HORZ		= 0,
		__EVT_VERT		= 1,
		__EVT_FLOAT		= 2,
		__EVT_MIN_VALUE = 0,
		__EVT_MAX_VALUE = 2,
	};
	virtual bool IsWrap(
		CExtBarButton::e_wrap_t _evt
		) const;
	virtual void SetWrap(
		CExtBarButton::e_wrap_t _evt,
		bool bWrap = true
		);

	virtual bool IsSeparator() const;
	virtual bool IsDisabled() const;
	virtual bool IsShowTipWhenDisabled() const;
	bool IsEnabled() const;
	virtual bool IsIndeterminate() const;
	virtual bool IsPressed() const;

	virtual void Show(
		bool bShow = true
		);
	virtual bool IsVisible() const;

	virtual void SetHover(
		bool bHover = true
		);
	virtual bool IsHover() const;

	virtual CExtSafeString GetText() const;
	
	virtual void SetLayoutRect( CDC &dc, const RECT & rectButton );
	virtual void SetRect( const RECT & rectButton );
	virtual CRect Rect() const;
	virtual CRect RectDropDown() const;
	virtual CRect RectWithoutDropDown() const;
	operator CRect() const
	{
		return Rect();
	}

	virtual CSize Size() const
	{
		return m_ActiveSize;
	}
	virtual void Size( const SIZE & _size )
	{
		m_ActiveSize = _size;
	}
	operator CSize() const
	{
		return m_ActiveSize;
	}

	virtual void SetMenu(
		HMENU hMenu,
		bool bPopupMenu,
		bool bAutoDestroyMenu
		);
	virtual HMENU GetMenu()
	{
		return m_hMenu;
	}
	operator HMENU()
	{
		return GetMenu();
	}
	virtual void SetAppendMdiWindowsMenu(
		bool bAppendMdiWindowsMenu
		)
	{
		m_bAppendMdiWindowsMenu = bAppendMdiWindowsMenu;
	}
	virtual bool IsAppendMdiWindowsMenu()
	{
		return m_bAppendMdiWindowsMenu;
	}

	virtual bool CanBePressedInDisabledState();
	virtual bool IsPaintDropDown(
		bool bCustomizeMode = false
		) const;
	virtual bool IsAbleToTrackMenu(
		bool bCustomizeMode = false
		) const;
	virtual bool IsContainerOfPopupLikeMenu();

	virtual LONG _CalcSplitButtonExtent( bool bHorz ) const;
	virtual CSize CalculateLayout(
		CDC & dc,
		CSize sizePreCalc,
		BOOL bHorz
		);

	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);

	virtual CExtCmdIcon * GetIconPtr();
	const CExtCmdIcon * GetIconPtr() const;

	CExtCmdIcon & GetIcon();
	const CExtCmdIcon & GetIcon() const;

	virtual CWnd * GetCmdTargetWnd();

	virtual const CExtAnimationParameters *
		AnimationClient_OnQueryAnimationParameters(
			INT eAPT // __EAPT_*** animation type
			) const;
	virtual bool AnimationClient_CacheNextState(
		CDC & dc,
		const RECT & rcAcAnimationTarget,
		bool bAnimate,
		INT eAPT // __EAPT_*** animation type
		);
	virtual bool AnimationClient_StatePaint(
		CDC & dc
		);
	virtual void AnimationClient_StateDoStep();

	virtual bool IsHorzBarOrientation() const;
	virtual bool IsPaintAble( CDC & dc ) const;
	virtual void PaintParentChain(
		CDC & dc
		);
	virtual void PaintChildren(
		CDC & dc,
		bool bPaintOneNearestChildrenLevelOnly
		);
	virtual void Paint( // old version version (left for compatibility)
		CDC & dc,
		bool bHorz // unused, same as IsHorzBarOrientation()
		);
	virtual void Paint( // new simplified version (for simple toolbars)
		CDC & dc
		);
	virtual void PaintCompound( // new fully featured version (for compound toolbars and rendering animation parts)
		CDC & dc,
		bool bPaintParentChain,
		bool bPaintChildren,
		bool bPaintOneNearestChildrenLevelOnly
		);
	virtual COLORREF OnQueryCustomAccentEffectForIcon(
		CDC & dc
		);

	virtual bool OnQueryHoverBasedMenuTracking() const;

	virtual CExtPopupMenuWnd * OnCreatePopupMenuWnd();

	virtual UINT OnTrackPopup(
		CPoint point,
		bool bSelectAny,
		bool bForceNoAnimation
		);
	virtual void OnHover(
		CPoint point,
		bool bOn,
		bool & bSuspendTips
		);
	virtual void OnHovering(
		CPoint point,
		bool & bSuspendTips
		);
	static CExtBarButton * g_pTrackingMenuTBB;
	virtual void OnClick(
		CPoint point,
		bool bDown
		);
	virtual bool OnRClick(
		CPoint point,
		bool bDown
		);
	virtual bool OnDblClick(
		CPoint point
		);
	virtual void OnDeliverCmd();
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);
	virtual LRESULT OnHelpHitTest(
		CPoint point
		);
	virtual void OnUpdateCmdUI(
		CWnd * pTarget,
		BOOL bDisableIfNoHndler,
		int nIndex
		);

	virtual bool CanStartCommandDrag();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
protected:
	virtual void _UpdateCmdNodeFromStyle();
public:
	virtual bool IsCustomizeActiveItem();
	virtual void SetCustomizeActiveItem();
	virtual int CalculateDropOverEffect(
		CPoint point,
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		bool bAllowTrackNodePopup = true
		);
	virtual CExtCustomizeCmdTreeNode * GetCmdNode(
		bool bInitial = false
		);
	const CExtCustomizeCmdTreeNode * GetCmdNode(
		bool bInitial = false
		) const;
	virtual void OnCustomizeUpdateProps(
		CExtCustomizeCmdTreeNode * pNode = NULL // update from basic node
		);
	virtual void SetBasicCmdNode(
		CExtCustomizeCmdTreeNode * pNode
		);
	virtual void SetCustomizedCmdNode(
		CExtCustomizeCmdTreeNode * pNode
		);
	virtual CExtCustomizeCmdTreeNode * OnCustomizeNodeInit(
		CExtCustomizeSite * pSite,
		CExtCustomizeCmdTreeNode * pParentNode // toolbar node
		);
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	virtual int GetInitialResizingStateH( // -1 - left side resizing, 1 - right side resizing, 0 - no resizing at specified point (in bar's client coord)
		CPoint point,
		int * p_nWidthMin = NULL,
		int * p_nWidthMax = NULL
		);
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	virtual void RedrawButton(
		bool bUpdateNow = false
		);
	virtual UINT OnGetTrackPopupFlags();
	virtual bool OnSetCursor( CPoint point );

	virtual bool IsChildButton(
		const CExtBarButton * pTestChildTBB,
		bool bTestOneLevelOnly = false
		) const;
	virtual CExtBarButton * ParentButtonGet();
	const CExtBarButton * ParentButtonGet() const;
	virtual void ParentButtonSet(
		CExtBarButton * pParentButton = NULL
		);
	virtual INT ChildButtonGetCount() const;
	virtual CExtBarButton * ChildButtonGetAt( INT nPos );
	const CExtBarButton * ChildButtonGetAt( INT nPos ) const;
	virtual bool ChildButtonSetAt(
		INT nPos,
		CExtBarButton * pTBB
		);
	virtual void ChildButtonInsertAt(
		INT nPos,
		CExtBarButton * pTBB
		);
	void ChildButtonAdd(
		CExtBarButton * pTBB
		);
	bool ChildButtonRemove(
		CExtBarButton * pTBB,
		bool bDestroy = false
		);
	void ChildButtonRemoveAll(
		bool bDestroy = false
		);
	virtual INT ChildButtonRemoveAt(
		INT nPos,
		INT nCountToRemove = 1, // -1 remove up to end
		bool bDestroy = false
		);
	virtual INT ChildButtonGetIndexOf(
		const CExtBarButton * pTBB
		) const;

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	virtual bool IsRibbonPaintingMode() const;
	virtual bool IsNoRibbonLayout() const;
	virtual CExtRibbonNode * Ribbon_GetNode(
		bool bInitial = false
		);
	const CExtRibbonNode * Ribbon_GetNode(
		bool bInitial = false
		) const;
	virtual CSize RibbonILV_CalcSize(
		CDC & dc,
		INT nILV = -1 // -1 use current visual level
		) const;
	bool RibbonWrapFromILE( INT nILE ) const;
	virtual INT RibbonILV_fromILE(
		INT nILE,
		bool * p_bIsWrap = NULL
		) const;
	virtual INT RibbonILV_Get(
		INT nType = 0 // -1 min, 0 current, 1 - max
		) const;
	virtual void RibbonILV_Set(
		INT nILV,
		INT nType = 0 // -1 min, 0 current, 1 - max
		);
	virtual INT RibbonILE_Get() const;
	virtual bool RibbonILE_Set( INT nILE ); // returns flag indicating whether ILV changed
	virtual INT RibbonILE_GetCollapsed() const;
	virtual bool RibbonILE_SetCollapsed( INT nILE ); // returns flag indicating whether collapsed state changed
	virtual CRect OnRibbonGetContentPadding() const;
	virtual void OnRibbonAlignContent( CDC & dc );
	virtual INT OnRibbonGetSeparatorExtent( bool bHorz );
	virtual void OnRibbonCalcMinMaxILE( INT & nMinILE, INT & nMaxILE );
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
protected:
	CExtPopupKeyTipWnd * m_pWndKeyTip;
public:
	virtual CExtCustomizeCmdKeyTip * OnKeyTipGetInfo();
	virtual CExtSafeString OnKeyTipGetText(
		INT nIndent
		);
	virtual CWnd * OnKeyTipGetParentWnd();
	virtual CPoint OnKeyTipGetGuideLines();
 	virtual CExtPopupKeyTipWnd * OnKeyTipGetWnd();
	virtual void OnKeyTipTrackingQuery(
		bool bShow,
		CExtCustomizeCmdKeyTip * pKeyTipChain,
		HDWP & hPassiveModeDWP
		);
	virtual void OnKeyTipTrackingQueryNested(
		bool bShow,
		CExtCustomizeCmdKeyTip * pKeyTipChain,
		bool bApplyToThisItem,
		HDWP & hPassiveModeDWP
		);
	virtual bool OnKeyTipTranslate(
		DWORD dwKeyCode,
		CExtCustomizeCmdKeyTip * pKeyTipChain,
		bool bAutoInvokeAction,
		bool & bActionInvoked,
		bool & bContinueKeyTipMode
		);
	virtual void OnKeyTipDisplay(
		CExtCustomizeCmdKeyTip & keyTipChain
		);
	virtual bool OnKeyTipInvokeAction(
		bool & bContinueKeyTipMode
		);
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)

	virtual void OnFlatTrackingStart(
		HDWP & hPassiveModeDWP
		);
	virtual void OnFlatTrackingStop();

	virtual bool OnQueryFlatTrackingEnabled() const;
	virtual CRect OnQueryFlatWalkingCalcRect() const;

	virtual CRect OnQueryBorderSizes(
		bool bHorz = true
		) const;
	virtual INT OnQueryAlignmentText(
		bool bHorz = true
		) const;
	virtual INT OnQueryAlignmentIcon(
		bool bHorz = true
		) const;
	virtual CRect OnQueryMarginsText(
		bool bHorz = true
		) const;
	virtual CRect OnQueryMarginsIcon(
		bool bHorz = true
		) const;
	virtual INT OnQueryMaxButtonWidth(
		bool bHorz = true
		) const;

	friend class CExtToolControlBar;
}; // class CExtBarButton

/////////////////////////////////////////////////////////////////////////////
// CExtBarColorButton

class __PROF_UIS_API CExtBarColorButton : public CExtBarButton
{
	COLORREF m_clrDefault, m_clrSelected;
	LPARAM m_lParamCookie;
	bool m_bEnableBtnColorDefault:1, m_bEnableBtnColorCustom:1;
	CExtSafeString m_sBtnTextColorDefault, m_sBtnTextColorCustom;

public:
	enum e_def_icon_type_t
	{
		__DIT_BOX			= 0,
		__DIT_FRAME			= 1,
		__DIT_CHAR			= 2,
		__DIT_FILL_2007		= 3,
		__DIT_CHAR_2007		= 4,
	};

private:
	e_def_icon_type_t m_eDIT;

public:
	virtual void OnSyncIcon();
	virtual CSize CalculateLayout(
		CDC & dc,
		CSize sizePreCalc,
		BOOL bHorz
		);

public:
	DECLARE_DYNCREATE( CExtBarColorButton );

	CExtBarColorButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0,
		COLORREF clrDefault = RGB(0,0,0),
		COLORREF clrSelected = RGB(0,0,0),
		LPARAM lParamCookie = 0L,
		bool bEnableBtnColorDefault = true,
		bool bEnableBtnColorCustom = true,
		__EXT_MFC_SAFE_LPCTSTR sBtnTextColorDefault = NULL,
		__EXT_MFC_SAFE_LPCTSTR sBtnTextColorCustom = NULL,
		e_def_icon_type_t eDIT = __DIT_BOX
		)
		: CExtBarButton( pBar, nCmdID, nStyle )
		, m_clrDefault( clrDefault )
		, m_clrSelected( clrSelected )
		, m_lParamCookie( lParamCookie )
		, m_bEnableBtnColorDefault( bEnableBtnColorDefault )
		, m_bEnableBtnColorCustom( bEnableBtnColorCustom )
		, m_sBtnTextColorDefault( (sBtnTextColorDefault==NULL) ? _T("") : sBtnTextColorDefault )
		, m_sBtnTextColorCustom( (sBtnTextColorCustom==NULL) ? _T("") : sBtnTextColorCustom )
		, m_eDIT( eDIT )
	{
		OnSyncIcon();
	}

	virtual COLORREF ColorGet( bool bSelected ) const;
	virtual void ColorSet( COLORREF clr, bool bSelected );
	virtual LPARAM LParamCookieGet() const;
	virtual void LParamCookieSet(
		LPARAM lParamCookie = 0L
		);
	virtual bool IsEnabledBtnColorDefault() const;
	virtual void EnableBtnColorDefault(
		bool bEnable = true
		);
	virtual bool IsEnabledBtnColorCustom() const;
	virtual void EnableBtnColorCustom(
		bool bEnable = true
		);
	virtual __EXT_MFC_SAFE_LPCTSTR BtnTextDefaultGet() const;
	virtual void BtnTextDefaultSet(
		__EXT_MFC_SAFE_LPCTSTR sText = NULL
		);
	virtual __EXT_MFC_SAFE_LPCTSTR BtnTextCustomGet() const;
	virtual void BtnTextCustomSet(
		__EXT_MFC_SAFE_LPCTSTR sText = NULL
		);
	e_def_icon_type_t IconTypeGet() const
	{
		ASSERT_VALID( this );
		return m_eDIT;
	}
	void IconTypeSet( e_def_icon_type_t eDIT = __DIT_BOX )
	{
		ASSERT_VALID( this );
		if( m_eDIT != eDIT )
		{
			m_eDIT = eDIT;
			OnSyncIcon();
		}
	}

	virtual void OnGenerateColorIcon(
		CExtCmdIcon & icon
		);

	virtual CExtCmdIcon * GetIconPtr();
	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);
	bool CanBePressedInDisabledState();
	virtual bool IsAbleToTrackMenu(
		bool bCustomizeMode = false
		) const;
	virtual UINT OnTrackPopup(
		CPoint point,
		bool bSelectAny,
		bool bForceNoAnimation
		);
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);
	virtual LRESULT OnHelpHitTest(
		CPoint point
		);
	virtual void OnUpdateCmdUI(
		CWnd * pTarget,
		BOOL bDisableIfNoHndler,
		int nIndex
		);
	
	virtual bool CanStartCommandDrag();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual int CalculateDropOverEffect(
		CPoint point,
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		bool bAllowTrackNodePopup = true
		);
	virtual CExtCustomizeCmdTreeNode * GetCmdNode(
		bool bInitial = false
		);
	virtual void SetBasicCmdNode(
		CExtCustomizeCmdTreeNode * pNode
		);
	virtual void SetCustomizedCmdNode(
		CExtCustomizeCmdTreeNode * pNode
		);
	virtual CExtCustomizeCmdTreeNode * OnCustomizeNodeInit(
		CExtCustomizeSite * pSite,
		CExtCustomizeCmdTreeNode * pParentNode // toolbar node
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	virtual void OnDeliverCmd();

}; // class CExtBarColorButton

/////////////////////////////////////////////////////////////////////////////
// CExtBarCheckBoxButton

class __PROF_UIS_API CExtBarCheckBoxButton : public CExtBarButton
{
public:
	DECLARE_DYNCREATE( CExtBarCheckBoxButton );

	CExtBarCheckBoxButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	virtual ~CExtBarCheckBoxButton();

	virtual CSize CalculateLayout(
		CDC & dc,
		CSize sizePreCalc,
		BOOL bHorz
		);
	virtual void PaintCompound(
		CDC & dc,
		bool bPaintParentChain,
		bool bPaintChildren,
		bool bPaintOneNearestChildrenLevelOnly
		);

	virtual CExtCmdIcon * GetIconPtr();
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	virtual bool IsNoRibbonLayout() const;
	virtual INT RibbonILV_Get(
		INT nType = 0 // -1 min, 0 current, 1 - max
		) const;
	virtual CSize RibbonILV_CalcSize(
		CDC & dc,
		INT nILV = -1 // -1 use current visual level
		) const;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

protected:
	virtual INT _GetBoxState() const;
	virtual CSize _GetBoxSize(
		CDC & dc
		) const;
	virtual void _PaintBox(
		CDC & dc,
		CRect & rcBox 
		) const;
		
}; // class CExtBarCheckBoxButton

/////////////////////////////////////////////////////////////////////////////
// CExtBarRadioBoxButton

class __PROF_UIS_API CExtBarRadioBoxButton : public CExtBarCheckBoxButton
{
public:
	DECLARE_DYNCREATE( CExtBarRadioBoxButton );

	CExtBarRadioBoxButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	virtual ~CExtBarRadioBoxButton();

protected:
	virtual INT _GetBoxState() const;
	virtual CSize _GetBoxSize(
		CDC & dc
		) const;
	virtual void _PaintBox(
		CDC & dc,
		CRect & rcBox 
		) const;
}; // class CExtBarRadioBoxButton

/////////////////////////////////////////////////////////////////////////////
// CExtBarLabelButton

class __PROF_UIS_API CExtBarLabelButton : public CExtBarButton
{
public:
	DECLARE_DYNCREATE( CExtBarLabelButton );

	CExtBarLabelButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	virtual ~CExtBarLabelButton();

	virtual CExtCmdIcon * GetIconPtr();
	virtual void OnUpdateCmdUI(
		CWnd * pTarget,
		BOOL bDisableIfNoHndler,
		int nIndex
		);
	virtual bool IsHover() const;
	virtual bool IsPressed() const;
	virtual bool IsPressedTracking() const;
	virtual void SetHover(
		bool bHover = true
		);
	virtual void OnHover(
		CPoint point,
		bool bOn,
		bool & bSuspendTips
		);
	virtual void OnClick(
		CPoint point,
		bool bDown
		);
	
	virtual bool AnimationClient_OnQueryEnabledState(
		INT eAPT // __EAPT_*** animation type
		) const;

	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);
	
	virtual bool OnQueryFlatTrackingEnabled() const;

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	virtual bool IsNoRibbonLayout() const;
	virtual INT RibbonILV_Get(
		INT nType = 0 // -1 min, 0 current, 1 - max
		) const;
	virtual CSize RibbonILV_CalcSize(
		CDC & dc,
		INT nILV = -1 // -1 use current visual level
		) const;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

}; // class CExtBarLabelButton

/////////////////////////////////////////////////////////////////////////////
// CExtBarContentExpandButton

typedef
	CArray < CExtBarButton *, CExtBarButton * >
	_tb_buttons_container_t;

#define __ID_TOOLBAR_RIGHT_BUTTON__ ((UINT)-66)
#define __RIGHT_BUTTON_HORZ_DX__ 11 // 14
#define __RIGHT_BUTTON_VERT_DY__ 11 // 12

class CExtPopupBaseWnd;
class CExtPopupMenuWnd;
class CExtPopupMenuSite;
class CExtMiniDockFrameWnd;

class __PROF_UIS_API CExtBarContentExpandButton : public CExtBarButton
{
public:
	DECLARE_DYNCREATE( CExtBarContentExpandButton );

protected:
	bool m_bClearContent:1;

	_tb_buttons_container_t m_buttons;

public:
	CExtBarContentExpandButton(
		CExtToolControlBar * pBar = NULL
		);
	virtual ~CExtBarContentExpandButton();

	bool IsContentClear()
	{
		ASSERT_VALID( this );
		return m_bClearContent;
	}
	void ClearContent( bool bClearContent = true )
	{
		if( this == NULL )
			return;
		ASSERT_VALID( this );
		m_bClearContent = bClearContent;
	}

	const _tb_buttons_container_t & GetButtons() const
	{
		return m_buttons;
	}
	_tb_buttons_container_t & GetButtons()
	{
		return m_buttons;
	}

	virtual CSize CalculateLayout(
		CDC & dc,
		CSize sizePreCalc,
		BOOL bHorz
		);

	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);

	virtual void PaintCompound(
		CDC & dc,
		bool bPaintParentChain,
		bool bPaintChildren,
		bool bPaintOneNearestChildrenLevelOnly
		);

	virtual UINT OnGetTrackPopupFlags();
	virtual UINT OnTrackPopup(
		CPoint point,
		bool bSelectAny,
		bool bForceNoAnimation
		);
	virtual void OnHover(
		CPoint point,
		bool bOn,
		bool & bSuspendTips
		);
	virtual void OnClick(
		CPoint point,
		bool bDown
		);
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);
	virtual LRESULT OnHelpHitTest(
		CPoint point
		);

	virtual bool CanStartCommandDrag();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual CExtCustomizeCmdTreeNode * GetCmdNode(
		bool bInitial = false
		);
	virtual void SetBasicCmdNode(
		CExtCustomizeCmdTreeNode * pNode
		);
	virtual void SetCustomizedCmdNode(
		CExtCustomizeCmdTreeNode * pNode
		);
	virtual CExtCustomizeCmdTreeNode * OnCustomizeNodeInit(
		CExtCustomizeSite * pSite,
		CExtCustomizeCmdTreeNode * pParentNode // toolbar node
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

}; // class CExtBarContentExpandButton

/////////////////////////////////////////////////////////////////////////////
// CExtBarSliderButton

class __PROF_UIS_API CExtBarSliderButton : public CExtBarButton
{
public:
	enum e_SliderArea_t
	{
		__ESA_ICON_AND_TEXT = 0, // both icon and text
		__ESA_SLIDER = 1, // all slider
		__ESA_SCROLLABLE = 2, // scrollable part of slider
		__ESA_PAGE_UP = 3,
		__ESA_PAGE_DOWN = 4,
		__ESA_LINE_UP = 5,
		__ESA_LINE_DOWN = 6,
		__ESA_THUMB = 7,
		__ESA_COUNT = 8,
		__ESA_UNKNOWN_OR_NOWHERE = -1,
		__ESA_PAINT_RANGE_FIRST = __ESA_SLIDER,
		__ESA_PAINT_RANGE_LAST = __ESA_THUMB,
		__ESA_CLICKABLE_RANGE_FIRST = __ESA_PAGE_UP,
		__ESA_CLICKABLE_RANGE_LAST = __ESA_THUMB,
	};
protected:
	ULONG m_nScrollTotalRange, m_nScrollPos, m_nScrollPageSize;
	INT m_nScrollButtonExtentH, m_nScrollButtonExtentV,
		m_nScrollControlExtentH, m_nScrollControlExtentV;
	CRect m_arrAreaRects[__ESA_COUNT];
	e_SliderArea_t m_eAreaTracked, m_eAreaPressed;
public:
	DECLARE_DYNCREATE( CExtBarSliderButton );
	CExtBarSliderButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0,
		ULONG nScrollTotalRange = 0L,
		ULONG nScrollPos = 0L,
		ULONG nScrollPageSize = 0L,
		INT nScrollButtonExtentH = 0,
		INT nScrollButtonExtentV = 0,
		INT nScrollControlExtentH = 50,
		INT nScrollControlExtentV = 50
		)
		: CExtBarButton( pBar, nCmdID, nStyle )
		, m_nScrollTotalRange( nScrollTotalRange )
		, m_nScrollPos( nScrollPos )
		, m_nScrollPageSize( nScrollPageSize )
		, m_nScrollButtonExtentH( nScrollButtonExtentH )
		, m_nScrollButtonExtentV( nScrollButtonExtentV )
		, m_nScrollControlExtentH( nScrollControlExtentH )
		, m_nScrollControlExtentV( nScrollControlExtentV )
		, m_eAreaTracked( __ESA_UNKNOWN_OR_NOWHERE )
		, m_eAreaPressed( __ESA_UNKNOWN_OR_NOWHERE )
	{
		for(	int nRectNo = 0;
				nRectNo < __ESA_COUNT;
				nRectNo++
				)
				m_arrAreaRects[ nRectNo ].
					SetRect( 0, 0, 0, 0 );
		ASSERT( m_nScrollPos <= m_nScrollTotalRange );
		ASSERT( m_nScrollButtonExtentH >= 0 );
		ASSERT( m_nScrollButtonExtentV >= 0 );
		ASSERT( m_nScrollControlExtentH >= 0 );
		ASSERT( m_nScrollControlExtentV >= 0 );
	}
	ULONG ScrollLimitGet() const;
	virtual ULONG ScrollTotalRangeGet() const;
	virtual ULONG ScrollTotalRangeSet( ULONG nScrollTotalRange );
	virtual ULONG ScrollPosGet() const;
	virtual ULONG ScrollPosSet( ULONG nScrollPos );
	virtual ULONG ScrollPageSizeGet() const;
	virtual ULONG ScrollPageSizeSet( ULONG nScrollPageSize );
	virtual INT ScrollButtonExtentHorzGet() const;
	virtual INT ScrollButtonExtentHorzSet( INT nScrollButtonExtent );
	virtual INT ScrollButtonExtentVertGet() const;
	virtual INT ScrollButtonExtentVertSet( INT nScrollButtonExtent );
	virtual INT ScrollControlExtentHorzGet() const;
	virtual INT ScrollControlExtentHorzSet( INT nScrollControlExtent );
	virtual INT ScrollControlExtentVertGet() const;
	virtual INT ScrollControlExtentVertSet( INT nScrollControlExtent );
	virtual void RecalcSliderLayout();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CExtBarSliderButton::e_SliderArea_t GetTrackedArea() const;
	CExtBarSliderButton::e_SliderArea_t GetPressedArea() const;
	CRect GetAreaRect( CExtBarSliderButton::e_SliderArea_t eSA ) const;

	// slider button overrides
	virtual void OnPaintScrollArea(
		CDC & dc,
		CExtBarSliderButton::e_SliderArea_t eSA,
		bool bHorz
		);

	// CExtBarButton overrides
	virtual bool GetSeparatedDropDown() const
	{
		ASSERT_VALID( this );
		return false;
	}
	virtual void SetSeparatedDropDown( bool bSeparatedDropDown = true )
	{
		ASSERT_VALID( this );
		ASSERT( FALSE );
		bSeparatedDropDown;
		return;
	}
	virtual bool GetAutoChangeID() const
	{
		ASSERT_VALID( this );
		return false;
	}
	virtual void SetAutoChangeID( bool bAutoChangeID = true )
	{
		ASSERT_VALID( this );
		ASSERT( FALSE );
		bAutoChangeID;
		return;
	}
	virtual void CtrlSet(
		CWnd * pCtrl,
		bool bCtrlAutoDestroyed
		)
	{
		ASSERT_VALID( this );
		ASSERT( FALSE );
		pCtrl;
		bCtrlAutoDestroyed;
		return;
	}
//	virtual bool GetNoRotateVerticalLayout() const
//	{
//		return true;
//	}
//	virtual void SetNoRotateVerticalLayout( bool bNoRotateVerticalLayout = true )
//	{
//		ASSERT( FALSE );
//		bNoRotateVerticalLayout;
//		return;
//	}
	virtual void SetMenu(
		HMENU hMenu,
		bool bPopupMenu,
		bool bAutoDestroyMenu
		);
	virtual HMENU GetMenu();
//	virtual bool IsLargeIcon() const;
//	virtual bool IsDisplayScreenTip() const;
//	virtual bool IsDisplayShortcutKeysOnScreenTip() const;
	virtual void SetRect( const RECT & rectButton );
	bool CanBePressedInDisabledState();
	virtual bool IsAbleToTrackMenu(
		bool bCustomizeMode = false
		) const;
	virtual bool IsContainerOfPopupLikeMenu();
//	virtual CExtCmdIcon * GetIconPtr();
	virtual CSize CalculateLayout(
		CDC & dc,
		CSize sizePreCalc,
		BOOL bHorz
		);
	virtual void PaintCompound(
		CDC & dc,
		bool bPaintParentChain,
		bool bPaintChildren,
		bool bPaintOneNearestChildrenLevelOnly
		);
	virtual UINT OnTrackPopup(
		CPoint point,
		bool bSelectAny,
		bool bForceNoAnimation
		);
	virtual void OnHover(
		CPoint point,
		bool bOn,
		bool & bSuspendTips
		);
	virtual void OnClick(
		CPoint point,
		bool bDown
		);
	virtual void OnDeliverCmd();
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);
	virtual LRESULT OnHelpHitTest(
		CPoint point
		);
	virtual void OnUpdateCmdUI(
		CWnd * pTarget,
		BOOL bDisableIfNoHndler,
		int nIndex
		);
	virtual bool OnSetCursor( CPoint point );
}; // class CExtBarSliderButton

/////////////////////////////////////////////////////////////////////////////
// CExtToolControlBar

class __PROF_UIS_API CExtToolControlBar : public CExtControlBar
{
public:
	DECLARE_DYNCREATE( CExtToolControlBar );

private:
	friend class CExtBarButton;
	friend class CExtBarContentExpandButton;
	friend class CExtBarMdiDocButton;
	class CExtToolControlBarCmdUI;
	friend class CExtPopupBaseWnd;
	friend class CExtPopupMenuWnd;
	friend class CExtPopupMenuSite;
	friend class CExtMiniDockFrameWnd;

public:
	virtual LONG _CalcSplitButtonExtent( const CExtBarButton * pTBB, bool bHorz ) const;
	bool IsRightExpandButton( int nBtnIdx );
protected:
	CExtBarContentExpandButton * m_pRightBtn;

	_tb_buttons_container_t m_buttons;
	
	virtual bool _AdjustBGInfo();
	virtual void _RemoveAllButtonsImpl();
	virtual int _GetButtonsCountImpl() const;

	virtual void _SetCursor(
		const CPoint & point
		);
	virtual void _RedrawOnPosChanged();

// Construction
public:
	CExtToolControlBar();

	virtual BOOL LoadToolBar(
		__EXT_MFC_SAFE_LPCTSTR lpszResourceName,
		COLORREF clrTransparent = RGB(192,192,192)
		);
	BOOL LoadToolBar(
		UINT nIDResource,
		COLORREF clrTransparent = RGB(192,192,192)
		)
	{
		return
			LoadToolBar(
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE(nIDResource) ),
				clrTransparent
				);
	}
	
	virtual BOOL InsertButton(
		int nPos = -1, // append
		UINT nCmdID = ID_SEPARATOR,
		BOOL bDoRecalcLayout = TRUE
		);
	virtual BOOL InsertSpecButton(
		int nPos, // -1 - append
		CExtBarButton * pButton,
		BOOL bDoRecalcLayout = TRUE
		);
	virtual BOOL RemoveButton(
		int nPos,
		BOOL bDoRecalcLayout = TRUE
		);
	virtual bool IsShowTipWhenDisabled( const CExtBarButton * pTBB ) const;

	virtual BOOL SetButtons(
		const UINT * lpIDArray = NULL,
		int nIDCount = 0
		); // lpIDArray can be NULL to allocate empty buttons
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual BOOL SetButtons(
		CExtCustomizeCmdTreeNode * pNode
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

// Attributes
public:

	static const UINT g_nMsgTrackButtonMenu;
	static const UINT g_nMsgContinueTrackMenu;
	bool m_bRightButtonDisplayBarsList:1;
	bool m_bPaletteMode:1;
	bool m_bForceBalloonGradientInDialogs:1;
	bool m_bForceNoBalloonWhenRedockable:1;

	int CommandToIndex( UINT nIDFind ) const;
	UINT GetButtonID( int nIndex ) const;
	virtual void GetButtonRect( int nIndex, LPRECT lpRect ) const;
	UINT GetButtonStyle( int nIndex ) const;
	void SetButtonStyle( int nIndex, UINT nStyle );

	CExtBarButton::e_wrap_t GetBtnWrapCode() const;

protected:
	virtual bool _IsShowContentWhenDragging() const;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	bool m_bKeyTipsDisplayed:1, m_bHelperKeyTipsSupported:1;
	CExtCustomizeCmdKeyTip m_keyTipChain;
public:
	virtual bool KeyTipsDisplayedGet() const;
	virtual void KeyTipsDisplayedSet(
		bool bKeyTipsDisplayed = true
		);
	virtual CExtCustomizeCmdKeyTip & KeyTipChainGet();
	const CExtCustomizeCmdKeyTip & KeyTipChainGet() const;
	virtual void KeyTipChainSet(
		const CExtCustomizeCmdKeyTip & keyTipChain
		);
	void KeyTipChainEmpty();
	virtual INT KeyTipTranslate(
		DWORD dwKeyCode,
		CExtCustomizeCmdKeyTip * pKeyTipChain,
		bool bAutoInvokeAction,
		bool & bActionInvoked,
		bool & bContinueKeyTipMode
		);
	virtual void KeyTipDisplay(
		CExtCustomizeCmdKeyTip & keyTipChain
		);
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)

public:
	virtual void OnFlatTrackingStart(
		HDWP & hPassiveModeDWP
		);
	virtual void OnFlatTrackingStop();

	virtual bool IsLargeIcons() const;
	virtual bool IsDisplayScreenTips() const;
	virtual bool IsDisplayShortcutKeysOnScreenTips() const;
	virtual LRESULT DoHelpHitTest( CPoint ptHelpHitTest );

	virtual CExtBarButton * OnCreateBarCommandBtn(
		UINT nCmdID,
		UINT nStyle = 0
		);
	virtual CExtBarContentExpandButton * OnCreateBarRightBtn();
	virtual COLORREF OnQueryCustomAccentEffectForIcon(
		CDC & dc,
		CExtBarButton * pTBB
		);
	BOOL InitContentExpandButton();

	// for changing button info
	void GetButtonInfo(
		int nIndex,
		UINT & nID,
		UINT & nStyle
		) const;
	void SetButtonInfo(
		int nIndex,
		UINT nID,
		UINT nStyle
		);
	CExtBarButton * GetButton( int nIndex );
	CExtBarContentExpandButton * GetRightButton();

	CWnd * GetButtonCtrl(
		int nIndex
		);
	bool SetButtonCtrl(
		int nIndex,
		CWnd * pCtrl = NULL,
		bool bCtrlAutoDestroyed = TRUE
		);

	void SetButtonCtrlVisibleVertically(
		int nIndex,
		bool bVisible = true
		);
	bool GetButtonCtrlVisibleVertically(
		int nIndex
		) const;

	int GetButtonByAccessKey( __EXT_MFC_SAFE_TCHAR vkTCHAR );

	HMENU GetButtonMenu(
		int nIndex
		);
	bool SetButtonMenu(
		int nIndex,
		HMENU hMenu = NULL,
		bool bPopupMenu = TRUE,
		bool bAutoDestroyMenu = TRUE,
		bool bDoRecalcLayout = TRUE
		);
	bool MarkButtonAsMdiWindowsMenu(
		int nIndex,
		bool bAppendMdiWindowsMenu = TRUE
		);
	virtual BOOL TrackButtonMenu(
		int nIndex,
		BOOL bSelectAny = FALSE,
		BOOL bDelayedTracking = FALSE,
		BOOL bNoAnimation = FALSE
		);
	int GetVisibleButton(
		int nBtnIdx,
		BOOL bNext,
		BOOL bPassDisabled = TRUE
		);
	bool m_bLastDropDownHT:1;
	bool m_bHelperFlatTrackingCalcEnabledLeftRight:1;
	bool m_bHelperFlatTrackingCalcEnabledUpDown:1;
	bool m_bHelperFlatTrackingCalcEnabledTab:1;
	virtual INT OnCalcFlatTrackingIndex(
		/*__EXT_MFC_SAFE_TCHAR*/ INT vkTCHAR,
		INT nStartSearchIndex
		);
	virtual bool OnCalcFlatTrackingIndexCheckPass(
		/*__EXT_MFC_SAFE_TCHAR*/ INT vkTCHAR,
		INT nPassIndex,
		const CExtBarButton * pPrevTBB,
		const CExtBarButton * pNextTBB
		) const;
	virtual bool OnCalcFlatTrackingTabOrder(
		CTypedPtrArray < CPtrArray, CExtBarButton * > & arrTabOrder
		);
	virtual bool OnCalcFlatTrackingIndexCheckIntersectionEnabled(
		CExtBarButton * pTBB,
		/*__EXT_MFC_SAFE_TCHAR*/ INT vkTCHAR
		);

	int GetButtonsCount() const { return _GetButtonsCountImpl(); }
	int GetMenuTrackingButton() const { return m_nBtnIdxMenuTracking; }

	virtual bool _OnHookButtonClick( CExtBarButton * pTBB, CPoint point, bool bDown );
	virtual bool _OnHookButtonRClick( CExtBarButton * pTBB, CPoint point, bool bDown );
	virtual bool _OnHookButtonDblClick( CExtBarButton * pTBB, CPoint point );
	virtual bool _OnHookButtonDeliverCmd( CExtBarButton * pTBB );

// Implementation
public:
	virtual ~CExtToolControlBar();

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual CSize CalcFixedLayout(
		BOOL bStretch,
		BOOL bHorz
		);
    virtual CSize CalcDynamicLayout(
		int nLength,
		DWORD dwMode
		);

	virtual void ToggleDocking();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual CExtBarButton * _GetButtonPtr( int nIndex ) const;
public:
	virtual void _InvalidateButton( int nIndex );
	virtual void _UpdateButton( int nIndex );
	virtual void _CheckHitTestablePoint(
		CPoint & point
		);
protected:
	CPoint m_ptLastClientHover;
public:
	virtual bool _UpdateHoverButton(
		CPoint point = CPoint(-1,-1), // default is use ::GetCursorPos()
		bool bEnableUpdateWindow = true
		);
protected:
	virtual DWORD RecalcDelayShow(
		AFX_SIZEPARENTPARAMS * lpLayout
		);

public:
	virtual bool OnQueryShiftRTL() const;
	virtual bool OnQueryRevertRTL() const;
	virtual void _RecalcPositionsImpl();
	virtual void _RecalcLayoutImpl();
	virtual void _RibbonPageRslaResetStateData();

public:
	// rect should have client area size
	virtual void _CalcInsideRect(
		CRect & rect,
		BOOL bHorz
		) const;
	
	virtual CSize _CalcLayout(
		DWORD dwMode,
		int nLength = -1
		);
	virtual CSize _CalcSize( BOOL bVerticallyDocked );
	virtual void _SizeToolBar(
		int nLength,
		BOOL bVert = FALSE
		);
	virtual int _WrapToolBarV(
		int nHeight
		);
	virtual int _WrapToolBarH(
		int nWidth,
		int nHeight = 32767
		);
protected:
    virtual void _DraggingUpdateState(
		const CPoint & point,
		bool bForceFloatMode
		);
	virtual CSize _CalcDesiredMinOuterSize( BOOL bHorz ) const;
	CExtControlBar::InternalAffixmentData m_AffixmentData;
	virtual CExtControlBar::InternalAffixmentData *
		_AffixmentGetOuter();
	virtual void _AffixmentSetOuter(
		CExtControlBar::InternalAffixmentData * pAffixmentData
		);
protected:
	bool m_bHelperTempFullRowMode:1, m_bInsideCalcLayout:1;
public:
	virtual bool _AffixmentIsAlignedHW() const;
	virtual bool _GetFullRowMode() const;
	static LRESULT _ContinueHelpHitTestMode(
		CExtToolControlBar * pBar
		);

public:
	virtual CSize _GetDefButtonSize() const;
	int _GetIndexOf( CExtBarButton * pTBB );

	virtual bool IsPaletteChevronMenu() const;

public:
	void _SwitchMenuTrackingIndex(
		int iNewMenuTrackingIndex = -1
		);
	static CExtToolControlBar * _GetMenuTrackingBar();
	static void _CloseTrackingMenus();
	static void _CloseCustomMenusAll();
	virtual void _CloseCustomMenus();
	static CExtToolControlBar * _GetMenuTrackingToolControlBar();
	static CExtMenuControlBar * _GetMenuTrackingMenuControlBar();
	static void _CbPaintCombinedContent(
		LPVOID pCookie,
		CDC & dc,
		const CWnd & refWndMenu,
		const CRect & rcExcludeArea, // in screen coords
		int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
		);

public:
	static bool g_bMenuTracking;
	static bool g_bMenuTrackingExpanded;
	static bool g_bToolbarLargeIcons;
	static bool g_bToolbarScreenTips;
	static bool g_bToolbarShortcutKeysOnScreenTips;

	bool m_bEnableDblClickToggleDocking:1;
	static bool g_bEnableDblClickToggleDocking;
protected:
	int m_nBtnIdxMenuTracking;
	int m_nBtnIdxCapture;    // index of button with capture (-1 => none)
	int m_nBtnIdxHover;
	volatile int m_nUpdateCtrlLockCount;

public:
	virtual void _UpdateCtrlLock();
	virtual void _UpdateCtrlUnlock();
	virtual bool _UpdateCtrlIsLocked() const;
	virtual bool _UpdateCtrlIsEnabled(
		CExtBarButton * pTBB
		);
	virtual void _UpdateCtrlAll();

	int _GetTrackingButtonIndex() const
	{
		ASSERT_VALID( this );
		return m_nBtnIdxMenuTracking;
	}
	int _GetHoverButtonIndex() const
	{
		ASSERT_VALID( this );
		return m_nBtnIdxHover;
	}

	virtual void OnUpdateCmdUI(
		CFrameWnd* pTarget,
		BOOL bDisableIfNoHndler
		);
	virtual bool OnQueryMultiRowLayout() const;
	virtual bool _IsSimplifiedDropDownButtons() const;
	virtual	bool _OnMouseMoveMsg(UINT nFlags, CPoint point);

	virtual bool IsForceHoverWhenMenuTracking() const;

protected:
	virtual bool _CanDockToInnerCircles() const;
	virtual bool _CanDockToTabbedContainers(
		CExtControlBar * pDestBar
		) const;

public:
	virtual CFont * OnGetToolbarFont(
		bool bVert,
		bool bMeasureFont,
		CObject * pQuerySrc
		);
	virtual void DoPaint( CDC * pDC );
protected:
	int _HitTestImpl(
		CPoint point,
		UINT nButtonStyleInclude = 0, // button must have style
		UINT nButtonStyleExclude = 0,  // button must have not style
		bool bAllowFindNearest = false // if did not hit - find nearest
		) const;
public:
	virtual int HitTest( CPoint point ) const;
protected:
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;

public:
	virtual bool OnQueryHoverBasedMenuTracking(
		const CExtBarButton * pTBB
		) const;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
protected:
	CExtCustomizeSite * m_pCustomizeSite;
	CExtBarButton * m_pDragSourceTBB, * m_pDragTargetTBB;
	int m_nDragTargetShift;
	bool m_bHelperSeparatorAction:1;
public:
	bool
		m_bCustomizationAllowed:1,
		m_bAutoShowOnCustomizationStart:1,
		m_bAutoHideOnCustomizationEnd:1;
	virtual CExtCustomizeSite * GetCustomizeSite();
	CExtCustomizeSite * GetCustomizeSite() const;
	virtual bool OnCustomizeRegisterBar(
		CExtCustomizeSite * pCustomizeSite
		);
	virtual void OnCustomizeGetBarInfo(
		CExtCustomizeSite::CToolBarCustomizeInfo * pInfo
		);
	virtual void OnCustomizeUpdateInitialBarState(
		CExtCustomizeCmdTreeNode * pParentNode,
		bool bRecalcLayout
		);
	virtual void OnCustomizeModeEnter();
	virtual void OnCustomizeModeLeave();
	virtual void OnCustomizeListBoxUpdate( CExtCustomizeToolbarsListBox * pWndList );
	virtual bool _IsDockSiteCustomizeMode() const;
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
	virtual INT OnCustomizeCalcCmdNodePos(
		INT nButtonPos,
		bool * p_bReinitGroupStart = NULL
		);
	// CExtCustomizeSite::ICustomizeDropSource
	virtual void OnCustomizeSourceDragComplete(
		DROPEFFECT de,
		bool bCanceled,
		bool * p_bNoResetActiveItem
		);

	virtual bool OnCustomizeCanShowHideButtons();
	virtual void OnCustomizeInitShowHideButtonMenu(
		CExtPopupMenuWnd * pFolder
		);
	static bool stat_CbXtraMarkState(
		CExtPopupMenuWnd * pPopup,
		LPVOID pItemData
		);
	static bool stat_CbCmdDeliver(
		CExtPopupMenuWnd * pPopup,
		LPVOID pItemData
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

public:
	class __PROF_UIS_API FtLayoutQuery
	{
	public:
		CArray < CRect, CRect > m_arrRowRects;
		CArray < INT, INT > m_arrRowBtnIndices;
		CExtToolControlBar * m_pToolBar;
		CExtMiniDockFrameWnd * m_pMiniFrame;
		HWND m_hWndFor;
		FtLayoutQuery();
		~FtLayoutQuery();
		void Empty();
		bool Query( HWND hWndFor );
		void ReMapRectsToTargetClient();
		void ReMapRectsToTargetWindow();
		void DrawRects( CDC & dc );
	}; // class FtLayoutQuery

	bool _DoFtLayoutQuery(
		FtLayoutQuery * pFtLayoutQuery,
		CDC * pToolBarOutputDC
		) const;

	virtual bool OnDeliverCmd( CExtBarButton * pTBB );

	virtual void AnimationSite_OnProgressShutdownTimer( UINT nTimerID );

protected:

	//{{AFX_VIRTUAL(CExtToolControlBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtToolControlBar)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnTrackButtonMenu(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMenuClosed( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif
}; // class CExtToolControlBar

/////////////////////////////////////////////////////////////////////////////
// CExtPanelControlBar

class __PROF_UIS_API CExtPanelControlBar : public CExtToolControlBar
{
public:
	DECLARE_DYNCREATE( CExtPanelControlBar );

// Construction
public:
	CExtPanelControlBar();

// Attributes
public:
	CRect m_rcMargins;

// Operations
public:

	virtual CSize CalcFixedLayout(
		BOOL bStretch,
		BOOL bHorz
		);
    virtual CSize CalcDynamicLayout(
		int nLength,
		DWORD dwMode
		);

	enum e_fixed_bar_size_query_t
	{
		__FSQ_FLOATING	= 0,
		__FSQ_DOCKED_H	= 1,
		__FSQ_DOCKED_V	= 2,
	};

	// called when need to know new bar size after re-docking
	virtual CSize OnQueryFixedBarNewSize(
		CExtPanelControlBar::e_fixed_bar_size_query_t eWhichSizeNeedToKnow
		) const;

	// should return size of margins for single child window
	virtual CRect OnQueryFixedBarChildWindowMargins() const;

protected:
	virtual CSize _QueryFixedBarNewSize(
		CExtPanelControlBar::e_fixed_bar_size_query_t eWhichSizeNeedToKnow
		) const;
	CSize _CalcLayout(
		DWORD dwMode,
		int nLength = -1
		);
	virtual CSize _CalcDesiredMinOuterSize( BOOL bHorz ) const;
	virtual INT _CalcDesiredMinHW() const;
	virtual INT _CalcDesiredMinVH() const;
	virtual CSize _CalcDesiredMinFloatedSize() const;
	virtual void _RepositionChild(
		bool bRedraw = true
		);
	virtual CSize _GetChildSize() const;
public:
	virtual bool _AffixmentIsAlignedHW() const;
	virtual bool OnQueryMultiRowLayout() const;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
public:
	virtual CExtCustomizeSite * GetCustomizeSite();
	virtual bool OnCustomizeRegisterBar(
		CExtCustomizeSite * pCustomizeSite
		);
	virtual void OnCustomizeModeEnter();
	virtual void OnCustomizeModeLeave();
	virtual void OnCustomizeListBoxUpdate( CExtCustomizeToolbarsListBox * pWndList );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

// Implementation
public:
	virtual ~CExtPanelControlBar();

	virtual LRESULT DoHelpHitTest( CPoint ptHelpHitTest );

	//{{AFX_VIRTUAL(CExtPanelControlBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtPanelControlBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtPanelControlBar

#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP

/////////////////////////////////////////////////////////////////////////////
// CExtBarUndoRedoButton

class __PROF_UIS_API CExtBarUndoRedoButton : public CExtBarButton
{
public:
	DECLARE_DYNCREATE( CExtBarUndoRedoButton );

	CExtBarUndoRedoButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	virtual ~CExtBarUndoRedoButton();

	virtual LPARAM LParamCookieGet() const;
	virtual void LParamCookieSet(
		LPARAM lParamCookie = 0L
		);

	bool CanBePressedInDisabledState();
	virtual bool IsAbleToTrackMenu(
		bool bCustomizeMode = false
		) const;
	virtual LRESULT OnHelpHitTest(
		CPoint point
		);
	virtual UINT OnTrackPopup(
		CPoint point,
		bool bSelectAny,
		bool bForceNoAnimation
		);
	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);
	virtual CExtPopupMenuWnd * OnCreateDropPopup(
		bool bContentExpand // true - inserting to content expand menu, false - tracking dropped popup
		);
	
	virtual bool OnPopupListBoxInitContent(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxSelChange(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxSelEndOK(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxSelEndCancel(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxClose(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxItemDraw(
		CExtPopupInplaceListBox & wndListBox,
		LPDRAWITEMSTRUCT pDIS
		);
	virtual bool OnPopupListBoxItemMeasure(
		CExtPopupInplaceListBox & wndListBox,
		LPMEASUREITEMSTRUCT pMIS
		);
	virtual CSize OnPopupListBoxMeasureTrackSize() const;
	virtual CSize OnPopupListBoxCalcItemExtraSizes() const;

	virtual bool OnPopupUndoRedoFormatCaption(
		CExtSafeString & strCaption,
		CExtPopupUndoRedoMenuWnd * pUndoRedoPopupMenuWnd
		);

protected:
	LPARAM m_lParamCookie;
	DWORD m_dwListBoxStyles; // popup list-box styles
	CExtSafeStringArray m_arrLbItems;
	INT m_nDropDownWidth; // (-1) - auto calc
	INT m_nDropDownHeightMax;

	static bool stat_CbFormatCaption(
		CExtSafeString & strCaption,
		CExtPopupUndoRedoMenuWnd * pUndoRedoPopupMenuWnd,
		CExtBarUndoRedoButton * pUndoRedoTBB
		);
	static bool stat_CbInitListBoxContent(
		CExtPopupInplaceListBox & wndListBox,
		CExtBarUndoRedoButton * pUndoRedoTBB
		);
	static bool stat_CbListBoxSelection(
		CExtPopupInplaceListBox & wndListBox,
		CExtBarUndoRedoButton * pUndoRedoTBB,
		int eSAT // CExtPopupInplaceListBox::e_sel_action_t
		);
	static bool stat_CbListBoxItemDraw(
		CExtPopupInplaceListBox & wndListBox,
		CExtBarUndoRedoButton * pUndoRedoTBB,
		LPDRAWITEMSTRUCT pDIS
		);
	static bool stat_CbListBoxItemMeasure(
		CExtPopupInplaceListBox & wndListBox,
		CExtBarUndoRedoButton * pUndoRedoTBB,
		LPMEASUREITEMSTRUCT pMIS
		);
	
}; // class CExtBarUndoRedoButton

#endif // __EXT_MFC_NO_UNDO_REDO_POPUP

#endif // __EXT_TOOLCONTROLBAR_H

