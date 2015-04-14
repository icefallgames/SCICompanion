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

#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
#define __EXT_CONTROLBAR_TABBED_FEATURES_H

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXTDOCKBAR_H)
	#include <../Src/ExtDockBar.h>
#endif // __EXTDOCKBAR_H

class CExtPopupMenuTipWnd;

/////////////////////////////////////////////////////////////////////////
// CExtDockDynTabBar window

class __PROF_UIS_API CExtDockDynTabBar : public CExtDockDynBar
{
public:

	CExtDockDynTabBar( UINT nCircleNo = 0 );
	
	DECLARE_DYNCREATE( CExtDockDynTabBar );

	virtual void OnDynamicLayoutUpdate();
	virtual void OnDynamicLayoutOptimize();

	// this is the one and only method of interest
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	virtual void CalcOrderedVector(
		ExtControlBarVector_t & vBars
		);

	CSize _RepositionTabbedChilds(
		bool bLockChilds
		);
	void _SyncTabbedChilds(
		bool bSyncSizes,
		bool bSyncSwitcher
		);

public:

#ifdef _DEBUG
	
	virtual void AssertValid() const
	{
		CExtDockDynBar::AssertValid();
	}
	
	virtual void Dump( CDumpContext & dc ) const
	{
		CExtDockDynBar::Dump( dc );
	}

#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtDockDynTabBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtDockDynTabBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtDockDynTabBar

/////////////////////////////////////////////////////////////////////////
// CExtDynTabWnd window

class __PROF_UIS_API CExtDynTabWnd : public CExtTabWnd
{
protected:
	class __PROF_UIS_API CDynDropTarget : public COleDropTarget
	{
		CExtDynTabWnd * m_pWndDynTab;
	public:
		CDynDropTarget()
			: m_pWndDynTab( NULL )
		{
		}
		virtual DROPEFFECT OnDragEnter(
			CWnd * pWnd,
			COleDataObject * pDataObject,
			DWORD dwKeyState,
			CPoint point
			)
		{
			ASSERT_VALID( this );
			return OnDragOver( pWnd, pDataObject, dwKeyState, point );
		}
		virtual DROPEFFECT OnDragOver(
			CWnd * pWnd,
			COleDataObject * pDataObject,
			DWORD dwKeyState,
			CPoint point
			);
		friend class CExtDynTabWnd;
	}; // class CDynDropTarget

	CDynDropTarget m_oleDropTarget;

public:

	CExtDynTabWnd();
	
	DECLARE_DYNCREATE( CExtDynTabWnd );

#ifdef _DEBUG
	
	virtual void AssertValid() const
	{
		CExtTabWnd::AssertValid();
	}
	
	virtual void Dump( CDumpContext & dc ) const
	{
		CExtTabWnd::Dump( dc );
	}

#endif

protected:

	virtual CExtCmdIcon * OnTabWndQueryItemIcon(
		const TAB_ITEM_INFO * pTii
		) const;
	virtual __EXT_MFC_SAFE_LPCTSTR OnTabWndQueryItemText(
		const TAB_ITEM_INFO * pTii
		) const;

	virtual bool OnTabWndStartDrag( LONG nIndex );

	virtual bool OnTabWndClickedItem(
		LONG nItemIndex,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);
	virtual bool OnTabWndSelectionChange(
		LONG nOldItemIndex,
		LONG nNewItemIndex,
		bool bPreSelectionTest
		);

	virtual void OnTabWndDrawItem(
		CDC & dc,
		CRect & rcTabItemsArea,
		LONG nItemIndex,
		TAB_ITEM_INFO * pTii,
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
		CExtCmdIcon * pIcon
		);
	virtual bool OnTabWndProcessDrag( 
		LONG nIndex 
		)
	{
		nIndex;
		return false;
	}

	LONG m_nSavedIndex;

public:

	LONG FindControlBar(
		const CExtControlBar * pBar
		) const
	{
		ASSERT_VALID( this );
		ASSERT_VALID( pBar );
		LPARAM lParam =
			reinterpret_cast < LPARAM > ( pBar );
		LONG nIndex = ItemFindByLParam( lParam, -1, true, true );
		return nIndex;
	}

