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
