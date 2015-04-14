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

#if (!defined __EXT_CONTROLS_COMMON_H)
#define __EXT_CONTROLS_COMMON_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_EXTCONTENTEXPANDWND_H)
	#include <ExtContentExpandWnd.h>
#endif

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __EXT_EDIT_H)
	#include <ExtEdit.h>
#endif

#if (!defined __EXT_COMBO_BOX_H)
	#include <ExtComboBox.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_HOOK_H)
	#include "../Src/ExtHook.h"
#endif

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXT_NC_FRAME_H)
	#include <ExtNcFrame.h>
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if ( ! defined __EXT_MFC_NO_LIST_VIEW_CTRL )

/////////////////////////////////////////////////////////////////////////////
// CExtListCtrlDataSortOrder

class __PROF_UIS_API CExtListCtrlDataSortOrder : public CObject
{
	void _AssignFromOther(
		const CExtListCtrlDataSortOrder & other
		);
public:
	DECLARE_SERIAL( CExtListCtrlDataSortOrder );
	struct __PROF_UIS_API COLUMN_INFO
	{
		LONG m_nColNo;
		bool m_bAscending:1;
		COLUMN_INFO(
			LONG nColNo = -1L,
			bool bAscending = true
			);
		COLUMN_INFO( const COLUMN_INFO & other );
		COLUMN_INFO & operator = ( const COLUMN_INFO & other );
		void Serialize( CArchive & ar );
		int Cmp( const COLUMN_INFO & other ) const;
		bool operator == ( const COLUMN_INFO & other ) const;
		bool operator != ( const COLUMN_INFO & other ) const;
		bool operator <= ( const COLUMN_INFO & other ) const;
		bool operator >= ( const COLUMN_INFO & other ) const;
		bool operator < ( const COLUMN_INFO & other ) const;
		bool operator > ( const COLUMN_INFO & other ) const;
	}; // struct COLUMN_INFO
	CArray < COLUMN_INFO, COLUMN_INFO & > m_arrItems;
	CExtListCtrlDataSortOrder();
	CExtListCtrlDataSortOrder(
		const CExtListCtrlDataSortOrder & other
		);
	virtual ~CExtListCtrlDataSortOrder();
	CExtListCtrlDataSortOrder & operator =(
		const CExtListCtrlDataSortOrder & other
		);
	bool operator == (
		const CExtListCtrlDataSortOrder & other
		) const;
	bool operator != (
		const CExtListCtrlDataSortOrder & other
		) const;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual void Serialize( CArchive & ar );
	bool ItemsUnique() const;
	void MakeItemsUnique();
	bool IsEmpty() const;
	void Empty();
	LONG GetColPos( LONG nColNo ) const;
	void SetupOrder(
		const CExtListCtrlDataSortOrder & _gdsoUpdate,
		bool bInvertIntersectionSortOrder
		);
	void UpdateIndices(
		LONG nColNo,
		LONG nInsertRemoveCount,
		bool bInsert
		);
	void SwapDroppedSeries(
		LONG nColNoSrc,
		LONG nColNoDropBefore
		);
}; // class CExtListCtrlDataSortOrder

/////////////////////////////////////////////////////////////////////////////
// CExtHeaderCtrl

#define __EXT_MFC_HDI_FILTER              0x0100
#define __EXT_MFC_HDI_STATE               0x0200

class __PROF_UIS_API CExtHeaderCtrl
	: public CHeaderCtrl
	, public CExtPmBridge
{
public:
    DECLARE_DYNCREATE( CExtHeaderCtrl );
	IMPLEMENT_CExtPmBridge_MEMBERS( CExtHeaderCtrl );

	CExtHeaderCtrl();
	virtual ~CExtHeaderCtrl();

	virtual void OnNotifyHdnTrack( HD_NOTIFY & _data );

	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);

	struct __PROF_UIS_API EXTENDED_ITEM_DATA
	{
		CExtCmdIcon m_iconItem, m_iconButton;
		CExtSafeString m_strTipTextItem, m_strTipTextButton;
		INT m_nExtentMin, m_nExtentMax;
		bool m_bDisableDnd:1;
		EXTENDED_ITEM_DATA();
		EXTENDED_ITEM_DATA( const EXTENDED_ITEM_DATA & other );
		virtual ~EXTENDED_ITEM_DATA();
		void AssignFromOther( const EXTENDED_ITEM_DATA & other );
		EXTENDED_ITEM_DATA & operator = ( const EXTENDED_ITEM_DATA & other );
		bool CanBeResized() const;
	}; /// struct EXTENDED_ITEM_DATA

public:
	static const UINT g_nMsgHeaderButtonClick;
	bool m_bShowTipsOverHeader:1;
	CExtListCtrlDataSortOrder m_sortOrder;
	INT m_nDividerHalfExtent;
	static bool g_bEnableOnIdleCalls;
protected:
	mutable CArray < EXTENDED_ITEM_DATA, EXTENDED_ITEM_DATA & > m_arrExtendedData;
public:
	INT m_nAdvancedTipStyle; // CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON
	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet() const;
	virtual void OnAdvancedPopupMenuTipWndDisplay(
		CExtPopupMenuTipWnd & _ATTW,
		const RECT & rcExcludeArea,
		__EXT_MFC_SAFE_LPCTSTR strToolTipText
		) const;
	INT ColNo2VisualIndex( INT nColNo ) const; // convert column number to visual index
	INT VisualIndex2ColNo( INT nIndex ) const; // convert visual index to column number
	virtual INT HitTestHeaderItem(
		const POINT & ptClient,
		bool * p_bOnButton = NULL,
		bool * p_bOnDividerAtRight = NULL,
		bool bComputeZeroWidthItem = true
		) const;
	virtual void OnEraseHeaderBackground(
		CDC & dc,
		const CRect & rcClient
		) const;
	virtual CSize OnCalcHeaderItemSize(
		INT nColNo
		) const;
	virtual void OnCalcHeaderItemLayout(
		INT nColNo,
		const CRect & _rcItemEntire,
		CRect & _rcItem,
		CRect & _rcIcon,
		CRect & _rcText,
		CRect & _rcSortArrow,
		CRect & _rcButton,
		CRect & _rcButtonIcon,
		bool * p_bSorted = NULL,
		bool * p_bSortedAscending = NULL
		) const;
	virtual void OnPaintHeaderItem(
		CDC & dc,
		const HD_ITEM & _item,
		INT nColNo,
		const CRect & _rcItemEntire,
		bool bDND = false
		) const;
	virtual void OnHeaderItemClick(
		INT nColNo,
		INT nMouseButton // 0 - left button, 1 - right button, 2 - middle button
		);
	virtual void OnHeaderItemButtonClick( INT nColNo );
	virtual void OnHeaderItemDividerAtRightDoubleClick( INT nColNo );

	//{{AFX_VIRTUAL(CExtHeaderCtrl)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	virtual LRESULT _DoInvokeParentWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CPoint m_ptScreenPressing;
	INT m_nHoverColNo, m_nHoverIndex, m_nPressingColNo, m_nPressingIndex, m_nHelperInitialResizingExtent;
	bool m_bOnButton:1, m_bOnDividerAtRight:1, m_bCancelingActions:1;
	virtual void _CancelActions();
	bool _DoSetCursor();
	virtual bool _DoSetCursor( const POINT ptScreen ); // CPoint(-32767,-32767) means invoke ::GetCursorPos()
	virtual bool _DndIsAllowed(
		INT nColNo = -1
		) const;
	virtual CPoint _DndGetStartOffset() const;
	virtual INT _DndCalcOuterDropTarget( CPoint point ) const;
	virtual void _DndDo(
		INT nColNo,
		CPoint ptStartDrag,
		bool bNotify = true // send HDN_BEGINDRAG and HDN_ENDDRAG messages
		);
	virtual void _DndSwap( INT nColNo, INT nColNoToDropBefore );
	virtual bool _PaintExpandedItemContent(
		CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA & picd
		) const;
public:
	HCURSOR m_hCursorDefault, m_hCursorResizingH1, m_hCursorResizingH2, m_hCursorOuterDragOK, m_hCursorOuterDragCancel;
	virtual void SortRulesGet(
		CExtListCtrlDataSortOrder & _sortOrder
		) const;
	virtual void SortRulesSet(
		const CExtListCtrlDataSortOrder & _sortOrder
		);
	const EXTENDED_ITEM_DATA & ExtendedItemDataGet( INT nColNo ) const;
	virtual EXTENDED_ITEM_DATA & ExtendedItemDataGet( INT nColNo );
	virtual INT HeaderItemExtentGet( INT nColNo ) const;
	virtual void HeaderItemExtentSet(
		INT nColNo,
		INT nExtent,
		bool bRedraw = true
		);

