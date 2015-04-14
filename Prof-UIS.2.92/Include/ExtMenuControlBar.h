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

#if (!defined __EXT_MENUCONTROLBAR_H)
#define __EXT_MENUCONTROLBAR_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_HOOK_H)
	#include "../Src/ExtHook.h"
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CExtBarMdiDocButton

class __PROF_UIS_API CExtBarMdiDocButton : public CExtBarButton
{
public:
	DECLARE_DYNCREATE(CExtBarMdiDocButton)

protected:
	CExtCmdIcon m_iconCache;
	HWND m_hWndActiveMdiChild;

	HWND _ActiveMdiChildWndGet();
	static void _ExtractWndIcon(
		HWND hWnd,
		CExtCmdIcon & icon
		);
public:
	HWND _ActiveMdiChildWndAttach(
		HWND hWndActiveMdiChildNew
		);
	HWND _ActiveMdiChildWndDetach();

	bool m_bHandleDblClickAsCloseCmd:1;

	CExtBarMdiDocButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	virtual ~CExtBarMdiDocButton();

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	virtual bool IsRibbonPaintingMode() const;
	virtual CSize RibbonILV_CalcSize(
		CDC & dc,
		INT nILV = -1 // -1 use current visual level
		) const;
#endif //  (!defined __EXT_MFC_NO_RIBBON_BAR)
	virtual CSize CalculateLayout(
		CDC & dc,
		CSize sizePreCalc,
		BOOL bHorz
		);
	virtual UINT GetStyle() const
	{
		return
			CExtBarButton::GetStyle()
			&
			(~TBBS_DISABLED)
			;
	}
	virtual void SetStyle( UINT nStyle )
	{
		nStyle &= ~TBBS_DISABLED;
		CExtBarButton::SetStyle( nStyle );
	}
	virtual void ModifyStyle(
		UINT nAdd,
		UINT nRemove = 0
		)
	{
		nAdd &= ~TBBS_DISABLED;
		nRemove |= TBBS_DISABLED;
		CExtBarButton::ModifyStyle(
			nAdd,
			nRemove
			);
	}

	virtual CExtCmdIcon * GetIconPtr();

	virtual void OnUpdateCmdUI(
		CWnd * pTarget,
		BOOL bDisableIfNoHndler,
		int nIndex
		);
	virtual bool OnDblClick(
		CPoint point
		);
	virtual CWnd * GetCmdTargetWnd();

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

	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);

	virtual UINT OnTrackPopup(
		CPoint point,
		bool bSelectAny,
		bool bForceNoAnimation
		);
}; // class CExtBarMdiDocButton

/////////////////////////////////////////////////////////////////////////////
// CExtBarMdiRightButton

#define __GAP_BETWEEN_WND_RIGHT_BUTTONS 2

class CExtMenuControlBar;

class __PROF_UIS_API CExtBarMdiRightButton : public CExtBarMdiDocButton
{
protected:
	struct __PROF_UIS_API MdiMenuBarRightButtonsInfo_t
	{
		CWnd * m_pWndMdiChildFrame;
		CExtMenuControlBar * m_pBar;
		int m_cxIcon,m_cyIcon;
		CSize m_calcSize;
		bool
			m_bInitialized:1,
			m_bBtnClose:1,
			m_bBtnMaximize:1,
			m_bBtnMinimize:1,
			m_bBtnHelp:1;
		CRect
			m_rcBtnClose,
			m_rcBtnMaximize,
			m_rcBtnRestore,
			m_rcBtnMinimize,
			m_rcBtnHelp;
		MdiMenuBarRightButtonsInfo_t();
		void EmptyLocations();
		void InitLocations(
			const CRect & rcBtn,
			BOOL bHorz
			);
	}; // struct MdiMenuBarRightButtonsInfo_t

	friend class CExtMenuControlBar;
	
public:
	DECLARE_DYNCREATE(CExtBarMdiRightButton)

public:
	CExtBarMdiRightButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	virtual ~CExtBarMdiRightButton();

