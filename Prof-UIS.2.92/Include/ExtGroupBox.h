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

#if (!defined __EXT_GROUPBOX_H)
#define __EXT_GROUPBOX_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CExtGroupBox window

class __PROF_UIS_API CExtGroupBox
	: public CButton
	, public CExtPmBridge
{
// Construction
public:
	DECLARE_DYNAMIC( CExtGroupBox );
	DECLARE_CExtPmBridge_MEMBERS( CExtGroupBox );

	CExtGroupBox();

// Attributes
public:

// Operations
public:

	enum e_style_t
	{
		STYLE_SYSTEM	= 0,
		STYLE_ETCHED	= 1,
		STYLE_ROUNDED	= 2,
		STYLE_FLAT		= 3,
		STYLE_CAPTION	= 4,
	};
	enum e_text_align_t
	{
		ALIGN_DEFAULT	= 0,
		ALIGN_LEFT		= 1,
		ALIGN_RIGHT		= 2,
		ALIGN_CENTER	= 3,
	};

	void SetStyle( e_style_t eStyle );
	e_style_t GetStyle() const;

	void SetTextAlign( e_text_align_t eTextAlign );
	e_text_align_t GetTextAlign() const;

	void SetCaptionHeaderHeight( int nHeight );
	int GetCaptionHeaderHeight() const;
	
	void SetBkColor( 
		COLORREF clrBk 
		);
	COLORREF GetBkColor() const;
	
	void SetTextColor(
		COLORREF clrText = COLORREF(-1L),
		bool bEnabled = true
		);
	COLORREF GetTextColor(
		bool bEnabled = true
		) const;
	
	void SetFrameColor(
		COLORREF clrFrame = COLORREF(-1L),
		bool bEnabled = true
		);
	COLORREF GetFrameColor(
		bool bEnabled = true
		) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtGroupBox)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtGroupBox();

protected:
	e_text_align_t m_eTextAlign;
	e_style_t m_eStyle;
	int m_nCaptionHeaderHeight;
	COLORREF m_clrBackground;
	COLORREF m_clrTextNormal;
	COLORREF m_clrTextDisabled;
	COLORREF m_clrFrameNormal;
	COLORREF m_clrFrameDisabled;
	
	virtual void DoPaint( 
		CDC * pDC,
		CRect & rcClient
		);

	virtual HFONT OnQueryFont() const;

	virtual void OnDrawLabelText(
		CDC & dc,
		CRect& rcText,
		__EXT_MFC_SAFE_LPCTSTR strText,
		DWORD dwDrawTextFlags,
		bool bEnabled
		);
	virtual void OnDrawFrameRect(
		CDC & dc,
		const CRect & rcClient,
		bool bEnabled
		);
	virtual COLORREF OnQueryTextColor(
		bool bEnabled
		) const;
	virtual COLORREF OnQueryFrameColor(
		bool bEnabled
		) const;
		
	// Generated message map functions
protected:
	//{{AFX_MSG(CExtGroupBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
}; // class CExtGroupBox

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_GROUPBOX_H
