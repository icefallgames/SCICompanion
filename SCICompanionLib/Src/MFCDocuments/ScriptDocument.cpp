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

// ScriptDocument.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ScriptDocument.h"
#include "FindAllDialog.h"
#include "NewCompileDialog.h"
#include "SCO.h"
#include "CrystalScriptStream.h"
#include "ClassBrowserDialog.h"
#include "CompiledScript.h"
#include "SyntaxParser.h"
#include "CompileContext.h"
#include "ScriptOMAll.h"
#include "DecompilerCore.h"
#include <ctime>
#include "CObjectWrap.h"
#include "Text.h"
#include "ResourceEntity.h"
#include "Disassembler.h"
#include "SummarizeScript.h"
#include "DecompileScript.h"
#include "DecompilerResults.h"
#include "GameFolderHelper.h"
#include "DecompilerConfig.h"
#include "format.h"
#include "ResourceBlob.h"
#include "DependencyTracker.h"
#include "OutputCodeHelper.h"
#include "ScriptConvert.h"
#include <filesystem>

using namespace std;

bool CompileLog::HasErrors()
{
    return _cErrors > 0;
}

void CompileLog::CalculateErrors()
{
    // Calculate errors;
    _cErrors += (int)count_if(_compileResults.begin(), _compileResults.end(), mem_fun_ref(&CompileResult::IsError));
    _cWarnings += (int)count_if(_compileResults.begin(), _compileResults.end(), mem_fun_ref(&CompileResult::IsWarning));
}


void CompileLog::SummarizeAndReportErrors()
{
    stringstream summaryMessage;
    summaryMessage << _cErrors << " errors, " << _cWarnings << " warnings.";
    ReportResult(CompileResult(summaryMessage.str()));

    if (_cErrors && appState->_fPlayCompileErrorSound)
    {
        // Play a sound.
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMEXCLAMATION, NULL, SND_ALIAS_ID | SND_ASYNC);
    }
}

// CScriptDocument

IMPLEMENT_DYNCREATE(CScriptDocument, CDocument)

CScriptDocument::CScriptDocument() : _buffer(*this), _dependencyTracker(nullptr)
{
}

CScriptDocument::~CScriptDocument()
{
    _buffer.FreeAll();
}

void CScriptDocument::SetDependencyTracker(DependencyTracker &tracker)
{
    _dependencyTracker = &tracker;
}

BEGIN_MESSAGE_MAP(CScriptDocument, CDocument)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
    ON_COMMAND(ID_COMPILE, OnCompile)
    ON_COMMAND(ID_SCRIPT_DISASSEMBLE, OnDisassemble)
    ON_COMMAND(ID_SCRIPT_VIEWOBJECTFILE, OnViewObjectFile)
    ON_COMMAND(ID_SCRIPT_VIEWSCRIPTRESOURCE, OnViewScriptResource)
    ON_COMMAND(ID_SCRIPT_VIEWSYNTAXTREE, OnViewSyntaxTree)
    ON_COMMAND(ID_CONVERTSCRIPT, OnConvertScript)
    ON_COMMAND(ID_DEBUGROOM, OnDebugRoom)
    ON_UPDATE_COMMAND_UI(ID_COMPILE, OnUpdateIsScript)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_DISASSEMBLE, OnUpdateIsScript)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_VIEWOBJECTFILE, OnUpdateIsScript)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_VIEWSCRIPTRESOURCE, OnUpdateIsScript)
    ON_UPDATE_COMMAND_UI(ID_DEBUGROOM, OnUpdateIsScript)
    ON_UPDATE_COMMAND_UI(ID_CONVERTSCRIPT, OnUpdateConvertScript)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINECOUNT, OnUpdateLineCount)
END_MESSAGE_MAP()



CCrystalTextBuffer *CScriptDocument::GetTextBuffer()
{
    return &_buffer;
}

void CScriptDocument::OnUpdateIsScript(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!_scriptId.IsHeader());
}

