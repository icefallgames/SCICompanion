#include "stdafx.h"
#include "GenerateDocsDialog.h"

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
// ClassBrowserDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "format.h"
#include "GameFolderHelper.h"
#include "ResourceContainer.h"
#include "CrystalScriptStream.h"
#include "CompileContext.h"
#include "ScriptOM.h"
#include "DocScript.h"
#include "OutputRST.h"

// TODO: customize
std::string docSourceFolder = "source";
std::string docBuildFolder = "build/html";

using namespace sci;

IMPLEMENT_DYNAMIC(GenerateDocsDialog, CDialog)
GenerateDocsDialog::GenerateDocsDialog(GameFolderHelper helper, CWnd* pParent /*=NULL*/)
: CDialog(GenerateDocsDialog::IDD, pParent), _fInitialized(false), _helper(helper)
{
}

GenerateDocsDialog::~GenerateDocsDialog()
{
}

void GenerateDocsDialog::_PopulateScripts()
{
    m_wndScripts.SetRedraw(FALSE);
    auto scriptResources = _helper.Resources(ResourceTypeFlags::Script, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::NameLookups);
    int itemNumber = 0;
    for (auto &blob : *scriptResources)
    {
        std::string name = blob->GetName();
        m_wndScripts.AddString(name.c_str());
    }
    m_wndScripts.SetRedraw(TRUE);
}

void GenerateDocsDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    if (!_fInitialized)
    {
        _fInitialized = true;
        DDX_Control(pDX, IDC_LISTSCRIPTS, m_wndScripts);
        DDX_Control(pDX, IDC_LISTGENERATED, m_wndGeneratedFiles);
        DDX_Control(pDX, IDC_EDITFOLDER, m_wndFolder);
        DDX_Control(pDX, IDC_EDITCOMMAND, m_wndCommand);

        m_wndFolder.SetWindowText(appState->_docGenFolder.c_str());
        m_wndCommand.SetWindowText(appState->_docGenCommand.c_str());

        CRect rect;
        GetDlgItem(IDC_BROWSER)->GetWindowRect(&rect);
        ScreenToClient(&rect);
        try
        {
            m_wndBrowser.CreateControl(CLSID_WebBrowser, "Documentation", WS_VISIBLE | WS_CHILD, rect, this, AFX_IDW_PANE_FIRST);
        }
        catch (COleException* pException)
        {
            char szError[1024];
            szError[0] = 0;
            pException->GetErrorMessage(szError, ARRAYSIZE(szError));
            appState->LogInfo(szError);
        }

        _PopulateScripts();
    }
}

//
// Takes a stream of html and shows it in the browser control.
//
void GenerateDocsDialog::_ShowHTML(const std::string &filename)
{
    IUnknown *pUnk = m_wndBrowser.GetControlUnknown();
    IWebBrowser *pBrowser;
    HRESULT hr = pUnk->QueryInterface(IID_PPV_ARGS(&pBrowser));
    if (SUCCEEDED(hr))
    {
        CString url(filename.c_str());
        BSTR bUrl = url.AllocSysString();
        COleVariant vNull((LPCTSTR)NULL, VT_BSTR);
        hr = pBrowser->Navigate(bUrl, &vNull, &vNull, &vNull, &vNull);
        pBrowser->Release();
    }
}


//
// Assembles all the scripts in the src directory, and provides them to the class browser.
//


BEGIN_MESSAGE_MAP(GenerateDocsDialog, CDialog)
    ON_BN_CLICKED(IDC_GENERATEDOC, &GenerateDocsDialog::OnBnClickedGeneratedoc)
    ON_LBN_SELCHANGE(IDC_LISTGENERATED, &GenerateDocsDialog::OnLbnSelchangeListgenerated)
    ON_EN_CHANGE(IDC_EDITFOLDER, &GenerateDocsDialog::OnEnChangeEditfolder)
    ON_EN_CHANGE(IDC_EDITCOMMAND, &GenerateDocsDialog::OnEnChangeEditcommand)
END_MESSAGE_MAP()

void GenerateDocsDialog::OnBnClickedGeneratedoc()
{
    m_wndGeneratedFiles.ResetContent();

    CString strText;
    m_wndFolder.GetWindowText(strText);
    std::string docGenFolder = (PCSTR)strText;
    m_wndCommand.GetWindowText(strText);
    std::string docCommand = (PCSTR)strText;

    std::vector<std::string> generatedFiles;

    std::string buildFolder = docGenFolder + "\\" + docSourceFolder;

    int cItems = m_wndScripts.GetCount();
    std::unique_ptr<int[]> selectedItems = std::make_unique<int[]>(cItems);
    int cSelItems = m_wndScripts.GetSelItems(cItems, selectedItems.get());
    for (int i = 0; i < cSelItems; i++)
    {
        CString strText;
        m_wndScripts.GetText(selectedItems.get()[i], strText);
        std::string scriptName = (PCSTR)strText;
        std::string fullPath = _helper.GetScriptFileName(scriptName);

        CompileLog log;
        std::unique_ptr<sci::Script> script = SimpleCompile(log, ScriptId(fullPath), true);
        if (script)
        {
            DocScript docScript(*script);
            OutputScriptRST(docScript, buildFolder, generatedFiles);
            OutputClassRST(docScript, buildFolder, generatedFiles);
        }
    }

    // Display generated files
    m_wndGeneratedFiles.SetRedraw(FALSE);
    for (auto &file : generatedFiles)
    {
        m_wndGeneratedFiles.AddString((file + ".rst").c_str());
        m_wndGeneratedFiles.AddString((file + ".html").c_str());
    }
    m_wndGeneratedFiles.SetRedraw(TRUE);

    if (cSelItems)
    {
        // Build
        // Need to ShellExecute, then wait for process to come back. see debugger
        // Then can open std out for errors.
        _RunBuild(docGenFolder, docCommand);
    }
}

