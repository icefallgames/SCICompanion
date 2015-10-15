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

    // Visuals
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;
    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    CExtButton m_wndBrowse;
    CExtEdit m_wndDescription;

    std::map<std::string, std::string> _descriptions;

public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedButtonbrowse();
    afx_msg void OnCbnSelchangeCombotemplate();
};
