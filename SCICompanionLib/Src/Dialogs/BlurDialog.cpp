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
#include "format.h"
#include "Resample.h"

using namespace std;
using namespace fmt;

BlurSettings g_blurSettings(200, 100, FALSE, TRUE, TRUE, TRUE, TRUE);

const int KernelSize = 7;

BlurDialog::BlurDialog(const PaletteComponent &palette, int colorCount, RasterComponent &raster, CelIndex celIndex, IColorShiftCallback &callback, bool applyToAll, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(BlurDialog::IDD, pParent),
    _palette(palette),
    _rasterActual(raster),
    _initialized(false),
    _settings(g_blurSettings),
    _celIndex(celIndex),
    _callback(callback),
    _applyToAll(applyToAll)
{
    _rasterSnapshot.reset(static_cast<RasterComponent*>(_rasterActual.Clone()));
}

BEGIN_MESSAGE_MAP(BlurDialog, CDialog)
    ON_BN_CLICKED(IDC_CHECKXWRAP, &BlurDialog::OnBnClickedSomeCheckbox)
    ON_BN_CLICKED(IDC_CHECKYWRAP, &BlurDialog::OnBnClickedSomeCheckbox)
    ON_BN_CLICKED(IDC_CHECKXBLUR, &BlurDialog::OnBnClickedSomeCheckbox)
    ON_BN_CLICKED(IDC_CHECKYBLUR, &BlurDialog::OnBnClickedSomeCheckbox)
    ON_BN_CLICKED(IDC_CHECKCOLORMATCH, &BlurDialog::OnBnClickedSomeCheckbox)
    ON_WM_HSCROLL() // for sliders
END_MESSAGE_MAP()

void BlurDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    if (!_initialized)
    {
        DDX_Control(pDX, IDC_SLIDERSIGMA, m_wndSliderSigma);
        m_wndSliderSigma.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderSigma.SetRange(10, 1000, TRUE);
        m_wndSliderSigma.SetPos(_settings.sigma);
        DDX_Control(pDX, IDC_SLIDERBOOST, m_wndSliderBoost);
        m_wndSliderBoost.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderBoost.SetRange(100, 1000, TRUE);
        m_wndSliderBoost.SetPos(_settings.boost);

        DDX_Control(pDX, IDC_EDITSIGMA, m_wndSigmaDisplay);
        DDX_Control(pDX, IDC_EDITBOOST, m_wndBoostDisplay);
        DDX_Control(pDX, IDC_CHECKXWRAP, m_wndXWrap);
        DDX_Control(pDX, IDC_CHECKYWRAP, m_wndYWrap);
        DDX_Control(pDX, IDC_CHECKXBLUR, m_wndXBlur);
        DDX_Control(pDX, IDC_CHECKYBLUR, m_wndYBlur);
        DDX_Control(pDX, IDC_CHECKCOLORMATCH, m_wndColorMatch);

        DDX_Control(pDX, IDOK, m_wndOk);
        DDX_Control(pDX, IDCANCEL, m_wndCancel);
        _initialized = true;

        _UpdateView();
    }

    DDX_Slider(pDX, IDC_SLIDERSIGMA, _settings.sigma);
    DDX_Slider(pDX, IDC_SLIDERBOOST, _settings.boost);
    DDX_Check(pDX, IDC_CHECKXWRAP, _settings.fXWrap);
    DDX_Check(pDX, IDC_CHECKYWRAP, _settings.fYWrap);
    DDX_Check(pDX, IDC_CHECKXBLUR, _settings.fXBlur);
    DDX_Check(pDX, IDC_CHECKYBLUR, _settings.fYBlur);
    DDX_Check(pDX, IDC_CHECKCOLORMATCH, _settings.fColorMatch);
}

void GaussianKernel(float sigma, std::vector<float> &kernel, float boost)
{
    //int kernelSize = 7;
    float sigmaSqr2 = sigma * sigma * 2.0f;
    float sigmaSqr2piInv = 1.0f / sqrtf((float)3.141592f * sigmaSqr2);

    // Try to chooce a good kernel size...
    float smallValue = 0.01f; // small number - weight near the ends of the kernel.
    int tempK = -logf(smallValue / sigmaSqr2piInv) * sigmaSqr2;
    tempK = (int)roundf(sqrt(tempK));
    int kernelSize = tempK * 2 + 1;
    int k = kernelSize / 2;

    for (int x = 0; x < kernelSize; x++)
    {
        int kx = x - k;
        int y = k;
        int ky = y - k;
        float g = 0.0f;

        float expo = expf((-(((float)kx * (float)kx + (float)ky * (float)ky) / sigmaSqr2)));
        g = sigmaSqr2piInv * expo;

        kernel.push_back(g);
    }

    float sum = 0.0f;
    for (float f : kernel)
    {
        sum += f;
    }
    for (int x = 0; x < kernelSize; x++)
    {
        kernel[x] /= sum;
        kernel[x] *= boost;
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
    float sigma = (float)_settings.sigma / 100.0f;
    float boost = (float)_settings.boost / 100.0f;
    m_wndSigmaDisplay.SetWindowTextA(format("{0}", sigma).c_str());
    m_wndBoostDisplay.SetWindowTextA(format("{0}", boost).c_str());


    if (_rasterSnapshot)
    {
        std::vector<float> kernel;
        GaussianKernel(sigma, kernel, boost);

        if (_applyToAll) // To all cels in loop
        {
            for (int nCel = 0; nCel < (int)_rasterActual.Loops[_celIndex.loop].Cels.size(); nCel++)
            {
                CelIndex index = CelIndex(_celIndex.loop, nCel);
                if (_settings.fColorMatch)
                {
                    BlurCel<RGBFormat>(_rasterSnapshot->GetCel(index), _rasterActual.GetCel(index), kernel, _settings, _palette);
                }
                else
                {
                    BlurCel<GreyFormat>(_rasterSnapshot->GetCel(index), _rasterActual.GetCel(index), kernel, _settings, _palette);
                }
                UpdateMirrors(_rasterActual, index);
            }
        }
        else
        {
            if (_settings.fColorMatch)
            {
                BlurCel<RGBFormat>(_rasterSnapshot->GetCel(_celIndex), _rasterActual.GetCel(_celIndex), kernel, _settings, _palette);
            }
            else
            {
                BlurCel<GreyFormat>(_rasterSnapshot->GetCel(_celIndex), _rasterActual.GetCel(_celIndex), kernel, _settings, _palette);
            }
            UpdateMirrors(_rasterActual, _celIndex);
        }
    }
    _callback.OnColorShift();
}


void BlurDialog::OnOK()
{
    g_blurSettings = _settings;
    __super::OnOK();
}

void BlurDialog::OnBnClickedSomeCheckbox()
{
    UpdateData(TRUE);
    _UpdateView();

}