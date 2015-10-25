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
// DontShowAgainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "DontShowAgainDialog.h"


// CDontShowAgainDialog dialog

CDontShowAgainDialog::CDontShowAgainDialog(PCTSTR pszMessage, int &iValue, CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CDontShowAgainDialog::IDD, pParent), _iValue(iValue)
{
    _pszMessage = pszMessage;
}

CDontShowAgainDialog::~CDontShowAgainDialog()
{
}

void CDontShowAgainDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_DONTSHOWAGAIN, _iValue);

    DDX_Control(pDX, IDC_MESSAGE, m_wndStaticMessage);
    m_wndStaticMessage.SetWindowText(_pszMessage);

    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDC_DONTSHOWAGAIN, m_wndDontShow);
}


BEGIN_MESSAGE_MAP(CDontShowAgainDialog, CExtResizableDialog)
END_MESSAGE_MAP()


// CDontShowAgainDialog message handlers
