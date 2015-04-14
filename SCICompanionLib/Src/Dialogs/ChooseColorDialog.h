#pragma once

#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
// CChooseColorDialog dialog

class CChooseColorDialog : public CBaseColorDialog, public IColorDialogCallback
{
public:
    CChooseColorDialog(CWnd* pParent = NULL);  // standard constructor

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);

    void Init(const EGACOLOR *pPalette);
    BYTE GetColor() { return _bChoice; }
    void SetColor(BYTE bChoice) { _bChoice = bChoice; m_wndStatic.SetSelection(bChoice); }

	virtual ~CChooseColorDialog();

// Dialog Data
	enum { IDD = IDD_DIALOGCOLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    CChooseColorStatic m_wndStatic;

    // Visuals
    CExtLabel m_wndDescription;
    CExtButton m_wndCancel;

    const EGACOLOR *_pPalette;
    BYTE _bChoice;
};
