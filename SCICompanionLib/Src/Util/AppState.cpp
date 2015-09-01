#include "stdafx.h"
#include "AppState.h"

#include "MainFrm.h"
#include "PicChildFrame.h"

#include "PicDoc.h"
#include "PicView.h"

#include "VocabDoc.h"
#include "VocabChildFrame.h"
#include "VocabView.h"

#include "ResourceListDoc.h"
#include "GameExplorerFrame.h"
#include "GameExplorerView.h"

#include "ScriptFrame.h"
#include "ScriptDocument.h"
#include "ScriptView.h"

#include "ViewChildFrame.h"
#include "RasterView.h"

#include "TextChildFrame.h"
#include "MessageChildFrame.h"
#include "TextDoc.h"
#include "TextView.h"

#include "SoundChildFrame.h"
#include "SoundDoc.h"
#include "SoundView.h"

#include "CursorChildFrame.h"

#include "FontChildFrame.h"

#include "RoomExplorerFrame.h"
#include "RoomExplorerDoc.h"
#include "RoomExplorerView.h"

#include "ResourceEntity.h"

#include "GamePropertiesDialog.h"

#include "ColoredToolTip.h"

#include "CrystalScriptStream.h"
#include "CodeAutoComplete.h"

#include "View.h"
#include "NewRasterResourceDocument.h"

#include "crc.h"

static const char c_szExecutableString[] = "Executable";
static const char c_szExeParametersString[] = "ExeCmdLineParameters";
static const char c_szDefaultExe[] = "sciv.exe";

// The one and only
extern AppState *appState;

CMultiDocTemplateWithNonViews::CMultiDocTemplateWithNonViews(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass) : CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{

}

void CMultiDocTemplateWithNonViews::InitialUpdateFrame(CFrameWnd *pFrame, CDocument *pDoc, BOOL bMakeVisible)
{
    CMDITabChildWnd *pPicFrame = (CMDITabChildWnd*)pFrame;
    if (pPicFrame)
    {
        pPicFrame->HookUpNonViews(pDoc);
    }

    __super::InitialUpdateFrame(pFrame, pDoc, bMakeVisible);
}

AppState::AppState(CWinApp *pApp)
{
    _pApp = pApp;

    // Place all significant initialization in InitInstance
    _pPicTemplate = NULL;
    _fGridLines = FALSE;
    _fScaleTracingImages = TRUE;
    _fDontShowTraceScaleWarning = FALSE;
    _fUseAutoSuggest = FALSE;
    _fAllowBraceSyntax = FALSE;
    _fAutoLoadGame = TRUE;
    _fDupeNewCels = TRUE;
    _fUseBoxEgo = FALSE;
    _fShowPolyDotted = FALSE;
    _fBrowseInfo = FALSE;
    _fScriptNav = TRUE;
    _fCodeCompletion = TRUE;
    _fHoverTips = TRUE;
    _fPlayCompileErrorSound = TRUE;
    _fUseOriginalAspectRatio = false;

    _pVocabTemplate = NULL;
    _pPicTemplate = NULL;

    _cxFakeEgo = 30;
    _cyFakeEgo = 48;
    _ptFakeEgo = CPoint(160, 120);
    _fObserveControlLines = false;
    _fObservePolygons = false;
    _fDontCheckPic = FALSE;
    _pidlFolder = NULL;
    _fNoGdiPlus = FALSE;

    _pResourceDoc = NULL;
    _shownType = ResourceType::View;

    _pACThread = nullptr;
#ifdef SCI_AUTOCOMPLETE
    _pACThread = new AutoCompleteThread();
#endif

    crcInit();

    // Prepare g_egaColorsExtended
    for (int i = 0; i < 256; i += 16)
    {
        CopyMemory(g_egaColorsExtended + i, g_egaColors, sizeof(g_egaColors));
    }
    // Fake EGA palette for when it's needed.
    memcpy(g_egaDummyPalette.Colors, g_egaColors, sizeof(g_egaColors));

    // Prepare g_vgaPaletteMapping
    for (int i = 0; i < 256; i++)
    {
        g_vgaPaletteMapping[i] = (uint8_t)i;
    }

    CelDataClipboardFormat = RegisterClipboardFormat("SCICompanionVGACelData");
}

