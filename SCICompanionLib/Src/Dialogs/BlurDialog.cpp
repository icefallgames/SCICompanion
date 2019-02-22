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
#include "BlurDialog.h"
#include "PaletteOperations.h"
#include "ImageUtil.h"
#include "RasterOperations.h"
#include "View.h"
#include <array>

using namespace std;

// Divided by 100
int g_lastSigma = 200;
int g_lastBoost = 100;

const int KernelSize = 7;

BlurDialog::BlurDialog(const PaletteComponent &palette, int colorCount, RasterComponent &raster, CelIndex celIndex, IColorShiftCallback &callback, bool applyToAll, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(BlurDialog::IDD, pParent), _palette(palette), _rasterActual(raster), _initialized(false), _sigma(g_lastSigma), _boost(g_lastBoost), _celIndex(celIndex), _callback(callback), _applyToAll(applyToAll)
{
    _rasterSnapshot.reset(static_cast<RasterComponent*>(_rasterActual.Clone()));
}

BEGIN_MESSAGE_MAP(BlurDialog, CDialog)
    //ON_BN_CLICKED(IDC_RADIOVIEW, &BlurDialog::OnBnClickedRadioview)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()

void BlurDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    if (!_initialized)
    {
        DDX_Control(pDX, IDC_SLIDERSIGMA, m_wndSliderSigma);
        m_wndSliderSigma.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderSigma.SetRange(0, 1000, TRUE);
        m_wndSliderSigma.SetPos(_sigma);
        DDX_Control(pDX, IDC_SLIDERBOOST, m_wndSliderBoost);
        m_wndSliderBoost.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderBoost.SetRange(0, 1000, TRUE);
        m_wndSliderBoost.SetPos(_boost);


        DDX_Control(pDX, IDOK, m_wndOk);
        DDX_Control(pDX, IDCANCEL, m_wndCancel);
        _initialized = true;
    }

    DDX_Slider(pDX, IDC_SLIDERSIGMA, _sigma);
    DDX_Slider(pDX, IDC_SLIDERBOOST, _boost);
}

void BlurCel(Cel &cel, float sigma, float boost)
{

}

void GaussianKernel(float sigma, std::array<float, KernelSize> &kernel)
{
    float sigmaSqr2 = sigma * sigma * 2.0f;
    float sigmaSqr2piInv = 1.0f / sqrtf((float)3.141592f * sigmaSqr2);

    int k = KernelSize / 2;

    for (int x = 0; x < KernelSize; x++)
    {
        int kx = x - k;
        int y = k;
        int ky = y - k;
        float g = 0.0f;

        float expo = expf((-(((float)kx * (float)kx + (float)ky * (float)ky) / sigmaSqr2)));
        g = sigmaSqr2piInv * expo;

        kernel[x] = g;
    }

    float sum = 0.0f;
    for (float f : kernel)
    {
        sum += f;
    }
    for (int x = 0; x < KernelSize; x++)
    {
        kernel[x] /= sum;
    }
}

void BlurDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd)
{
    UpdateData(TRUE);
    _UpdateView();
}


// Call this when params change
void BlurDialog::_UpdateView()
{
    if (_rasterSnapshot)
    {
        std::array<float, KernelSize> kernel;
        GaussianKernel((float)_sigma / 100.0f, kernel);

        if (_applyToAll) // To all cels in loop
        {
            for (int nCel = 0; nCel < (int)_rasterActual.Loops[_celIndex.loop].Cels.size(); nCel++)
            {
                CelIndex index = CelIndex(_celIndex.loop, nCel);
                Cel &celActual = _rasterActual.GetCel(index);

                UpdateMirrors(_rasterActual, index);
            }
        }
        else
        {
            UpdateMirrors(_rasterActual, _celIndex);
        }
    }
    _callback.OnColorShift();
}


void BlurDialog::OnOK()
{
    g_lastBoost = _boost;
    g_lastSigma = _sigma;
    __super::OnOK();
}