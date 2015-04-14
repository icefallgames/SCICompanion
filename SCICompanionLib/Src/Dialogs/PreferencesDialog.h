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
    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    void _SyncBrowseInfo();

    CEdit m_wndFakeEgoX;
    CEdit m_wndFakeEgoY;

    BOOL _fBrowseInfoStart;

    // Visuals
    CExtCheckBox m_wndBrowserInfo;
    CExtCheckBox m_wndCodeCompletion;
    CExtCheckBox m_wndParamInfo;
    CExtCheckBox m_wndHoverTips;
    CExtGroupBox m_wndGroup1;

    CExtCheckBox m_wndCheck1;
    CExtCheckBox m_wndCheck2;
    CExtCheckBox m_wndCheck3;
    CExtCheckBox m_wndCheck4;
    CExtCheckBox m_wndCheck5;
    CExtCheckBox m_wndCheck6;
    CExtCheckBox m_wndCheck7;
    CExtCheckBox m_wndCheck8;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;

    CComboBox m_wndMIDIDevices;
    CExtLabel m_wndStatic1;

public:
    afx_msg void OnBnClickedBrowseinfo();
};
