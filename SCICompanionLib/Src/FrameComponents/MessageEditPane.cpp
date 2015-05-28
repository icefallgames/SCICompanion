#include "stdafx.h"
#include "AppState.h"
#include "MessageEditPane.h"
#include "MessageDoc.h"
#include "Text.h"
#include "Message.h"
#include "CObjectWrap.h"
#include "format.h"
#include "MessageSource.h"

MessageEditPane::MessageEditPane(CWnd* pParent /*=NULL*/)
    : CExtDialogFwdCmd(IDD, pParent), _hAccel(nullptr), _initialized(false)
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
    }
    DDX_Text(pDX, IDC_EDITSEQ, _spinnerValue);
}

BEGIN_MESSAGE_MAP(MessageEditPane, CExtDialogFwdCmd)
    ON_WM_DRAWITEM()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_BUTTONADDSEQ, &MessageEditPane::OnBnClickedButtonaddseq)
    ON_BN_CLICKED(IDC_BUTTONCLONE, &MessageEditPane::OnBnClickedButtonclone)
    ON_EN_CHANGE(IDC_EDITMESSAGE, &MessageEditPane::OnEnChangeEditmessage)
    ON_CBN_SELCHANGE(IDC_COMBONOUN, &MessageEditPane::OnCbnSelchangeCombonoun)
    ON_CBN_SELCHANGE(IDC_COMBOVERB, &MessageEditPane::OnCbnSelchangeComboverb)
    ON_EN_CHANGE(IDC_EDITSEQ, &MessageEditPane::OnEnChangeEditseq)
    ON_EN_KILLFOCUS(IDC_EDITMESSAGE, &MessageEditPane::OnEnKillfocusEditmessage)
    ON_CBN_SELCHANGE(IDC_COMBOCONDITION, &MessageEditPane::OnCbnSelchangeCombocondition)
    ON_CBN_SELCHANGE(IDC_COMBOTALKER, &MessageEditPane::OnCbnSelchangeCombotalker)
    ON_BN_CLICKED(IDC_BUTTONADDNEW, &MessageEditPane::OnBnClickedButtonaddnew)
END_MESSAGE_MAP()

BOOL MessageEditPane::OnEraseBkgnd(CDC *pDC)
{
    return __super::OnEraseBkgnd(pDC);
}

void _PopulateComboFromMessageSource(CExtComboBox &wndCombo, MessageSource *source, bool skipPrefix)
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
        wndCombo.InsertString(-1, "none");
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
#define VK_C        67
#define VK_V        86
#define VK_X        88
#define VK_Z        90

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
            if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST))
            {
                // Fwd the delete key to the edit control
                if ((pMsg->message != WM_CHAR) && (pMsg->wParam == VK_DELETE))
                {
                    ::SendMessage(m_wndEditMessage.GetSafeHwnd(), pMsg->message, pMsg->wParam, pMsg->lParam);
                    fRet = 1; // Don't dispatch message, we handled it.
                }
            }
        }
        if (!fRet)
        {
            if (pMsg->message == WM_KEYDOWN)
            {
                if (GetKeyState(VK_CONTROL))
                {
                    if (pMsg->wParam == VK_C)
                    {
                        m_wndEditMessage.Copy();
                        fRet = TRUE;
                    }
                    if (pMsg->wParam == VK_V)
                    {
                        m_wndEditMessage.Paste();
                        fRet = TRUE;
                    }
                    if (pMsg->wParam == VK_X)
                    {
                        m_wndEditMessage.Cut();
                        fRet = TRUE;
                    }
                    if (pMsg->wParam == VK_Z)
                    {
                        m_wndEditMessage.Undo();
                        fRet = TRUE;
                    }
                }
            }
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

    // Set up anchoring for resize
    AddAnchor(IDC_EDITMESSAGE, CPoint(0, 0), CPoint(100, 100));
    // Hide the sizing grip
    ShowSizeGrip(FALSE);
    return fRet;
}

