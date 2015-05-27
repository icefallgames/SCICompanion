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
    : CExtDialogFwdCmd(IDD, pParent), _hAccel(nullptr)
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

    DDX_Control(pDX, IDC_BUTTONADDSEQ, m_wndButton1);
    DDX_Control(pDX, IDC_BUTTONCLONE, m_wndButton2);
    DDX_Control(pDX, IDC_SPINSEQ, m_wndSpinner);
    
}

BEGIN_MESSAGE_MAP(MessageEditPane, CExtDialogFwdCmd)
    ON_WM_DRAWITEM()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_BUTTONADDSEQ, &MessageEditPane::OnBnClickedButtonaddseq)
    ON_BN_CLICKED(IDC_BUTTONCLONE, &MessageEditPane::OnBnClickedButtonclone)
END_MESSAGE_MAP()

BOOL MessageEditPane::OnEraseBkgnd(CDC *pDC)
{
    return __super::OnEraseBkgnd(pDC);
}

void _PopulateComboFromMessageSource(CExtComboBox &wndCombo, MessageSource *source)
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
            wndCombo.InsertString(-1, define.first.c_str());
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
            _PopulateComboFromMessageSource(m_wndComboNoun, _pDoc->GetNounMessageSource());
        }
        if (IsFlagSet(hint, MessageChangeHint::VerbsChanged))
        {
            _PopulateComboFromMessageSource(m_wndComboVerb, appState->GetResourceMap().GetVerbsMessageSource());
        }
        if (IsFlagSet(hint, MessageChangeHint::TalkersChanged))
        {
            _PopulateComboFromMessageSource(m_wndComboTalker, appState->GetResourceMap().GetTalkersMessageSource());
        }
        if (IsFlagSet(hint, MessageChangeHint::ConditionsChanged))
        {
            _PopulateComboFromMessageSource(m_wndComboCondition, _pDoc->GetConditionMessageSource());
        }
    }
}

void MessageEditPane::UpdateNonView(CObject *pObject)
{
    // TODO: Provide more specific update mechanism
    MessageChangeHint hint = GetHint<MessageChangeHint>(pObject);
    if (IsFlagSet(hint, MessageChangeHint::Selection))
    {
        _Update();
    }
    _UpdateCombos(hint);
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
            m_wndEditSequence.SetWindowTextA(fmt::format("{0}", entry->Sequence).c_str());
            // TODO: more stuff
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
        _Update();
        _UpdateCombos(MessageChangeHint::ConditionsChanged | MessageChangeHint::TalkersChanged | MessageChangeHint::VerbsChanged | MessageChangeHint::NounsChanged);
    }
}



void MessageEditPane::OnBnClickedButtonaddseq()
{
    // TODO: Add another with with same everything, but different sequence
}


void MessageEditPane::OnBnClickedButtonclone()
{
    // TODO: Clone the current one
}
