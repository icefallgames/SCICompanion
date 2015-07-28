// PicChildFrm.h : interface of the CPicChildFrame class
//


#pragma once

#include "SliderDialogBar.h"
#include "VPCDialogBar.h"
#include "MDITabChildWnd.h"
#include "PicDoc.h"
#include "CObjectWrap.h"
#include "PicHelperPane.h"
#include "ExtControlBarNoX.h"

// fwd decl
class CPicView;
class CPicCommandView;

class CExtPanelControlBarNoMargin : public CExtPanelControlBar
{
protected:
    virtual CRect OnQueryFixedBarChildWindowMargins() const
    {
        return CRect(0, 0, 0, 0);
    }
};

class CExtZoomSlider : public CExtBarSliderButton, public INonViewClient
{
public:
    CExtZoomSlider::CExtZoomSlider(CExtToolControlBar *pBar, UINT nCmdId) :
        CExtBarSliderButton(pBar, nCmdId, 0,
    		// scroll total/pos/page
            15, 3, 0,
		    // button extent horz(left/right)/vert(up/down) in pixels
		    // (if zeros - use slider-like layout instead of scrollbar-like)
		    0, 0,
		    // total slider control extent horz/vert in pixels
		    100, 100)
    {
        _pDocument = NULL;
    }
    void SetDocument(CPicDoc *pDocument)
    {
        _pDocument = pDocument;
        if (_pDocument)
        {
            _pDocument->AddNonViewClient(this);
        }
    }
    void UpdateNonView(CObject *pObject) override
    {
        PicChangeHint hint = GetHint<PicChangeHint>(pObject);
        if (IsFlagSet(hint, PicChangeHint::ZoomLevel))
        {
            ScrollPosSet(_pDocument->GetZoom());
            RedrawButton(true);
        }
    }
    ULONG ScrollPosSet(ULONG nScrollPos)
    {
        ULONG nOldPos = __super::ScrollPosSet(nScrollPos);
        _pDocument->SetZoom((int)nScrollPos);
        return nOldPos;
    }
private:
    CPicDoc *_pDocument;
};

class CPicChildFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CPicChildFrame)
public:
	CPicChildFrame();

// Attributes
public:
    void HookUpNonViews(CDocument *pDoc);

// Operations
public:

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Implementation
public:
	virtual ~CPicChildFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual MDITabType GetTabType() { return TAB_PIC; }
    void OnUpdateControl(CCmdUI *pCmdUI);
    void OnUpdateDelete(CCmdUI *pCmdUI);
    void OnUpdateAlwaysEnabled(CCmdUI *pCmdUI) { pCmdUI->Enable(); }
    void OnDelete();
    void OnShowTraceImageDropDown(NMHDR* pnmtb, LRESULT *plr);
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);

    CToolBar    m_wndToolBarMenu;
    CToolBar    m_wndToolBarDraw;
    CToolBar    m_wndToolBarColors;

    CExtPanelControlBarNoMargin m_wndContainer;
    CVPCDialogBar m_wndDialogBarColors;

    // For visual, priority, control
    CExtToolControlBar m_wndToolBarScreens;
    //CExtLabel m_wndLabelInToolbar;
    
    //CExtToolControlBar m_wndToolBarPalettes;
    //CExtLabel m_wndLabelInPalette;

    // For zoom level
    CExtToolControlBar m_wndToolBarZoom;
    CExtZoomSlider *_pZoomSliderTBB;       // Gets destroyed automatically by m_wndToolBarZoom
    CPositionSlider m_wndPositionSlider;

    PicHelperPane m_wndPicHelper;
    CExtControlBarNoX m_wndResizableBarPicHelper;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
