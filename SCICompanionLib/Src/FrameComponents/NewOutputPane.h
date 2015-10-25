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
#include "CompileInterfaces.h"
#include "ExtTabControl.h"
#include "ExtDialogFwdCmd.h"

class NewOutputPane : public CExtDialogFwdCmd
{
public:
    NewOutputPane(CWnd* pParent = NULL);

    // Dialog Data
    enum
    {
        IDD = IDD_OUTPUTPANE,
    };

    void ClearResults(OutputPaneType type);
    void AddBatch(OutputPaneType type, std::vector<CompileResult> &compileResults);
    void FinishAdd(OutputPaneType type);

    void ShowPage(OutputPaneType type);

protected:
    void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    void DoDataExchange(CDataExchange* pDX) override;
    BOOL OnInitDialog() override;
    BOOL PreTranslateMessage(MSG* pMsg) override;

    void DrawItem(DRAWITEMSTRUCT *pDrawItemStruct);

    DECLARE_MESSAGE_MAP()


private:
    void _SyncCombo();
    void _SyncTabSelection();
    int _GetCurrentType();
    CListBox m_wndListBox;
    ExtTabControl m_wndTabs;

    std::vector<CompileResult> _results[3];

public:
    afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLbnDblclkList1();
    afx_msg void OnCopy();
    // afx_msg void OnUpdateCopy(CCmdUI *pCmdUI);
};