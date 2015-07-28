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
#include "VGADither.h"

using namespace std;
using namespace fmt;
using namespace Gdiplus;

#define FROMCLIPBOARD_TIMER 2345

const int MaxPaletteColors = 256;

int g_iScaleImageVGA = 0;   // Don't scale by default

// The end result of this dialog is image data (a Cel) and possibly a palette

CBitmapToVGADialog::CBitmapToVGADialog(
        std::unique_ptr<Gdiplus::Bitmap> existingBitmap,
        const Cel *currentBackgroundOptional,
        const PaletteComponent *targetPalette,
        bool fixedPalette,
        int colorCount,
        bool allowInsertAtCurrentPosition,
        size16 picDimensions,
        uint8_t defaultTransparentColor,
        PaletteAlgorithm defaultAlgorithm,
        DefaultPaletteUsage defaultColorUsage,
        const char *pszTitle,
        CWnd* pParent /*=nullptr*/)
    : CExtNCW<CExtResizableDialog>(CBitmapToVGADialog::IDD, pParent),
    PrepareBitmapBase(-1, IDC_STATICORIG, picDimensions),
    _currentBackgroundOptional(currentBackgroundOptional),
    _existingBitmap(move(existingBitmap)),
    _transparentColor(defaultTransparentColor),
    _paletteAlgorithm(defaultAlgorithm),
    _defaultColorUsage(defaultColorUsage),
    _pszTitle(pszTitle),
    _manuallyModifiedColors(false),
    _fixedPalette(fixedPalette),
    _paletteSize(colorCount)
{
    _alphaThreshold = 128;
    _allowInsertAtCurrentPosition = allowInsertAtCurrentPosition;
    _needsUpdate = true;
    _insertAtCurrentPosition = 0;

    if (fixedPalette)
    {
        assert(_paletteAlgorithm != PaletteAlgorithm::Quantize);
        _numUnusedPaletteEntriesInGlobalPalette = 0;
        assert(targetPalette && "Fixed palette must provide a palette");
        _globalPalette = make_unique<PaletteComponent>(*targetPalette);
    }
    else
    {
        assert(_paletteSize == 256);
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
    }

    if (targetPalette)
    {
        _targetCurrentPalette = make_unique<PaletteComponent>(*targetPalette);
        _finalResultPalette = make_unique<PaletteComponent>(*targetPalette);    // Just something to start with, so the palette box shows something.
    }

    if (!_finalResultPalette && _globalPalette)
    {
        _finalResultPalette = make_unique<PaletteComponent>(*_globalPalette);
    }


    _pCRBitmap = nullptr;
    _iScaleImage = g_iScaleImageVGA;
    _honorGlobalPalette = BST_CHECKED; // Always honour by default
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

void CBitmapToVGADialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeft)
{
    // Enable the refresh button
    m_wndRefresh.EnableWindow(TRUE);
    m_wndEditPaletteRanges.SetWindowText("Press refresh...");
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
        
        DDX_Control(pDX, IDC_STATICARROW, m_wndRightArrow);

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
        DDX_Control(pDX, IDC_STATICGROUP4, m_wndGroup4);
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
        DDX_Control(pDX, IDC_CHECKOVERLAY, m_wndCheckOverlay);
        DDX_Control(pDX, IDC_CHECKDITHER, m_wndDither);
        m_wndDither.SetCheck(BST_UNCHECKED);
        DDX_Control(pDX, IDC_CHECKDITHERALPHA, m_wndDitherAlpha);
        m_wndDitherAlpha.SetCheck(BST_UNCHECKED);

        DDX_Control(pDX, IDC_BUTTONCLIPBOARD, m_wndFromClipboard);

        DDX_Control(pDX, IDC_STATICGROUP3, m_wndGroup3);
        DDX_Control(pDX, IDC_EDITTRANSPARENTCOLOR, m_wndEditTransparentColor);
        SetDlgItemInt(IDC_EDITTRANSPARENTCOLOR, _transparentColor, FALSE);
        DDX_Control(pDX, IDC_STATICTRANSLABEL, m_wndLabel4);
        DDX_Control(pDX, IDC_EDITALPHATHRESHOLD, m_wndEditAlphaThreshold);
        SetDlgItemInt(IDC_EDITALPHATHRESHOLD, _alphaThreshold, FALSE);
        DDX_Control(pDX, IDC_STATICALPHALABEL, m_wndLabel5);
        DDX_Control(pDX, IDC_CHECKDONTUSEINPALETTE, m_wndCheckDontUseInPalette);
        m_wndCheckDontUseInPalette.SetCheck(1);
        
        DDX_Control(pDX, IDC_BUTTONREFRESH, m_wndRefresh);
        m_wndRefresh.EnableWindow(FALSE);
        m_wndRefresh.SetIcon(IDI_REFRESH, 0, 0, 0, 16, 16);
        DDX_Control(pDX, IDC_EDITRANGES, m_wndEditPaletteRanges);

        // Check the appropriate algorithm by default
        CExtCheckBox *algorithmCheckBoxes[] = { &m_wndRadio1, &m_wndRadio2, &m_wndRadio3 };
        algorithmCheckBoxes[(int)_paletteAlgorithm]->SetCheck(BST_CHECKED);
        _fInitializedControls = true;

        DDX_Control(pDX, IDC_STATICPALETTE, m_wndPalette);
        m_wndPalette.ShowSelection(TRUE);
        m_wndPalette.ShowFocusBoxes(FALSE);
        m_wndPalette.SetAutoHandleSelection(true);
        m_wndPalette.SetCallback(this);
        _UpdatePalette();

        if (_existingBitmap)
        {
            // Hide certain controls if the dialog was given a bitmap on which to operate.
            m_wndButton2.ShowWindow(SW_HIDE);       // Browse
            m_wndFromClipboard.ShowWindow(SW_HIDE);
            m_wndCheck1.ShowWindow(SW_HIDE);        // Scale
            m_wndCheckOverlay.ShowWindow(SW_HIDE);
        }

        if (_fixedPalette)
        {
            // Hide the first algorithm (generate palette)
            m_wndRadio1.ShowWindow(SW_HIDE);
        }
    }

    if (_defaultColorUsage == DefaultPaletteUsage::UnusedColors)
    {
        m_wndCheckGlobalPalette.SetWindowTextA(format("Only use {0} free palette entries", _numUnusedPaletteEntriesInGlobalPalette).c_str());
    }
    else
    {
        m_wndCheckGlobalPalette.SetWindowTextA(format("Use {0} global palette entries", (_paletteSize - _numUnusedPaletteEntriesInGlobalPalette)).c_str());
    }

    DDX_Control(pDX, IDC_EDITSTATUS, m_wndEditStatus);
    m_wndEditStatus.FmtLines(TRUE);
    DDX_Control(pDX, IDC_STATICPIC, m_wndPic);
    DDX_Control(pDX, IDC_STATICORIG, m_wndOrig);
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

