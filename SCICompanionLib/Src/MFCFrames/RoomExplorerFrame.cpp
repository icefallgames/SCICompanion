// CRoomExplorerFrame.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "RoomExplorerFrame.h"


// CRoomExplorerFrame

IMPLEMENT_DYNCREATE(CRoomExplorerFrame, CMDITabChildWnd)

CRoomExplorerFrame::CRoomExplorerFrame()
{
}

CRoomExplorerFrame::~CRoomExplorerFrame()
{
}


BEGIN_MESSAGE_MAP(CRoomExplorerFrame, CMDITabChildWnd)
END_MESSAGE_MAP()

int CRoomExplorerFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}


// CRoomExplorerFrame message handlers
