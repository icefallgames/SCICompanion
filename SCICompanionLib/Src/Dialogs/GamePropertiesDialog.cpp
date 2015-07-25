// GamePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "GamePropertiesDialog.h"


// CGamePropertiesDialog dialog

CGamePropertiesDialog::CGamePropertiesDialog(CWnd* pParent /*=NULL*/)
	: CExtNCW<CExtResizableDialog>(CGamePropertiesDialog::IDD, pParent)
{
}

CGamePropertiesDialog::~CGamePropertiesDialog()
{
}

void CGamePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
    ShowSizeGrip(FALSE);
	CExtResizableDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_RADIOCPP, _fCPPSyntax);
    DDX_Check(pDX, IDC_RADIOSCISTUDIO, _fSCIStudioSyntax);
    DDX_Text(pDX, IDC_EDITGAMENAME, _strGameName);
    DDX_Text(pDX, IDC_EDITEXECUTABLE, _strGameExecutable);
    DDX_Text(pDX, IDC_EDITEXECUTABLEPARAMETERS, _strGameExecutableParameters);

    DDX_Control(pDX, IDC_BUTTONBROWSE, m_wndBrowse);
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_RADIOCPP, m_wndRadioCpp);
    DDX_Control(pDX, IDC_RADIOSCISTUDIO, m_wndRadioSCIStudio);
    DDX_Control(pDX, IDC_STATICSCRIPTLANG, m_wndLanguage);
    DDX_Control(pDX, IDC_STATICGENERAL, m_wndGeneral);
    DDX_Control(pDX, IDC_STATICGAMENAME, m_wndGameName);
    DDX_Control(pDX, IDC_STATICEXE, m_wndExe);
    DDX_Control(pDX, IDC_STATICEXEPARAM, m_wndExeParam);
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
END_MESSAGE_MAP()


// CGamePropertiesDialog message handlers
