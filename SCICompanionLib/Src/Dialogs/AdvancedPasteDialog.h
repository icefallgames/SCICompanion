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

class ResourceEntity;
class CNewRasterResourceDocument;
struct sRECT;


class AdvancedPasteDialog : public CExtResizableDialog
{
public:
    AdvancedPasteDialog(CWnd* pParent = nullptr);   // standard constructor

                                               // Dialog Data
    enum { IDD = IDD_ADVANCEDPASTE };

    BOOL PreTranslateMessage(MSG* pMsg) override;
    void SetRasterDocument(CNewRasterResourceDocument *pDoc);

private:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()


    // Visuals
    CExtButton m_wndOk;
    CExtSpinWnd m_wndSourceLoop;

    HACCEL _hAccel;

    bool _initialized;
    CNewRasterResourceDocument *_pDoc;

    int _destLoop;
    int _sourceLoop;
    int _blendOp;
    BOOL _fillAllCels;

public:
    afx_msg void OnBnClickedDoit();

};
