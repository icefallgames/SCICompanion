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

#include "CompileContext.h"

// CCompileDialog dialog

class CNewCompileDialog : public CExtResizableDialog
{
public:
	CNewCompileDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewCompileDialog();
    bool GetResult() { return _fResult; }
    bool GetAborted() { return _fAbort; }
    virtual void OnCancel();

// Dialog Data
	enum { IDD = IDD_COMPILEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    LRESULT CompileAll(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnInitDialog();
    virtual void OnDestroy();
	DECLARE_MESSAGE_MAP()

    CExtProgressWnd m_wndProgress;
    CExtEdit m_wndDisplay;
    bool _fResult;
    bool _fAbort;
    bool _fDone;
    int _nScript;
    std::vector<ScriptId> _scripts;
    CompileTables _tables;
    PrecompiledHeaders _headers;
    CompileLog _log;

    // Visuals
    CExtButton m_wndCancel;
};
