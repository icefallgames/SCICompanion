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
