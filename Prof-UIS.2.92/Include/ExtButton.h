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

#if (!defined __EXT_BUTTON_H)
#define __EXT_BUTTON_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_MOUSECAPTURESINK_H)
	#include <../Src/ExtMouseCaptureSink.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_CONTROLBAR_H)
	#include <ExtControlBar.h>
#endif

class CExtPopupMenuWnd;
class CExtPopupMenuTipWnd;

/////////////////////////////////////////////////////////////////////////////
// CExtButton window

class __PROF_UIS_API CExtButton
	: public CButton
	, public CExtMouseCaptureSink
	, public CExtAnimationSingleton
{
public:
	DECLARE_DYNCREATE( CExtButton );
	DECLARE_PROF_UIS_WINDOW_METHODS;
    CExtButton();
	virtual ~CExtButton();
	CMenu m_menu;
	bool m_bNoRefToCmdMngr:1;
	DWORD m_dwMenuOpt; // TPMX_TOPALIGN is default
	UINT m_nMenuLeftAreaWidth; // 0 is default
	INT m_nAdvancedTipStyle; // CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON
	virtual void _OnClick(
		bool bSelectAny,
		bool bSeparatedDropDownClicked
		);
	virtual bool _OnTrackPopup(
		bool bSelectAny
		);
	virtual bool _IsMenuAvail();
	virtual LONG _CalcSplitButtonExtent() const;
	void _CalcClientRects(
		CRect & rcMainArea,
		CRect & rcDropDown
		) const;
	INT _HtMouseOnButton( const POINT & ptClient ) const; // 0 - not, 1-main area, 2-dropdown area
	INT _HtMouseOnButton() const; // 0 - not, 1-main area, 2-dropdown area
	virtual bool OnPutToPopupMenu( CExtPopupMenuWnd * pPopup );
	virtual bool _QuickActivationHandle(
		bool bCheckWindowState = true
		);
	virtual bool _QuickActivationCheck(
		DWORD dwChar,
		bool bCheckWindowState = true
		);
	bool m_bQuickActivationEnabled:1, m_bQuickActivationSeparatedDropDown:1;
protected:
	bool m_bQuickActivationCancelNearest:1;
public:
    //{{AFX_VIRTUAL(CExtButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

public:
	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);
	virtual void AnimationSite_OnProgressShutdownTimer( UINT nTimerID );
	virtual HWND AnimationSite_GetSafeHWND() const;
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

// Implementation
public:
	virtual BOOL GetDefault(
		bool bInitial = true
		) const;
	virtual BOOL SupportsCommand() const;
	virtual BOOL SupportsCheck() const;
	virtual BOOL SupportsAutoCheck() const;
	virtual INT GetType() const;
	virtual BOOL Is3StateType() const;

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

	BOOL SetBtnCursor( 
		int nCursorId = -1 
		);

	BOOL GetSeparatedDropDown() const;
	void SetSeparatedDropDown(
		BOOL bSeparatedDropDown = TRUE
		);

	virtual BOOL GetAutoChangeID() const;
	virtual void SetAutoChangeID(
		BOOL bAutoChangeID = TRUE
		);
	virtual BOOL GetAutoChangeCmdIcon() const;
	virtual void SetAutoChangeCmdIcon(
		BOOL bAutoChangeCmdIcon = TRUE
		);

	virtual void SetFlat( BOOL bState = TRUE );
	virtual BOOL GetFlat() const;

	virtual BOOL GetDrawBorder() const;
	virtual void SetDrawBorder( BOOL bEnable = TRUE );

	virtual BOOL GetDrawFocusRect() const;
	virtual void SetDrawFocusRect( BOOL bEnable = TRUE );

	// for back compatibility
	void SetIcon(
		__EXT_MFC_SAFE_LPCTSTR sResourceIconInID,
		__EXT_MFC_SAFE_LPCTSTR sResourceIconOutID = NULL,
		__EXT_MFC_SAFE_LPCTSTR sResourceIconDisabledID = NULL,
		__EXT_MFC_SAFE_LPCTSTR sResourceIconPressedID = NULL,
		INT nWidth = 0,
		INT nHeight = 0
		);
	// for back compatibility
	void SetIcon(
		UINT nResourceIconInID,
		UINT nResourceIconOutID = 0,
		UINT nResourceIconDisabledID = 0,
		UINT nResourceIconPressedID = 0,
		INT nWidth = 0,
		INT nHeight = 0
		);
	// for back compatibility
	void SetIcon(
		HICON hIconIn,
		HICON hIconOut = NULL,
		HICON hIconDisabled = NULL,
		HICON hIconPressed = NULL
		);

	void SetTextColorDisabled( 
		COLORREF clrText 
		);
	void SetTextColorNormal( 
		COLORREF clrText 
		);
	void SetTextColorHover( 
		COLORREF clrText 
		);
	void SetTextColorPressed( 
		COLORREF clrText 
		);
	void SetBkColor( 
		COLORREF clrBk 
		);

	COLORREF GetTextColorDisabled() const;
	COLORREF GetTextColorNormal() const;
	COLORREF GetTextColorHover() const;
	COLORREF GetTextColorPressed() const;
	COLORREF GetBkColor() const;

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

	INT SetFocusMode(
		INT nFocusMode
		);
	INT GetFocusMode() const;

