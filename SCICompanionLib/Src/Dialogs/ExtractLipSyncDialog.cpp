#include "stdafx.h"
#include "ExtractLipSyncDialog.h"
#include "PhonemeMap.h"
#include "Sync.h"
#include "LipSyncUtil.h"
#include "format.h"
#include "AppState.h"
#include <sphelper.h>
#include "sapi_lipsync.h"
#include <locale>
#include <codecvt>

#define LIPSYNC_TIMER 2345

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

struct LipSyncDialogTaskResult
{
    SyncComponent Sync;
    std::vector<alignment_result> RawResults;
};

#define UWM_LIPSYNCTASKDONE (WM_APP + 1)

// ExtractLipSyncDialog dialog

ExtractLipSyncDialog::ExtractLipSyncDialog(const ResourceEntity &resource, uint8_t talker, CWnd* pParent /*=NULL*/)
    : AudioPlaybackUI<CExtResizableDialog>(ExtractLipSyncDialog::IDD, pParent), _talker(talker), _talkerToViewMap(appState), _initialized(false)
{
    _audioResource = resource.Clone();
    _taskSink = std::make_unique<CWndTaskSink<LipSyncDialogTaskResult>>(this, UWM_LIPSYNCTASKDONE);
}

ExtractLipSyncDialog::~ExtractLipSyncDialog()
{
}

void ExtractLipSyncDialog::_UpdateSyncList()
{
    m_wndSyncList.SetRedraw(FALSE);
    m_wndSyncList.DeleteAllItems();
    if (_audioResource->TryGetComponent<SyncComponent>())
    {
        int index = 0;
        for (auto &entry : _audioResource->TryGetComponent<SyncComponent>()->Entries)
        {
            m_wndSyncList.InsertItem(index, fmt::format("{}", (int)entry.Tick).c_str());
            m_wndSyncList.SetItem(index, 1, LVIF_TEXT, fmt::format("{}", (int)entry.Cel).c_str(), 0, 0, 0, 0);
            index++;
        }
    }
    m_wndSyncList.SetRedraw(TRUE);

}

void ExtractLipSyncDialog::_InitSyncListColumns()
{
    ListView_SetExtendedListViewStyle(m_wndSyncList.GetSafeHwnd(), LVS_EX_FULLROWSELECT);

    // Name
    LVCOLUMN col = { 0 };
    col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
    col.iOrder = 0;
    col.iSubItem = 0;
    col.pszText = "Ticks";
    col.cx = 40;
    col.fmt = LVCFMT_LEFT;
    m_wndSyncList.InsertColumn(0, &col);

    col.iOrder = 1;
    col.iSubItem = 1;
    col.pszText = "Cel";
    col.cx = 40;
    col.fmt = LVCFMT_LEFT;
    m_wndSyncList.InsertColumn(1, &col);
}

void ExtractLipSyncDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    if (!_initialized)
    {
        _initialized = true;

        DoDataExchangeHelper(pDX);

        DDX_Control(pDX, IDC_EDIT_WORDS, m_rawLipSyncWords);
        DDX_Control(pDX, ID_GENERATELIPSYNC, m_wndLipSyncButton);
        DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
        // Set this here instead of the rc file, due to the xp toolset issue mentioned above.
        m_wndProgress.ModifyStyle(0, PBS_MARQUEE, 0);
        // For some reason this seems necessary, even though I'm using a marquee progress bar:
        m_wndProgress.SetRange(0, 100);
        m_wndProgress.SetPos(1);

        DDX_Control(pDX, IDC_EDIT_LOOP, m_wndLoopNumber);
        DDX_Control(pDX, IDC_EDIT_VIEW, m_wndViewNumber);
        DDX_Control(pDX, IDC_STATIC_TALKER, m_wndTalkerLabel);
        DDX_Control(pDX, IDC_ANIMATE, m_wndMouth);
        m_wndMouth.SetBackground(g_PaintManager->GetColor(COLOR_BTNFACE));
        m_wndMouth.SetFillArea(true);
        SetMouthElement(&m_wndMouth);

        string label = fmt::format("Talker {0} mouth view:", (int)_talker);
        m_wndTalkerLabel.SetWindowText(label.c_str());

        DDX_Control(pDX, IDC_LIST_SYNCRESOURCE, m_wndSyncList);
        _InitSyncListColumns();
        _UpdateSyncList();

        // Visuals
        DDX_Control(pDX, IDCANCEL, m_wndCancel);

        SetAudioResource(_audioResource.get());

        DDX_Control(pDX, IDC_WAVEFORM, m_wndWaveform);
        m_wndWaveform.SetResource(_audioResource.get());
        SetWaveformElement(&m_wndWaveform);

        _UpdateViewLoop();
        _SyncViewLoop();
    }
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
    OnInitDialogHelper();
    ShowSizeGrip(FALSE);
    return fRet;
}

void ExtractLipSyncDialog::OnCancel()
{
    // TODO: what to do? if we cancelled? how do I stop it
    __super::OnCancel();
}

void ExtractLipSyncDialog::_UpdateWords(const std::vector<alignment_result> &rawResults)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::stringstream ss;
    for (auto &result : rawResults)
    {
        ss << converter.to_bytes(result.m_orthography);
        ss << " ";
    }
    m_rawLipSyncWords.SetWindowText(ss.str().c_str());
}

const int LipSyncMarqueeMilliseconds = 30;

LRESULT ExtractLipSyncDialog::_OnLipSyncDone(WPARAM wParam, LPARAM lParam)
{
    m_wndLipSyncButton.EnableWindow(TRUE);
    LipSyncDialogTaskResult result = _taskSink->GetResponse();

    _audioResource->RemoveComponent<SyncComponent>();
    _audioResource->AddComponent<SyncComponent>(make_unique<SyncComponent>(result.Sync));
    m_wndWaveform.SetRawLipSyncData(result.RawResults);
    _UpdateWords(result.RawResults);
    m_wndProgress.SendMessage(PBM_SETMARQUEE, FALSE, LipSyncMarqueeMilliseconds);
    _UpdateSyncList();
    return 0;
}

BEGIN_MESSAGE_MAP(ExtractLipSyncDialog, AudioPlaybackUI<CExtResizableDialog>)
    ON_WM_DESTROY()
    ON_MESSAGE(UWM_LIPSYNCTASKDONE, _OnLipSyncDone)
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

LipSyncDialogTaskResult CreateLipSyncComponentAndRawDataFromAudioAndPhonemes(const AudioComponent &audio, const PhonemeMap &phonemeMap)
{
    std::vector<alignment_result> rawResults;
    LipSyncDialogTaskResult result;
    std::unique_ptr<SyncComponent> syncComponent = CreateLipSyncComponentFromAudioAndPhonemes(audio, phonemeMap, &rawResults);
    if (syncComponent)
    {
        result = { *syncComponent, rawResults };
    }
    return result;
}

void ExtractLipSyncDialog::OnBnClickedGeneratelipsync()
{
    AudioComponent audioCopy = _audio->GetComponent<AudioComponent>();
    // TODO: Get the proper phonemes.
    PhonemeMap samplePhonemeMap(GetExeSubFolder("Samples") + "\\sample_phoneme_map.ini");

    m_wndLipSyncButton.EnableWindow(FALSE);

    _taskSink->StartTask(
        [audioCopy, samplePhonemeMap]() { return CreateLipSyncComponentAndRawDataFromAudioAndPhonemes(audioCopy, samplePhonemeMap); }
    );
    m_wndProgress.SendMessage(PBM_SETMARQUEE, TRUE, LipSyncMarqueeMilliseconds);
}
