#pragma once

#include "resource.h"       // main symbols
#include "ResourceMap.h"
#include "ResourceRecency.h"
#include "IntellisenseListBox.h"
#include "ColoredToolTip.h"
#include "CompileInterfaces.h"

class AutoCompleteThread;
class CScriptView;

// FWD decl
class CScriptDoc;
class ResourceEntity;
class CResourceListDoc;
class AppState;

//
// This allows us to hook up the slider bar, celdialogbar, and other "non views"
//
class CMultiDocTemplateWithNonViews : public CMultiDocTemplate
{
public:
    CMultiDocTemplateWithNonViews(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
    virtual void InitialUpdateFrame(CFrameWnd *pFrame, CDocument *pDoc, BOOL bMakeVisible);
};

class AppState
{
public:
    AppState(CWinApp *pApp = nullptr);
    ~AppState();

private:
    CWinApp *_pApp;

    // Overrides
public:
    BOOL InitInstance();
    int ExitInstance();

    CMultiDocTemplate *GetPicTemplate() { return _pPicTemplate; }
    CMultiDocTemplate *GetVocabTemplate() { return _pVocabTemplate; }
    CMultiDocTemplate *GetViewTemplate() { return _pViewTemplate; }
    CMultiDocTemplate *GetTextTemplate() { return _pTextTemplate; }
    CMultiDocTemplate *GetSoundTemplate() { return _pSoundTemplate; }
    CMultiDocTemplate *GetFontTemplate() { return _pFontTemplate; }
    CMultiDocTemplate *GetCursorTemplate() { return _pCursorTemplate; }
    CMultiDocTemplate *GetScriptTemplate() { return _pScriptTemplate; }
    CMultiDocTemplate *GetMessageTemplate() { return _pMessageTemplate; }
    CMultiDocTemplate *GetPaletteTemplate() { return _pPaletteTemplate; }
    
    
    void OpenScript(std::string strName, const ResourceBlob *pData = NULL, WORD wScriptNum = InvalidResourceNumber);
    void OpenScript(WORD w);
    void OpenScriptHeader(std::string strName);
    void OpenScriptAtLine(ScriptId script, int iLine);
    void OpenMostRecentResource(ResourceType type, uint16_t wNum);
    void OpenMostRecentResourceAt(ResourceType type, uint16_t number, int index);
    void SetScriptFrame(CFrameWnd *pScriptFrame) { _pScriptFrame = pScriptFrame; }
    DWORD CreateUniqueRuntimeID();
    CResourceMap &GetResourceMap() { return _resourceMap; }
    SCIVersion &GetVersion() { return _resourceMap.GetSCIVersion(); }
    UINT GetCommandClipboardFormat() { return _uClipboardFormat; }
    CDocument* OpenDocumentFile(PCTSTR lpszFileName);
    ResourceEntity *GetSelectedViewResource();
    void SetExportFolder(LPITEMIDLIST pidl); // Takes ownership
    const LPITEMIDLIST GetExportFolder() { return _pidlFolder; }
    CIntellisenseListBox &GetIntellisense() { return m_wndIntel; }
    CColoredToolTip &GetToolTipCtrl() { return m_wndToolTip; }
    bool IsBrowseInfoEnabled() { return _fBrowseInfo != 0; }
    BOOL IsCodeCompletionEnabled() { return _fBrowseInfo && _fCodeCompletion; }
    BOOL AreHoverTipsEnabled() { return _fBrowseInfo && _fHoverTips; }
    BOOL IsParamInfoEnabled() { return _fBrowseInfo && _fParamInfo; }
    void GenerateBrowseInfo();
    void ResetClassBrowser();
    void ClearResourceManagerDoc() { _pResourceDoc = NULL; }

    void GiveMeAutoComplete(CScriptView *pSV);

    // Output pane
    void OutputResults(std::vector<CompileResult> &compileResults);
    // For finer control
    void ShowOutputPane();
    void OutputClearResults();
    void OutputAddBatch(std::vector<CompileResult> &compileResults);
    void OutputFinishAdd();

    // Game explorer
    void ShowResourceType(ResourceType iType);
    ResourceType GetShownResourceType();
    void SetExplorerFrame(CFrameWnd *pFrame) { _pExplorerFrame = pFrame; }

    // Game properties
    std::string GetGameName();
    void SetGameName(PCTSTR pszName);
    std::string GetGameExecutable();
    void SetGameExecutable(PCTSTR pszName);
    std::string GetGameExecutableParameters();
    void SetGameExecutableParameters(PCTSTR pszName);

    void LogInfo(const TCHAR *pszFormat, ...);

    // Global settings:
    int _cxFakeEgo;
    int _cyFakeEgo;
    BOOL _fUseBoxEgo;
    int _fGridLines;
    int _fScaleTracingImages;
    BOOL _fDontShowTraceScaleWarning;
    BOOL _fUseAutoSuggest;
    bool _fAllowBraceSyntax;
    BOOL _fAutoLoadGame;
    BOOL _fDupeNewCels;
    BOOL _fSCI01;
    BOOL _fBrowseInfo;
    BOOL _fParamInfo;
    BOOL _fCodeCompletion;
    BOOL _fHoverTips;
    BOOL _fPlayCompileErrorSound;

    BOOL _fNoGdiPlus;   // GDI+ is not available

    // This setting is not persisted across instances of the app:
    BOOL _fDontCheckPic;

    // Last known position of a fake ego:
    CPoint _ptFakeEgo;
    // Last selected view
    int _iView;
    std::unique_ptr<ResourceEntity> _pViewResource;
    bool _fObserveControlLines; // Does fake ego observe control lines?

    // Prof-UIS command profile
    PCSTR _pszCommandProfile;

    ResourceRecency _resourceRecency;

public: // TODO for now
    HRESULT _GetGameStringProperty(PCTSTR pszProp, PTSTR pszValue, size_t cchValue);
    HRESULT _SetGameStringProperty(PCTSTR pszProp, PCTSTR pszValue);
    HRESULT _GetGameIni(PTSTR pszValue, size_t cchValue);

    Gdiplus::GdiplusStartupInput _gdiplusStartupInput;
    ULONG_PTR _gdiplusToken;

    // Do not free these member.  It is just here for convenience.
    CMultiDocTemplate *_pPicTemplate;
    CMultiDocTemplate *_pVocabTemplate;
    CMultiDocTemplate *_pViewTemplate;
    CMultiDocTemplate *_pResourceTemplate;
    CMultiDocTemplate *_pScriptTemplate;
    CMultiDocTemplate *_pTextTemplate;
    CMultiDocTemplate *_pSoundTemplate;
    CMultiDocTemplate *_pFontTemplate;
    CMultiDocTemplate *_pCursorTemplate;
    CMultiDocTemplate *_pRoomExplorerTemplate;
    CMultiDocTemplate *_pMessageTemplate;
    CMultiDocTemplate *_pPaletteTemplate;

    CFrameWnd *_pScriptFrame;

    // Game explorer
    CResourceListDoc *_pResourceDoc;
    ResourceType _shownType;
    CFrameWnd *_pExplorerFrame;

    CResourceMap _resourceMap;

    UINT _uClipboardFormat;

    CFile _logFile;

    // Last folder for exporting resources
    LPITEMIDLIST _pidlFolder;

    CIntellisenseListBox m_wndIntel;
    CColoredToolTip m_wndToolTip;
    AutoCompleteThread *_pACThread;
};

extern AppState *appState;