const char c_szLine[] = "--------------------------------------------------------";
void CScriptDocument::OnCompile()
{
    if (_scriptId.IsHeader())
    {
        AfxMessageBox("Header files can not be compiled.", MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL, 0);
    }
    else
    {
        // Check if the script is modified, and if so, save it.
        if (_buffer.IsModified())
        {
            OnFileSave();
        }

        DeferResourceAppend defer(appState->GetResourceMap());
        CompileLog log;
        _ClearErrorCount();
        CompileTables tables;
        tables.Load(appState->GetVersion());
        PrecompiledHeaders headers(appState->GetResourceMap());
        CompileResults results(log);
        bool fSuccess = NewCompileScript(results, log, tables, headers, _scriptId);
        if (fSuccess)
        {
            tables.Save();
        }

        // put a timestamp in.
        char sz[100];
        struct tm time;
#pragma warning (push)
#pragma warning (disable: 4996)
        _getsystime(&time);
#pragma warning (pop)

        strftime(sz, ARRAYSIZE(sz), "%I:%M:%S%p", &time);
        log.ReportResult(CompileResult(sz));

        stringstream str;
        str << "Compiling " << _scriptId.GetFileName();
        str << (fSuccess ? " succeeded." : " failed.");
        log.ReportResult(CompileResult(c_szLine));
        log.ReportResult(CompileResult(str.str()));

        string info = fmt::format(
            "Object data: {0} bytes   Code: {1} bytes   Script vars: {2} bytes   Strings: {3} bytes    Saids: {4} bytes",
            results.Stats.Objects,
            results.Stats.Code,
            results.Stats.Locals,
            results.Stats.Strings,
            results.Stats.Saids
        );
        log.ReportResult(CompileResult(info));

        HRESULT hr = defer.Commit();
        if (FAILED(hr))
        {
            char sz[200];
            StringCchPrintf(sz, ARRAYSIZE(sz), "There was a problem writing the compiled script: %x", hr);
            log.ReportResult(CompileResult(sz));
            char *dummyFN[1];
            dummyFN[0] = "";
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, HRESULT_CODE(hr), 0,  sz, ARRAYSIZE(sz), dummyFN);
            log.ReportResult(CompileResult(sz, CompileResult::CRT_Error));
            log.CalculateErrors();
        }
        _DoErrorSummary(log);

        appState->OutputResults(OutputPaneType::Compile, log.Results());
    }
}

std::unique_ptr<sci::Script> SimpleCompile(CompileLog &log, ScriptId &scriptId, bool addCommentsToOM)
{
    std::unique_ptr<sci::Script> script = make_unique<sci::Script>();
    script->SetScriptId(scriptId);
    // Make a new buffer.
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(scriptId.GetFullPath().c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);
        if (SyntaxParser_Parse(*script, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log, addCommentsToOM))
        {

        }
    }
    log.CalculateErrors();
    buffer.FreeAll();
    return script;
}

