// GamePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "GamePropertiesDialog.h"


// CGamePropertiesDialog dialog

CGamePropertiesDialog::CGamePropertiesDialog(RunLogic &runLogic, CWnd* pParent /*=NULL*/)
    : CExtNCW<CExtResizableDialog>(CGamePropertiesDialog::IDD, pParent), _runLogic(runLogic), _initialized(false)
{
    
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

    DDX_Control(pDX, IDC_STATICPROFILE, m_wndStaticProfile);
    DDX_Control(pDX, IDC_COMBOPROFILE, m_wndComboProfile);

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
END_MESSAGE_MAP()

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
