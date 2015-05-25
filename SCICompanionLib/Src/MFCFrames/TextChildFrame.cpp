#include "stdafx.h"
#include "AppState.h"
#include "TextChildFrame.h"

IMPLEMENT_DYNCREATE(CTextChildFrame, CMDITabChildWnd)

CTextChildFrame::CTextChildFrame()
{
}

CTextChildFrame::~CTextChildFrame()
{
}


BEGIN_MESSAGE_MAP(CTextChildFrame, CMDITabChildWnd)
END_MESSAGE_MAP()

int CTextChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}