bool NewCompileScript(CompileResults &results, CompileLog &log, CompileTables &tables, PrecompiledHeaders &headers, ScriptId &script)
{
    bool fRet = false;
	ClassBrowserLock lock(appState->GetClassBrowser());
    lock.Lock();

    // Make a new buffer.
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(script.GetFullPath().c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);

		std::unique_ptr<sci::Script> pScript = std::make_unique<sci::Script>(script);

        if (SyntaxParser_Parse(*pScript, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log))
        {
            if (script.GetResourceNumber() != pScript->GetScriptNumber())
            {
                log.ReportResult(
                    CompileResult(fmt::format("Script {0} ({1}) declared itself as resource {2}", script.GetResourceNumber(), script.GetTitle(), pScript->GetScriptNumber()),
                    CompileResult::CompileResultType::CRT_Warning));
            }

            // Compile and save script resource.
            // Compile our own script!
            if (GenerateScriptResource(appState->GetVersion(), *pScript, headers, tables, results, appState->GetResourceMap().Helper().GetGenerateDebugInfo()))
            {
                WORD wNum = results.GetScriptNumber();

                // Save the text resource - but only if it's different than what's there (otherwise needless text resource turds pile up)
                if (!results.GetTextComponent().Texts.empty())
                {
                    assert(script.Language() != LangSyntaxStudio);

                    ResourceEntity &textResource = results.GetTextResource();
                    // Mark it as being auto-generated by a script compile:
                    textResource.GetComponent<TextComponent>().AddString(AutoGenTextSentinel);

                    auto existingTextResource = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::Text, textResource.ResourceNumber);
                    if (!existingTextResource || !existingTextResource->GetComponent<TextComponent>().AreTextsEqual(textResource.GetComponent<TextComponent>()))
                    {
                        appState->GetResourceMap().AppendResource(textResource, appState->GetVersion().DefaultVolumeFile, textResource.ResourceNumber, "");
                        log.ReportResult(
                            CompileResult(fmt::format("Text resource {1} changed. Added {0} entries.", results.GetTextComponent().Texts.size(), textResource.ResourceNumber),
                            CompileResult::CompileResultType::CRT_Message)
                            );
                    } // Else don't save.
                }

                // Update any tables that need to be modified (global class table, selector table)

                // Save the script resource
                std::vector<BYTE> &output = results.GetScriptResource();
                const GameFolderHelper &helper = appState->GetResourceMap().Helper();
                appState->GetResourceMap().AppendResource(ResourceBlob(helper, nullptr, ResourceType::Script, output, helper.Version.DefaultVolumeFile, wNum, NoBase36, helper.Version, helper.GetDefaultSaveSourceFlags()));

                std::vector<BYTE> &outputHep = results.GetHeapResource();
                if (!outputHep.empty())
                {
                    appState->GetResourceMap().AppendResource(ResourceBlob(helper, nullptr, ResourceType::Heap, outputHep, helper.Version.DefaultVolumeFile, wNum, NoBase36, helper.Version, helper.GetDefaultSaveSourceFlags()));
                }

                appState->GetDependencyTracker().ClearScript(pScript->GetScriptId());

                // Save the corresponding sco file.
                CSCOFile &sco = results.GetSCO();
                {
                    SaveSCOFile(helper, sco, script);
                }

                if (!results.GetDebugInfo().empty())
                {
                    // Save debug information.
                    std::string scdFileName = helper.GetScriptDebugFileName(script.GetResourceNumber());
                    ofstream scdFile(scdFileName.c_str(), ios::out | ios::binary);
                    // REVIEW: yucky
                    scdFile.write((const char *)&results.GetDebugInfo()[0], (std::streamsize)results.GetDebugInfo().size());
                    scdFile.close();
                }
                fRet = true;
            }
        }
        log.CalculateErrors();
        buffer.FreeAll();
    }
    return fRet;
}

void DisassembleScript(WORD wScript)
{
    CompiledScript compiledScript(0);
    if (compiledScript.Load(appState->GetResourceMap().Helper(), appState->GetVersion(), wScript))
    {
        // Write some crap.
        GlobalCompiledScriptLookups scriptLookups;
        ObjectFileScriptLookups objectFileLookups(appState->GetResourceMap().Helper(), appState->GetResourceMap().GetCompiledScriptLookups()->GetSelectorTable());
        if (scriptLookups.Load(appState->GetResourceMap().Helper()))
        {
            std::stringstream out;
            ::DisassembleScript(compiledScript, out, &scriptLookups, &objectFileLookups, appState->GetResourceMap().GetVocab000());
            ShowTextFile(out.str().c_str(), "script.sca.txt");
        }
    }
}

void CScriptDocument::OnDisassemble()
{
    WORD wScript;
    // Make the compiled script...
    if (SUCCEEDED(appState->GetResourceMap().GetScriptNumber(_scriptId, wScript)))
    {
        DisassembleScript(wScript);
    }
}

void DecompileScript(const GameFolderHelper &helper, WORD wScript, IDecompilerResults &results)
{
    CompiledScript compiledScript(0);
    if (compiledScript.Load(helper, appState->GetVersion(), wScript))
    {
        unique_ptr<sci::Script> pScript = DecompileScript(nullptr, *appState->GetResourceMap().GetCompiledScriptLookups(), helper, wScript, compiledScript, results);
        std::stringstream ss;
        sci::SourceCodeWriter out(ss, helper.GetDefaultGameLanguage(), pScript.get());
        pScript->OutputSourceCode(out);
        ShowTextFile(ss.str().c_str(), "script.scp.txt");
    }
}

