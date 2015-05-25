
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
    bool IsOverClose(CPoint point, int *pindex);
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
    bool _overCloseIcon;
    bool _capturing;
};

