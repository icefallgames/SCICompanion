#pragma once

#include "NoFlickerStatic.h"
#include "PrepareBitmapBase.h"
#include "ChooseColorStatic.h"

// CBitmapToVGADialog dialog

struct Cel;

class CBitmapToVGADialog : public CExtNCW<CExtResizableDialog>, public PrepareBitmapBase, public IColorDialogCallback
{
public:
    CBitmapToVGADialog(const Cel *currentBackgroundOptional, const PaletteComponent *targetPalette, bool allowInsertAtCurrentPosition, size16 picDimensions, CWnd* pParent = NULL);   // standard constructor
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
    CExtButton m_wndButton2;
    CExtButton m_wndButton3;
    CExtButton m_wndButton4;
    CExtGroupBox m_wndGroup1;
    CExtGroupBox m_wndGroup2;
    CExtCheckBox m_wndCheck1;
    CExtCheckBox m_wndCheckInsert;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
    CExtRadioButton m_wndRadio1;    // palette options
    CExtRadioButton m_wndRadio2;
    CExtRadioButton m_wndRadio3;
    int _paletteAlgorithm;
    CExtCheckBox m_wndCheckGlobalPalette;
    CExtCheckBox m_wndCheckOverlay;

    CExtGroupBox m_wndGroup3;
    CExtEdit m_wndEditTransparentColor;
    CExtLabel m_wndLabel4;
    CExtCheckBox m_wndCheckDontUseInPalette;
    
    CChooseColorStatic m_wndPalette;
    CExtButton m_wndRefresh;

    bool _fInitializedControls;

    std::unique_ptr<Cel> _finalResult;
    std::unique_ptr<PaletteComponent> _finalResultPalette;

    std::unique_ptr<PaletteComponent> _targetCurrentPalette;
    std::unique_ptr<PaletteComponent> _calculatedPalette;
    std::unique_ptr<PaletteComponent> _globalPalette;
    int _numUnusedPaletteEntriesInGlobalPalette;
    int _honorGlobalPalette;
    int _insertAtCurrentPosition;
    bool _allowInsertAtCurrentPosition;

    bool _needsUpdate;

    uint8_t _transparentColor;

    const Cel *_currentBackgroundOptional;

    afx_msg void OnScaleClicked();
    afx_msg void OnBnClickedRadio2();
    afx_msg void OnBnClickedCheckglobalpalette();
    afx_msg void OnBnClickedRadio1();
    afx_msg void OnBnClickedRadio3();
public:
    afx_msg void OnBnClickedCheckinsert();
    afx_msg void OnEnKillfocusEdittransparentcolor();
    afx_msg void OnBnClickedButtonrefresh();
    afx_msg void OnBnClickedCheckoverlay();
};

