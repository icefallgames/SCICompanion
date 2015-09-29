#include "stdafx.h"
#include <sphelper.h>
#include "sapi_lipsync.h"
#include "phone_estimate.h"
#include "ExtractLipSyncDialog.h"

#include "AppState.h"
#include "ScriptOM.h"
#include "NewCompileDialog.h"
#include "ScriptDocument.h"

#include <locale>
#include <codecvt>

#define LIPSYNC_TIMER 2345

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ExtractLipSyncDialog dialog

ExtractLipSyncDialog::ExtractLipSyncDialog(const std::string &filename, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(ExtractLipSyncDialog::IDD, pParent), _filename(filename), _hrCoinit(E_FAIL)
{
}

ExtractLipSyncDialog::~ExtractLipSyncDialog()
{
    _lipSync.reset(nullptr);
    _sapi51Estimator.reset(nullptr);

    if (SUCCEEDED(_hrCoinit))
    {
        CoUninitialize();
    }
}

void ExtractLipSyncDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
    DDX_Control(pDX, IDC_EDIT, m_wndDisplay);

    // Visuals
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
}

BOOL ExtractLipSyncDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    ShowSizeGrip(FALSE);

    _hrCoinit = CoInitialize(nullptr);

    // 1. [optional] declare the SAPI 5.1 estimator. 
    // NOTE: for different phoneme sets: create a new estimator
    _sapi51Estimator = std::make_unique<phoneme_estimator>();

    // 2. declare the sapi lipsync object and call the lipsync method to
    // start the lipsync process
    _lipSync = std::make_unique<sapi_textless_lipsync>(_sapi51Estimator.get());

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wfilename = converter.from_bytes(_filename);
    if (_lipSync->lipsync(wfilename))
    {
        SetTimer(LIPSYNC_TIMER, 200, nullptr);
    }
    else
    {
        PostMessage(WM_CLOSE, 0, 0);
    }

    // TODO: STart it here.

    // make a timer and check periodically.
    // 

    return fRet;
}

void ExtractLipSyncDialog::OnCancel()
{
    // TODO: what to do? if we cancelled? how do I stop it
    __super::OnCancel();
}

void ExtractLipSyncDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == LIPSYNC_TIMER)
    {
        if (_lipSync->isDone())
        {
            KillTimer(nIDEvent);

            // 5. finalize the lipsync results for printing
            // this call will estimate phoneme timings 
            _lipSync->finalize_phoneme_alignment();

            std::stringstream ss;
            _lipSync->print_results(ss);
            ShowTextFile(ss.str().c_str(), "LipSync.txt");

            ///PostMessage(WM_CLOSE, 0, 0);
            /*
            std::vector<alignment_result> &results = _lipSync->get_phoneme_alignment();
            for (auto &result : results)
            {
                result.
            }*/
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

BEGIN_MESSAGE_MAP(ExtractLipSyncDialog, CExtResizableDialog)
    ON_WM_DESTROY()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// ExtractLipSyncDialog message handlers
