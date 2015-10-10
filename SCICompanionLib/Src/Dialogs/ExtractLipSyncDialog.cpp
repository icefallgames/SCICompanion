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
#include "ChooseTalkerViewLoopDialog.h"
#include "Message.h"
#include "AudioMap.h"
#include "AudioEditDialog.h"
#include "AudioNegative.h"
#include "AudioProcessing.h"
#include "PhonemeDialog.h"

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

ExtractLipSyncDialog::ExtractLipSyncDialog(const ResourceEntity &resource, uint8_t talker, const std::string &talkerName, const std::string &messageText, bool useText, CWnd * pParent /*=NULL*/)
    : AudioPlaybackUI<CExtResizableDialog>(ExtractLipSyncDialog::IDD, pParent),
    _talker(talker),
    _talkerName(talkerName),
    _initialized(false),
    _wantToUseSample(false),
    _nLoop(0),
    _nView(0),
    _messageText(messageText),
    _useText(useText)
{
    uint16_t view, loop;
    if (appState->GetResourceMap().GetTalkerToViewMap().TalkerToViewLoop(_talker, view, loop))
    {
        _nView = view;
        _nLoop = loop;
        _wantToUseSample = false;
    }
    else
    {
        _wantToUseSample = true;
    }
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

    // Sync information may have changed, and this is the way we do it:
    SetAudioResource(_audioResource.get());
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

        DDX_Control(pDX, IDC_CHECK_USESAMPLE, m_wndUseSample);
        DDX_Control(pDX, IDC_CHECK_USETEXT, m_wndUseText);
        m_wndUseText.SetCheck(_useText ? BST_CHECKED : BST_UNCHECKED);
        DDX_Control(pDX, IDC_BUTTON_SETVIEW, m_wndSetView);
        DDX_Control(pDX, IDC_BUTTON_DELETE_SYNC, m_wndDeleteSync);
        m_wndDeleteSync.SetIcon(IDI_DELETE, 0, 0, 0, 16, 16);
        DDX_Control(pDX, IDC_BUTTON_IMPORTSYNC, m_wndImportSync);
        m_wndImportSync.SetIcon(IDI_FILE_OPEN, 0, 0, 0, 16, 16);
        DDX_Control(pDX, IDC_BUTTON_EXPORTSYNC, m_wndExportSync);
        m_wndExportSync.SetIcon(IDI_FILE_SAVE, 0, 0, 0, 16, 16);

        DDX_Control(pDX, IDC_BUTTON_RAW, m_wndButtonRaw);
        std::string rawLipSyncData;
        if (_audioResource->TryGetComponent<SyncComponent>())
        {
            rawLipSyncData = _audioResource->TryGetComponent<SyncComponent>()->RawData;
        }
        m_wndButtonRaw.ShowWindow(!rawLipSyncData.empty() ? SW_SHOW : SW_HIDE);

        DDX_Control(pDX, IDC_EDIT_WORDS, m_rawLipSyncWords);
        DDX_Control(pDX, ID_GENERATELIPSYNC, m_wndLipSyncButton);
        DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
        // Set this here instead of the rc file, due to the xp toolset issue mentioned above.
        m_wndProgress.ModifyStyle(0, PBS_MARQUEE, 0);
        // For some reason this seems necessary, even though I'm using a marquee progress bar:
        m_wndProgress.SetRange(0, 100);
        m_wndProgress.SetPos(1);

        DDX_Control(pDX, IDC_STATIC_TALKER, m_wndTalkerLabel);
        DDX_Control(pDX, IDC_ANIMATE, m_wndMouth);
        m_wndMouth.SetBackground(g_PaintManager->GetColor(COLOR_BTNFACE));
        m_wndMouth.SetFillArea(true);
        SetMouthElement(&m_wndMouth);

        DDX_Control(pDX, IDC_EDIT_PHONEMEMAP, m_wndEditPhonemeMap);
        DDX_Control(pDX, IDC_EDIT_PHONEMEMAPSTATUS, m_wndEditPhonemeMapStatus);

        DDX_Control(pDX, IDC_LIST_SYNCRESOURCE, m_wndSyncList);
        _InitSyncListColumns();
        _UpdateSyncList();

        DDX_Control(pDX, IDC_GROUP_TALKER, m_wndGroupTalker);
        m_wndGroupTalker.SetWindowText(fmt::format("Talker {0} ({1})", (int)_talker, _talkerName).c_str());

        DDX_Control(pDX, IDC_GROUP_MESSAGE, m_wndGroupMessage);
        m_wndGroupMessage.SetWindowText(fmt::format("Message {0}: \"{1}...\"", _audioResource->ResourceNumber, _messageText.substr(0, 35)).c_str());

        // Visuals
        DDX_Control(pDX, IDCANCEL, m_wndCancel);
        DDX_Control(pDX, IDOK, m_wndOk);
        DDX_Control(pDX, IDC_STATIC_SYNC36, m_wndStatic1);
        DDX_Control(pDX, IDC_STATIC_PHONEME, m_wndStatic3);
        DDX_Control(pDX, IDC_GROUP_VIEWLOOP, m_wndGroupViewLoop);
        DDX_Control(pDX, IDC_STATIC_WORDS, m_wndStatic4);
        DDX_Control(pDX, IDC_BUTTON_RELOADMAPPING, m_wndReloadMapping);
        DDX_Control(pDX, IDC_BUTTON_OPENMAPPING, m_wndOpenMapping);
        DDX_Control(pDX, IDC_BUTTON_RESETMAPPING, m_wndResetMapping);

        SetAudioResource(_audioResource.get());

        DDX_Control(pDX, IDC_WAVEFORM, m_wndWaveform);
        m_wndWaveform.SetResource(_audioResource->TryGetComponent<AudioComponent>());
        SetWaveformElement(&m_wndWaveform);
        if (!rawLipSyncData.empty())
        {
            m_wndWaveform.SetRawLipSyncData(_audioResource->GetComponent<SyncComponent>());
        }

        DDX_Control(pDX, IDC_EDITPHONEMEMAP, m_wndEditPhoneme);

        _SyncViewLoop();

        DDX_Control(pDX, IDC_EDITAUDIO, m_wndEditAudio);
        m_wndEditAudio.SetIcon(IDI_EDITPALETTE, 0, 0, 0, 24, 24);
        m_wndEditAudio.EnableWindow(_audioResource->TryGetComponent<AudioNegativeComponent>() != nullptr);
    }
}

