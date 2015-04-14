#pragma once

//
// CMDITabChildWnd frame
//
// Implements "tabbing" by adding/removing itself to the tabs exposed by CMainFrame.
//
class CMDITabChildWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMDITabChildWnd)

public:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual void OnDestroy();
    virtual void OnMDIActivate(BOOL fActivate, CWnd *pActivate, CWnd *pDeactivate);
    virtual void HookUpNonViews(CDocument *pDoc) {};
    virtual MDITabType GetTabType() { return TAB_NONE; }

protected:
	CMDITabChildWnd();           // protected constructor used by dynamic creation
	virtual ~CMDITabChildWnd();
    void _UpdateFrameTitle(CDocument *pDocument);

protected:
	DECLARE_MESSAGE_MAP()
    virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
    virtual void OnIdleUpdateCmdUI();
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

private:
    void _RemoveSelf();

    bool _fModifiedCache;
    bool _fFirstTime;
    bool _fRemovedSelf;
};