	void SaveIndex( LONG nIndex = -1 )
	{
		ASSERT_VALID( this );
		m_nSavedIndex = nIndex;
	}
	void SaveIndex( CExtControlBar * pBar )
	{
		ASSERT_VALID( this );
		ASSERT_VALID( pBar );
		m_nSavedIndex = FindControlBar( pBar );
		ASSERT( m_nSavedIndex >= 0 && m_nSavedIndex < ItemGetCount() );
	}
	void RestoreIndex( CExtControlBar * pBar )
	{
		ASSERT_VALID( this );
		ASSERT_VALID( pBar );
		ASSERT( pBar->GetParent()->GetParent() == GetParent() );

		ASSERT( m_nSavedIndex >= 0 && m_nSavedIndex <= ItemGetCount() );
		LONG nExistingIndex = FindControlBar( pBar );
		if( nExistingIndex != m_nSavedIndex )
		{
			if( nExistingIndex >= 0 )
				ItemRemove( nExistingIndex, 1, false );
			ASSERT( m_nSavedIndex >= 0 && m_nSavedIndex <= ItemGetCount() );
			InsertControlBar( pBar, m_nSavedIndex );
		}
		m_nSavedIndex = -1L;
		UpdateTabWnd( true );
	}

	LONG SelectControlBar( CExtControlBar * pBar );
	
	void InsertControlBar(
		CExtControlBar * pBar,
		INT nPos = -1, // default - append
		bool bTempOnDrag = false, // is tab item temporary (when dragging)
		bool bUpdateTabWnd = false
		);
	void RemoveTemporaryItems(
		bool bUpdateTabWnd = false
		);

	CExtControlBar * GetBarAt(
		LONG nIndex,
		bool bExcludeTemporary
		);

	LONG ReindexPosCalc( CPoint ptCursor );

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtDynTabWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtDynTabWnd)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CExtDynTabControlBar;
}; // class CExtDynTabWnd

/////////////////////////////////////////////////////////////////////////
// CExtDynTabControlBar window

class __PROF_UIS_API CExtDynTabControlBar : public CExtDynControlBar
{
protected:
	CExtDynTabWnd * m_pWndTabSwitcher;
	virtual CExtDynTabWnd * _OnQueryTabSwitcherWndInstance();
	const CExtDynTabWnd * _OnQueryTabSwitcherWndInstance() const;
public:
	DECLARE_DYNCREATE( CExtDynTabControlBar );
	CExtDynTabControlBar();
	virtual ~CExtDynTabControlBar();

#ifdef _DEBUG
	
	virtual void AssertValid() const
	{
		CExtDynControlBar::AssertValid();
	}
	
	virtual void Dump( CDumpContext & dc ) const
	{
		CExtDynControlBar::Dump( dc );
	}

#endif

public:
	virtual bool OnSwitcherSelectionChange(
		LONG nOldItemIndex,
		LONG nNewItemIndex,
		bool bPreSelectionTest
		);
	virtual bool IsBarWithGripper(
		bool * pbGripperAtTop = NULL,
		bool * pbTextOnGripper = NULL
		) const;
	virtual void OnShowControlBarChain(
		bool bShow,
		bool bDelay,
		bool bOnlyOneBar = false
		);

	void InsertTemporaryItemsFrom(
		CExtControlBar * pBar,
		LONG nIndex,
		bool bUpdateTabWnd
		);

	void InsertBar( 
		CExtControlBar * pBar,
		LONG nIndex,
		bool bUpdateTabWnd
		);
	
	void UpdateBarVisibility(
		CExtControlBar * pBar
		);


	LONG FindControlBar(
		const CExtControlBar * pBar
		) const
	{
		ASSERT_VALID( this );
		return _OnQueryTabSwitcherWndInstance()->FindControlBar( pBar );
	}

	void SaveIndex( LONG nIndex = -1 )
	{
		ASSERT_VALID( this );
		_OnQueryTabSwitcherWndInstance()->SaveIndex( nIndex );
	}

	void SaveIndex( CExtControlBar * pBar )
	{
		ASSERT_VALID( this );
		_OnQueryTabSwitcherWndInstance()->SaveIndex( pBar );
	}

	void RestoreIndex( CExtControlBar * pBar )
	{
		ASSERT_VALID( this );
		_OnQueryTabSwitcherWndInstance()->RestoreIndex( pBar );
	}

