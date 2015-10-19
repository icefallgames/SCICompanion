#include "stdafx.h"
#include "AppState.h"
#include "MessageEditPane.h"
#include "MessageDoc.h"
#include "Text.h"
#include "Message.h"
#include "CObjectWrap.h"
#include "format.h"
#include "MessageSource.h"
#include "WindowsUtil.h"
#include "NounsAndCases.h"
#include "AudioMap.h"
#include "AudioPlayback.h"
#include "Audio.h"
#include "ExtractLipSyncDialog.h"
#include "PhonemeMap.h"
#include "Sync.h"
#include "LipSyncUtil.h"
#include "AudioProcessing.h"
#include "AudioNegative.h"
#include "AudioEditDialog.h"

#define MOUTH_TIMER 9753

#define UM_LIPSYNCDONE        (WM_APP + 0)

using namespace std;

MessageEditPane::MessageEditPane(CWnd* pParent /*=NULL*/)
    : AudioPlaybackUI<CExtDialogFwdCmd>(IDD, pParent, true), _hAccel(nullptr), _initialized(false), _verbEdited(false), _nounEdited(false), _conditionEdited(false), _talkerEdited(false),
    _lipSyncTaskSink(this, UM_LIPSYNCDONE)

{
    // Load our accelerators
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORMESSAGEEDIT), RT_ACCELERATOR);
    _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORMESSAGEEDIT));
}

MessageEditPane::~MessageEditPane()
{
}

void MessageEditPane::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    if (!_initialized)
    {
        _initialized = true;

        DoDataExchangeHelper(pDX);

        DDX_Control(pDX, IDC_CHECK_USETEXT, m_wndUseText);
        m_wndUseText.SetCheck(BST_UNCHECKED);    // Textless by default, the timing is more accurate.
        DDX_Control(pDX, IDC_COMBONOUN, m_wndComboNoun);
        DDX_Control(pDX, IDC_COMBOVERB, m_wndComboVerb);
        DDX_Control(pDX, IDC_COMBOTALKER, m_wndComboTalker);
        DDX_Control(pDX, IDC_COMBOCONDITION, m_wndComboCondition);

        DDX_Control(pDX, IDC_EDITMESSAGE, m_wndEditMessage);
        DDX_Control(pDX, IDC_EDITSEQ, m_wndEditSequence);

        DDX_Control(pDX, IDC_STATIC3, m_wndLabel1);
        DDX_Control(pDX, IDC_STATIC6, m_wndLabel2);
        DDX_Control(pDX, IDC_STATIC7, m_wndLabel3);
        DDX_Control(pDX, IDC_STATIC8, m_wndLabel4);
        DDX_Control(pDX, IDC_STATIC9, m_wndLabel5);
        DDX_Control(pDX, IDC_STATIC2, m_wndLabel6);
        DDX_Control(pDX, IDC_STATIC_REC, m_wndStaticRec);

        DDX_Control(pDX, IDC_BUTTONADDSEQ, m_wndButton1);
        DDX_Control(pDX, IDC_BUTTONCLONE, m_wndButton2);
        DDX_Control(pDX, IDC_BUTTONADDNEW, m_wndButton3);
        
        DDX_Control(pDX, IDC_BUTTONCOMMIT, m_wndButtonFakeCommit);

        DDX_Control(pDX, IDC_SPINSEQ, m_wndSpinner);

        DDX_Control(pDX, IDC_EDIT_SAMPLEBIT, m_wndInfo);
        DDX_Control(pDX, IDC_BUTTON_PLAY2, m_wndPlay);
        DDX_Control(pDX, IDC_BUTTON_STOP, m_wndStop);
        DDX_Control(pDX, IDC_CHECK_AUTOPREV, m_wndAutoPreview);
        DDX_Control(pDX, IDC_GROUP_AUDIO, m_wndAudioGroup);

        DDX_Control(pDX, IDC_ANIMATE, m_wndMouth);
        m_wndMouth.SetBackground(g_PaintManager->GetColor(COLOR_BTNFACE));
        m_wndMouth.SetFillArea(true);
        SetMouthElement(&m_wndMouth);

        DDX_Control(pDX, IDC_BUTTONLIPSYNC, m_wndQuickLipSync);
        DDX_Control(pDX, IDC_BUTTONLIPSYNC_DIALOG, m_wndLipSyncDialog);

        DDX_Control(pDX, IDC_STATIC_BASE36NAME, m_wndLabelBase36);

        DDX_Control(pDX, IDC_BUTTONDELETEAUDIO, m_wndDeleteAudio);
        m_wndDeleteAudio.SetIcon(IDI_DELETE, 0, 0, 0, 16, 16);

        DDX_Control(pDX, IDC_EDITAUDIO, m_wndEditAudio);
        m_wndEditAudio.SetIcon(IDI_WAVEFORM, 0, 0, 0, 24, 24);
    }
    DDX_Text(pDX, IDC_EDITSEQ, _spinnerValue);
}

