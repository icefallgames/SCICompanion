#pragma once

// CInsertObject dialog

class CInsertObject : public CExtResizableDialog
{
public:
	CInsertObject(LangSyntax lang, CWnd* pParent = NULL);   // standard constructor
	virtual ~CInsertObject();
    CString &GetBuffer() { return _strBuffer; }

// Dialog Data
	enum { IDD = IDD_INSERTSCIOBJECT };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    void _PrepareControls();
    BOOL _PrepareBuffer();
	DECLARE_MESSAGE_MAP()

    CString _strBuffer;
    CExtEdit m_wndEditName;
    CExtComboBox m_wndComboType;
    CExtCheckBox m_wndCheckScript;

    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;

    LangSyntax _lang;
};
