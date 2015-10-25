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
// EditViewChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ViewChildFrame.h"
#include "RasterView.h"
#include "ViewCelChooser.h"
#include "ViewCelListBox.h"

// CEditViewChildFrame

IMPLEMENT_DYNCREATE(CEditViewChildFrame, CMDITabChildWnd)

CEditViewChildFrame::CEditViewChildFrame() : _splitterCreated(false), _pViewMainView(nullptr)
{
}

CEditViewChildFrame::~CEditViewChildFrame()
{
}

BEGIN_MESSAGE_MAP(CEditViewChildFrame, CMDITabChildWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()



void CEditViewChildFrame::HookUpNonViews(CDocument *pDoc)
{

}

int CEditViewChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

void CEditViewChildFrame::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);

    CRect rect;
    GetClientRect(&rect);
    if (_splitterCreated)
    {
        CRect leftRectCurrent;
        m_wndSplitterLeft.GetClientRect(&leftRectCurrent);
        float leftPercentage = (float)leftRectCurrent.Width() / (float)_lastSize.cx;

        CRect topRectCurrent;
        _pViewMainView->GetClientRect(&topRectCurrent);
        float topPercentage = (float)topRectCurrent.Height() / (float)_lastSize.cy;

        int newLeftSize = (int)(leftPercentage * (float)cx);
        m_wndSplitter.SetColumnInfo(0, newLeftSize, 5);
        m_wndSplitter.SetColumnInfo(1, cx - newLeftSize, 5);
        m_wndSplitter.RecalcLayout();

        int newTopSize = (int)(topPercentage * (float)cy);
        m_wndSplitterLeft.SetRowInfo(0, newTopSize, 5);
        m_wndSplitterLeft.SetRowInfo(1, cy - newTopSize, 5);
        m_wndSplitterLeft.RecalcLayout();
    }

    _lastSize = CSize(cx, cy);
}

BOOL CEditViewChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
    // This is the code for a static splitter.
    CRect cr;
    GetClientRect(&cr);
    int widthUnit = cr.Width() / 5;
    CSize paneSize;

    int rc;
    m_wndSplitter.CreateStatic(this, 1, 2);
    rc = m_wndSplitterLeft.CreateStatic(&m_wndSplitter, 2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplitter.IdFromRowCol(0, 0));
    if (rc == FALSE)
    {
        return rc;
    }

    int heightUnit = cr.Height() / 3;

    paneSize.SetSize(widthUnit * 3, heightUnit * 2);
    rc = m_wndSplitterLeft.CreateView(0, 0, RUNTIME_CLASS(CRasterView), paneSize, pContext);
    if (rc == FALSE)
    {
        return rc;
    }
    _pViewMainView = (CRasterView*)m_wndSplitterLeft.GetPane(0, 0);

    paneSize.SetSize(widthUnit * 3, heightUnit * 1);
    rc = m_wndSplitterLeft.CreateView(1, 0, RUNTIME_CLASS(ViewCelListBox), paneSize, pContext);
    ViewCelListBox *celListBox = (ViewCelListBox*)m_wndSplitterLeft.GetPane(1, 0);
    celListBox->SetHorizontal(true);
    if (rc == FALSE)
    {
        return rc;
    }

    paneSize.SetSize(widthUnit * 2, cr.Height());
    rc = m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(ViewCelListBox), paneSize, pContext);
    celListBox = (ViewCelListBox*)m_wndSplitter.GetPane(0, 1);
    celListBox->SetHorizontal(false);

    m_wndSplitter.SetColumnInfo(0, widthUnit * 3, 5);
    m_wndSplitter.SetColumnInfo(1, widthUnit * 2, 5);
    m_wndSplitterLeft.SetRowInfo(0, heightUnit * 2, 5);
    m_wndSplitterLeft.SetRowInfo(1, heightUnit * 1, 5);
    m_wndSplitter.RecalcLayout();
    m_wndSplitterLeft.RecalcLayout();

    _lastSize = cr.Size();

    _splitterCreated = true;

    return rc;
}

BOOL CEditViewChildFrame::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
    BOOL fRet = __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    if (!fRet)
    {
        // If unhandled, give it the main (even if it was non-active).
        // NOTE: if it was active, it was already asked to handle it above.
        // We could optimize for that case here, if desired.
        if (_pViewMainView)
        {
            fRet = _pViewMainView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
        }

        if (!fRet)
        {
            // Give to CommandType view?

        }
    }
    return fRet;
}


// CEditViewChildFrame message handlers
