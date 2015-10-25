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
// FindAllDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "FindAllDialog.h"
#include "resource.h"

// CFindAllDialog dialog
CFindAllDialog::CFindAllDialog(int &bMatchWholeWord, int &bMatchCase, int &bFindInAll, CString &strFindWhat, CWnd* pParent)
: CExtResizableDialog(CFindAllDialog::IDD, pParent), m_bMatchWholeWord(bMatchWholeWord), m_bMatchCase(bMatchCase), m_bFindInAll(bFindInAll), m_strFindWhat(strFindWhat)
{
}

CFindAllDialog::~CFindAllDialog()
{
}

void CFindAllDialog::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_CHECK_MATCHCASE, m_bMatchCase);
    DDX_Check(pDX, IDC_CHECK_MATCHWHOLEWORD, m_bMatchWholeWord);
    DDX_Text(pDX, IDC_EDIT_FINDWHAT, m_strFindWhat);

    // Data exchange doesn't seem to work with radio buttons, so do this:
    DDX_Control(pDX, IDC_RADIO_ALLFILES, m_wndRadioAllFiles);
    m_wndRadioAllFiles.SetCheck(m_bFindInAll);
    DDX_Control(pDX, IDC_RADIO_OPENFILES, m_wndRadioOpenFiles);
    m_wndRadioOpenFiles.SetCheck(m_bFindInAll ? BST_UNCHECKED : BST_CHECKED);

    DDX_Control(pDX, IDC_EDIT_FINDWHAT, m_wndEdit);

    OnEditChange();

    DDX_Control(pDX, IDC_GROUPSEARCH, m_wndGroupSearch);
    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_CHECK_MATCHCASE, m_wndMatchCase);
    DDX_Control(pDX, IDC_CHECK_MATCHWHOLEWORD, m_wndMatchWholeWord);
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
}


BEGIN_MESSAGE_MAP(CFindAllDialog, CExtResizableDialog)
    ON_EN_CHANGE(IDC_EDIT_FINDWHAT, OnEditChange)
END_MESSAGE_MAP()

void CFindAllDialog::OnEditChange()
{
    CWnd *pWnd = GetDlgItem(IDOK);
    if (pWnd)
    {
        CString strText;
        m_wndEdit.GetWindowText(strText);
        pWnd->EnableWindow(!strText.IsEmpty());
    }
}

void CFindAllDialog::OnOK()
{
    // Data exchange doesn't seem to work with radio buttons, so do this:
    m_bFindInAll = m_wndRadioAllFiles.GetCheck();
    __super::OnOK();
}

// CFindAllDialog message handlers