BEGIN_MESSAGE_MAP(MessageEditPane, AudioPlaybackUI<CExtDialogFwdCmd>)
    ON_MESSAGE(UM_LIPSYNCDONE, _OnLipSyncDone)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_EN_CHANGE(IDC_EDITMESSAGE, &MessageEditPane::OnEnChangeEditmessage)
    ON_CBN_SELCHANGE(IDC_COMBONOUN, &MessageEditPane::OnCbnSelchangeCombonoun)
    ON_CBN_SELCHANGE(IDC_COMBOVERB, &MessageEditPane::OnCbnSelchangeComboverb)
    ON_EN_CHANGE(IDC_EDITSEQ, &MessageEditPane::OnEnChangeEditseq)
    ON_EN_KILLFOCUS(IDC_EDITMESSAGE, &MessageEditPane::OnEnKillfocusEditmessage)
    ON_CBN_SELCHANGE(IDC_COMBOCONDITION, &MessageEditPane::OnCbnSelchangeCombocondition)
    ON_CBN_SELCHANGE(IDC_COMBOTALKER, &MessageEditPane::OnCbnSelchangeCombotalker)
    ON_CBN_EDITCHANGE(IDC_COMBOVERB, &MessageEditPane::OnCbnEditchangeComboverb)
    ON_CBN_KILLFOCUS(IDC_COMBOVERB, &MessageEditPane::OnCbnKillfocusComboverb)
    ON_CBN_SETFOCUS(IDC_COMBOVERB, &MessageEditPane::OnCbnSetfocusComboverb)
    ON_CBN_SETFOCUS(IDC_COMBONOUN, &MessageEditPane::OnCbnSetfocusCombonoun)
    ON_CBN_KILLFOCUS(IDC_COMBONOUN, &MessageEditPane::OnCbnKillfocusCombonoun)
    ON_CBN_EDITCHANGE(IDC_COMBONOUN, &MessageEditPane::OnCbnEditchangeCombonoun)
    ON_CBN_SETFOCUS(IDC_COMBOCONDITION, &MessageEditPane::OnCbnSetfocusCombocondition)
    ON_CBN_KILLFOCUS(IDC_COMBOCONDITION, &MessageEditPane::OnCbnKillfocusCombocondition)
    ON_CBN_EDITCHANGE(IDC_COMBOCONDITION, &MessageEditPane::OnCbnEditchangeCombocondition)
    ON_CBN_SETFOCUS(IDC_COMBOTALKER, &MessageEditPane::OnCbnSetfocusCombotalker)
    ON_CBN_KILLFOCUS(IDC_COMBOTALKER, &MessageEditPane::OnCbnKillfocusCombotalker)
    ON_CBN_EDITCHANGE(IDC_COMBOTALKER, &MessageEditPane::OnCbnEditchangeCombotalker)
    ON_BN_CLICKED(IDC_BUTTONLIPSYNC, &MessageEditPane::OnBnClickedButtonlipsync)
    ON_BN_CLICKED(IDC_BUTTONLIPSYNC_DIALOG, &MessageEditPane::OnBnClickedButtonlipsyncDialog)
    ON_BN_CLICKED(IDC_BUTTONDELETEAUDIO, &MessageEditPane::OnBnClickedButtondeleteaudio)
    ON_BN_CLICKED(IDC_EDITAUDIO, &MessageEditPane::OnBnClickedEditaudio)
