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
