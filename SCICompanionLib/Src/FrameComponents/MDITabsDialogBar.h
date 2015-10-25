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

#include <afxtabctrl.h>

//
// CMDITabsDialogBar
//
// Dialog bar that hosts a CMDITabs control
//

class CScriptDocument;
class CResourceDocument;
class CMDITabChildWnd;

class CMDITabsDialogBar : public CTabCtrl
{
public:
    CMDITabsDialogBar();

    void AddTab(CFrameWnd *pFrame, MDITabType iType);
    void RemoveTab(CFrameWnd *pFrame);
    CScriptDocument *ActivateScript(ScriptId script);
    CResourceDocument *ActivateResourceDocument(ResourceType type, WORD wNum);
    CScriptDocument *GetOpenScriptDocument(WORD wNum);
    void OnActivateTab(CFrameWnd *pFrame);
    bool CanGoBack();
    bool CanGoForward();
    void GoBack();
    void GoForward();
    void OnUpdateTitles();

protected:
	DECLARE_MESSAGE_MAP()

private:
    CDocument *_GetIfKindOf(int i, const CRuntimeClass* pClass, CMDITabChildWnd **pActive) const;
    CFrameWnd *_GetNextPrev(bool fNext);
    int IsOverClose(CPoint point, int *pindex);
    void _GoBackForward(bool fForward);
    LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnSelChange(NMHDR *pnmhdr, LRESULT *lResult);
    void DrawItem(LPDRAWITEMSTRUCT);
    void DrawItemBorder(LPDRAWITEMSTRUCT);
    void DrawMainBorder(LPDRAWITEMSTRUCT);

    virtual void OnMouseMove(UINT nFlags, CPoint point);
    LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnLButtonDown(UINT nFlags, CPoint point);

    void OnPaint();
    BOOL OnEraseBkgnd(CDC *pDC);

    CFont m_font;
    CImageList m_imagelist;

    CExtBitmap _tabBitmap[13];
    CExtBitmap _tabBitmapNS[13];

    typedef std::list<CFrameWnd*> travellog_list;
    travellog_list _travelLog;
    CFrameWnd *_pActiveTab;
    bool _fNavigatingViaTravelLog;

    CExtBitmap _closeBitmap;
    CExtBitmap _closeBitmapActive;
    bool _fMouseWithin;
    int _overCloseIcon;
    bool _capturing;
    int _hoverIndex;
};

