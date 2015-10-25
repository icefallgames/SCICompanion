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

#include "Components.h"
#include "ViewUIElement.h"

#define ANIMATE_TIMER 1234

struct PaletteComponent;
class ResourceEntity;

// CAnimateDialog dialog

class CAnimateDialog : public CExtResizableDialog
{
public:
    CAnimateDialog(CWnd* pParent, const ResourceEntity *resource, const PaletteComponent *palette);   // standard constructor

    void SetTitle(PCTSTR pszTitle) { _strTitle = pszTitle; }
    void SetLoop(int nLoop);

    void OnSize(UINT nType, int cx, int cy);
    void OnTimer(UINT_PTR nIDEvent);
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd);

// Dialog Data
	enum { IDD = IDD_DIALOGANIMATE };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnPlay();

    void _UpdateBackground();
    void _AutoSize();
    CSize _RecalcSizeNeeded();
    void _UpdateButton();
    int _GetCelCount();
    const Cel &GetCel();

	DECLARE_MESSAGE_MAP()

    const ResourceEntity *_resource;
    const PaletteComponent *_palette;
    CString _strTitle;
    int _nLoop;
    ViewUIElement m_wndAnimate;
    CExtSliderWnd m_wndSlider;
    CExtButton m_wndButton;
    CExtButton m_wndButtonBG;


    CRect _rectDialogSize;
    int _nCel;
    bool _fAnimating;
    bool _fInitialized;

    // Visuals
    CExtButton m_wndOK;
public:
    afx_msg void OnBnClickedButtonbg();
};