END_MESSAGE_MAP()

void MessageEditPane::OnPlayAudio()
{
    _audioPlayback.Play();
}

BOOL MessageEditPane::OnEraseBkgnd(CDC *pDC)
{
    return __super::OnEraseBkgnd(pDC);
}

void _PopulateComboFromMessageSource(CExtComboBox &wndCombo, const MessageSource *source, bool skipPrefix)
{
    wndCombo.SetRedraw(FALSE);
    CString beforeText;
    int saveSel = wndCombo.GetCurSel();
    if (saveSel != -1)
    {
        wndCombo.GetLBText(saveSel, beforeText);
    }
    wndCombo.ResetContent();
    if (source)
    {
        for (auto &define : source->GetDefines())
        {
            PCSTR psz = define.first.c_str();
            if (skipPrefix && (define.first.size() > 2))
            {
                psz += 2;
            }
            wndCombo.InsertString(-1, psz);
        }
    }
    wndCombo.SetRedraw(TRUE);
}

// MessageEditPane message handlers

//
// To properly handle commands we do two things here:
// 1) Process our own accelerators
//      - The main frame's accelerator table doesn't get used in dialogs in control bars,
//        so we need to do our own.  So, e.g. Ctrl-Z becomes ID_EDIT_UNDO
// 2) Fwd any commands we receive to our frame
//      if we don't handle them ourselves.
//      - they don't bubble up naturally in dialogs in control bars.
//      - we do this by inheriting from CExtDialogFwdCmd
//
BOOL MessageEditPane::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (GetFocus() == static_cast<CWnd*>(&m_wndEditMessage))
    {
        if (!fRet)
        {
            fRet = HandleEditBoxCommands(pMsg, m_wndEditMessage);
        }
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

BOOL MessageEditPane::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();

    OnInitDialogHelper();

    // Set up anchoring for resize
    AddAnchor(IDC_EDITMESSAGE, CPoint(0, 0), CPoint(100, 100));

    // Recording functionality:
    AddAnchor(IDC_EDIT_SAMPLEBIT, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTON_PLAY2, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTON_STOP, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTONBROWSE, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTON_EXPORT, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTON_RECORD, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_SLIDER, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_CHECK_AUTOPREV, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_STATIC_DURATION, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_COMBO_WAVEFORMAT, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTONLIPSYNC, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_ANIMATE, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTONLIPSYNC_DIALOG, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_STATIC_BASE36NAME, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_STATIC_REC, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_CHECK_USETEXT, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTONDELETEAUDIO, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_EDITAUDIO, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_STATIC_CLIPPED, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_GROUP_AUDIO, CPoint(100, 0), CPoint(100, 0));

    // Hide the sizing grip
    ShowSizeGrip(FALSE);
    return fRet;
}

void MessageEditPane::_UpdateCombos(MessageChangeHint hint)
{
    if (_pDoc)
    {
        const ResourceEntity *resource = _pDoc->GetResource();
        if (IsFlagSet(hint, MessageChangeHint::NounsChanged))
        {
            _PopulateComboFromMessageSource(m_wndComboNoun, resource ? &resource->GetComponent<NounsAndCasesComponent>().GetNouns() : nullptr, true);
        }
        if (IsFlagSet(hint, MessageChangeHint::VerbsChanged))
        {
            _PopulateComboFromMessageSource(m_wndComboVerb, appState->GetResourceMap().GetVerbsMessageSource(), true);
        }
        if (IsFlagSet(hint, MessageChangeHint::TalkersChanged))
        {
            _PopulateComboFromMessageSource(m_wndComboTalker, appState->GetResourceMap().GetTalkersMessageSource(), false);
        }
        if (IsFlagSet(hint, MessageChangeHint::ConditionsChanged))
        {
            _PopulateComboFromMessageSource(m_wndComboCondition, resource ? &resource->GetComponent<NounsAndCasesComponent>().GetCases() : nullptr, true);
        }
    }
}

