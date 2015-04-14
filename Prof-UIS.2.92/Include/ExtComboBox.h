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

#if (!defined __EXT_COMBO_BOX_H)
#define __EXT_COMBO_BOX_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_MOUSECAPTURESINK_H)
	#include <../Src/ExtMouseCaptureSink.h>
#endif

#if (!defined __EXT_EDIT_H)
	#include <ExtEdit.h>
#endif

#if (!defined __EXT_HOOK_H)
	#include "../Src/ExtHook.h"
#endif

#if (!defined __EXT_CONTROLBAR_H)
	#include <ExtControlBar.h>
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExtPopupMenuTipWnd;
class CExtComboBoxBase;

/////////////////////////////////////////////////////////////////////////////
// CExtComboBoxPopupListBox window

class __PROF_UIS_API CExtComboBoxPopupListBox : public CWnd
{
protected:
	bool m_bFirstActivationInvocation:1, m_bSimpleModeComboBox:1;
	CRect m_rcWndRestore;
public:
	DECLARE_DYNAMIC( CExtComboBoxPopupListBox );

	bool m_bInvalidateOnMouseMove:1;
	HWND m_hWndCB;
	CExtComboBoxBase * m_pCB;

	CExtComboBoxPopupListBox();
	virtual ~CExtComboBoxPopupListBox();

	//{{AFX_VIRTUAL(CExtComboBoxPopupListBox)
	protected:
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL
protected:
	bool _IsEnabledItem( CPoint point );
	//{{AFX_MSG(CExtComboBoxPopupListBox)
	afx_msg	void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtComboBoxPopupListBox

/////////////////////////////////////////////////////////////////////////////
// CExtComboBoxFilterPopupListBox window

class __PROF_UIS_API CExtComboBoxFilterPopupListBox : public CListBox
{
public:
	DECLARE_DYNAMIC( CExtComboBoxFilterPopupListBox );

	CExtComboBoxFilterPopupListBox(
		CExtComboBoxBase * pCB = NULL
		);
	virtual ~CExtComboBoxFilterPopupListBox();
	
// Attributes
public:
	class __PROF_UIS_API LB_ITEM
	{
	public:
		LB_ITEM();
		~LB_ITEM();

		LPARAM LParamGet() const;
		void LParamSet( 
			LPARAM lParam
			);

	protected:
		LPARAM m_lParam;

	}; // class LB_ITEM

	typedef CTypedPtrArray < CPtrArray, LB_ITEM * > FLBItemsArr_t;

// Operations
public:
	LONG HitTest( 
		const POINT & ptClient
		) const;

	LB_ITEM * LbItemGet( 
		LONG nIndex 
		) const;

protected:
	INT LbItemGetCount() const;
	INT LbItemGetIndexOf( 
		const LB_ITEM * pItem 
		) const;
	INT LbItemInsert( 
		INT nIndex = -1 // append
		);
	bool LbItemRemove(
		LONG nIndex
		);
	void LbItemRemoveAll();

protected:
	HWND m_hWndCB;
	CExtComboBoxBase * m_pCB;
	
	bool m_bResizingX:1;
	bool m_bResizingY:1;
	CPoint m_ptCursorPosLast;

	HCURSOR m_hCursorSizeWE;	// horizontal two-headed arrow
	HCURSOR m_hCursorSizeNS;	// vertical two-headed arrow
	HCURSOR m_hCursorSizeNWSE;	// two-headed arrow with ends at upper left and lower right 

	FLBItemsArr_t m_arrLBItems;

	friend class CExtComboBoxBase;

protected:
	virtual bool _ProcessMouseMove( 
		CPoint point 
		);
	virtual bool _ProcessMouseClick(
		CPoint point,
		bool bButtonPressed,
		INT nMouseButton // MK_... values
		);

