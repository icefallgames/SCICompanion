// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "MDITabsDialogBar.h"
#include "RasterSidePane.h"
#include "MDITabChildWnd.h"
#include "PicCommandSidePane.h"
#include "QuickScripts.h"
#include "ExtControlBarNoX.h"
#include "BarContainerDialog.h"
#include "OutputPane.h"
#include "ScriptComboBox.h"
#include "SamplesDialogBar.h"
#include "SoundToolboxSidePane.h"
#include "MessageSidePane.h"

class CNewScriptDialog;

class CExtMenuControlBarHideShow : public CExtMenuControlBar
{
protected:
    BOOL _UpdateMenuBar(BOOL bDoRecalcLayout = 1);
    bool _ExcludeMenu(PCSTR pszName);
};

class CBrowseInfoStatusPane : public CExtLabel, public IClassBrowserEvents
{
public:
    CBrowseInfoStatusPane();
    ~CBrowseInfoStatusPane();

    // IClassBrowserEvents
    // Note: this method may be called on a different thread.
    void NotifyClassBrowserStatus(BrowseInfoStatus status, int iPercent);

private:
    DECLARE_MESSAGE_MAP()
    void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnDrawLabelText(CDC &dc, const RECT &rcText, __EXT_MFC_SAFE_LPCTSTR strText, DWORD dwDrawTextFlags, bool bEnabled);
    LRESULT _OnStatusReady(WPARAM wParam, LPARAM lParam);
    BrowseInfoStatus _status;
    int _lastPercent;

    // Text to post back to the UI thread.
    std::string _textToPost;
    mutable CRITICAL_SECTION _csTextPosting;
};

class CMainFrame : public CExtNCW<CMDIFrameWnd>
{
    DECLARE_DYNAMIC(CMainFrame)    

public:
    CMainFrame();


// Attributes
public:

// Operations
public:

// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    void AddTab(CFrameWnd *pNewFrame, MDITabType iType);
    void RemoveTab(CFrameWnd *pNewFrame);
    void OnActivateTab(CFrameWnd *pWnd);
    void OnDeactivateTab(CFrameWnd *pWnd);
    CMDITabsDialogBar &Tabs() { return m_wndTab; }
    CBrowseInfoStatusPane &BrowseInfoStatus() { return m_BrowseInfoStatus; }

    OutputPane& GetOutputPane() { return m_wndOutput; }
    void ShowOutputPane();
    BOOL DestroyWindow();

    void RefreshExplorerTools();

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:  // control bar embedded members
    CExtStatusControlBar m_wndStatusBar;
    CBrowseInfoStatusPane m_BrowseInfoStatus;

    CExtToolControlBar m_wndToolBar;
    CExtMenuControlBarHideShow m_wndMenuBar;

    // The tabs across the top
    CMDITabsDialogBar  m_wndTab;

    // The toolbar under the tab
    CExtToolControlBar m_wndPicTools;
    CExtToolControlBar m_wndScriptTools;
    CClassComboBox m_wndScriptToolComboBoxClass;
    //CMethodComboBox m_wndScriptToolComboBoxFunction;
    CExtToolControlBar m_wndTextTools;
    CExtToolControlBar m_wndSoundTools;
    CExtToolControlBar m_wndVocabTools;
    CExtToolControlBar m_wndViewTools;
    CExtToolControlBar m_wndExplorerTools;
    CExtToolControlBar m_wndMessageTools;

    // These combine to form a control bar that contains an empty dialog
    CExtControlBarNoX m_wndResizableBarGeneral;
    CBarContainerDialog m_dlgEmpty;

    // We don't actually show this, but we use its m_nMenuMarkerID member variable.
    CExtThemeSwitcherToolControlBar m_ThemeSwitcher;

