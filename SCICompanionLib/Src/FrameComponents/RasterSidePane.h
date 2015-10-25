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
#pragma once


#include "NonViewClient.h"
#include "ColorButton.h"
#include "ChooseColorStatic.h"
#include "ColorButton.h"
#include "DialogFactory.h"
#include "ExtDialogFwdCmd.h"
#include "ResourceMap.h"

// fwd decl
class CResourceDocument;
class CNewRasterResourceDocument;

// RasterSidePane

class RasterSidePane : public CExtDialogFwdCmd, public INonViewClient, public IColorDialogCallback, public IDialogFactory
{
public:
	RasterSidePane(CWnd *pParent = NULL);
	virtual ~RasterSidePane();

    void SetDocument(CDocument *pDoc);

    // INonViewClient
    virtual void UpdateNonView(CObject *pObject) override;

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeft);

    // IDialogFactory
    virtual void InvokeDialog(UINT nID, RECT *prcTarget);

    void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    BOOL PreTranslateMessage(MSG* pMsg);

private:
    void DoDataExchange(CDataExchange* pDX);
    void OnMirrorSelection();
    void OnPaletteSelection();
    void OnChooseLoop();
    void OnAddCelBefore();
    void OnAddCelAfter();
    void OnDeleteCel();
    void OnAddLoopBefore();
    void OnAddLoopAfter();
    void OnDeleteLoop();
    void OnEditCelWidth();
    void OnEditCelHeight();
    void OnEditCelX();
    void OnEditCelY();
    void OnEditLineHeight();
    void OnEditNumChars();
    void OnEditSampleText();
    void OnUp();
    void OnDown();
    void OnRight();
    void OnLeft();
    void OnMakeFont();
    void OnPenStyle();
    void OnApplyToAll();
    void OnIsScalable();

    void _UpdatePaletteChoices();
    void _UpdatePaletteResources();
    void _OnUpdateCommandUIs();
    void OnTreeSelectionChanged(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
    void _SyncCelPane();
    void _SyncLoopPane();
    void _SyncMirror();
    void _SyncSampleText();
    void _OnAddCel(bool before);
    void _OnAddLoop(bool before);
    void _OnEditSize(UINT nID, CWnd *pwnd);
    void _OnPenStyleChanged();
    void _OnDirectionButton(CPoint point);
    void _SyncPalette();

private:
    CNewRasterResourceDocument *_pDoc;

    // Cels:
    bool _fSupportsSize;
    CExtEdit m_wndCelWidth;
    CExtEdit m_wndCelHeight;
    bool _fSupportsPlacement;
    CExtEdit m_wndCelX;
    CExtEdit m_wndCelY;

    bool _fTransparency;
    CColorPickerButton m_wndTransparent;

    // Loops:
    bool _fSupportsLoops;
    CExtComboBox m_wndMirrorOf;
    CExtLabel m_wndStaticMirrorOf;

    // View-specific
    CExtComboBox m_wndPaletteChoice;
    CExtButton m_wndEditPaletteButton;

    // Color: (supported by all)
    CChooseColorStatic m_wndPalette;
    CExtLabel m_wndChosenColors;

    // Font related things
    bool _fSupportsFonts;
    CEdit m_wndLineHeight;
    CEdit m_wndSampleText;
    CEdit m_wndNumChars;

    // Wingdings font for directional buttons
    CFont m_font;

    // "Apply to all" checkbox.
    bool _fSupportsApplyAll;
    CExtCheckBox m_wndApplyToAll;

    // This classes needed purely for visuals
    CExtButton m_wndLeft;
    CExtButton m_wndRight;
    CExtButton m_wndUp;
    CExtButton m_wndDown;
    CExtButton m_wndLoopAfter;
    CExtButton m_wndLoopBefore;
    CExtButton m_wndDeleteLoop;
    CExtButton m_wndCelAfter;
    CExtButton m_wndCelBefore;
    CExtButton m_wndDeleteCel;
    CExtButton m_wndMakeFont;
    CBrushButton m_wndButtonPenStyle;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;
    CExtLabel m_wndStatic4;
    CExtLabel m_wndStatic5;
    CExtLabel m_wndStatic6;
    CExtLabel m_wndStatic7;
    CExtLabel m_wndStatic8;
    CExtLabel m_wndStatic9;
    CExtGroupBox m_wndGroup1;
    CExtGroupBox m_wndGroup2;
    CExtCheckBox m_wndIsScalable;

    int _cRows;
    int _cColumns;

    bool _fRebuildingTree;

    HACCEL _hAccel;
};


