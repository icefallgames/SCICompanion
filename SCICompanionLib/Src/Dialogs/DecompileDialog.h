#pragma once
#include "afxwin.h"
#include "GameFolderHelper.h"
#include "DecompilerResults.h"

class CSCOFile;

class DecompilerDialogResults : public IDecompilerResults
{
public:
    DecompilerDialogResults(HWND hwnd) : _aborted(false), _hwnd(hwnd) {}
    void AddResult(DecompilerResultType type, const std::string &message) override;

    bool IsAborted() override { return _aborted; }
    void SetAborted() { _aborted = true; }

private:
    bool _aborted;
    HWND _hwnd;
};

class DecompileDialog : public CExtResizableDialog
{
public:
    DecompileDialog(CWnd* pParent = nullptr);   // standard constructor

    // Dialog Data
    enum { IDD = IDD_DECOMPILER };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL PreTranslateMessage(MSG* pMsg) override;
    BOOL OnInitDialog() override;

    DECLARE_MESSAGE_MAP()
private:
    CExtEdit m_wndScript;
    CExtEdit m_wndResults;
    CExtEdit m_wndStatus;
    CListCtrl m_wndListScripts;

    // Visuals
    CExtButton m_wndCancel;
    CExtButton m_wndDecompile;
    CExtButton m_wndDecomileCancel;
    CExtButton m_wndClearSCO;
    CExtButton m_wndSetFilenames;

    CExtCheckBox m_wndDebugControlFlow;
    CExtCheckBox m_wndDebugInstConsumption;

    // Our own copy of this
    GameFolderHelper _helper;

    CTreeCtrl m_wndTreeSCO;
    std::unique_ptr<CSCOFile> _sco;
    std::vector<int> _scoPublicProcIndices; // Since sco exports include both instances and procs, but we can only edit proc names.
    bool _inSCOLabelEdit;
    bool _inScriptListLabelEdit;

    int previousSelection;
    bool initialized;

    void _PopulateScripts();
    void _UpdateScripts(std::set<uint16_t> updatedScripts);
    void _PopulateSCOTree();
    void _InitScriptList();
    void _SyncSelection(bool force = false);
    void _SyncButtonState();

    static UINT s_ThreadWorker(void *pParam);
    void OnTimer(UINT_PTR nIDEvent);
    LRESULT UpdateStatus(WPARAM wParam, LPARAM lParam);
    std::unique_ptr<DecompilerDialogResults> _decompileResults;
    std::unique_ptr<CWinThread> _pThread;
    std::set<uint16_t> _scriptNumbers;
    HANDLE _hThread;
    bool _debugControlFlow;
    bool _debugInstConsumption;
    std::unique_ptr<GlobalCompiledScriptLookups> _lookups;
public:
    afx_msg void OnLvnItemchangedListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnBeginlabeleditTreesco(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnEndlabeleditTreesco(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnEndlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedDecompile();
    afx_msg void OnBnClickedAssignfilenames();
    afx_msg void OnBnClickedDecompilecancel();
};