void ExtractLipSyncDialog::_SyncViewLoop()
{
    // This indicates that _useSample or _nView or _nLoop has changed.
    
    bool needsNew = !_viewResource || (_viewResource->ResourceNumber != _nView) || (_wantToUseSample != _actuallyUsingSample);
    if (needsNew)
    {
        _viewResource = nullptr;
        if (!_wantToUseSample)
        {
            _viewResource = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::View, _nView);
            _actuallyUsingSample = false;
        }
        if (!_viewResource)
        {
            // Fallback to sample view when we want it, or when we couldn't load it.
            std::string mouthSampleFilename = appState->GetResourceMap().GetSamplesFolder() + "\\views\\MouthShapes.bin";
            ResourceBlob blob;
            if (SUCCEEDED(blob.CreateFromFile("it's a mouth", mouthSampleFilename, sciVersion1_1, -1, -1)))
            {
                _viewResource = CreateResourceFromResourceData(blob);
                _actuallyUsingSample = true;
            }
        }
        m_wndMouth.SetResource(_viewResource.get());
    }
    m_wndMouth.SetLoop(_nLoop);

    string label = _wantToUseSample ?
        fmt::format("Sample View/Loop") :
        fmt::format("Using View {0}, Loop {1}", _nView, _nLoop);
    m_wndTalkerLabel.SetWindowText(label.c_str());

    m_wndReloadMapping.EnableWindow(!_wantToUseSample);
    m_wndEditPhoneme.EnableWindow(!_wantToUseSample);
    m_wndOpenMapping.EnableWindow(!_wantToUseSample);
    m_wndResetMapping.EnableWindow(!_wantToUseSample);

    // Try to load a phonememap.
    if (_wantToUseSample)
    {
        _phonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\sample_phoneme_map.ini");
        m_wndEditPhonemeMapStatus.SetWindowText("Loaded sample phoneme map.");
        m_wndGroupViewLoop.SetWindowText("Sample");
        m_wndEditPhonemeMap.SetWindowText(_phonemeMap->GetFileContents().c_str());
    }
    else
    {
        _ReloadPhonemeMap();
    }
}

void ExtractLipSyncDialog::_ReloadPhonemeMap()
{
    // Try to load one. If we can't, show errors and load a default one.
    _phonemeMap = LoadPhonemeMapForViewLoop(appState, _nView, _nLoop);
    if (_phonemeMap->HasErrors() && _phonemeMap->GetFileContents().empty())
    {
        // couldn't even load the file. Show the deafult one?
        _phonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\default_phoneme_map.ini");
        m_wndEditPhonemeMapStatus.SetWindowText("Loaded a default phoneme map, as none exists for this view/loop");
    }
    else
    {
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
    m_wndGroupViewLoop.SetWindowText(fmt::format("View/Loop {0}/{1}:", _nView, _nLoop).c_str());
    m_wndEditPhonemeMap.SetWindowText(_phonemeMap->GetFileContents().c_str());
}

BOOL ExtractLipSyncDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    OnInitDialogHelper();
    ShowSizeGrip(FALSE);
    return fRet;
}

