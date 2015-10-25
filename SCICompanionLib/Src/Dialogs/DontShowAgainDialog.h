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


// CDontShowAgainDialog dialog

class CDontShowAgainDialog : public CExtResizableDialog
{
public:
	CDontShowAgainDialog(PCTSTR pszMessage, int &iValue, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDontShowAgainDialog();

// Dialog Data
	enum { IDD = IDD_DONTSHOWAGAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    PCTSTR _pszMessage;
    int &_iValue;

    CExtLabel m_wndStaticMessage;

    // Visuals
    CExtButton m_wndOK;
    CExtCheckBox m_wndDontShow;
};
