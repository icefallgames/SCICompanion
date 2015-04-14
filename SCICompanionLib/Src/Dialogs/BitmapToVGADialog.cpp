#include "stdafx.h"
#include "resource.h"
#include "BitmapToVGADialog.h"
#include "PaletteOperations.h"
#include "AppState.h"
#include "format.h"
#include "ColorQuantization.h"
#include "Components.h"
#include "RasterOperations.h"
#include "ImageUtil.h"

using namespace std;
using namespace fmt;

#define FROMCLIPBOARD_TIMER 2345

int g_iScaleImageVGA = 0;   // Don't scale by default

// The end result of this dialog is image data (a Cel) and possibly a palette

CBitmapToVGADialog::CBitmapToVGADialog(const PaletteComponent *targetPalette, bool allowInsertAtCurrentPosition, CWnd* pParent /*=nullptr*/)
    : CExtNCW<CExtResizableDialog>(CBitmapToVGADialog::IDD, pParent),
    PrepareBitmapBase(-1, IDC_STATICORIG)
{
    _transparentColor = 255;
    _allowInsertAtCurrentPosition = allowInsertAtCurrentPosition;
    _needsUpdate = true;
    _paletteAlgorithm = 0;
    _insertAtCurrentPosition = 0;

    const PaletteComponent *globalPalette = appState->GetResourceMap().GetPalette999();
    _numUnusedPaletteEntriesInGlobalPalette = 256;
    if (globalPalette)
    {
        _numUnusedPaletteEntriesInGlobalPalette = 0;
        _globalPalette = make_unique<PaletteComponent>(*globalPalette);
        for (int i = 0; i < 256; i++)
        {
            if (_globalPalette->Colors[i].rgbReserved == 0x0)
            {
                _numUnusedPaletteEntriesInGlobalPalette++;
            }
        }

        if (_numUnusedPaletteEntriesInGlobalPalette == 0)
        {
            // Some games (SQ4) don't have any entries marked "unused" in the global palette.
            // There are no great options here (other than allowing the user to specify a range)
            // So instead we'll try to detect which ones are unused by seeing which ones are black.
            // Assume 0 and 255 are used.
            for (int i = 1; i < 255; i++)
            {
                RGBQUAD color = _globalPalette->Colors[i];
                if ((color.rgbBlue == 0) && (color.rgbRed == 0) && (color.rgbGreen == 0))
                {
                    // Officially mark it as unused.
                    _globalPalette->Colors[i].rgbReserved = 0x0;
                    _numUnusedPaletteEntriesInGlobalPalette++;
                }
            }
        }
    }

    if (targetPalette)
    {
        _targetCurrentPalette = make_unique<PaletteComponent>(*targetPalette);
    }

    _pCRBitmap = nullptr;
    _iScaleImage = g_iScaleImageVGA;
    _honorGlobalPalette = true; // Always honour by default
    _size.SetSize(0, 0);

    _pbmpNormal = nullptr;
    _pbmpScaled = nullptr;
    _pbmpCurrent = nullptr;
    _bContrastCenterNormal = 0x80;
    _bContrastCenterScaled = 0x80;

    _nBrightness = 50;
    _nContrast = 50;
    _nSaturation = 50;

    _fInitializedControls = false;
}

std::unique_ptr<Cel> CBitmapToVGADialog::GetFinalResult()
{
    return move(_finalResult);
}

std::unique_ptr<PaletteComponent> CBitmapToVGADialog::GetFinalResultPalette()
{
    return move(_finalResultPalette);
}

CBitmapToVGADialog::~CBitmapToVGADialog()
{
    delete[] _pCRBitmap;
    delete _pbmpNormal;
    delete _pbmpScaled;
    delete _pbmpCurrent;

    g_iScaleImageVGA = _iScaleImage;
}