protected:
    //{{AFX_MSG(CExtButton)
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus( CWnd * pOldWnd );
	afx_msg void OnKillFocus( CWnd * pNewWnd );
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnCancelMode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif
	afx_msg LRESULT _OnPutToPopupMenuImpl(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

protected:
	static void _CbPaintCombinedContent(
		LPVOID pCookie,
		CDC & dc,
		const CWnd & refWndMenu,
		const CRect & rcExcludeArea, // in screen coords
		int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
		);
	virtual void _DoClick();

public:
	virtual void _DrawImpl( // buffered paint
		CDC & dc,
		bool bTransparent = false,
		bool bCombinedContent = false
		);
	virtual void _RenderImpl( // non-buffered paint
		CDC & dc,
		bool bTransparent = false,
		bool bCombinedContent = false
		);

protected:
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;

	virtual int OnQueryMaxTipWidth( 
		__EXT_MFC_SAFE_LPCTSTR lpszText 
		);

	void InitToolTip();
	virtual void OnQueryWindowText(
		CExtSafeString & strWindowText
		) const;
	virtual HFONT OnQueryFont() const;
	
	virtual CRect OnQueryBorderSizes() const;

	virtual INT OnQueryAlignmentText() const;
	virtual INT OnQueryAlignmentIcon() const;
	virtual CRect OnQueryMarginsText() const;
	virtual CRect OnQueryMarginsIcon() const;

	INT m_nIconAlignment;

	CRect m_rcIconMargins;
	CRect m_rcTextMargins;
	
	INT m_nFocusMode;

	bool m_bDefaultButton:1;

	INT m_nButtonType, m_nCheck;
	bool m_bDrawFocusRect:1, m_bDrawBorder:1, m_bMouseOver:1, m_bPushed:1,
		m_bPushTracking:1, m_bKeyTracking:1, m_bSeparatedDropDown:1,
		m_bAutoChangeID:1, m_bAutoChangeCmdIcon:1, m_bState:1;
	INT m_nTrackingHT;

    bool m_bIconOnly:1; // STAR - icon only

	HCURSOR m_hCursor;
	CToolTipCtrl m_wndToolTip;

// 	HICON m_hIconIn;
// 	HICON m_hIconOut;

public:
	CExtCmdIcon m_icon;

protected:
	COLORREF m_clrBackground;
	COLORREF m_clrTextDisabled;
	COLORREF m_clrTextNormal;
	COLORREF m_clrTextHover;
	COLORREF m_clrTextPressed;

	CExtSafeString m_strToolTipText;
	bool m_bToolTipActive:1;
	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet() const;
	virtual void OnAdvancedPopupMenuTipWndDisplay(
		CExtPopupMenuTipWnd & _ATTW,
		const RECT & rcExcludeArea
		) const;
}; // class CExtButton

/////////////////////////////////////////////////////////////////////////////

class __PROF_UIS_API CExtColorButton : public CExtButton
{
	DECLARE_DYNCREATE(CExtColorButton);

// Construction
public:
    CExtColorButton();
	virtual ~CExtColorButton();

	CExtSafeString m_sBtnTextColorDefault;
	CExtSafeString m_sBtnTextColorCustom;

// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CExtColorButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	bool m_bEnableBtnColorDefault;
	bool m_bEnableBtnColorCustom;
	bool m_bUseStdColorDlg;
	bool m_bTrackColorPickerSelection;
	COLORREF m_clrDefault, m_clrSelected, m_clrLastHover;
	LPARAM m_lParamCookie;

