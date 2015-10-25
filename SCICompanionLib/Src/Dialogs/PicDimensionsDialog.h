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

#include "resource.h"

class PicDimensionsDialog : public CExtResizableDialog
{
public:
    PicDimensionsDialog(size16 size, CWnd* pParent = NULL);   // standard constructor

    size16 GetSize() { return _size; }
    virtual void OnOK();

// Dialog Data
	enum { IDD = IDD_PICSIZE };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    size16 _size;
    CExtEdit m_wndEditWidth;
    CExtEdit m_wndEditHeight;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
};
