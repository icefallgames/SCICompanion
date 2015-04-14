#pragma once

#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
#include "PaletteDefinitionCallback.h"

extern volatile int g_fChecked;
extern volatile int g_fPreview;

class CPaletteDefinitionDialog : public CExtResizableDialog, public IColorDialogCallback
{
public:
    CPaletteDefinitionDialog(CWnd* pParent = NULL);  // standard constructor

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);
    void OnColorHover(BYTE bIndex) {};

    void InitPalettes(const EGACOLOR *pPalette); // copy to palette
    void RetrievePalettes(EGACOLOR *pPalette); // copy back out.
    BOOL GetWriteEntirePalette() { return g_fChecked; }
    void SetCallback(IPaletteDefinitionCallback *pCallback) { _pCallbackWeak = pCallback; }

	virtual ~CPaletteDefinitionDialog();

// Dialog Data
	enum { IDD = IDD_DEFINEPALETTE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnTabChange(NMHDR *pnmhdr, LRESULT *ples);
    void OnCheckClick();
    void OnPreviewToggle();
    void OnAdvancedClick();
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
    IPaletteDefinitionCallback *_pCallbackWeak;
};