int AppState::AspectRatioY(int value) const
{
    if (_fUseOriginalAspectRatio)
    {
        // Multiply by 1.2 to achieve original Sierra aspect ratio
        return value * 12 / 10;
    }
    else
    {
        return value;
    }
}

int AppState::InverseAspectRatioY(int value) const
{
    if (_fUseOriginalAspectRatio)
    {
        // Multiply by 1.2 to achieve original Sierra aspect ratio
        return value * 10 / 12;
    }
    else
    {
        return value;
    }
}

void AppState::GiveMeAutoComplete(CScriptView *pSV)
{
    pSV->SetAutoComplete(&m_wndIntel, &m_wndToolTip, &m_wndToolTip, _pACThread);
}

AppState::~AppState()
{
    delete _pACThread;
    CoTaskMemFree(_pidlFolder);
}

// Takes ownership of pidl
void AppState::SetExportFolder(LPITEMIDLIST pidl)
{
    CoTaskMemFree(_pidlFolder);
    _pidlFolder = pidl;
}

// The one and only AppState object
AppState *appState;

// AppState initialization
BOOL AppState::InitInstance()
{
    _pszCommandProfile = "SCIComp2";

    // InitCommonControls() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    InitCommonControls();

    InitDitherCritSec();

    HMODULE hinstGdiPlus = LoadLibrary("gdiplus.dll");
    if (hinstGdiPlus)
    {
        FreeLibrary(hinstGdiPlus);
    }
    else
    {
        _fNoGdiPlus = TRUE;
    }

    if (!_fNoGdiPlus)
    {
        if (Gdiplus::Ok != GdiplusStartup(&_gdiplusToken, &_gdiplusStartupInput, nullptr))
        {
            AfxMessageBox(TEXT("Unable to initialize gdi+."), MB_ERRORFLAGS);
            _fNoGdiPlus = TRUE;
        }
    }

    return TRUE;
}


#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW 0x00020000
#endif

//
// strName is the header name, *including* the .sh
//
void AppState::OpenScriptHeader(std::string strName)
{
    if (_pApp && _pScriptTemplate)
    {
        std::string fullPath = _resourceMap.GetIncludePath(strName);
        ScriptId scriptId(fullPath);
        if (!scriptId.IsNone())
        {
            // If it's already open, just activate it.
            CMainFrame *pMainWnd = static_cast<CMainFrame*>(_pApp->m_pMainWnd);
            CScriptDocument *pDocAlready = pMainWnd->Tabs().ActivateScript(scriptId);
            if (pDocAlready == NULL)
            {
                CScriptDocument *pDocument =
                    static_cast<CScriptDocument*>(_pScriptTemplate->OpenDocumentFile(scriptId.GetFullPath().c_str(), TRUE));
                if (pDocument)
                {
                    pDocument->SetTitle(scriptId.GetFileNameOrig().c_str());
                    // Initialize the document somehow.
                }
            }
        }
    }
}

void AppState::OpenScript(WORD w)
{
    TCHAR szGameIni[MAX_PATH];
    HRESULT hr = _GetGameIni(szGameIni, ARRAYSIZE(szGameIni));
    if (SUCCEEDED(hr))
    {
        TCHAR szKeyName[MAX_PATH];
        StringCchPrintf(szKeyName, ARRAYSIZE(szKeyName), TEXT("n%03d"), w);
        TCHAR szScriptName[100];
        if (GetPrivateProfileString(GetResourceInfo(ResourceType::Script).pszTitleDefault, szKeyName, szKeyName, szScriptName, ARRAYSIZE(szScriptName), szGameIni))
        {
            OpenScript(szScriptName, NULL, w);
        }
    }
}

