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
// NewGameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "NewGameDialog.h"
//#include <shfolder.h>
#include <shlobj.h>
#include "atlimage.h"
#include "GameFolderHelper.h"

// NewGameDialog dialog

NewGameDialog::NewGameDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(NewGameDialog::IDD, pParent)
{
}

NewGameDialog::~NewGameDialog()
{
}

PCSTR ConfigSectionName = "Config";

void NewGameDialog::_PopulateTemplates()
{
    std::string templateFolder = appState->GetResourceMap().GetTemplateFolder() + "\\";
    std::string findFirstString = templateFolder + "SCI*";
    // Find top-level folders that start with SCI
    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFolder = FindFirstFile(findFirstString.c_str(), &findData);
    if (hFolder != INVALID_HANDLE_VALUE)
    {
        BOOL ok = TRUE;
        while (ok)
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                (0 == strncmp(findData.cFileName, "SCI", 3)))
            {
                // Include this
                m_wndComboTemplate.AddString(findData.cFileName);
            }
            else
            {
                PCSTR pszExt = PathFindExtension(findData.cFileName);
                std::string key(findData.cFileName, pszExt);
                if (0 == strncmp(pszExt, ".txt", 4))
                {
                    std::ifstream descriptionFile(templateFolder + findData.cFileName);
                    std::string description((std::istreambuf_iterator<char>(descriptionFile)),
                        std::istreambuf_iterator<char>());
                    _descriptions[key] = description;
                }
                else if (0 == strncmp(pszExt, ".bmp", 4))
                {
                    CImage image;
                    if (SUCCEEDED(image.Load((templateFolder + findData.cFileName).c_str())))
                    {
                        std::unique_ptr<CBitmap> bitmap = std::make_unique<CBitmap>();
                        bitmap->Attach(image.Detach());
                        _bitmaps[key] = std::move(bitmap);
                    }
                }
                else if (0 == strncmp(pszExt, ".ini", 4))
                {
                    if (GetPrivateProfileInt(ConfigSectionName, "IsDefault", 0, (templateFolder + findData.cFileName).c_str()))
                    {
                        _defaultTemplate = key;
                    }
                }
            }

            ok = FindNextFile(hFolder, &findData);
        }
        FindClose(hFolder);
    }

    if (m_wndComboTemplate.GetCount() > 0)
    {
        int defaultIndex = m_wndComboTemplate.FindString(-1, _defaultTemplate.c_str());
        if (defaultIndex != -1)
        {
            m_wndComboTemplate.SetCurSel(defaultIndex);
        }
        else
        {
            m_wndComboTemplate.SetCurSel(0);
        }
        OnCbnSelchangeCombotemplate();
    }
}

void NewGameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDITPATH, m_wndPath);
    DDX_Control(pDX, IDC_EDITNAME, m_wndName);
    DDX_Control(pDX, IDC_BUTTONBROWSE, m_wndBrowse);
    
    // Visuals
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
    DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
    DDX_Control(pDX, IDC_STATIC3, m_wndStatic3);
    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_wndDescription);
    DDX_Control(pDX, IDC_STATIC_IMAGE, m_wndImage);

    DDX_Control(pDX, IDC_STATIC4, m_wndStatic4);
    DDX_Control(pDX, IDC_COMBOLANGUAGE, m_wndComboLanguage);
    m_wndComboLanguage.SetCurSel(0);

    DDX_Control(pDX, IDC_COMBOTEMPLATE, m_wndComboTemplate);
    _PopulateTemplates();
}

BOOL NewGameDialog::OnInitDialog()
{
    __super::OnInitDialog();

    char szPath[MAX_PATH];
    if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, FALSE))
    {
        m_wndPath.SetWindowText(szPath);
    }
    m_wndName.SetWindowText("New Game");
    m_wndName.SetSel(0, -1);
    m_wndName.SetFocus();

    ShowSizeGrip(FALSE);

    return FALSE; // Since we set focus ourselves.
}


BEGIN_MESSAGE_MAP(NewGameDialog, CExtResizableDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTONBROWSE, OnBnClickedButtonbrowse)
    ON_CBN_SELCHANGE(IDC_COMBOTEMPLATE, &NewGameDialog::OnCbnSelchangeCombotemplate)
END_MESSAGE_MAP()

// NewGameDialog message handlers

