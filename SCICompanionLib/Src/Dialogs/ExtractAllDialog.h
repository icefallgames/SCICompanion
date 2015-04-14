#pragma once

#include "CompileContext.h"
#include "ExtractAll.h"

// CCompileDialog dialog

class ExtractAllDialog : public CExtResizableDialog, public IExtractProgress
{
public:
    ExtractAllDialog(CWnd* pParent = NULL);   // standard constructor
    virtual ~ExtractAllDialog();
    bool GetResult() { return _fResult; }
    bool GetAborted() { return _fAbort; }
    virtual void OnCancel();

    // Dialog Data
    enum { IDD = IDD_EXTRACTALL };

    bool SetProgress(const std::string &info, int amountDone, int totalAmount) override;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    LRESULT UpdateStatus(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnInitDialog();
    static UINT s_ThreadWorker(void *pParam);

    DECLARE_MESSAGE_MAP()

    CExtProgressWnd m_wndProgress;
    CExtEdit m_wndDisplay;
    CExtEdit m_wndFolderLocation;
    CExtLabel m_wndStatic;

    bool _fResult;
    bool _fAbort;
    bool _fExtracting;
    int _nScript;

    // Visuals
    CExtButton m_wndCancel;
    CExtButton m_wndExtract;
    CExtButton m_wndBrowse;
    CExtCheckBox m_wndExtractResources;
    CExtCheckBox m_wndExtractPicImage;
    CExtCheckBox m_wndExtractViewImage;
    CExtCheckBox m_wndDisassembleScripts;

    std::unique_ptr<CWinThread> _pThread;
    CString _location;
    SCIVersion _version;
    bool _extractPicImages;
    bool _extractViewImages;
    bool _disassembleScripts;
    HANDLE _hThread;

    void OnTimer(UINT_PTR nIDEvent);

public:
    afx_msg void OnBnClickedButtonbrowse();
    afx_msg void OnBnClickedExtract();
};
