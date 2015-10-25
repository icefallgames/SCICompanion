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

#include "Resource.h"
// CBarContainerDialog dialog

class CBarContainerDialog : public CExtResizableDialog
{
public:
	CBarContainerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBarContainerDialog();

// Dialog Data
	enum { IDD = IDD_DIALOGEMPTY };

    // iType can be any unique type (e.g. MDITabType, or ResourceType)
    // REVIEW: we could templatize this.
    void SelectChild(CWnd *pWnd, int iType);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	typedef std::unordered_map<int, CWnd *> windowmap;
    windowmap _childWindows;
};