protected:
	//{{AFX_MSG(CExtHeaderCtrl)
	afx_msg void OnPaint();
	afx_msg void OnWindowPosChanging( WINDOWPOS FAR * lpwndpos );
	afx_msg BOOL OnEraseBkgnd( CDC * pDC );
	afx_msg BOOL OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDblClk( UINT nFlags, CPoint point );
	afx_msg void OnMButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnMButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnMButtonDblClk( UINT nFlags, CPoint point );
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged( CWnd * pWnd );
	//}}AFX_MSG
	afx_msg LRESULT _OnPaintExpandedItemContent( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
}; /// class CExtHeaderCtrl

/////////////////////////////////////////////////////////////////////////////
// CExtListCtrl

class __PROF_UIS_API CExtListCtrl
	: public CListCtrl
	, public CExtPmBridge
	, public CExtHookSpy
{
public:
	DECLARE_DYNCREATE( CExtListCtrl );
	IMPLEMENT_CExtPmBridge_MEMBERS( CExtListCtrl );
	CExtListCtrl();
	virtual ~CExtListCtrl();
protected:
	CExtListCtrl( UINT, CWnd * ) { } // for CExtNCSB template class compilation only
	CExtListCtrl( __EXT_MFC_SAFE_LPCTSTR, CWnd * ) { } // for CExtNCSB template class compilation only
public:
	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);
	bool m_bSortEnabled:1, m_bSortEnabledMultiple:1,
		m_bColumnAutoSizingByData:1, m_bColumnAutoSizingByHeader:1,
	m_bSelectAllOnCtrlA:1;
protected:
	CExtHeaderCtrl m_wndHeaderCtrl;
	virtual bool _Init();
	virtual CExtHeaderCtrl & _OnQueryHeaderCtrl();
public:
	CExtHeaderCtrl & GetHeaderCtrl();
	const CExtHeaderCtrl & GetHeaderCtrl() const;
	void GetSelectedItemsList(
		CList < INT, INT > & _listSelectedItems, 
		bool bAddToTail = true
		);
	virtual bool VerifySortOrder(
		const CExtListCtrlDataSortOrder & _sortOrder
		) const;
	virtual INT CompareItems(
		const CExtListCtrlDataSortOrder & _sortOrder,
		INT nItemIndex1,
		INT nItemIndex2
		) const;
	virtual void SwapItems(
		INT nItemIndex1,
		INT nItemIndex2
		);
	bool SortItems(
		INT nItemIndexLow = 0,
		INT nItemIndexHigh = -1
		);
	virtual bool SortItems(
		const CExtListCtrlDataSortOrder & _sortOrder,
		INT nItemIndexLow = 0,
		INT nItemIndexHigh = -1
		);
	void ModifyExtendedStyle( DWORD dwRemove, DWORD dwAdd );
	virtual void ColumnAutoSize(
		INT nColNo,
		bool bAutoSizeByData = true,
		bool bAutoSizeByHeader = true
		);
	INT ColumnExtentGet( INT nColNo ) const;
	void ColumnExtentSet( INT nColNo, INT nExtent );
	virtual void StateSerialize(
		CArchive & ar,
		bool bExtents = true,
		bool bColumnOrder = true,
		bool bSortOrder = true,
		bool bListViewMode = true
		);
	virtual bool StateLoad(
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
		HKEY hKeyRoot = HKEY_CURRENT_USER,
		bool bExtents = true,
		bool bColumnOrder = true,
		bool bSortOrder = true,
		bool bListViewMode = true
		);
	virtual bool StateSave(
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
		HKEY hKeyRoot = HKEY_CURRENT_USER,
		bool bExtents = true,
		bool bColumnOrder = true,
		bool bSortOrder = true,
		bool bListViewMode = true
		);
protected:
	bool m_bReadOnlyLabels:1;
