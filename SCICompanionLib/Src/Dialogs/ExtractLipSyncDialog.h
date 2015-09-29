#pragma once

#include "resource.h"

class sapi_textless_lipsync;
class phoneme_estimator;

class ExtractLipSyncDialog : public CExtResizableDialog
{
public:
    ExtractLipSyncDialog(const std::string &filename, CWnd* pParent = NULL);   // standard constructor
    virtual ~ExtractLipSyncDialog();
    virtual void OnCancel();

    // Dialog Data
    enum { IDD = IDD_LIPSYNCDIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

    afx_msg void OnTimer(UINT_PTR nIDEvent);

    std::unique_ptr<sapi_textless_lipsync> _lipSync;
    std::unique_ptr<phoneme_estimator> _sapi51Estimator;

    CExtProgressWnd m_wndProgress;
    CExtEdit m_wndDisplay;

    // Visuals
    CExtButton m_wndCancel;

    HRESULT _hrCoinit;
    std::string _filename;
};
