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

class RunLogic;

// CGamePropertiesDialog dialog

class CGamePropertiesDialog : public CExtNCW<CExtResizableDialog>
{
public:
	CGamePropertiesDialog(RunLogic &runLogic, CWnd* pParent = NULL);   // standard constructor
	virtual ~CGamePropertiesDialog();

// Dialog Data
	enum { IDD = IDD_GAMEPROPERTIESDIALOG };

    CString _strGameName;

protected:
    void OnOK() override;

private:
    void _Update();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnBrowse();
	DECLARE_MESSAGE_MAP()

    CExtButton m_wndBrowse;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtGroupBox m_wndGeneral;
    CExtLabel m_wndGameName;
    CExtLabel m_wndExe;
    CExtLabel m_wndExeParam;
    CExtLabel m_wndStaticProfile;
    CExtLabel m_wndStatic4;
    CExtComboBox m_wndComboLanguage;
    CExtComboBox m_wndComboProfile;
    CExtEdit m_wndEditParams;
    CExtEdit m_wndEditExe;
    RunLogic &_runLogic;

    std::unordered_map<std::string, std::string> _optionToExe;
    std::unordered_map<std::string, std::string> _optionToParams;
    std::unordered_set<std::string> _dirtyOptionsExe;
    std::unordered_set<std::string> _dirtyOptionsParams;
    bool _initialized;

public:
    afx_msg void OnEnKillfocusEditexecutable();
    afx_msg void OnEnKillfocusEditexecutableparameters();
    afx_msg void OnCbnSelchangeComboprofile();
};
