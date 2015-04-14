// ChooseColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ChooseColorDialog.h"


// CChooseColorDialog dialog

CChooseColorDialog::CChooseColorDialog(CWnd* pParent /*=NULL*/)
	: CBaseColorDialog(CChooseColorDialog::IDD, pParent)
{
    _pPalette = NULL;
    _bChoice = 0;

    // 40 things here.
    m_wndStatic.SetPalette(5, 8, nullptr, 0, nullptr);
    m_wndStatic.ShowSelection(TRUE);
}

CChooseColorDialog::~CChooseColorDialog()
{
}



void CChooseColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseColorDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStatic);
    m_wndStatic.SetCallback(this);

    // Visuals
    DDX_Control(pDX, IDC_DIALOGDESCRIPTION, m_wndDescription);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
}

void CChooseColorDialog::Init(const EGACOLOR *pPalette)
{
    _pPalette = pPalette;
    m_wndStatic.SetPalette(5, 8, _pPalette, ARRAYSIZE(g_egaColors), g_egaColors);
}

BEGIN_MESSAGE_MAP(CChooseColorDialog, CBaseColorDialog)
    ON_WM_KEYDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()


void CChooseColorDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    if (fLeftClick)
    {
        // We only have one child - it must have been it.
        SetColor(bIndex);
        _fEnded = TRUE;
        EndDialog(IDOK);
    }
}