	//{{AFX_VIRTUAL(CExtComboBoxFilterPopupListBox)
	public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDIS );
	virtual void MeasureItem( LPMEASUREITEMSTRUCT lpMIS );
	protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CExtComboBoxFilterPopupListBox)
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnCancelMode();
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnLBAddString( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnLBInsertString( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnLBDeleteString( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnLBResetContent( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnLBAddFile( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	afx_msg UINT OnNcHitTest(CPoint point);
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif
	DECLARE_MESSAGE_MAP()
	
}; // class CExtComboBoxFilterPopupListBox

/////////////////////////////////////////////////////////////////////////////
// CExtComboEditCtrlHook window
// (internal combobox edit ctrl)

class __PROF_UIS_API CExtComboEditCtrlHook : public CExtEditBase
{
// Construction
public:

	DECLARE_DYNCREATE( CExtComboEditCtrlHook );
	CExtComboEditCtrlHook();

// Attributes
public:

// Operations
public:
	virtual CExtComboBoxBase * OnQueryComboBox() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtComboEditCtrlHook)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtComboEditCtrlHook();

protected:
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;
	virtual COLORREF OnQueryBackColor() const;
	virtual COLORREF OnQueryTextColor() const;

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtComboEditCtrlHook)
	afx_msg void OnContextMenu(CWnd* pWnd,CPoint pos );	
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CExtComboBox;

}; // class __PROF_UIS_API CExtComboEditCtrlHook

/////////////////////////////////////////////////////////////////////////////
// CExtComboBoxBase window

#define __EXT_COMBO_BOX_WM_USER_REALSELENDOK			( WM_USER + 123 )
#define __EXT_COMBO_BOX_WM_USER_AUTOCOMPLETE			( WM_USER + 124 )

class __PROF_UIS_API CExtComboBoxBase
	: public CComboBox
	, public CExtHookSpy
	, public CExtAnimationSingleton
{
// Construction
public:
	DECLARE_DYNCREATE( CExtComboBoxBase );

	CExtComboBoxBase();

	class __PROF_UIS_API LB_ITEM
	{
	public:
		LB_ITEM();
		~LB_ITEM();

		class CELL
		{
		public:
			CELL( 
				__EXT_MFC_SAFE_LPCTSTR sItemText = NULL
				);
			~CELL();

			LONG m_nLParam;
			COLORREF m_clrText;
			COLORREF m_clrBack;
			CExtSafeString m_sItemText;

		}; // class CELL

		INT LbItemCellInsert( 
			__EXT_MFC_SAFE_LPCTSTR sItemText = NULL,
			INT nPos = -1 // append
			);
		bool LbItemCellRemove(
			LONG nItem
			);
		CELL * LbItemCellGet( 
			LONG nItem 
			) const;
		INT LbItemCellGetIndexOf( 
			const CELL * pItemCell 
			) const;
		INT LbItemCellGetCount() const;

		bool IsEnabled() const;
		void Enable(
			bool bEnable = true
			);

	protected:
		bool m_bEnabled:1;

		typedef CTypedPtrArray < CPtrArray, CELL * > ItemCellsArr_t;
		ItemCellsArr_t m_arrItemCells;

	}; // class LB_ITEM
	
	typedef CTypedPtrArray < CPtrArray, LB_ITEM * > LBItemsArr_t;

// Attributes
public:
	bool m_bEnableAutoComplete:1;
	bool m_bUseCaseSensitiveAutoComplete:1;
	bool m_bEnableAutoFilter:1;
	bool m_bHandleCtxMenus:1;
	bool m_bEnableSelChangeWorkaround:1;

protected:
	bool m_bInitialized:1;
	bool m_bAutoComplete:1;
	CExtComboEditCtrlHook m_wndInnerEditHook;
	HWND m_hWndPopupListBox;
	CExtComboBoxPopupListBox * m_pWndPopupListBox;
	CString m_sSavedText;

	COLORREF m_clrBack;
	COLORREF m_clrBackPrev;
	CBrush m_brBack;
	COLORREF m_clrText;

	CToolTipCtrl m_wndToolTip;
	void InitToolTip();

	typedef
		CMap < LONG, LONG, LONG, LONG >
		MapColWidth_t;
	MapColWidth_t m_mapColumnsWidth;

	LBItemsArr_t m_arrLBItems;

protected:
	virtual void _OnDrawComboImpl(
		bool bPressed,
		bool bHover,
		CDC * pDC = NULL
		);
public:
	virtual CExtComboBoxPopupListBox * OnComboBoxPopupListBoxInstantiate();
	virtual CExtComboBoxPopupListBox * OnComboBoxPopupListBoxQuery(
		HWND hWndSubclass = NULL
		);
	virtual HWND AnimationSite_GetSafeHWND() const;

	void ShowFilterPopupList( 
		CExtSafeString & sFilterText 
		);

	CSize GetFilterPopupListSize() const;
	CSize SetFilterPopupListSize( 
		CSize szSize 
		);

	LB_ITEM * LbItemGet( 
		LONG nItem 
		) const;
	LONG LbColumnWidthGet( 
		LONG nColNo
		) const;
	void LbColumnWidthSet( 
		LONG nColNo,
		LONG nWidth
		);
	bool LbItemIsEnabled(
		LONG nItem
		) const;
	void LbItemEnable(
		LONG nItem,
		bool bEnable = true
		);

protected:
	INT LbItemSet(
		INT nIndex,
		__EXT_MFC_SAFE_LPCTSTR lpszItem		
		);
	INT LbItemInsert(
		__EXT_MFC_SAFE_LPCTSTR lpszItem,
		INT nIndex = -1 // append
		);
	bool LbItemRemove(
		LONG nIndex
		);
	void LbItemRemoveAll();

// Implementation
public:
	virtual ~CExtComboBoxBase();

	void SetTooltipText(
		int nId,
		BOOL bActivate = TRUE
		);
	void SetTooltipText(
		CExtSafeString * spText,
		BOOL bActivate = TRUE
		);
	void SetTooltipText(
		CExtSafeString & sText,
		BOOL bActivate = TRUE
		);
	void SetTooltipText(
		__EXT_MFC_SAFE_LPCTSTR sText,
		BOOL bActivate = TRUE
		);
	void ActivateTooltip(
		BOOL bEnable = TRUE
		);

	void SetBkColor( 
		COLORREF clrBk 
		);
	COLORREF GetBkColor() const;

	void SetTextColor( 
		COLORREF clrText 
		);
	COLORREF GetTextColor() const;

	CEdit * GetInnerEditCtrl() const;

	INT AddStringUnique( 
		__EXT_MFC_SAFE_LPCTSTR lpszString 
		);
	LONG FindItemExact(
		__EXT_MFC_SAFE_LPCTSTR lpszString,
		LONG nIndexStart = -1
		) const;

protected:
	virtual bool _CreateHelper();

	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;

public:
	INT m_nAdvancedTipStyle; // CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON
protected:
	CExtSafeString m_strToolTipText;
	bool m_bToolTipActive:1;
	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet() const;
	virtual void OnAdvancedPopupMenuTipWndDisplay(
		CExtPopupMenuTipWnd & _ATTW,
		const RECT & rcExcludeArea
		) const;

	virtual int OnQueryMaxTipWidth( 
		__EXT_MFC_SAFE_LPCTSTR lpszText 
		);
	virtual void OnSubclassInnerEdit();
	virtual CEdit * OnQueryInnerEditCtrl() const;
	
protected:
	CExtComboBoxFilterPopupListBox * m_pPopupListWnd;
	CSize m_szFilterPopupListBox;

	virtual CExtComboBoxFilterPopupListBox * OnFilterPopupListCreate();
	virtual void OnFilterPopupListClose();
	virtual void OnFilterPopupListSyncContents( 
		CExtSafeString & sFilterText
		);
	virtual void OnFilterPopupListSelEndOK();
	virtual void OnFilterPopupListSelChanged();
	virtual void OnFilterPopupListSyncSelection(
		bool bSetEditSel = true
		);
	virtual CSize OnFilterPopupListQueryMinSize();
	virtual CSize OnFilterPopupListQueryMaxSize();

	virtual bool OnHookSpyPreTranslateMessage(
		MSG * pMSG
		);

	friend class CExtComboBoxFilterPopupListBox;

public:
	virtual HFONT OnQueryFont() const;
	virtual COLORREF OnQueryBackColor() const;
	virtual COLORREF OnQueryTextColor() const;
protected:
	virtual CSize OnPopupListBoxCalcItemExtraSizes(
		LONG nItem
		) const;
	virtual CSize OnPopupListBoxCalcItemCellExtraSizes() const;
	virtual LONG OnPopupListBoxQueryColumnWidth(
		LONG nColNo
		) const;

	virtual void OnPopupListBoxDrawItem( 
		CDC & dc,
		const RECT & rcItem,
		const CSize & szExtra,
		UINT nState,
		const CExtComboBoxBase::LB_ITEM * pLbItem
		);
	virtual void OnPopupListBoxMeasureItem( 
		LPMEASUREITEMSTRUCT lpMIS 
		);

public:
	virtual bool _OnCbLbPreWndProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam );
	virtual void _OnCbLbPostWndProc( LRESULT lResult, UINT message, WPARAM wParam, LPARAM lParam );

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtComboBoxBase)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDIS);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtComboBoxBase)
	afx_msg LRESULT OnCBAddString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCBInsertString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCBResetContent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCBDeleteString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCBSetCurSel(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnEditCtrlUpdate();
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	friend class CExtComboEditCtrlHook;

}; // class __PROF_UIS_API CExtComboBoxBase