//
// strName is the script name, with out the .sc.
// e.g. "rm050"
//
void AppState::OpenScript(std::string strName, const ResourceBlob *pData, WORD wScriptNum)
{
    if (_pScriptTemplate && _pApp)
    {
        ScriptId scriptId = _resourceMap.Helper().GetScriptId(strName);
        if (!scriptId.IsNone())
        {
            if (wScriptNum == InvalidResourceNumber)
            {
                if (pData)
                {
                    wScriptNum = pData->GetNumber();
                }
                else
                {
                    if (FAILED(GetResourceMap().GetScriptNumber(scriptId, wScriptNum)))
                    {
                        LogInfo("Couldn't get script number for %s", scriptId.GetFullPath());
                    }
                }
            }
            scriptId.SetResourceNumber(wScriptNum);

            // If it's already open, just activate it.
            CMainFrame *pMainWnd = static_cast<CMainFrame*>(_pApp->m_pMainWnd);
            CScriptDocument *pDocAlready = pMainWnd->Tabs().ActivateScript(scriptId);
            if (pDocAlready == NULL)
            {
                std::string fullPath = scriptId.GetFullPath();
                bool fOpened = false;
                // Do an extra check first here - we don't want MFC to put up error UI if the path
                // can not be found, since we're going to do that.
                if (PathFileExists(fullPath.c_str()))
                {
                    CScriptDocument *pDocument =
                        static_cast<CScriptDocument*>(_pScriptTemplate->OpenDocumentFile(fullPath.c_str(), TRUE));
                    fOpened = (pDocument != NULL);
                    if (pDocument)
                    {
                        pDocument->SetTitle(scriptId.GetFileNameOrig().c_str());
                        // We lost context...
                        pDocument->SetScriptNumber(scriptId.GetResourceNumber());
                    }
                }
                if (!fOpened)
                {
                    std::string message = scriptId.GetFullPath();
                    message += " could not be opened.";
                    if (pData)
                    {
                        message += "\nWould you like to see the disassembly instead?";
                        if (IDYES == AfxMessageBox(message.c_str(), MB_YESNO | MB_APPLMODAL))
                        {
                            // Show the disassembly.
                            DisassembleScript((WORD)pData->GetNumber());
                        }
                    }
                    else
                    {
                        AfxMessageBox(message.c_str(), MB_OK | MB_ICONEXCLAMATION);
                    }
                }
            }
        }
    }
}

void AppState::OpenScriptAtLine(ScriptId script, int iLine)
{
    CMainFrame *pMainWnd = static_cast<CMainFrame*>(_pApp->m_pMainWnd);
    if (script.GetResourceNumber() == InvalidResourceNumber)
    {
        WORD wScriptNumber;
        GetResourceMap().GetScriptNumber(script, wScriptNumber);
        script.SetResourceNumber(wScriptNumber);
    }
    CScriptDocument *pDoc = pMainWnd->Tabs().ActivateScript(script);
    if (pDoc == NULL)
    {
        // Make an new one.
        pDoc = static_cast<CScriptDocument*>(_pScriptTemplate->OpenDocumentFile(script.GetFullPath().c_str(), TRUE));
        if (pDoc)
        {
            pDoc->SetTitle(script.GetFileNameOrig().c_str());
        }
    }
    if (pDoc)
    {
        // We lost context...
        pDoc->SetScriptNumber(script.GetResourceNumber());

        CFrameWnd *pFrame = pMainWnd->GetActiveFrame();
        if (pFrame)
        {
            CView *pView = pFrame->GetActiveView();
            if (pView->IsKindOf(RUNTIME_CLASS(CScriptView)))
            {
                CScriptView *pSV = static_cast<CScriptView*>(pView);
                int y = iLine - 1; // Off by 1
                // Ensure within bounds
                CCrystalTextBuffer *pBuffer = pSV->LocateTextBuffer();
                if (pBuffer)
                {
                    y = min(y, pBuffer->GetLineCount() - 1);
                    CPoint pt(0, y);
                    pSV->HighlightLine(pt);
                    pSV->EnsureVisible(pt);
                    pSV->SetCursorPos(pt);
                }
            }
        }
    }
}

