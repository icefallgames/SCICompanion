#include "stdafx.h"
#include "AppState.h"
#include "ExtractAllDialog.h"
#include "format.h"

using namespace std;
using namespace fmt;

#define UWM_UPDATESTATUS (WM_APP + 1)
#define CHEKCDONE_TIMER 3456

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ExtractAllDialog dialog

ExtractAllDialog::ExtractAllDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(ExtractAllDialog::IDD, pParent)
{
    _version = appState->GetVersion();
    _fResult = false;
    _fAbort = false;
    _fExtracting = false;
}

ExtractAllDialog::~ExtractAllDialog()
{
}

bool ExtractAllDialog::SetProgress(const std::string &info, int amountDone, int totalAmount)
{
    std::string *ptrToString = new std::string(info);
    PostMessage(UWM_UPDATESTATUS, MAKELONG(amountDone, totalAmount), reinterpret_cast<LPARAM>(ptrToString));
    return !_fAbort;
}

void ExtractAllDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
    DDX_Control(pDX, IDC_EDIT, m_wndDisplay);
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic);
    DDX_Control(pDX, IDC_EDIT1, m_wndFolderLocation);

    // Visuals
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_BUTTONBROWSE, m_wndBrowse);
    DDX_Control(pDX, IDC_EXTRACT, m_wndExtract);
    DDX_Control(pDX, IDC_CHECK1, m_wndExtractResources);
    m_wndExtractResources.SetCheck(BST_CHECKED);
    DDX_Control(pDX, IDC_CHECK2, m_wndExtractPicImage);
    m_wndExtractPicImage.SetCheck(BST_CHECKED);
    DDX_Control(pDX, IDC_CHECK3, m_wndExtractViewImage);
    m_wndExtractViewImage.SetCheck(BST_CHECKED);
    DDX_Control(pDX, IDC_CHECK4, m_wndDisassembleScripts);
    m_wndDisassembleScripts.SetCheck(BST_CHECKED);
    DDX_Control(pDX, IDC_CHECK5, m_wndExportMessages);
    m_wndExportMessages.SetCheck(BST_CHECKED);
}

BOOL ExtractAllDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    ShowSizeGrip(FALSE);
    SetTimer(CHEKCDONE_TIMER, 100, nullptr);
    return fRet;
}

void ExtractAllDialog::OnCancel()
{
    if (_fExtracting)
    {
        // We're still doing stuff.  Signal ourself to close.
        _fAbort = true;
    }
    else
    {
        __super::OnCancel();
    }
}

BEGIN_MESSAGE_MAP(ExtractAllDialog, CExtResizableDialog)
    ON_MESSAGE(UWM_UPDATESTATUS, UpdateStatus)
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTONBROWSE, &ExtractAllDialog::OnBnClickedButtonbrowse)
    ON_BN_CLICKED(IDC_EXTRACT, &ExtractAllDialog::OnBnClickedExtract)
END_MESSAGE_MAP()

LRESULT ExtractAllDialog::UpdateStatus(WPARAM wParam, LPARAM lParam)
{
    int totalAmount = HIWORD(wParam);
    int count = LOWORD(wParam);
    std::string *stringPtr = reinterpret_cast<std::string*>(lParam);

    m_wndProgress.SetRange(0, totalAmount);
    m_wndProgress.SetPos(count);
    m_wndDisplay.SetWindowTextA(stringPtr->c_str());
    delete stringPtr;

    return 0;
}

// ExtractAllDialog message handlers

void ExtractAllDialog::OnBnClickedButtonbrowse()
{
    HRESULT hr = OleInitialize(nullptr);
    BROWSEINFO   bi = {};
    char szDisplayName[MAX_PATH];
    szDisplayName[0] = 0;
    bi.hwndOwner = this->GetSafeHwnd();
    bi.pidlRoot = nullptr;
    bi.pszDisplayName = szDisplayName;
    bi.lpszTitle = _T("Please select a destination folder for the extracted files:");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lParam = 0;
    bi.iImage = 0;
    LPITEMIDLIST   pidl = SHBrowseForFolder(&bi);
    TCHAR   szPathName[MAX_PATH];
    if (nullptr != pidl)
    {
        if (SHGetPathFromIDList(pidl, szPathName))
        {
            m_wndFolderLocation.SetWindowTextA(szPathName);
        }
    }
    if (SUCCEEDED(hr))
    {
        OleUninitialize();
    }
}

// TODO: SEnsible defaults, save in static.. dunno if they need to be saved

bool DirectoryExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

const char testFileName[] = "thisisatest.txt";

void ExtractAllDialog::OnBnClickedExtract()
{
    CString strLocation;
    m_wndFolderLocation.GetWindowTextA(strLocation);
    bool good = true;

    if (strLocation.IsEmpty())
    {
        good = false;
        AfxMessageBox("Please specify a folder to extract to.", MB_ICONWARNING | MB_OK);
    }

    if (good)
    {
        if (0 == strLocation.CompareNoCase(appState->GetResourceMap().GetGameFolder().c_str()))
        {
            good = (IDYES == AfxMessageBox("The extract location is the game folder.\nAre you sure you want to continue?", MB_ICONWARNING | MB_YESNO));
        }
    }

    if (good)
    {
        good = EnsureFolderExists((PCSTR)strLocation, false);
        if (!good)
        {
            string details = format("Creating \"{}\"", (PCSTR)strLocation);
            AfxMessageBox(GetMessageFromLastError(details).c_str(), MB_OK | MB_ICONERROR);
        }
    }

    if (good)
    {
        char szTestFile[MAX_PATH];
        PathCombine(szTestFile, strLocation, testFileName);
        {
            ScopedHandle holder;
            holder.hFile = CreateFile(szTestFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            good = holder.hFile != INVALID_HANDLE_VALUE;
            if (!good)
            {
                string details = format("Writing to \"{}\"", (PCSTR)strLocation);
                AfxMessageBox(GetMessageFromLastError(details).c_str(), MB_OK | MB_ICONERROR);
            }
        }
        DeleteFile(szTestFile);
    }

    if (good)
    {
        // We can finally do this
        _fExtracting = true;
        m_wndExtract.EnableWindow(FALSE);

        _location = strLocation;
        _extractPicImages = m_wndExtractPicImage.GetCheck() != 0;
        _extractViewImages = m_wndExtractViewImage.GetCheck() != 0;
        _disassembleScripts = m_wndDisassembleScripts.GetCheck() != 0;
        _exportMessages = m_wndExportMessages.GetCheck() != 0;

        try
        {
            _future = make_unique<future<void>>(async(launch::async, s_ThreadWorker, this));
        }
        catch (std::system_error)
        {
            _fExtracting = false;
        }
    }
}

void ExtractAllDialog::s_ThreadWorker(ExtractAllDialog *pThis)
{
    ExtractAllResources(pThis->_version, (PCSTR)pThis->_location, pThis->_extractPicImages, pThis->_extractViewImages, pThis->_disassembleScripts, pThis->_exportMessages, pThis);
}

void ExtractAllDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == CHEKCDONE_TIMER)
    {
        if (_future && (future_status::ready == _future->wait_for(std::chrono::seconds(0))))
        {
            _fExtracting = false;
            _future = nullptr;
            m_wndDisplay.SetWindowTextA(_fAbort ? "Aborted" : "Done!");
            _fAbort = false;
            m_wndExtract.EnableWindow(TRUE);
            m_wndProgress.SetPos(0);
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}