#pragma once


// SaveResourceDialog dialog
#include "resource.h"

class SaveResourceDialog : public CExtResizableDialog
{
public:
	SaveResourceDialog(bool warnOnOverwrite, ResourceType type, CWnd* pParent = NULL);   // standard constructor
	virtual ~SaveResourceDialog();
    void Init(int iPackageNumber, int iResourceNumber) { _iResourceNumber = iResourceNumber; _iPackageNumber = iPackageNumber; }
    void Init(int iPackageNumber, int iResourceNumber, const std::string &name) { _iResourceNumber = iResourceNumber; _iPackageNumber = iPackageNumber; _name = name; }
    int GetPackageNumber() { return _iPackageNumber; }
    int GetResourceNumber() { return _iResourceNumber; }
    std::string GetName() { return _name; }
    virtual void OnOK();

// Dialog Data
	enum { IDD = IDD_RESOURCENUM };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnEditPackage();
    void OnEditResource();
    BOOL _ValidateData();

	DECLARE_MESSAGE_MAP()

    int _iResourceNumber;
    int _iPackageNumber;
    std::string _name;

    bool _warnOnOverwrite;
    ResourceType _type;

    CExtEdit m_wndEditPackage;
    CExtEdit m_wndEditResource;
    CExtEdit m_wndEditName;

    // Visuals
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
};