void MessageEditPane::UpdateNonView(CObject *pObject)
{
    // TODO: Provide more specific update mechanism
    MessageChangeHint hint = GetHint<MessageChangeHint>(pObject);
    _UpdateCombos(hint);

    if (IsFlagSet(hint, MessageChangeHint::Selection | MessageChangeHint::AllMessageFiles | MessageChangeHint::ItemChanged | MessageChangeHint::Changed))
    {
        _Update();
    }
}

const TextComponent *MessageEditPane::_GetResource()
{
    const TextComponent *text = nullptr;
    if (_pDoc)
    {
        const ResourceEntity *entity = _pDoc->GetResource();
        if (entity)
        {
            text = entity->TryGetComponent<TextComponent>();
        }
    }
    return text;
}

const TextEntry *MessageEditPane::_GetEntry()
{
    const TextEntry *entry = nullptr;
    const TextComponent *text = _GetResource();
    if (text)
    {
        int index = _GetSelectedIndex();
        if ((index != -1) && (index < (int)text->Texts.size()))
        {
            entry = &text->Texts[index];
        }
    }
    return entry;
}

int MessageEditPane::_GetSelectedIndex()
{
    if (_pDoc)
    {
        return _pDoc->GetSelectedIndex();
    }
    return -1;
}

void _UpdateComboFromValue(CExtComboBox &wndCombo, int value, const MessageSource *source)
{
    if (source)
    {
        if (value == 0)
        {
            wndCombo.SetCurSel(-1);
            wndCombo.SetWindowTextA("0");
        }
        else
        {
            int index = source->IndexOf((uint16_t)value);
            if (index == -1)
            {
                string valueText = fmt::format("{0}", value);
                wndCombo.SetWindowTextA(valueText.c_str());
            }
            else
            {
                wndCombo.SetCurSel(index);
            }
        }
    }
    else
    {
        wndCombo.SetCurSel(-1);
        wndCombo.SetWindowTextA("0");
    }
}

void MessageEditPane::_UpdateSequence(int sequence)
{
    _spinnerValue = sequence;
    UpdateData(false);
}

bool MessageEditPane::_UpdateAudio(const TextEntry &messageEntry)
{
    bool hasAudio = false;
    if (appState->GetVersion().HasSyncResources)
    {
        uint32_t tuple = GetMessageTuple(messageEntry);
        _audioPlayback.Stop();

        ResourceEntity *audioEntity = _pDoc->GetAudioResource();
        hasAudio = !!audioEntity;
        SetAudioResource(audioEntity);

        // TODO: Make this part of ResourceMap so we only load once?
        // TODO: Optimize all this resource loading.
        uint16_t view, loop;
        int currentMouthView = _mouthView ? _mouthView->ResourceNumber : -1;
        if (appState->GetResourceMap().GetTalkerToViewMap().TalkerToViewLoop(messageEntry.Talker, view, loop))
        {
            if (view != currentMouthView)
            {
                _mouthView = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::View, view);
            }
        }
        else
        {
            if (-1 != currentMouthView)
            {
                // Clear it out so we provide the default one again...
                _mouthView.reset(nullptr);
            }
        }
        if (!_mouthView)
        {
            std::string mouthSampleFilename = appState->GetResourceMap().GetSamplesFolder() + "\\views\\MouthShapes.bin";
            ResourceBlob blob;
            if (SUCCEEDED(blob.CreateFromFile("it's a mouth", mouthSampleFilename, sciVersion1_1, -1, -1)))
            {
                _mouthView = CreateResourceFromResourceData(blob);
            }
        }

        _mouthLoop = loop;
        _mouthCel = 0;

        if (_mouthView)
        {
            m_wndMouth.SetResource(_mouthView.get());
            m_wndMouth.SetLoop(_mouthLoop);
            m_wndMouth.SetCel(_mouthCel);
        }

        m_wndLabelBase36.SetWindowText(default_reskey(_pDoc->GetNumber(), tuple).c_str());
    }
    else
    {
        m_wndLabelBase36.SetWindowText("");
    }
    return hasAudio;
}

