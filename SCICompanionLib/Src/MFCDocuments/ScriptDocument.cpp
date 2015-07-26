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
#include "SCIDocs.h"
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

using namespace std;


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

CScriptDocument::CScriptDocument() : _buffer(*this)
{
}

CScriptDocument::~CScriptDocument()
{
    _buffer.FreeAll();
}


BEGIN_MESSAGE_MAP(CScriptDocument, CDocument)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
    ON_COMMAND(ID_COMPILE, OnCompile)
#ifdef DOCSUPPORT
    ON_COMMAND(ID_COMPILEDOCS, OnCompileDocs)
#endif
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

#ifdef DOCSUPPORT
void CScriptDocument::OnCompileDocs()
{
    // Make a new buffer.
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(_scriptId.GetFullPath().c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);
		std::unique_ptr<sci::Script> pScript(new sci::Script(_scriptId));
        CompileLog log;
        if (g_Parser.Parse(*pScript, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log, true))
        {
            CompileDocs(*pScript);
        }
        else
        {
            // An error while compiling.
            log.SummarizeAndReportErrors();
            appState->OutputResults(OutputPaneType::Compile, log.Results());
        }
        buffer.FreeAll();
    }
}
#endif

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
        bool fSuccess = NewCompileScript(log, tables, headers, _scriptId);
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

std::unique_ptr<sci::Script> SimpleCompile(CompileLog &log, ScriptId &scriptId)
{
    std::unique_ptr<sci::Script> script = make_unique<sci::Script>();
    // Make a new buffer.
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(scriptId.GetFullPath().c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);
        if (g_Parser.Parse(*script, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log))
        {

        }
    }
    log.CalculateErrors();
    buffer.FreeAll();
    return script;
}

