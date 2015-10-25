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

#include "MDITabChildWnd.h"
#include "MessageNounConditionPane.h"
#include "MessageEditPane.h"
#include "ExtControlBarNoX.h"

class CMessageChildFrame : public CMDITabChildWnd
{
    DECLARE_DYNCREATE(CMessageChildFrame)
protected:
    CMessageChildFrame();           // protected constructor used by dynamic creation
    virtual ~CMessageChildFrame();
    virtual MDITabType GetTabType() { return TAB_MESSAGE; }

    void HookUpNonViews(CDocument *pDoc) override;

private:
    afx_msg void OnAudioPlay();
    afx_msg void OnAudioStop();
    afx_msg void OnAudioRecord();

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()

    MessageNounConditionPane m_wndNounConditionPane;
    CExtControlBarNoX m_wndResizableBarNounCondition;
    MessageEditPane m_wndMessageEditPane;
    CExtControlBarNoX m_wndResizableBarMessageEdit;
};


