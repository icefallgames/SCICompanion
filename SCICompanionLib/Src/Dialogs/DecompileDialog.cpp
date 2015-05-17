#include "stdafx.h"
#include "AppState.h"
#include "DecompileDialog.h"
#include "CompiledScript.h"
#include "DecompilerCore.h"
#include "SCO.h"
#include "DecompilerResults.h"
#include "GameFolderHelper.h"
#include "DecompilerConfig.h"
#include "format.h"

using namespace std;

#define UWM_UPDATESTATUS (WM_APP + 1)
#define CHEKCDONE_TIMER 3456

DecompileDialog::DecompileDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(DecompileDialog::IDD, pParent), previousSelection(-1), _inScriptListLabelEdit(false), _inSCOLabelEdit(false), initialized(false), _helper(appState->GetResourceMap().Helper()), _hThread(nullptr)
{
    if (_helper.GetGameLanguage() == LangSyntaxUnknown)
    {
        _helper.Language = LangSyntaxSCIStudio;
    }
}

BOOL DecompileDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    //ShowSizeGrip(FALSE);
    SetTimer(CHEKCDONE_TIMER, 100, nullptr);

    return fRet;
}

void DecompileDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDITSCRIPT, m_wndScript);
    DDX_Control(pDX, IDC_DECOMPILATIONRESULTS, m_wndResults);
    DDX_Control(pDX, IDC_LISTSCRIPTS, m_wndListScripts);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_DECOMPILE, m_wndDecompile);
    DDX_Control(pDX, IDC_CLEARSCO, m_wndClearSCO);
    DDX_Control(pDX, IDC_ASSIGNFILENAMES, m_wndSetFilenames);
    DDX_Control(pDX, IDC_DECOMPILECANCEL, m_wndDecomileCancel);
    DDX_Control(pDX, IDC_DECOMPILESTATUS, m_wndStatus);
    DDX_Control(pDX, IDC_CHECKCONTROLFLOW, m_wndDebugControlFlow);
    DDX_Control(pDX, IDC_CHECKINSTRUCTIONCONSUMPTION, m_wndDebugInstConsumption);
    DDX_Control(pDX, IDC_EDITDEBUGMATCH, m_wndDebugFunctionMatch);
    m_wndDebugFunctionMatch.SetWindowTextA("*");
    DDX_Control(pDX, IDC_INSTRUCTIONS, m_wndSCOLabel);

    m_wndScript.SetWindowText(
        "Start with \"Set filenames\" to give scripts meaningful names based upon their contents.\r\n"
        "\r\n"
        "Select a script on the left to decompile it. You may rename scripts as you wish.\r\n"
        "\r\n"
        "Decompiling a script will also generate a .sco file. The .sco file tracks procedure and variable names which are not present in the compiled script. By default they are given names such as \"local4\"\r\n"
        "You may edit the names to make them more meaningful, and they will be picked up the next time you decompile the script.\r\n"
        "\r\n"
        "You may delete the .sco file if you wish to clear out the names you have given."
        );
    
    DDX_Control(pDX, IDC_PROGRESS1, m_wndProgress);
    // For some reason this seems necessary, even though I'm using a marquee progress bar:
    m_wndProgress.SetRange(0, 100);
    m_wndProgress.SetPos(1);

    DDX_Control(pDX, IDC_TREESCO, m_wndTreeSCO);

    if (!initialized)
    {
        // Ensure we have a src directory
        string sourceFolder = _helper.GetSrcFolder();
        if (!PathFileExists(sourceFolder.c_str()))
        {
            if (!CreateDirectory(sourceFolder.c_str(), nullptr))
            {
                std::string error = GetMessageFromLastError(sourceFolder);
                AfxMessageBox(error.c_str(), MB_OK | MB_APPLMODAL);
            }
        }

        // Ensure we have a decompiler.ini
        string decompilerIniFile = sourceFolder + "\\Decompiler.ini";
        if (!PathFileExists(decompilerIniFile.c_str()))
        {
            string decompilerFolderContents = appState->GetResourceMap().GetDecompilerFolder();
            decompilerFolderContents += "\\*.*";
            CopyFilesOver(GetSafeHwnd(), decompilerFolderContents, sourceFolder);
        }

        _InitScriptList();
        _PopulateScripts();
        initialized = true;
    }
}

