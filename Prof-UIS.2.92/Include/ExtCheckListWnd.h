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

#if (!defined __EXT_CHECK_LIST_H)
#define __EXT_CHECK_LIST_H

#if (!defined __EXT_MFC_NO_CHECK_LIST)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CExtCheckListWnd window

class __PROF_UIS_API CExtCheckListWnd
	: public CListBox
	, public CExtPmBridge
{

// Construction
public:
	DECLARE_DYNAMIC( CExtCheckListWnd )
	DECLARE_CExtPmBridge_MEMBERS( CExtCheckListWnd );

	bool m_bUsePM:1;

	CExtCheckListWnd();

// Attributes
public:
	struct EXT_ITEM_DATA
	{
	public:
		INT m_nCheck;
		BOOL m_bEnabled;
		DWORD m_dwUserData;

		EXT_ITEM_DATA()
		{
			m_nCheck = 0;
			m_bEnabled = TRUE;
			m_dwUserData = 0;
		};
	};

	void SetCheckStyle( 
		DWORD nStyle 
		);
	DWORD GetCheckStyle() const;

	void SetCheck(
		INT nIndex, 
		INT nCheck
		);
	INT GetCheck(
		INT nIndex
		);
	
	void Enable(
		INT nIndex, 
		BOOL bEnabled = TRUE
		);
	BOOL IsEnabled(
		INT nIndex
		);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtCheckListWnd)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation 
public: 
	virtual ~CExtCheckListWnd();
	
	virtual HFONT OnQueryItemFont( 
		INT nIndex 
		) const;
	virtual CRect OnQueryItemMargins( 
		INT nIndex 
		) const;
	virtual CRect OnQueryItemCheckMargins( 
		INT nIndex 
		) const;
	virtual CSize OnQueryItemCheckSize( 
		CDC & dc, 
		INT nIndex 
		);
	virtual void OnPaintItemCheck(
		CDC & dc,
		const RECT & rcCheck,
		INT nIndex
		);
	
protected:
	DWORD m_nStyle;
public: 
	void SetSelectionCheck( 
	   INT nCheck 
	   );
	INT CheckBoxFromPoint(
		CPoint point, 
		BOOL & bInCheck
		);

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtCheckListWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnLBAddString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBFindString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBFindStringExact(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBGetItemData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBInsertString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBSelectString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBSetItemData(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_MFC_NO_CHECK_LIST)

#endif // (!defined __EXT_CHECK_LIST_H)
