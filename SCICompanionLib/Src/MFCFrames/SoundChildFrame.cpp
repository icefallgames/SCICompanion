// VocabChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SoundChildFrame.h"


// CVocabChildFrame

IMPLEMENT_DYNCREATE(CSoundChildFrame, CMDITabChildWnd)

CSoundChildFrame::CSoundChildFrame()
{
}

CSoundChildFrame::~CSoundChildFrame()
{
}


BEGIN_MESSAGE_MAP(CSoundChildFrame, CMDITabChildWnd)
END_MESSAGE_MAP()

int CSoundChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}


// CSoundChildFrame message handlers
