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
#pragma once
#include "afxwin.h"
#include "GameFolderHelper.h"
#include "DecompilerResults.h"
#include <future>

class CSCOFile;
class IDecompilerConfig;

class DecompilerDialogResults : public IDecompilerResults
{
public:
    DecompilerDialogResults(HWND hwnd) : _aborted(false), _hwnd(hwnd), _successCount(0), _fallbackCount(0), _successBytes(0), _fallbackBytes(0) {}
    void AddResult(DecompilerResultType type, const std::string &message) override;
    void InformStats(bool functionSuccessful, int byteCount) override;
    bool IsAborted() override { return _aborted; }

    void SetAborted() { _aborted = true; }

    // Stats
    int _successCount;
    int _fallbackCount;
    int _successBytes;
    int _fallbackBytes;

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
    CProgressCtrl m_wndProgress;

    CExtCheckBox m_wndDebugControlFlow;
    CExtCheckBox m_wndDebugInstConsumption;
    CExtEdit m_wndDebugFunctionMatch;
    CExtLabel m_wndSCOLabel;

    CExtCheckBox m_wndAsm;

    // Our own copy of this
    GameFolderHelper _helper;

    CTreeCtrl m_wndTreeSCO;
    std::unique_ptr<CSCOFile> _sco;
    std::vector<int> _scoPublicProcIndices; // Since sco exports include both instances and procs, but we can only edit proc names.
    bool _inSCOLabelEdit;
    bool _inScriptListLabelEdit;
    bool _needsSetFilenames;

    int previousSelection;
    bool initialized;

    void _PopulateScripts();
    void _UpdateScripts(std::set<uint16_t> updatedScripts);
    void _PopulateSCOTree();
    void _InitScriptList();
    void _SyncSelection(bool force = false);
    void _SyncButtonState();

    static void s_DecompileThreadWorker(DecompileDialog *pThis);
    void OnTimer(UINT_PTR nIDEvent);
    LRESULT UpdateStatus(WPARAM wParam, LPARAM lParam);
    std::unique_ptr<DecompilerDialogResults> _decompileResults;
    std::unique_ptr<std::future<void>> _future;

    std::set<uint16_t> _scriptNumbers;
    bool _debugControlFlow;
    bool _debugInstConsumption;
    bool _debugAsm;
    CString _debugFunctionMatch;
    std::unique_ptr<GlobalCompiledScriptLookups> _lookups;
    std::unique_ptr<IDecompilerConfig> _decompilerConfig;
    void _AssignFilenames();
public:
    afx_msg void OnLvnItemchangedListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnBeginlabeleditTreesco(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnEndlabeleditTreesco(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnEndlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedDecompile();
    afx_msg void OnBnClickedAssignfilenames();
    afx_msg void OnBnClickedDecompilecancel();
    afx_msg void OnBnClickedClearsco();
};
