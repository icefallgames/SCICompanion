/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#include "stdafx.h"
#include "AppState.h"
#include "MessageSidePane.h"
#include "MessageDoc.h"
#include "MessageSource.h"
#include "Text.h"
#include "Message.h"
#include "CObjectWrap.h"


MessageSidePane::MessageSidePane(CWnd* pParent /*=NULL*/)
    : CExtDialogFwdCmd(IDD, pParent), _hAccel(nullptr)
{
    // Load our accelerators
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORMESSAGEEDIT), RT_ACCELERATOR);
    _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORMESSAGEEDIT));
}

MessageSidePane::~MessageSidePane()
{
}

void MessageSidePane::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LISTTALKERS, m_wndTalkers);
    DDX_Control(pDX, IDC_LISTVERBS, m_wndVerbs);
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
    DDX_Control(pDX, IDC_BUTTONADDVERB, m_wndButton1);
    DDX_Control(pDX, IDC_BUTTONADDTALKER, m_wndButton2);
    DDX_Control(pDX, IDC_BUTTONDELETEVERB, m_wndButton3);
    DDX_Control(pDX, IDC_BUTTONDELETETALKER, m_wndButton4);
    m_wndButton3.SetIcon(IDI_DELETE, 0, 0, 0, 16, 16);
    m_wndButton4.SetIcon(IDI_DELETE, 0, 0, 0, 16, 16);

    DDX_Control(pDX, IDC_GOTOSCRIPT, m_wndGotoScript);
    m_wndGotoScript.m_bUseStdCommandNotification = true;

    // Show these all the time, even if audio36 isn't support. No harm.
    DDX_Control(pDX, IDC_CHECK_MONITOR, m_wndCheckMonitor);
    m_wndCheckMonitor.SetIcon(IDI_MONITOR, 0, 0, 0, 16, 16);
    DDX_Control(pDX, IDC_LEVELMETER, m_wndLevelMeter);
    m_wndLevelMeter.SetCheckBox(&m_wndCheckMonitor);    // Associated with level meter so it can be updated.
}


BEGIN_MESSAGE_MAP(MessageSidePane, CExtDialogFwdCmd)
    ON_COMMAND(IDC_GOTOSCRIPT, OnGotoScript)
    ON_WM_DRAWITEM()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_BUTTONADDVERB, &MessageSidePane::OnBnClickedButtonaddverb)
    ON_BN_CLICKED(IDC_BUTTONADDTALKER, &MessageSidePane::OnBnClickedButtonaddtalker)
    ON_BN_CLICKED(IDC_BUTTONDELETEVERB, &MessageSidePane::OnBnClickedButtondeleteverb)
    ON_BN_CLICKED(IDC_BUTTONDELETETALKER, &MessageSidePane::OnBnClickedButtondeletetalker)
    ON_BN_CLICKED(IDC_CHECK_MONITOR, &MessageSidePane::OnBnClickedButtoncheckmonitor)
END_MESSAGE_MAP()

BOOL MessageSidePane::OnEraseBkgnd(CDC *pDC)
{
    return __super::OnEraseBkgnd(pDC);
}


// MessageSidePane message handlers

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
BOOL MessageSidePane::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

BOOL MessageSidePane::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();

    // Set up anchoring for resize
    AddAnchor(IDC_LISTVERBS, CPoint(0, 0), CPoint(100, 50));
    AddAnchor(IDC_LISTTALKERS, CPoint(0, 50), CPoint(100, 100));
    AddAnchor(IDC_STATIC1, CPoint(0, 0), CPoint(0, 0));
    AddAnchor(IDC_STATIC2, CPoint(0, 50), CPoint(0, 50));
    AddAnchor(IDC_BUTTONADDTALKER, CPoint(0, 50), CPoint(100, 50));
    AddAnchor(IDC_BUTTONADDVERB, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTONDELETETALKER, CPoint(100, 50), CPoint(100, 50));
    AddAnchor(IDC_BUTTONDELETEVERB, CPoint(100, 0), CPoint(100, 0));
    AddAnchor(IDC_CHECK_MONITOR, CPoint(0, 0), CPoint(0, 0));
    AddAnchor(IDC_LEVELMETER, CPoint(0, 0), CPoint(100, 0));
    // Hide the sizing grip
    ShowSizeGrip(FALSE);

    return fRet;
}

void MessageSidePane::UpdateNonView(CObject *pObject)
{
    // TODO: Provide more specific update mechanism
    MessageChangeHint hint = GetHint<MessageChangeHint>(pObject);
    if (IsFlagSet(hint, MessageChangeHint::Changed))
    {
        _Update();
    }
}

void MessageSidePane::_Update()
{
    if (_pDoc)
    {
    }
}

void MessageSidePane::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CMessageDoc*>(pDoc);
    if (_pDoc)
    {
        // This is as good time as any to trigger a reload.
        m_wndVerbs.SetSource(_pDoc, MessageSourceType::Verbs);
        m_wndTalkers.SetSource(_pDoc, MessageSourceType::Talkers);

        _pDoc->AddNonViewClient(this);
        _Update();
    }
}


void MessageSidePane::OnBnClickedButtonaddverb()
{
    m_wndVerbs.AddNewItem();
}

void MessageSidePane::OnBnClickedButtonaddtalker()
{
    m_wndTalkers.AddNewItem();
}


void MessageSidePane::OnBnClickedButtondeleteverb()
{
    m_wndVerbs.DeleteSelectedItem();
}

void MessageSidePane::OnBnClickedButtoncheckmonitor()
{
    m_wndLevelMeter.Monitor(m_wndCheckMonitor.GetCheck() == BST_CHECKED);
}

void MessageSidePane::OnBnClickedButtondeletetalker()
{
    m_wndTalkers.DeleteSelectedItem();
}

void MessageSidePane::OnGotoScript()
{
    if (_pDoc)
    {
        const ResourceEntity *resource = _pDoc->GetResource();
        if (resource)
        {
            appState->OpenMostRecentResource(ResourceType::Script, (uint16_t)resource->ResourceNumber);
        }
    }

}