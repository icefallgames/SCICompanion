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
// ScriptFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptFrame.h"
#include "AppState.h"
#include "VocabAutoCompleteDialog.h"
#include "editcmd.h"

// CVocabChildFrame

IMPLEMENT_DYNCREATE(CScriptFrame, CMDITabChildWnd)

CScriptFrame::CScriptFrame()
{
    appState->SetScriptFrame(this);
}

CScriptFrame::~CScriptFrame()
{
    appState->SetScriptFrame(NULL);
}


BEGIN_MESSAGE_MAP(CScriptFrame, CMDITabChildWnd)
    ON_WM_CREATE()
END_MESSAGE_MAP()



// CVocabChildFrame message handlers


int CScriptFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    EnableDocking(CBRS_ALIGN_ANY);

    //m_wndStatusBar.SetPane0Icon(IDI_EXCLAMATION);

    return 0;
}

