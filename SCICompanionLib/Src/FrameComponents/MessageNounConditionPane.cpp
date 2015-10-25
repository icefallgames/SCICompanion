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
#include "MessageNounConditionPane.h"
#include "MessageDoc.h"
#include "MessageSource.h"
#include "Text.h"
#include "Message.h"
#include "CObjectWrap.h"


MessageNounConditionPane::MessageNounConditionPane(CWnd* pParent /*=NULL*/)
    : CExtDialogFwdCmd(IDD, pParent), _hAccel(nullptr)
{
    // Load our accelerators
    // HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS), RT_ACCELERATOR);
    // _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS));
}

MessageNounConditionPane::~MessageNounConditionPane()
{
}

void MessageNounConditionPane::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LISTCONDITIONS, m_wndConditions);
    DDX_Control(pDX, IDC_LISTNOUNS, m_wndNouns);
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
    DDX_Control(pDX, IDC_BUTTONADDNOUN, m_wndButton1);
    DDX_Control(pDX, IDC_BUTTONADDCONDITION, m_wndButton2);
    DDX_Control(pDX, IDC_BUTTONDELETENOUN, m_wndButton3);
    m_wndButton3.SetIcon(IDI_DELETE, 0, 0, 0, 16, 16);
    DDX_Control(pDX, IDC_BUTTONDELETECONDITION, m_wndButton4);
    m_wndButton4.SetIcon(IDI_DELETE, 0, 0, 0, 16, 16);
}


BEGIN_MESSAGE_MAP(MessageNounConditionPane, CExtDialogFwdCmd)
    ON_WM_DRAWITEM()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_BUTTONADDNOUN, &MessageNounConditionPane::OnBnClickedButtonaddnoun)
    ON_BN_CLICKED(IDC_BUTTONADDCONDITION, &MessageNounConditionPane::OnBnClickedButtonaddcondition)
    ON_BN_CLICKED(IDC_BUTTONDELETENOUN, &MessageNounConditionPane::OnBnClickedButtondeletenoun)
    ON_BN_CLICKED(IDC_BUTTONDELETECONDITION, &MessageNounConditionPane::OnBnClickedButtondeletecondition)
END_MESSAGE_MAP()

BOOL MessageNounConditionPane::OnEraseBkgnd(CDC *pDC)
{
    return __super::OnEraseBkgnd(pDC);
}


// MessageNounConditionPane message handlers

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
BOOL MessageNounConditionPane::PreTranslateMessage(MSG* pMsg)
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

BOOL MessageNounConditionPane::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();

    // Set up anchoring for resize
    AddAnchor(IDC_LISTNOUNS, CPoint(0, 0), CPoint(100, 50));
    AddAnchor(IDC_LISTCONDITIONS, CPoint(0, 50), CPoint(100, 100));
    AddAnchor(IDC_STATIC1, CPoint(0, 0), CPoint(0, 0));
    AddAnchor(IDC_STATIC2, CPoint(0, 50), CPoint(0, 50));
    AddAnchor(IDC_BUTTONADDCONDITION, CPoint(0, 50), CPoint(100, 50));
    AddAnchor(IDC_BUTTONDELETECONDITION, CPoint(100, 50), CPoint(100, 50));
    AddAnchor(IDC_BUTTONADDNOUN, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTONDELETENOUN, CPoint(100, 0), CPoint(100, 0));
    // Hide the sizing grip
    ShowSizeGrip(FALSE);

    return fRet;
}

void MessageNounConditionPane::UpdateNonView(CObject *pObject)
{
    // TODO: Provide more specific update mechanism
    MessageChangeHint hint = GetHint<MessageChangeHint>(pObject);
    if (IsFlagSet(hint, MessageChangeHint::Changed))
    {
        _Update();
    }
}

void MessageNounConditionPane::_Update()
{
    if (_pDoc)
    {
        m_wndNouns.SetSource(_pDoc, MessageSourceType::Nouns);
        m_wndConditions.SetSource(_pDoc, MessageSourceType::Conditions);
    }
}

void MessageNounConditionPane::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CMessageDoc*>(pDoc);
    if (_pDoc)
    {
        _pDoc->AddNonViewClient(this);
        _Update();
    }
}

void MessageNounConditionPane::OnBnClickedButtonaddnoun()
{
    m_wndNouns.AddNewItem();
}

void MessageNounConditionPane::OnBnClickedButtonaddcondition()
{
    m_wndConditions.AddNewItem();
}

void MessageNounConditionPane::OnBnClickedButtondeletenoun()
{
    m_wndNouns.DeleteSelectedItem();
}


void MessageNounConditionPane::OnBnClickedButtondeletecondition()
{
    m_wndConditions.DeleteSelectedItem();
}
