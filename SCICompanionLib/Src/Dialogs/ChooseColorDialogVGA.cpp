// ChooseColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ChooseColorDialogVGA.h"

// CChooseColorDialogVGA dialog

CChooseColorDialogVGA::CChooseColorDialogVGA(const PaletteComponent &palette, CWnd* pParent)
    : CBaseColorDialog(CChooseColorDialogVGA::IDD, pParent), _palette(palette)
{
    _bChoice = 0;

    // 256 things here.
    m_wndStatic.SetPalette(16, 16, reinterpret_cast<const EGACOLOR*>(palette.Mapping), ARRAYSIZE(palette.Mapping), palette.Colors, false);
    m_wndStatic.ShowSelection(TRUE);
}

void CChooseColorDialogVGA::DoDataExchange(CDataExchange* pDX)
{
    CBaseColorDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStatic);
    m_wndStatic.SetCallback(this);

    // Visuals
    DDX_Control(pDX, IDC_DIALOGDESCRIPTION, m_wndDescription);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
}

BEGIN_MESSAGE_MAP(CChooseColorDialogVGA, CBaseColorDialog)
    ON_WM_KEYDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()

void CChooseColorDialogVGA::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    if (fLeftClick)
    {
        // We only have one child - it must have been it.
        SetColor(bIndex);
        _fEnded = TRUE;
        EndDialog(IDOK);
    }
}

