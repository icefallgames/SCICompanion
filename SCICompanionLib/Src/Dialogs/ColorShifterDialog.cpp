#include "stdafx.h"
#include "ColorShifterDialog.h"
#include "PaletteOperations.h"
#include "ImageUtil.h"
#include "RasterOperations.h"
#include "View.h"

using namespace std;

ColorShifterDialog::ColorShifterDialog(const PaletteComponent &palette, RasterComponent &raster, CelIndex celIndex, IColorShiftCallback &callback, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(ColorShifterDialog::IDD, pParent), _palette(palette), _rasterActual(raster), _shiftExtent(ShiftExtent::Cel), _celIndex(celIndex), _callback(callback)
{
    // This is the path used for dialog-based guys.
    // CountActualUsedColors(cels, _actualUsedColors);
}

BEGIN_MESSAGE_MAP(ColorShifterDialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTONUP, &ColorShifterDialog::OnBnClickedButtonup)
    ON_BN_CLICKED(IDC_BUTTONDOWN, &ColorShifterDialog::OnBnClickedButtondown)
    ON_BN_CLICKED(IDC_BUTTONRIGHT, &ColorShifterDialog::OnBnClickedButtonright)
    ON_BN_CLICKED(IDC_BUTTONLEFT, &ColorShifterDialog::OnBnClickedButtonleft)
    ON_BN_CLICKED(IDC_RADIOCEL, &ColorShifterDialog::OnBnClickedRadiocel)
    ON_BN_CLICKED(IDC_RADIOLOOP, &ColorShifterDialog::OnBnClickedRadioloop)
    ON_BN_CLICKED(IDC_RADIOVIEW, &ColorShifterDialog::OnBnClickedRadioview)
END_MESSAGE_MAP()

void ColorShifterDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    DDX_Control(pDX, IDC_STATIC1, m_wndStatic);
    bool initialSelection[256] = {};
    m_wndStatic.SetPalette(16, 16, reinterpret_cast<const EGACOLOR *>(_palette.Mapping), 256, _palette.Colors, false);
    m_wndStatic.SetShowHover(false);
    m_wndStatic.SetSelection(initialSelection);
    m_wndStatic.ShowSelection(TRUE);
    m_wndStatic.SetAutoHandleSelection(true);
    m_wndStatic.ShowUnused(true);
    m_wndStatic.SetCallback(this);
    m_wndStatic.SetActualUsedColors(_actualUsedColors);

    DDX_Control(pDX, IDC_EDITRANGE, m_wndEditRange);
    DDX_Control(pDX, IDC_EDIT1, m_wndEditDescription);

    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);

    DDX_Control(pDX, IDC_BUTTONUP, m_wndUp);
    DDX_Control(pDX, IDC_BUTTONDOWN, m_wndDown);
    DDX_Control(pDX, IDC_BUTTONLEFT, m_wndLeft);
    DDX_Control(pDX, IDC_BUTTONRIGHT, m_wndRight);

    DDX_Control(pDX, IDC_RADIOCEL, m_wndCel);
    m_wndCel.SetCheck(BST_CHECKED);
    DDX_Control(pDX, IDC_RADIOLOOP, m_wndLoop);
    DDX_Control(pDX, IDC_RADIOVIEW, m_wndView);

    DDX_Control(pDX, IDC_STATICGROUP, m_wndGroup);

    if (!m_font.GetSafeHandle())
    {
        LOGFONT logFont = { 0 };
        logFont.lfHeight = -18;
        logFont.lfCharSet = SYMBOL_CHARSET;
        logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
        logFont.lfWeight = FW_NORMAL;
        StringCchCopy(logFont.lfFaceName, ARRAYSIZE(logFont.lfFaceName), TEXT("WingDings"));
        m_font.CreateFontIndirect(&logFont);

        m_wndUp.SetFont(&m_font);
        m_wndDown.SetFont(&m_font);
        m_wndLeft.SetFont(&m_font);
        m_wndRight.SetFont(&m_font);

        m_wndEditDescription.SetWindowTextA(
            "Shift-click selects a range of colors. Ctrl-click toggles selection. Use the arrow keys or direction buttons to shift the selected colors."
            );

        _UpdateView();
    }
}