const int NameColumn = 0;
const int NumberColumn = 1;
const int SCOColumn = 2;
const int SourceColumn = 3;

struct
{
    char *text;
    int width;
    int id;
}
c_DecompileColumns[] =
{
    { "Name", 90, NameColumn },
    { "Num", 45, NumberColumn },
    { "SCO", 35, SCOColumn },
    { "Src", 30, SourceColumn },
};

void DecompileDialog::_InitScriptList()
{
    for (int i = 0; i < ARRAYSIZE(c_DecompileColumns); i++)
    {
        LVCOLUMN col = { 0 };
        col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        col.iOrder = i;
        col.iSubItem = c_DecompileColumns[i].id;
        col.pszText = c_DecompileColumns[i].text;
        col.cx = c_DecompileColumns[i].width;
        col.fmt = LVCFMT_LEFT;
        m_wndListScripts.InsertColumn(i, &col);
    }
}

char c_Yes[] = "Yes";
char c_Empty[] = "";

void DecompileDialog::_UpdateScripts(set<uint16_t> updatedScripts)
{
    for (int i = 0; i < m_wndListScripts.GetItemCount(); i++)
    {
        uint16_t scriptNum = (uint16_t)m_wndListScripts.GetItemData(i);
        if (contains(updatedScripts, scriptNum))
        {
            string name = _helper.FigureOutName(ResourceType::Script, scriptNum);
            LVITEM item = {};
            item.iItem = i;

            item.mask = LVIF_TEXT;
            item.iSubItem = SCOColumn;
            string scoFilename = _helper.GetScriptObjectFileName(name);
            item.pszText = PathFileExists(scoFilename.c_str()) ? c_Yes : c_Empty;
            m_wndListScripts.SetItem(&item);

            item.mask = LVIF_TEXT;
            item.iSubItem = SourceColumn;
            string scriptFilename = _helper.GetScriptFileName(name);
            item.pszText = PathFileExists(scriptFilename.c_str()) ? c_Yes : c_Empty;
            m_wndListScripts.SetItem(&item);
        }
    }
    _SyncSelection();
}

int CALLBACK _SortListByNumber(LPARAM lpOne, LPARAM lpTwo, LPARAM lpArg)
{
    return ((int)lpOne - (int)lpTwo);
}

void DecompileDialog::_PopulateScripts()
{
    SetRedraw(FALSE);
    m_wndListScripts.DeleteAllItems();

    auto scriptResources = _helper.Resources(ResourceTypeFlags::Script, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::NameLookups);
    int itemNumber = 0;
    for (auto &blob : *scriptResources)
    {
        string name = blob->GetName();
        LVITEM item = {};
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.pszText = const_cast<LPSTR>(name.c_str());
        item.iItem = itemNumber;
        item.iSubItem = NameColumn;
        item.lParam = blob->GetNumber();    // associated data is the script number
        m_wndListScripts.InsertItem(&item);

        item.mask = LVIF_TEXT;
        item.iSubItem = NumberColumn;
        string scriptNumString = fmt::format("{0}", blob->GetNumber());
        item.pszText = const_cast<LPSTR>(scriptNumString.c_str());
        m_wndListScripts.SetItem(&item);

        item.mask = LVIF_TEXT;
        item.iSubItem = SCOColumn;
        string scoFilename = _helper.GetScriptObjectFileName(name);
        item.pszText = PathFileExists(scoFilename.c_str()) ? c_Yes : c_Empty;
        m_wndListScripts.SetItem(&item);

        item.mask = LVIF_TEXT;
        item.iSubItem = SourceColumn;
        string scriptFilename = _helper.GetScriptFileName(name);
        item.pszText = PathFileExists(scriptFilename.c_str()) ? c_Yes : c_Empty;
        m_wndListScripts.SetItem(&item);

        itemNumber++;
    }
    m_wndListScripts.SortItems(_SortListByNumber, 0);
    SetRedraw(TRUE);
}

LPARAM _IndexToParam(bool isVariable, int index)
{
    return index | (isVariable ? 0x80000000 : 0);
}
void _ParamToIndex(LPARAM param, int &index, bool &isVariable)
{
    isVariable = (param & 0x80000000) != 0;
    index = param & ~0x80000000;
}