void FixDuplicateObjectNames(CompiledScript &compiledScript, const SelectorTable &selectorTable)
{
    // Occasionally a script will have objects with duplicate names. Rather than a bug, this indicates that there were two separate objects that had
    // their name property explicitly provided. An example is _MapInSection.sc in QFG2.
    // There are a few ways to address it, but we'll try the following here:
    //  Check for any name dupes in the objects.
    //  If so, change their name to some unique name
    //  Then add a name property with a value pointing to the original string.
    unordered_map<string, int> countOfNames;
    unordered_map<string, char> suffixes;
    for (const auto &object : compiledScript.GetObjects())
    {
        countOfNames[object->GetName()]++;
        suffixes[object->GetName()] = 'a';
    }

    for (auto &object : compiledScript.GetObjects())
    {
        int count = countOfNames[object->GetName()];
        if (count > 1)
        {
            // This is a multiple named one.
            std::string newName = fmt::format("{0}_{1}", object->GetName(), suffixes[object->GetName()]++);
            object->AdjustName(newName); // This will track the old name so we can explicitly list it
        }
    }
}

std::unique_ptr<sci::Script> DecompileScript(const IDecompilerConfig *config, GlobalCompiledScriptLookups &scriptLookups, const GameFolderHelper &helper, WORD wScript, CompiledScript &compiledScript, IDecompilerResults &results, bool debugControlFlow, bool debugInstConsumption, PCSTR pszDebugFilter, bool decompileAsm, bool substituteTextTuples)
{
    unique_ptr<sci::Script> pScript;
    ObjectFileScriptLookups objectFileLookups(helper, scriptLookups.GetSelectorTable());
    // Ok if pText fails (and is NULL)
    unique_ptr<ResourceEntity> textResource = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::Text, wScript);
    TextComponent *pText = nullptr;
    if (textResource)
    {
        pText = textResource->TryGetComponent<TextComponent>();
    }

    FixDuplicateObjectNames(compiledScript, config->GetSelectorTable());

    DecompileLookups decompileLookups(config, helper, wScript, &scriptLookups, &objectFileLookups, &compiledScript, pText, &compiledScript, results);
    decompileLookups.DebugControlFlow = debugControlFlow;
    decompileLookups.DebugInstructionConsumption = debugInstConsumption;
    decompileLookups.pszDebugFilter = pszDebugFilter;
    decompileLookups.DecompileAsm = decompileAsm;
    decompileLookups.SubstituteTextTuples = substituteTextTuples;
    pScript.reset(Decompile(helper, compiledScript, decompileLookups, appState->GetResourceMap().GetVocab000()));

    if (helper.Language == LangSyntaxSCI)
    {
        ConvertToSCISyntaxHelper(*pScript, &scriptLookups);
    }

    return pScript;
}

void CScriptDocument::OnViewObjectFile()
{
    if (!_scriptId.IsHeader())
    {
        // Need the script name.
        string objectFileName = appState->GetResourceMap().Helper().GetScriptObjectFileName(_scriptId.GetTitle());
        if (!objectFileName.empty())
        {
            HANDLE hFile = CreateFile(objectFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
            if (hFile)
            {
                sci::streamOwner streamOwner(hFile);
                CSCOFile scoFile;
                SelectorTable selectorTable;
                selectorTable.Load(appState->GetResourceMap().Helper());
                if (scoFile.Load(streamOwner.getReader(), selectorTable))
                {
                    stringstream out;
                    scoFile.DebugOut(out);
                    string strFileName = _scriptId.GetTitle();
                    strFileName += ".sco.txt";
                    ShowTextFile(out.str().c_str(), strFileName);
                }
                CloseHandle(hFile);
            }
        }
    }
}
void CScriptDocument::OnViewScriptResource()
{
    if (!_scriptId.IsHeader())
    {
        WORD wScript;
        // Make the compiled script...
        if (SUCCEEDED(appState->GetResourceMap().GetScriptNumber(_scriptId, wScript)))
        {
            CompiledScript compiledScript(0);
            if (compiledScript.Load(appState->GetResourceMap().Helper(), appState->GetVersion(), wScript))
            {
                // Write some crap.
                std::stringstream out;
                DebugOut(compiledScript, out);
                ShowTextFile(out.str().c_str(), "scriptresource.txt");
            }
        }
    }
}

unique_ptr<sci::Script> _ParseScript(ScriptId id)
{
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(id.GetFullPath().c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);

        std::unique_ptr<sci::Script> pScript = std::make_unique<sci::Script>(id);
        CompileLog log;
        bool result = SyntaxParser_Parse(*pScript, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log);
        buffer.FreeAll();
        if (result)
        {
            return pScript;
        }
    }
    return nullptr;
}

