/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
// SaveResourceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SaveResourceDialog.h"

using namespace std;

// SaveResourceDialog dialog

SaveResourceDialog::SaveResourceDialog(bool warnOnOverwrite, ResourceType type, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(SaveResourceDialog::IDD, pParent), _iPackageNumber(0), _iResourceNumber(0), _warnOnOverwrite(warnOnOverwrite), _type(type)
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

    DDX_Control(pDX, IDC_EDITNAME, m_wndEditName);
    m_wndEditName.SetWindowText(_name.c_str());
    
    // Visuals
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
    DDX_Control(pDX, IDC_STATIC3, m_wndLabel3);
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
        if (appState->GetResourceMap().GetDefaultResourceSaveLocation() == ResourceSaveLocation::Package)
        {
            AfxMessageBox(TEXT("Please specify a package number between 1 and 63.  Unless this is being distributed on floppy disks, use 1."),
                MB_OK | MB_APPLMODAL | MB_ICONSTOP);
            fRet = FALSE;
        }
    }

    if (fRet && _warnOnOverwrite)
    {
        if (appState->GetResourceMap().DoesResourceExist(_type, _iResourceNumber))
        {
            fRet = (IDYES == AfxMessageBox("A resource already exists for this number. Overwrite?", MB_YESNO | MB_ICONWARNING));
        }
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

    CString strName;
    m_wndEditName.GetWindowText(strName);
    _name = (PCSTR)strName;

    if (_ValidateData())
    {
        __super::OnOK();
    }
}