	virtual void OnGenerateColorBtnIcon(
		CExtCmdIcon & icon
		);

protected:
	virtual bool _OnTrackPopup(
		bool bSelectAny
		);
	virtual bool _IsMenuAvail();
	virtual void _RenderImpl( // non-buffered paint
		CDC & dc,
		bool bTransparent = false,
		bool bCombinedContent = false
		);
	virtual void _OnClick(
		bool bSelectAny,
		bool bSeparatedDropDownClicked
		);
	
    //{{AFX_MSG(CExtColorButton)
	//}}AFX_MSG
	afx_msg LRESULT OnColorChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnColorChangedFinally(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnColorSelectCustom(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

}; // class CExtColorButton

__PROF_UIS_API void AFXAPI __EXT_MFC_DDX_ColorButton( CDataExchange * pDX, INT nIDC, COLORREF & value );

/////////////////////////////////////////////////////////////////////////////
// CExtCheckBox window

class __PROF_UIS_API CExtCheckBox : public CExtButton
{
public:
	DECLARE_DYNCREATE( CExtCheckBox );
	CExtCheckBox();
	virtual ~CExtCheckBox();

protected:
	virtual void _OnClick(
		bool bSelectAny,
		bool bSeparatedDropDownClicked
		);
	virtual void _RenderImpl( // buffered paint
		CDC & dc,
		bool bTransparent = false,
		bool bCombinedContent = false
		);
	
protected:
	virtual bool OnPaintBox(
		CDC & dc,
		CRect & rcBox,
		CExtPaintManager::eBoxState_t eState
		);
	virtual INT OnQueryAlignmentText() const;
	
	// Overrides
	//{{AFX_VIRTUAL(CExtCheckBox)
	//}}AFX_VIRTUAL
	
	//{{AFX_MSG(CExtCheckBox)
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

}; // class CExtCheckBox

/////////////////////////////////////////////////////////////////////////////
// CExtRadioButton window

class __PROF_UIS_API CExtRadioButton : public CExtCheckBox
{
public:
	DECLARE_DYNCREATE( CExtRadioButton );
	CExtRadioButton();
	virtual ~CExtRadioButton();
	
protected:
	
	virtual void _OnClick(
		bool bSelectAny,
		bool bSeparatedDropDownClicked
		);
	virtual void _RenderImpl( // buffered paint
		CDC & dc,
		bool bTransparent = false,
		bool bCombinedContent = false
		);

	// Overrides
	//{{AFX_VIRTUAL(CExtRadioButton)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
	//{{AFX_MSG(CExtRadioButton)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	
	bool _CheckNextPrevRadioButton( bool bNext );
	void _UncheckRadioButtonsInGroup();
	
	virtual bool OnPaintBox(
		CDC & dc,
		CRect & rcBox,
		CExtPaintManager::eBoxState_t eState
		);
		
}; // class CExtRadioButton

/////////////////////////////////////////////////////////////////////////////
// CExtHyperLinkButton window

class __PROF_UIS_API CExtHyperLinkButton : public CExtButton
{
public:
	DECLARE_DYNCREATE( CExtHyperLinkButton );
	CExtHyperLinkButton();
	virtual ~CExtHyperLinkButton();

	// target URL, something like http://..., file://...,
	// www...., web...., mailto:..., C:\\..., filename.ext
	// (if it is empty, then window text, i.e. button caption,
	// is used as the target URL)
	CExtSafeString m_strURL;
	// use WM_COMMAND instead of OnHyperLinkOpen()
	// (by default is not set; set it to true if
	// hyperlink should work as push button)
	bool m_bUseStdCommandNotification:1;
	// allow visited state highlighting
	bool m_bAllowVisited:1;
	// is visited
	mutable bool m_bVisited:1;
	
	void SetTextColorVisited( 
		COLORREF clrText 
		);
	COLORREF GetTextColorVisited();

	void SetTextUnderline( 
		bool bTextUnderline 
		);
	bool GetTextUnderline() const;

	bool m_bAutoAdjustSizeByText:1;
protected:
	virtual void _OnClick(
		bool bSelectAny,
		bool bSeparatedDropDownClicked
		);
	virtual void _RenderImpl( // non-buffered paint
		CDC & dc,
		bool bTransparent = false,
		bool bCombinedContent = false
		);
public:
	enum e_OpenMode_t
	{
		__EOM_OPEN = 0,
		__EOM_EDIT = 1,
		__EOM_PRINT = 2,
	};

protected:
	COLORREF m_clrTextVisited;
	bool m_bTextUnderline:1;

	virtual bool _OpenVia_COM(
		__EXT_MFC_SAFE_LPCTSTR strURL,
		CExtHyperLinkButton::e_OpenMode_t eOM = CExtHyperLinkButton::__EOM_OPEN,
		bool bAllowErrorAlert = false
		) const;
	virtual bool _OpenVia_Shell(
		__EXT_MFC_SAFE_LPCTSTR strURL,
		CExtHyperLinkButton::e_OpenMode_t eOM = CExtHyperLinkButton::__EOM_OPEN,
		bool bAllowErrorAlert = false
		) const;
	virtual bool _OpenVia_RegInfo(
		__EXT_MFC_SAFE_LPCTSTR strURL,
		CExtHyperLinkButton::e_OpenMode_t eOM = CExtHyperLinkButton::__EOM_OPEN,
		bool bAllowErrorAlert = false
		) const;
public:
	static bool CExtHyperLinkButton::stat_HyperLinkOpen(
		__EXT_MFC_SAFE_LPCTSTR strURL,
		CExtHyperLinkButton::e_OpenMode_t eOM = CExtHyperLinkButton::__EOM_OPEN,
		bool bAllowErrorAlert = false
		);
	static bool stat_OpenVia_COM(
		__EXT_MFC_SAFE_LPCTSTR strURL,
		CExtHyperLinkButton::e_OpenMode_t eOM = CExtHyperLinkButton::__EOM_OPEN,
		bool bAllowErrorAlert = false
		);
	static bool stat_OpenVia_Shell(
		__EXT_MFC_SAFE_LPCTSTR strURL,
		CExtHyperLinkButton::e_OpenMode_t eOM = CExtHyperLinkButton::__EOM_OPEN,
		bool bAllowErrorAlert = false
		);
	static bool stat_OpenVia_RegInfo(
		__EXT_MFC_SAFE_LPCTSTR strURL,
		CExtHyperLinkButton::e_OpenMode_t eOM = CExtHyperLinkButton::__EOM_OPEN,
		bool bAllowErrorAlert = false
		);

	virtual void OnPaintHyperLink(
		CDC & dc,
		CRect rcText,
		bool bVisited,
		bool bEnabled,
		bool bDrawFocusRect,
		bool bFocus,
		bool bHover,
		bool bPushed
		) const;
	virtual COLORREF OnQueryHyperLinkTextColor(
		bool bVisited,
		bool bEnabled,
		bool bDrawFocusRect,
		bool bFocus,
		bool bHover,
		bool bPushed
		) const;
	virtual CExtSafeString OnQueryHyperLinkURL(
		bool bDisplayURL // true - display URL, false - navigate URL
		) const;
	virtual bool OnHyperLinkOpen(
		__EXT_MFC_SAFE_LPCTSTR strURL,
		CExtHyperLinkButton::e_OpenMode_t eOM = CExtHyperLinkButton::__EOM_OPEN,
		bool bAllowErrorAlert = false
		) const;
	
	// Overrides
	//{{AFX_VIRTUAL(CExtHyperLinkButton)
	//}}AFX_VIRTUAL
	
protected:
	
    //{{AFX_MSG(CExtHyperLinkButton)
	//}}AFX_MSG
	afx_msg LRESULT OnSetText( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
		
}; // class CExtHyperLinkButton

/////////////////////////////////////////////////////////////////////////////
// CExtIconButton

class __PROF_UIS_API CExtIconButton : public CExtButton
{
public:
	DECLARE_DYNCREATE( CExtIconButton );
	CExtIconButton();
	virtual ~CExtIconButton();

	CRect m_rcFocusRectPadding;

	// Overrides
	//{{AFX_VIRTUAL(CExtIconButton)
	//}}AFX_VIRTUAL
	
	//{{AFX_MSG(CExtIconButton)
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
protected:
	virtual void _RenderImpl( // buffered paint
		CDC & dc,
		bool bTransparent = false,
		bool bCombinedContent = false
		);
}; /// class CExtIconButton

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif /// __EXT_BUTTON_H