void CScriptDocument::OnViewSyntaxTree()
{
    // What we do
    // 1) Parse this script and generate a syntax tree.
    // 2) If successful, write to the file.
    // 3) Reload
    CScriptStreamLimiter limiter(&_buffer);
    CCrystalScriptStream stream(&limiter);
    //SCIClassBrowser &browser = appState->GetClassBrowser(); 
    //browser.Lock();
    // 1)
    sci::Script script(_scriptId);
    CompileLog log;
    bool fCompile = SyntaxParser_Parse(script, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log);;
    if (fCompile)
    {
        PrepForLanguage(appState->GetResourceMap().Helper().GetDefaultGameLanguage(), script);

        std::stringstream out;
        sci::SourceCodeWriter theCode(out, appState->GetResourceMap().Helper().GetDefaultGameLanguage(), &script);
        theCode.indentChar = '\t';
        theCode.indentAmount = 1;
        script.OutputSourceCode(theCode);

        ShowTextFile(out.str().c_str(), "syntaxtree.txt");
    }


#if NOT_NEEDED
    // Repurposing it for something else: calculating the dependency tree
    set<string> complete;
    stack<ScriptId> toProcess;
    toProcess.push(_scriptId);
    while (!toProcess.empty())
    {
        ScriptId id = toProcess.top();
        toProcess.pop();
        string title = id.GetTitle();
        ToUpper(title);
        complete.insert(title);
        unique_ptr<sci::Script> parsedScript = _ParseScript(id);
        if (parsedScript)
        {
            for (auto use : parsedScript->GetUses())
            {
                string USE = use;
                ToUpper(USE);
                if (complete.find(USE) == complete.end())
                {
                    toProcess.push(appState->GetResourceMap().Helper().GetScriptId(use));
                }
            }
        }
    }
#endif
}

void CScriptDocument::OnDebugRoom()
{
    int resourceNumber = _scriptId.GetResourceNumber();
    if (resourceNumber != -1)
    {
        appState->RunGame(true, resourceNumber);
    }
}

// Converts to the default game language.
void CScriptDocument::OnConvertScript()
{
    CompileLog log;

    if (ConvertScript(appState->GetVersion(), appState->GetResourceMap().Helper().GetDefaultGameLanguage(), _scriptId, log, true))
    {
        _buffer.FreeAll();
        _buffer.LoadFromFile(_scriptId.GetFullPath().c_str());

        // Play a sound and rejoice!
        // Nah, don't.
        // PlaySound((LPCSTR)SND_ALIAS_SYSTEMHAND, NULL, SND_ALIAS_ID | SND_ASYNC);

        // Tell the view.
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapObject(ScriptChangeHint::Converted | ScriptChangeHint::Pos, this));

        // Update our title
        _OnUpdateTitle();
    }
    else
    {
        log.SummarizeAndReportErrors();
        AfxMessageBox("The original script has compile errors. They must be fixed before conversion can take place.", MB_ERRORFLAGS);
    }
    appState->OutputResults(OutputPaneType::Compile, log.Results());
}

void CScriptDocument::OnUpdateConvertScript(CCmdUI *pCmdUI)
{
    // Enable conversion if this script's language is different than the game's language.
    pCmdUI->Enable(appState->GetResourceMap().Helper().GetDefaultGameLanguage() != _scriptId.Language());
}

void CScriptDocument::OnUpdateLineCount(CCmdUI *pCmdUI)
{
    pCmdUI->SetText(fmt::format("{0} lines.", _buffer.GetLineCount()).c_str());
}

void CScriptDocument::_DoErrorSummary(ICompileLog &log)
{
    log.SummarizeAndReportErrors();
}

void CScriptDocument::_ClearErrorCount()
{
}

// CScriptDocument diagnostics

#ifdef _DEBUG
void CScriptDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CScriptDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CScriptDocument serialization

void CScriptDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

// We don't want the user editing these files. They should use the message editor instead.
bool IsReadOnly(LPCTSTR pathName)
{
    return (0 == lstrcmp(PathFindExtension(pathName), ".shm")) ||
        (0 == lstrcmp(PathFindExtension(pathName), ".shp")) ||
        (0 == lstrcmpi(PathFindFileName(pathName), "Verbs.sh")) ||
        (0 == lstrcmpi(PathFindFileName(pathName), "Talkers.sh")) ||
        (0 == lstrcmpi(PathFindFileName(pathName), "keys.sh")) ||
        (0 == lstrcmpi(PathFindFileName(pathName), "sci.sh"));
}