	void SelectControlBar( CExtControlBar * pBar )
	{
		ASSERT_VALID( this );
		_OnQueryTabSwitcherWndInstance()->SelectControlBar( pBar );
	}

	void InsertControlBar(
		CExtControlBar * pBar,
		INT nPos = -1, // default - append
		bool bTempOnDrag = false, // is tab item temporary (when dragging)
		bool bUpdateTabWnd = false
		)
	{
		ASSERT_VALID( this );
		_OnQueryTabSwitcherWndInstance()->InsertControlBar( pBar, nPos, bTempOnDrag, bUpdateTabWnd );
	}

	void RemoveTemporaryItems(
		bool bUpdateTabWnd = false
		)
	{
		ASSERT_VALID( this );
		_OnQueryTabSwitcherWndInstance()->RemoveTemporaryItems( bUpdateTabWnd );
	}

	CExtControlBar * GetBarAt(
		LONG nIndex,
		bool bExcludeTemporary
		)
	{
		ASSERT_VALID( this );
		return _OnQueryTabSwitcherWndInstance()->GetBarAt( nIndex, bExcludeTemporary );
	}

	LONG ReindexPosCalc( CPoint ptCursor )
	{
		ASSERT_VALID( this );
		return _OnQueryTabSwitcherWndInstance()->ReindexPosCalc( ptCursor );
	}

	void InvalidateSwitcher()
	{
		ASSERT_VALID( this );
		CExtDynTabWnd * pWndTabSwitcher = _OnQueryTabSwitcherWndInstance();
		if(		pWndTabSwitcher->GetSafeHwnd() == NULL
			||	( ! pWndTabSwitcher->IsWindowVisible() )
			)
			return;
		pWndTabSwitcher->Invalidate();
		//pWndTabSwitcher->UpdateWindow();
	}

