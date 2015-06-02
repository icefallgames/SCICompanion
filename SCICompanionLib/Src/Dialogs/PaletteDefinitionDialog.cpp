// ChooseColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ChooseColorDialog.h"
#include "PaletteDefinitionDialog.h"
#include "ChooseColorAdvancedDialog.h"

volatile int g_fChecked = 0;
volatile int g_fPreview = 0;

CPaletteDefinitionDialog::CPaletteDefinitionDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CPaletteDefinitionDialog::IDD, pParent)
{
    _bSelection = 0;
    _iCurPalette = 0;

    // 40 things here.
    m_wndStaticPalette.ShowSelection(TRUE);
    m_wndStaticPalette.SetSelection(_bSelection); // Just always start with zero.
    m_wndStaticPalette.SetPalette(5, 8, _palette, ARRAYSIZE(g_egaColors), g_egaColors); // In which someone will copy something.

    // Just 16 here.
    m_wndStaticColors.ShowSelection(TRUE);
    m_wndStaticColors.ShowSelectionNumbers(TRUE); // show a 1 and 2
    m_wndStaticColors.SetPalette(2, 8, g_egaColorChooserPalette, ARRAYSIZE(g_egaColors), g_egaColors);// just provide a basic palette

}

CPaletteDefinitionDialog::~CPaletteDefinitionDialog()
{
}

void CPaletteDefinitionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStaticColors);
    m_wndStaticColors.SetCallback(this);
    DDX_Control(pDX, IDC_PALETTECOLORS, m_wndStaticPalette);
    m_wndStaticPalette.SetCallback(this);

    // The tab control:
    DDX_Control(pDX, IDC_PALETTETAB, m_wndTab);
    for (int i = 0; i < 4; i++)
    {
        TCHAR szBuf[4];
        StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), i);
        m_wndTab.InsertItem(i, szBuf);
    }
    m_wndTab.SetCurSel(0); // Set first one active

    DDX_Control(pDX, IDC_CHECK1, m_wndCheck);
    DDX_Control(pDX, IDC_CHECK2, m_wndPreview);

    m_wndCheck.SetCheck(g_fChecked);
    m_wndPreview.SetCheck(g_fPreview);
    if (g_fPreview && _pCallbackWeak)
    {
        _pCallbackWeak->OnSomethingChanged(g_fChecked, _palette, _iCurPalette);
    }

    // Visuals
    DDX_Control(pDX, IDC_BUTTONADVANCED, m_wndAdvanced);
    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);

    // Set initial selection in color boxes.
    m_wndStaticColors.SetSelection(GetCurrentPalettePtr()[_bSelection].color1);
    m_wndStaticColors.SetAuxSelection(GetCurrentPalettePtr()[_bSelection].color2);
    m_wndStaticPalette.SetPalette(5, 8, GetCurrentPalettePtr(), ARRAYSIZE(g_egaColors), g_egaColors);
    m_wndStaticPalette.OnPaletteUpdated();
}


BEGIN_MESSAGE_MAP(CPaletteDefinitionDialog, CExtResizableDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_PALETTETAB, OnTabChange)
    ON_BN_CLICKED(IDC_CHECK1, OnCheckClick)
    ON_BN_CLICKED(IDC_CHECK2, OnPreviewToggle)
    ON_BN_CLICKED(IDC_BUTTONADVANCED, OnAdvancedClick)
END_MESSAGE_MAP()


void CPaletteDefinitionDialog::InitPalettes(const EGACOLOR *pPalette)
{
    CopyMemory(_palette, pPalette, sizeof(_palette));

    // Set initial selection in color boxes.
    if (m_wndStaticColors.GetSafeHwnd())
    {
        m_wndStaticColors.SetSelection(GetCurrentPalettePtr()[_bSelection].color1);
        m_wndStaticColors.SetAuxSelection(GetCurrentPalettePtr()[_bSelection].color2);
        m_wndStaticPalette.SetPalette(5, 8, GetCurrentPalettePtr(), ARRAYSIZE(g_egaColors), g_egaColors);
        m_wndStaticPalette.OnPaletteUpdated();
    }
}

void CPaletteDefinitionDialog::RetrievePalettes(EGACOLOR *pPalette)
{
    CopyMemory(pPalette, _palette, sizeof(_palette));
}

