#include "stdafx.h"
#include "AppState.h"
#include "DecompileDialog.h"
#include "CompiledScript.h"
#include "DecompilerCore.h"
#include "SCO.h"
#include "DecompilerResults.h"
#include "GameFolderHelper.h"
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
    { "Name", 70, NameColumn },
    { "Number", 55, NumberColumn },
    { "SCO", 30, SCOColumn },
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
                m_wndListScripts.SendMessage(TVM_ENDEDITLABELNOW, (VK_ESCAPE == pMsg->wParam));
            }
            return TRUE;
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
        // TODO: a message indicating we saved.
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
    // TODO: Add your control notification handler code here
    *pResult = 0;
}


void DecompileDialog::OnLvnEndlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
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
        _pThread.reset(AfxBeginThread(s_ThreadWorker, this, 0, 0, CREATE_SUSPENDED, nullptr));
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
}

void DecompileDialog::OnBnClickedAssignfilenames()
{
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
                for (const auto &object : script->GetObjects())
                {
                    if (!object->IsInstance())
                    {
                        suggestedName = object->GetName();
                        break;
                    }
                }
                if (suggestedName.empty())
                {
                    for (const auto &object : script->GetObjects())
                    {
                        if (object->IsInstance() && object->IsPublic)
                        {
                            suggestedName = object->GetName();
                            break;
                        }
                    }
                }
            }
            if (!suggestedName.empty())
            {
                appState->GetResourceMap().AssignName(ResourceType::Script, script->GetScriptNumber(), suggestedName.c_str());
            }
        }

        _PopulateScripts();
    }
}

void DecompileDialog::OnBnClickedDecompilecancel()
{
    if (_decompileResults)
    {
        _decompileResults->SetAborted();
    }
}

UINT DecompileDialog::s_ThreadWorker(void *pParam)
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
                        unique_ptr<sci::Script> pScript = DecompileScript(*pThis->_lookups, helper, scriptNum, compiledScript, *pThis->_decompileResults, pThis->_debugControlFlow, pThis->_debugInstConsumption);
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

    return 0;
}

void DecompilerDialogResults::AddResult(DecompilerResultType type, const std::string &message)
{
    std::string *ptrToString = new std::string(message);
    ::PostMessage(_hwnd, UWM_UPDATESTATUS, static_cast<WPARAM>(type), reinterpret_cast<LPARAM>(ptrToString));
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
    }
    delete stringPtr;
    return 0;
}
