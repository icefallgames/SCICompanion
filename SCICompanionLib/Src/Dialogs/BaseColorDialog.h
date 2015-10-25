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

#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
// CBaseColorDialog dialog

class CBaseColorDialog : public CExtResizableDialog
{
public:
    CBaseColorDialog(UINT nID, CWnd* pParent);
    void SetTrackRect(RECT *prc) { _rcTrack = *prc; _fUseTrack = TRUE; }

    virtual ~CBaseColorDialog();

protected:
    virtual void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    void _SetPosition(); // Determine the right position to start this in.

    RECT _rcTrack;
    BOOL _fUseTrack;
    BOOL _fEnded;

    CWnd m_wndDummyOwner;
};
