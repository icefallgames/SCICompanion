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

#define MOUTH_TIMER 9753

using namespace std;

MessageEditPane::MessageEditPane(CWnd* pParent /*=NULL*/)
    : AudioPlaybackUI<CExtDialogFwdCmd>(IDD, pParent), _hAccel(nullptr), _initialized(false), _verbEdited(false), _nounEdited(false), _conditionEdited(false), _talkerEdited(false)
{
    // Load our accelerators
    // HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS), RT_ACCELERATOR);
    // _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS));
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

        DDX_Control(pDX, IDC_BUTTONADDSEQ, m_wndButton1);
        DDX_Control(pDX, IDC_BUTTONCLONE, m_wndButton2);
        DDX_Control(pDX, IDC_BUTTONADDNEW, m_wndButton3);
        
        DDX_Control(pDX, IDC_BUTTONCOMMIT, m_wndButtonFakeCommit);

        DDX_Control(pDX, IDC_SPINSEQ, m_wndSpinner);

        DDX_Control(pDX, IDC_EDIT_SAMPLEBIT, m_wndInfo);
        DDX_Control(pDX, IDC_BUTTON_PLAY2, m_wndPlay);
        DDX_Control(pDX, IDC_BUTTON_STOP, m_wndStop);
        DDX_Control(pDX, IDC_CHECK_AUTOPREV, m_wndAutoPreview);

        DDX_Control(pDX, IDC_ANIMATE, m_wndMouth);
        m_wndMouth.SetBackground(g_PaintManager->GetColor(COLOR_BTNFACE));
        m_wndMouth.SetFillArea(true);
    }
    DDX_Text(pDX, IDC_EDITSEQ, _spinnerValue);
}

BEGIN_MESSAGE_MAP(MessageEditPane, AudioPlaybackUI<CExtDialogFwdCmd>)
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
END_MESSAGE_MAP()