void NewGameDialog::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    // Steps:
    // 1) Warn if the directory already exists.
    // 2) Copy the files over
    // 3) Set the name in the ini file

    bool fContinue = true;
    // 1) Warning if the directory already exists.
    char szPath[MAX_PATH];
    m_wndPath.GetWindowText(szPath, ARRAYSIZE(szPath));
    if (!CreateDirectory(szPath, NULL) && (GetLastError() == ERROR_ALREADY_EXISTS))
    {
        fContinue = false;
        char szMessage[MAX_PATH * 2];
        StringCchPrintf(szMessage, ARRAYSIZE(szMessage), "%s\nThis folder already exists.  Are you sure you want to continue?", szPath);
        fContinue = (IDYES == AfxMessageBox(szMessage, MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL));
    }

    uint16_t openToRoom = 0;
    if (fContinue)
    {
        // 2) Copy the files over
        // TODO: Need to deal with language.
        std::string templateCoreFolder = appState->GetResourceMap().GetTemplateFolder();
        CString strText;
        m_wndComboTemplate.GetWindowTextA(strText);
        std::string templateSubFolder = (PCSTR)strText;
        std::string templateFolder = templateCoreFolder + "\\" + templateSubFolder;

        std::string iniConfigName = templateFolder + ".ini";
        openToRoom = (uint16_t)GetPrivateProfileInt(ConfigSectionName, "OpenTo", 0, iniConfigName.c_str());

        fContinue = !templateCoreFolder.empty() && !templateSubFolder.empty();
        if (fContinue)
        {
            fContinue = false;
            templateFolder += "\\*";
            fContinue = CopyFilesOver(GetSafeHwnd(), templateFolder, szPath);
        }
    }

    if (fContinue)
    {
        // 3) Set the name in the ini file
        char szName[MAX_PATH];
        m_wndName.GetWindowText(szName, ARRAYSIZE(szName));

        char szGameIni[MAX_PATH];
        PathCombine(szGameIni, szPath, "game.ini");
        fContinue = (0 != WritePrivateProfileString("Game", "Name", szName, szGameIni));
        if (fContinue)
        {
            // Set the game language.
            int curSel = m_wndComboLanguage.GetCurSel();
            if (curSel != CB_ERR)
            {
                LangSyntax lang = (LangSyntax)curSel;
                GameFolderHelper helper;
                helper.GameFolder = szPath;
                helper.SetIniString(GameSection, LanguageKey, (lang == LangSyntaxSCI) ? LanguageValueSCI : LanguageValueCpp);
            }
        }
        if (!fContinue)
        {
            char szMessage[MAX_PATH * 2];
            char szReason[200];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, (DWORD)GetLastError(), 0,  szReason, ARRAYSIZE(szReason), NULL);
            StringCchPrintf(szMessage, ARRAYSIZE(szMessage), "Couldn't write the name of the game to game.ini\n%s", szReason);
            AfxMessageBox(szMessage, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
        }
    }

    if (fContinue)
    {
        // Open the new game, and then open the script editor to rm001 of the template game
        appState->OpenDocumentFile(szPath);
        appState->OpenScript(openToRoom);
        OnOK();
    }
}

void NewGameDialog::OnBnClickedButtonbrowse()
{
    // TODO: Add your control notification handler code here
    BROWSEINFO info = { 0 };
    info.hwndOwner = GetSafeHwnd();
    info.lpszTitle = "Choose a folder";
    info.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_NEWDIALOGSTYLE;
    info.pszDisplayName = NULL;
    HRESULT hr = OleInitialize(NULL);
    LPITEMIDLIST pidl = SHBrowseForFolder(&info);
    if (pidl)
    {
        char szPath[MAX_PATH];
        if (SHGetPathFromIDList(pidl, szPath))
        {
            m_wndPath.SetWindowText(szPath);
        }
        IMalloc *pMalloc;
        if (SUCCEEDED(SHGetMalloc(&pMalloc)))
        {
            pMalloc->Free(pidl);
        }
    }
    if (SUCCEEDED(hr))
    {
        OleUninitialize();
    }
}


void NewGameDialog::OnCbnSelchangeCombotemplate()
{
    int curSel = m_wndComboTemplate.GetCurSel();
    if (curSel != CB_ERR)
    {
        CString str;
        m_wndComboTemplate.GetLBText(curSel, str);
        m_wndDescription.SetWindowText(_descriptions[(PCSTR)str].c_str());
        m_wndImage.SetBitmap(*_bitmaps[(PCSTR)str]);
    }
}