	virtual bool NcButtons_HandleClick(
		CExtBarNcAreaButton * pNcAreaButton,
		CPoint point,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual void NcButtons_PostClick(
		CExtBarNcAreaButton * pNcAreaButton,
		CPoint point,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleQueryVisibility(
		bool & bButtonIsVisible,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleDraw(
		CDC & dc,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleHitTest(
		UINT & nExternalHT,
		CPoint point,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleQueryCursor(
		HCURSOR & hExternalCursor,
		CPoint point,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleReposition(
		CExtBarNcAreaButton * pPrevBtn,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);
	virtual bool NcButtons_HandleToolHitTest(
		__EXT_MFC_INT_PTR & hExternalToolHT,
		CPoint point,
		TOOLINFO * pTI,
		CExtBarNcAreaButton * pNcAreaButton,
		CExtControlBar * pBarEventSource,
		CExtControlBar * pBarActiveInContainer
		);

	void SyncSwitcher();
	void RemoveSelFromSwitcher();
	void RemoveControlBar( CExtControlBar * pBar );

	bool IsSwitcherVisible()
	{
		ASSERT_VALID( this );
		CExtDynTabWnd * pWndTabSwitcher = _OnQueryTabSwitcherWndInstance();
		ASSERT_VALID( pWndTabSwitcher );
		if( pWndTabSwitcher->GetSafeHwnd() == NULL )
			return false;
		bool bVisible = (pWndTabSwitcher->GetStyle() & WS_VISIBLE) ? true : false;
		return bVisible;
	}

	void GetSwitcherWindowRect( RECT * pRcSwitcherWnd ) const
	{
		ASSERT_VALID( this );
		ASSERT( pRcSwitcherWnd != NULL );
		_OnQueryTabSwitcherWndInstance()->GetWindowRect( pRcSwitcherWnd );
	}

	LONG GetSwitcherItemCount() const
	{
		ASSERT_VALID( this );
		return _OnQueryTabSwitcherWndInstance()->ItemGetCount();
	}
	LONG GetSwitcherVisibleItemCount() const
	{
		ASSERT_VALID( this );
		return _OnQueryTabSwitcherWndInstance()->ItemGetVisibleCount();
	}

	LONG SetSwitcherSelection(
		LONG nSelIndex,
		bool bEnsureVisible = false,
		bool bUpdateTabWnd = false
		)
	{
		ASSERT_VALID( this );
		CExtDynTabWnd * pWndTabSwitcher = _OnQueryTabSwitcherWndInstance();
		ASSERT_VALID( pWndTabSwitcher );
		LONG nItemCount = pWndTabSwitcher->ItemGetCount();
		LONG nValidSelIndex = 0;
		LONG nItemIndex = 0L;
		for( ; nItemIndex < nItemCount; nItemIndex ++ )
		{
			CExtTabWnd::TAB_ITEM_INFO * pTII = pWndTabSwitcher->ItemGet( nItemIndex );
			ASSERT_VALID( pTII );
			if( ! pTII->VisibleGet() )
				continue;
			if( nValidSelIndex == nSelIndex )
				break;
			nValidSelIndex ++;
		} // for( ; nItemIndex < nItemCount; nItemIndex ++ )
		if( nItemIndex != nItemCount )
			nSelIndex = nItemIndex;
		return pWndTabSwitcher->SelectionSet( nSelIndex, bEnsureVisible, bUpdateTabWnd );
	}
	LONG GetSwitcherSelection() const
	{
		ASSERT_VALID( this );
		return _OnQueryTabSwitcherWndInstance()->SelectionGet();
	}

	void SetSwitcherRedraw( bool bRedraw )
	{
		_OnQueryTabSwitcherWndInstance()->SetRedraw( bRedraw ? TRUE : FALSE );
	}

	virtual bool IsCloseOnlyOneBarInTabGroup() const;

protected:
	virtual bool _CanDockToTabbedContainers(
		CExtControlBar * pDestBar
		) const;
	virtual CExtDynTabControlBar * _GetNearestTabbedContainer();

	void _InsertTemporaryItemsFromImpl(
		CExtControlBar * pBar,
		LONG & nIndex
		);
	void _InsertBarImpl( 
		CExtControlBar * pBar,
		LONG & nIndex
		);

	INT _CalcTabHeight() const;

	virtual CSize _CalcLayoutMinSize() const;

	virtual INT _CalcDesiredMinHW() const;
	virtual INT _CalcDesiredMinVH() const;
	virtual CSize _CalcDesiredMinFloatedSize() const;

	virtual INT _GripperWidthAtLeftGet() const;
	virtual INT _GripperHeightAtTopGet() const;

	virtual CSize CalcDynamicLayout(
		int nLength,
		DWORD nMode
		);

	virtual CExtDockDynBar * OnCreateDynamicDockBarObject();

public:
	virtual void OnRepositionSingleChild(
		int cx = -1, // if negative - get from client area
		int cy = -1,
		bool bRedraw = true
		);
	virtual void OnGetBarCaptionText(
		e_bar_caption_text_t eBCT,
		CExtSafeString & strCaptionText
		) const;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtDynTabControlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtDynTabControlBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CExtDynTabWnd;

}; // class CExtDynTabControlBar

/////////////////////////////////////////////////////////////////////////
// CExtDynAutoHideArea window

class __PROF_UIS_API CExtDynAutoHideArea : public CExtTabWnd
{
protected:
	class __PROF_UIS_API CDynDropTarget : public COleDropTarget
	{
		CExtDynAutoHideArea * m_pWndAutoHideArea;
		CExtDynAutoHideSlider * m_pWndSlider;
	public:
		CDynDropTarget()
			: m_pWndAutoHideArea( NULL )
			, m_pWndSlider( NULL )
		{
		}
		virtual DROPEFFECT OnDragEnter(
			CWnd * pWnd,
			COleDataObject * pDataObject,
			DWORD dwKeyState,
			CPoint point
			)
		{
			ASSERT_VALID( this );
			return OnDragOver( pWnd, pDataObject, dwKeyState, point );
		}
		virtual DROPEFFECT OnDragOver(
			CWnd * pWnd,
			COleDataObject * pDataObject,
			DWORD dwKeyState,
			CPoint point
			);
		friend class CExtDynAutoHideArea;
	}; // class CDynDropTarget

	CDynDropTarget m_oleDropTarget;

	CExtDynAutoHideSlider * m_pWndSlider;
	bool m_bActivationLocked:1, m_bInsertMode:1;
	CRect m_rcLastPos;

public:
	static bool g_bEmbeddedMode;
	bool m_bCancelNearestActivateAnimation:1;

	CExtDynAutoHideArea();
	
	DECLARE_DYNCREATE( CExtDynAutoHideArea );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual void OnCustomizeModeEnter();
	virtual void OnCustomizeModeLeave();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#ifdef _DEBUG
	
	virtual void AssertValid() const
	{
		CExtTabWnd::AssertValid();
		ASSERT( m_bReflectParentSizing );
	}
	
	virtual void Dump( CDumpContext & dc ) const
	{
		CExtTabWnd::Dump( dc );
	}

#endif

	virtual DWORD GetTabWndStyle() const;
	virtual BOOL Create(
		CFrameWnd * pParentWnd,
		CExtDynAutoHideSlider * pWndSlider,
		UINT nDockedDockBarID
		);
	void AdjustOrder();

	void AppendControlBar(
		CExtControlBar * pBar,
		bool bForceNoShow
		);
	void RemoveControlBar(
		CExtControlBar * pBar,
		bool bForceNoShow
		);

	LONG FindControlBar(
		const CExtControlBar * pBar
		) const
	{
		ASSERT_VALID( this );
		ASSERT_VALID( pBar );
		LPARAM lParam =
			reinterpret_cast < LPARAM > ( pBar );
		LONG nIndex = ItemFindByLParam( lParam, -1, true, true );
		return nIndex;
	}

	CExtDynAutoHideSlider * GetAutoHideSlider();

protected:

	virtual void OnTabWndDrawItem(
		CDC & dc,
		CRect & rcTabItemsArea,
		LONG nItemIndex,
		TAB_ITEM_INFO * pTii,
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
		CExtCmdIcon * pIcon
		);

	virtual void OnTabWndItemInsert(
		LONG nItemIndex,
		TAB_ITEM_INFO * pTii
		);

	virtual CExtCmdIcon * OnTabWndQueryItemIcon(
		const TAB_ITEM_INFO * pTii
		) const;
	virtual __EXT_MFC_SAFE_LPCTSTR OnTabWndQueryItemText(
		const TAB_ITEM_INFO * pTii
		) const;

	virtual bool OnTabWndClickedItem(
		LONG nItemIndex,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);
	virtual bool OnTabWndSelectionChange(
		LONG nOldItemIndex,
		LONG nNewItemIndex,
		bool bPreSelectionTest
		);

	virtual void OnTabWndSyncVisibility();

	virtual void OnTabWndMeasureItemAreaMargins(
		LONG & nSpaceBefore,
		LONG & nSpaceAfter,
		LONG & nSpaceOver
		);

public:
	static CExtDynAutoHideArea * stat_FindAreaOfBar(
		CExtControlBar * pBar,
		LONG * p_nSelIdx = NULL
		);
	static void stat_FindAreas(
		CFrameWnd * pDockSite,
		CExtDynAutoHideArea ** ppAreaTop,
		CExtDynAutoHideArea ** ppAreaBottom,
		CExtDynAutoHideArea ** ppAreaLeft,
		CExtDynAutoHideArea ** ppAreaRight
		);
	static void stat_FindAreas(
		HWND hWndStartSearch,
		CExtDynAutoHideArea ** ppAreaTop,
		CExtDynAutoHideArea ** ppAreaBottom,
		CExtDynAutoHideArea ** ppAreaLeft,
		CExtDynAutoHideArea ** ppAreaRight
		);
	bool _CanActivate();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtDynAutoHideArea)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtDynAutoHideArea)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	friend class CExtDynAutoHideSlider;
	friend class CExtControlBar;
	friend class CExtBarNcAreaButton;
}; // class CExtDynAutoHideArea

/////////////////////////////////////////////////////////////////////////
// CExtDynAutoHideSlider window

class __PROF_UIS_API CExtDynAutoHideSlider
	: public CWnd
	, CExtPmBridge
{
protected:
	HWND m_hWndAnimateSrc, m_hWndBar;
public:
	CExtDynAutoHideArea * _GetWndAnimateSrc();
	CExtControlBar * _GetBar();
protected:
	//CExtDynAutoHideArea * m_pWndAnimateSrc;
	//CExtControlBar * m_pBar;
	CSize m_sizeControlBar;
	bool m_bActiveState:1;
	bool m_bWndActive:1;
	bool m_bDeactivateTesting:1;
	bool m_bAnimation:1;
	UINT m_nResizingMode;
	INT m_nResizingStart, m_nResizingMetric;
	CExtBarNcAreaButton
		* m_pLastHitTestM,
		* m_pLastHitTestT,
		* m_pLastHitTestB;
	CToolTipCtrl m_wndToolTip;
	CRect m_rcMax;
	
	void _AnalyzeButtonsHover();
	
	static HHOOK g_hMouseHook;
	static HHOOK g_hKeyboardHook;
	static CExtDynAutoHideSlider * g_pHookTarget;

	static LRESULT CALLBACK _HookMouseProc(
		int nCode,      // hook code
		WPARAM wParam,  // message identifier
		LPARAM lParam   // mouse coordinates
		);
	static LRESULT CALLBACK _HookKeyboardProc(
		int nCode,      // hook code
		WPARAM wParam,  // virtual-key code
		LPARAM lParam   // keystroke-message information
		);
	bool _HookMouseClick(
		UINT nMkCode,
		bool bDown,
		bool bNcArea,
		CPoint ptScreen
		);
	bool _HookMouseMove(
		bool bNcArea,
		CPoint ptScreen
		);
	bool _HookKeyEvent(
		UINT nChar,
		UINT nRepCnt,
		UINT nFlags
		);

public:

	CExtDynAutoHideSlider();
	virtual ~CExtDynAutoHideSlider();
	
	DECLARE_DYNCREATE( CExtDynAutoHideSlider );
	DECLARE_CExtPmBridge_MEMBERS( CExtDynAutoHideSlider );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	virtual void OnCustomizeModeEnter(
		CExtDynAutoHideArea * pWndAutoHideArea
		);
	virtual void OnCustomizeModeLeave(
		CExtDynAutoHideArea * pWndAutoHideArea
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#ifdef _DEBUG
	
	virtual void AssertValid() const
	{
		CWnd::AssertValid();
	}
	
	virtual void Dump( CDumpContext & dc ) const
	{
		CWnd::Dump( dc );
	}

#endif

	CExtControlBar * GetBar();
	const CExtControlBar * GetBar() const;
	static bool RegisterAutohideSliderWndClass();

	virtual BOOL Create(
		CFrameWnd * pParentWnd
		);

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	void Activate(
		CExtControlBar * pBar,
		bool bActivate,
		bool bAnimate,
		bool bWait
		);

	void CalcInnerNcRects(
		RECT * pRcInnerArea,
		RECT * pRcDrawResizer,
		RECT * pRcDrawGripper
		);
	void CalcNcOffsets(
		RECT * pRcNcSizes
		);
	// for command hit testing (used for automatic tooltips)
	virtual __EXT_MFC_INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet() const;
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtDynAutoHideSlider)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtDynAutoHideSlider)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	friend class CExtDynAutoHideArea;
	friend class CExtDynAutoHideArea::CDynDropTarget;
	friend class CExtControlBar;
	friend class CExtBarNcAreaButton;

}; // class CExtDynAutoHideSlider

#define __AUTOHIDE_SLIDER_WNDCLASS _T("ProfUIS-AutohideSlider")

#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

class CExtDynamicBarSite;
class CExtDynamicBarNcAreaButtonMenu;

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicTabbedControlBar

class __PROF_UIS_API CExtDynamicTabbedControlBar : public CExtDynTabControlBar
{
	CExtDynamicBarSite * m_pDBS;
public:
	DECLARE_DYNCREATE( CExtDynamicTabbedControlBar );
	CExtDynamicTabbedControlBar();
	virtual ~CExtDynamicTabbedControlBar();
	CExtDynamicBarSite * GetBarSite();
	virtual void OnNcAreaButtonsReinitialize();
protected:
	virtual bool _CanDockToTabbedContainers(
		CExtControlBar * pDestBar
		) const;
	virtual bool _CanDockLTRB(
		CExtControlBar * pDestBar
		) const;
	friend class CExtDynamicBarNcAreaButtonMenu;
}; // class CExtDynamicTabbedControlBar

#endif // (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H
