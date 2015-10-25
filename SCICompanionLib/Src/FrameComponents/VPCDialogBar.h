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
#include "ExtDialogFwdCmd.h"
#include "NonViewClient.h"

class CPicDoc;

// CVPCDialogBar

class CVPCDialogBar : public CExtDialogFwdCmd, public INonViewClient
{
public:
    CVPCDialogBar(CWnd* pParent = NULL);
	virtual ~CVPCDialogBar();

    void SetDocument(CDocument *pDoc);

    // INonViewClient
    virtual void UpdateNonView(CObject *pObject) override;

protected:
    void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

private:
    CPicDoc *_pDoc;

    CExtCheckBox m_wndVToggle;
    CExtCheckBox m_wndPToggle;
    CExtCheckBox m_wndCToggle;
    CColorPickerButton m_wndVSet;
    CColorPickerButton m_wndPSet;
    CColorPickerButton m_wndCSet;

    CBrushButton m_wndPenSet;

    // Visuals
    CExtLabel m_wndLabel;
};