/////////////////////////////////////////////////////////////////////////////
// CExtComboBox window

class __PROF_UIS_API CExtComboBox : public CExtComboBoxBase
{
// Construction
public:
	DECLARE_DYNCREATE( CExtComboBox );
	CExtComboBox();
protected:
	CExtComboBox( UINT, CWnd * ) { } // for CExtNCSB template class compilation only
	CExtComboBox( __EXT_MFC_SAFE_LPCTSTR, CWnd * ) { } // for CExtNCSB template class compilation only

// Attributes
public:

protected:
	bool m_bLButtonDown:1;
	bool m_bPainted:1;
	DWORD m_dwLastStateCode;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtComboBox)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtComboBox();

	virtual void AnimationSite_OnProgressShutdownTimer( UINT nTimerID );
	virtual const CExtAnimationParameters *
		AnimationClient_OnQueryAnimationParameters(
			INT eAPT // __EAPT_*** animation type
			) const;
//	virtual bool AnimationClient_CacheNextState(
//		CDC & dc,
//		const RECT & rcAcAnimationTarget,
//		bool bAnimate,
//		INT eAPT // __EAPT_*** animation type
//		);

protected:
	virtual void _OnDrawComboImpl(
		bool bPressed,
		bool bHover,
		CDC * pDC = NULL
		);
	bool IsFocused() const;
	bool IsHovered() const;

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtComboBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CExtCheckComboBox window

