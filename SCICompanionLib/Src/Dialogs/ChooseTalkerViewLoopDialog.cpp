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
// ChooseTalkerViewLoopDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ChooseTalkerViewLoopDialog.h"
#include "format.h"

using namespace std;

// ChooseTalkerViewLoopDialog dialog

ChooseTalkerViewLoopDialog::ChooseTalkerViewLoopDialog(uint16_t talker, int view, int loop, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(ChooseTalkerViewLoopDialog::IDD, pParent), _nView(view), _nLoop(loop), _talker(talker)
{
}

ChooseTalkerViewLoopDialog::~ChooseTalkerViewLoopDialog()
{
}

void ChooseTalkerViewLoopDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    DDX_Control(pDX, IDC_EDITVIEW, m_wndEditView);
    m_wndEditView.LimitText(5);
    m_wndEditView.SetWindowText(fmt::format("{0}", _nView).c_str());

    DDX_Control(pDX, IDC_EDITLOOP, m_wndEditLoop);
    m_wndEditLoop.LimitText(5);
    m_wndEditLoop.SetWindowText(fmt::format("{0}", _nLoop).c_str());

    // Visuals
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
    DDX_Control(pDX, IDC_STATIC3, m_wndLabel3);
}


BEGIN_MESSAGE_MAP(ChooseTalkerViewLoopDialog, CDialog)
END_MESSAGE_MAP()


// ChooseTalkerViewLoopDialog message handlers

void ChooseTalkerViewLoopDialog::OnOK()
{
    CString str;
    m_wndEditLoop.GetWindowText(str);
    _nLoop = StrToInt(str);

    m_wndEditView.GetWindowText(str);
    _nView = StrToInt(str);

    __super::OnOK();
}