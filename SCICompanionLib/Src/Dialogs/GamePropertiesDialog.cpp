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
// GamePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "GamePropertiesDialog.h"
#include "ScriptConvert.h"
#include "CompileContext.h"
#include "ExtractAll.h"
#include "ResourceContainer.h"

// CGamePropertiesDialog dialog

CGamePropertiesDialog::CGamePropertiesDialog(RunLogic &runLogic, CWnd* pParent /*=NULL*/)
    : CExtNCW<CExtResizableDialog>(CGamePropertiesDialog::IDD, pParent), _runLogic(runLogic), _initialized(false), _wasAspectRatioChanged(false), _gameNeedsReload(false)
{
    _fAspectRatioStart = appState->GetResourceMap().Helper().GetUseSierraAspectRatio(!!appState->_fUseOriginalAspectRatioDefault);
    _fPatchFileStart = appState->GetResourceMap().Helper().GetResourceSaveLocation(ResourceSaveLocation::Default) == ResourceSaveLocation::Patch;
}

CGamePropertiesDialog::~CGamePropertiesDialog()
{
}

void CGamePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
    ShowSizeGrip(FALSE);
	CExtResizableDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITGAMENAME, _strGameName);
    DDX_Control(pDX, IDC_EDITEXECUTABLE, m_wndEditExe);
    DDX_Control(pDX, IDC_EDITEXECUTABLEPARAMETERS, m_wndEditParams);

    DDX_Control(pDX, IDC_BUTTONBROWSE, m_wndBrowse);
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATICGENERAL, m_wndGeneral);
    DDX_Control(pDX, IDC_STATICGAMENAME, m_wndGameName);
    DDX_Control(pDX, IDC_STATICEXE, m_wndExe);
    DDX_Control(pDX, IDC_STATICEXEPARAM, m_wndExeParam);

    DDX_Control(pDX, IDC_STATIC4, m_wndStatic4);
    DDX_Control(pDX, IDC_COMBOLANGUAGE, m_wndComboLanguage);
    LangSyntax lang = appState->GetResourceMap().Helper().GetDefaultGameLanguage();
    m_wndComboLanguage.SetCurSel((int)lang);

    DDX_Control(pDX, IDC_STATICPROFILE, m_wndStaticProfile);
    DDX_Control(pDX, IDC_COMBOPROFILE, m_wndComboProfile);

    DDX_Control(pDX, IDC_CHECKASPECTRATIO, m_wndCheckAspectRatio);
    m_wndCheckAspectRatio.SetCheck(_fAspectRatioStart ? BST_CHECKED : BST_UNCHECKED);

    DDX_Control(pDX, IDC_CHECKPATCHFILES, m_wndCheckPatchFiles);
    m_wndCheckPatchFiles.SetCheck(_fPatchFileStart ? BST_CHECKED : BST_UNCHECKED);

    if (!_initialized)
    {
        _initialized = true;

        m_wndComboProfile.SetRedraw(FALSE);
        m_wndComboProfile.ResetContent();
        for (auto &option : _runLogic.GetExecutableProfiles())
        {
            m_wndComboProfile.AddString(option.c_str());

            _optionToExe[option] = _runLogic.GetExe(option);
            _optionToParams[option] = _runLogic.GetParams(option);
        }
        int sel = m_wndComboProfile.FindString(-1, _runLogic.GetExecutableProfile().c_str());
        if (sel != CB_ERR)
        {
            m_wndComboProfile.SetCurSel(sel);
            _Update();
        }
        m_wndComboProfile.SetRedraw(TRUE);
    }
}

void CGamePropertiesDialog::_Update()
{
    int sel = m_wndComboProfile.GetCurSel();
    if (sel != CB_ERR)
    {
        std::string option = _runLogic.GetExecutableProfiles()[sel];

        m_wndEditExe.SetWindowText(_optionToExe[option].c_str());
        m_wndEditParams.SetWindowText(_optionToParams[option].c_str());
    }
}

void CGamePropertiesDialog::OnBrowse()
{
    CFileDialog fileDialog(TRUE,
                           NULL,
                           NULL,
                           OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
                           TEXT("Programs|*.exe;*.cmd;*.bat|All files|*.*||"));
    std::string gameFolder = appState->GetResourceMap().GetGameFolder();
    fileDialog.m_pOFN->lpstrInitialDir = gameFolder.c_str();

    if (IDOK == fileDialog.DoModal())
    {
        // Transfer the filename to the editbox.
		SetDlgItemText(IDC_EDITEXECUTABLE, fileDialog.GetPathName());
    }
}

BEGIN_MESSAGE_MAP(CGamePropertiesDialog, CExtNCW<CExtResizableDialog>)
    ON_COMMAND(IDC_BUTTONBROWSE, OnBrowse)
    ON_EN_KILLFOCUS(IDC_EDITEXECUTABLE, &CGamePropertiesDialog::OnEnKillfocusEditexecutable)
    ON_EN_KILLFOCUS(IDC_EDITEXECUTABLEPARAMETERS, &CGamePropertiesDialog::OnEnKillfocusEditexecutableparameters)
    ON_CBN_SELCHANGE(IDC_COMBOPROFILE, &CGamePropertiesDialog::OnCbnSelchangeComboprofile)
    ON_BN_CLICKED(IDC_CHECKASPECTRATIO, &CGamePropertiesDialog::OnAspectRatioClicked)
END_MESSAGE_MAP()

