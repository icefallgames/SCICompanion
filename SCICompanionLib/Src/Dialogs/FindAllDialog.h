/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#pragma once


// CFindAllDialog dialog

class CFindAllDialog : public CExtResizableDialog
{
public:
	CFindAllDialog(int &bMatchWholeWord, int &bMatchCase, int &bFindInAll, CString &strFindWhat, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindAllDialog();

// Dialog Data
	enum { IDD = IDD_DIALOGFINDALL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnEditChange();
    void OnOK();

	DECLARE_MESSAGE_MAP()

    int &m_bMatchWholeWord;
    int &m_bMatchCase;
    int &m_bFindInAll;
    CString &m_strFindWhat;

    CExtRadioButton m_wndRadioAllFiles;
    CExtRadioButton m_wndRadioOpenFiles;
    CExtEdit m_wndEdit;

    // Visuals
    // IDC_GROUPSEARCH
    CExtGroupBox m_wndGroupSearch;
    // ok, cancel
    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    //  IDC_CHECK_MATCHCASE
    // IDC_CHECK_MATCHWHOLEWORD
    CExtCheckBox m_wndMatchCase;
    CExtCheckBox m_wndMatchWholeWord;
    // IDC_STATIC1
    CExtLabel m_wndStatic1;
};
