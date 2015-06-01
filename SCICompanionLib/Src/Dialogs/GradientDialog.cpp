#include "stdafx.h"
#include "GradientDialog.h"
#include "PaletteOperations.h"

GradientDialog::GradientDialog(PaletteComponent &palette, uint8_t start, uint8_t end, CWnd* pParent)
    : CExtResizableDialog(GradientDialog::IDD, pParent), _palette(palette), _start(start), _endInclusive(end)
{
    // 256 things here.
    m_wndStatic.SetPalette(16, 16, reinterpret_cast<const EGACOLOR*>(palette.Mapping), ARRAYSIZE(palette.Mapping), palette.Colors, false);
    m_wndStatic.ShowSelection(FALSE);



    memset(&_edge, 0, sizeof(_edge));
    memset(&_center, 255, sizeof(_center));
}

void GradientDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStatic);

    // Visuals
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_BUTTONCENTER, m_wndCenter);
    DDX_Control(pDX, IDC_BUTTONEDGES, m_wndEdges);

    _SyncPalette();
}

BEGIN_MESSAGE_MAP(GradientDialog, CExtResizableDialog)
    ON_BN_CLICKED(IDC_BUTTONCENTER, &GradientDialog::OnBnClickedButtoncenter)
    ON_BN_CLICKED(IDC_BUTTONEDGES, &GradientDialog::OnBnClickedButtonedges)
END_MESSAGE_MAP()

void GradientDialog::_SyncPalette()
{
    int centerIndex = ((int)_endInclusive + (int)_start) / 2;
    RGBQUAD centerRGB = RGBQUADFromCOLORREF(_center);
    RGBQUAD edgeRGB = RGBQUADFromCOLORREF(_edge);
    for (int i = _start; i <= (int)_endInclusive; i++)
    {
        int progression1000 = min(1000, abs(centerIndex - i) * 1000 / (centerIndex - _start));
        int invProgression1000 = 1000 - progression1000;
        // 1 at edges, 0 at center. 
        _palette.Colors[i].rgbBlue = (progression1000 * edgeRGB.rgbBlue + invProgression1000 * centerRGB.rgbBlue) / 1000;
        _palette.Colors[i].rgbRed = (progression1000 * edgeRGB.rgbRed + invProgression1000 * centerRGB.rgbRed) / 1000;
        _palette.Colors[i].rgbGreen = (progression1000 * edgeRGB.rgbGreen + invProgression1000 * centerRGB.rgbGreen) / 1000;
    }

    m_wndStatic.SetPalette(16, 16, reinterpret_cast<const EGACOLOR*>(_palette.Mapping), ARRAYSIZE(_palette.Mapping), _palette.Colors, false);
    m_wndStatic.Invalidate(FALSE);
}

void GradientDialog::OnBnClickedButtoncenter()
{
    CExtColorDlg colorDialog(_center, _center);
    colorDialog.m_strCaption = "Edit center color";
    if (IDOK == colorDialog.DoModal())
    {
        _center = colorDialog.m_clrNew;
    }
    _SyncPalette();
}

void GradientDialog::OnBnClickedButtonedges()
{
    CExtColorDlg colorDialog(_edge, _edge);
    colorDialog.m_strCaption = "Edit center color";
    if (IDOK == colorDialog.DoModal())
    {
        _edge = colorDialog.m_clrNew;
    }
    _SyncPalette();
}
