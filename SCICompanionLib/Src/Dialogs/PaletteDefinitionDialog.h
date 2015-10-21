#pragma once

#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
#include "PaletteDefinitionCallback.h"
#include "Pic.h"
#include "PicDoc.h"

struct PicComponent;

extern volatile int g_fChecked;
extern volatile int g_fPreview;

class CPaletteDefinitionDialog : public CExtResizableDialog, public IColorDialogCallback
{
public:
    CPaletteDefinitionDialog(IVGAPaletteDefinitionCallback &callback, PicComponent &pic, ptrdiff_t pos, CWnd* pParent = nullptr);  // standard constructor

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);
    void OnColorHover(BYTE bIndex) {};

	virtual ~CPaletteDefinitionDialog();

    bool GetChanged() { return _changed; }

// Dialog Data
	enum { IDD = IDD_DEFINEPALETTE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnTabChange(NMHDR *pnmhdr, LRESULT *ples);
    void OnCheckClick();
    void OnPreviewToggle();
    void OnAdvancedClick();
    void ApplyPreview();
    void ApplyChanges();
    void OnOK() override;
    BOOL GetWriteEntirePalette() { return g_fChecked; }
    EGACOLOR *GetCurrentPalettePtr() { return &_palette[_iCurPalette * 40]; }

	DECLARE_MESSAGE_MAP()

    CChooseColorStatic m_wndStaticPalette;
    CChooseColorStatic m_wndStaticColors;
    CTabCtrl m_wndTab;
    CExtCheckBox m_wndCheck;
    CExtCheckBox m_wndPreview;

    // Visuals
    CExtButton m_wndAdvanced;
    CExtButton m_wndOK;
    CExtButton m_wndCancel;

    EGACOLOR _palette[4 * 40];

    int _iCurPalette; // from 0 to 3
    BOOL _bSelection;

    IVGAPaletteDefinitionCallback &_callback;
    PicComponent &_pic;
    PicComponent _copy;
    ptrdiff_t _position;
    ViewPort _viewport;
    bool _changed;
};
