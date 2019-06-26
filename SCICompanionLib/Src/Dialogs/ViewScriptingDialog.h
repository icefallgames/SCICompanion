/***************************************************************************
Copyright (c) 2019 Philip Fortier

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

#include "IViewScriptingCallback.h"
#include "ViewUIElement.h"

class ViewScriptingDialog : public CExtResizableDialog
{
public:
    ViewScriptingDialog(CWnd* pParent = nullptr);   // standard constructor

                                               // Dialog Data
    enum { IDD = IDD_VIEWSCRIPTING };

    BOOL PreTranslateMessage(MSG* pMsg) override;

    void SetResource(const ResourceEntity *resource, const PaletteComponent *palette);
    void SetLoop(int loop);
    void SetCallback(IViewScriptingCallback *callback) { _callback = callback; }

    void OnSize(UINT nType, int cx, int cy);
    void OnTimer(UINT_PTR nIDEvent);
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd);



protected:
    virtual void OnOK();

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    afx_msg void OnPlay();
    afx_msg void OnBnClickedButtonbg();
    afx_msg void OnCbnSelchangeFiles();

    void _UpdateBackground();
    void _AutoSize();
    CSize _RecalcSizeNeeded();
    void _UpdateButton();
    int _GetCelCount();
    const Cel &GetCel();
    void _PopulateFilesList();

    DECLARE_MESSAGE_MAP()

    CExtButton m_wndSave;

    CExtEdit m_wndEditCode;

    // Visuals
    CExtButton m_wndOK;
    CExtButton m_wndCancel;

    HACCEL _hAccel;

    bool _initialized;

    IViewScriptingCallback *_callback;

    // From animate dialog
    const ResourceEntity *_resource;
    const PaletteComponent *_palette;
    int _nLoop;
    ViewUIElement m_wndAnimate;
    CExtSliderWnd m_wndSlider;
    CExtButton m_wndButton;
    CExtButton m_wndButtonBG;
    CRect _rectDialogSize;
    int _nCel;
    bool _fAnimating;

    CExtComboBox m_wndFiles;
    std::vector<std::string> _fileNames;

public:
    afx_msg void OnBnClickedSave();
};