void CBitmapToVGADialog::DoDataExchange(CDataExchange* pDX)
{
    int nBrightness = _nBrightness;
    int nContrast = _nContrast;
    int nSaturation = _nSaturation;
    int iScale = _iScaleImage;

    CExtNCW<CExtResizableDialog>::DoDataExchange(pDX);

    if (!_fInitializedControls)
    {
        // Necessary for prof-uis
        DDX_Control(pDX, IDC_SLIDERBRIGHTNESS, m_wndSlider2);
        m_wndSlider2.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        DDX_Control(pDX, IDC_SLIDERCONTRAST, m_wndSlider3);
        m_wndSlider3.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        DDX_Control(pDX, IDC_SLIDERSATURATION, m_wndSlider4);
        m_wndSlider4.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        DDX_Control(pDX, IDC_BUTTONBROWSE, m_wndButton2);
        DDX_Control(pDX, IDOK, m_wndButton3);
        DDX_Control(pDX, IDCANCEL, m_wndButton4);
        DDX_Control(pDX, IDC_STATICGROUP1, m_wndGroup1);
        DDX_Control(pDX, IDC_STATICGROUP2, m_wndGroup2);
        DDX_Control(pDX, IDC_CHECKSCALE, m_wndCheck1);
        DDX_Control(pDX, IDC_CHECKGLOBALPALETTE, m_wndCheckGlobalPalette);
        m_wndCheckGlobalPalette.SetCheck(1);    // Checked by default.
        DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
        DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
        DDX_Control(pDX, IDC_STATIC3, m_wndLabel3);
        DDX_Control(pDX, IDC_RADIO1, m_wndRadio1);
        DDX_Control(pDX, IDC_RADIO2, m_wndRadio2);
        DDX_Control(pDX, IDC_RADIO3, m_wndRadio3);
        DDX_Control(pDX, IDC_CHECKINSERT, m_wndCheckInsert);

        DDX_Control(pDX, IDC_STATICGROUP3, m_wndGroup3);
        DDX_Control(pDX, IDC_EDITTRANSPARENTCOLOR, m_wndEditTransparentColor);
        SetDlgItemInt(IDC_EDITTRANSPARENTCOLOR, _transparentColor, FALSE);
        DDX_Control(pDX, IDC_STATICTRANSLABEL, m_wndLabel4);
        DDX_Control(pDX, IDC_CHECKDONTUSEINPALETTE, m_wndCheckDontUseInPalette);
        m_wndCheckDontUseInPalette.SetCheck(1);
        
        m_wndRadio1.SetCheck(1); // Check the first guy by default
        _paletteAlgorithm = 0;
        _fInitializedControls = true;
    }

    m_wndCheckGlobalPalette.SetWindowTextA(format("Only use {0} free palette entries", _numUnusedPaletteEntriesInGlobalPalette).c_str());

    DDX_Control(pDX, IDC_EDITSTATUS, m_wndEditStatus);
    m_wndEditStatus.FmtLines(TRUE);
    DDX_Control(pDX, IDC_STATICPIC, m_wndPic);
    DDX_Control(pDX, IDC_STATICORIG, m_wndOrig);
    DDX_Control(pDX, IDC_BUTTONCLIPBOARD, m_wndFromClipboard);
    m_wndFromClipboard.EnableWindow(IsClipboardFormatAvailable(CF_BITMAP));
    DDX_Check(pDX, IDC_CHECKSCALE, _iScaleImage);
    DDX_Check(pDX, IDC_CHECKGLOBALPALETTE, _honorGlobalPalette);
    DDX_Check(pDX, IDC_CHECKINSERT, _insertAtCurrentPosition);

    // MinMaxSlider busted in MFC 7.0
    // DDV_MinMaxSlider(pDX, _nColors, 1, 40); 

    GetDlgItem(IDC_SLIDERBRIGHTNESS)->SendMessage(TBM_SETRANGEMIN, FALSE, (LPARAM)0);
    GetDlgItem(IDC_SLIDERBRIGHTNESS)->SendMessage(TBM_SETRANGEMAX, TRUE, (LPARAM)100);
    GetDlgItem(IDC_SLIDERBRIGHTNESS)->SendMessage(TBM_SETTICFREQ, 10, 0);
    DDX_Slider(pDX, IDC_SLIDERBRIGHTNESS, _nBrightness);

    GetDlgItem(IDC_SLIDERCONTRAST)->SendMessage(TBM_SETRANGEMIN, FALSE, (LPARAM)0);
    GetDlgItem(IDC_SLIDERCONTRAST)->SendMessage(TBM_SETRANGEMAX, TRUE, (LPARAM)100);
    GetDlgItem(IDC_SLIDERCONTRAST)->SendMessage(TBM_SETTICFREQ, 10, 0);
    DDX_Slider(pDX, IDC_SLIDERCONTRAST, _nContrast);

    GetDlgItem(IDC_SLIDERSATURATION)->SendMessage(TBM_SETRANGEMIN, FALSE, (LPARAM)0);
    GetDlgItem(IDC_SLIDERSATURATION)->SendMessage(TBM_SETRANGEMAX, TRUE, (LPARAM)100);
    GetDlgItem(IDC_SLIDERSATURATION)->SendMessage(TBM_SETTICFREQ, 10, 0);
    DDX_Slider(pDX, IDC_SLIDERSATURATION, _nSaturation);

    DDX_Control(pDX, IDC_STATICPALETTE, m_wndPalette);

    if ((_nBrightness != nBrightness) ||
        (_nContrast != nContrast) ||
        (_nSaturation != nSaturation) ||
        (_iScaleImage != iScale))
    {
        _UpdateOrigBitmap(this);
    }

    if (!_targetCurrentPalette)
    {
        // This is a constant for the life of the dialog.
        m_wndRadio2.EnableWindow(FALSE);
    }

    if (!_allowInsertAtCurrentPosition)
    {
        m_wndCheckInsert.ShowWindow(SW_HIDE);
    }

    SetTimer(FROMCLIPBOARD_TIMER, 1000, nullptr);

    _Update();
}

