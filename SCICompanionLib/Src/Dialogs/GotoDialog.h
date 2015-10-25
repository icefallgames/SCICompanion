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
#include "afxwin.h"


// CGotoDialog dialog

class CGotoDialog : public CExtResizableDialog
{
public:
	CGotoDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGotoDialog();
    int GetLineNumber() { return _iLineNumber; }

// Dialog Data
	enum { IDD = IDD_DIALOG_GOTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CExtEdit m_LineNumber;
    int _iLineNumber;
    std::string Title;
    std::string Label;

    // Visuals
    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    CExtLabel m_wndStatic1;
};
