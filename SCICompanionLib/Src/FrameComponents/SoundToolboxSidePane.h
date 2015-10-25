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
#include "Sound.h"

struct SoundComponent;
class CSoundDoc;

class MidiCommandListBox : public CListBox
{
public:
    MidiCommandListBox(std::vector<DWORD> &_accumulatedTicksPerEvent) : CListBox(), _accumulatedTicksPerEvent(_accumulatedTicksPerEvent) {}

protected:
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    std::vector<DWORD> &_accumulatedTicksPerEvent;
};

// SoundToolboxSidePane dialog

class SoundToolboxSidePane : public CExtDialogFwdCmd, public INonViewClient
{
public:
	SoundToolboxSidePane(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SoundToolboxSidePane();

// Dialog Data
	enum { IDD = IDD_SOUNDTOOLBOX };

    void OnDeleteCommands();

    void SetDocument(CDocument *pDoc);
    CSoundDoc *GetDocument() { return _pDoc; }

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;

    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    LRESULT OnOcmDrawItem(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSelChange();
    afx_msg void OnCueChange();
    afx_msg void OnAllChannels();
    afx_msg void OnEnableLoopPoint();
    afx_msg void OnAddCue();
    afx_msg void OnDeleteCue();
    afx_msg void OnEditTime();
    afx_msg void OnEditValue();
    afx_msg void OnCheckCumulative();

private:
    const SoundComponent *GetSound();
    void _UpdateItemCount();
    void _UpdateCues();
    void _UpdateOnSelectedCue();
    void _InvalidateCurrentCaretPos();
    const std::vector<SoundEvent> &_GetCurrentChannelEvents();
    
    CSoundDoc *_pDoc;
    int _iUserSelectedPos;
    int _iCurrentPicPos;        // Current position in CommandView (equal to pic position)
    bool _fAttached;

    CFont m_font;

    CExtComboBox m_wndDevices;
    CExtCheckBox m_wndCheckAllChannels;
    CExtCheckBox m_wndCheckEnableLoopPoint;
    MidiCommandListBox m_wndList;
    CListBox m_wndCues;
    CExtEdit m_wndEditTime;
    CExtEdit m_wndEditValue;
    CExtButton m_wndAdd;
    CExtButton m_wndDelete;
    CExtCheckBox m_wndCumulative;

    // Visuals
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;
    CExtGroupBox m_wndStatic4;
    CExtLabel m_wndStatic5;

    HACCEL _hAccel;

    std::vector<DWORD> _accumulatedTicksPerEvent;
};