//
// Checks to see if the resource is already open - if so, it activates it.
// Otherwise, it opens a new document for it.
//
void AppState::OpenMostRecentResource(ResourceType type, uint16_t wNum)
{
    CMainFrame *pMainWnd = static_cast<CMainFrame*>(_pApp->m_pMainWnd);
    CResourceDocument *pDocAlready = pMainWnd->Tabs().ActivateResourceDocument(type, wNum);
    if (pDocAlready == nullptr)
    {
        std::unique_ptr<ResourceBlob> blob = move(_resourceMap.MostRecentResource(type, wNum, true));
        OpenResource(blob.get());
        _resourceRecency.AddResourceToRecency(blob.get());
    }
}

void AppState::ReopenScriptDocument(uint16_t wNum)
{
    CMainFrame *pMainWnd = static_cast<CMainFrame*>(_pApp->m_pMainWnd);
    CScriptDocument *pDocAlready = pMainWnd->Tabs().GetOpenScriptDocument(wNum);
    if (pDocAlready)
    {
        pDocAlready->OnOpenDocument(GetResourceMap().Helper().GetScriptFileName(wNum).c_str());
    }
}

void AppState::OpenMostRecentResourceAt(ResourceType type, uint16_t number, int index)
{
    OpenMostRecentResource(type, number);

    // Now it should be open...
    // MDI view implementation file
    CMDIChildWnd * pChild = ((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();
    if (pChild)
    {
        CView * pView = pChild->GetActiveView();
        if (pView)
        {
            if (pView->IsKindOf(RUNTIME_CLASS(CVocabView)))
            {
                ((CVocabView*)pView)->SelectGroup(index);
            }
            else if (pView->IsKindOf(RUNTIME_CLASS(CListView)))
            {
                CListView *pTextView = (CListView *)pView;
                pTextView->GetListCtrl().SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
                pTextView->GetListCtrl().EnsureVisible(index, FALSE);
            }
        }
    }
}

// Output pane stuff
void AppState::OutputResults(OutputPaneType type, std::vector<CompileResult> &compileResults)
{
    OutputClearResults(type);
    ShowOutputPane(type);
    OutputAddBatch(type, compileResults);
    OutputFinishAdd(type);
}
void AppState::ShowOutputPane(OutputPaneType type)
{
    static_cast<CMainFrame*>(_pApp->m_pMainWnd)->ShowOutputPane(type);
}
void AppState::OutputClearResults(OutputPaneType type)
{
    static_cast<CMainFrame*>(_pApp->m_pMainWnd)->GetOutputPane().ClearResults(type);
}
void AppState::OutputAddBatch(OutputPaneType type, std::vector<CompileResult> &compileResults)
{
    static_cast<CMainFrame*>(_pApp->m_pMainWnd)->GetOutputPane().AddBatch(type, compileResults);
}
void AppState::OutputFinishAdd(OutputPaneType type)
{
    static_cast<CMainFrame*>(_pApp->m_pMainWnd)->GetOutputPane().FinishAdd(type);
}

UINT AppState::GetMidiDeviceId()
{
    UINT deviceId = 0;
    UINT cDevices = midiOutGetNumDevs();
    for (UINT i = 0; i < cDevices; i++)
    {
        MIDIOUTCAPS outcaps = { 0 };
        MMRESULT result = midiOutGetDevCaps(i, &outcaps, sizeof(outcaps));
        if (result == MMSYSERR_NOERROR)
        {
            if (0 == lstrcmpi(outcaps.szPname, _midiDeviceName.c_str()))
            {
                deviceId = i;
                break;
            }
        }
    }
    return deviceId;
}

//
// Background thread to load the class browser.
//
UINT LoadClassBrowserThreadWorker(void *pParam)
{
    SCIClassBrowser *pBrowser = (SCIClassBrowser *)pParam;
    pBrowser->Lock();
    if (!pBrowser->ReLoadFromSources())
    {
        // Might not be a fan-made game... try loading from the resources themselves so
        // that we are able to provide a class hierarchy at least.
        pBrowser->ReLoadFromCompiled();
    }
    pBrowser->Unlock();
    return 1;
}

CDocument* AppState::OpenDocumentFile(PCTSTR lpszFileName)
{
    return _pApp->OpenDocumentFile(lpszFileName);
}

void AppState::ShowResourceType(ResourceType iType)
{
    if (_pExplorerFrame)
    {
        _shownType = iType;
        if (_pResourceDoc) // Could be initial update, when we don't have it yet.
        {
            _pResourceDoc->ShowResourceType(iType);
            ((CMDIFrameWnd*)_pApp->m_pMainWnd)->MDIActivate(_pExplorerFrame);
        }
    }
}
ResourceType AppState::GetShownResourceType()
{
    return _shownType;
}

void AppState::GenerateBrowseInfo()
{
    GetResourceMap().GetClassBrowser()->SetVersion(GetVersion());
    CWinThread *pThread = AfxBeginThread(LoadClassBrowserThreadWorker, GetResourceMap().GetClassBrowser(), THREAD_PRIORITY_BELOW_NORMAL, 0, 0, NULL);
    if (pThread == NULL)
    {
        LogInfo(TEXT("Failed to create background class browser thread."));
    }
}

void AppState::ResetClassBrowser()
{
    GetResourceMap().GetClassBrowser()->Reset();
}

BOOL CALLBACK InvalidateChildProc(HWND hwnd, LPARAM lParam)
{
    if (IsWindowVisible(hwnd))
    {
        InvalidateRect(hwnd, nullptr, TRUE);
        EnumChildWindows(hwnd, InvalidateChildProc, 0);
    }
    return TRUE;
}

void AppState::NotifyChangeAspectRatio()
{
    // 1. Resource list view needs to regenerate its imagelists. Tell them that they need to reload resources.
    GetResourceMap().NotifyToReloadResourceType(ResourceType::View);
    GetResourceMap().NotifyToReloadResourceType(ResourceType::Pic);
    // 2. And then let's just refresh all windows
    CMainFrame *pMainWnd = static_cast<CMainFrame*>(_pApp->m_pMainWnd);
    if (pMainWnd)
    {
        EnumChildWindows(AfxGetMainWnd()->GetSafeHwnd(), InvalidateChildProc, 0);
    }
}

void AppState::TerminateDebuggedProcess()
{
    if (IsProcessBeingDebugged())
    {
        if (!TerminateProcessTree(_hProcessDebugged.hFile, 0))
        {
            AfxMessageBox("Unable to terminate process.", MB_OK | MB_ICONERROR);
        }
        _hProcessDebugged.Close();
        GetResourceMap().AbortDebuggerThread();
    }
}

bool AppState::IsProcessBeingDebugged()
{
    bool stillRunning = false;
    if (_hProcessDebugged.hFile && (_hProcessDebugged.hFile != INVALID_HANDLE_VALUE))
    {
        DWORD result = WaitForSingleObject(_hProcessDebugged.hFile, 0);
        stillRunning = (result == WAIT_TIMEOUT);
        if (!stillRunning)
        {
            _hProcessDebugged.Close();
            GetResourceMap().AbortDebuggerThread();
        }
    }
    return stillRunning;
}

int AppState::ExitInstance()
{
    _recentViews.clear();

    _pPicTemplate = nullptr; // Just in case someone asks us (note: don't need to free)

    DeleteDitherCritSec();

    if (!_fNoGdiPlus)
    {
        Gdiplus::GdiplusShutdown(_gdiplusToken);
    }

    return 0;
}

DWORD g_dwID = 0;
DWORD AppState::CreateUniqueRuntimeID()
{
    return g_dwID++;
}

//
// Game properties
//
std::string AppState::GetGameName()
{
    TCHAR szGameName[MAX_PATH];
    _GetGameStringProperty(TEXT("Name"), szGameName, ARRAYSIZE(szGameName));
    return szGameName;
}

void AppState::SetGameName(PCTSTR pszName)
{
    _SetGameStringProperty(TEXT("Name"), pszName);
}

std::string AppState::GetGameExecutable()
{
    TCHAR szGameExe[MAX_PATH];
    _GetGameStringProperty(c_szExecutableString, szGameExe, ARRAYSIZE(szGameExe));
    return szGameExe;

}

void AppState::SetGameExecutable(PCTSTR pszExe)
{
    _SetGameStringProperty(c_szExecutableString, pszExe);
}

std::string AppState::GetGameExecutableParameters()
{
    TCHAR szGameExe[MAX_PATH];
    _GetGameStringProperty(c_szExeParametersString, szGameExe, ARRAYSIZE(szGameExe));
    return szGameExe;

}

void AppState::RunGame(bool debug, int optionalResourceNumber)
{
    if (GetResourceMap().IsGameLoaded())
    {
        BOOL fShellEx = FALSE;
        std::string gameFolder = GetResourceMap().GetGameFolder();
        TCHAR szGameIni[MAX_PATH];
        if (SUCCEEDED(StringCchPrintf(szGameIni, ARRAYSIZE(szGameIni), TEXT("%s\\game.ini"), gameFolder.c_str())))
        {
            // Warning if any script patches are applied.
            // TODO: scan game folder for script.000, pic.000, etc...

            char szGameName[MAX_PATH];
            if (GetPrivateProfileString("Game", c_szExecutableString, c_szDefaultExe, szGameName, ARRAYSIZE(szGameName), szGameIni))
            {
                char szParameters[MAX_PATH];
                *szParameters = 0;
                GetPrivateProfileString("Game", c_szExeParametersString, "", szParameters, ARRAYSIZE(szParameters), szGameIni);
                if (SUCCEEDED(StringCchPrintf(szGameIni, ARRAYSIZE(szGameIni), "%s\\%s", gameFolder.c_str(), szGameName)))
                {
                    if (debug)
                    {
                        GetResourceMap().StartDebuggerThread(optionalResourceNumber);
                    }

                    fShellEx = TRUE;

                    SHELLEXECUTEINFO ei = {};
                    ei.fMask = SEE_MASK_DEFAULT | SEE_MASK_NOCLOSEPROCESS;
                    ei.cbSize = sizeof(ei);
                    ei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
                    ei.lpFile = szGameIni;
                    ei.lpVerb = "Open";
                    ei.lpParameters = szParameters;
                    ei.lpDirectory = gameFolder.c_str();
                    ei.nShow = SW_SHOWNORMAL;
                    // ShellExecuteEx throws an RPC exception, but I guess that's ok.
                    if (!ShellExecuteEx(&ei))
                    {
                        INT_PTR iResult = GetLastError();

                    //INT_PTR iResult = (INT_PTR)ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), 0, szGameIni, szParameters, gameFolder.c_str(), SW_SHOWNORMAL);
                    //if (iResult <= 32)
                    //{
                        // Prepare error.
                        TCHAR szReason[MAX_PATH];
                        szReason[0] = 0;
                        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, (DWORD)iResult, 0, szReason, ARRAYSIZE(szReason), NULL);

                        TCHAR szError[MAX_PATH];
                        StringCchPrintf(szError, ARRAYSIZE(szError), TEXT("Failed to start %s: %s"), szGameIni, szReason);
                        AfxMessageBox(szError, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);

                        GetResourceMap().AbortDebuggerThread();
                    }
                    else
                    {
                        _hProcessDebugged.Close();
                        _hProcessDebugged.hFile = ei.hProcess;
                    }
                }
            }
        }
        if (!fShellEx)
        {
            AfxMessageBox(TEXT("Failed to locate game executable."), MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
        }
    }
    else
    {
        AfxMessageBox(TEXT("Please load a game first."), MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
    }
}

void AppState::OnGameFolderUpdate()
{
    if (_pApp)
    {
        CMainFrame *pMainWnd = static_cast<CMainFrame*>(_pApp->m_pMainWnd);
        pMainWnd->RefreshExplorerTools();
        _hProcessDebugged.Close();
        _recentViews.clear();
    }
}

void AppState::SetGameExecutableParameters(PCTSTR pszExe)
{
    _SetGameStringProperty(c_szExeParametersString, pszExe);
}

HRESULT AppState::_GetGameStringProperty(PCTSTR pszProp, PTSTR pszValue, size_t cchValue)
{
    TCHAR szGameIni[MAX_PATH];
    HRESULT hr = _GetGameIni(szGameIni, ARRAYSIZE(szGameIni));
    if (SUCCEEDED(hr))
    {
        hr = GetPrivateProfileString(TEXT("Game"), pszProp, TEXT(""), pszValue, (DWORD)cchValue, szGameIni) ? S_OK : ResultFromLastError();
    }
    return hr;
}

HRESULT AppState::_SetGameStringProperty(PCTSTR pszProp, PCTSTR pszValue)
{
    TCHAR szGameIni[MAX_PATH];
    HRESULT hr = _GetGameIni(szGameIni, ARRAYSIZE(szGameIni));
    if (SUCCEEDED(hr))
    {
        hr = WritePrivateProfileString(TEXT("Game"), pszProp, pszValue, szGameIni) ? S_OK : ResultFromLastError();
    }
    return hr;
}

HRESULT AppState::_GetGameIni(PTSTR pszValue, size_t cchValue)
{
    HRESULT hr = E_FAIL;
    if (_resourceMap.IsGameLoaded())
    {
        hr = StringCchPrintf(pszValue, cchValue, TEXT("%s\\game.ini"), _resourceMap.GetGameFolder().c_str());
    }
    return hr;
}

const size_t RecentViewQueueSize = 10;

void AppState::SetRecentlyInteractedView(int resourceNumber)
{
    if (resourceNumber != -1)
    {
        auto itFind = find(_recentViews.begin(), _recentViews.end(), resourceNumber);
        if (itFind != _recentViews.end())
        {
            _recentViews.erase(itFind);
        }
        if (_recentViews.size() > RecentViewQueueSize)
        {
            _recentViews.erase(_recentViews.begin());
        }
        _recentViews.insert(_recentViews.begin(), resourceNumber);
    }
}

int AppState::GetSelectedViewResourceNumber()
{
    if (_recentViews.empty())
    {
        return 0;
    }
    else
    {
        return _recentViews[0];
    }
}

std::vector<int> &AppState::GetRecentViews() { return _recentViews; }

void AppState::LogInfo(const TCHAR *pszFormat, ...)
{
    if (_logFile.m_hFile != INVALID_HANDLE_VALUE)
    {
        TCHAR szMessage[MAX_PATH];
        va_list argList;
        va_start(argList, pszFormat);
        StringCchVPrintf(szMessage, ARRAYSIZE(szMessage), pszFormat, argList);
        StringCchCat(szMessage, ARRAYSIZE(szMessage), TEXT("\n"));
        _logFile.Write(szMessage, lstrlen(szMessage) * sizeof(TCHAR));
        va_end(argList);
    }
}

// AppState message handlers

// TODO: better error messages.
void DisplayFileError(HRESULT hr, BOOL fOpen, LPCTSTR pszFileName)
{
    TCHAR szMessage[MAX_PATH * 2];
    StringCchPrintf(szMessage, ARRAYSIZE(szMessage), TEXT("%s: There was an error %s this file: %x"),
        pszFileName ? pszFileName : TEXT(""),
        fOpen ? TEXT("opening") : TEXT("saving"),
        hr);
    AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP);
}