std::unique_ptr<ResourceEntity> ExtractLipSyncDialog::GetResult()
{
    return std::move(_audioResource);
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
    ON_BN_CLICKED(ID_GENERATELIPSYNC, &ExtractLipSyncDialog::OnBnClickedGeneratelipsync)
    ON_BN_CLICKED(IDC_BUTTON_RELOADMAPPING, &ExtractLipSyncDialog::OnBnClickedButtonReloadmapping)
    ON_BN_CLICKED(IDC_BUTTON_SETVIEW, &ExtractLipSyncDialog::OnBnClickedButtonSetview)
    ON_BN_CLICKED(IDC_CHECK_USESAMPLE, &ExtractLipSyncDialog::OnBnClickedCheckUsesample)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_SYNC, &ExtractLipSyncDialog::OnBnClickedButtonDeleteSync)
    ON_BN_CLICKED(IDC_BUTTON_RAW, &ExtractLipSyncDialog::OnBnClickedButtonRaw)
    ON_BN_CLICKED(IDC_BUTTON_EXPORTSYNC, &ExtractLipSyncDialog::OnBnClickedButtonExportsync)
    ON_BN_CLICKED(IDC_BUTTON_IMPORTSYNC, &ExtractLipSyncDialog::OnBnClickedButtonImportsync)
    ON_BN_CLICKED(IDC_EDITAUDIO, &ExtractLipSyncDialog::OnBnClickedEditaudio)
    ON_BN_CLICKED(IDC_EDITPHONEMEMAP, &ExtractLipSyncDialog::OnBnClickedEditphonememap)
    ON_BN_CLICKED(IDC_BUTTON_RESETMAPPING2, &ExtractLipSyncDialog::OnBnClickedButtonOpenmapping)
END_MESSAGE_MAP()

void ExtractLipSyncDialog::OnBnClickedButtonResetmapping()
{
    if (IDYES == AfxMessageBox("Replace the current mapping with the template?", MB_YESNO))
    {
        _phonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\default_phoneme_map.ini");
        m_wndEditPhonemeMap.SetWindowText(_phonemeMap->GetFileContents().c_str());
    }
}

LipSyncDialogTaskResult CreateLipSyncComponentAndRawDataFromAudioAndPhonemes(const AudioComponent &audio, const std::string &optionalText, const PhonemeMap &phonemeMap)
{
    std::vector<alignment_result> rawResults;
    LipSyncDialogTaskResult result;
    std::unique_ptr<SyncComponent> syncComponent = CreateLipSyncComponentFromAudioAndPhonemes(audio, optionalText, phonemeMap, &rawResults);
    if (syncComponent)
    {
        result = { *syncComponent, rawResults };
    }
    return result;
}

void ExtractLipSyncDialog::OnBnClickedGeneratelipsync()
{
    AudioComponent audioCopy;
    if (_audioResource->TryGetComponent<AudioNegativeComponent>())
    {
        // If we have an audio negative, use it for generating lipsync data. The data is higher quality when generated
        // with sixteen bit audio.
        ProcessSound(_audioResource->GetComponent<AudioNegativeComponent>(), audioCopy, AudioFlags::SixteenBit);
    }
    else
    {
        audioCopy = _audioResource->GetComponent<AudioComponent>();
    }

    if (_phonemeMap)
    {
        if (!_phonemeMap->IsEmpty() ||
            (IDYES == AfxMessageBox("The phoneme map appears to be all zeroes. Continue anyway?", MB_YESNO | MB_ICONWARNING)))
        {
            std::string optionalText;
            if (m_wndUseText.GetCheck() == BST_CHECKED)
            {
                optionalText = _messageText;
            }
            m_wndLipSyncButton.EnableWindow(FALSE);
            PhonemeMap phonemeMapCopy = *_phonemeMap;
            _taskSink->StartTask(
                [audioCopy, optionalText, phonemeMapCopy]() { return CreateLipSyncComponentAndRawDataFromAudioAndPhonemes(audioCopy, optionalText, phonemeMapCopy); }
            );
            m_wndProgress.SendMessage(PBM_SETMARQUEE, TRUE, LipSyncMarqueeMilliseconds);
        }
    }
}

