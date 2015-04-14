// CPaletteChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "PaletteChildFrame.h"

// CPaletteChildFrame

IMPLEMENT_DYNCREATE(CPaletteChildFrame, CMDITabChildWnd)

CPaletteChildFrame::CPaletteChildFrame()
{
}

CPaletteChildFrame::~CPaletteChildFrame()
{
}

BEGIN_MESSAGE_MAP(CPaletteChildFrame, CMDITabChildWnd)
END_MESSAGE_MAP()

void CPaletteChildFrame::HookUpNonViews(CDocument *pDoc)
{
}

int CPaletteChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}


// CRoomExplorerFrame message handlers
