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

#if (!defined __EXTCOLORDLG_H)
#define __EXTCOLORDLG_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXTCOLORCTRL_H)
	#include <ExtColorCtrl.h>
#endif

#if (!defined __EXT_BUTTON_H)
	#include <ExtButton.h>
#endif

#if (!defined __EXT_COMBO_BOX_H)
	#include <ExtComboBox.h>
#endif

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXT_LABEL_H)
	#include <ExtLabel.h>
#endif

#if (!defined __EXT_SPIN_H)
	#include <ExtSpinWnd.h>
#endif

#if (!defined __EXT_NC_FRAME_H)
	#include <ExtNcFrame.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtColorDlg dialog

#if (!defined __EXT_MFC_NO_SPIN)
	typedef CExtSpinWnd			CExtSpinWndInColorDlg;
	typedef CExtEdit			CExtEditInColorDlg;
#else
	typedef CSpinButtonCtrl		CExtSpinWndInColorDlg;
	typedef CEdit				CExtEditInColorDlg;
#endif

#if (!defined __EXT_MFC_NO_NC_FRAME)
	#define __BASEOF_CExtColorDlg__ CExtNCW < CExtResizableDialog >
#else
	#define __BASEOF_CExtColorDlg__ CExtResizableDialog
#endif

#if _MFC_VER == 0x700
	#ifndef __EXT_PROFUIS_STATIC_LINK
		template class __PROF_UIS_API __BASEOF_CExtColorDlg__;
	#endif  // #ifndef __EXT_PROFUIS_STATIC_LINK
#endif

class __PROF_UIS_API CExtColorDlg
	: public __BASEOF_CExtColorDlg__
{
// Construction
public:
	DECLARE_DYNAMIC( CExtColorDlg );
	CExtColorDlg(
		COLORREF clrInit = RGB(255,255,255),
		COLORREF clrNew = RGB(255,255,255),
		CWnd * pParent = NULL
		);	// standard constructor
	virtual ~CExtColorDlg();

	COLORREF m_clrInit,m_clrNew;
	CExtSafeString m_strCaption; // if non-empty - used to setup color dialog caption
	CExtSafeString m_strSection, m_strEntryDialogPosition, m_strEntryColorModelSelection;

// Dialog Data
	//{{AFX_DATA(CExtColorDlg)
	CExtSpinWndInColorDlg	m_SpinC;
	CExtSpinWndInColorDlg	m_SpinM;
	CExtSpinWndInColorDlg	m_SpinY;
	CExtSpinWndInColorDlg	m_SpinS;
	CExtSpinWndInColorDlg	m_SpinR;
	CExtSpinWndInColorDlg	m_SpinL;
	CExtSpinWndInColorDlg	m_SpinH;
	CExtSpinWndInColorDlg	m_SpinG;
	CExtSpinWndInColorDlg	m_SpinB;
	CExtEditInColorDlg	m_EditC;
	CExtEditInColorDlg	m_EditM;
	CExtEditInColorDlg	m_EditY;
	CExtEditInColorDlg	m_EditS;
	CExtEditInColorDlg	m_EditR;
	CExtEditInColorDlg	m_EditL;
	CExtEditInColorDlg	m_EditH;
	CExtEditInColorDlg	m_EditG;
	CExtEditInColorDlg	m_EditB;
	CExtComboBox	m_ComboType;
	CExtButton	m_BtnCancel;
	CExtButton	m_BtnOK;
	CExtLabel m_labelColors;
	CExtLabel m_labelChR;
	CExtLabel m_labelChG;
	CExtLabel m_labelChB;
	CExtLabel m_labelChC;
	CExtLabel m_labelChM;
	CExtLabel m_labelChY;
	CExtLabel m_labelChH;
	CExtLabel m_labelChS;
	CExtLabel m_labelChL;
	CExtLabel m_labelTxtNew;
	CExtLabel m_labelTxtCurrent;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();
	virtual void OnCancel();

// Implementation
protected:
	bool m_bDialogInitColplete,m_bSyncingValues;
	HICON m_hIcon;
	CExtColorCtrl m_wndColorCtrl,m_wndColorDiff;

	virtual bool OnQueryAutomaticRTLTransform() const
	{
		return true;
	}
	virtual bool _ColorModelLoad();
	virtual bool _ColorModelSave();

	// Generated message map functions
	//{{AFX_MSG(CExtColorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelendokComboSelType();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	LRESULT OnColorChanged(WPARAM wParam,LPARAM lParam);
	afx_msg void SyncFromEditRGB();
	afx_msg void SyncFromEditCMY();
	afx_msg void SyncFromEditHSL();
	DECLARE_MESSAGE_MAP()

private:
	void SyncToEditRGB(
		COLORREF clr,
		bool bSyncRGB,
		bool bSyncCMY,
		bool bSyncHLS
		);
public:
	virtual __EXT_MFC_INT_PTR DoModal();
}; // class CExtColorDlg

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXTCOLORDLG_H

