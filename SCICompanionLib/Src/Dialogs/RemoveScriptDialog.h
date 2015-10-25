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


// CRemoveScriptDialog dialog

class CRemoveScriptDialog : public CExtResizableDialog
{
public:
	CRemoveScriptDialog(WORD wScript, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRemoveScriptDialog();

// Dialog Data
	enum { IDD = IDD_REMOVESCRIPT };

    bool AlsoDelete() { return _fAlsoDelete; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnOK();

	DECLARE_MESSAGE_MAP()

    CExtCheckBox m_wndCheckDelete;
    CExtLabel m_wndStaticQuestion;

    CExtButton m_wndYes;
    CExtButton m_wndNo;

    WORD _wScript;

    bool _fAlsoDelete;
};
