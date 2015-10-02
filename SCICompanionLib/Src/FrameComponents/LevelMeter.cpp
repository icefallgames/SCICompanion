#include "stdafx.h"
#include "LevelMeter.h"
#include "AudioRecording.h"
#include "format.h"

IMPLEMENT_DYNAMIC(LevelMeter, CStatic)

#define LEVEL_TIMER 8361

// LevelMeter control

LevelMeter::~LevelMeter() {}

LevelMeter::LevelMeter()
    : CStatic(), _level(0)
{
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
        KillTimer(LEVEL_TIMER);
    }
}

void LevelMeter::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == LEVEL_TIMER)
    {
        _level = g_audioRecording.GetLevel();
        RedrawWindow(nullptr);
    }
}

void LevelMeter::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    LPRECT prc = &lpDrawItemStruct->rcItem;
    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);

    int maxSegments = 10;
    int activeSegmentCount = _level / maxSegments;
    int gap = 1;
    int segmentWidth = RECTWIDTH(*prc) / maxSegments;
    int lightWidth = max(1, segmentWidth - gap);
    int remainder = 0;
    for (int i = 0; i < activeSegmentCount; i++)
    {
        COLORREF color = RGB(0, 255, 0);
        if (i > 7)
        {
            color = RGB(255, 255, 0);
        }
        else if (i > 9)
        {
            color = RGB(255, 0, 0);
        }
        CRect rcLight = { i * segmentWidth, prc->top, i * segmentWidth + lightWidth, prc->bottom };
        dc.FillSolidRect(&rcLight, color);
        CRect rcGap = { rcLight.right, prc->top, rcLight.right + gap, prc->bottom };
        dc.FillSolidRect(&rcGap, RGB(0, 0, 0));
        remainder = rcGap.right;
    }
    CRect rcEnd = { remainder, prc->top, prc->right, prc->bottom };
    dc.FillSolidRect(&rcEnd, RGB(0, 0, 0));

    dc.Detach();
}

BEGIN_MESSAGE_MAP(LevelMeter, CStatic)
    ON_WM_TIMER()
    ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()