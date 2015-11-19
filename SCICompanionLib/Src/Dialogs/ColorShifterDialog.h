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
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
#include "View.h"
#include "ColorShiftCallback.h"

struct PaletteComponent;
struct RasterComponent;

enum class ShiftExtent
{
    Cel,
    Loop,
    View
};

class ColorShifterDialog : public CExtResizableDialog, public IColorDialogCallback
{
public:
    ColorShifterDialog(const PaletteComponent &palette, int colorCount, RasterComponent &raster, CelIndex celIndex, IColorShiftCallback &callback, CWnd* pParent = NULL);   // standard constructor

    BOOL PreTranslateMessage(MSG* pMsg) override;

    // Dialog Data
    enum { IDD = IDD_COLOR_SHIFTER };

    void OnColorClick(BYTE bIndex, int nID, BOOL fLeft) override;
    void OnColorHover(BYTE bIndex) override {}

private:
    void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

    void _UpdateView();
    void _SyncSelection();
    void _Shift(int offset);
    void _CountUsedColors();

    DECLARE_MESSAGE_MAP()

    IColorShiftCallback &_callback;

    CelIndex _celIndex;
    RasterComponent &_rasterActual;
    std::unique_ptr<RasterComponent> _rasterSnapshot;
    const PaletteComponent &_palette;
    bool _actualUsedColors[256];

    ShiftExtent _shiftExtent;

    int _totalShift;
    bool _startingSelection[256];

    int _rows, _columns;

    CChooseColorStatic m_wndStatic;
    CExtEdit m_wndEditRange;
    CExtEdit m_wndEditDescription;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;

    CExtRadioButton m_wndCel;
    CExtRadioButton m_wndView;
    CExtRadioButton m_wndLoop;
    CExtGroupBox m_wndGroup;

    CExtButton m_wndUp;
    CExtButton m_wndDown;
    CExtButton m_wndRight;
    CExtButton m_wndLeft;

    CFont m_font;
public:
    afx_msg void OnBnClickedButtonup();
    afx_msg void OnBnClickedButtondown();
    afx_msg void OnBnClickedButtonright();
    afx_msg void OnBnClickedButtonleft();
    afx_msg void OnBnClickedRadiocel();
    afx_msg void OnBnClickedRadioloop();
    afx_msg void OnBnClickedRadioview();
};