void _FlipImageData(Cel &cel)
{
    FlipImageData(&cel.Data[0], cel.size.cx, cel.size.cy, CX_ACTUAL(cel.size.cx));
}

std::unique_ptr<PaletteComponent> _RemapImagePalette(uint8_t *data, int cx, int cy, const PaletteComponent &imagePalette, const PaletteComponent &globalPalette)
{
    // Start off with a copy of global palette.
    std::unique_ptr<PaletteComponent> newPalette = make_unique<PaletteComponent>(globalPalette);

    // global palette will have "holes" in it. We will fill those holes with the values from image palette
    uint8_t remapOldToNew[256];
    int imagePaletteIndex = 0;
    int globalPaletteIndex = 0;
    while (globalPaletteIndex < 256)
    {
        if (globalPalette.Colors[globalPaletteIndex].rgbReserved == 0x0)
        {
            // Global palette isn't using this one. The image palette can put a color here
            remapOldToNew[imagePaletteIndex] = globalPaletteIndex;      // We'll use this to map the image pixels to their new index.
            newPalette->Colors[globalPaletteIndex] = imagePalette.Colors[imagePaletteIndex];
            newPalette->Colors[globalPaletteIndex].rgbReserved = 0x3;   // Mark as used
            imagePaletteIndex++;
        }
        else
        {
            // Global palette is using this entry. We can mark it as unused in our image's new palette.
            newPalette->Colors[globalPaletteIndex].rgbReserved = 0x0;
        }
        globalPaletteIndex++;
    }

    // Fill in the rest of the mapping array. We'll map any remaining entries to 0, just so we aren't
    // dealing with uninitialized data
    // This is a problem, of course, if the imported image has more palette entries than there are
    // free ones, but we should have already warned the user about that.
    for (int i = imagePaletteIndex; i < 256; i++)
    {
        remapOldToNew[i] = 0;
    }

    // Now actually remap the pixels
    for (int i = 0; i < (CX_ACTUAL(cx) * cy); i++)
    {
        data[i] = remapOldToNew[data[i]];
    }

    return newPalette;
}

void CBitmapToVGADialog::_SyncControlState()
{
    bool optionsChanged = false;
    bool disableRadio3 = false;
    if (!_originalPalette)
    {
        // We can't offer an option to use the image palette
        m_wndRadio3.EnableWindow(FALSE);
        disableRadio3 = true;
        if (_paletteAlgorithm == 2)
        {
            m_wndRadio1.SetCheck(1);
            m_wndRadio3.SetCheck(0);
            _paletteAlgorithm = 0;
            optionsChanged = true;
        }
    }
    else
    {
        m_wndRadio3.EnableWindow(TRUE);
    }

    // We can't use anything but the current pic palette if we "insert at current position"
    if (_insertAtCurrentPosition)
    {
        if (_paletteAlgorithm != 1)
        {
            m_wndRadio2.SetCheck(1);
            _paletteAlgorithm = 1;
            optionsChanged = true;
        }
        m_wndRadio1.EnableWindow(FALSE);
        m_wndRadio3.EnableWindow(FALSE);
    }
    else
    {
        m_wndRadio1.EnableWindow(TRUE);
        m_wndRadio3.EnableWindow(!disableRadio3);
    }

    if (_paletteAlgorithm == 2)
    {
        // Ensure all sliders and the scale checkbox are disabled.
        if (m_wndCheck1.IsWindowEnabled())
        {
            optionsChanged = true;
            m_wndCheck1.EnableWindow(FALSE);
        }
        m_wndSlider2.EnableWindow(FALSE);
        m_wndSlider3.EnableWindow(FALSE);
        m_wndSlider4.EnableWindow(FALSE);
        _disableAllEffects = true;
    }
    else
    {
        m_wndCheck1.EnableWindow(TRUE);
        m_wndSlider2.EnableWindow(TRUE);
        m_wndSlider3.EnableWindow(TRUE);
        m_wndSlider4.EnableWindow(TRUE);
        _disableAllEffects = false;
    }

    if (optionsChanged)
    {
        _UpdateOrigBitmap(this);
        _needsUpdate = true;
    }
    Invalidate();
}

