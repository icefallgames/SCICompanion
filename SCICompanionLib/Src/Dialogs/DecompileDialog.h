#pragma once
#include "afxwin.h"

class CSCOFile;

class DecompileDialog : public CExtResizableDialog
{
public:
    DecompileDialog(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    enum { IDD = IDD_DECOMPILER };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL PreTranslateMessage(MSG* pMsg) override;

    DECLARE_MESSAGE_MAP()
private:
    CExtEdit m_wndScript;
    CExtEdit m_wndResults;
    CListCtrl m_wndListScripts;

    // Visuals
    CExtButton m_wndOK;
    CExtButton m_wndDecompile;
    CExtButton m_wndClearSCO;
    CExtButton m_wndSetFilenames;

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
public:
    afx_msg void OnLvnItemchangedListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnBeginlabeleditTreesco(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnEndlabeleditTreesco(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnEndlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedDecompile();
    afx_msg void OnBnClickedAssignfilenames();
};
