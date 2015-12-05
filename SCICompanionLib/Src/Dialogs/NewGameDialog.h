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


// NewGameDialog dialog

class NewGameDialog : public CExtResizableDialog
{
public:
	NewGameDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~NewGameDialog();

// Dialog Data
	enum { IDD = IDD_NEWGAMEDIALOG };

protected:
    void _PopulateTemplates();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    CExtEdit m_wndPath;
    CExtEdit m_wndName;
    CExtComboBox m_wndComboTemplate;
    CExtComboBox m_wndComboLanguage;

    // Visuals
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;
    CExtLabel m_wndStatic4;

    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    CExtButton m_wndBrowse;
    CExtEdit m_wndDescription;
    CStatic m_wndImage;

    std::map<std::string, std::string> _descriptions;
    std::map<std::string, std::unique_ptr<CBitmap>> _bitmaps;
    std::string _defaultTemplate;

public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedButtonbrowse();
    afx_msg void OnCbnSelchangeCombotemplate();
};
