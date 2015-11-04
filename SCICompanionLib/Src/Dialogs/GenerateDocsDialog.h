#pragma once

#include "resource.h"
#include "GameFolderHelper.h"
// GenerateDocsDialog dialog

class GameFolderHelper;

class GenerateDocsDialog : public CDialog
{
    DECLARE_DYNAMIC(GenerateDocsDialog)

public:
    GenerateDocsDialog(GameFolderHelper helper, CWnd* pParent = NULL);   // standard constructor
    virtual ~GenerateDocsDialog();

    // Dialog Data
    enum { IDD = IDD_DOCGENERATOR };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    void OnSelChange();
    void OnInstanceSelChange();

private:
    DECLARE_MESSAGE_MAP()
    void _ShowHTML(const std::string &filename);
    void _PopulateScripts();
    void _RunBuild(const std::string &docGenFolder, const std::string &docGenCommand);

protected:
    CListCtrl m_wndScripts;
    CListBox m_wndGeneratedFiles;
    CEdit m_wndFolder;
    CEdit m_wndCommand;

    CWnd m_wndBrowser;
    GameFolderHelper _helper;
    bool _fInitialized;
public:
    afx_msg void OnBnClickedGeneratedoc();
    afx_msg void OnLbnSelchangeListgenerated();
    afx_msg void OnEnChangeEditfolder();
    afx_msg void OnEnChangeEditcommand();
};