public:
	bool ReadOnlyLabelsGet() const;
	void ReadOnlyLabelsSet( bool bReadOnlyLabels );
	void SelectedItemsListGet(
		CList < INT, INT > & _listSelectedItems,
		bool bAddToTail = true
		) const;
	void SelectedItemsListSet(
		const CList < INT, INT > & _listSelectedItems
		);
	bool IsItemSelected(
		INT nItemIndex
		) const;
	void SelectItem(
		INT nItemIndex,
		bool bSelect = true
		);
	INT FocusedItemGet() const;
	INT FocusedItemSet( // returns previous focused item index
		INT nItemIndex,
		bool bSelectFocusedItem = true,
		bool bUnselectOtherItems = true
		);
	virtual void OnNotifyHdnTrack( CExtHeaderCtrl & wndHeader, HD_NOTIFY & _data );

	//{{AFX_VIRTUAL(CExtListCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CExtListCtrl)
	afx_msg void OnHeaderItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnDeleteItem( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnDeleteAllItems( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnBeginLabelEdit( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnBeginDrag( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnEndDrag( NMHDR * pNMHDR, LRESULT * pResult );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; /// class CExtListCtrl


#if ( _MFC_VER == 0x700 ) && (! defined __EXT_PROFUIS_STATIC_LINK)
	template class __PROF_UIS_API CExtNCSB_Impl < CExtListCtrl >;
#endif

template < > class
//#if ( _MFC_VER != 0x700 )
	__PROF_UIS_API
//#endif
	CExtNCSB < CExtListCtrl > : public CExtNCSB_Impl < CExtListCtrl >
{
public:
	CExtNCSB(
		bool bNcsbDelayedInitialization = false,
		bool bNcsbForceMiddleContainerMode = false
		);
	virtual ~CExtNCSB();
	virtual void AdjustScrollMetrics();
protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
}; // template < > class CExtNCSB < CExtListCtrl >

//#if ( _MFC_VER == 0x700 ) && (! defined __EXT_PROFUIS_STATIC_LINK)
//	template class __PROF_UIS_API CExtNCSB < CExtListCtrl >;
//#endif

#endif //  ( ! defined __EXT_MFC_NO_LIST_VIEW_CTRL )

#if ( ! defined __EXT_MFC_NO_TREE_VIEW_CTRL )

/////////////////////////////////////////////////////////////////////////////
// CExtTreeCtrl

#define __EXT_TREE_CTRL_BORDER_GAP 1

// hit test flags
#define __EXT_TVHT_NOWHERE            0x0001 // nowhere and not on any item
#define __EXT_TVHT_ONITEMICON         0x0002 // icon
#define __EXT_TVHT_ONITEMLABEL        0x0004 // item label
#define __EXT_TVHT_ONITEM             ( __EXT_TVHT_ONITEMICON | __EXT_TVHT_ONITEMLABEL | __EXT_TVHT_ONITEMSTATEICON )
#define __EXT_TVHT_ONITEMINDENT       0x0008 // tree lines area
#define __EXT_TVHT_ONITEMBUTTON       0x0010 // box
#define __EXT_TVHT_ONITEMRIGHT        0x0020 // at right
#define __EXT_TVHT_ONITEMSTATEICON    0x0040

#define __EXT_TVHT_ONCHECKBOX         0x0080 // check box
#define __EXT_TVHT_ONCONTROL          0x0100 // in-place control area

// styles
#define __EXT_TREE_MULTIPLE_SELECTION				0x80000000
#define __EXT_TREE_SELECT_DISABLE_ITEMS				0x40000000
#define __EXT_TREE_FOCUS_DISABLE_ITEMS				0x20000000
#define __EXT_TREE_SELECT_COLLAPSED_ITEMS			0x10000000
#define __EXT_TREE_SUBTRACT_SELECTION				0x08000000
#define __EXT_TREE_HOVER_DISABLE_ITEMS				0x04000000
#define __EXT_TREE_AUTO_DISABLE_ITEMS_BY_PARENT		0x02000000
#define __EXT_TREE_USER_DEFINED_24					0x01000000
#define __EXT_TREE_USER_DEFINED_23					0x00800000
#define __EXT_TREE_USER_DEFINED_22					0x00400000
#define __EXT_TREE_USER_DEFINED_21					0x00200000
#define __EXT_TREE_USER_DEFINED_20					0x00100000
#define __EXT_TREE_USER_DEFINED_19					0x00080000
#define __EXT_TREE_USER_DEFINED_18					0x00040000
#define __EXT_TREE_USER_DEFINED_17					0x00020000
#define __EXT_TREE_USER_DEFINED_16					0x00010000
#define __EXT_TREE_USER_DEFINED_15					0x00008000
#define __EXT_TREE_USER_DEFINED_14					0x00004000
#define __EXT_TREE_USER_DEFINED_13					0x00002000
#define __EXT_TREE_USER_DEFINED_12					0x00001000
#define __EXT_TREE_USER_DEFINED_11					0x00000800
#define __EXT_TREE_USER_DEFINED_10					0x00000400
#define __EXT_TREE_USER_DEFINED_09					0x00000200
#define __EXT_TREE_USER_DEFINED_08					0x00000100
#define __EXT_TREE_USER_DEFINED_07					0x00000080
#define __EXT_TREE_USER_DEFINED_06					0x00000040
#define __EXT_TREE_USER_DEFINED_05					0x00000020
#define __EXT_TREE_USER_DEFINED_04					0x00000010
#define __EXT_TREE_USER_DEFINED_03					0x00000008
#define __EXT_TREE_USER_DEFINED_02					0x00000004
#define __EXT_TREE_USER_DEFINED_01					0x00000002
#define __EXT_TREE_USER_DEFINED_00					0x00000001

#define __EXT_TREE_DEFAULT_STYLES				0

// extended styles
#define __EXT_TREE_EX_HOVER_TREE_BOXES				0x80000000
#define __EXT_TREE_EX_HOVER_CHECK_BOXES				0x40000000
#define __EXT_TREE_EX_HOVER_RADIO_BUTTONS			0x20000000
#define __EXT_TREE_EX_USER_DEFINED_28				0x10000000
#define __EXT_TREE_EX_USER_DEFINED_27				0x08000000
#define __EXT_TREE_EX_USER_DEFINED_26				0x04000000
#define __EXT_TREE_EX_USER_DEFINED_25				0x02000000
#define __EXT_TREE_EX_USER_DEFINED_24				0x01000000
#define __EXT_TREE_EX_USER_DEFINED_23				0x00800000
#define __EXT_TREE_EX_USER_DEFINED_22				0x00400000
#define __EXT_TREE_EX_USER_DEFINED_21				0x00200000
#define __EXT_TREE_EX_USER_DEFINED_20				0x00100000
#define __EXT_TREE_EX_USER_DEFINED_19				0x00080000
#define __EXT_TREE_EX_USER_DEFINED_18				0x00040000
#define __EXT_TREE_EX_USER_DEFINED_17				0x00020000
#define __EXT_TREE_EX_USER_DEFINED_16				0x00010000
#define __EXT_TREE_EX_USER_DEFINED_15				0x00008000
#define __EXT_TREE_EX_USER_DEFINED_14				0x00004000
#define __EXT_TREE_EX_USER_DEFINED_13				0x00002000
#define __EXT_TREE_EX_USER_DEFINED_12				0x00001000
#define __EXT_TREE_EX_USER_DEFINED_11				0x00000800
#define __EXT_TREE_EX_USER_DEFINED_10				0x00000400
#define __EXT_TREE_EX_USER_DEFINED_09				0x00000200
#define __EXT_TREE_EX_USER_DEFINED_08				0x00000100
#define __EXT_TREE_EX_USER_DEFINED_07				0x00000080
#define __EXT_TREE_EX_USER_DEFINED_06				0x00000040
#define __EXT_TREE_EX_USER_DEFINED_05				0x00000020
#define __EXT_TREE_EX_USER_DEFINED_04				0x00000010
#define __EXT_TREE_EX_USER_DEFINED_03				0x00000008
#define __EXT_TREE_EX_USER_DEFINED_02				0x00000004
#define __EXT_TREE_EX_USER_DEFINED_01				0x00000002
#define __EXT_TREE_EX_USER_DEFINED_00				0x00000001

#define __EXT_TREE_EX_DEFAULT_STYLES				0

#define __EXT_TREE_CONTROL_TIMER_ID_TRACKHOVER		13

class __PROF_UIS_API CExtTreeCtrl
	: public CTreeCtrl
	, public CExtHookSpy
{
public:
	DECLARE_DYNCREATE( CExtTreeCtrl )

	CExtTreeCtrl();
	virtual ~CExtTreeCtrl();

	enum e_line_type_t
	{
		line_empty,
		line_corner,
		line_beam,
		line_branch
	};

	enum e_check_box_type_t
	{
		e_cbt_none,
		e_cbt_radio,
		e_cbt_check_box,
		e_cbt_tri_state_check_box,
	};

	enum e_item_control_type_t
	{
		__EICT_NONE,
		__EICT_EDIT,
		__EICT_DROPDOWN_COMBOBOX,
		__EICT_DROPLIST_COMBOBOX,
		__EICT_PROGRESS_BAR_THEMED,
		__EICT_PROGRESS_BAR_WITH_BOXES,
		__EICT_PROGRESS_BAR_SMOOTH,
		__EICT_PROGRESS_BAR_ENDLESS,
	};

	struct __PROF_UIS_API COMBOBOXITEMDATA_t
	{
		CExtSafeString m_strLabel;
		LPARAM m_lParam;
		COMBOBOXITEMDATA_t(
			__EXT_MFC_SAFE_LPCTSTR strLabel = NULL,
			LPARAM lParam = 0L
			);
		COMBOBOXITEMDATA_t( const COMBOBOXITEMDATA_t & other );
		virtual ~COMBOBOXITEMDATA_t();
		COMBOBOXITEMDATA_t & operator = ( const COMBOBOXITEMDATA_t & other );
	}; // struct COMBOBOXITEMDATA_t

	struct __PROF_UIS_API TREEITEMINFO_t
	{
		COLORREF
			m_clrTextNormal,	m_clrBackgroundNormal,
			m_clrTextSelected,	m_clrBackgroundSelected,
			m_clrTextDisabled,	m_clrBackgroundDisabled,
			m_clrTextHover;
		bool m_bSelected:1, m_bDisabled:1, m_bBold:1, m_bItalic:1, m_bUnderline:1;
		e_check_box_type_t m_eCheckBoxType;
		e_item_control_type_t m_eICT;
		INT m_nCheckBoxState; // 0-unchecked, 1-checked, 2-indeterminate
		CExtSafeString m_strRadioGroupName;
		INT m_nIconIndexCollapsed, m_nIconIndexExpanded;
		INT m_nProgressMin, m_nProgressMax, m_nProgressPos;
		LPARAM m_lParam;
		INT m_nControlWidth, m_nDropDownHeight;
		CExtSafeString m_strEditText;
		DWORD m_dwAdditionalControlStyles, m_dwAdditionalLabelEditorStyles;
	protected:
		CTypedPtrArray < CPtrArray, COMBOBOXITEMDATA_t * > m_arrComboBoxItems;
	public:
		TREEITEMINFO_t();
		TREEITEMINFO_t( const TREEITEMINFO_t & other );
		virtual ~TREEITEMINFO_t();
	protected:
		void Init();
		void AssignFromOther( const TREEITEMINFO_t & other );
	public:
		TREEITEMINFO_t & operator = ( const TREEITEMINFO_t & other );
		INT ComboBoxItem_GetCount() const;
		COMBOBOXITEMDATA_t * ComboBoxItem_GetAt( INT nIndex );
		const COMBOBOXITEMDATA_t * ComboBoxItem_GetAt( INT nIndex ) const;
		COMBOBOXITEMDATA_t & ComboBoxItem_GetRefAt( INT nIndex );
		const COMBOBOXITEMDATA_t & ComboBoxItem_GetRefAt( INT nIndex ) const;
		void ComboBoxItem_InsertAt(
			INT nIndex,
			COMBOBOXITEMDATA_t * pCID,
			bool bInsertClonedCopy // = true
			);
		void ComboBoxItem_InsertAt(
			INT nIndex,
			const COMBOBOXITEMDATA_t & _CID
			);
		void ComboBoxItem_Add(
			COMBOBOXITEMDATA_t * pCID,
			bool bInsertClonedCopy // = true
			);
		void ComboBoxItem_Add( const COMBOBOXITEMDATA_t & _CID );
		INT ComboBoxItem_RemoveAt(
			INT nIndex = 0,
			INT nCountToRemove = -1
			); // default parameters - remove all, returns removed count
	}; // struct TREEITEMINFO_t
public:
	DWORD m_dwCheckBoxAreaFlags;
protected:
	mutable CFont m_fontNormal, m_fontBold, m_fontItalic, m_fontBoldItalic,
				  m_fontNormalUnderline, m_fontBoldUnderline, 
				  m_fontItalicUnderline, m_fontBoldItalicUnderline;
	void _FontsSync();
	void _FontsDestroy();
	mutable CMap < HTREEITEM, HTREEITEM, TREEITEMINFO_t *, TREEITEMINFO_t * > m_mapItemInfo;
	virtual void _UnregisterItemsFromMap(
		HTREEITEM hti = NULL
		);
	virtual void _UnregisterItemsFromMapLinear();
	virtual void _UnregisterOneItemFromMap(
		HTREEITEM hti
		);
	virtual void PostNcDestroy();
	//{{AFX_VIRTUAL(CExtTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CExtTreeCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnSetFont( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSettingChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnStyleChanging( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnStyleChanged( WPARAM wParam, LPARAM lParam );
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCancelMode();
	afx_msg LRESULT _OnPaintExpanedItemContent(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnContextMenu( CWnd* pWnd, CPoint point );
	//}}AFX_MSG
	afx_msg void OnTimer( __EXT_MFC_UINT_PTR nIDEvent );
	DECLARE_MESSAGE_MAP()
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif

public:
// overrides
	virtual void OnTreeItemUnRegisterData(
		TREEITEMINFO_t & _TII
		);
	virtual void OnTreeItemContextMenu(
		HTREEITEM hti,
		CPoint point, // client coordinates
		DWORD dwHitTestFlags
		);
	virtual void OnPaintEntireTree( CDC & dc );
	virtual void OnPaintTreeItem(
		HTREEITEM hti,
		CDC & dc,
		CRect rcItemEntire,
		bool bFocusedWindow,
		bool bDisabledItem,
		bool bSelectedItem,
		bool bFocusedItem,
		bool bDH
		);
	virtual void OnPaintTreeItemLines(
		HTREEITEM hti,
		CDC & dc,
		INT nLevelIndent,
		CRect rcLinesArea,
		COLORREF clrForceLines
		);
	virtual void OnPaintTreeItemExpandCollapseButton(
		HTREEITEM hti,
		CDC & dc,
		CRect rcTreeBoxArea,
		COLORREF clrForceBoxLines,
		COLORREF clrForceBoxBackground,
		COLORREF clrForceBoxBk
		);
	virtual void OnPaintTreeItemBackground(
		HTREEITEM hti,
		CDC & dc,
		CRect rcItemLabel,
		bool bFocusedWindow,
		bool bDisabledItem,
		bool bSelectedItem,
		bool bFocusedItem,
		bool bDH,
		COLORREF clrItemBk
		);
	virtual void OnPaintTreeItemText(
		HTREEITEM hti,
		CDC & dc,
		CRect rcItemLabel,
		COLORREF clrItemText
		);
	virtual void OnPaintTreeItemInsertMark(
		HTREEITEM hti,
		CDC & dc,
		bool bInsertMarkAfter,
		COLORREF clrInsertMark,
		CRect rcItemEntire,
		CRect rcItemLabel
		);

	virtual CFont * OnQueryItemFont( HTREEITEM hti );

	virtual void OnPaintTreeItemCheckBox(
		HTREEITEM hti,
		CDC & dc,
		CRect rcTreeCheckBoxArea
		);
	virtual void OnPaintTreeItemRadioBox(
		HTREEITEM hti,
		CDC & dc,
		CRect rcTreeRadioBoxArea
		);
	virtual void OnPaintTreeItemIcon(
		HTREEITEM hti,
		CDC & dc,
		const CExtCmdIcon & _icon,
		CRect rcTreeIconArea,
		BYTE nSCA = BYTE(255)
		);
	virtual void OnPaintTreeItemControl(
		HTREEITEM hti,
		CDC & dc,
		CRect rcTreeControlArea,
		COLORREF clrControlText
		);

protected:
	INT m_nLastMouseButtonEventType[3]; // last events for VK_LBUTTON[0], VK_MBUTTON[0] and VK_RBUTTON[0], values is 0-down, 1-up, 2-double click
	bool _OnTreeMouseClickImpl(
		int nMouseButton,
		int nClick,
		UINT nMouseEventFlags,
		CPoint point
		);
public:
	virtual bool OnTreeMouseClick(
		HTREEITEM hti,
		DWORD dwHitTestFlags,
		int nMouseButton,
		int nClick,
		UINT nMouseEventFlags,
		CPoint point
		);
	virtual bool OnTreeItemDoDragDetect(
		HTREEITEM hti,
		DWORD dwHitTestFlags,
		int nMouseButton,
		int nClick,
		UINT nMouseEventFlags,
		CPoint point
		);

	virtual void OnTreeMouseMove(UINT nFlags, CPoint point);

// APIs
public:
	virtual BOOL Expand( HTREEITEM hti, UINT nCode );
	bool TreeItemIsExpanded( HTREEITEM hti ) const;
	bool HasItem( HTREEITEM hti ) const;
	bool HasAncestor(
		HTREEITEM htiParent,
		HTREEITEM htiChild,
		INT nMaxStepCount = -1
		) const;
	TREEITEMINFO_t * TreeItemInfoGetPtr( HTREEITEM hti );
	const TREEITEMINFO_t * TreeItemInfoGetPtr( HTREEITEM hti ) const;
	TREEITEMINFO_t & TreeItemInfoGet( HTREEITEM hti );
	const TREEITEMINFO_t & TreeItemInfoGet( HTREEITEM hti ) const;

	enum e_line_style_t
	{
		line_style_solid,
		line_style_dots
	};
	enum e_tree_box_style_t
	{
		tree_box_style_square,
		tree_box_style_round,
		tree_box_style_bitmap
	};
protected:
	e_line_style_t m_eOS;
	e_tree_box_style_t m_eTBS;
	COLORREF m_clrExpandCollapseButtonSign, m_clrExpandCollapseButtonFill;
	COLORREF m_clrLines;
	COLORREF m_clrTreeBkColor;
	COLORREF m_clrProgressBorder;
	COLORREF m_clrProgressBackgroundColor;
	COLORREF m_clrProgressBoxColor;

public:
	e_line_style_t LineStyleGet() const;
	void LineStyleSet( e_line_style_t eOS );

	e_tree_box_style_t TreeBoxStyleGet() const;
	void TreeBoxStyleSet( e_tree_box_style_t eTBS );

	COLORREF ExpandCollapseButtonSignColorGet() const;
	void ExpandCollapseButtonSignColorSet( COLORREF clr );

	COLORREF ExpandCollapseButtonFillColorGet() const;
	void ExpandCollapseButtonFillColorSet( COLORREF clr );

	COLORREF LineColorGet() const;
	void LineColorSet( COLORREF clr );

	COLORREF TreeBkColorGet() const;
	void TreeBkColorSet( COLORREF clr );

	COLORREF ProgressBorderColorGet() const;
	void ProgressBorderColorSet( COLORREF clr );

	COLORREF ProgressBackgroundColorGet() const;
	void ProgressBackgroundColorSet( COLORREF clr );

	COLORREF ProgressBoxColorGet() const;
	void ProgressBoxColorSet( COLORREF clr );

	virtual bool OnQueryItemEnabledState( HTREEITEM hti ) const;
	bool IsItemEnabled( HTREEITEM hti ) const;
	void EnableItem(
		HTREEITEM hti,
		bool bEnable = true
		);
	bool IsItemSelected( HTREEITEM hti ) const;
	void SelectItem(
		HTREEITEM hti,
		bool bSelect = true,
		bool bSubtract = false
		);
	void GetSelectedItemsList(
		CList < HTREEITEM, HTREEITEM > & _listSelectedItems,
		HTREEITEM htiStart = NULL, // NULL - root
		bool bDeep = true,
		bool bAnalyseSelectedChildren = false,
		bool bAddToTail = true
		) const;
protected:
	void _GetSelectedItemsList_Impl(
		CList < HTREEITEM, HTREEITEM > & _listSelectedItems,
		HTREEITEM htiStart = NULL, // NULL - root
		bool bDeep = true,
		bool bAnalyseSelectedChildren = false,
		bool bAddToTail = true
		) const;
public:
	void SelectItemRange(
		HTREEITEM htiFrom,
		HTREEITEM htiTo,
		bool bSelect,
		bool bExpandedOnly = true,
		bool bIncludeDisabled = true,
		bool bSubtract = false
		);

	HTREEITEM GetInPlaceEditedItem() const;
	HTREEITEM GetFocusedItem() const;
	void FocusItem(
		HTREEITEM hti,
		bool bSelect = false,
		bool bUnselectOtherItems = false,
		bool bMouseClickEvent = false
		);
	static const UINT g_nMsgTreeItemDelayedFocus;
	virtual void OnTreeItemDelayedFocus(
		HTREEITEM hti,
		bool bStartWaiting,
		bool bMouseClickEvent = false
		);

	CExtContentExpandWnd  m_wndContentExpand;

	HTREEITEM _GetNextParent( HTREEITEM hti ) const;
	HTREEITEM _GetPreviousItem( HTREEITEM hti, bool bExpandedOnly ) const;
	HTREEITEM _GetLastChildItem( HTREEITEM hti ) const;
	HTREEITEM _LinearItemGetNext(
		HTREEITEM htiFrom,
		bool bExpandedOnly = true
		) const;
	HTREEITEM _LinearItemGetPrev(
		HTREEITEM htiFrom,
		bool bExpandedOnly = true
		) const;
	HTREEITEM LinearItemGetNext(
		HTREEITEM htiFrom,
		bool bExpandedOnly = true,
		bool bIncludeDisabled = true
		) const;
	HTREEITEM LinearItemGetPrev(
		HTREEITEM htiFrom,
		bool bExpandedOnly = true,
		bool bIncludeDisabled = true
		) const;

	bool LinearItemIsBefore(
		HTREEITEM htiFrom,
		HTREEITEM htiBefore,
		bool bExpandedOnly = true
		) const;
	bool LinearItemIsAfter(
		HTREEITEM htiFrom,
		HTREEITEM htiAfter,
		bool bExpandedOnly = true
		) const;
	HTREEITEM LinearItemFromItem(
		HTREEITEM htiFrom,
		INT nOffset,
		INT * p_nResultShift = NULL,
		bool bExpandedOnly = true,
		bool bIncludeDisabled = true
		);


protected:
	DWORD m_dwTreeStyle, m_dwTreeStyleEx;
public:
	DWORD TreeGetStyle() const;
	void TreeModifyStyle(
		DWORD dwStyleAdd,
		DWORD dwStyleRemove = 0
		);
	DWORD TreeGetStyleEx() const;
	void TreeModifyStyleEx(
		DWORD dwStyleExAdd,
		DWORD dwStyleExRemove = 0
		);

	INT TreeGetDisplayedItemCount(
		bool * p_bLastIsPartiallyVisible = NULL
		) const;

protected:
	HTREEITEM m_htiAnchorItem, m_htiInsertMark;
	bool m_bInsertMarkAfter:1;

public:
	HTREEITEM AnchorItemGet() const;
	void AnchorItemSet( HTREEITEM hti );

protected:
	void _SingleExpand( HTREEITEM hti, bool bSingleExpand );

public:
	bool MultipleSelectionGet() const;
	void MultipleSelectionSet( bool bSet = true );
	
	bool SelectDisabledItemsGet() const;
	void SelectDisabledItemsSet( bool bSet = true );
	
	bool FocusDisabledItemsGet() const;
	void FocusDisabledItemsSet( bool bSet = true );
	
	bool SelectCollapsedItemsGet() const;
	void SelectCollapsedItemsSet( bool bSet = true );
	
	bool SubtractSelectionGet() const;
	void SubtractSelectionSet( bool bSet = true );

	bool FullRowSelectionGet() const;
	void FullRowSelectionSet( bool bSet = true );

	bool HasButtonsGet() const;
	void HasButtonsSet( bool bSet = true );

	bool HasLinesGet() const;
	void HasLinesSet( bool bSet = true );

	bool LinesAtRootGet() const;
	void LinesAtRootSet( bool bSet = true );

	bool ShowSelectionAlwaysGet() const;
	void ShowSelectionAlwaysSet( bool bSet = true );

	bool SingleExpandGet() const;
	void SingleExpandSet( bool bSet = true );

	bool TrackSelectionGet() const;
	void TrackSelectionSet( bool bSet = true );

//	bool CheckBoxesGet() const;
//	void CheckBoxesSet( bool bSet = true );

	bool HoverDisabledItemsGet() const;
	void HoverDisabledItemsSet( bool bSet = true );

	bool AutoDisableChildrenItemsGet() const;
	void AutoDisableChildrenItemsSet( bool bSet = true );

	bool HoverTreeBoxesGet() const;
	void HoverTreeBoxesSet( bool bSet = true );

	bool HoverCheckBoxesGet() const;
	void HoverCheckBoxesSet( bool bSet = true );

	bool HoverRadioBoxesGet() const;
	void HoverRadioBoxesSet( bool bSet = true );

	virtual INT OnTreeItemCalcCheckBoxAreaWidth() const;

	virtual bool TreeItemRectsGet(
		HTREEITEM hti,
		CRect * pRectEntire,
		CRect * pRectLineArea = NULL,
		CRect * pRectExpandCollapseButton = NULL,
		CRect * pRectCheckBox = NULL,
		CRect * pRectIcon = NULL,
		CRect * pRectLabel = NULL,
		CRect * pRectControl = NULL,
		CRect * pRectRightPart = NULL
		) const;

	enum e_tree_item_rect_type_t
	{
		e_tirt_entire, // all item (entire client area width)
		e_tirt_line_area,
		e_tirt_tree_box,
		e_tirt_check_box,
		e_tirt_icon,
		e_tirt_label,
		e_tirt_control,
		e_tirt_right_part,
	};

	bool TreeItemRectGet(
		HTREEITEM hti,
		CRect & rcRetVal,
		e_tree_item_rect_type_t eTIRT
		) const;
	virtual INT TreeItemCalcLabelWidth(
		HTREEITEM hti
		) const;
	virtual HTREEITEM TreeItemHitTest(
		CPoint ptClient,
		DWORD & dwHitTestFlags
		) const;

	virtual void _DeselectRadioBoxesNextPrev(
		HTREEITEM hti,
		CExtSafeString m_strRadioGroupName
		);

	void _CheckRadioBox( HTREEITEM hti );
protected:
	bool m_bCanceling:1;
	CArray < CExtCmdIcon *, CExtCmdIcon * > m_arrIcons;
	virtual void _DoDelayedTreeIconInit();
public:
	virtual INT TreeIconGetCount() const;
	virtual void TreeIconInsert(
		const CExtCmdIcon & _icon,
		INT nPos
		);
	void TreeIconAdd(
		const CExtCmdIcon & _icon
		);
	virtual void TreeIconRemove(
		INT nPos,
		INT nCount = 1
		);
	void TreeIconRemoveAll();
	virtual CExtCmdIcon & TreeIconGetAt( INT nPos );
	const CExtCmdIcon & TreeIconGetAt( INT nPos ) const;
protected:
	HTREEITEM m_htiInplaceEdited;
	HWND m_hWndChildControl;
	bool m_bReadOnlyLabels:1;
public:
	bool ReadOnlyLabelsGet() const;
	void ReadOnlyLabelsSet( bool bReadOnlyLabels );
	const CExtCmdIcon * OnTreeItemQueryIcon(
		HTREEITEM hti
		) const;
	virtual HWND OnInplaceControlCreate(
		HTREEITEM hti,
		CRect rcControl
		);
	virtual void OnInplaceControlCancel();
	virtual void OnInplaceControlComplete(
		__EXT_MFC_SAFE_LPCTSTR strEditedText,
		bool bEditingLabel
		);

	class __PROF_UIS_API CInplaceEdit
		: public CExtEdit
		, public CExtHookSpy
	{
	public:
		bool m_bEditingLabel:1, m_bResultSaved:1, m_bCancelMode:1;
		CInplaceEdit( bool bEditingLabel );
	protected:
		virtual void PreSubclassWindow();
		virtual void PostNcDestroy();
		virtual bool OnHookSpyKeyMsg(
			MSG * pMSG
			);
		virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	}; // class CInplaceEdit

	class __PROF_UIS_API CInplaceComboBox : public CExtComboBox
	{
		bool m_bResultSaved:1;
	public:
		CInplaceComboBox();
	protected:
		virtual void PreSubclassWindow();
		virtual void PostNcDestroy();
		virtual bool OnHookSpyKeyMsg(
			MSG * pMSG
			);
		virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	}; // class CInplaceComboBox

	virtual bool OnQueryWindowFocusedState() const;

protected:
	INT m_nMarginCheckBoxLeft, m_nMarginCheckBoxRight,
		m_nMarginIconLeft, m_nMarginIconRight,
		m_nMarginLabelLeft, m_nMarginLabelRight,
		m_nMarginControlLabelLeft, m_nMarginControlLabelRight,
		m_nPaddingLabelLeft, m_nPaddingLabelRight;

public:
	INT MarginCheckBoxLeftGet() const;
	void MarginCheckBoxLeftSet( INT nMargin );
	INT MarginCheckBoxRightGet() const;
	void MarginCheckBoxRightSet( INT nMargin );
	INT MarginIconLeftGet() const;
	void MarginIconLeftSet( INT nMargin );
	INT MarginIconRightGet() const;
	void MarginIconRightSet( INT nMargin );
	INT MarginLabelLeftGet() const;
	void MarginLabelLeftSet( INT nMargin );
	INT MarginLabelRightGet() const;
	void MarginLabelRightSet( INT nMargin );
	INT MarginControlLabelLeftGet() const;
	void MarginControlLabelLeftSet( INT nMargin );
	INT MarginControlLabelRightGet() const;
	void MarginControlLabelRightSet( INT nMargin );
	INT PaddingLabelLeftGet() const;
	void PaddingLabelLeftSet( INT nPadding );
	INT PaddingLabelRightGet() const;
	void PaddingLabelRightSet( INT nPadding );

	CExtBitmap m_bmpTreeBoxExpanded, m_bmpTreeBoxCollapsed,
				m_bmpTreeBoxExpandedHover, m_bmpTreeBoxCollapsedHover;

	virtual bool _NeedsChildren( HTREEITEM hti ) const;
	virtual void _DeleteChildren( HTREEITEM hti );

protected:
	HCURSOR m_hCursorHand;
	HTREEITEM m_htiHover;
	DWORD m_dwLastHoverHitTestFlags;

public:
	// flags for LPARAM of g_nMsgTreeItemDelayedFocus
	enum delayed_focus_event_type_t
	{
		__PARM_DELAYED_FOCUS_BEGIN_WAITING = 1,
		__PARM_DELAYED_FOCUS_MOUSE_CLICK = 2,
	};
protected:
	UINT m_nDelayedFocusTimerID, m_nDelayedFocusTime;
	UINT m_nDelayedEditingTimerID;
	HTREEITEM m_htiDelayedFocus;
public:
	virtual void OnTreeItemHoverChanged(
		HTREEITEM htiHoverNew,
		DWORD dwHitTestFlagsNew,
		HTREEITEM htiHoverOld,
		DWORD dwHitTestFlagsOld
		);
	virtual void OnTreeItemSetCursor(
		HTREEITEM htiHover,
		DWORD dwHitTestFlags
		);
	void LoadWinXPTreeBox();
	void LoadWinVistaTreeBox();
	void _CheckDeepFocusBegin(
		CList < HTREEITEM, HTREEITEM > & _listFocusPath
		);
	HTREEITEM _CheckDeepFocusEnd(
		CList < HTREEITEM, HTREEITEM > & _listFocusPath
		);
	HTREEITEM FindItemByText(
		HTREEITEM htiStartWalk,
		__EXT_MFC_SAFE_LPCTSTR strText,
		UINT nStartWalkCode = TVGN_CHILD,
		UINT nContinueWalkCode = TVGN_NEXT
		);
}; // class CExtTreeCtrl

/////////////////////////////////////////////////////////////////////////////
// CExtWTC

template < class _BT > class CExtWTC : public _BT
{
public:
	CExtWTC()
	{
	}
	virtual ~CExtWTC()
	{
	}
	virtual bool UpdateContent( HTREEITEM hti, bool bCompleteRescan )
	{
		ASSERT_VALID( this );
		hti;
		bCompleteRescan;
		ASSERT( FALSE ); // this method should be overridden
		return false;
	}
	virtual void UpdateRootContent()
	{
		ASSERT_VALID( this );
		PreExpandItem( TVI_ROOT );
		Expand( TVI_ROOT, TVE_EXPAND );
		SCROLLINFO _si;
		GetScrollInfo( SB_HORZ, &_si );
		EnsureVisible( GetChildItem( TVI_ROOT ) );
		SetScrollInfo( SB_HORZ, &_si, FALSE );
	}
	virtual void PreExpandItem( HTREEITEM hti )
	{
		hti;
	}
	virtual BOOL Expand( HTREEITEM hti, UINT nCode )
	{
		ASSERT_VALID( this );
		if(		nCode == TVE_COLLAPSE
			||	( nCode == TVE_TOGGLE && TreeItemIsExpanded( hti ) )
			)
		{
			_DeleteChildren( hti );
			TVITEM _tvi;
			_tvi.hItem = hti;
			_tvi.mask = TVIF_HANDLE|TVIF_CHILDREN;
			_tvi.cChildren = 1;
			SetItem( &_tvi );
 			SetItemState( hti, 0, TVIS_EXPANDED );
			return CExtTreeCtrl::Expand( hti, nCode );
		}
		//CWaitCursor _waitCursor;
		bool bCheckChildren = UpdateContent( hti, true );
		if( hti != TVI_ROOT )
		{
			TVITEM _tvi;
			_tvi.hItem = hti;
			_tvi.mask = TVIF_HANDLE | TVIF_CHILDREN;
			_tvi.cChildren = _NeedsChildren( hti ) ? 0 : 1;
			if( bCheckChildren )
				SetItem( &_tvi );
			else if ( _tvi.cChildren == 0 )
				SetItemState( hti, 0, TVIS_EXPANDED );
		}
		_BT::Expand( hti, TVE_EXPAND );
		return TRUE;
	}
	virtual void RefreshSubItems( HTREEITEM hti )
	{
		ASSERT_VALID( this );
		if(		hti != TVI_ROOT
			&&	( ! ItemHasChildren( hti ) )
			)
			return;
		_DeleteChildren( hti );
		if( hti == TVI_ROOT )
			UpdateRootContent();
		else
		{
			PreExpandItem( hti );
			Expand( hti, TVE_EXPAND );
		}
	}
}; /// class CExtWTC

#endif // ( ! defined __EXT_MFC_NO_TREE_VIEW_CTRL )

#if (! defined __EXT_MFC_NO_MSG_BOX )

//////////////////////////////////////////////////////////////////////////////
// CExtMsgBox window

// first group of message box flags uses same values as in Win32
#define __EXT_MB_OK											0x00000000L
#define __EXT_MB_OKCANCEL									0x00000001L
#define __EXT_MB_ABORTRETRYIGNORE							0x00000002L
#define __EXT_MB_YESNOCANCEL								0x00000003L
#define __EXT_MB_YESNO										0x00000004L
#define __EXT_MB_RETRYCANCEL								0x00000005L
#define __EXT_MB_CANCELTRYCONTINUE							0x00000006L

#define __EXT_MB_ICONHAND									0x00000010L
#define __EXT_MB_ICONQUESTION								0x00000020L
#define __EXT_MB_ICONEXCLAMATION							0x00000030L
#define __EXT_MB_ICONASTERISK								0x00000040L
#define __EXT_MB_ICONWARNING								__EXT_MB_ICONEXCLAMATION
#define __EXT_MB_ICONERROR									__EXT_MB_ICONHAND
#define __EXT_MB_ICONINFORMATION							__EXT_MB_ICONASTERISK
#define __EXT_MB_ICONSTOP									__EXT_MB_ICONHAND

#define __EXT_MB_DEFBUTTON1									0x00000000L
#define __EXT_MB_DEFBUTTON2									0x00000100L
#define __EXT_MB_DEFBUTTON3									0x00000200L
#define __EXT_MB_DEFBUTTON4									0x00000300L

#define __EXT_MB_SYSTEMMODAL								0x00001000L
#define __EXT_MB_HELP										0x00004000L
#define __EXT_MB_SETFOREGROUND								0x00010000L
#define __EXT_MB_TOPMOST									0x00040000L
#define __EXT_MB_RIGHT										0x00080000L
#define __EXT_MB_RTLREADING									0x00100000L

// masks
#define __EXT_MB_TYPEMASK									0x0000000FL
#define __EXT_MB_ICONMASK									0x000000F0L
#define __EXT_MB_DEFMASK									0x00000F00L
#define __EXT_MB_MODEMASK									0x00003000L
#define __EXT_MB_MISCMASK									0x0000C000L

// second group of message box flags defines extended features
#define __EXT_MB_DEFBUTTON5									0x00000400L
#define __EXT_MB_DEFBUTTON6									0x00000500L

#define __EXT_MB_CONTINUEABORT								0x00000007L
#define __EXT_MB_SKIPSKIPALLCANCEL							0x00000008L
#define __EXT_MB_IGNOREIGNOREALLCANCEL						0x00000009L

#define __EXT_MB_DO_NOT_DISPLAY_AGAIN						0x01000000L
#define __EXT_MB_DO_NOT_ASK_AGAIN							0x02000000L
#define __EXT_MB_CHECK_BOX_PRESENT							(__EXT_MB_DO_NOT_DISPLAY_AGAIN|__EXT_MB_DO_NOT_ASK_AGAIN)
#define __EXT_MB_YES_TO_ALL									0x04000000L
#define __EXT_MB_NO_TO_ALL									0x08000000L

// use right aligned buttons at bottom
#define __EXT_MB_RIGHT_ALIGN								0x10000000L
// do not pay sound
#define __EXT_MB_NO_SOUND									0x20000000L
#define __EXT_MB_USER_DEFINED_01							0x40000000L
#define __EXT_MB_USER_DEFINED_00							0x80000000L

#define __EXT_MB_DEFAULT_STYLES								(MB_OK)

// do not apply default icon if there is no __EXT_MB_ICON*** style specified
#define __EXT_MB_EX_NO_DEFAULT_ICON_STYLE					0x00000001L
// display Copy button for copying message text to clipboard as rightmost button
#define __EXT_MB_EX_COPY_BUTTON								0x00000002L
// display Copy button as leftmost
#define __EXT_MB_EX_COPY_BTN_FIRST							0x00000004L
// display read only edit control with message text instead of label control
#define __EXT_MB_EX_USE_EDIT_CONTROL						0x00000008L
// use scrollable edit control
#define __EXT_MB_EX_SCROLLABLE_EDIT							0x00000010L
// disable copying message text to clipboard on Ctrl+C and Ctrl+Insert
#define __EXT_MB_EX_DISABLE_KEYBOARD_COPY_ACCEL				0x00000020L
// check box is not checked initially
#define __EXT_MB_EX_CHECK_BOX_IS_NOT_CHECKED_INTIALLY		0x00000040L

#define __EXT_MB_EX_USER_DEFINED_24							0x00000080L
#define __EXT_MB_EX_USER_DEFINED_23							0x00000100L
#define __EXT_MB_EX_USER_DEFINED_22							0x00000200L
#define __EXT_MB_EX_USER_DEFINED_21							0x00000400L
#define __EXT_MB_EX_USER_DEFINED_20							0x00000800L
#define __EXT_MB_EX_USER_DEFINED_19							0x00001000L
#define __EXT_MB_EX_USER_DEFINED_18							0x00002000L
#define __EXT_MB_EX_USER_DEFINED_17							0x00004000L
#define __EXT_MB_EX_USER_DEFINED_16							0x00008000L
#define __EXT_MB_EX_USER_DEFINED_15							0x00010000L
#define __EXT_MB_EX_USER_DEFINED_14							0x00020000L
#define __EXT_MB_EX_USER_DEFINED_13							0x00040000L
#define __EXT_MB_EX_USER_DEFINED_12							0x00080000L
#define __EXT_MB_EX_USER_DEFINED_11							0x00100000L
#define __EXT_MB_EX_USER_DEFINED_10							0x00200000L
#define __EXT_MB_EX_USER_DEFINED_09							0x00400000L
#define __EXT_MB_EX_USER_DEFINED_08							0x00800000L
#define __EXT_MB_EX_USER_DEFINED_07							0x01000000L
#define __EXT_MB_EX_USER_DEFINED_06							0x02000000L
#define __EXT_MB_EX_USER_DEFINED_05							0x04000000L
#define __EXT_MB_EX_USER_DEFINED_04							0x08000000L
#define __EXT_MB_EX_USER_DEFINED_03							0x10000000L
#define __EXT_MB_EX_USER_DEFINED_02							0x20000000L
#define __EXT_MB_EX_USER_DEFINED_01							0x40000000L
#define __EXT_MB_EX_USER_DEFINED_00							0x80000000L

#define __EXT_MB_EX_DEFAULT_STYLES							(0)

// first group of message box button identifiers uses same values as in Win32
#define ID_EXT_MBB_OK										1
#define ID_EXT_MBB_CANCEL									2
#define ID_EXT_MBB_ABORT									3
#define ID_EXT_MBB_RETRY									4
#define ID_EXT_MBB_IGNORE									5
#define ID_EXT_MBB_YES										6
#define ID_EXT_MBB_NO										7
#define ID_EXT_MBB_CLOSE									8
#define ID_EXT_MBB_HELP										9
#define ID_EXT_MBB_TRY_AGAIN								10
#define ID_EXT_MBB_CONTINUE									11

// second group of message box button identifiers defines extended features
#define ID_EXT_MBB_YES_TO_ALL								14
#define ID_EXT_MBB_NO_TO_ALL								15
#define ID_EXT_MBB_SKIP										16
#define ID_EXT_MBB_SKIP_ALL									17
#define ID_EXT_MBB_IGNORE_ALL								18
#define ID_EXT_MBB_CHECK_BOX								19
#define ID_EXT_MBB_EX_COPY_BUTTON							20

// constants (not styles) defining default margins, paddings and colors used by CExtMsgBox box parts
#define __EXT_MSG_BOX_DEFAULT_WINDOW_BORDER_WIDTH			8
#define __EXT_MSG_BOX_DEFAULT_WINDOW_BORDER_HEIGHT			8
#define __EXT_MSG_BOX_DEFAULT_BUTTON_BORDER_WIDTH			5
#define __EXT_MSG_BOX_DEFAULT_BUTTON_BORDER_HEIGHT			1
#define __EXT_MSG_BOX_DEFAULT_BUTTON_WIDTH					50
#define __EXT_MSG_BOX_DEFAULT_BUTTON_HEIGHT					12
#define __EXT_MSG_BOX_DEFAULT_EDITOR_MIN_HEIGHT_PX			32
#define __EXT_MSG_BOX_DEFAULT_EDITOR_MAX_HEIGHT_PX			80
#define __EXT_MSG_BOX_DEFAULT_EDITOR_COLOR_TEXT				RGB(0,0,0)
#define __EXT_MSG_BOX_DEFAULT_EDITOR_COLOR_BKGND			RGB(255,255,255)
#define __EXT_MSG_BOX_CHECK_BOX_ADDITIONAL_SPACE			14
#define __EXT_MSG_BOX_BTN_SPACE								6

class __PROF_UIS_API CExtMsgBox
	: public 
#if ( _MFC_VER == 0x700 )
	CExtResizableDialog
#else
	CExtNCW < CExtResizableDialog >
#endif
{
protected:
	CExtSafeString m_strCaption, m_strMessageText, m_strUniqueID;
	UINT m_nMsgBoxStyle, m_nMsgBoxStyleEx, m_nHelpID, m_nTimeoutSeconds, m_nTimeoutTimer, m_nDefaultButtonDlgCtrlID, m_nEscapeButtonDlgCtrlID;
	bool m_bIsDisabledTimeout:1, m_bDynamicIcon:1;
	struct __PROF_UIS_API msgbox_button_info_t
	{
		UINT m_nButtonDlgCtrlID;
		CExtSafeString m_strCaption;
		msgbox_button_info_t(
			UINT nButtonDlgCtrlID,
			__EXT_MFC_SAFE_LPCTSTR strCaption
			)
			: m_nButtonDlgCtrlID( nButtonDlgCtrlID )
			, m_strCaption( LPCTSTR(strCaption) )
		{
		}
	};
	CTypedPtrArray < CPtrArray, msgbox_button_info_t * > m_arrMsgBoxButtons;
	CExtLabel m_wndLabelMsgBoxIcon, m_wndLabelMsgBoxMessageText;
	CExtNCSB < CExtEdit > m_wndEditMsgBoxMessageText;
	CSize m_sizeButton, m_sizeCheckBox, m_sizeMessageText, m_sizeBorder, m_sizeStdButton, m_sizeStdButtonBorder;
	INT m_nCheckBoxGap, m_nButtonSpace, m_nMinEditCtrlHeight, m_nMaxEditCtrlHeight;
	HWND m_hWndParentFromCTOR;
public:
	COLORREF m_clrEditorText, m_clrEditorBkgnd;
	static __EXT_MFC_SAFE_LPCTSTR g_strRegSectionName;
	CExtBitmap m_bmpIcon;
	DECLARE_DYNCREATE( CExtMsgBox );
	CExtMsgBox(
		HWND hWndParent = NULL,
		__EXT_MFC_SAFE_LPCTSTR strUniqueID = NULL // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
		);
	CExtMsgBox(
		HWND hWndParent,
		__EXT_MFC_SAFE_LPCTSTR strMessageText,
		__EXT_MFC_SAFE_LPCTSTR strCaption = NULL,
		UINT nMsgBoxStyle = __EXT_MB_DEFAULT_STYLES,
		UINT nHelpID = 0,
		__EXT_MFC_SAFE_LPCTSTR strUniqueID = NULL, // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
		UINT nMsgBoxStyleEx = __EXT_MB_EX_DEFAULT_STYLES
		);
	CExtMsgBox(
		HWND hWndParent,
		UINT nMessageResourceID,
		UINT nCaptionResourceID = 0,
		UINT nMsgBoxStyle = __EXT_MB_DEFAULT_STYLES,
		UINT nHelpID = 0,
		__EXT_MFC_SAFE_LPCTSTR strUniqueID = NULL, // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
		UINT nMsgBoxStyleEx = __EXT_MB_EX_DEFAULT_STYLES
		);
	virtual ~CExtMsgBox();
	//{{AFX_DATA(CExtMsgBox)
	//}}AFX_DATA
protected:
	//{{AFX_VIRTUAL(CExtMsgBox)
protected:
	virtual void DoDataExchange( CDataExchange * pDX );
	virtual BOOL OnInitDialog();
	virtual BOOL OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult );
public:
	virtual BOOL OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo );
	virtual BOOL PreTranslateMessage( MSG * pMsg );
	//}}AFX_VIRTUAL
protected:
	void _AddDefaultIconStyle();
public:
	virtual UINT MsgBoxStyleGet() const;
	virtual void MsgBoxStyleSet( UINT nMsgBoxStyle );
	virtual UINT MsgBoxStyleExGet() const;
	virtual void MsgBoxStyleExSet( UINT nMsgBoxStyleEx );
	virtual INT MinEditCtrlHeightGet() const;
	virtual void MinEditCtrlHeightSet( INT nMaxEditCtrlHeight );
	virtual INT MaxEditCtrlHeightGet() const;
	virtual void MaxEditCtrlHeightSet( INT nMaxEditCtrlHeight );
	virtual CExtSafeString MessageGet() const;
	virtual void MessageSet( __EXT_MFC_SAFE_LPCTSTR strMessageText );
	void MessageSet( UINT nMessageResourceID );
	virtual CExtSafeString CaptionGet() const;
	virtual void CaptionSet( __EXT_MFC_SAFE_LPCTSTR strCaption );
	void CaptionSet( UINT nCaptionResourceID );
	virtual UINT TimeoutGet(
		bool * p_bIsDisabledTimeout = NULL
		) const;
	virtual void TimeoutSet(
		UINT nTimeoutSeconds,
		bool bIsDisabledTimeout = false
		);
	bool IsDisabledTimeout() const;
	static int DoMsgBox(
		HWND hWndParent,
		UINT nMessageResourceID,
		UINT nCaptionResourceID = 0,
		UINT nMsgBoxStyle = __EXT_MB_DEFAULT_STYLES,
		UINT nHelpID = 0,
		__EXT_MFC_SAFE_LPCTSTR strUniqueID = NULL, // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
		UINT nMsgBoxStyleEx = __EXT_MB_EX_DEFAULT_STYLES,
		UINT nTimeoutSeconds = 0,
		bool bIsDisabledTimeout = false,
		CExtBitmap * pBmpIcon = NULL
		);
	static int DoMsgBox(
		HWND hWndParent,
		__EXT_MFC_SAFE_LPCTSTR strMessageText, 
		__EXT_MFC_SAFE_LPCTSTR strCaption = NULL,
		UINT nMsgBoxStyle = __EXT_MB_DEFAULT_STYLES,
		UINT nHelpID = 0,
		__EXT_MFC_SAFE_LPCTSTR strUniqueID = NULL, // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
		UINT nMsgBoxStyleEx = __EXT_MB_EX_DEFAULT_STYLES,
		UINT nTimeoutSeconds = 0,
		bool bIsDisabledTimeout = false,
		CExtBitmap * pBmpIcon = NULL
		);
	virtual __EXT_MFC_INT_PTR DoModal();
	void EndDialog( int nResult );
	virtual void RegisterButton(
		UINT nButtonDlgCtrlID,
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		bool bIsDefault = false,
		bool bIsEscape = false
		);
	void RegisterButton(
		UINT nButtonDlgCtrlID,
		UINT nCaptionResourceID,
		bool bIsDefault = false,
		bool bIsEscape = false
		);
	virtual void UnRegisterAllButtons();
	static void ResetAllMsgBoxes();
	void ResetMsgBox();
	virtual CExtSafeString OnMsgBoxGetRegVarName() const; // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
protected:
	virtual bool OnMsgBoxTimeoutDecrease();
	virtual void OnMsgBoxRegisterButtons();
	virtual bool OnMsgBoxCreateIconLabel();
	virtual bool OnMsgBoxCreateMessageLabel();
	virtual bool OnMsgBoxCreateCheckBox();
	virtual bool OnMsgBoxCreateButtons();
	virtual void OnMsgBoxLayout();
	virtual bool OnMsgBoxCopyToClipboard();
	virtual void OnMsgBoxHelp();
protected:
	//{{AFX_MSG(CExtMsgBox)
	//}}AFX_MSG
	afx_msg void OnTimer( __EXT_MFC_UINT_PTR nIDEvent );
	DECLARE_MESSAGE_MAP()
}; // class CExtMsgBox

int __PROF_UIS_API ProfUISMsgBox(
	HWND hWndParent,
	UINT nMessageResourceID,
	UINT nCaptionResourceID = 0,
	UINT nMsgBoxStyle = __EXT_MB_DEFAULT_STYLES,
	UINT nHelpID = 0,
	__EXT_MFC_SAFE_LPCTSTR strUniqueID = NULL, // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	UINT nMsgBoxStyleEx = __EXT_MB_EX_DEFAULT_STYLES,
	UINT nTimeoutSeconds = 0,
	bool bIsDisabledTimeout = false,
	CExtBitmap * pBmpIcon = NULL
	);
int __PROF_UIS_API ProfUISMsgBox(
	HWND hWndParent,
	__EXT_MFC_SAFE_LPCTSTR strMessageText, 
	__EXT_MFC_SAFE_LPCTSTR strCaption = NULL,
	UINT nMsgBoxStyle = __EXT_MB_DEFAULT_STYLES,
	UINT nHelpID = 0,
	__EXT_MFC_SAFE_LPCTSTR strUniqueID = NULL, // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	UINT nMsgBoxStyleEx = __EXT_MB_EX_DEFAULT_STYLES,
	UINT nTimeoutSeconds = 0,
	bool bIsDisabledTimeout = false,
	CExtBitmap * pBmpIcon = NULL
	);

#define IMPLEMENT_CWinAPP_DoMessageBox \
virtual int DoMessageBox( LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt ) \
{ \
HWND hWndParent = ::GetActiveWindow(); \
	if( hWndParent == NULL ) \
	{ \
		CWnd * pMainWnd = GetMainWnd(); \
		if( pMainWnd->GetSafeHwnd() != NULL ) \
			hWndParent = ::GetLastActivePopup( pMainWnd->m_hWnd ); \
	} \
	return ::ProfUISMsgBox( hWndParent, lpszPrompt, _T(""), nType, nIDPrompt ); \
}

#endif // (! defined __EXT_MFC_NO_MSG_BOX )

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_CONTROLS_COMMON_H)
