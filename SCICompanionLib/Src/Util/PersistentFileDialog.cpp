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
#include "stdafx.h"
#include "PersistentFileDialog.h"

namespace
{
    const TCHAR c_szSettingsSection[] = _T("Settings");
    const TCHAR c_szLastFolderKey[] = _T("LastBrowsedFolder");
}

CString GetLastUsedFolder()
{
    return AfxGetApp()->GetProfileString(c_szSettingsSection, c_szLastFolderKey, _T(""));
}

void SetLastUsedFolder(const CString &folder)
{
    AfxGetApp()->WriteProfileString(c_szSettingsSection, c_szLastFolderKey, folder);
}

CPersistentFileDialog::CPersistentFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
    DWORD dwFlags, LPCTSTR lpszFilter, CWnd *pParentWnd)
    : CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
    _initialDir = GetLastUsedFolder();
    if (!_initialDir.IsEmpty())
    {
        m_ofn.lpstrInitialDir = _initialDir;
    }
}

INT_PTR CPersistentFileDialog::DoModal()
{
    INT_PTR result = CFileDialog::DoModal();
    if (result == IDOK)
    {
        CString folder = GetFolderPath();
        if (!folder.IsEmpty())
        {
            SetLastUsedFolder(folder);
        }
    }
    return result;
}
