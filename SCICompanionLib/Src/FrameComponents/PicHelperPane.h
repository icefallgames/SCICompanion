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
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"
#include "ZoomCelStatic.h"
#include "PicDrawManager.h"
#include "resource.h"

class CPicDoc;
class CPicView;
struct PicComponent;

class PicHelperPane : public CExtResizableDialog, public INonViewClient, public IPicDrawPlugin
{
public:
    PicHelperPane(CWnd* pParent = NULL);   // standard constructor
    virtual ~PicHelperPane();

    // Dialog Data
    enum
    {
        IDD = IDD_PICHELPER,
    };

    // IPicDrawPlugin
    void DrawOnPic(ViewPort &viewPort, PicData &picData, PicScreenFlags flags) override {}
    bool WillDrawOnPic() override { return false; }
    PicScreenFlags GetRequiredScreens() override;
    PicPositionFlags GetRequiredPicPosition() override {
        return PicPositionFlags::PostPlugin | PicPositionFlags::Final;      // Potentially we could hide the thing, then only require 
    }

    void SetDocument(CDocument *pDoc);
    CPicDoc *GetDocument() { return _pDoc; }

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;

    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    BOOL OnEraseBkgnd(CDC *pDC);
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    void _Sync();

    DECLARE_MESSAGE_MAP()

private:
    const PicComponent *_GetEditPic();

    CPicDoc *_pDoc;
    IBitmapEditor *_be;
    CPoint _cursorPos;

    bool _fHello;

    ZoomCelStatic m_wndZoomCelStatic;

    CExtCheckBox m_wndFollowMainView;
    CExtCheckBox m_wndFollowMainView2;

    // Visual
    CExtLabel m_wndText1;
    CExtCheckBox m_wndCheckV;
    CExtCheckBox m_wndCheckP;
    CExtCheckBox m_wndCheckC;
    CExtLabel m_wndText2;
    CExtCheckBox m_wndCheckV2;
    CExtCheckBox m_wndCheckP2;
    CExtCheckBox m_wndCheckC2;

    ZoomCelStatic m_wndFinal;

    HACCEL _hAccel;
    bool _initialized;
public:
    afx_msg void OnBnClickedCheckv();
    afx_msg void OnBnClickedCheckp();
    afx_msg void OnBnClickedCheckc();
    afx_msg void OnBnClickedCheckFollowview2();
    afx_msg void OnBnClickedCheckv2();
    afx_msg void OnBnClickedCheckp2();
    afx_msg void OnBnClickedCheckc2();
    afx_msg void OnBnClickedCheckFollowview();
};