bool NewCompileScript(CompileLog &log, CompileTables &tables, PrecompiledHeaders &headers, ScriptId &script)
{
    bool fRet = false;
	ClassBrowserLock lock(*appState->GetResourceMap().GetClassBrowser());
    lock.Lock();

    // Make a new buffer.
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(script.GetFullPath().c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);

		std::unique_ptr<sci::Script> pScript = std::make_unique<sci::Script>(script);

        if (g_Parser.Parse(*pScript, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log))
        {
            if (script.GetResourceNumber() != pScript->GetScriptNumber())
            {
                log.ReportResult(
                    CompileResult(fmt::format("Script {0} ({1}) declared itself as resource {2}", script.GetResourceNumber(), script.GetTitle(), pScript->GetScriptNumber()),
                    CompileResult::CompileResultType::CRT_Warning));
            }

            // Compile and save script resource.
            // Compile our own script!
            CompileResults results(log);
            if (GenerateScriptResource(appState->GetVersion(), *pScript, headers, tables, results))
            {
                WORD wNum = results.GetScriptNumber();

                // Update any tables that need to be modified (global class table, selector table)
                // Save the text resource.
                if (!results.GetTextComponent().Texts.empty())
                {
                    assert(script.Language() == LangSyntaxCpp);
#ifdef IS_IMPLEMENTED
                    // Needs an implementation. We're just trying to save a text resource that was automatically
                    // generated.
                    results.GetTextResource().SetResourceNumber(wNum);
                    sci::istream serial;
                    results.GetTextResource()->WriteTo(serial);
                    appState->GetResourceMap().AppendResource(blob);
#endif
                }

                // Save the script resource
                std::vector<BYTE> &output = results.GetScriptResource();
                appState->GetResourceMap().AppendResource(ResourceBlob(nullptr, ResourceType::Script, output, appState->GetVersion().DefaultVolumeFile, wNum, appState->GetVersion(), ResourceSourceFlags::ResourceMap));

                std::vector<BYTE> &outputHep = results.GetHeapResource();
                if (!outputHep.empty())
                {
                    appState->GetResourceMap().AppendResource(ResourceBlob(nullptr, ResourceType::Heap, outputHep, appState->GetVersion().DefaultVolumeFile, wNum, appState->GetVersion(), ResourceSourceFlags::ResourceMap));
                }

                // Save the corresponding sco file.
                CSCOFile &sco = results.GetSCO();
                {
                    SaveSCOFile(appState->GetResourceMap().Helper(), sco, script);
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
    if (compiledScript.Load(appState->GetResourceMap().Helper(), appState->GetVersion(), wScript, false))
    {
        // Write some crap.
        GlobalCompiledScriptLookups scriptLookups;
        ObjectFileScriptLookups objectFileLookups(appState->GetResourceMap().Helper());
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
    if (compiledScript.Load(helper, appState->GetVersion(), wScript, false))
    {
        unique_ptr<sci::Script> pScript = DecompileScript(nullptr, *appState->GetResourceMap().GetCompiledScriptLookups(), helper, wScript, compiledScript, results);
        std::stringstream ss;
        sci::SourceCodeWriter out(ss, helper.GetGameLanguage(), pScript.get());
        pScript->OutputSourceCode(out);
        ShowTextFile(ss.str().c_str(), "script.scp.txt");
    }
}

std::unique_ptr<sci::Script> DecompileScript(const IDecompilerConfig *config, GlobalCompiledScriptLookups &scriptLookups, const GameFolderHelper &helper, WORD wScript, CompiledScript &compiledScript, IDecompilerResults &results, bool debugControlFlow, bool debugInstConsumption, PCSTR pszDebugFilter, bool decompileAsm)
{
    unique_ptr<sci::Script> pScript;
    ObjectFileScriptLookups objectFileLookups(helper);
    // Ok if pText fails (and is NULL)
    unique_ptr<ResourceEntity> textResource = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::Text, wScript);
    TextComponent *pText = nullptr;
    if (textResource)
    {
        pText = textResource->TryGetComponent<TextComponent>();
    }

    DecompileLookups decompileLookups(config, helper, wScript, &scriptLookups, &objectFileLookups, &compiledScript, pText, &compiledScript, results);
    decompileLookups.DebugControlFlow = debugControlFlow;
    decompileLookups.DebugInstructionConsumption = debugInstConsumption;
    decompileLookups.pszDebugFilter = pszDebugFilter;
    decompileLookups.DecompileAsm = decompileAsm;
    pScript.reset(Decompile(helper, compiledScript, decompileLookups, appState->GetResourceMap().GetVocab000()));
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
                if (scoFile.Load(streamOwner.getReader()))
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
            if (compiledScript.Load(appState->GetResourceMap().Helper(), appState->GetVersion(), wScript, false))
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
        bool result = g_Parser.Parse(*pScript, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log);
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
#if DONT_KNOW_WHAT_THIS_DID
    CScriptStreamLimiter limiter(&_buffer);
    CCrystalScriptStream stream(&limiter);
	SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
    browser.Lock();

    sci::Script script(_scriptId);
    if (g_Parser.Parse(script, stream, NULL))
    {
        std::stringstream out;
		sci::SourceCodeWriter debugOut(out, appState->GetResourceMap().GetGameLanguage());
        script.OutputSourceCode(debugOut);
        ShowTextFile(out.str().c_str(), "syntaxtree.txt");
    }
#endif

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

}

void CScriptDocument::OnDebugRoom()
{
    int resourceNumber = _scriptId.GetResourceNumber();
    if (resourceNumber != -1)
    {
        appState->RunGame(true, resourceNumber);
    }
}

void CScriptDocument::OnConvertScript()
{
    if (appState->GetResourceMap().GetGameLanguage() != _scriptId.Language())
    {
        if (LangSyntaxCpp == appState->GetResourceMap().GetGameLanguage())
        {
            // What we do
            // 1) Parse this script and generate a syntax tree.
            // 2) Create a new file of the same name, but with a different file extension.
            // 3) Officially mark the script as being in a new language.
            // 4) Reload
            CScriptStreamLimiter limiter(&_buffer);
            CCrystalScriptStream stream(&limiter);
            SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser(); 
            browser.Lock();
            // 1)
            sci::Script script(_scriptId);
            CompileLog log;
            bool fCompile = g_Parser.Parse(script, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log);;
            if (fCompile)
            {
                // 1.5 Make some substitutions for properties that have minus signs in them.
                // e.g. convert b-moveCnt to b_moveCnt, since c-style syntax can't handle - in a variable name.
                class PropertyAliasConvert : public sci::IExploreNode
                {
                public:
                    void ExploreNode(sci::IExploreNodeContext *pContext, sci::SyntaxNode &node, sci::ExploreNodeState state)
                    {
						if (state == sci::ExploreNodeState::Pre)
						{
							switch (node.GetNodeType())
							{
							case sci::NodeTypeValue:
							case sci::NodeTypeComplexValue:
							{
								sci::PropertyValueBase &base = static_cast<sci::PropertyValueBase&>(node);
								if ((base.GetType() == sci::ValueType::Token) ||
									(base.GetType() == sci::ValueType::Selector) ||
									(base.GetType() == sci::ValueType::Pointer))
								{
									base.SetValue(GetTokenAlias(base.GetStringValue()), base.GetType());
								}
							}
							break;
							case sci::NodeTypeClassProperty:
							{
								sci::ClassProperty &classProp = static_cast<sci::ClassProperty&>(node);
								classProp.SetName(GetTokenAlias(classProp.GetName()));
							}
							break;

							case sci::NodeTypeLValue:
							{
								sci::LValue &lvalue = static_cast<sci::LValue&>(node);
								lvalue.SetName(GetTokenAlias(lvalue.GetName()));
							}
							break;
							case sci::NodeTypeSendParam:
							{
								sci::SendParam &sendParam = static_cast<sci::SendParam&>(node);
								sendParam.SetName(GetTokenAlias(sendParam.GetName()));
							}
							break;
							}
						}
                    }
                };
                PropertyAliasConvert propertyAliasConvert;
                script.Traverse(NULL, propertyAliasConvert);

                std::stringstream out;
				sci::SourceCodeWriter debugOut(out, LangSyntaxCpp, &script);
                script.OutputSourceCode(debugOut);

                // 2)
                // Can't use CResourceMap::GetScriptFileName just yet, since we haven't offically converted the script
                std::string fullPath = _scriptId.GetFullPath();
                if (_scriptId.IsHeader())
                {
                    fullPath.replace(fullPath.find(".sh"), strlen(".sh"), ".shp");
                }
                else
                {
                    fullPath.replace(fullPath.find(".sc"), strlen(".sc"), ".scp");
                }
                ScriptId newScriptId(fullPath);
                newScriptId.SetResourceNumber(_scriptId.GetResourceNumber()); // Transfer resource num
                // Headers don't have resource numbers:
                ASSERT(!newScriptId.IsHeader() || (newScriptId.GetResourceNumber() == InvalidResourceNumber));
                ofstream newFile(fullPath.c_str());
                newFile << out.str();
                if (newFile.good())
                {
                    // All went well
                    newFile.close();

                    // 3)
                    if (!newScriptId.IsHeader())
                    {
                        appState->GetResourceMap().SetScriptLanguage(newScriptId, LangSyntaxCpp);
                    }

                    std::stringstream ss;
                    ss << _scriptId.GetFileName() << " was successfully converted to " << newScriptId.GetFileName();
                    log.ReportResult(CompileResult(ss.str()));
                    appState->OutputResults(OutputPaneType::Compile, log.Results());

                    // 4) Switch to the new script
                    _scriptId = newScriptId;
                    _buffer.FreeAll();
                    _buffer.LoadFromFile(_scriptId.GetFullPath().c_str());

                    // Play a sound and rejoice!
                    PlaySound((LPCSTR)SND_ALIAS_SYSTEMHAND, NULL, SND_ALIAS_ID | SND_ASYNC);
                    // Tell the view.
                    UpdateAllViewsAndNonViews(nullptr, 0, &WrapObject(ScriptChangeHint::Converted | ScriptChangeHint::Pos, this));

                    // Update our title
                    _OnUpdateTitle();
                }
            }
            else
            {
                log.SummarizeAndReportErrors();
                appState->OutputResults(OutputPaneType::Compile, log.Results());
                AfxMessageBox("The original script has compile errors. They must be fixed before conversion can take place.", MB_ERRORFLAGS);
            }
        }
    }
}


void CScriptDocument::OnUpdateConvertScript(CCmdUI *pCmdUI)
{
    // Enable conversion if this script's language is different than the game's language.
    pCmdUI->Enable(appState->GetResourceMap().GetGameLanguage() != _scriptId.Language());
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
void CScriptDocument::SetNameAndContent(ScriptId scriptId, int iResourceNumber, std::string &text)
{
    _scriptId = scriptId;
    int nEndLine = 0;
    int nEndChar = 0;
    _buffer.InsertText(NULL, 0, 0, text.c_str(), nEndLine, nEndChar);
    _buffer.SetModified();
    appState->GetResourceMap().AssignName(ResourceType::Script, iResourceNumber, scriptId.GetTitle().c_str());
    // Since we're assigning a name, we'd better save it too.
    OnFileSave();
}

void CScriptDocument::OnFileSave()
{
    std::string path = _scriptId.GetFullPath();
    // Get the current active script, and save it.
    _buffer.SaveToFile(path.c_str());
    // Update the classbrowser...
	SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
    {
        ClassBrowserLock lock(browser);
        lock.Lock();
        browser.ReLoadScript(path.c_str());
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
        ScriptId scriptNew(strFileName);
        // But save it under the new name.
        if (_buffer.SaveToFile(strFileName))
        {
            // We have a new identity
            _scriptId = scriptNew;
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
    SetTitle(_scriptId.GetFileName().c_str());
}

