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
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"
#include "ExtTabControl.h"

class CPicDoc;
class SCIPolygon;
struct PicComponent;

class PicCommandListBox : public CListBox
{
protected:
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

class PolygonListBox : public CListBox
{
protected:
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

// PicCommandSidePane dialog

class PicCommandSidePane : public CExtDialogFwdCmd, public INonViewClient
{
public:
    PicCommandSidePane(bool showPalette, CWnd* pParent = NULL);   // standard constructor
	virtual ~PicCommandSidePane();

// Dialog Data
	enum
    {
        IDD = IDD_PICCOMMANDS,
        IDD_VGA = IDD_PICCOMMANDS_VGA1,
    };

    void OnDeleteCommands();

    void SetDocument(CDocument *pDoc);
    CPicDoc *GetDocument() { return _pDoc; }
    int GetCurrentPosition() { return _iCurrentPicPos; }

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;

    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    LRESULT OnOcmDrawItem(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSelChange();
    afx_msg void OnPolySelChange();
    afx_msg void OnGotoScript();
    afx_msg void OnClickPolygons();
    afx_msg void OnClickCommands();
    afx_msg void OnBnClickedShowPolys();
    afx_msg void OnCbnSelchangeComboPolyType();
    afx_msg void PushEditPointsToPoly();
    afx_msg void PushNameToPoly();
    afx_msg void OnTcnSelchangeTabWhichList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPaletteSelection();
    BOOL OnEraseBkgnd(CDC *pDC);
    void OnCropCommands();
    void OnCopyCommands();
    void OnCutCommands();
    void _OnUpdateCommands();
    void _SyncPaletteChoice();

private:
    void _ShowPolyOrCommands();
    void _OnDelete(bool fCut, bool fCopy);
    void _UpdateItemCount();
    void _UpdatePolyItemCount();
    const PicComponent *_GetEditPic();
    void _InvalidateCurrentCaretPos();
    void _UpdatePalette();
    void _SyncPolyTypeCombo();
    void _SyncPolyChoice();
    const SCIPolygon *_GetCurrentPolygon();
    void _UpdatePaletteChoices();
    bool _ShowingPolygons();
    
    CPicDoc *_pDoc;
    int _iUserSelectedPos;
    int _iCurrentPicPos;        // Current position in CommandView (equal to pic position)
    bool _fAttached;

    CFont m_font;

    PicCommandListBox m_wndListCommands;
    CExtNoFlickerStatic m_wndPalette;
    bool _showPalette;

    PolygonListBox m_wndListPolygons;
    CExtLabel m_wndStaticPolyType;
    CExtLabel m_wndStaticPolyName;
    CExtComboBox m_wndComboPolyType;
    CExtCheckBox m_wndCheckShowPolys;
    CExtEdit m_wndEditPolyPoints;
    CExtEdit m_wndEditPolyName;

    CExtHyperLinkButton m_wndGotoScript;

    ExtTabControl m_wndTabWhichList;

    // Visual
    CExtButton m_wndUploadPointsButton;
    CExtButton m_wndUploadNameButton;
    CExtButton m_wndEditPaletteButton;
    CExtComboBox m_wndComboPaletteChoices;
    CExtButton m_wndDelete;
    CExtButton m_wndCopy;
    CExtButton m_wndCrop;
    CExtButton m_wndSetBackground;
    HACCEL _hAccel;
};
