// NewGameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "NewGameDialog.h"
//#include <shfolder.h>
#include <shlobj.h>

// NewGameDialog dialog

NewGameDialog::NewGameDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(NewGameDialog::IDD, pParent)
{
}

NewGameDialog::~NewGameDialog()
{
}

void NewGameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDITPATH, m_wndPath);
    DDX_Control(pDX, IDC_EDITNAME, m_wndName);

    // Visuals
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
    DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_GROUP1, m_wndGroup1);
    DDX_Control(pDX, IDC_SYNTAX_SCI, m_wndSyntaxSCI);
    m_wndSyntaxSCI.SetCheck(true); // REVIEW: temporary until CPP is enabled
    DDX_Control(pDX, IDC_SYNTAX_CPP, m_wndSyntaxCPP);
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

    if (fContinue)
    {
        // 2) Copy the files over
        std::string templateFolder = appState->GetResourceMap().GetTemplateFolder();
        fContinue = !templateFolder.empty();
        if (fContinue)
        {
            fContinue = false;
            templateFolder += "\\*";
            char sz[MAX_PATH];
            StringCchCopy(sz, ARRAYSIZE(sz), templateFolder.c_str());
            size_t iDblNullTerm = strlen(sz) + 1;
            if (iDblNullTerm < ARRAYSIZE(sz))
            {
                sz[iDblNullTerm] = 0; // pFrom needs to have double null terminate...
                // 2) copy the files over.
                SHFILEOPSTRUCT fileOp = { 0 };
                fileOp.hwnd = GetSafeHwnd();
                fileOp.wFunc = FO_COPY;
                fileOp.pFrom = sz;
                fileOp.pTo = szPath;
                fileOp.fFlags = FOF_NOCONFIRMMKDIR | FOF_NOCOPYSECURITYATTRIBS;
                fContinue = (SHFileOperation(&fileOp) == 0);
            }
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
        appState->OpenScript("rm001");
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
