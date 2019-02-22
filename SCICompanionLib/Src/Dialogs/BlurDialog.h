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

class BlurDialog : public CExtResizableDialog
{
public:
    BlurDialog(const PaletteComponent &palette, int colorCount, RasterComponent &raster, CelIndex celIndex, IColorShiftCallback &callback, bool applyToAll, CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    enum { IDD = IDD_BLUR };

    void OnOK() override;

private:
    void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd);

    void _UpdateView();

    DECLARE_MESSAGE_MAP()

    IColorShiftCallback &_callback;

    CelIndex _celIndex;
    RasterComponent &_rasterActual;
    std::unique_ptr<RasterComponent> _rasterSnapshot;
    const PaletteComponent &_palette;

    CExtButton m_wndOk;
    CExtButton m_wndCancel;

    CExtSliderWnd m_wndSliderBoost;
    CExtSliderWnd m_wndSliderSigma;

    int _sigma;
    int _boost;
    int _fXWrap;
    int _fYWrap;

    bool _applyToAll;

    bool _initialized;

public:
    //afx_msg void OnBnClickedRadioview();
};
