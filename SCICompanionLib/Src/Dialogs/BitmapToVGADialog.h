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

#include "NoFlickerStatic.h"
#include "PrepareBitmapBase.h"
#include "ChooseColorStatic.h"

enum class DitherAlgorithm;

struct Cel;

enum class PaletteAlgorithm
{
    Quantize = 0,
    MatchExisting = 1,
    UseImported = 2,
};

enum class DefaultPaletteUsage
{
    UnusedColors = 0,
    UsedColors = 1,
};

enum class ColorMatching;

class CBitmapToVGADialog : public CExtNCW<CExtResizableDialog>, public PrepareBitmapBase, public IColorDialogCallback
{
public:
    CBitmapToVGADialog(
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
        const char *pszTitle = nullptr,
        bool insertAtCurrentDefault = false,
        CWnd* pParent = NULL);   // standard constructor
    virtual ~CBitmapToVGADialog();

    // Dialog Data
    enum { IDD = IDD_BITMAPTOVGA };

    bool GetInsertAtCurrentPosition() { return _insertAtCurrentPosition != 0; }
    std::unique_ptr<Cel> GetFinalResult();
    std::unique_ptr<PaletteComponent> GetFinalResultPalette();

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeft) override;

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnCancel();
    virtual void OnOK();
    void OnTimer(UINT_PTR nIDEvent);
    void _AddToEdit(PCTSTR pszText);
    void _UpdatePalette();
    void _PushPaletteSelection();
    void _SyncControlState();
    void _SetAvailableColorsFromGlobalPalette();
    afx_msg void OnBrowse();
    afx_msg void OnPasteFromClipboard();
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd);

    void _Update();

    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

    CExtEdit m_wndEditStatus;
    CExtNoFlickerStatic m_wndPic;
    CExtButton m_wndFromClipboard;

    CExtNoFlickerStatic m_wndOrig;

    // just for prof-uis
    CExtSliderWnd m_wndSlider2;
    CExtSliderWnd m_wndSlider3;
    CExtSliderWnd m_wndSlider4;
    CExtSliderWnd m_wndSlider5;
    CExtButton m_wndButton2;
    CExtButton m_wndButton3;
    CExtButton m_wndButton4;
    CExtGroupBox m_wndGroup1;
    CExtGroupBox m_wndGroup2;
    CExtGroupBox m_wndGroup4;
    CExtCheckBox m_wndCheck1;
    CExtCheckBox m_wndCheckInsert;
    CExtCheckBox m_wndDither;
    CExtCheckBox m_wndGamma;
    CExtLabel m_wndLabelDitherAlpha;
    CExtComboBox m_wndComboDitherAlpha;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
    CExtLabel m_wndLabel6;
    CExtLabel m_wndLabel8;
    CExtComboBox m_wndComboMatch;
    CExtRadioButton m_wndRadio1;    // palette options
    CExtRadioButton m_wndRadio2;
    CExtRadioButton m_wndRadio3;
    PaletteAlgorithm _paletteAlgorithm;
    DefaultPaletteUsage _defaultColorUsage;
    const char *_pszTitle;
    CExtCheckBox m_wndCheckGlobalPalette;
    CExtCheckBox m_wndCheckOverlay;
    CExtLabel m_wndRightArrow;

    CExtGroupBox m_wndGroup3;
    CExtEdit m_wndEditTransparentColor;
    CExtEdit m_wndEditAlphaThreshold;
    CExtLabel m_wndLabel4;
    CExtLabel m_wndLabel5;
    CExtCheckBox m_wndCheckDontUseInPalette;
    
    CChooseColorStatic m_wndPalette;
    CExtButton m_wndRefresh;
    CExtEdit m_wndEditPaletteRanges;

    bool _fInitializedControls;

    std::unique_ptr<Cel> _finalResult;
    std::unique_ptr<PaletteComponent> _finalResultPalette;

    std::unique_ptr<PaletteComponent> _targetCurrentPalette;
    std::unique_ptr<PaletteComponent> _calculatedPalette;
    std::unique_ptr<PaletteComponent> _globalPalette;
    int _numUnusedPaletteEntriesInGlobalPalette;
    int _honorGlobalPalette;
    bool _manuallyModifiedColors;
    int _insertAtCurrentPosition;
    bool _allowInsertAtCurrentPosition;

    bool _needsUpdate;

    uint8_t _transparentColor;
    uint8_t _alphaThreshold;

    const Cel *_currentBackgroundOptional;

    std::unique_ptr<Gdiplus::Bitmap> _existingBitmap;

    bool _fixedPalette;
    int _paletteSize;

    ColorMatching _colorMatching;

    afx_msg void OnScaleClicked();
    afx_msg void OnBnClickedRadio2();
    afx_msg void OnBnClickedCheckglobalpalette();
    afx_msg void OnBnClickedRadio1();
    afx_msg void OnBnClickedRadio3();
public:
    afx_msg void OnBnClickedCheckinsert();
    afx_msg void OnEnKillfocusEdittransparentcolor();
    afx_msg void OnBnClickedButtonrefresh();
    afx_msg void OnBnClickedThatThatShouldUpdate();
    afx_msg void OnEnKillfocusEditalphathreshold();
    afx_msg void OnCbnSelchangeCombomatch();
    afx_msg void OnBnClickedCheckdontuseinpalette();
    afx_msg void OnBnClickedGammacorrected();
};

std::unique_ptr<Cel> GdiPlusBitmapToCel(
    Gdiplus::Bitmap &bmpCurrent,
    bool performDither,
    bool gammaCorrected,
    DitherAlgorithm alphaDither,
    ColorMatching colorMatching,
    uint8_t alphaThreshold,
    uint8_t transparentColor,
    bool excludeTransparentColorFromPalette,
    int paletteSize,
    const bool *usableColors,
    const RGBQUAD *colors,
    const uint8_t *mapping
    );