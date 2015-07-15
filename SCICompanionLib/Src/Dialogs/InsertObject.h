#pragma once

// CInsertObject dialog
namespace sci
{
    class ClassDefinition;
}

class CInsertObject : public CExtResizableDialog
{
public:
	CInsertObject(LangSyntax lang, CWnd* pParent = NULL);   // standard constructor
    CString &GetBuffer() { return _strBuffer; }

// Dialog Data
	enum { IDD = IDD_INSERTSCIOBJECT };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    void _SyncSelection();
    void _PrepareControls();
    BOOL _PrepareBuffer();
    void _SetupObjects();
    sci::ClassDefinition *_GetCurrentObject();
	DECLARE_MESSAGE_MAP()

    CString _strBuffer;
    CExtEdit m_wndEditName;
    CExtComboBox m_wndComboType;

    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;

    CExtLabel m_wndStatic3;
    CExtLabel m_wndStatic4;
    CListBox m_wndListProps;
    CListBox m_wndListMethods;

    std::vector<std::unique_ptr<sci::ClassDefinition>> _objects;

    LangSyntax _lang;
public:
    afx_msg void OnCbnSelchangeObjecttype();
};
