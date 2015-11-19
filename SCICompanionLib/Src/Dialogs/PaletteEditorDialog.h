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
#pragma once

#include "resource.h"
#include "NonViewClient.h"
#include "PaletteEditorCommon.h"

//#define DEBUG_OUTPUT_CYCLE
//#define DEBUG_OUTPUT_CYCLE_PATH ""

class CResourceDocument;
struct PaletteComponent;
struct Cel;

class PaletteEditorDialog : public PaletteEditorCommon<CExtResizableDialog>, public INonViewClient
{
public:
    PaletteEditorDialog(IVGAPaletteDefinitionCallback *callback, PaletteComponent &palette, const std::vector<const Cel*> &cels, bool enableCycling);  // standard constructor
    PaletteEditorDialog();

    // Dialog Data
    enum { IDD = IDD_PALETTE_EDITOR };

    void DoDataExchange(CDataExchange* pDX) override;

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedButtonSaverange();

    BOOL PreTranslateMessage(MSG* pMsg) override;

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick) override;

protected:
    void OnCancel() override;
    void OnOK() override;

private:
    void OnTimer(UINT_PTR nIDEvent);
    void _UpdateCycleUI();
    void _SyncCheckState();
    void _SyncSelectionCycle();
    void _Cycle();

    CExtCheckBox m_wndPreviewCycling;
    CExtRadioButton m_wndCycleLeft;
    CExtRadioButton m_wndCycleRight;
    CExtSliderWnd m_wndCycleSlider;
    int _cycleCountdown;
    CExtButton m_wndSaveAs;
    bool _enableCycling;
    bool _cycleForward;
    bool _cycling;
    bool _initialized;
    std::unique_ptr<PaletteComponent> _cyclePaletteCopy;
    HACCEL _hAccel;

#ifdef DEBUG_OUTPUT_CYCLE
    std::unique_ptr<Cel> _celTemp;
#endif

public:
    afx_msg void OnBnClickedCheckpreviewcycling();
    afx_msg void OnBnClickedButtoncycleleft();
    afx_msg void OnBnClickedButtoncycleright();
    afx_msg void OnSaveAsResource();
};
