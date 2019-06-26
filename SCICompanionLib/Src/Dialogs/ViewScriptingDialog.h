/***************************************************************************
Copyright (c) 2019 Philip Fortier

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

#include "IViewScriptingCallback.h"

class ViewScriptingDialog : public CExtResizableDialog
{
public:
    ViewScriptingDialog(CWnd* pParent = nullptr);   // standard constructor

                                               // Dialog Data
    enum { IDD = IDD_VIEWSCRIPTING };

    BOOL PreTranslateMessage(MSG* pMsg) override;

    void SetNumber(int number);
    void SetCallback(IViewScriptingCallback *callback) { _callback = callback; }

protected:
    virtual void OnOK();

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    CExtButton m_wndSave;

    CExtEdit m_wndEditCode;

    // Visuals
    CExtButton m_wndOK;

    HACCEL _hAccel;

    bool _initialized;

    int _number;

    IViewScriptingCallback *_callback;

public:
    afx_msg void OnBnClickedSave();
};
