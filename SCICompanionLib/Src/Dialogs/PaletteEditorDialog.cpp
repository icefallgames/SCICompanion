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
#include "PaletteEditorDialog.h"
#include "ExtColorDlg.h"
#include "PaletteOperations.h"
#include "RasterOperations.h"
#include "ImageUtil.h"
#include "ResourceDocument.h"
#include "ResourceEntity.h"

#define PALETTE_TIMER 6789

PaletteEditorDialog::PaletteEditorDialog(IVGAPaletteDefinitionCallback *callback, PaletteComponent &palette, const std::vector<const Cel*> &cels, bool enableCycling)
    : PaletteEditorCommon<CExtResizableDialog>(callback, PaletteEditorDialog::IDD, nullptr), _cycleForward(true), _cycling(false), _initialized(false), _enableCycling(enableCycling)
{
    Init(palette, cels);

    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPALETTE), RT_ACCELERATOR);
    _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPALETTE));

#ifdef DEBUG_OUTPUT_CYCLE
    _celTemp = std::make_unique<Cel>(*cels.back());
#endif
}

// INonViewClient
void PaletteEditorDialog::UpdateNonView(CObject *pObject)
{
    UpdateCommon(pObject);
}

void PaletteEditorDialog::_SyncSelectionCycle()
{
    std::vector<std::pair<uint8_t, uint8_t>> ranges = GetSelectedRanges(m_wndStatic);
    bool canCycle = _IsSingleRangeSelected(ranges);
    if (canCycle)
    {
        // If the user changed the selection while we were cycling, then act as if we toggled it off and on.
        // Otherwise, we'll cycle the wrong things.
        if (_cycling)
        {
            OnBnClickedCheckpreviewcycling();
            OnBnClickedCheckpreviewcycling();
        }
    }
    else
    {
        if (_cycling)
        {
            OnBnClickedCheckpreviewcycling();
        }
    }
    m_wndPreviewCycling.EnableWindow(canCycle);
    m_wndCycleLeft.EnableWindow(canCycle);
    m_wndCycleRight.EnableWindow(canCycle);
    m_wndCycleSlider.EnableWindow(canCycle);

    _SyncCheckState();
}

void PaletteEditorDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    __super::OnColorClick(bIndex, nID, fLeftClick);
    _SyncSelectionCycle();
}

void PaletteEditorDialog::OnCancel()
{
    if (_cycling)
    {
        OnBnClickedCheckpreviewcycling();
    }
    __super::OnCancel();
}

void PaletteEditorDialog::OnOK()
{
    if (_cycling)
    {
        OnBnClickedCheckpreviewcycling();
    }
    __super::OnOK();
}

void PaletteEditorDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    if (!_initialized)
    {
        DDX_Control(pDX, IDC_CHECKPREVIEWCYCLING, m_wndPreviewCycling);
        DDX_Control(pDX, IDC_BUTTONCYCLELEFT, m_wndCycleLeft);
        DDX_Control(pDX, IDC_BUTTONCYCLERIGHT, m_wndCycleRight);
        DDX_Control(pDX, IDC_SLIDERCYCLESPEED, m_wndCycleSlider);
        m_wndCycleSlider.SetRange(0, 10);
        
        DDX_Control(pDX, IDC_BUTTON_SAVEPALETTE, m_wndSaveAs);

        if (!_enableCycling)
        {
            m_wndPreviewCycling.ShowWindow(SW_HIDE);
            m_wndCycleLeft.ShowWindow(SW_HIDE);
            m_wndCycleRight.ShowWindow(SW_HIDE);
        }

        _UpdateCycleUI();

        //SetTimer(PALETTE_TIMER, 17, nullptr); // 16.666 (60 FPS). This seems to match the game clock.
        SetTimer(PALETTE_TIMER, 8, nullptr);    // 8 (120 FPS). This seems to match the game clock. Or am I doing something wrong?

        _SyncSelectionCycle();
    }
}

