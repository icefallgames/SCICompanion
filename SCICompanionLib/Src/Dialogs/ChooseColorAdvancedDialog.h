#pragma once

#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"

class CChooseColorAdvancedDialog : public CExtResizableDialog, public IColorDialogCallback
{
public:
    CChooseColorAdvancedDialog(CWnd* pParent = NULL);  // standard constructor

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);

    EGACOLOR GetColor();
    void SetColor(EGACOLOR color);

	virtual ~CChooseColorAdvancedDialog();

// Dialog Data
	enum { IDD = IDD_COLORADVANCED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    CChooseColorStatic m_wndStatic;
    EGACOLOR _allColorsPalette[256];
    BYTE _bChoice;

    // Visuals
    CExtButton m_wndCancel;
    CExtLabel m_wndDesc;
};
