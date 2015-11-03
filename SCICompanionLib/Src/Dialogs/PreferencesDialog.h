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


// CPreferencesDialog dialog

class CPreferencesDialog : public CExtResizableDialog
{
public:
	CPreferencesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreferencesDialog();

// Dialog Data
	enum { IDD = IDD_PREFERENCES };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

	DECLARE_MESSAGE_MAP()

    void _SyncBrowseInfo();

    CEdit m_wndFakeEgoX;
    CEdit m_wndFakeEgoY;

    BOOL _fBrowseInfoStart;
    BOOL _fAspectRatioStart;

    // Visuals
    CExtCheckBox m_wndBrowserInfo;
    CExtCheckBox m_wndCodeCompletion;
    CExtCheckBox m_wndScriptNav;
    CExtCheckBox m_wndHoverTips;
    CExtGroupBox m_wndGroup1;

    CExtCheckBox m_wndCheck1;
    CExtCheckBox m_wndCheck3;
    CExtCheckBox m_wndCheck4;
    CExtCheckBox m_wndCheck5;
    CExtCheckBox m_wndCheck6;
    CExtCheckBox m_wndCheck7;
    CExtCheckBox m_wndCheck8;
    CExtCheckBox m_wndCheck9;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;

    CComboBox m_wndMIDIDevices;

public:
    afx_msg void OnBnClickedBrowseinfo();
};