void MessageEditPane::OnNewResourceCreated(std::unique_ptr<ResourceEntity> audioResource, const std::string &name, bool isRecording)
{
    const TextEntry *entry = _GetEntry();
    if (entry)
    {
        assert(audioResource->SourceFlags == ResourceSourceFlags::AudioCache);
        audioResource->Base36Number = GetMessageTuple(*entry);
        audioResource->ResourceNumber = _pDoc->GetNumber();

        ResourceEntity *existing = _pDoc->GetAudioResource();
        if (!isRecording && existing && existing->TryGetComponent<SyncComponent>())
        {
            // If there is an existing lipsync component, ask if the user wants to remove it.
            // A scenario where don't want to remove it would be if they are exporting the wave for audio prcessing, and
            // then re-importing it.
            if (IDYES == AfxMessageBox("The audio resource you're replacing has lip sync data. Do you want to keep this lip sync data?", MB_YESNO | MB_ICONQUESTION))
            {
                audioResource->AddComponent<SyncComponent>(std::make_unique<SyncComponent>(existing->GetComponent<SyncComponent>()));
            }
        }
        _pDoc->SetAudioResource(std::move(audioResource));

        // Now we may have just deleted the old one, so let's update.
        _Update();
    }
}

void MessageEditPane::_Update()
{
    bool hasAudio = false;
    if (_pDoc)
    {
        CWnd *controls[] = { &m_wndEditMessage };
        const TextEntry *entry = _GetEntry();
        for (CWnd *control : controls)
        {
            control->EnableWindow(entry != nullptr);
        }

        if (entry)
        {
            m_wndEditMessage.SetWindowTextA(entry->Text.c_str());
            _UpdateSequence(entry->Sequence);
            
            const NounsAndCasesComponent &nounsAndCases = _pDoc->GetResource()->GetComponent<NounsAndCasesComponent>();
            _UpdateComboFromValue(m_wndComboVerb, entry->Verb, appState->GetResourceMap().GetVerbsMessageSource());
            _UpdateComboFromValue(m_wndComboTalker, entry->Talker, appState->GetResourceMap().GetTalkersMessageSource());
            _UpdateComboFromValue(m_wndComboNoun, entry->Noun, &nounsAndCases.GetNouns());
            _UpdateComboFromValue(m_wndComboCondition, entry->Condition, &nounsAndCases.GetCases());

            SetAllowRecording(true);
            hasAudio = _UpdateAudio(*entry);
            if (hasAudio)
            {
                // Enable the edit button if there's a negative
                m_wndEditAudio.EnableWindow(_pDoc->GetAudioResource()->TryGetComponent<AudioNegativeComponent>() ? TRUE : FALSE);
            }
        }
        else
        {
            SetAllowRecording(false);
            m_wndEditMessage.SetWindowTextA("");
            m_wndLabelBase36.SetWindowText("");
        }
    }

    // Hide/show audio controls when appropriate.
    int cmdShowSupportsSync = appState->GetVersion().HasSyncResources ? SW_SHOW : SW_HIDE;
    int cmdShowHasAudio = hasAudio ? SW_SHOW : SW_HIDE;

    m_wndInfo.ShowWindow(cmdShowHasAudio);
    m_wndPlay.ShowWindow(cmdShowHasAudio);
    m_wndStop.ShowWindow(cmdShowSupportsSync);
    m_wndStaticRec.ShowWindow(cmdShowSupportsSync);
    m_wndRecord.ShowWindow(cmdShowSupportsSync);
    m_wndSlider.ShowWindow(cmdShowHasAudio);
    m_wndAutoPreview.ShowWindow(cmdShowHasAudio);
    m_wndDuration.ShowWindow(cmdShowHasAudio);
    m_wndBrowse.ShowWindow(cmdShowSupportsSync);
    m_wndExport.ShowWindow(cmdShowHasAudio);
    m_wndWaveType.ShowWindow(cmdShowSupportsSync);
    m_wndMouth.ShowWindow(cmdShowHasAudio);
    m_wndQuickLipSync.ShowWindow(cmdShowHasAudio);
    m_wndLipSyncDialog.ShowWindow(cmdShowHasAudio);
    m_wndUseText.ShowWindow(cmdShowHasAudio);
    m_wndDeleteAudio.ShowWindow(cmdShowHasAudio);
    m_wndEditAudio.ShowWindow(cmdShowHasAudio);
    m_wndAudioGroup.ShowWindow(cmdShowSupportsSync);
}

