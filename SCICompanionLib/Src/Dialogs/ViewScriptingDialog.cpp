/***************************************************************************
Copyright (c) 2019 Philip Fortier

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
#include "AppState.h"
#include "ViewScriptingDialog.h"
#include "Components.h"
#include "WindowsUtil.h"
#include <filesystem>
#include <regex>
#include <format.h>

using namespace std::tr2::sys;
using namespace fmt;
using namespace std;

// ViewScriptingDialog dialog

ViewScriptingDialog::ViewScriptingDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(ViewScriptingDialog::IDD, pParent), _initialized(false), _number(-1)
{
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPALETTE), RT_ACCELERATOR);
    _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPALETTE));
}

void ViewScriptingDialog::SetNumber(int number)
{
    _number = number;
    std::string filename = appState->GetResourceMap().Helper().GetViewScriptFilename(_number);
    
    if (PathFileExists(filename.c_str()))
    {
        CStdioFile file;
        file.Open(filename.c_str(), CFile::modeRead | CFile::typeText);

        CString buffer;
        CString textfile;
        while (file.ReadString(textfile))
        {
            buffer += textfile;
            buffer += "\r\n";
        }

        file.Close();
        m_wndEditCode.SetWindowText(buffer);
    }
}

BOOL ViewScriptingDialog::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (!fRet)
    {
        if (GetFocus() == static_cast<CWnd*>(&m_wndEditCode))
        {
            if (!fRet)
            {
                fRet = HandleEditBoxCommands(pMsg, m_wndEditCode);
            }
        }
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

void ViewScriptingDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Control(pDX, ID_SAVE, m_wndSave);
    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDC_EDIT_CODE, m_wndEditCode);

    if (!_initialized)
    {
        _initialized = true;
    }
}

BEGIN_MESSAGE_MAP(ViewScriptingDialog, CExtResizableDialog)
    ON_BN_CLICKED(ID_SAVE, &ViewScriptingDialog::OnBnClickedSave)
END_MESSAGE_MAP()

void ViewScriptingDialog::OnBnClickedSave()
{
    std::string filename = appState->GetResourceMap().Helper().GetViewScriptFilename(_number);

    CString code;
    m_wndEditCode.GetWindowTextA(code);
    MakeTextFile(code, filename);
}


void ViewScriptingDialog::OnOK()
{
    // Don't close the dialog.
    // Instead, call our callback here.
}