// Use this instead of OnOpenDocument in order to properly set scriptNumber
BOOL CScriptDocument::OnOpenDocument(LPCTSTR lpszPathName, uint16_t scriptNumber)
{
    if (!__super::OnOpenDocument(lpszPathName))
        return FALSE;
    _scriptId = ScriptId(lpszPathName);
    _scriptId.SetResourceNumber(scriptNumber);
    _buffer.FreeAll();
    BOOL result = _buffer.LoadFromFile(lpszPathName);
    _buffer.SetReadOnly(IsReadOnly(lpszPathName));
    return result;
}

BOOL CScriptDocument::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!__super::OnOpenDocument(lpszPathName))
		return FALSE;
	_scriptId = ScriptId(lpszPathName);
    _buffer.FreeAll();
	BOOL result = _buffer.LoadFromFile(lpszPathName);
    _buffer.SetReadOnly(IsReadOnly(lpszPathName));
    return result;
}
BOOL CScriptDocument::OnSaveDocument(LPCTSTR lpszPathName) 
{
	_buffer.SaveToFile(lpszPathName);
    _scriptId = ScriptId(lpszPathName);
	return TRUE;	//	Note - we didn't call inherited member!
}
BOOL CScriptDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
    return _buffer.InitNew();
}
void CScriptDocument::SaveIfModified()
{
    if (IsModified())
    {
        OnFileSave();
    }
}
void CScriptDocument::SetNameAndContent(ScriptId scriptId, int iResourceNumber, std::string &text)
{
    _scriptId = scriptId;
    int nEndLine = 0;
    int nEndChar = 0;
    _buffer.InsertText(NULL, 0, 0, text.c_str(), nEndLine, nEndChar);
    _buffer.SetModified();
    appState->GetResourceMap().AssignName(ResourceType::Script, iResourceNumber, NoBase36, scriptId.GetTitle().c_str());
    // Since we're assigning a name, we'd better save it too.
    OnFileSave();
}

void CScriptDocument::OnFileSave()
{
    std::string path = _scriptId.GetFullPath();
    // Get the current active script, and save it.
    _buffer.SaveToFile(path.c_str());
    // Update the classbrowser...
	SCIClassBrowser &browser = appState->GetClassBrowser();
    browser.TriggerReloadScript(path.c_str());

    if (_dependencyTracker)
    {
        _dependencyTracker->NotifyScriptFileChanged(_scriptId);
    }

    // Notify clients *after* we have updated the class browser.
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapObject(ScriptChangeHint::Saved, this));
}

const TCHAR g_rgszScriptFilter[] = TEXT("Script (*.sc)|*.sc;|Header (*.sh)|*.sh|All Files (*.*)|*.*");
const TCHAR g_rgszHeaderFilter[] = TEXT("Header (*.sh)|*.sh|Script (*.sc)|*.sc;|All Files (*.*)|*.*");


void CScriptDocument::OnFileSaveAs()
{
    // Set up the save dialog.
    CString strTitle = _scriptId.GetTitle().c_str();
    CFileDialog fileDialog(FALSE,
                           _scriptId.IsHeader() ? ".sh" : ".sc",
                           strTitle,
                           OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
                           _scriptId.IsHeader() ? g_rgszHeaderFilter : g_rgszScriptFilter);
    CString strFolder = _scriptId.GetFolder().c_str();
    fileDialog.m_pOFN->lpstrInitialDir = strFolder;
    if (IDOK == fileDialog.DoModal())
    {
        // We have a new filename.
        CString strFileName = fileDialog.GetPathName();
        // But save it under the new name.
        if (_buffer.SaveToFile(strFileName))
        {
            // We have a new identity
            _scriptId = ScriptId(strFileName);
        }
    }
}

void CScriptDocument::UpdateModified()
{
    SetModifiedFlag(_buffer.IsModified());
    _OnUpdateTitle();
}

void CScriptDocument::_OnUpdateTitle()
{
    SetTitle(_scriptId.GetFileNameOrig().c_str());
}