void CPaletteDefinitionDialog::OnTabChange(NMHDR *pnmhdr, LRESULT *ples)
{
    _iCurPalette = m_wndTab.GetCurSel();
    m_wndStaticPalette.SetPalette(5, 8, GetCurrentPalettePtr(), ARRAYSIZE(g_egaColors), g_egaColors);
    m_wndStaticPalette.OnPaletteUpdated();

    // Also update the selections in the colors list
    m_wndStaticColors.SetSelection(GetCurrentPalettePtr()[_bSelection].color1);
    m_wndStaticColors.SetAuxSelection(GetCurrentPalettePtr()[_bSelection].color2);

    // And finally, if we're previewing...
    if (g_fPreview && _pCallbackWeak)
    {
        _pCallbackWeak->OnSomethingChanged(g_fChecked, _palette, _iCurPalette);
    }
}

void CPaletteDefinitionDialog::OnCheckClick()
{
    // Update the selection.
    g_fChecked = m_wndCheck.GetCheck();
}

void CPaletteDefinitionDialog::OnPreviewToggle()
{
    g_fPreview = m_wndPreview.GetCheck();

    if (_pCallbackWeak)
    {
        if (g_fPreview)
        {
            _pCallbackWeak->OnSomethingChanged(g_fChecked, _palette, _iCurPalette);
        }
        else
        {
            _pCallbackWeak->OnPreviewOff();
        }
    }
}

void CPaletteDefinitionDialog::OnAdvancedClick()
{
    CChooseColorAdvancedDialog dialog;
    EGACOLOR color = GetCurrentPalettePtr()[_bSelection];
    dialog.SetColor(color);
    if (IDOK == dialog.DoModal())
    {

        color = dialog.GetColor();

        GetCurrentPalettePtr()[_bSelection] = color;
        m_wndStaticColors.SetSelection(color.color1);
        m_wndStaticColors.SetAuxSelection(color.color2);

        m_wndStaticPalette.SetPalette(5, 8, GetCurrentPalettePtr(), ARRAYSIZE(g_egaColors), g_egaColors);
        m_wndStaticPalette.OnPaletteUpdated();
    }
}

void CPaletteDefinitionDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    // Figure out which child it was from.
    if (nID == m_wndStaticPalette.GetDlgCtrlID())
    {
        // It was from the palette.  Make this the new selection.
        if (_bSelection < 40)
        {
            _bSelection = bIndex;
            m_wndStaticPalette.SetSelection(_bSelection);

            m_wndStaticColors.SetSelection(GetCurrentPalettePtr()[_bSelection].color1);
            m_wndStaticColors.SetAuxSelection(GetCurrentPalettePtr()[_bSelection].color2);
        }
    }
    else // colors.
    {
        ASSERT(nID == m_wndStaticColors.GetDlgCtrlID());
        ASSERT(bIndex < 16);
        ASSERT(_bSelection < 40);
        if (fLeftClick)
        {
            // Change color 1.
            GetCurrentPalettePtr()[_bSelection].color1 = bIndex;
            m_wndStaticColors.SetSelection(bIndex);
        }
        else
        {
            // Change color 2.
            GetCurrentPalettePtr()[_bSelection].color2 = bIndex;
            m_wndStaticColors.SetAuxSelection(bIndex);
        }
        m_wndStaticPalette.SetPalette(5, 8, GetCurrentPalettePtr(), ARRAYSIZE(g_egaColors), g_egaColors);
        m_wndStaticPalette.OnPaletteUpdated();

        if (_pCallbackWeak && g_fPreview)
        {
            // If we're supposed to preview, tell the callback that something changed.
            _pCallbackWeak->OnSomethingChanged(g_fChecked, _palette, _iCurPalette);
        }
    }
}

bool _GetPaletteFilename(bool open, const std::string &dialogTitle, std::string &filename)
{
    CFileDialog fileDialog(open, nullptr, nullptr, 0, "PAL files (*.pal)|*.pal|All Files|*.*|");
    fileDialog.m_ofn.lpstrTitle = dialogTitle.c_str();;
    if (IDOK == fileDialog.DoModal())
    {
        filename = (PCSTR)fileDialog.GetPathName();
        return true;
    }
    return false;
}
