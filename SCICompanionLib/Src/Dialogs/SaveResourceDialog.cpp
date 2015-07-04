// SaveResourceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SaveResourceDialog.h"

// SaveResourceDialog dialog

SaveResourceDialog::SaveResourceDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(SaveResourceDialog::IDD, pParent), _iPackageNumber(0), _iResourceNumber(0)
{
}

SaveResourceDialog::~SaveResourceDialog()
{
}

int _GetValidResource(int iResource)
{
    return max(min(iResource, appState->GetVersion().GetMaximumResourceNumber()), 0);
}

int _GetValidPackage(int iPackage)
{
    return max(min(iPackage, 63), 0);
}


void SaveResourceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    TCHAR sz[10];
    if (_iPackageNumber == -1)
    {
        _iPackageNumber = appState->GetVersion().DefaultVolumeFile;
    }
    _iPackageNumber = _GetValidPackage(_iPackageNumber);
    
    DDX_Control(pDX, IDC_EDITPACKAGE, m_wndEditPackage);
    m_wndEditPackage.LimitText(2);
    StringCchPrintf(sz, ARRAYSIZE(sz), TEXT("%d"), _iPackageNumber);
    m_wndEditPackage.SetWindowText(sz);
    // In SCI11, resources always go in the same package file. So disable being able to edit this.
    if (appState->GetVersion().MapFormat >= ResourceMapFormat::SCI11)
    {
        m_wndEditPackage.EnableWindow(FALSE);
    }

    DDX_Control(pDX, IDC_EDITRESOURCE, m_wndEditResource);
    m_wndEditResource.LimitText(5);
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
    // The audio map is resource 65535, so allow that through.
    if ((_iResourceNumber < 0) || (_iResourceNumber > appState->GetVersion().GetMaximumResourceNumber() && _iResourceNumber != 0xFFFF))
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