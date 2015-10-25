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

#include "NewScriptDialog.h"

// fwd decl
class ScriptId;

//
// CNewRoomDialog dialog
//
// This dialog will return the new scriptnumber, the text to use in this script,
// and the script name (script id).  It will also write the appropriate entry in
// the game.ini
//
class CNewRoomDialog : public CNewScriptDialog
{
	DECLARE_DYNAMIC(CNewRoomDialog)

public:
	CNewRoomDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewRoomDialog();

// Dialog Data
	enum { IDD = IDD_NEWROOM };

protected:
    virtual void OnOK();
    virtual void _PrepareDialog();
    virtual void _DiscoveredScriptName(PCTSTR pszName);
    virtual void _PrepareBuffer();
    int _GetMinSuggestedScriptNumber() override;
    void _PrepareBufferOld();
    virtual void _AttachControls(CDataExchange *pDX);
	DECLARE_MESSAGE_MAP()
    afx_msg void OnRadioSameAsRoom();
    afx_msg void OnRadioOther();

    int _nPicScript;

    CExtRadioButton m_wndRadioUseRoom;
    CExtRadioButton m_wndRadioUseOther;
    CListBox m_wndListBox;
    CExtEdit m_wndEditPicNumber;
    CExtCheckBox m_wndCheckMessage;
    CExtCheckBox m_wndCheckPolys;

    // Visuals
    CExtEdit m_wndRoomNumber;
    CExtLabel m_wndRoomNumberLabel;
    CExtLabel m_wndUses;
    CExtGroupBox m_wndPicNumberGroup;
};
