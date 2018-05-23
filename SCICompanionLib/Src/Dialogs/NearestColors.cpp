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
// ChooseColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "NearestColors.h"


#define NEARESTCOLOR_TIMER 7458

uint8_t g_nearestColorSource;

bool g_NearestColorDither = false;

// NearestColorsDialog dialog

// TODO:
// - update thingy, and have timer that redoes it.
// - make a modal dialog?
// - checkbox affects dither.

NearestColorsDialog::NearestColorsDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(NearestColorsDialog::IDD, pParent)
{
    g_nearestColorSource = 0;

    EgaColorToRGBQuad(EGAColorFromByte(0)); // ping it to fill g_rgColorCombosHSLGamma

    std::fill_n(_myPalette, ARRAYSIZE(_myPalette), EGAColorFromByte(0));
    std::fill_n(_myPaletteRGB, ARRAYSIZE(_myPaletteRGB), RGBQUAD());
    // 2 rows, 8 columns
    m_wndStatic.SetPalette(2, 8, _myPalette, ARRAYSIZE(g_egaColorsMixed), g_egaColorsMixed, g_NearestColorDither);
    //m_wndStatic.ShowSelection(TRUE);
}

NearestColorsDialog::~NearestColorsDialog()
{

}

struct IndexAndDistance
{
    int index;
    int distance;
};

void NearestColorsDialog::_SyncColor()
{
    RGBQUAD main = g_egaColorsMixed[g_nearestColorSource];
    IndexAndDistance distance[256];
    for (int i = 0; i < 256; i++)
    {
        RGBQUAD other = g_egaColorsMixed[i];
        distance[i].distance = GetColorDistanceRGB(main, other);
        distance[i].index = i;
    }
    std::sort(std::begin(distance), std::end(distance),
        [](const IndexAndDistance &a, const IndexAndDistance &b) -> bool
    {
        return a.distance < b.distance;
    }
        );

    int paletteIndex = 0;
    for (; paletteIndex < 8; paletteIndex++)
    {
        _myPalette[paletteIndex] = EGAColorFromByte(g_nearestColorSource);
        _myPaletteRGB[paletteIndex] = g_egaColorsMixed[g_nearestColorSource];
    }
    int index = 0;
    std::set<COLORREF> used; // Used for de-duping

    while (paletteIndex < 16)
    {
        bool added = false;
        while ((index < 256) && !added)
        {
            if (distance[index].distance > 0) 
            {
                RGBQUAD rgb = g_egaColorsMixed[(uint8_t)distance[index].index];
                COLORREF cref = _ColorRefFromRGBQuad(rgb);
                if (!contains(used, cref))
                {
                    used.insert(cref);
                    _myPalette[paletteIndex] = EGAColorFromByte((uint8_t)distance[index].index);
                    _myPaletteRGB[paletteIndex] = rgb;
                    added = true;
                }
            }
            index++;
        }
        paletteIndex++;
    }

    // And update.
    m_wndStatic.ShowSourceIndices(true);
    m_wndStatic.SetPalette(2, 8, _myPalette, ARRAYSIZE(g_egaColorsMixed), g_egaColorsMixed, g_NearestColorDither);
    m_wndStatic.Invalidate();
}

void NearestColorsDialog::DoDataExchange(CDataExchange* pDX)
{
    CExtResizableDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStatic);
    DDX_Control(pDX, IDC_DITHERCHECK, m_wndDither);
    m_wndDither.SetCheck(g_NearestColorDither);

    // Visuals
    DDX_Control(pDX, IDC_DIALOGDESCRIPTION, m_wndDescription);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);

    SetTimer(NEARESTCOLOR_TIMER, 200, nullptr);
}

void NearestColorsDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == NEARESTCOLOR_TIMER)
    {
        _SyncColor();
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

void NearestColorsDialog::OnBnClickedDitherCheck()
{
    g_NearestColorDither = (m_wndDither.GetCheck() == BST_CHECKED);
    _SyncColor();
}


BEGIN_MESSAGE_MAP(NearestColorsDialog, CExtResizableDialog)
    ON_WM_KEYDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_ACTIVATE()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_DITHERCHECK, OnBnClickedDitherCheck)
END_MESSAGE_MAP()