void ExtractLipSyncDialog::OnBnClickedButtonReloadmapping()
{
    _ReloadPhonemeMap();
}


void ExtractLipSyncDialog::OnBnClickedButtonSetview()
{
    ChooseTalkerViewLoopDialog dialog(_talker, _nView, _nLoop);
    if (IDOK == dialog.DoModal())
    {
        bool dirty = _wantToUseSample || (_nView != dialog.GetView()) || (_nLoop != dialog.GetLoop());
        if (dirty)
        {
            _nView = dialog.GetView();
            _nLoop = dialog.GetLoop();
            _wantToUseSample = false;
            m_wndUseSample.SetCheck(BST_UNCHECKED);
            appState->GetResourceMap().GetTalkerToViewMap().SetTalkerToViewLoop(_talker, _nView, _nLoop);
            _SyncViewLoop();
        }
    }
}


void ExtractLipSyncDialog::OnBnClickedCheckUsesample()
{
    _wantToUseSample = (m_wndUseSample.GetCheck() == BST_CHECKED);
    _SyncViewLoop();
}


void ExtractLipSyncDialog::OnBnClickedButtonDeleteSync()
{
    if (IDYES == AfxMessageBox("Delete this message's lip sync data?", MB_YESNO))
    {
        _audioResource->RemoveComponent<SyncComponent>();
        std::vector<alignment_result> empty;
        m_wndWaveform.SetRawLipSyncData(empty);
        _UpdateWords(empty);
        _UpdateSyncList();
    }
}


void ExtractLipSyncDialog::OnBnClickedButtonRaw()
{
    ShowTextFile(_audioResource->GetComponent<SyncComponent>().RawData.c_str(), fmt::format("{0}_rawlipsync.txt", default_reskey(_audioResource->ResourceNumber, _audioResource->Base36Number)));
}

const char c_szLipSyncTxtFilter[] = "txt files (*.txt)|*.txt|All Files|*.*|";

void ExtractLipSyncDialog::OnBnClickedButtonExportsync()
{
    if (_audioResource->TryGetComponent<SyncComponent>())
    {
        CFileDialog fileDialog(FALSE, ".txt", fmt::format("{0}_lipsync.txt", default_reskey(_audioResource->ResourceNumber, _audioResource->Base36Number)).c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, c_szLipSyncTxtFilter);
        if (IDOK == fileDialog.DoModal())
        {
            CString strFileName = fileDialog.GetPathName();
            SyncToFile(*_audioResource->TryGetComponent<SyncComponent>(), (PCSTR)strFileName);
        }
    }
}

void ExtractLipSyncDialog::OnBnClickedButtonImportsync()
{
    CFileDialog fileDialog(TRUE, ".txt", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, c_szLipSyncTxtFilter);
    if (IDOK == fileDialog.DoModal())
    {
        CString strFileName = fileDialog.GetPathName();
        SyncComponent sync;
        if (SyncFromFile(sync, (PCSTR)strFileName))
        {
            _audioResource->AddComponent(std::make_unique<SyncComponent>(sync));
            _UpdateSyncList();
        }
    }
}


void ExtractLipSyncDialog::OnBnClickedEditaudio()
{
    if (_audioResource)
    {
        _audioPlayback.Stop();
        AudioEditDialog dialog(*_audioResource);
        if (IDOK == dialog.DoModal())
        {
            m_wndWaveform.SetResource(_audioResource->TryGetComponent<AudioComponent>());
        }
    }
}


void ExtractLipSyncDialog::OnBnClickedEditphonememap()
{
    if (_phonemeMap && _viewResource && !_actuallyUsingSample)
    {
        PhonemeMap copy = *_phonemeMap;
        PhonemeDialog dialog(_nView, _nLoop, copy);
        if (dialog.DoModal() == IDOK)
        {
            // Update the phoneme map
            *_phonemeMap = copy;

            // And save it...
            std::string errors;
            CString strText;
            std::string message = "Saving ";
            message += GetPhonemeMapFilespec(appState, _nView, _nLoop);
            if (!SaveForViewLoop(*_phonemeMap, appState, _nView, _nLoop, errors))
            {
                message += " : ";
                message += errors;
            }
            m_wndEditPhonemeMapStatus.SetWindowText(message.c_str());

            OnBnClickedButtonReloadmapping(); // And reload...
        }
    }
}

void ExtractLipSyncDialog::OnBnClickedButtonOpenmapping()
{
    if (_phonemeMap)
    {
        std::string fullPath = GetPhonemeMapPath(appState, _nView, _nLoop);
        ShowFile(fullPath);
    }
}
