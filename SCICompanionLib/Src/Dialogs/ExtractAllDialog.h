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

#include "ExtractAll.h"
#include <future>

// CCompileDialog dialog

class ExtractAllDialog : public CExtResizableDialog, public IExtractProgress
{
public:
    ExtractAllDialog(CWnd* pParent = nullptr);   // standard constructor
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
    static void s_ThreadWorker(ExtractAllDialog *pParam);

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
    CExtCheckBox m_wndExportMessages;
    CExtCheckBox m_wndGenerateWav;

    std::unique_ptr<std::future<void>> _future;
    CString _location;
    SCIVersion _version;
    bool _extractResources;
    bool _extractPicImages;
    bool _extractViewImages;
    bool _disassembleScripts;
    bool _exportMessages;
    bool _generateWavs;

    void OnTimer(UINT_PTR nIDEvent);

public:
    afx_msg void OnBnClickedButtonbrowse();
    afx_msg void OnBnClickedExtract();
};
