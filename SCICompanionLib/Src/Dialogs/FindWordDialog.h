#pragma once


// CFindWordDialog dialog

class CFindWordDialog : public CFindReplaceDialog
{
public:
	CFindWordDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindWordDialog();
    void SetStatus(PCTSTR pszStatus);

// Dialog Data
	enum { IDD = IDD_DIALOGFINDWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    CStatic m_wndStatus;    
    CEdit m_wndWord;
};