void ColorShifterDialog::_UpdateView()
{
    if (_rasterSnapshot)
    {
        for (int nLoop = 0; nLoop < _rasterActual.LoopCount(); nLoop++)
        {
            for (int nCel = 0; nCel < (int)_rasterActual.Loops[nLoop].Cels.size(); nCel++)
            {
                if ((_shiftExtent == ShiftExtent::View) ||
                    ((_shiftExtent == ShiftExtent::Loop) && (_celIndex.loop == nLoop)) ||
                    ((_shiftExtent == ShiftExtent::Cel) && (_celIndex.loop == nLoop) && (_celIndex.cel == nCel)))
                {
                    Cel &celActual = _rasterActual.Loops[nLoop].Cels[nCel];
                    const Cel &celStart = _rasterSnapshot->Loops[nLoop].Cels[nCel];
                    // Adjust this cel.
                    for (int y = 0; y < celActual.size.cy; y++)
                    {
                        int index = y * CX_ACTUAL(celActual.size.cx);
                        for (int x = 0; x < celActual.size.cx; x++)
                        {
                            uint8_t value = celStart.Data[index];
                            if (_startingSelection[value])
                            {
                                celActual.Data[index] = value + _totalShift; // Wrap is ok. Desired.
                            }
                            index++;
                        }
                    }
                    UpdateMirrors(_rasterActual, CelIndex(nLoop, nCel));
                }
            }
        }
    }
    _CountUsedColors();
    _callback.OnColorShift();
}

// Call this on evert shift, and on
void ColorShifterDialog::_CountUsedColors()
{
    vector<const Cel*> activeCels;
    for (int nLoop = 0; nLoop < _rasterActual.LoopCount(); nLoop++)
    {
        for (int nCel = 0; nCel < (int)_rasterActual.Loops[nLoop].Cels.size(); nCel++)
        {
            if ((_shiftExtent == ShiftExtent::View) ||
                ((_shiftExtent == ShiftExtent::Loop) && (_celIndex.loop == nLoop)) ||
                ((_shiftExtent == ShiftExtent::Cel) && (_celIndex.loop == nLoop) && (_celIndex.cel == nCel)))
            {
                activeCels.push_back(&_rasterActual.Loops[nLoop].Cels[nCel]);
            }
        }
    }
    bool usedColors[256];
    CountActualUsedColors(activeCels, usedColors);
    m_wndStatic.SetActualUsedColors(usedColors);
}

void ColorShifterDialog::_SyncSelection()
{
    std::vector<std::pair<uint8_t, uint8_t>> ranges = GetSelectedRanges(m_wndStatic);
    std::string rangeText = GetRangeText(ranges);
    m_wndEditRange.SetWindowTextA(rangeText.c_str());
    m_wndStatic.GetMultipleSelection(_startingSelection);
}

void ColorShifterDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeft)
{
    _SyncSelection();
    // Now take a snapshot so we have something to start from, and reset the shift.
    _rasterSnapshot.reset(static_cast<RasterComponent*>(_rasterActual.Clone()));
    _totalShift = 0;
    // Hack: Take focus, since if focus was on the "extent" radio buttons, they will eat arrow keys.
    m_wndStatic.SetFocus();
}

void ColorShifterDialog::_Shift(int offset)
{
    _totalShift += offset;
    if (_totalShift < 0)
    {
        _totalShift += 256;
    }
    _totalShift %= 256;
    // This is the tricky part.
    // 1) Get the selection indices. The *original* ones from the last selection change (we'll need to stash it)
    // 2) Calculate the total shift
    // 3) Go through the cels' data and adjust all the indices.
    // 4) Update the selection in the palette. Also update the used indices.

    bool newSelection[256];
    for (int i = 0; i < ARRAYSIZE(newSelection); i++)
    {
        newSelection[(i + _totalShift) % ARRAYSIZE(newSelection)] = _startingSelection[i];
    }
    m_wndStatic.SetSelection(newSelection);
    m_wndStatic.Invalidate();

    _UpdateView();
}

BOOL ColorShifterDialog::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (pMsg->message == WM_KEYDOWN)
    {
        fRet = TRUE;
        switch (pMsg->wParam)
        {
            case VK_UP:
                _Shift(-16);
                break;
            case VK_DOWN:
                _Shift(16);
                break;
            case VK_RIGHT:
                _Shift(1);
                break;
            case VK_LEFT:
                _Shift(-1);
                break;
            default:
                fRet = FALSE;
        }
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

void ColorShifterDialog::OnBnClickedButtonup()
{
    _Shift(-16);
}


void ColorShifterDialog::OnBnClickedButtondown()
{
    _Shift(16);
}


void ColorShifterDialog::OnBnClickedButtonright()
{
    _Shift(1);
}

void ColorShifterDialog::OnBnClickedButtonleft()
{
    _Shift(-1);
}

void ColorShifterDialog::OnBnClickedRadiocel()
{
    _shiftExtent = ShiftExtent::Cel;
    _UpdateView();
}


void ColorShifterDialog::OnBnClickedRadioloop()
{
    _shiftExtent = ShiftExtent::Loop;
    _UpdateView();
}


void ColorShifterDialog::OnBnClickedRadioview()
{
    _shiftExtent = ShiftExtent::View;
    _UpdateView();
}
