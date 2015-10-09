#pragma once

#include "resource.h"       // main symbols
#include "ResourceMap.h"
#include "ResourceRecency.h"
#include "IntellisenseListBox.h"
#include "ColoredToolTip.h"
#include "CompileInterfaces.h"

class AutoCompleteThread2;
class CScriptView;

struct HoverTipPayload;
struct HoverTipResponse;

template<typename _TPayload, typename _TResponse>
class BackgroundScheduler;

// FWD decl
class CScriptDoc;
class ResourceEntity;
class CResourceListDoc;
class AppState;
struct AudioProcessingSettings;

template<typename _TPayload, typename _TResponse>
class BackgroundScheduler;

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
    void ReopenScriptDocument(uint16_t wNum);
    void OpenMostRecentResourceAt(ResourceType type, uint16_t number, int index);
    void SetScriptFrame(CFrameWnd *pScriptFrame) { _pScriptFrame = pScriptFrame; }
    DWORD CreateUniqueRuntimeID();
    CResourceMap &GetResourceMap() { return _resourceMap; }
    SCIVersion &GetVersion() { return _resourceMap.GetSCIVersion(); }
    UINT GetCommandClipboardFormat() { return _uClipboardFormat; }
    CDocument* OpenDocumentFile(PCTSTR lpszFileName);
    int GetSelectedViewResourceNumber();
    std::vector<int> &GetRecentViews();
    void SetExportFolder(LPITEMIDLIST pidl); // Takes ownership
    const LPITEMIDLIST GetExportFolder() { return _pidlFolder; }
    CIntellisenseListBox &GetIntellisense() { return m_wndIntel; }
    CColoredToolTip &GetToolTipCtrl() { return m_wndToolTip; }
    bool IsBrowseInfoEnabled() { return _fBrowseInfo != 0; }
    BOOL IsCodeCompletionEnabled() { return _fBrowseInfo && _fCodeCompletion; }
    BOOL AreHoverTipsEnabled() { return _fBrowseInfo && _fHoverTips; }
    BOOL IsScriptNavEnabled() { return _fBrowseInfo && _fScriptNav; }
    void GenerateBrowseInfo();
    void ResetClassBrowser();
    void ClearResourceManagerDoc() { _pResourceDoc = NULL; }
    void NotifyChangeAspectRatio();
    void SetRecentlyInteractedView(int resourceNumber);

    void TerminateDebuggedProcess();
    bool IsProcessBeingDebugged();

    int AspectRatioY(int value) const;
    int InverseAspectRatioY(int value) const;

    void GiveMeAutoComplete(CScriptView *pSV);
    void HideTipWindows();

    // Output pane
    void OutputResults(OutputPaneType type, std::vector<CompileResult> &compileResults);
    // For finer control
    void ShowOutputPane(OutputPaneType type);
    void OutputClearResults(OutputPaneType type);
    void OutputAddBatch(OutputPaneType type, std::vector<CompileResult> &compileResults);
    void OutputFinishAdd(OutputPaneType type);

    UINT GetMidiDeviceId();

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

    void RunGame(bool debug, int optionalResourceNumber);

    void OnGameFolderUpdate();

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
    BOOL _fShowPolyDotted;
    BOOL _fBrowseInfo;
    BOOL _fScriptNav;
    BOOL _fCodeCompletion;
    BOOL _fHoverTips;
    BOOL _fPlayCompileErrorSound;
    BOOL _fUseOriginalAspectRatio;
    std::string _midiDeviceName;
    std::unique_ptr<AudioProcessingSettings> _audioProcessing;

    BOOL _fNoGdiPlus;   // GDI+ is not available

    // This setting is not persisted across instances of the app:
    BOOL _fDontCheckPic;

    // Last known position of a fake ego:
    CPoint _ptFakeEgo;
    // Last selected views
    std::vector<int> _recentViews;
    bool _fObserveControlLines; // Does fake ego observe control lines?
    bool _fObservePolygons;     // Does it observe polygons?

    // Prof-UIS command profile
    PCSTR _pszCommandProfile;

    ResourceRecency _resourceRecency;

    UINT CelDataClipboardFormat;

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
    AutoCompleteThread2 *_pACThread;

    std::unique_ptr<BackgroundScheduler<HoverTipPayload, HoverTipResponse>> _pHoverTipScheduler;

    ScopedHandle _hProcessDebugged;
};

extern AppState *appState;