void MessageEditPane::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CMessageDoc*>(pDoc);
    if (_pDoc)
    {

        _pDoc->AddNonViewClient(this);
        _UpdateCombos(MessageChangeHint::ConditionsChanged | MessageChangeHint::TalkersChanged | MessageChangeHint::VerbsChanged | MessageChangeHint::NounsChanged);
        _Update();
    }
}

void MessageEditPane::OnEnChangeEditmessage()
{
    // Validation happens on focus lost, so nothing to do here.
}

bool _GetValueFromText(CComboBox &wndCombo, bool isSelChange, const MessageSource *source, uint8_t &finalValue)
{
    bool success = false;
    if (source)
    {
        CString temp;
        if (isSelChange && (wndCombo.GetCurSel() != -1))
        {
            wndCombo.GetLBText(wndCombo.GetCurSel(), temp);
        }
        else
        {
            wndCombo.GetWindowText(temp);
        }
        string rawResult = (PCSTR)temp;
        string result = rawResult;
        if (result.compare(0, source->MandatoryPrefix.size(), source->MandatoryPrefix) != 0)
        {
            result = source->MandatoryPrefix + result;
        }
        int index = source->IndexOf(result);
        if (index == -1)
        {
            // Doesn't match anything. Is it a number?
            try
            {
                size_t end;
                int valueFromString = stoi(rawResult, &end);
                if ((end == rawResult.length()) && (valueFromString >= 0) && (valueFromString < 256))
                {
                    // Accept this as a number.
                    success = true;
                    finalValue = (uint8_t)valueFromString;
                }
            }
            catch (std::invalid_argument)
            {

            }
        }
        else
        {
            success = true;
            finalValue = (uint8_t)source->GetDefines()[index].second;
        }
    }
    return success;
}

template<typename _FGetProperty>
void _RespondToComboChange(MessageEditPane *mep, CComboBox &wndCombo, MessageSourceType sourceType, _FGetProperty getProperty, bool isSelChange)
{
    CMessageDoc *pDoc = mep->GetDocument();
    if (pDoc)
    {
        const MessageSource *source = GetMessageSourceFromType(pDoc, sourceType, false);
        uint8_t value;
        if (_GetValueFromText(wndCombo, isSelChange, source, value))
        {
            const TextEntry *entry = mep->_GetEntry();
            if (entry)
            {
                TextEntry copy = *entry;
                uint8_t &property = getProperty(copy);
                property = value;
                pDoc->SetEntry(copy);
            }
        }
        else
        {
            mep->_Update();
        }
    }
}

void MessageEditPane::OnEnChangeEditseq()
{
    if (_pDoc)
    {
        UpdateData(true);
        if (_spinnerValue < 1)
        {
            _spinnerValue = 1;
            UpdateData(false);
        }
        if (_spinnerValue > 63)
        {
            // Only 6 bits allowed, since the top two can be used to indicate lip sync data is present.
            _spinnerValue = 63;
            UpdateData(false);
        }

        const TextEntry *entry = _GetEntry();
        if (entry)
        {
            TextEntry copy = *entry;
            copy.Sequence = this->_spinnerValue;
            _pDoc->SetEntry(copy);
        }
    }
}