	virtual CExtCmdIcon * GetIconPtr();

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

	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);

	virtual UINT OnTrackPopup(
		CPoint point,
		bool bSelectAny,
		bool bForceNoAnimation
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

	virtual bool OnDblClick(
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

	virtual bool OnQueryFlatTrackingEnabled() const;

}; // class CExtBarMdiRightButton

/////////////////////////////////////////////////////////////////////////////
// CExtMenuControlBar

class __PROF_UIS_API CExtMenuControlBar
	: public CExtToolControlBar
	, public CExtHookSink
{
protected:
	friend class CExtBarMdiDocButton;
	friend class CExtBarMdiRightButton;
	friend struct CExtBarMdiRightButton::MdiMenuBarRightButtonsInfo_t;

	virtual void _GetMdiMenuBarRightButtonsInfo(
		CExtBarMdiRightButton::MdiMenuBarRightButtonsInfo_t & _info,
		CExtBarMdiRightButton * pTBB,
		BOOL bHorz
		);

	UINT m_nMdiDocButtonCmdID;
	HWND m_hWndHelper;

	CExtSafeString m_sMdiWindowPopupName;
	
	bool m_bFlatTracking:1,	m_bSysMenuTracking:1;
	int m_nFlatTrackingIndex,m_nOldTrackingIndex;
	CMenu m_menuFrame, m_menuDoc;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtCustomizeSite::CCmdMenuInfo * m_pMenuInfo;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

public:
	bool m_bAdaptiveCmdAllocMode:1;

	virtual int _FlatTrackingGet() const;
	virtual void _FlatTrackingSet( bool bFlatTracking );
	virtual int _FlatTrackingIndexGet() const;
	virtual void _FlatTrackingIndexSet( int nFlatTrackingIndex );
	virtual void _UpdateFlatTracking(
		BOOL bRepaint = TRUE
		);
	virtual void _CancelFlatTracking(
		BOOL bRepaint = TRUE
		);

	virtual void OnFlatTrackingStart(
		HDWP & hPassiveModeDWP
		);
	virtual void OnFlatTrackingStop();

	CRect _GetMainFrameSysIconRect();
	CRect _GetChildFrameSysIconRect();

protected:
	virtual bool OnHookWndMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		);

	HWND _GetHwndPlacement();
	HWND _GetHwndMainFrame();
	HWND _GetHwndChildFrame();
	HWND _GetHwndMdiArea();

	bool m_bMdiApp:1;
	virtual bool _IsMdiApp() const;
	virtual BOOL _UpdateMenuBar(
		BOOL bDoRecalcLayout = TRUE
		);
	virtual CMenu * _GetMenu();
	virtual HWND _GetActiveMdiChildWnd(
		BOOL & bMaximized
		);
	virtual BOOL _InstallMdiDocButtons(
		BOOL bDoRecalcLayout = TRUE
		);
	virtual BOOL _SyncActiveMdiChild();
	virtual BOOL _TrackFrameSystemMenu(
		CWnd * pFrame,
		CPoint * pPoint = NULL, // NULL means calc menu track area automatically
		BOOL bSelectAny = FALSE,
		LPCRECT rcExcludeArea = NULL,
		UINT nTrackFlags = (UINT)(-1),
		BOOL bCombinedMode = FALSE
		);
	virtual bool _InitMdiChildHook( HWND hWndMdiClient );

public:
    DECLARE_DYNCREATE(CExtMenuControlBar);
// Construction
public:
    CExtMenuControlBar();

// Attributes
public:
	// hide expand button on menu bar if possible ?
	static bool g_bHideMenuBarExpandContentButton;

	static const UINT g_nMsgPrepareSystemMenu;
	
	static bool g_bCancelNearestAltUp;
	static bool g_bKeyCodeTracking;

	static bool g_bTranslateContextHelpCmd;
	static ACCEL g_accelContextHelp;

	virtual const ACCEL * _GetTranslatedContextHelpAccel() const;

	// multiple rows in docked state
	bool m_bMultiRowLayout:1;

protected:
	bool m_bMenuBarUsesDelayedButtonMenuTracking:1;
	bool m_bDelayedSyncActiveMdiChild:1;
	bool m_bDelayedUpdateMenuBar:1;
	virtual void _DelaySyncActiveMdiChild();
	virtual void _DelayUpdateMenuBar();
public:
	bool m_bUseFastKeyboardMenuTracking:1,
		m_bAllowProfMainFrameSysmenu:1,
		m_bAllowProfChildFrameSysmenu:1,
		m_bHandleDblClickAsCloseCmd:1;

// Operations
public:

	CMenu * GetMenu(); // get current menu
	BOOL UpdateMenuBar( // update after menu changed
		BOOL bDoRecalcLayout = TRUE
		);

	virtual void SetMdiWindowPopupName(
		__EXT_MFC_SAFE_LPCTSTR sMdiWindowPopupName = NULL
		);

	// LoadMenuBar() required only for CFrameWnd
	// based windows principally without menu
	virtual BOOL LoadMenuBar(
		UINT nResourceID
		);

protected:
	bool m_bHelperTrackingCharCode:1;
public:
	virtual BOOL TranslateMainFrameMessage(MSG* pMsg);
	virtual BOOL TrackMainFrameSystemMenu(
		CPoint * pPoint = NULL, // NULL means calc menu track area automatically
		BOOL bSelectAny = FALSE
		);
	virtual BOOL TrackChildFrameSystemMenu(
		CPoint * pPoint = NULL, // NULL means calc any track area automatically
		BOOL bSelectAny = FALSE
		);

// Overridables

public:
	virtual void OnUpdateCmdUI(
		CFrameWnd * pTarget,
		BOOL bDisableIfNoHndler
		);
	virtual LRESULT DoHelpHitTest( CPoint ptHelpHitTest );

	virtual bool _GetFullRowMode() const;

	virtual bool OnQueryMultiRowLayout() const;
	virtual CExtBarContentExpandButton * OnCreateBarRightBtn();

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual CExtCustomizeSite::CCmdMenuInfo * MenuInfoGet();
	virtual CExtCustomizeSite::CCmdMenuInfo * MenuInfoDetach();
	virtual CExtCustomizeSite::CCmdMenuInfo * MenuInfoAttach(
		CExtCustomizeSite::CCmdMenuInfo * pMenuInfo
		);
	virtual void MenuInfoUpdate();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

protected:
	virtual void _Dragging_OnStart();
	virtual void _Dragging_OnStop(
		InternalDraggingState_t & _dsOld,
		InternalDraggingState_t & _dsNew,
		bool bCancel
		);
	virtual BOOL TrackButtonMenu(
		int nIndex,
		BOOL bSelectAny = FALSE,
		BOOL bDelayedTracking = FALSE,
		BOOL bNoAnimation = FALSE
		);
public:
	virtual CSize _GetDefButtonSize() const;
	virtual bool OnQueryShiftRTL() const;
	virtual bool OnQueryRevertRTL() const;
	virtual void _RecalcPositionsImpl();
	virtual void _RecalcLayoutImpl();
protected:
	virtual void _KillFrameMenu();

// Overrides
public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CExtMenuControlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
// Implementation
public:
    virtual ~CExtMenuControlBar();

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual void OnCustomizeModeEnter();
	virtual void OnCustomizeModeLeave();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	BOOL IsMenuBarTracking()
	{
		if(	m_bFlatTracking
			|| m_bSysMenuTracking
			)
			return TRUE;
		return FALSE;
	}

	mutable bool m_bNonCommandButtonMode:1;
	virtual bool _IsNonCommandButtonMode() const;

	virtual bool IsDisplayMdiDocumentButtons() const;
	virtual	bool _OnMouseMoveMsg(UINT nFlags, CPoint point);
	virtual bool _IsSimplifiedDropDownButtons() const;

// Generated message map functions
protected:
    //{{AFX_MSG(CExtMenuControlBar)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnTrackButtonMenu(WPARAM wParam,LPARAM lParam);
    DECLARE_MESSAGE_MAP()
}; // class CExtMenuControlBar

#endif // __EXT_MENUCONTROLBAR_H

