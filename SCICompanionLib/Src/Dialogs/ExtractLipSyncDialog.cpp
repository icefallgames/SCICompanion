#include "stdafx.h"
#include "ExtractLipSyncDialog.h"
#include "PhonemeMap.h"
#include "Sync.h"
#include "LipSyncUtil.h"
#include "format.h"
#include "AppState.h"

#define LIPSYNC_TIMER 2345

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

// ExtractLipSyncDialog dialog

ExtractLipSyncDialog::ExtractLipSyncDialog(const AudioComponent &audio, uint8_t talker, CWnd* pParent /*=NULL*/)
    : AudioPlaybackUI<CExtResizableDialog>(ExtractLipSyncDialog::IDD, pParent), _talker(talker), _talkerToViewMap(appState)
{
    _audioCopy = std::make_unique<AudioComponent>(audio);
}

ExtractLipSyncDialog::~ExtractLipSyncDialog()
{
}

void ExtractLipSyncDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);

    DDX_Control(pDX, IDC_EDIT_LOOP, m_wndLoopNumber);
    DDX_Control(pDX, IDC_EDIT_VIEW, m_wndViewNumber);
    DDX_Control(pDX, IDC_STATIC_TALKER, m_wndTalkerLabel);
    DDX_Control(pDX, IDC_ANIMATE, m_wndMouth);
    m_wndMouth.SetBackground(g_PaintManager->GetColor(COLOR_BTNFACE));
    m_wndMouth.SetFillArea(true);

    string label = fmt::format("Talker {0} mouth view:", (int)_talker);
    m_wndTalkerLabel.SetWindowText(label.c_str());

    // Visuals
    DDX_Control(pDX, IDCANCEL, m_wndCancel);

    _UpdateViewLoop();
    _SyncViewLoop();
}

void ExtractLipSyncDialog::_SyncViewLoop()
{
    CString strView;
    m_wndViewNumber.GetWindowText(strView);
    uint16_t viewNumber = 65535;
    try
    {
        viewNumber = (uint16_t)stoi((PCSTR)strView);
    }
    catch (std::invalid_argument){}

    CString strLoop;
    m_wndLoopNumber.GetWindowText(strLoop);
    uint16_t loopNumber = 65535;
    try
    {
        loopNumber = (uint16_t)stoi((PCSTR)strLoop);
    }
    catch (std::invalid_argument){}

    bool needsNew = !_viewResource || (_viewResource->ResourceNumber != viewNumber);
    if (needsNew)
    {
        _viewResource = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::View, viewNumber);
        if (!_viewResource)
        {
            std::string mouthSampleFilename = appState->GetResourceMap().GetSamplesFolder() + "\\views\\MouthShapes.bin";
            ResourceBlob blob;
            if (SUCCEEDED(blob.CreateFromFile("it's a mouth", mouthSampleFilename, sciVersion1_1, -1, -1)))
            {
                _viewResource = CreateResourceFromResourceData(blob);
            }
        }
        m_wndMouth.SetResource(_viewResource.get(), nullptr);
    }
    m_wndMouth.SetLoop(loopNumber);

    _talkerToViewMap.SetTalkerToViewLoop(_talker, viewNumber, loopNumber);
}

void ExtractLipSyncDialog::_UpdateViewLoop()
{
    uint16_t view, loop;
    if (_talkerToViewMap.TalkerToViewLoop(_talker, view, loop))
    {
        m_wndLoopNumber.SetWindowText(fmt::format("{}", loop).c_str());
        m_wndViewNumber.SetWindowText(fmt::format("{}", view).c_str());
    }
    else
    {
        m_wndLoopNumber.SetWindowText("");
        m_wndViewNumber.SetWindowText("");
    }
}

BOOL ExtractLipSyncDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    ShowSizeGrip(FALSE);
    return fRet;
}

void ExtractLipSyncDialog::OnCancel()
{
    // TODO: what to do? if we cancelled? how do I stop it
    __super::OnCancel();
}

BEGIN_MESSAGE_MAP(ExtractLipSyncDialog, AudioPlaybackUI<CExtResizableDialog>)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_RESETMAPPING, &ExtractLipSyncDialog::OnBnClickedButtonResetmapping)
    ON_EN_KILLFOCUS(IDC_EDIT_PHONEMEMAP, &ExtractLipSyncDialog::OnEnKillfocusEditPhonememap)
    ON_EN_KILLFOCUS(IDC_EDIT_VIEW, &ExtractLipSyncDialog::OnEnKillfocusEditView)
    ON_EN_KILLFOCUS(IDC_EDIT_LOOP, &ExtractLipSyncDialog::OnEnKillfocusEditLoop)
    ON_BN_CLICKED(ID_GENERATELIPSYNC, &ExtractLipSyncDialog::OnBnClickedGeneratelipsync)
END_MESSAGE_MAP()

void ExtractLipSyncDialog::OnBnClickedButtonResetmapping()
{
    // TODO: get the default phonememap, after asking user if they're sure...
}


void ExtractLipSyncDialog::OnEnKillfocusEditPhonememap()
{
    // TODO: Commit the phonemap here
}


void ExtractLipSyncDialog::OnEnKillfocusEditView()
{
    _SyncViewLoop();
}


void ExtractLipSyncDialog::OnEnKillfocusEditLoop()
{
    _SyncViewLoop();
}


void ExtractLipSyncDialog::OnBnClickedGeneratelipsync()
{
    // TODO: Start the lipsync data gen task
}