void DecompileDialog::_PopulateSCOTree()
{
    m_wndTreeSCO.DeleteAllItems();
    if (_sco)
    {
        TVINSERTSTRUCT exportRoot = {};
        exportRoot.hParent = TVI_ROOT;
        exportRoot.hInsertAfter = TVI_FIRST;
        exportRoot.item.pszText = "Public procedures";
        exportRoot.item.mask = TVIF_TEXT;
        HTREEITEM hExportRoot = m_wndTreeSCO.InsertItem(&exportRoot);

        TVINSERTSTRUCT varRoot = {};
        varRoot.hParent = TVI_ROOT;
        varRoot.hInsertAfter = hExportRoot;
        varRoot.item.pszText = "Variables";
        varRoot.item.mask = TVIF_TEXT;
        HTREEITEM hVarRoot = m_wndTreeSCO.InsertItem(&varRoot);

        HTREEITEM hPrevious = TVI_FIRST;
        int index = 0;
        for (auto &publicProc : _scoPublicProcIndices)
        {
            TVINSERTSTRUCT item = {};
            item.hParent = hExportRoot;
            item.hInsertAfter = hPrevious;
            string name = _sco->GetExports()[publicProc].GetName();
            item.item.pszText = const_cast<PSTR>(name.c_str());
            item.item.mask = TVIF_TEXT | TVIF_PARAM;
            item.item.lParam = _IndexToParam(false, index);
            hPrevious = m_wndTreeSCO.InsertItem(&item);
            index++;
        }

        hPrevious = TVI_FIRST;
        index = 0;
        for (auto &variable : _sco->GetVariables())
        {
            TVINSERTSTRUCT item = {};
            item.hParent = hVarRoot;
            item.hInsertAfter = hPrevious;
            item.item.pszText = const_cast<PSTR>(variable.GetName().c_str());
            item.item.mask = TVIF_TEXT | TVIF_PARAM;
            item.item.lParam = _IndexToParam(true, index);
            hPrevious = m_wndTreeSCO.InsertItem(&item);
            index++;
        }

        m_wndTreeSCO.Expand(hExportRoot, TVE_EXPAND);
        m_wndTreeSCO.Expand(hVarRoot, TVE_EXPAND);
    }
}

void DecompileDialog::_SyncSelection(bool force)
{
    UINT selectedCount = m_wndListScripts.GetSelectedCount();
    bool selected = (selectedCount > 0);

    _SyncButtonState();

    if (selected)
    {
        POSITION pos = m_wndListScripts.GetFirstSelectedItemPosition();
        int selectedItem = m_wndListScripts.GetNextSelectedItem(pos);
        if (force || (selectedItem != previousSelection))
        {
            LPARAM param = m_wndListScripts.GetItemData(selectedItem);
            string sourceFileName = _helper.GetScriptFileName((uint16_t)param);
            std::ifstream scriptFile(sourceFileName.c_str());
            if (scriptFile.is_open())
            {
                std::string scriptText;
                std::string line;
                while (std::getline(scriptFile, line))
                {
                    scriptText += line;
                    scriptText += "\r\n";
                }
                m_wndScript.SetWindowText(scriptText.c_str());
            }
            else
            {
                m_wndScript.SetWindowTextA("Source not found.");
            }

            // Load the .sco file
            _sco.reset(nullptr);
            _scoPublicProcIndices.clear();
            _sco = GetExistingSCOFromScriptNumber(_helper, (uint16_t)param);
            if (_sco)
            {
                // Detect which exports are not procedures by seeing if its name
                // matches a public instance in the compiled script (which should be sync'd with the SCO)
                CompiledScript compiledScript((uint16_t)param);
                compiledScript.Load(_helper, _helper.Version, param, true);
                int exportIndex = 0;
                for (auto &publicExport : _sco->GetExports())
                {
                    bool isPublicProc = true;
                    for (auto &object : compiledScript.GetObjects())
                    {
                        if (object->IsPublic && (object->GetName() == publicExport.GetName()))
                        {
                            isPublicProc = false;   // It's an object
                            break;
                        }
                    }
                    if (isPublicProc)
                    {
                        _scoPublicProcIndices.push_back(exportIndex);
                    }
                    exportIndex++;
                }
            }
            _PopulateSCOTree();

            previousSelection = selectedItem;
        }
    }
    else
    {
        m_wndScript.SetWindowTextA("");
        m_wndTreeSCO.DeleteAllItems();
        _sco.reset(nullptr);
        _scoPublicProcIndices.clear();
    }

    // TODO Sync single selection...
}