const uint8_t AlphaThreshold = 128;

void CBitmapToVGADialog::_Update()
{
    _finalResult.reset(nullptr);
    _finalResultPalette.reset(nullptr);
    // We need to generate a final image for m_wndPic
    switch (_paletteAlgorithm)
    {
    case 0:
        {
            if (_pbmpCurrent)
            {
                // Generate a palette
                // We can use _pbmpCurrent, the scaled, modified whatever.
                UINT cx = _pbmpCurrent->GetWidth();
                UINT cy = _pbmpCurrent->GetHeight();

                PixelFormat foo = _pbmpCurrent->GetPixelFormat();

                Gdiplus::Rect rect(0, 0, cx, cy);
                Gdiplus::BitmapData bitmapData;
                if (Ok == _pbmpCurrent->LockBits(&rect, ImageLockModeRead, PixelFormat24bppRGB, &bitmapData))
                {
                    std::unique_ptr<Cel> temp = make_unique<Cel>();
                    std::unique_ptr<PaletteComponent> tempPalette = make_unique<PaletteComponent>();
                    // TODO: Transparent color should be set by the user, and should not be part of the palette (unless desired)
                    temp->TransparentColor = _transparentColor;
                    temp->size = size16((uint16_t)cx, (uint16_t)cy);

                    uint8_t *pDIBBits24 = (uint8_t *)bitmapData.Scan0;

                    std::unique_ptr<uint8_t[]> sciBits = QuantizeImage(pDIBBits24, cx, bitmapData.Stride, cy, _honorGlobalPalette ? _globalPalette->Colors : nullptr, tempPalette->Colors, _transparentColor);
                    temp->Data.allocate(PaddedSize(temp->size));
                    temp->Data.assign(&sciBits.get()[0], &sciBits.get()[temp->Data.size()]);

                    _finalResult = move(temp);
                    _finalResultPalette = move(tempPalette);

                    _pbmpCurrent->UnlockBits(&bitmapData); // REVIEW: Not exception-safe
                }
            }
        }
        break;

    case 1:
        if (_targetCurrentPalette && _pbmpCurrent)
        {
            // Map to the current pic palette
            // We can use _pbmpCurrent, the scaled, modified whatever.
            UINT cx = _pbmpCurrent->GetWidth();
            UINT cy = _pbmpCurrent->GetHeight();

            // We need to come up with a list of the colors used in _targetCurrentPalette.
            // For every pixel, we need to map its values to the closest one, and store that index

            Gdiplus::Rect rect(0, 0, cx, cy);
            Gdiplus::BitmapData bitmapData;

            PixelFormat format1 = _pbmpCurrent->GetPixelFormat();
            PixelFormat format2 = PixelFormat32bppARGB;


            if (Ok == _pbmpCurrent->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData))
            {
                std::unique_ptr<Cel> temp = make_unique<Cel>();
                std::unique_ptr<PaletteComponent> tempPalette = make_unique<PaletteComponent>();
                temp->TransparentColor = _transparentColor;
                temp->size = size16((uint16_t)cx, (uint16_t)cy);

                uint8_t *pDIBBits24 = (uint8_t *)bitmapData.Scan0;
                temp->Data.allocate(PaddedSize(temp->size));

                for (UINT y = 0; y < cy; y++)
                {
                    for (UINT x = 0; x < cx; x++)
                    {
                        // Find the index that best maps this color
                        int bestIndex = _transparentColor;
                        int bestDistance = INT_MAX;
                        uint8_t *source = pDIBBits24 + (x * 4) + y * bitmapData.Stride;
                        RGBQUAD rgb = { *source, *(source + 1), *(source + 2) };
                        bool isTransparent = (*(source + 3) < AlphaThreshold);
                        if (isTransparent)
                        {
                            bestIndex = _transparentColor;
                        }
                        else
                        {
                            for (int i = 0; i < 256; i++)
                            {
                                RGBQUAD paletteColor = _targetCurrentPalette->Colors[i];
                                if ((i != (int)_transparentColor) && (paletteColor.rgbReserved != 0x0))
                                {
                                    int distance = GetColorDistance(rgb, paletteColor);
                                    if (distance < bestDistance)
                                    {
                                        bestIndex = i;
                                        bestDistance = distance;
                                    }
                                }
                            }
                        }

                        temp->Data[x + y * CX_ACTUAL(cx)] = (uint8_t)bestIndex;
                    }
                }

                _finalResult = move(temp);
                _finalResultPalette = make_unique<PaletteComponent>(*_targetCurrentPalette);

                _pbmpCurrent->UnlockBits(&bitmapData); // REVIEW: Not exception-safe
            }
        }
        break;
    case 2:
        // Use imported palette... no mods allowed, so use _pbmpOrig, _originalPalette
        // This should only be enabled if _originalPalette is set.
        if (_originalPalette)
        {

            UINT cx = _pbmpOrig->GetWidth();
            UINT cy = _pbmpOrig->GetHeight();

            Gdiplus::Rect rect(0, 0, cx, cy);
            Gdiplus::BitmapData bitmapData;
            if (Ok == _pbmpOrig->LockBits(&rect, ImageLockModeRead, PixelFormat8bppIndexed, &bitmapData))
            {
                std::unique_ptr<Cel> temp = make_unique<Cel>();
                std::unique_ptr<PaletteComponent> tempPalette = make_unique<PaletteComponent>();
                temp->size = size16((uint16_t)cx, (uint16_t)cy);
                temp->TransparentColor = _transparentColor;
                uint8_t *pDIBBits8 = (uint8_t *)bitmapData.Scan0;
                temp->Data.allocate(PaddedSize(temp->size));
                temp->Data.assign(pDIBBits8, pDIBBits8 + temp->Data.size());

                _finalResult = move(temp);
                if (_honorGlobalPalette && _globalPalette)
                {
                    _finalResultPalette = _RemapImagePalette(&_finalResult->Data[0], cx, cy, *_originalPalette, *_globalPalette);
                }
                else
                {
                    _finalResultPalette = make_unique<PaletteComponent>(*_originalPalette);
                }

                _pbmpOrig->UnlockBits(&bitmapData); // REVIEW: Not exception-safe
            }
        }
        break;
    }

    // Now given the Cel, generate a bitmap from it.
    if (_finalResult)
    {
        _FlipImageData(*_finalResult);
        CBitmap bitmap;
        bitmap.Attach(GetBitmap(*_finalResult, _finalResultPalette.get(), _finalResult->size.cx, _finalResult->size.cy, BitmapScaleOptions::None, 0));
        m_wndPic.FromBitmap((HBITMAP)bitmap, _finalResult->size.cx, _finalResult->size.cy);
        m_wndPic.ShowWindow(SW_SHOW);
    }
    else
    {
        m_wndPic.ShowWindow(SW_HIDE);
    }
    _UpdatePalette();

    // Put up some kind of informative message
    if (_originalPalette)
    {
        std::string text = format("Embedded palette has {0} entries.", _numberOfPaletteEntries);
        if (_numberOfPaletteEntries > _numUnusedPaletteEntriesInGlobalPalette)
        {
            text += format(" Global palette has only {0} unused entries!", _numUnusedPaletteEntriesInGlobalPalette);
        }
        _AddToEdit(text.c_str());
    }
    else
    {
        _AddToEdit("No embedded palette detected.");
    }

    GetDlgItem(IDOK)->EnableWindow(!!_finalResult);

    _needsUpdate = false;
}

