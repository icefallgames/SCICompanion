#include "stdafx.h"
#include "ColorAdjustDialog.h"
#include "PaletteOperations.h"
#include "format.h"

using namespace std;

ColorAdjustDialog::ColorAdjustDialog(PaletteComponent &palette, IVGAPaletteDefinitionCallback *callback, bool *selection, CWnd* pParent)
    : CExtResizableDialog(ColorAdjustDialog::IDD, pParent), _palette(palette), _paletteOriginal(palette), _callback(callback), _selection(selection), _initialized(false)
{
    _crTint = RGB(255, 255, 255);
    _hue = 0;
    _saturation = 0;
    _brightness = 0;
    _tintStrength = 255;
}

void ColorAdjustDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    if (!_initialized)
    {
        // Visuals
        DDX_Control(pDX, IDC_STATICHUE, m_wndStatic1);
        DDX_Control(pDX, IDC_STATICSATURATION, m_wndStatic2);
        DDX_Control(pDX, IDC_STATICBRIGHTNESS, m_wndStatic3);
        DDX_Control(pDX, IDC_STATICTINTSTRENGTH, m_wndStatic5);
        DDX_Control(pDX, IDOK, m_wndOk);
        DDX_Control(pDX, IDCANCEL, m_wndCancel);
        DDX_Control(pDX, IDC_BUTTONCOLOR, m_wndColor);

        DDX_Control(pDX, IDC_EDITHUE, m_wndEditHue);
        DDX_Control(pDX, IDC_EDITSATURATION, m_wndEditSaturation);
        DDX_Control(pDX, IDC_EDITBRIGHTNESS, m_wndEditBrightness);
        DDX_Control(pDX, IDC_EDITTINTSTRENGTH, m_wndEditTintStrength);

        DDX_Control(pDX, IDC_SLIDERHUE, m_wndSliderHue);
        m_wndSliderHue.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderHue.SetRange(-180, 180, TRUE);
        m_wndSliderHue.SetPos(0);
        DDX_Control(pDX, IDC_SLIDERSATURATION, m_wndSliderSaturation);
        m_wndSliderSaturation.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderSaturation.SetRange(-100, 100, TRUE);
        m_wndSliderSaturation.SetPos(0);
        DDX_Control(pDX, IDC_SLIDERBRIGHTNESS, m_wndSliderBrightness);
        m_wndSliderBrightness.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderBrightness.SetRange(-100, 100, TRUE);
        m_wndSliderBrightness.SetPos(0);
        DDX_Control(pDX, IDC_SLIDERTINTSTRENGTH, m_wndSliderTintStrength);
        m_wndSliderTintStrength.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderTintStrength.SetRange(0, 255, TRUE);
        m_wndSliderTintStrength.SetPos(255);

        _PushToEdit();

        _initialized = true;
    }

    DDX_Slider(pDX, IDC_SLIDERHUE, _hue);
    DDX_Slider(pDX, IDC_SLIDERSATURATION, _saturation);
    DDX_Slider(pDX, IDC_SLIDERBRIGHTNESS, _brightness);
    DDX_Slider(pDX, IDC_SLIDERTINTSTRENGTH, _tintStrength);
    m_wndColor.SetColorAndState(RGBQUADFromCOLORREF(_crTint), TRUE);
    _SyncPalette();
    _PushToEdit();
}

BEGIN_MESSAGE_MAP(ColorAdjustDialog, CExtResizableDialog)
    ON_EN_KILLFOCUS(IDC_EDITHUE, &ColorAdjustDialog::OnEnKillfocusEdithue)
    ON_EN_KILLFOCUS(IDC_EDITSATURATION, &ColorAdjustDialog::OnEnKillfocusEditsaturation)
    ON_EN_KILLFOCUS(IDC_EDITBRIGHTNESS, &ColorAdjustDialog::OnEnKillfocusEditbrightness)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_BUTTONCOLOR, &ColorAdjustDialog::OnBnClickedButtoncolor)
    ON_EN_KILLFOCUS(IDC_EDITTINTSTRENGTH, &ColorAdjustDialog::OnEnKillfocusEdittintstrength)
END_MESSAGE_MAP()

void ColorAdjustDialog::_PushToEdit()
{
    m_wndEditHue.SetWindowText(fmt::format("{0}", m_wndSliderHue.GetPos()).c_str());
    m_wndEditSaturation.SetWindowText(fmt::format("{0}", m_wndSliderSaturation.GetPos()).c_str());
    m_wndEditBrightness.SetWindowText(fmt::format("{0}", m_wndSliderBrightness.GetPos()).c_str());
    m_wndEditTintStrength.SetWindowText(fmt::format("{0}", m_wndSliderTintStrength.GetPos()).c_str());
}

void ColorAdjustDialog::_SyncPalette()
{
    double hue = (double)_hue / 180.0;
    double sat = (double)_saturation / 100.0;
    double lum = (double)_brightness / 100.0;
    for (size_t i = 0; i < 256; i++)
    {
        if (_selection[i])
        {
            RGBQUAD orig = _paletteOriginal.Colors[i];
            COLORREF crOld = RGB(orig.rgbRed, orig.rgbGreen, orig.rgbBlue);
            COLORREF crNew = CExtBitmap::stat_HLS_Adjust(crOld, hue, lum, sat);

            int r = (int)GetRValue(crNew) * (int)GetRValue(_crTint) / 255;
            int g = (int)GetGValue(crNew) * (int)GetGValue(_crTint) / 255;
            int b = (int)GetBValue(crNew) * (int)GetBValue(_crTint) / 255;
            COLORREF crNewTinted = RGB(r, g, b);
            crNew = CExtBitmap::stat_RGB_Blend(crNewTinted, crNew, _tintStrength);

            _palette.Colors[i] = RGBQUADFromCOLORREF(crNew);
            _palette.Colors[i].rgbReserved = orig.rgbReserved;
        }
    }
    if (_callback)
    {
        _callback->OnVGAPaletteChanged();
    }
}

int GetNumber(CExtEdit &wndEdit)
{
    CString textStr;
    wndEdit.GetWindowTextA(textStr);
    string text = (PCSTR)textStr;
    size_t position = 0;
    int number = stoi(text, &position);
    return number;
}

void ColorAdjustDialog::OnEnKillfocusEdithue()
{
    m_wndSliderHue.SetPos(GetNumber(m_wndEditHue));
    UpdateData(TRUE);
}


void ColorAdjustDialog::OnEnKillfocusEditsaturation()
{
    m_wndSliderSaturation.SetPos(GetNumber(m_wndEditSaturation));
    UpdateData(TRUE);
}


void ColorAdjustDialog::OnEnKillfocusEditbrightness()
{
    m_wndSliderBrightness.SetPos(GetNumber(m_wndEditBrightness));
    UpdateData(TRUE);
}

void ColorAdjustDialog::OnEnKillfocusEdittintstrength()
{
    m_wndSliderTintStrength.SetPos(GetNumber(m_wndEditTintStrength));
    UpdateData(TRUE);
}

void ColorAdjustDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd)
{
    UpdateData(TRUE);
}

void ColorAdjustDialog::OnBnClickedButtoncolor()
{
    CExtColorDlg colorDialog(_crTint, _crTint);
    colorDialog.m_strCaption = "Choose tint color";
    if (IDOK == colorDialog.DoModal())
    {
        _crTint = colorDialog.m_clrNew;
    }
    UpdateData(TRUE);
}