BEGIN_MESSAGE_MAP(DecompileDialog, CExtResizableDialog)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTSCRIPTS, &DecompileDialog::OnLvnItemchangedListscripts)
    ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREESCO, &DecompileDialog::OnTvnBeginlabeleditTreesco)
    ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREESCO, &DecompileDialog::OnTvnEndlabeleditTreesco)
    ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LISTSCRIPTS, &DecompileDialog::OnLvnBeginlabeleditListscripts)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LISTSCRIPTS, &DecompileDialog::OnLvnEndlabeleditListscripts)
    ON_BN_CLICKED(IDC_DECOMPILE, &DecompileDialog::OnBnClickedDecompile)
    ON_BN_CLICKED(IDC_ASSIGNFILENAMES, &DecompileDialog::OnBnClickedAssignfilenames)
    ON_BN_CLICKED(IDC_DECOMPILECANCEL, &DecompileDialog::OnBnClickedDecompilecancel)
    ON_WM_TIMER()
    ON_MESSAGE(UWM_UPDATESTATUS, UpdateStatus)
    ON_BN_CLICKED(IDC_CLEARSCO, &DecompileDialog::OnBnClickedClearsco)
END_MESSAGE_MAP()

// All this to handle the user pressing enter on a listview item.
BOOL DecompileDialog::PreTranslateMessage(MSG* pMsg)
{
    if (_inSCOLabelEdit || _inScriptListLabelEdit)
    {
        if (WM_KEYDOWN == pMsg->message &&
            (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
        {
            if (_inSCOLabelEdit)
            {
                m_wndTreeSCO.SendMessage(TVM_ENDEDITLABELNOW, (VK_ESCAPE == pMsg->wParam));
            }
            else
            {
                if (TranslateMessage(pMsg))
                {
                    DispatchMessage(pMsg);
                }
            }
            return TRUE;
        }
    }
    else
    {
        if (WM_KEYDOWN == pMsg->message &&
            (VK_F2 == pMsg->wParam))
        {
            if (GetFocus()->GetSafeHwnd() == m_wndListScripts.GetSafeHwnd())
            {
                int selectedItem = m_wndListScripts.GetSelectionMark();
                if (selectedItem != -1)
                {
                    m_wndListScripts.EditLabel(selectedItem);
                }
                return TRUE;
            }
            else if (GetFocus()->GetSafeHwnd() == m_wndTreeSCO.GetSafeHwnd())
            {
                HTREEITEM hTreeItem = m_wndTreeSCO.GetSelectedItem();
                if (hTreeItem != nullptr)
                {
                    m_wndTreeSCO.EditLabel(hTreeItem);
                }
                return TRUE;
            }
        }
    }
    return __super::PreTranslateMessage(pMsg);
}

void DecompileDialog::OnLvnItemchangedListscripts(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    BOOL bSelectedNow = (pNMLV->uNewState  & LVIS_SELECTED);
    BOOL bSelectedBefore = (pNMLV->uOldState  & LVIS_SELECTED);
    if (bSelectedNow && (bSelectedNow != bSelectedBefore))
    {
        _SyncSelection();
    }
    *pResult = 0;
}


void DecompileDialog::OnTvnBeginlabeleditTreesco(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    if (m_wndTreeSCO.GetParentItem(pTVDispInfo->item.hItem) != TVI_ROOT)
    {
        _inSCOLabelEdit = true;
    }
    *pResult = !_inSCOLabelEdit;
}


void DecompileDialog::OnTvnEndlabeleditTreesco(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    if (pTVDispInfo->item.pszText)
    {
        // Label was edited. Reject it if it doesn't match our standards, or is a dupe of another
        // TODO: Validate against variable standard (e.g. alphanum, etc...)
        // TODO: check it's not dupe of another
        bool isVariable;
        int index;
        _ParamToIndex(pTVDispInfo->item.lParam, index, isVariable);
        // Note that the array index does not correspond to the actual export index. That's ok though.
        if (isVariable)
        {
            _sco->GetVariables()[index].SetName(pTVDispInfo->item.pszText);
        }
        else
        {
            _sco->GetExports()[_scoPublicProcIndices[index]].SetName(pTVDispInfo->item.pszText);
        }
        *pResult = 1;
        SaveSCOFile(_helper, *_sco);
        m_wndStatus.SetWindowTextA(fmt::format("Saved changes to {0}",
            PathFindFileName(_helper.GetScriptObjectFileName(_sco->GetScriptNumber()).c_str())).c_str()
            );
    }
    else
    {
        *pResult = 0;
    }
    _inSCOLabelEdit = false;
}


void DecompileDialog::OnLvnBeginlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    _inScriptListLabelEdit = true;
    *pResult = 0;
}


void DecompileDialog::OnLvnEndlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

    if (pDispInfo->item.pszText)
    {
        uint16_t scriptNumber = (uint16_t)pDispInfo->item.lParam;
        // Rename the .sco and .sc files
        string scOld = _helper.GetScriptFileName(scriptNumber);
        string scoOld = _helper.GetScriptObjectFileName(scriptNumber);
        appState->GetResourceMap().AssignName(ResourceType::Script, scriptNumber, pDispInfo->item.pszText);
        
        // And move them.
        try
        {
            movefile(scOld, _helper.GetScriptFileName(scriptNumber));
            movefile(scoOld, _helper.GetScriptObjectFileName(scriptNumber));
        }
        catch (std::exception &e)
        {
            AfxMessageBox(e.what(), MB_ICONWARNING | MB_OK);
        }

        *pResult = 1;
    }
    else
    {
        *pResult = 0;
    }

    _inScriptListLabelEdit = false;
}

void DecompileDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == CHEKCDONE_TIMER)
    {
        if (_hThread && (_hThread != INVALID_HANDLE_VALUE))
        {
            DWORD result = WaitForSingleObject(_hThread, 0);
            if (result == WAIT_OBJECT_0)
            {
                _hThread = INVALID_HANDLE_VALUE;
                _decompileResults.reset(nullptr);
                _pThread.reset(nullptr);
                _SyncButtonState();
                m_wndStatus.SetWindowTextA("");
                _UpdateScripts(_scriptNumbers);
                _SyncSelection(true);

                for (uint16_t scriptNumber : _scriptNumbers)
                {
                    appState->ReopenScriptDocument(scriptNumber);
                }
            }
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

void DecompileDialog::OnBnClickedDecompile()
{
    m_wndResults.SetWindowTextA("");

    _debugControlFlow = m_wndDebugControlFlow.GetCheck() != 0;
    _debugInstConsumption = m_wndDebugInstConsumption.GetCheck() != 0;
    m_wndDebugFunctionMatch.GetWindowTextA(_debugFunctionMatch);

    // Get a list of scripts to decompile
    _scriptNumbers.clear();
    POSITION pos = m_wndListScripts.GetFirstSelectedItemPosition();
    while (pos != nullptr)
    {
        int selectedItem = m_wndListScripts.GetNextSelectedItem(pos);
        uint16_t scriptNumber = (uint16_t)m_wndListScripts.GetItemData(selectedItem);
        _scriptNumbers.insert(scriptNumber);
    }

    if (!_scriptNumbers.empty())
    {
        _pThread.reset(AfxBeginThread(s_DecompileThreadWorker, this, 0, 0, CREATE_SUSPENDED, nullptr));
        if (_pThread)
        {
            _decompileResults = make_unique<DecompilerDialogResults>(this->GetSafeHwnd());
            _SyncButtonState();
            _pThread->m_bAutoDelete = FALSE;
            _hThread = _pThread->m_hThread;
            _pThread->ResumeThread();
        }
    }
}

const int MarqueeMilliseconds = 30;

void DecompileDialog::_SyncButtonState()
{
    UINT selectedCount = m_wndListScripts.GetSelectedCount();
    bool selected = (selectedCount > 0);
    bool decompiling = (_decompileResults != nullptr);

    m_wndCancel.EnableWindow(!decompiling);
    m_wndDecompile.EnableWindow(!decompiling && selected);
    m_wndClearSCO.EnableWindow(!decompiling && selected);
    m_wndSetFilenames.EnableWindow(!decompiling);
    m_wndDecomileCancel.EnableWindow(decompiling);
    m_wndProgress.ShowWindow(decompiling ? SW_SHOW : SW_HIDE);
    m_wndProgress.SendMessage(PBM_SETMARQUEE, decompiling, MarqueeMilliseconds);
}

void DecompileDialog::OnBnClickedAssignfilenames()
{
    unordered_set<string> importantClasses = { "Game" }; // e.g. needed for KQ6, 994

    unordered_set<string> usedNames;

    GlobalCompiledScriptLookups *lookups = appState->GetResourceMap().GetCompiledScriptLookups();
    if (lookups)
    {
        for (CompiledScript *script : lookups->GetGlobalClassTable().GetAllScripts())
        {
            string suggestedName;
            if (script->GetScriptNumber() == 0)
            {
                suggestedName = "Main";
            }
            else
            {
                // Look for the first class in the file. If none found, then the first public instance.
                string firstPublicInstance;
                string firstClass;
                for (const auto &object : script->GetObjects())
                {
                    if (!object->IsInstance() && (firstClass.empty() || importantClasses.find(object->GetName()) != importantClasses.end()))
                    {
                        firstClass = object->GetName();
                    }
                    else if (object->IsInstance() && object->IsPublic && firstPublicInstance.empty())
                    {
                        firstPublicInstance = object->GetName();
                    }
                }
                suggestedName = firstClass.empty() ? firstPublicInstance : firstClass;
            }
            if (!suggestedName.empty())
            {
                // Make it unique if we already named something this (this happens in LSL6)
                if (usedNames.find(suggestedName) != usedNames.end())
                {
                    suggestedName += fmt::format("_{0}", script->GetScriptNumber());
                }
                usedNames.insert(suggestedName);

                appState->GetResourceMap().AssignName(ResourceType::Script, script->GetScriptNumber(), suggestedName.c_str());
            }
        }

        _PopulateScripts();
        // For some reason assigning names doesn't cause it to repaint, so invalidate:
        m_wndListScripts.Invalidate(FALSE);
    }
}

void DecompileDialog::OnBnClickedDecompilecancel()
{
    if (_decompileResults)
    {
        _decompileResults->SetAborted();
    }
}

UINT DecompileDialog::s_DecompileThreadWorker(void *pParam)
{
    DecompileDialog *pThis = reinterpret_cast<DecompileDialog*>(pParam);

    try
    {
        set<uint16_t> scriptNumbers = pThis->_scriptNumbers;
        GameFolderHelper helper = pThis->_helper;

        if (!pThis->_lookups)
        {
            pThis->_decompileResults->AddResult(DecompilerResultType::Update, "Creating script lookups...");
            pThis->_lookups = make_unique<GlobalCompiledScriptLookups>();
            pThis->_lookups->Load(helper);
            pThis->_decompileResults->AddResult(DecompilerResultType::Update, "Loading decompiler.ini...");
        }

        // Redo this one each time
        pThis->_decompilerConfig = CreateDecompilerConfig(helper, pThis->_lookups->GetSelectorTable());
        if (!pThis->_decompilerConfig->error.empty())
        {
            string errorMessage = "Decompiler.ini: ";
            errorMessage += pThis->_decompilerConfig->error;
            pThis->_decompileResults->AddResult(DecompilerResultType::Warning, errorMessage);
        }

        if (pThis->_lookups)
        {
            for (uint16_t scriptNum : scriptNumbers)
            {
                pThis->_decompileResults->AddResult(DecompilerResultType::Important, fmt::format("Decompiling script {0}", scriptNum));
                CompiledScript compiledScript(0);
                if (compiledScript.Load(helper, helper.Version, scriptNum, false))
                {
                    if (pThis->_decompileResults->IsAborted())
                    {
                        pThis->_decompileResults->AddResult(DecompilerResultType::Warning, "Operation aborted");
                    }
                    else
                    {
                        unique_ptr<sci::Script> pScript = DecompileScript(pThis->_decompilerConfig.get(), *pThis->_lookups, helper, scriptNum, compiledScript, *pThis->_decompileResults, pThis->_debugControlFlow, pThis->_debugInstConsumption, (PCSTR)pThis->_debugFunctionMatch);
                        // Dump it to the .sc file
                        // TODO: If it already exists, we might want to ask for confirmation.
                        std::stringstream ss;
                        sci::SourceCodeWriter out(ss, helper.GetGameLanguage(), pScript.get());
                        pScript->OutputSourceCode(out);
                        string sourceFilename = helper.GetScriptFileName(scriptNum);
                        MakeTextFile(ss.str().c_str(), sourceFilename);
                        pThis->_decompileResults->AddResult(DecompilerResultType::Important, fmt::format("Generated {0}", sourceFilename));
                    }
                }
            }
        }
    }
    catch (...)
    {
        int x = 0;
    }

    // Stats reporting
    int divisor = max(1, (pThis->_decompileResults->_successCount + pThis->_decompileResults->_fallbackCount));     // avoid / by zero
    int successPercentage = pThis->_decompileResults->_successCount * 100 / divisor;
    int divisorBytes = max(1, (pThis->_decompileResults->_successBytes + pThis->_decompileResults->_fallbackBytes));     // avoid / by zero
    int successBytesPercentage = pThis->_decompileResults->_successBytes * 100 / divisorBytes;
    pThis->_decompileResults->AddResult(DecompilerResultType::Important,
        fmt::format("Decompiled {0} of {1} functions successfully ({2}%).", pThis->_decompileResults->_successCount, (pThis->_decompileResults->_successCount + pThis->_decompileResults->_fallbackCount), successPercentage)
        );

    pThis->_decompileResults->AddResult(DecompilerResultType::Important,
        fmt::format("Overall bytecount success rate: {0}%.", successBytesPercentage)
        );

    if (pThis->_decompileResults->_fallbackCount)
    {
        pThis->_decompileResults->AddResult(DecompilerResultType::Important, "Fell back to assembly for the remaining functions.");
    }

    return 0;
}

void DecompilerDialogResults::AddResult(DecompilerResultType type, const std::string &message)
{
    std::string *ptrToString = new std::string(message);
    ::PostMessage(_hwnd, UWM_UPDATESTATUS, static_cast<WPARAM>(type), reinterpret_cast<LPARAM>(ptrToString));
}

void DecompilerDialogResults::InformStats(bool functionSuccessful, int byteCount)
{
    if (functionSuccessful)
    {
        _successCount++;
        _successBytes += byteCount;
    }
    else
    {
        _fallbackCount++;
        _fallbackBytes += byteCount;
    }
}

LRESULT DecompileDialog::UpdateStatus(WPARAM wParam, LPARAM lParam)
{
    DecompilerResultType type = static_cast<DecompilerResultType>(wParam);
    std::string *stringPtr = reinterpret_cast<std::string*>(lParam);

    std::string preamble;
    if (type == DecompilerResultType::Warning)
    {
        preamble = "WARNING: ";
    }
    else if (type == DecompilerResultType::Error)
    {
        preamble = "ERROR: ";
    }
    std::string display = fmt::format("{0}{1}",
        preamble,
        *stringPtr
        );

    if (type == DecompilerResultType::Update)
    {
        m_wndStatus.SetWindowTextA(display.c_str());
    }
    else
    {
        CString str;
        m_wndResults.GetWindowTextA(str);
        if (!str.IsEmpty())
        {
            str.Append("\r\n");
        }
        str.Append(display.c_str());
        m_wndResults.SetWindowTextA(str);
        m_wndResults.LineScroll(m_wndResults.GetLineCount());
    }
    delete stringPtr;
    return 0;
}


void DecompileDialog::OnBnClickedClearsco()
{
    // Get a list of scripts to clear SCOs
    vector<string> scosDeleted;
    set<uint16_t> scriptsToUpdate;
    POSITION pos = m_wndListScripts.GetFirstSelectedItemPosition();
    while (pos != nullptr)
    {
        int selectedItem = m_wndListScripts.GetNextSelectedItem(pos);
        uint16_t scriptNumber = (uint16_t)m_wndListScripts.GetItemData(selectedItem);
        string scoFilename = _helper.GetScriptObjectFileName(scriptNumber);
        try
        {
            deletefile(scoFilename);
            scosDeleted.push_back(scoFilename);
            scriptsToUpdate.insert(scriptNumber);
        }
        catch (std::exception &e)
        {
            AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
            break;
        }
    }

    if (!scosDeleted.empty())
    {
        if (scosDeleted.size() == 1)
        {
            m_wndResults.SetWindowTextA(fmt::format("Deleted {0}", scosDeleted[0]).c_str());
        }
        else
        {
            m_wndResults.SetWindowTextA(fmt::format("Deleted {0} .sco files", scosDeleted.size()).c_str());
        }
        _SyncSelection(true);
    }
    _UpdateScripts(scriptsToUpdate);
}