void CBitmapToVGADialog::OnCancel()
{
    __super::OnCancel();
}

void CBitmapToVGADialog::OnOK()
{
    BOOL fClose = TRUE;
    if (fClose)
    {
        __super::OnOK();
    }
}

BEGIN_MESSAGE_MAP(CBitmapToVGADialog, CExtNCW<CExtResizableDialog>)
    ON_BN_CLICKED(IDC_CHECKSCALE, OnScaleClicked)
    ON_BN_CLICKED(IDC_BUTTONBROWSE, OnBrowse)
    ON_BN_CLICKED(IDC_BUTTONCLIPBOARD, OnPasteFromClipboard)
    ON_WM_HSCROLL()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_RADIO2, &CBitmapToVGADialog::OnBnClickedRadio2)
    ON_BN_CLICKED(IDC_CHECKGLOBALPALETTE, &CBitmapToVGADialog::OnBnClickedCheckglobalpalette)
    ON_BN_CLICKED(IDC_RADIO1, &CBitmapToVGADialog::OnBnClickedRadio1)
    ON_BN_CLICKED(IDC_RADIO3, &CBitmapToVGADialog::OnBnClickedRadio3)
    ON_BN_CLICKED(IDC_CHECKINSERT, &CBitmapToVGADialog::OnBnClickedCheckinsert)
    ON_EN_KILLFOCUS(IDC_EDITTRANSPARENTCOLOR, &CBitmapToVGADialog::OnEnKillfocusEdittransparentcolor)