void MessageEditPane::OnEnKillfocusEditmessage()
{
    if (_pDoc)
    {
        CString strText;
        this->m_wndEditMessage.GetWindowTextA(strText);
        const TextEntry *entry = _GetEntry();
        if (entry)
        {
            TextEntry copy = *entry;
            copy.Text = (PCSTR)strText;
            _pDoc->SetEntry(copy);
        }
    }
}

uint8_t &GetNounProperty(TextEntry &entry) { return entry.Noun; }

void MessageEditPane::OnCbnSelchangeCombonoun()
{
    _RespondToComboChange(this, m_wndComboNoun, MessageSourceType::Nouns, GetNounProperty, true);
}

uint8_t &GetVerbProperty(TextEntry &entry) { return entry.Verb; }

void MessageEditPane::OnCbnSelchangeComboverb()
{
    _RespondToComboChange(this, m_wndComboVerb, MessageSourceType::Verbs, GetVerbProperty, true);
}

uint8_t &GetConditionProperty(TextEntry &entry) { return entry.Condition; }

void MessageEditPane::OnCbnSelchangeCombocondition()
{
    _RespondToComboChange(this, m_wndComboCondition, MessageSourceType::Conditions, GetConditionProperty, true);
}

uint8_t &GetTalkerProperty(TextEntry &entry) { return entry.Talker; }

void MessageEditPane::OnCbnSelchangeCombotalker()
{
    _RespondToComboChange(this, m_wndComboTalker, MessageSourceType::Talkers, GetTalkerProperty, true);
}

void MessageEditPane::OnCbnSetfocusComboverb()
{
    _verbEdited = false;
}
void MessageEditPane::OnCbnKillfocusComboverb()
{
    if (_verbEdited)
    {
        _RespondToComboChange(this, m_wndComboVerb, MessageSourceType::Verbs, GetVerbProperty, false);
    }
}
void MessageEditPane::OnCbnEditchangeComboverb()
{
    _verbEdited = true;
}

void MessageEditPane::OnCbnSetfocusCombonoun()
{
    _nounEdited = false;
}
void MessageEditPane::OnCbnKillfocusCombonoun()
{
    if (_nounEdited)
    {
        _RespondToComboChange(this, m_wndComboNoun, MessageSourceType::Nouns, GetNounProperty, false);
    }
}
void MessageEditPane::OnCbnEditchangeCombonoun()
{
    _nounEdited = true;
}


void MessageEditPane::OnCbnSetfocusCombocondition()
{
    _conditionEdited = false;
}
void MessageEditPane::OnCbnKillfocusCombocondition()
{
    if (_conditionEdited)
    {
        _RespondToComboChange(this, m_wndComboCondition, MessageSourceType::Conditions, GetConditionProperty, false);
    }
}
void MessageEditPane::OnCbnEditchangeCombocondition()
{
    _conditionEdited = true;
}


void MessageEditPane::OnCbnSetfocusCombotalker()
{
    _talkerEdited = false;
}
void MessageEditPane::OnCbnKillfocusCombotalker()
{
    if (_talkerEdited)
    {
        _RespondToComboChange(this, m_wndComboTalker, MessageSourceType::Talkers, GetTalkerProperty, false);
    }
}
void MessageEditPane::OnCbnEditchangeCombotalker()
{
    _talkerEdited = true;
}

SyncComponent CreateLipSyncComponentFromAudioAndPhonemes2(const AudioComponent &audio, const std::string &optionalText, const PhonemeMap &phonemeMap)
{
    std::unique_ptr<SyncComponent> syncComponent = CreateLipSyncComponentFromAudioAndPhonemes(audio, optionalText, phonemeMap);
    if (syncComponent)
    {
        return *syncComponent;
    }
    return SyncComponent(); // empty one, failure
}