void CGamePropertiesDialog::OnAspectRatioClicked()
{
    _wasAspectRatioChanged = true;
}

void CGamePropertiesDialog::OnOK()
{
    for (auto &option : _dirtyOptionsExe)
    {
        _runLogic.SetExe(option, _optionToExe[option]);
    }
    for (auto &option : _dirtyOptionsParams)
    {
        _runLogic.SetParams(option, _optionToParams[option]);
    }

    // Always set the active choice
    int sel = m_wndComboProfile.GetCurSel();
    if (sel != CB_ERR)
    {
        std::string option = _runLogic.GetExecutableProfiles()[sel];
        _runLogic.SetExecutableProfile(option);
    }

    bool useSierraAspectRatio = m_wndCheckAspectRatio.GetCheck() == BST_CHECKED;
    if (_wasAspectRatioChanged)
    {
        // Set it
        appState->GetResourceMap().Helper().SetUseSierraAspectRatio(useSierraAspectRatio);
    }
    appState->_fUseOriginalAspectRatioCached = useSierraAspectRatio;
    if (_fAspectRatioStart != useSierraAspectRatio)
    {
        appState->NotifyChangeAspectRatio();
    }

    LangSyntax lang = appState->GetResourceMap().Helper().GetDefaultGameLanguage();
    int curSel = m_wndComboLanguage.GetCurSel();
    if ((curSel != CB_ERR) && (curSel != (int)lang))
    {
        appState->GetResourceMap().SetGameLanguage((LangSyntax)curSel);

        if (IDYES == AfxMessageBox("You've changed the default script language of the game. You can convert scripts one-by-one, or all at once right now. Do you want to try to convert all the scripts and headers now?", MB_YESNO | MB_ICONINFORMATION))
        {
            if (IDYES == AfxMessageBox("SCI Companion will convert all .sh and .sc files in the src directory, and all .shp and .shm files in the poly and msg directories. This may take several minutes.\nThe old code will be backed up to the convert-bak directory. Go ahead with this?", MB_YESNO | MB_ICONWARNING))
            {
                // Mainly intended to convert to Sierra syntax, so warn one more time if we're converting to Studio syntax.
                if ((lang != LangSyntaxSCI) || (IDYES == AfxMessageBox("Converting back to SCI Studio syntax is not a well-tested scenario. Continue anyway?", MB_YESNO | MB_ICONWARNING)))
                {
                    CompileLog log;
                    ConvertGame(appState->GetResourceMap(), (LangSyntax)curSel, log);
                    appState->OutputResults(OutputPaneType::Compile, log.Results());
                }
            }
        }
    }

    bool usePatchFiles = m_wndCheckPatchFiles.GetCheck() == BST_CHECKED;
    if (usePatchFiles != _fPatchFileStart)
    {
        std::string message = usePatchFiles ?
            "Switching to patch files for resource management. All resources in the resource package will be extracted to patch files in the game directory.\nIt is recommended that you backup your game.\nContinue?" :
            "Switching to resource packages for resource management. All patch files will be built into the resource package and then deleted.\nIt is recommended that you backup your game.\nContinue?";

        if (IDYES == AfxMessageBox(message.c_str(), MB_ICONWARNING | MB_YESNO))
        {
            if (usePatchFiles)
            {
                // Extract all resources
                ExtractAllResources(appState->GetVersion(), appState->GetResourceMap().GetGameFolder(), true, false, false, false, false, false, nullptr);
            }
            else
            {
                // Put everything into resource.map, resource.001, etc...
                appState->GetResourceMap().PurgeUnnecessaryResources();
                // Now delete...
                auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::All, ResourceEnumFlags::ExcludePackagedFiles);
                std::vector<std::unique_ptr<ResourceBlob>> itemsToDelete;
                for (auto &blob : *resourceContainer)
                {
                    itemsToDelete.push_back(move(blob));
                }
                for (auto &blob : itemsToDelete)
                {
                    appState->GetResourceMap().DeleteResource(blob.get());
                }
            }

            // Set it
            appState->GetResourceMap().Helper().SetResourceSaveLocation(usePatchFiles ? ResourceSaveLocation::Patch : ResourceSaveLocation::Package);

            _gameNeedsReload = true;
        }
        else
        {
            AfxMessageBox("No changed made.", MB_OK);
        }
    }

    __super::OnOK();
}

void CGamePropertiesDialog::OnEnKillfocusEditexecutable()
{
    int sel = m_wndComboProfile.GetCurSel();
    if (sel != CB_ERR)
    {
        std::string option = _runLogic.GetExecutableProfiles()[sel];
        CString strExe;
        m_wndEditExe.GetWindowText(strExe);
        std::string exe = (PCSTR)strExe;

        if (exe != _optionToExe[option])
        {
            _optionToExe[option] = exe;
            _dirtyOptionsExe.insert(option);
        }
    }
}

void CGamePropertiesDialog::OnEnKillfocusEditexecutableparameters()
{
    int sel = m_wndComboProfile.GetCurSel();
    if (sel != CB_ERR)
    {
        std::string option = _runLogic.GetExecutableProfiles()[sel];
        CString strParams;
        m_wndEditParams.GetWindowText(strParams);
        std::string params = (PCSTR)strParams;

        if (params != _optionToExe[option])
        {
            _optionToParams[option] = params;
            _dirtyOptionsParams.insert(option);
        }
    }
}


void CGamePropertiesDialog::OnCbnSelchangeComboprofile()
{
    _Update();
}