END_MESSAGE_MAP()


// CBitmapToVGADialog message handlers

void CBitmapToVGADialog::OnBrowse()
{
    _OnBrowse(this);
    _SyncControlState();
    _Update();
}

void CBitmapToVGADialog::_AddToEdit(PCTSTR pszText)
{
    m_wndEditStatus.SetWindowText(pszText);
}

void CBitmapToVGADialog::OnScaleClicked()
{
    // Get the most recent values
    UpdateData(TRUE);
    _Update();
}

void CBitmapToVGADialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd)
{
    UpdateData(TRUE);
}

void CBitmapToVGADialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == FROMCLIPBOARD_TIMER)
    {
        m_wndFromClipboard.EnableWindow(IsClipboardFormatAvailable(CF_BITMAP));
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

void CBitmapToVGADialog::_UpdatePalette()
{
    if (_finalResultPalette)
    {
        m_wndPalette.SetPalette(16, 16, reinterpret_cast<const EGACOLOR *>(_finalResultPalette->Mapping), 256, _finalResultPalette->Colors, false);
        m_wndPalette.ShowWindow(SW_SHOW);
        m_wndPalette.Invalidate(FALSE);
    }
    else
    {
        m_wndPalette.ShowWindow(SW_HIDE);
    }
}

void CBitmapToVGADialog::OnPasteFromClipboard()
{
    _OnPasteFromClipboard(this);
    _SyncControlState();
    _Update();
}

BOOL CBitmapToVGADialog::OnInitDialog()
{
    __super::OnInitDialog();

    GetDlgItem(IDOK)->EnableWindow(FALSE);

    // Put a little help text
    _AddToEdit(TEXT("Open a file or pull from clipboard"));

    OnPasteFromClipboard();

    return TRUE;
}

void CBitmapToVGADialog::OnBnClickedCheckglobalpalette()
{
    // Do this manually, since DDX_Check doesn't seem to work.
    _honorGlobalPalette = m_wndCheckGlobalPalette.GetCheck();
    _Update();
}

void CBitmapToVGADialog::OnBnClickedRadio1()
{
    _paletteAlgorithm = 0;
    _SyncControlState();
    _Update();
}

void CBitmapToVGADialog::OnBnClickedRadio2()
{
    _paletteAlgorithm = 1;
    _SyncControlState();
    _Update();
}

void CBitmapToVGADialog::OnBnClickedRadio3()
{
    _paletteAlgorithm = 2;
    _SyncControlState();
    _Update();
}


void CBitmapToVGADialog::OnBnClickedCheckinsert()
{
    _insertAtCurrentPosition = m_wndCheckInsert.GetCheck();
    _SyncControlState();
    _Update();
}


void CBitmapToVGADialog::OnEnKillfocusEdittransparentcolor()
{
    // Validate the edit control
    BOOL translated;
    _transparentColor = min(255, GetDlgItemInt(IDC_EDITTRANSPARENTCOLOR, &translated, FALSE));
    SetDlgItemInt(IDC_EDITTRANSPARENTCOLOR, _transparentColor, FALSE);
}
