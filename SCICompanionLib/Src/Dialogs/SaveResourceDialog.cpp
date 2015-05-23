// SaveResourceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SaveResourceDialog.h"

// SaveResourceDialog dialog

SaveResourceDialog::SaveResourceDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(SaveResourceDialog::IDD, pParent)
{
}

SaveResourceDialog::~SaveResourceDialog()
{
}

int _GetValidResource(int iResource)
{
    return max(min(iResource, 999), 0);
}

int _GetValidPackage(int iPackage)
{
    return max(min(iPackage, 63), 0);
}


void SaveResourceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    DDX_Control(pDX, IDC_EDITPACKAGE, m_wndEditPackage);
    m_wndEditPackage.LimitText(2);
    TCHAR sz[4];
    StringCchPrintf(sz, ARRAYSIZE(sz), TEXT("%d"), _GetValidPackage(_iPackageNumber));
    m_wndEditPackage.SetWindowText(sz);

    DDX_Control(pDX, IDC_EDITRESOURCE, m_wndEditResource);
    m_wndEditResource.LimitText(3);
    StringCchPrintf(sz, ARRAYSIZE(sz), TEXT("%d"), _GetValidResource(_iResourceNumber));
    m_wndEditResource.SetWindowText(sz);

    // Visuals
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
}


BEGIN_MESSAGE_MAP(SaveResourceDialog, CDialog)
END_MESSAGE_MAP()


// SaveResourceDialog message handlers


BOOL SaveResourceDialog::_ValidateData()
{
    BOOL fRet = TRUE;
    if ((_iResourceNumber < 0) || (_iResourceNumber > appState->GetVersion().GetMaximumResourceNumber()))
    {
        AfxMessageBox(TEXT("Please specify a resource number between 0 and 999"),
                      MB_OK | MB_APPLMODAL | MB_ICONSTOP);
        fRet = FALSE;    
    }
    else if ((_iPackageNumber < appState->GetVersion().DefaultVolumeFile) || (_iPackageNumber > 63))
    {
        AfxMessageBox(TEXT("Please specify a package number between 1 and 63.  Unless this is being distributed on floppy disks, use 1."),
                      MB_OK | MB_APPLMODAL | MB_ICONSTOP);
        fRet = FALSE;
    }
    return fRet;
}

void SaveResourceDialog::OnOK()
{
    CString str;
    m_wndEditPackage.GetWindowText(str);
    _iPackageNumber = StrToInt(str);

    m_wndEditResource.GetWindowText(str);
    _iResourceNumber = StrToInt(str);

    if (_ValidateData())
    {
        __super::OnOK();
    }
}