class __PROF_UIS_API CExtCheckComboBox : public CExtComboBox
{
public:
	bool m_bUsePM:1;
	DECLARE_DYNCREATE( CExtCheckComboBox );
	CExtCheckComboBox();
	virtual ~CExtCheckComboBox();
	BOOL Create( DWORD dwStyle, const RECT& rect, CWnd * pParentWnd, UINT nID );
	void CheckSet(
		INT nIndex,
		bool bCheck = true
		);
	bool CheckGet( INT nIndex ) const;
	void CheckAll(
		bool bCheck = true
		);
	virtual void OnPaintItemCheck( CDC & dc, const RECT & rcCheck, INT nIndex );
	virtual bool _OnCbLbPreWndProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam );
	virtual CExtComboBoxPopupListBox * OnComboBoxPopupListBoxInstantiate();
protected:
	virtual void _OnDrawComboImpl(
		bool bPressed,
		bool bHover,
		CDC * pDC = NULL
		);
	//{{AFX_VIRTUAL(CExtCheckComboBox)
	protected:
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	virtual void DrawItem( LPDRAWITEMSTRUCT pDIS );
	virtual void MeasureItem( LPMEASUREITEMSTRUCT pMIS );
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	public:
	virtual BOOL PreTranslateMessage( MSG * pMsg );
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtCheckComboBox)
	afx_msg LRESULT OnGetText( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetTextLength( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CExtSafeString m_strText;
	bool m_bHaveText:1, m_bHaveItemHeight:1;
	virtual void _UpdateText();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_COMBO_BOX_H
