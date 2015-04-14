// VocabChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "VocabChildFrame.h"


// CVocabChildFrame

IMPLEMENT_DYNCREATE(CVocabChildFrame, CMDITabChildWnd)

CVocabChildFrame::CVocabChildFrame()
{
}

CVocabChildFrame::~CVocabChildFrame()
{
}


BEGIN_MESSAGE_MAP(CVocabChildFrame, CMDITabChildWnd)
END_MESSAGE_MAP()

int CVocabChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}


// CVocabChildFrame message handlers
