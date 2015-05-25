#include "stdafx.h"
#include "AppState.h"
#include "MessageChildFrame.h"

IMPLEMENT_DYNCREATE(CMessageChildFrame, CMDITabChildWnd)

CMessageChildFrame::CMessageChildFrame()
{
}

CMessageChildFrame::~CMessageChildFrame()
{
}


BEGIN_MESSAGE_MAP(CMessageChildFrame, CMDITabChildWnd)
END_MESSAGE_MAP()

int CMessageChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}
