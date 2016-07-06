/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
// ChooseColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ChooseColorDialog.h"
#include "PaletteDefinitionDialog.h"
#include "ChooseColorAdvancedDialog.h"
#include "PicCommands.h"
#include "ColorAdjustDialog.h"

volatile int g_fChecked = 1; // write entire palette by default
volatile int g_fPreview = 1; // Should be on by default

CPaletteDefinitionDialog::CPaletteDefinitionDialog(IEGAPaletteDefinitionCallback &callback, PicComponent &pic, ptrdiff_t pos, uint8_t paletteNumber, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(CPaletteDefinitionDialog::IDD, pParent), _callback(callback), _pic(pic), _position(pos), _changed(false), _copy(pic), _viewport(paletteNumber)
{
    // Store the current palette state by processing all pallet commands up to position.
    for (ptrdiff_t i = 0; i < _position; i++)
    {
        auto &command = _pic.commands[i];
        if (command.type == PicCommand::CommandType::SetPalette)
        {
            SetPaletteCommand_Draw(&command, nullptr, &_viewport);
        }
        else if (command.type == PicCommand::CommandType::SetPaletteEntry)
        {
            SetPaletteEntryCommand_Draw(&command, nullptr, &_viewport);
        }
    }
    // Set initial palette state:
    memcpy(_palette, _viewport.pPalettes, sizeof(_palette));

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
    DDX_Control(pDX, IDC_BUTTONADJUST, m_wndAdjust);

    m_wndCheck.SetCheck(g_fChecked);
    m_wndPreview.SetCheck(g_fPreview);
    if (g_fPreview)
    {
        _callback.SetPosition(-1);
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
    ON_BN_CLICKED(IDC_BUTTONADJUST, &CPaletteDefinitionDialog::OnBnClickedButtonadjust)
END_MESSAGE_MAP()

void CPaletteDefinitionDialog::OnTabChange(NMHDR *pnmhdr, LRESULT *ples)
{
    _iCurPalette = m_wndTab.GetCurSel();
    m_wndStaticPalette.SetPalette(5, 8, GetCurrentPalettePtr(), ARRAYSIZE(g_egaColors), g_egaColors);
    m_wndStaticPalette.OnPaletteUpdated();

    // Also update the selections in the colors list
    m_wndStaticColors.SetSelection(GetCurrentPalettePtr()[_bSelection].color1);
    m_wndStaticColors.SetAuxSelection(GetCurrentPalettePtr()[_bSelection].color2);

    ApplyPreview();
}

void CPaletteDefinitionDialog::ApplyPreview()
{
    if (g_fPreview)
    {
        ApplyChanges();
        if (_changed)
        {
            _callback.SetPosition(-1);
        }
    }
}

void CPaletteDefinitionDialog::OnOK()
{
    ApplyChanges();
    __super::OnOK();
}

void CPaletteDefinitionDialog::ApplyChanges()
{
    _pic = _copy;
    _changed = InsertPaletteCommands(_pic, _position, _viewport.pPalettes, _palette, GetWriteEntirePalette());
}

void CPaletteDefinitionDialog::OnCheckClick()
{
    // Update the selection.
    g_fChecked = m_wndCheck.GetCheck();
}

void CPaletteDefinitionDialog::OnPreviewToggle()
{
    g_fPreview = m_wndPreview.GetCheck();

    ApplyPreview();
    if (!g_fPreview)
    {
        // Restore
        _pic = _copy;
        _callback.SetPosition(_position);   // Back to the actual position
    }
    else
    {
        _callback.SetPosition(-1);
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

        ApplyPreview();
    }
}

bool _GetPaletteFilename(bool open, const std::string &dialogTitle, std::string &filename)
{
    CFileDialog fileDialog(open, ".pal", nullptr, OFN_NOCHANGEDIR, "PAL files (*.pal)|*.pal|All Files|*.*|");
    fileDialog.m_ofn.lpstrTitle = dialogTitle.c_str();;
    if (IDOK == fileDialog.DoModal())
    {
        filename = (PCSTR)fileDialog.GetPathName();
        return true;
    }
    return false;
}


void CPaletteDefinitionDialog::OnBnClickedButtonadjust()
{
    // Keep a copy of the palette
    EGACOLOR paletteBackup[4 * 40];
    std::copy(std::begin(_palette), std::end(_palette), paletteBackup);

    // Pretend we're a VGA palette
    bool multipleSelection[256] = {};
    // The selected colors will be those from the current palette:
    for (int i = 0; i < 40; i++)
    {
        multipleSelection[_iCurPalette * 40 + i] = true;
    }
    // Now, convert to RGB:
    paletteComponentEGA = std::make_unique<PaletteComponent>();
    std::transform(std::begin(_palette), std::end(_palette), paletteComponentEGA->Colors,
        [](EGACOLOR ega)
    {
        return EgaColorToRGBQuad(ega);
    }
        );

    ColorAdjustDialog dialog(*paletteComponentEGA, this, multipleSelection, this);
    if (IDOK == dialog.DoModal())
    {
        // We're good
    }
    else
    {
        // Restore original
        std::copy(std::begin(paletteBackup), std::end(paletteBackup), _palette);
    }
}

// Possible improvements:
//  Offer choice of matching algorithms
//  Offer subselection of colors

void CPaletteDefinitionDialog::OnVGAPaletteChanged()
{
    // Just update all 40 colors
    for (int i = 0; i < 40; i++)
    {
        COLORREF color = _ColorRefFromRGBQuad(paletteComponentEGA->Colors[_iCurPalette * 40 + i]);
        // 1: general matching, true: gamma-correct, 1: all 136 colors. 3 would be only smooth ones.
        _palette[_iCurPalette * 40 + i] = GetClosestEGAColor(1, true, 1, color);
    }

    m_wndStaticPalette.SetPalette(5, 8, GetCurrentPalettePtr(), ARRAYSIZE(g_egaColors), g_egaColors);
    m_wndStaticPalette.OnPaletteUpdated();
    ApplyPreview();
}
