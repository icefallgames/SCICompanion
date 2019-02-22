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

using namespace std;
using namespace fmt;

BlurSettings g_blurSettings = { 200, 100, TRUE, TRUE, FALSE, TRUE, TRUE };


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

struct GreyFormat
{
    GreyFormat(float f) : _value(f) {}
    static GreyFormat ToFormat(byte b, const PaletteComponent &palette)
    {
        return GreyFormat((float)b / 255.0f);
    }
    static GreyFormat Default()
    {
        return GreyFormat(0);
    }
    byte ToByte(const PaletteComponent &palette)
    {
        return (byte)min(255, (int)(_value * 255.0f));
    }

    void Scale(float scale)
    {
        _value *= scale;
    }
    void Add(const GreyFormat &other)
    {
        _value += other._value;
    }

    float _value;
};


struct RGBFormat
{
    RGBFormat(float r, float g, float b) : _r(r), _g(g), _b(b) {}
    static RGBFormat ToFormat(byte b, const PaletteComponent &palette)
    {
        RGBQUAD rgb = palette.Colors[b];
        return RGBFormat((float)rgb.rgbRed / 255.0f, (float)rgb.rgbGreen / 255.0f, (float)rgb.rgbBlue / 255.0f);
    }
    static RGBFormat Default()
    {
        return RGBFormat(0, 0, 0);
    }
    byte ToByte(const PaletteComponent &palette)
    {
        RGBQUAD color;
        color.rgbRed = (byte)min(255, (int)(_r * 255.0f));
        color.rgbGreen = (byte)min(255, (int)(_g * 255.0f));
        color.rgbBlue = (byte)min(255, (int)(_b * 255.0f));
        return FindBestPaletteIndexNoTx(palette, color);
    }

    void Scale(float scale)
    {
        _r *= scale;
        _g *= scale;
        _b *= scale;
    }
    void Add(const RGBFormat &other)
    {
        _r += other._r;
        _g += other._g;
        _b += other._b;
    }

    float _r, _g, _b;
};


template<class _TColorFormat>
void BlurCel(const Cel &celSource, Cel &cel, std::array<float, KernelSize> &kernel, const BlurSettings &settings, const PaletteComponent &palette)
{
    int k = KernelSize / 2;
    Cel celFirstPass = celSource;
    int width = celSource.size.cx;
    int widthMult = celSource.size.cx * k;
    int heightMult = celSource.size.cy * k;
    int height = celSource.size.cy;

    if (settings.fXBlur)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int indexDest = y * cel.GetStride() + x;
                _TColorFormat value = _TColorFormat::Default();
                for (int i = -k; i <= k; i++)
                {
                    int xTemp = (x + i);
                    xTemp = settings.fXWrap ? ((xTemp + widthMult) % width) : min((width - 1), max(0, xTemp));
                    int indexSource = y * cel.GetStride() + xTemp;
                    _TColorFormat temp = _TColorFormat::ToFormat(celSource.Data[indexSource], palette);
                    temp.Scale(kernel[i + k]);
                    value.Add(temp);
                }
                celFirstPass.Data[indexDest] = value.ToByte(palette);
            }
        }
    }
    else
    {
        celFirstPass = celSource;
    }
    
    if (settings.fYBlur)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                int indexDest = y * cel.GetStride() + x;
                _TColorFormat value = _TColorFormat::Default();
                for (int i = -k; i <= k; i++)
                {
                    int yTemp = y + i;
                    yTemp = settings.fYWrap ? ((yTemp + heightMult) % height) : min((height - 1), max(0, yTemp));
                    int indexSource = yTemp * cel.GetStride() + x;
                    _TColorFormat temp = _TColorFormat::ToFormat(celFirstPass.Data[indexSource], palette);
                    temp.Scale(kernel[i + k]);
                    value.Add(temp);

                }
                cel.Data[indexDest] = value.ToByte(palette);
            }
        }
    }
    else
    {
        cel = celFirstPass;
    }
}

void GaussianKernel(float sigma, std::array<float, KernelSize> &kernel, float boost)
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
        std::array<float, KernelSize> kernel;
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