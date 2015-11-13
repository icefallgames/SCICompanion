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


// EditCelDataDialog dialog
#include "resource.h"

struct Cel;

class EditCelDataDialog : public CExtResizableDialog
{
public:
    EditCelDataDialog(int16_t &priority, Cel &cel, CWnd* pParent = NULL);   // standard constructor
    virtual ~EditCelDataDialog();
    virtual void OnOK();

    // Dialog Data
    enum { IDD = IDD_EDITCELDATA };

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    Cel &_cel;
    int16_t &_priority;

    CExtEdit m_wndEditX;
    CExtEdit m_wndEditY;
    CExtEdit m_wndEditPriority;

    // Visuals
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
};