void MessageEditPane::OnPlayAudio()
{
    g_audioPlayback.Play();
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
    AddAnchor(IDC_BUTTON_RECORD, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_SLIDER, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_CHECK_AUTOPREV, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_STATIC_DURATION, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_COMBO_WAVEFORMAT, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTONLIPSYNC, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_ANIMATE, CPoint(100, 0), CPoint(100, 0));

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

TextEntry *MessageEditPane::_GetEntry(TextComponent &text)
{
    TextEntry *entry = nullptr;
    int index = _GetSelectedIndex();
    if ((index != -1) && (index < (int)text.Texts.size()))
    {
        entry = &text.Texts[index];
    }
    return entry;
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

void MessageEditPane::_UpdateAudio(const TextEntry &messageEntry)
{
    if (appState->GetVersion().HasSyncResources)
    {
        uint32_t tuple = GetMessageTuple(messageEntry);
        g_audioPlayback.Stop();
        SetAudioResource(_pDoc->FindAudioResource(tuple));

        // phil
        std::unique_ptr<PhonemeMap> samplePhonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\sample_phoneme_map.ini");
        if (!_mouthView)
        {
            _mouthLoop = 0;
            _mouthCel = 0;

            std::string mouthSampleFilename = appState->GetResourceMap().GetSamplesFolder() + "\\views\\MouthShapes.bin";
            ResourceBlob blob;
            if (SUCCEEDED(blob.CreateFromFile("it's a mouth", mouthSampleFilename, sciVersion1_1, -1, -1)))
            {
                _mouthView = CreateResourceFromResourceData(blob);
            }
        }

        if (_mouthView)
        {
            m_wndMouth.SetResource(_mouthView.get(), nullptr);
            m_wndMouth.SetLoop(_mouthLoop);
            m_wndMouth.SetCel(_mouthCel);
        }
    }
}

void MessageEditPane::OnNewResourceCreated(std::unique_ptr<ResourceEntity> audioResource, const std::string &name)
{
    const TextEntry *entry = _GetEntry();
    if (entry)
    {
        audioResource->SourceFlags = (GetVolumeToUse(appState->GetVersion(), GetMessageTuple(*entry)) == AudioVolumeName::Sfx) ? ResourceSourceFlags::Sfx : ResourceSourceFlags::Aud;
        audioResource->Base36Number = GetMessageTuple(*entry);
        audioResource->ResourceNumber = _pDoc->GetNumber();

        // TODO: This is temporary. For now we'll add everything.
        _pDoc->AddNewAudioResource(std::move(audioResource));
        // Don't do this now. Instead, add to a list until we save?
        // appState->GetResourceMap().AppendResource(*audioResource);

        // Now we may have just deleted the old one, so let's update.
        _Update();
    }
}

void MessageEditPane::_Update()
{
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

            _UpdateAudio(*entry);
        }
        else
        {
            // TODO: Clear everything out
            m_wndEditMessage.SetWindowTextA("");
        }
    }

    // Hide/show audio controls when appropriate.
    int cmdShow = appState->GetVersion().HasSyncResources ? SW_SHOW : SW_HIDE;
    m_wndInfo.ShowWindow(cmdShow);
    m_wndPlay.ShowWindow(cmdShow);
    m_wndStop.ShowWindow(cmdShow);
    m_wndRecord.ShowWindow(cmdShow);
    m_wndSlider.ShowWindow(cmdShow);
    m_wndAutoPreview.ShowWindow(cmdShow);
    m_wndDuration.ShowWindow(cmdShow);
    m_wndBrowse.ShowWindow(cmdShow);
    m_wndWaveType.ShowWindow(cmdShow);
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

void MessageEditPane::_AddEntryAtCurrentPosition(const TextEntry &entry)
{
    if (_pDoc)
    {
        int index = _pDoc->GetSelectedIndex();
        index++;
        _pDoc->ApplyChanges<TextComponent>(
            [index, &entry](TextComponent &text)
        {
            text.Texts.insert(text.Texts.begin() + index, entry);
            return WrapHint(MessageChangeHint::Changed);
        }
        );

        // Now select it
        _pDoc->SetSelectedIndex(index);
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
            pDoc->ApplyChanges<TextComponent>(
                [mep, value, getProperty](TextComponent &text)
            {
                TextEntry *entry = mep->_GetEntry(text);
                if (entry)
                {
                    uint8_t &property = getProperty(*entry);
                    if (property != value)
                    {
                        property = value;
                        return WrapHint(MessageChangeHint::ItemChanged);
                    }
                }
                return WrapHint(MessageChangeHint::None);
            }
            );
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
        _pDoc->ApplyChanges<TextComponent>(
            [this](TextComponent &text)
        {
            TextEntry *entry = this->_GetEntry(text);
            entry->Sequence = this->_spinnerValue;
            return WrapHint(MessageChangeHint::ItemChanged);
        }
        );
    }
}

void MessageEditPane::OnEnKillfocusEditmessage()
{
    if (_pDoc)
    {
        _pDoc->ApplyChanges<TextComponent>(
            [this](TextComponent &text)
        {
            TextEntry *entry = this->_GetEntry(text);
            CString strText;
            this->m_wndEditMessage.GetWindowTextA(strText);
            MessageChangeHint hint = MessageChangeHint::None;
            if (entry->Text != (PCSTR)strText)
            {
                entry->Text = (PCSTR)strText;
                hint |= MessageChangeHint::ItemChanged;
            }
            return WrapHint(hint);
        }
            );
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

void MessageEditPane::OnBnClickedButtonlipsync()
{
    std::string tempWaveFilename = appState->GetResourceMap().Helper().GameFolder + "\\lipsync-temp.wav";
    if (_audio)
    {
        WriteWaveFile(tempWaveFilename, _audio->GetComponent<AudioComponent>());
        ExtractLipSyncDialog dialog(tempWaveFilename);
        if (IDCANCEL == dialog.DoModal()) // IDCANCEL temp
        {
            std::unique_ptr<PhonemeMap> samplePhonemeMap = std::make_unique<PhonemeMap>(GetExeSubFolder("Samples") + "\\sample_phoneme_map.ini");
            std::unique_ptr<SyncComponent> syncComponent = dialog.CreateLipSyncComponent(*samplePhonemeMap);

            // Update the sync component (or add one)
            const TextEntry *entry = _GetEntry();
            uint32_t tuple = GetMessageTuple(*entry);
            ResourceEntity *entity = _pDoc->FindAudioResource(tuple);
            entity->RemoveComponent<SyncComponent>();
            entity->AddComponent<SyncComponent>(std::move(syncComponent));
        }
    }
}

void MessageEditPane::OnPlaybackTimer()
{
    if (_audio)
    {
        const SyncComponent *syncComponent = _audio->TryGetComponent<SyncComponent>();
        if (syncComponent)
        {
            uint16_t tickPosition = (uint16_t)g_audioPlayback.QueryPosition(syncComponent->GetMaxTicks());
            uint16_t cel = syncComponent->GetCelAtTick(tickPosition);
            if (cel != 0xffff)
            {
                m_wndMouth.SetCel(cel, true);
            }
        }
    }
}
