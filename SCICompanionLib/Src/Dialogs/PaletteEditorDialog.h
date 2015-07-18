#pragma once

#include "resource.h"
#include "NonViewClient.h"
#include "PaletteEditorCommon.h"

class CResourceDocument;
struct PaletteComponent;
struct Cel;

class PaletteEditorDialog : public PaletteEditorCommon<CExtResizableDialog>, public INonViewClient
{
public:
    PaletteEditorDialog(IVGAPaletteDefinitionCallback *callback, PaletteComponent &palette, const std::vector<const Cel*> &cels, bool enableCycling);  // standard constructor
    PaletteEditorDialog();

    // Dialog Data
    enum { IDD = IDD_PALETTE_EDITOR };

    void DoDataExchange(CDataExchange* pDX) override;

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedButtonSaverange();

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick) override;

protected:
    void OnCancel() override;
    void OnOK() override;

private:
    void OnTimer(UINT_PTR nIDEvent);
    void _UpdateCycleUI();
    void _SyncCheckState();
    void _SyncSelectionCycle();

    CExtCheckBox m_wndPreviewCycling;
    CExtRadioButton m_wndCycleLeft;
    CExtRadioButton m_wndCycleRight;
    CExtSliderWnd m_wndCycleSlider;
    int _cycleCountdown;
    CExtButton m_wndSaveAs;
    bool _enableCycling;
    bool _cycleForward;
    bool _cycling;
    bool _initialized;
    std::unique_ptr<PaletteComponent> _cyclePaletteCopy;
public:
    afx_msg void OnBnClickedCheckpreviewcycling();
    afx_msg void OnBnClickedButtoncycleleft();
    afx_msg void OnBnClickedButtoncycleright();
    afx_msg void OnSaveAsResource();
};
