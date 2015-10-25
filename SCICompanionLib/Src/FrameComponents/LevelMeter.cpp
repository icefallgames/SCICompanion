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
#include "stdafx.h"
#include "LevelMeter.h"
#include "AudioRecording.h"
#include "format.h"

IMPLEMENT_DYNAMIC(LevelMeter, CStatic)

#define LEVEL_TIMER 8361
#define LOW_FREQ_TIMER 9526
// LevelMeter control

const int MaxSegments = 15;


LevelMeter::~LevelMeter() {}

LevelMeter::LevelMeter()
    : CStatic(), _level(0), _monitor(false), _maxRecentLevel(0)
{
}

void LevelMeter::PreSubclassWindow()
{
    SetTimer(LOW_FREQ_TIMER, 100, nullptr);
    __super::PreSubclassWindow();
}

void LevelMeter::Monitor(bool monitor)
{
    if (monitor)
    {
        SetTimer(LEVEL_TIMER, 30, nullptr);
    }
    else
    {
        _level = 0;
        _maxRecentLevel = 0;
        KillTimer(LEVEL_TIMER);
    }
    if (_monitor != monitor)
    {
        _monitor = monitor;
        if (!_monitor)
        {
            g_audioRecording.StopMonitor();
        }
        Invalidate(FALSE);
    }
    if (_checkbox)
    {
        _checkbox->SetCheck(_monitor ? BST_CHECKED : BST_UNCHECKED);
    }
}

void LevelMeter::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == LEVEL_TIMER)
    {
        _level = g_audioRecording.GetLevel();
        _maxRecentLevel = max(_maxRecentLevel, _level);
        RedrawWindow(nullptr);
    }
    else if (nIDEvent == LOW_FREQ_TIMER)
    {
        // If we're not monitoring, but we've started recording, turn ourselves on.
        if (!_monitor && g_audioRecording.IsRecording())
        {
            Monitor(true);
        }

        // Also, use this to decrease the level we show
        _maxRecentLevel -= 100 / MaxSegments;
        _maxRecentLevel = max(_maxRecentLevel, _level);
    }
}

void LevelMeter::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    LPRECT prc = &lpDrawItemStruct->rcItem;
    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);

    InflateRect(prc, -1, -1);

    int activeSegmentCount = _maxRecentLevel * MaxSegments / 100;
    int gap = 1;
    int segmentWidth = RECTWIDTH(*prc) / MaxSegments;
    int lightWidth = max(1, segmentWidth - gap);
    int remainder = 0;
    for (int i = 0; i < activeSegmentCount; i++)
    {
        COLORREF color = RGB(0, 255, 0);
        if (i >= 14)
        {
            color = RGB(255, 0, 0);
        }
        else if (i > 10)
        {
            color = RGB(255, 255, 0);
        }
        CRect rcLight = { i * segmentWidth, prc->top, i * segmentWidth + lightWidth, prc->bottom };
        dc.FillSolidRect(&rcLight, color);
        CRect rcGap = { rcLight.right, prc->top, rcLight.right + gap, prc->bottom };
        dc.FillSolidRect(&rcGap, RGB(0, 0, 0));
        remainder = rcGap.right;
    }
    CRect rcEnd = { remainder, prc->top, prc->right, prc->bottom };
    dc.FillSolidRect(&rcEnd, _monitor ? RGB(0, 0, 0) : RGB(96, 96, 96));

    // Draw a bevel around the edge
    InflateRect(prc, 1, 1);
    CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ hOldPen = dc.SelectObject(pen);
    dc.MoveTo(prc->left, prc->top);
    dc.LineTo(prc->right, prc->top);
    dc.LineTo(prc->right, prc->bottom);
    CPen lightPen(PS_SOLID, 1, RGB(128, 128, 128));
    dc.SelectObject(lightPen);
    dc.LineTo(prc->left, prc->bottom);
    dc.LineTo(prc->left, prc->top);
    dc.SelectObject(hOldPen);

    dc.Detach();
}

BEGIN_MESSAGE_MAP(LevelMeter, CStatic)
    ON_WM_TIMER()
    ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()