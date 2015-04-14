#pragma once


// SaveResourceDialog dialog
#include "resource.h"

class SaveResourceDialog : public CExtResizableDialog
{
public:
	SaveResourceDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SaveResourceDialog();
    void Init(int iPackageNumber, int iResourceNumber) { _iResourceNumber = iResourceNumber; _iPackageNumber = iPackageNumber; }
    int GetPackageNumber() { return _iPackageNumber; }
    int GetResourceNumber() { return _iResourceNumber; }
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
    CExtEdit m_wndEditPackage;
    CExtEdit m_wndEditResource;

    // Visuals
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
};