    // The empty dialog can contain these subdialogs:
    RasterSidePane m_dlgForPanelDialogFont;
    RasterSidePane m_dlgForPanelDialogView;
    RasterSidePane m_dlgForPanelDialogCursor;
    PicCommandSidePane m_dlgForPanelDialogPic;
    PicCommandSidePane m_dlgForPanelDialogPicVGA;
    SoundToolboxSidePane m_dlgForPanelDialogSound;
    QuickScriptsSidePane m_dlgForPanelDialogScript;
    CSamplesDialogBar m_dlgForPanelDialogGame;
    MessageSidePane m_dlgForPanelDialogMessage;

    // The output window at the bottom (find and compile results)
    CExtControlBar m_wndResizableBarOutput;
    OutputPane m_wndOutput;

    // Our cache of the currently active document.
    CMDITabChildWnd *_pActiveFrame;
    bool _fDidntGetDocYet;

// Generated message map functions
private:
    void ActivateFrame(int nCmdShow);

    void _RefreshToolboxPanel(CFrameWnd *pWnd);
    void _ApplyIndicators(MDITabType tabType);
    void _RefreshToolboxPanelOnDeactivate(CFrameWnd *pWnd);
    void _OnNewScriptDialog(CNewScriptDialog &dialog);
    void _HideTabIfNot(MDITabType iTabTypeCurrent, MDITabType iTabTypeCompare, CExtControlBar &bar);
    void _FindInFilesOfType(ICompileLog &log, PCTSTR pszWildcard, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord);
    void _FindInTexts(ICompileLog &log, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord);
    void _FindInVocab000(ICompileLog &log, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord);
    void _AddFindResults(std::vector<char> &buffer, ICompileLog &log, PCTSTR pszFullPath, PCTSTR pszFileName, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord);
    void _PrepareExplorerCommands();
    void _PrepareRasterCommands();
    void _PrepareScriptCommands();
    void _PrepareMainCommands();
    void _PreparePicCommands();
    void _PrepareTextCommands();
    void _PrepareMessageCommands();
    void _PrepareVocabCommands();
    void _PrepareSoundCommands();
    BOOL PreTranslateMessage(MSG* pMsg);
    LRESULT OnConstructPopupMenuCB(WPARAM wParam, LPARAM lParam);
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnFileNewPic();
    afx_msg void OnFileNewGame();
    afx_msg void OnFileNewView();
    afx_msg void OnFileNewFont();
    afx_msg void OnFileNewCursor();
    afx_msg void OnFileNewText();
    afx_msg void OnFileNewMessage();
    afx_msg void OnFileNewPalette();
    afx_msg void OnFileNewSound();
    afx_msg void OnFileOpenResource();
    afx_msg void OnFileAddResource();
    afx_msg void OnUpdateNewPic(CCmdUI *pCmdUI);
    afx_msg void OnUpdateNewMessage(CCmdUI *pCmdUI);
    afx_msg void OnUpdateShowIfGameLoaded(CCmdUI *pCmdUI);
    afx_msg void OnShowPreferences();
    afx_msg void OnRebuildResources();
    afx_msg void OnRebuildClassTable();
    afx_msg void OnExtractAllResources();
    afx_msg BOOL OnShowResource(UINT nId);
    afx_msg void OnIdleUpdateCmdUI();
    afx_msg void OnNewRoom();
    afx_msg void OnNewScript();
    afx_msg void OnCompileAll();
    afx_msg void OnUpdateNewPalette(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAlwaysEnabled(CCmdUI *pCmdUI) { pCmdUI->Enable(); }
    afx_msg void OnFindInFiles();
    afx_msg void OnUpdateBackForward(CCmdUI *pCmdUI);
    afx_msg void OnUpdateScriptCombo(CCmdUI *pCmdUI);
    afx_msg void OnGoForward();
    afx_msg void OnGoBack();
    afx_msg void OnClassBrowser();
    afx_msg void OnManageDecompilation();
    afx_msg void OnUpdateClassBrowser(CCmdUI *pCmdUI);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSelfTest();

    DECLARE_MESSAGE_MAP()

    int _fMatchWholeWord;
    int _fMatchCase;
    int _fFindInAll;
    bool _fSelfTest;

    WINDOWPLACEMENT m_dataFrameWP;
};


