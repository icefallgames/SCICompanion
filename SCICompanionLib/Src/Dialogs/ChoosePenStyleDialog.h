#pragma once

#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
// CChoosePenStyleDialog dialog

class CChoosePenStyleDialog : public CBaseColorDialog, public IColorDialogCallback
{
public:
    CChoosePenStyleDialog(CWnd* pParent = NULL);  // standard constructor

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);

    void SetPenStyle(const PenStyle *pPenStyle);
    void GetPenStyle(PenStyle *pPenStyle);

	virtual ~CChoosePenStyleDialog();

// Dialog Data
	enum { IDD = IDD_DIALOGPATTERN };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    CChooseBrushStatic m_wndStatic;
    int _iUseRandomNR;
    const EGACOLOR *_pPalette;
    BYTE _bChoice;

    CExtButton m_wndCancel;
    CExtLabel m_wndStatic1;
    CExtCheckBox m_wndCheck;
};
