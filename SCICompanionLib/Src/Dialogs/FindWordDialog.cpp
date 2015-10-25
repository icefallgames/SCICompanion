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
// FindWordDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "FindWordDialog.h"
#include "dlgs.h"

// CFindWordDialog dialog
CFindWordDialog::CFindWordDialog(CWnd* pParent /*=NULL*/)
	: CFindReplaceDialog()
{
    // REVIEW: this does not work!!
    this->m_fr.hInstance = AfxGetInstanceHandle();
    this->m_fr.Flags |= FR_ENABLETEMPLATE;
    this->m_fr.lpTemplateName = MAKEINTRESOURCE(IDD_DIALOGFINDWORD);
}

CFindWordDialog::~CFindWordDialog()
{
}

void CFindWordDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATICSTATUS, m_wndStatus);
    DDX_Control(pDX, edt1, m_wndWord);
}

void CFindWordDialog::SetStatus(PCTSTR pszStatus)
{
    m_wndStatus.SetWindowText(pszStatus);
    m_wndWord.SetSel(0, -1, FALSE);
}

BEGIN_MESSAGE_MAP(CFindWordDialog, CFindReplaceDialog)
END_MESSAGE_MAP()


// CFindWordDialog message handlers