void MessageEditPane::OnBnClickedButtonlipsync()
{
    const ResourceEntity *resource = _pDoc->GetAudioResource();
    if (resource)
    {
        AudioComponent audioCopy;
        if (resource->TryGetComponent<AudioNegativeComponent>())
        {
            // If we have an audio negative, use it for generating lipsync data. The data is higher quality when generated
            // with sixteen bit audio.
            ProcessSound(resource->GetComponent<AudioNegativeComponent>(), audioCopy, AudioFlags::SixteenBit);
        }
        else
        {
            audioCopy = resource->GetComponent<AudioComponent>();
        }

        const TextEntry *entry = _GetEntry();
        std::string optionalText;
        if (m_wndUseText.GetCheck() == BST_CHECKED)
        {
            optionalText = entry->Text;
        }

        uint16_t view, loop;
        std::unique_ptr<PhonemeMap> phonemeMap;
        if (appState->GetResourceMap().GetTalkerToViewMap().TalkerToViewLoop(entry->Talker, view, loop))
        {
            phonemeMap = LoadPhonemeMapForViewLoop(appState, view, loop);
        }
        if (!phonemeMap || phonemeMap->HasErrors())
        {
            phonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\sample_phoneme_map.ini");
        }
        if (phonemeMap)
        {
            PhonemeMap phonemeMapCopy = *phonemeMap;
            m_wndQuickLipSync.EnableWindow(FALSE);
            _lipSyncTaskSink.StartTask(
                [audioCopy, optionalText, phonemeMapCopy]() { return CreateLipSyncComponentFromAudioAndPhonemes2(audioCopy, optionalText, phonemeMapCopy); }
            );
        }
    }
}

LRESULT MessageEditPane::_OnLipSyncDone(WPARAM wParam, LPARAM lParam)
{
    // TODO: Check for task failure
    m_wndQuickLipSync.EnableWindow(TRUE);

    // Update the sync component (or add one)
    std::unique_ptr<SyncComponent> syncComponent = std::make_unique<SyncComponent>(_lipSyncTaskSink.GetResponse());
    _pDoc->ModifyCurrentAudioResource(
        [&syncComponent](ResourceEntity &audioResource)
    {
        // AddComponent does a replace if that component already exists.
        audioResource.AddComponent<SyncComponent>(std::move(syncComponent));
        return true;
    }
        );
    _Update();
    return 0;
}

void MessageEditPane::OnBnClickedButtonlipsyncDialog()
{
    if (_pDoc->GetAudioResource())
    {
        const TextEntry *entry = _GetEntry();

        AutomaticStop();

        std::string talkerName;
        MessageSource *talkersSource = appState->GetResourceMap().GetTalkersMessageSource();
        if (talkersSource)
        {
            talkerName = talkersSource->ValueToName(entry->Talker);
        }




        // phil temp min max
        uint8_t maxV = 0;
        uint8_t minV = 255;
        for (uint8_t value : _pDoc->GetAudioResource()->GetComponent<AudioComponent>().DigitalSamplePCM)
        {
            maxV = max(maxV, value);
            minV = min(minV, value);
        }
        std::string foo = fmt::format("min:{0}  max:{1}\n", (int)minV, (int)maxV);
        OutputDebugString(foo.c_str());




        ExtractLipSyncDialog dialog(*_pDoc->GetAudioResource(), entry->Talker, talkerName, entry->Text, m_wndUseText.GetCheck() == BST_CHECKED);
        if (IDOK == dialog.DoModal())
        {
            // The lipsync dialog may have actually changed the audio too.
            _pDoc->SetAudioResource(std::move(dialog.GetResult()));
            _Update();
        }
    }
}

void MessageEditPane::OnBnClickedButtondeleteaudio()
{
    if (_pDoc)
    {
        if (IDYES == AfxMessageBox("Delete the audio associated with this entry?", MB_YESNO | MB_ICONWARNING))
        {
            _pDoc->SetAudioResource(nullptr);
        }
    }
}

void MessageEditPane::OnBnClickedEditaudio()
{
    _audioPlayback.Stop();
    bool modified = false;
    _pDoc->ModifyCurrentAudioResource(
        [&modified](ResourceEntity &audioResource)
    {
        AudioEditDialog dialog(audioResource);
        modified = (IDOK == dialog.DoModal());
        return modified;
    }
    );

    if (modified)
    {
        _Update();
    }
}