void CBitmapToVGADialog::_SetAvailableColorsFromGlobalPalette()
{
    bool available[MaxPaletteColors];
    for (int i = 0; i < ARRAYSIZE(available); i++)
    {
        bool temp = _globalPalette->Colors[i].rgbReserved == 0;
        if (_defaultColorUsage == DefaultPaletteUsage::UsedColors)
        {
            temp = !temp;
        }
        available[i] = temp;
    }
    m_wndPalette.SetSelection(available);
}

void _FlipImageData(Cel &cel)
{
    FlipImageData(&cel.Data[0], cel.size.cx, cel.size.cy, CX_ACTUAL(cel.size.cx));
}

std::unique_ptr<PaletteComponent> _RemapImagePalette(uint8_t *data, int cx, int cy, const PaletteComponent &imagePalette, const PaletteComponent &globalPalette, int paletteSize)
{
    // Start off with a copy of global palette.
    std::unique_ptr<PaletteComponent> newPalette = make_unique<PaletteComponent>(globalPalette);

    // global palette will have "holes" in it. We will fill those holes with the values from image palette
    uint8_t remapOldToNew[MaxPaletteColors];
    int imagePaletteIndex = 0;
    int globalPaletteIndex = 0;
    while (globalPaletteIndex < paletteSize)
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
    for (int i = imagePaletteIndex; i < ARRAYSIZE(remapOldToNew); i++)
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
        if (_paletteAlgorithm == PaletteAlgorithm::UseImported)
        {
            m_wndRadio1.SetCheck(1);
            m_wndRadio3.SetCheck(0);
            _paletteAlgorithm = PaletteAlgorithm::Quantize;
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
        if (_paletteAlgorithm != PaletteAlgorithm::MatchExisting)
        {
            m_wndRadio2.SetCheck(1);
            _paletteAlgorithm = PaletteAlgorithm::MatchExisting;
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

    if (_paletteAlgorithm == PaletteAlgorithm::UseImported)
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

    if (_paletteAlgorithm != PaletteAlgorithm::UseImported)
    {
        // Enable dithering for the first two algorithms, but not when we're
        // using the imported palette directly.
        m_wndDither.EnableWindow(TRUE);
        m_wndDitherAlpha.EnableWindow(TRUE);
    }
    else
    {
        m_wndDither.EnableWindow(FALSE);
        m_wndDitherAlpha.EnableWindow(FALSE);
    }

    if (optionsChanged)
    {
        _UpdateOrigBitmap(this);
        _needsUpdate = true;
    }
    Invalidate();
}

PaletteComponent _CreatePaletteWithFreeSlots(const PaletteComponent *source, CChooseColorStatic &wndPalette)
{
    PaletteComponent palette;
    palette = *source;
    bool selection[MaxPaletteColors];
    wndPalette.GetMultipleSelection(selection);
    for (int i = 0; i < ARRAYSIZE(selection); i++)
    {
        palette.Colors[i].rgbReserved = selection[i] ? 0 : 0x1;
    }
    return palette;
}

void _Overlay(Cel &cel, const Cel *currentBackgroundOptional)
{
    if (currentBackgroundOptional)
    {
        // We'll use the current image as a reference for size and such. Otherwise we can't enlargen the background.
        // Wait a sec... if we have a currentbackground and we're overlaying, we should use that!
        Cel newCel = *currentBackgroundOptional;
        for (int y = 0; y < newCel.size.cy; y++)
        {
            int destRowOffset = y * CX_ACTUAL(newCel.size.cx);
            for (int x = 0; x < newCel.size.cx; x++)
            {
                bool isTransparent = true;
                uint8_t newColor = currentBackgroundOptional->Data[destRowOffset + x];
                if ((y < cel.size.cy) && (x < cel.size.cx))
                {
                    int srcRowOffset = y * CX_ACTUAL(cel.size.cx);
                    isTransparent = (cel.Data[srcRowOffset + x] == cel.TransparentColor);
                    if (!isTransparent)
                    {
                        newColor = cel.Data[srcRowOffset + x];
                    }
                }
                newCel.Data[destRowOffset + x] = newColor;
            }
        }
        // Replace!
        cel = newCel;
    }
}

void CBitmapToVGADialog::_Update()
{
    _finalResult.reset(nullptr);
    _finalResultPalette.reset(nullptr);

    PaletteComponent referencePalette = _CreatePaletteWithFreeSlots(_targetCurrentPalette.get(), m_wndPalette);

    m_wndEditPaletteRanges.SetWindowText(GetRangeText(GetSelectedRanges(m_wndPalette)).c_str());

    bool performAlphaDither = (m_wndDitherAlpha.GetCheck() == BST_CHECKED);
    bool performDither = (m_wndDither.GetCheck() == BST_CHECKED);

    // We need to generate a final image for m_wndPic
    switch (_paletteAlgorithm)
    {
    case PaletteAlgorithm::Quantize:
        {
            if (_pbmpCurrent)
            {
                assert(!_fixedPalette);
                // Generate a palette
                // We can use _pbmpCurrent, the scaled, modified whatever.
                UINT cx = _pbmpCurrent->GetWidth();
                UINT cy = _pbmpCurrent->GetHeight();
                unique_ptr<RGBQUAD[]> imageData = ConvertGdiplusToRaw(*_pbmpCurrent);
                if (imageData)
                {
                    CutoutAlpha(imageData.get(), cx, cy, performAlphaDither, _alphaThreshold);

                    std::unique_ptr<Cel> temp = make_unique<Cel>();
                    std::unique_ptr<PaletteComponent> tempPalette;
                    if (_targetCurrentPalette)
                    {
                        tempPalette = make_unique<PaletteComponent>(*_targetCurrentPalette);
                    }
                    else
                    {
                        tempPalette = make_unique<PaletteComponent>();
                    }

                    temp->TransparentColor = _transparentColor;
                    temp->size = size16((uint16_t)cx, (uint16_t)cy);

                    std::unique_ptr<uint8_t[]> sciBits = QuantizeImage(imageData.get(), cx, cy, (_honorGlobalPalette != BST_UNCHECKED) ? referencePalette.Colors : nullptr, tempPalette->Colors, _transparentColor);

                    if (performDither)
                    {
                        // This is expensive, and we only need to do this if we're dithering, as sciBits already contains the quantized image data.
                        RGBToPalettized(sciBits.get(), imageData.get(), cx, cy, performDither, ARRAYSIZE(tempPalette->Colors), tempPalette->Mapping, tempPalette->Colors, _transparentColor);
                    }

                    temp->Data.allocate(PaddedSize(temp->size));
                    temp->Data.assign(&sciBits.get()[0], &sciBits.get()[temp->Data.size()]);

                    _finalResult = move(temp);
                    _finalResultPalette = move(tempPalette);
                }
            }
        }
        break;

    case PaletteAlgorithm::MatchExisting:
        if (_targetCurrentPalette && _pbmpCurrent)
        {
            // Try to map colors to the current pic palette
            // We can use _pbmpCurrent, the scaled, modified whatever.
            UINT cx = _pbmpCurrent->GetWidth();
            UINT cy = _pbmpCurrent->GetHeight();

            unique_ptr<RGBQUAD[]> imageData = ConvertGdiplusToRaw(*_pbmpCurrent);
            if (imageData)
            {
                CutoutAlpha(imageData.get(), cx, cy, performAlphaDither, _alphaThreshold);

                std::unique_ptr<Cel> temp = make_unique<Cel>();
                temp->TransparentColor = _transparentColor;
                temp->size = size16((uint16_t)cx, (uint16_t)cy);
                temp->Data.allocate(PaddedSize(temp->size));

                bool usableColors[MaxPaletteColors];
                m_wndPalette.GetMultipleSelection(usableColors);
                RGBQUAD targetColors[MaxPaletteColors];
                // rgbReserved must be 0x1 where there are usable colors and 0x0 elsewhere.
                for (int i = 0; i < _paletteSize; i++)
                {
                    targetColors[i] = _targetCurrentPalette->Colors[i];
                    targetColors[i].rgbReserved = usableColors[i] ? 0x1 : 0x0;
                }

                RGBToPalettized(&temp->Data[0], imageData.get(), cx, cy, performDither, _paletteSize, _targetCurrentPalette->Mapping, targetColors, _transparentColor);

                _finalResult = move(temp);
                _finalResultPalette = make_unique<PaletteComponent>(*_targetCurrentPalette);
            }
        }
        break;
    case PaletteAlgorithm::UseImported:
        // Use imported palette... no mods allowed, so use _pbmpOrig, _originalPalette
        // This should only be enabled if _originalPalette is set.
        if (_originalPalette)
        {
            std::vector<Cel> tempCels;
            PaletteComponent tempPalette;
            if (GetCelsAndPaletteFromGdiplus(*_pbmpOrig, _transparentColor, tempCels, tempPalette))
            {
                _finalResult = make_unique<Cel>(tempCels[0]);
                if ((_honorGlobalPalette != BST_UNCHECKED) && _globalPalette)
                {
                    _finalResultPalette = _RemapImagePalette(&_finalResult->Data[0], tempCels[0].size.cx, tempCels[0].size.cy, *_originalPalette, referencePalette, _paletteSize);
                }
                else
                {
                    _finalResultPalette = make_unique<PaletteComponent>(*_originalPalette);
                }
            }
        }
        break;
    }

    // Now given the Cel, generate a bitmap from it.
    if (_finalResult)
    {
        _FlipImageData(*_finalResult);
        if (m_wndCheckOverlay.GetCheck() == BST_CHECKED)
        {
            // Overlay this image on the current background
            _Overlay(*_finalResult, _currentBackgroundOptional);
        }
        CBitmap bitmap;
        bitmap.Attach(GetBitmap(*_finalResult, _finalResultPalette.get(), _finalResult->size.cx, _finalResult->size.cy, BitmapScaleOptions::None, 0));
        m_wndPic.FromBitmap((HBITMAP)bitmap, _finalResult->size.cx, _finalResult->size.cy, true);
        m_wndPic.ShowWindow(SW_SHOW);
        _UpdatePalette();
    }
    else
    {
        m_wndPic.ShowWindow(SW_HIDE);
    }

    // Put up some kind of informative message
    if (_originalPalette)
    {
        std::string text = format("Embedded palette has {0} entries.", _numberOfPaletteEntries);
        int numEntriesToUse = (_defaultColorUsage == DefaultPaletteUsage::UsedColors) ? (_paletteSize - _numUnusedPaletteEntriesInGlobalPalette) : _numUnusedPaletteEntriesInGlobalPalette;
        if (_numberOfPaletteEntries > numEntriesToUse)
        {
            if (_defaultColorUsage == DefaultPaletteUsage::UsedColors)
            {
                text += format(" Global palette has only {0} main entries!", (_paletteSize - _numUnusedPaletteEntriesInGlobalPalette));
            }
            else
            {
                text += format(" Global palette has only {0} unused entries!", _numUnusedPaletteEntriesInGlobalPalette);
            }
        }
        _AddToEdit(text.c_str());
    }
    else
    {
        _AddToEdit("No embedded palette detected.");
    }

    GetDlgItem(IDOK)->EnableWindow(!!_finalResult);
    m_wndRefresh.EnableWindow(FALSE);
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
    ON_BN_CLICKED(IDC_BUTTONREFRESH, &CBitmapToVGADialog::OnBnClickedButtonrefresh)
    ON_BN_CLICKED(IDC_CHECKOVERLAY, &CBitmapToVGADialog::OnBnClickedThatThatShouldUpdate)
    ON_BN_CLICKED(IDC_CHECKDITHER, &CBitmapToVGADialog::OnBnClickedThatThatShouldUpdate)
    ON_BN_CLICKED(IDC_CHECKDITHERALPHA, &CBitmapToVGADialog::OnBnClickedThatThatShouldUpdate)
    ON_EN_KILLFOCUS(IDC_EDITALPHATHRESHOLD, &CBitmapToVGADialog::OnEnKillfocusEditalphathreshold)
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

void CBitmapToVGADialog::_PushPaletteSelection()
{
    if (_honorGlobalPalette == BST_CHECKED)
    {
        _SetAvailableColorsFromGlobalPalette();
    }
    else if (_honorGlobalPalette == BST_UNCHECKED)
    {
        bool selectAll[MaxPaletteColors];
        std::fill_n(selectAll, ARRAYSIZE(selectAll), true);
        m_wndPalette.SetSelection(selectAll);
    }
    // else indeterminate
}

void CBitmapToVGADialog::_UpdatePalette()
{
    if (_finalResultPalette)
    {
        int rows = max(1, (int)sqrt(_paletteSize));
        int columns = _paletteSize / rows;
        m_wndPalette.SetPalette(rows, columns, reinterpret_cast<const EGACOLOR *>(_finalResultPalette->Mapping), _paletteSize, _finalResultPalette->Colors, false);
        m_wndPalette.ShowWindow(SW_SHOW);
        _PushPaletteSelection();
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

    if (_pszTitle)
    {
        SetWindowText(_pszTitle);
    }

    GetDlgItem(IDOK)->EnableWindow(FALSE);

    // Put a little help text
    _AddToEdit(TEXT("Open a file or pull from clipboard"));

    if (_existingBitmap)
    {
        _Init(move(_existingBitmap), this);
        _UpdateOrigBitmap(this);
        _SyncControlState();
        _Update();
    }
    else
    {
        OnPasteFromClipboard();
    }

    return TRUE;
}

void CBitmapToVGADialog::OnBnClickedCheckglobalpalette()
{
    // Do this manually, since DDX_Check doesn't seem to work.
    _honorGlobalPalette = m_wndCheckGlobalPalette.GetCheck();
    _manuallyModifiedColors = true;
    _PushPaletteSelection();
    _Update();
}

void CBitmapToVGADialog::OnBnClickedRadio1()
{
    _paletteAlgorithm = PaletteAlgorithm::Quantize;
    if (!_manuallyModifiedColors)
    {
        assert(_honorGlobalPalette != BST_INDETERMINATE);
        _honorGlobalPalette = BST_CHECKED;
        _PushPaletteSelection();
    }
    _SyncControlState();
    _Update();
}

void CBitmapToVGADialog::OnBnClickedRadio2()
{
    _paletteAlgorithm = PaletteAlgorithm::MatchExisting;
    if (!_manuallyModifiedColors)
    {
        assert(_honorGlobalPalette != BST_INDETERMINATE);
        _honorGlobalPalette = BST_CHECKED;
        _PushPaletteSelection();
    }
    _SyncControlState();
    _Update();
}

void CBitmapToVGADialog::OnBnClickedRadio3()
{
    _paletteAlgorithm = PaletteAlgorithm::UseImported;
    if (!_manuallyModifiedColors)
    {
        assert(_honorGlobalPalette != BST_INDETERMINATE);
        _honorGlobalPalette = BST_UNCHECKED;
        _PushPaletteSelection();
    }
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
    _Update();
}


void CBitmapToVGADialog::OnBnClickedButtonrefresh()
{
    _honorGlobalPalette = BST_INDETERMINATE;
    m_wndCheckGlobalPalette.SetCheck(_honorGlobalPalette);
    m_wndRefresh.EnableWindow(FALSE);
    _manuallyModifiedColors = true;
    _Update();
}


void CBitmapToVGADialog::OnBnClickedThatThatShouldUpdate()
{
    _Update();
}


void CBitmapToVGADialog::OnEnKillfocusEditalphathreshold()
{
    // Validate the edit control
    BOOL translated;
    _alphaThreshold = min(255, GetDlgItemInt(IDC_EDITALPHATHRESHOLD, &translated, FALSE));
    SetDlgItemInt(IDC_EDITALPHATHRESHOLD, _alphaThreshold, FALSE);
    _Update();
}
