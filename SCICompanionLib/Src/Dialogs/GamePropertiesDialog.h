#pragma once


// CGamePropertiesDialog dialog

class CGamePropertiesDialog : public CExtNCW<CExtResizableDialog>
{
public:
	CGamePropertiesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGamePropertiesDialog();

// Dialog Data
	enum { IDD = IDD_GAMEPROPERTIESDIALOG };

    int _fCPPSyntax;
    int _fSCIStudioSyntax;
    CString _strGameName;
    CString _strGameExecutable;
    CString _strGameExecutableParameters;

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnBrowse();
	DECLARE_MESSAGE_MAP()

    CExtButton m_wndBrowse;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtRadioButton m_wndRadioCpp;
    CExtRadioButton m_wndRadioSCIStudio;
    CExtGroupBox m_wndGeneral;
    CExtGroupBox m_wndLanguage;
    CExtLabel m_wndGameName;
    CExtLabel m_wndExe;
    CExtLabel m_wndExeParam;
};