void GenerateDocsDialog::_RunBuild(const std::string &docGenFolder, const std::string &docGenCommand)
{
    /*
    HANDLE hOut = INVALID_HANDLE_VALUE;
    HANDLE hPipeRead, hPipeWrite;
    // Set the bInheritHandle flag so pipe handles are inherited. 
    SECURITY_ATTRIBUTES securityAttributes = {};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;
    if (CreatePipe(&hPipeRead, &hPipeWrite, &securityAttributes, 1024))
    {
        // Ensure the read handle to the pipe for STDOUT is not inherited.
        if (SetHandleInformation(hPipeRead, HANDLE_FLAG_INHERIT, 0))
        {
            STARTUPINFO startupInfo = {};
            startupInfo.cb = sizeof(STARTUPINFO);
            startupInfo.dwFlags |= STARTF_USESTDHANDLES;
            startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            startupInfo.hStdOutput = hPipeWrite;
            startupInfo.hStdError = hPipeWrite;

            std::string commandLine = fmt::format("cmd.exe /C {0}", docGenCommand);
            std::vector<char> szCommandLine(commandLine.begin(), commandLine.end());
            szCommandLine.push_back('\0');

            PROCESS_INFORMATION processInfo = {};
            BOOL create = CreateProcess(nullptr, &szCommandLine[0], nullptr, nullptr, TRUE, 0, nullptr, docGenFolder.c_str(), &startupInfo, &processInfo);
            if (create)
            {
                std::string output;
                char szOut[1024];
                DWORD cbRead = 0;
                BOOL readStuff = FALSE;
                do
                {
                    if (!PeekNamedPipe(hPipeRead, szOut, ARRAYSIZE(szOut), &cbRead, nullptr, nullptr) || (cbRead == 0))
                    {
                        break;
                    }
                    readStuff = ReadFile(hPipeRead, szOut, ARRAYSIZE(szOut), &cbRead, nullptr);
                    std::copy(szOut, szOut + cbRead, std::back_inserter(output));
                } while (readStuff || (cbRead == 0));

                if (processInfo.hProcess)
                {
                    WaitForSingleObject(processInfo.hProcess, 5000);
                }

                CloseHandle(processInfo.hThread);
                CloseHandle(processInfo.hProcess);
            }
        }
        CloseHandle(hPipeWrite); // Close our end of the write pipe
        CloseHandle(hPipeRead);
    }*/

    std::string params = fmt::format("/C {0}", docGenCommand);
    SHELLEXECUTEINFO ei = {};
    ei.fMask = SEE_MASK_DEFAULT | SEE_MASK_NOCLOSEPROCESS;
    ei.cbSize = sizeof(ei);
    ei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
    ei.lpFile = "cmd.exe";
    ei.lpVerb = nullptr;
    ei.lpParameters = params.c_str();
    ei.lpDirectory = docGenFolder.c_str();
    ei.nShow = SW_SHOWNORMAL;

    // ShellExecuteEx throws an RPC exception, but I guess that's ok.
    if (!ShellExecuteEx(&ei))
    {
        INT_PTR iResult = GetLastError();

        TCHAR szReason[MAX_PATH];
        szReason[0] = 0;
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, (DWORD)iResult, 0, szReason, ARRAYSIZE(szReason), NULL);

        TCHAR szError[MAX_PATH];
        StringCchPrintf(szError, ARRAYSIZE(szError), TEXT("Failed to start %s: %s"), ei.lpParameters, szReason);
    }
    else
    {
        if (ei.hProcess)
        {
            WaitForSingleObject(ei.hProcess, 5000);
        }
    }
}

void GenerateDocsDialog::OnLbnSelchangeListgenerated()
{
    CString strText;
    m_wndFolder.GetWindowText(strText);
    std::string docGenFolder = (PCSTR)strText;

    int sel = m_wndGeneratedFiles.GetCurSel();
    if (sel != LB_ERR)
    {
        CString strText;
        m_wndGeneratedFiles.GetText(sel, strText);
        if (-1 != strText.Find(".html"))
        {
            _ShowHTML(docGenFolder + "\\" + docBuildFolder + "\\" + (PCSTR)strText);
        }
        else
        {
            _ShowHTML(docGenFolder + "\\" + docSourceFolder + "\\" + (PCSTR)strText);
        }
    }
}

void GenerateDocsDialog::OnEnChangeEditfolder()
{
    CString strText;
    m_wndFolder.GetWindowText(strText);
    appState->_docGenFolder = (PCSTR)strText;
}


void GenerateDocsDialog::OnEnChangeEditcommand()
{
    CString strText;
    m_wndCommand.GetWindowText(strText);
    appState->_docGenCommand = (PCSTR)strText;
}
