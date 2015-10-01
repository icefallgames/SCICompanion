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
    : AudioPlaybackUI<CExtResizableDialog>(ExtractLipSyncDialog::IDD, pParent), _talker(talker), _talkerToViewMap(appState), _initialized(false), _haveActualViewResource(false)
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

        DDX_Control(pDX, IDC_EDIT_PHONEMEMAP, m_wndEditPhonemeMap);
        DDX_Control(pDX, IDC_EDIT_PHONEMEMAPSTATUS, m_wndEditPhonemeMapStatus);

        DDX_Control(pDX, IDC_LIST_SYNCRESOURCE, m_wndSyncList);
        _InitSyncListColumns();
        _UpdateSyncList();

        // Visuals
        DDX_Control(pDX, IDCANCEL, m_wndCancel);
        DDX_Control(pDX, IDOK, m_wndOk);
        DDX_Control(pDX, IDC_STATIC_SYNC36, m_wndStatic1);
        DDX_Control(pDX, IDC_STATIC_LOOP, m_wndStatic2);
        DDX_Control(pDX, IDC_STATIC_PHONEME, m_wndStatic3);
        DDX_Control(pDX, IDC_STATIC_WORDS, m_wndStatic4);
        DDX_Control(pDX, IDC_BUTTON_COMMITMAPPING, m_wndCommitMapping);
        DDX_Control(pDX, IDC_BUTTON_RESETMAPPING, m_wndResetMapping);

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

    bool useSampleMap = false;
    bool needsNew = !_viewResource || (_viewResource->ResourceNumber != viewNumber);
    if (needsNew)
    {
        _viewResource = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::View, viewNumber);
        _haveActualViewResource = !!_viewResource;
        if (!_viewResource)
        {
            std::string mouthSampleFilename = appState->GetResourceMap().GetSamplesFolder() + "\\views\\MouthShapes.bin";
            ResourceBlob blob;
            if (SUCCEEDED(blob.CreateFromFile("it's a mouth", mouthSampleFilename, sciVersion1_1, -1, -1)))
            {
                _viewResource = CreateResourceFromResourceData(blob);
                useSampleMap = true;
            }
        }
        m_wndMouth.SetResource(_viewResource.get(), nullptr);
    }
    m_wndMouth.SetLoop(loopNumber);

    _talkerToViewMap.SetTalkerToViewLoop(_talker, viewNumber, loopNumber);

    // Try to load a phonememap.
    if (useSampleMap)
    {
        _phonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\sample_phoneme_map.ini");
        m_wndCommitMapping.EnableWindow(TRUE);
        m_wndEditPhonemeMapStatus.SetWindowText("Loaded sample phoneme map.");
    }
    else
    {
        // Try to load one. If we can't, show errors and load a default one.
        _phonemeMap = LoadPhonemeMapForViewLoop(appState, viewNumber, loopNumber);
        if (_phonemeMap->HasErrors() && _phonemeMap->GetFileContents().empty())
        {
            // couldn't even load the file. Show the deafult one?
            _phonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\default_phoneme_map.ini");
            m_wndCommitMapping.EnableWindow(TRUE);
            m_wndEditPhonemeMapStatus.SetWindowText("Loaded a default phoneme map, as none exists for this view/loop");
        }
        else
        {
            m_wndCommitMapping.EnableWindow(FALSE);
            if (_phonemeMap->HasErrors())
            {
                m_wndEditPhonemeMapStatus.SetWindowText(_phonemeMap->GetErrors().c_str());
            }
            else
            {
                std::string status = "Loaded " + _phonemeMap->GetFilespec();
                m_wndEditPhonemeMapStatus.SetWindowText(status.c_str());
            }
        }
    }
    m_wndEditPhonemeMap.SetWindowText(_phonemeMap->GetFileContents().c_str());
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

std::unique_ptr<SyncComponent> ExtractLipSyncDialog::GetSyncComponent() const
{
    const SyncComponent *sync = _audioResource->TryGetComponent<SyncComponent>();
    std::unique_ptr<SyncComponent> syncReturn;
    if (sync)
    {
        syncReturn = std::make_unique<SyncComponent>(*sync);
    }
    return syncReturn;
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
    ON_BN_CLICKED(IDC_BUTTON_COMMITMAPPING, &ExtractLipSyncDialog::OnBnClickedButtonCommitmapping)
END_MESSAGE_MAP()

void ExtractLipSyncDialog::OnBnClickedButtonResetmapping()
{
    if (IDYES == AfxMessageBox("Replace the current mapping with the template?", MB_YESNO))
    {
        _phonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\default_phoneme_map.ini");
        m_wndCommitMapping.EnableWindow(TRUE);
        m_wndEditPhonemeMap.SetWindowText(_phonemeMap->GetFileContents().c_str());
    }
}

void ExtractLipSyncDialog::OnEnKillfocusEditPhonememap()
{
    m_wndCommitMapping.EnableWindow(TRUE);
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
    if (_phonemeMap)
    {
        m_wndLipSyncButton.EnableWindow(FALSE);
        PhonemeMap phonemeMapCopy = *_phonemeMap;
        _taskSink->StartTask(
            [audioCopy, phonemeMapCopy]() { return CreateLipSyncComponentAndRawDataFromAudioAndPhonemes(audioCopy, phonemeMapCopy); }
        );
        m_wndProgress.SendMessage(PBM_SETMARQUEE, TRUE, LipSyncMarqueeMilliseconds);
    }
}


void ExtractLipSyncDialog::OnBnClickedButtonCommitmapping()
{
    if (!_haveActualViewResource)
    {
        AfxMessageBox("You need to choose an existing view and loop before saving.", MB_OK | MB_ICONWARNING);
    }
    else
    {
        std::string errors;
        CString strText;
        m_wndEditPhonemeMap.GetWindowText(strText);
        std::string message = "Saving ";
        message += GetPhonemeMapFilespec(appState, _viewResource->ResourceNumber, m_wndMouth.GetLoop());
        if (!SaveForViewLoop((PCSTR)strText, appState, _viewResource->ResourceNumber, m_wndMouth.GetLoop(), errors))
        {
            message += " : ";
            message += errors;
        }
        m_wndEditPhonemeMapStatus.SetWindowText(message.c_str());
    }
}