void PaletteEditorDialog::_SyncCheckState()
{
    m_wndPreviewCycling.SetCheck(_cycling);
}

void PaletteEditorDialog::_UpdateCycleUI()
{
    m_wndCycleLeft.SetCheck(!_cycleForward);
    m_wndCycleRight.SetCheck(_cycleForward);
}

BEGIN_MESSAGE_MAP(PaletteEditorDialog, PaletteEditorCommon)
    ON_BN_CLICKED(IDC_CHECKPREVIEWCYCLING, &PaletteEditorDialog::OnBnClickedCheckpreviewcycling)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTONCYCLELEFT, &PaletteEditorDialog::OnBnClickedButtoncycleleft)
    ON_BN_CLICKED(IDC_BUTTONCYCLERIGHT, &PaletteEditorDialog::OnBnClickedButtoncycleright)
    ON_BN_CLICKED(IDC_BUTTON_SAVEPALETTE, OnSaveAsResource)
END_MESSAGE_MAP()

#ifdef DEBUG_OUTPUT_CYCLE
int g_saveAsGifTest = 0;
#endif

void PaletteEditorDialog::_Cycle()
{
    std::vector<std::pair<uint8_t, uint8_t>> ranges = GetSelectedRanges(m_wndStatic);
    if (_IsSingleRangeSelected(ranges))
    {
        int offset = _cycleForward ? 1 : -1;
        PaletteComponent prev = *_palette;
        for (int i = ranges[0].first; i <= (int)ranges[0].second; i++)
        {
            int oldIndex = i + offset;
            if (oldIndex < (int)ranges[0].first)
            {
                oldIndex = ranges[0].second;
            }
            if (oldIndex > (int)ranges[0].second)
            {
                oldIndex = ranges[0].first;
            }
            _palette->Colors[i] = prev.Colors[oldIndex];
        }

#ifdef DEBUG_OUTPUT_CYCLE
        g_saveAsGifTest++;
        g_saveAsGifTest %= ((int)ranges.back().second - (int)ranges.back().first + 1);
        std::vector<Cel> cels;
        cels.push_back(*_celTemp);
        SaveCelsAndPaletteToGIFFile(fmt::format("{0}\\image_{1:0>3}.gif", DEBUG_OUTPUT_CYCLE_PATH, g_saveAsGifTest).c_str(), cels, 256, _palette->Colors, _palette->Mapping, _celTemp->TransparentColor);
#endif
    }
}

void PaletteEditorDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == PALETTE_TIMER)
    {
        if (_cycling)
        {
            _cycleCountdown--;
            if (_cycleCountdown <= 0)
            {
                _cycleCountdown += m_wndCycleSlider.GetPos();
                _Cycle();
                if (_callback)
                {
                    _callback->OnVGAPaletteChanged();
                }
            }
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

void PaletteEditorDialog::OnBnClickedCheckpreviewcycling()
{
    _cycling = !_cycling;
    if (_cycling)
    {
        // Make a copy of the palette so we can restore it
        _cyclePaletteCopy = std::make_unique<PaletteComponent>(*_palette);

#ifdef DEBUG_OUTPUT_CYCLE
        g_saveAsGifTest = 0; 
#endif

    }
    else
    {
        // Restore the palette
        *_palette = *_cyclePaletteCopy;
    }
    if (_callback)
    {
        _callback->OnVGAPaletteChanged();
    }
}

void PaletteEditorDialog::OnBnClickedButtoncycleleft()
{
    _cycleForward = false;
}


void PaletteEditorDialog::OnBnClickedButtoncycleright()
{
    _cycleForward = true;
}

void PaletteEditorDialog::OnSaveAsResource()
{
    std::unique_ptr<ResourceEntity> newPalette(CreatePaletteResource(appState->GetVersion()));
    newPalette->GetComponent<PaletteComponent>() = *this->_palette;
    appState->GetResourceMap().AppendResourceAskForNumber(*newPalette, "", true);
}

BOOL PaletteEditorDialog::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}