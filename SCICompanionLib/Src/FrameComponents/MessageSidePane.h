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
#pragma once

#include "NonViewClient.h"
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"
#include "MessageEditorListCtrl.h"
#include "LevelMeter.h"

class CMessageDoc;
struct TextComponent;

class MessageSidePane : public CExtDialogFwdCmd, public INonViewClient
{
public:
    MessageSidePane(CWnd* pParent = NULL);   // standard constructor
    virtual ~MessageSidePane();

    // Dialog Data
    enum
    {
        IDD = IDD_MESSAGETOOLBOX,
    };

    void SetDocument(CDocument *pDoc);
    CMessageDoc *GetDocument() { return _pDoc; }

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;

    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

    BOOL OnEraseBkgnd(CDC *pDC);

private:
    void _Update();

    CMessageDoc *_pDoc;

    MessageEditorListCtrl m_wndVerbs;
    MessageEditorListCtrl m_wndTalkers;

    // Visual
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtButton m_wndButton1;
    CExtButton m_wndButton2;
    CExtButton m_wndButton3;
    CExtButton m_wndButton4;

    CExtCheckBox m_wndCheckMonitor;
    LevelMeter m_wndLevelMeter;

    CExtHyperLinkButton m_wndGotoScript;

    HACCEL _hAccel;
public:
    afx_msg void OnGotoScript();
    afx_msg void OnBnClickedButtonaddverb();
    afx_msg void OnBnClickedButtonaddtalker();
    afx_msg void OnBnClickedButtondeleteverb();
    afx_msg void OnBnClickedButtoncheckmonitor();
    afx_msg void OnBnClickedButtondeletetalker();
};
