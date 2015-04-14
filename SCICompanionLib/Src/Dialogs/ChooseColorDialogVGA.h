#pragma once

#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"

class CChooseColorDialogVGA : public CBaseColorDialog, public IColorDialogCallback
{
public:
    CChooseColorDialogVGA(const PaletteComponent &palette, CWnd* pParent = NULL);  // standard constructor

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);

    BYTE GetColor() { return _bChoice; }
    void SetColor(BYTE bChoice) { _bChoice = bChoice; m_wndStatic.SetSelection(bChoice); }

    // Dialog Data
    enum { IDD = IDD_DIALOGCOLORVGA };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    CChooseColorStatic m_wndStatic;

    // Visuals
    CExtLabel m_wndDescription;
    CExtButton m_wndCancel;

    BYTE _bChoice;

    const PaletteComponent &_palette;
};