void MessageEditPane::_UpdateCombos(MessageChangeHint hint)
{
    if (_pDoc)
    {
        if (IsFlagSet(hint, MessageChangeHint::NounsChanged))
        {
            _PopulateComboFromMessageSource(m_wndComboNoun, _pDoc->GetNounMessageSource(), true);
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
            _PopulateComboFromMessageSource(m_wndComboCondition, _pDoc->GetConditionMessageSource(), true);
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

void _UpdateComboFromValue(CExtComboBox &wndCombo, int value, MessageSource *source)
{
    if (source)
    {
        if (value == 0)
        {
            wndCombo.SetCurSel(0);
        }
        else
        {
            int index = source->IndexOf((uint16_t)value) + 1;
            wndCombo.SetCurSel(index);
        }
    }
    else
    {
        wndCombo.SetCurSel(0);
    }
}

void MessageEditPane::_UpdateSequence(int sequence)
{
    _spinnerValue = sequence;
    UpdateData(false);
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
            
            _UpdateComboFromValue(m_wndComboVerb, entry->Verb, appState->GetResourceMap().GetVerbsMessageSource());
            _UpdateComboFromValue(m_wndComboTalker, entry->Talker, appState->GetResourceMap().GetVerbsMessageSource());
            _UpdateComboFromValue(m_wndComboNoun, entry->Noun, _pDoc->GetNounMessageSource());
            _UpdateComboFromValue(m_wndComboCondition, entry->Condition, _pDoc->GetNounMessageSource());
        }
        else
        {
            // TODO: Clear everything out
            m_wndEditMessage.SetWindowTextA("");
        }
    }
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

void MessageEditPane::OnBnClickedButtonaddseq()
{
    const TextEntry *text = _GetEntry();
    if (text)
    {
        TextEntry newEntry = *text;
        newEntry.Sequence++;
        _AddEntryAtCurrentPosition(newEntry);
    }
}

void MessageEditPane::OnBnClickedButtonclone()
{
    const TextEntry *text = _GetEntry();
    if (text)
    {
        _AddEntryAtCurrentPosition(*text);
    }
}

void MessageEditPane::OnBnClickedButtonaddnew()
{
    // Default to the NARRATOR talker (usually 99), and sequence 1.
    TextEntry newEntry({ 0, 0, 0, 1, 99, 0, "" });
    _AddEntryAtCurrentPosition(newEntry);
}

void MessageEditPane::OnEnChangeEditmessage()
{
    // Validation happens on focus lost, so nothing to do here.
}

void MessageEditPane::OnCbnSelchangeCombonoun()
{
    if (_pDoc)
    {
        MessageSource *nounSource = _pDoc->GetNounMessageSource();
        if (nounSource)
        {
            int index = m_wndComboNoun.GetCurSel();
            index--;

            _pDoc->ApplyChanges<TextComponent>(
                [this, index, nounSource](TextComponent &text)
            {
                TextEntry *entry = this->_GetEntry(text);
                if ((index >= 0) && (index < (int)nounSource->GetDefines().size()))
                {
                    entry->Noun = (uint8_t)nounSource->GetDefines()[index].second;
                }
                else
                {
                    entry->Noun = 0;
                }
                return WrapHint(MessageChangeHint::ItemChanged);
            }
            );
        }
    }
}


void MessageEditPane::OnCbnSelchangeComboverb()
{
    if (_pDoc)
    {
        MessageSource *verbSource = appState->GetResourceMap().GetVerbsMessageSource();
        if (verbSource)
        {
            int index = m_wndComboVerb.GetCurSel();
            index--;

            _pDoc->ApplyChanges<TextComponent>(
                [this, index, verbSource](TextComponent &text)
            {
                TextEntry *entry = this->_GetEntry(text);
                if ((index >= 0) && (index < (int)verbSource->GetDefines().size()))
                {
                    entry->Verb = (uint8_t)verbSource->GetDefines()[index].second;
                }
                else
                {
                    entry->Verb = 0;
                }
                return WrapHint(MessageChangeHint::ItemChanged);
            }
            );
        }
    }
}

void MessageEditPane::OnEnChangeEditseq()
{
    if (_pDoc)
    {
        UpdateData(true);
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

void MessageEditPane::OnCbnSelchangeCombocondition()
{
    if (_pDoc)
    {
        MessageSource *condSource = _pDoc->GetConditionMessageSource();
        if (condSource)
        {
            int index = m_wndComboCondition.GetCurSel();
            index--;

            _pDoc->ApplyChanges<TextComponent>(
                [this, index, condSource](TextComponent &text)
            {
                TextEntry *entry = this->_GetEntry(text);
                if ((index >= 0) && (index < (int)condSource->GetDefines().size()))
                {
                    entry->Condition = (uint8_t)condSource->GetDefines()[index].second;
                }
                else
                {
                    entry->Condition = 0;
                }
                return WrapHint(MessageChangeHint::ItemChanged);
            }
            );
        }
    }
}


void MessageEditPane::OnCbnSelchangeCombotalker()
{
    if (_pDoc)
    {
        MessageSource *talkersSource = appState->GetResourceMap().GetTalkersMessageSource();
        if (talkersSource)
        {
            int index = m_wndComboTalker.GetCurSel();
            index--;

            _pDoc->ApplyChanges<TextComponent>(
                [this, index, talkersSource](TextComponent &text)
            {
                TextEntry *entry = this->_GetEntry(text);
                if ((index >= 0) && (index < (int)talkersSource->GetDefines().size()))
                {
                    entry->Talker = (uint8_t)talkersSource->GetDefines()[index].second;
                }
                else
                {
                    entry->Talker = 0;
                }
                return WrapHint(MessageChangeHint::ItemChanged);
            }
            );
        }
    }
}
