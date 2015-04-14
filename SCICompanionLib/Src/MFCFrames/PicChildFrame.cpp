// PicChildFrame.cpp : implementation of the CPicChildFrame class
//
#include "stdafx.h"
#include "PicChildFrame.h"
#include "resource.h"
#include "PicView.h"
#include "ChooseColorDialog.h"
#include "MainFrm.h"
#include "AppState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPicChildFrame

IMPLEMENT_DYNCREATE(CPicChildFrame, CMDITabChildWnd)

BEGIN_MESSAGE_MAP(CPicChildFrame, CMDITabChildWnd)
    ON_WM_CREATE()
    ON_COMMAND(ID_EDIT_DELETE, OnDelete)
    ON_UPDATE_COMMAND_UI(IDC_SETVISUAL, OnUpdateControl)
    ON_UPDATE_COMMAND_UI(IDC_SETCONTROL, OnUpdateControl)
    ON_UPDATE_COMMAND_UI(IDC_SETPRIORITY, OnUpdateControl)
    ON_UPDATE_COMMAND_UI(IDC_SETPENSTYLE, OnUpdateControl)
    ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateDelete)
    ON_WM_HSCROLL()

    // REVIEW: I really need to put these toolbars somewhere else, but
    // for now, I'll need to provide this way of getting them back, in
    // case they disappear.
    ON_COMMAND_EX(IDR_TOOLBARSCREENS, OnBarCheck)
    ON_UPDATE_COMMAND_UI(IDR_TOOLBARSCREENS, OnUpdateControlBarMenu)
    ON_COMMAND_EX(IDR_TOOLBARPALETTES, OnBarCheck)
    ON_UPDATE_COMMAND_UI(IDR_TOOLBARPALETTES, OnUpdateControlBarMenu)
    ON_COMMAND_EX(IDR_TOOLBAR_PICSLIDER, OnBarCheck)
    ON_UPDATE_COMMAND_UI(IDR_TOOLBAR_PICSLIDER, OnUpdateControlBarMenu)

    ON_COMMAND_EX(ID_BAR_PICHELPER, OnBarCheck)
    ON_UPDATE_COMMAND_UI(ID_BAR_PICHELPER, OnUpdateControlBarMenu)
END_MESSAGE_MAP()

// CChildFrame construction/destruction

CPicChildFrame::CPicChildFrame()
{
    _pZoomSliderTBB = NULL;
}

CPicChildFrame::~CPicChildFrame()
{
}


void CPicChildFrame::OnUpdateControl(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CPicChildFrame::OnDelete()
{
    //_pCommandView->OnDeleteCommands();
}

void CPicChildFrame::OnUpdateDelete(CCmdUI *pCmdUI)
{
    //_pCommandView->OnUpdateDelete(pCmdUI);
}

// Hack for position slider, since MFC seems to have trouble if the slider is a direct child of
// the frame - it fwds all scroll messages to the view, and ignores reflection.
void CPicChildFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    if (pScrollBar && pScrollBar->GetSafeHwnd() == m_wndPositionSlider.GetSafeHwnd())
    {
        m_wndPositionSlider.HScroll(nSBCode, nPos);
    }
    __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

const struct
{
    int nID;
    PCTSTR pszLabel;
}
c_toolbarIDToLabel[] = 
{
    { ID_PENTOOL,   TEXT("Pen") },
    { ID_LINE,      TEXT("Line") },
    { ID_FILL,      TEXT("Fill") },
    { ID_CIRCLE,    TEXT("Circle") },
    { ID_DRAWOFF,   TEXT("Off") },
};

void _AssignToolBarText(CToolBar *pToolBar)
{
    for (int i = 0; i < ARRAYSIZE(c_toolbarIDToLabel); i++)
    {
        int iIndex = pToolBar->CommandToIndex(c_toolbarIDToLabel[i].nID);
        pToolBar->SetButtonText(iIndex, c_toolbarIDToLabel[i].pszLabel);
        pToolBar->SetButtonStyle(iIndex, TBSTYLE_AUTOSIZE);

    }
}

void CPicChildFrame::HookUpNonViews(CDocument *pDoc)
{
    _pZoomSliderTBB->SetDocument(static_cast<CPicDoc*>(pDoc));
    m_wndPositionSlider.SetDocument(pDoc);
    m_wndDialogBarColors.SetDocument(pDoc);
    m_wndPicHelper.SetDocument(pDoc);
}

#define CENTERLABEL_FUDGE 8

int CPicChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;


    // the pic helper dialog
    m_wndResizableBarPicHelper.SetInitDesiredSizeVertical(CSize(240, 100));
    if (!m_wndResizableBarPicHelper.Create("Helper", this, ID_BAR_PICHELPER))
    {
        TRACE0("Failed to create m_wndResizableBarPicHelper\n");
        return -1;		// fail to create
    }
    m_wndResizableBarPicHelper.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    if (!m_wndPicHelper.Create(PicHelperPane::IDD, &m_wndResizableBarPicHelper))
    //if (!m_wndPicHelper.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, CRect(0, 0, 0, 0),
      //  &m_wndResizableBarPicHelper, ID_CONTROL_OUTPUTPANE))
    {
        TRACE0("Failed to create empty dialog\n");
        return -1;		// fail to create
    }

    // The VPC dialog bar
    if (!m_wndContainer.Create("Toolbox", this, ID_BAR_VPC))
    {
	    TRACE0("Failed to create container for VPC bar\n");
	    return -1;		// fail to create
    }
    if (!m_wndDialogBarColors.Create(MAKEINTRESOURCE(IDD_VPC), &m_wndContainer))
    {
	    TRACE0("Failed to create VPC bar\n");
	    return -1;		// fail to create
    }

    //
    // The screens toolbar
    //
    if(!m_wndToolBarScreens.Create("Screen", this, AFX_IDW_TOOLBAR))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }

    // The 3 buttons
    m_wndToolBarScreens.InsertButton(-1, ID_SHOWVISUALSCREEN);
    m_wndToolBarScreens.InsertButton(-1, ID_SHOWPRIORITYSCREEN);
    m_wndToolBarScreens.InsertButton(-1, ID_SHOWCONTROLSCREEN);

    
	if(!m_wndToolBarZoom.Create("Zoom Slider", this, IDR_TOOLBAR_PICSLIDER))
	{
		TRACE0("Failed to create toolbar m_wndToolBarZoom \n");
		return -1;      // fail to create
	}
	_pZoomSliderTBB = new CExtZoomSlider(&m_wndToolBarZoom, ID_ZOOMSLIDER);
	VERIFY(m_wndToolBarZoom.InsertSpecButton(-1, _pZoomSliderTBB, FALSE));


    m_wndContainer.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarScreens.EnableDocking(CBRS_ALIGN_TOP);
	//m_wndToolBarPalettes.EnableDocking(CBRS_ALIGN_TOP);
	m_wndToolBarZoom.EnableDocking(CBRS_ALIGN_TOP);
    m_wndResizableBarPicHelper.EnableDocking(CBRS_ALIGN_RIGHT); // make this up

    if (!CExtControlBar::FrameEnableDocking(this))
    {
        ASSERT( FALSE );
        return -1;
    }

    m_wndResizableBarPicHelper.DockControlBarInnerOuter(AFX_IDW_DOCKBAR_RIGHT, true, this, true);

    // The position slider.
    if (!m_wndPositionSlider.Create(WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_BOTTOM, CRect(0, 0, 0, 0), this, AFX_IDW_DIALOGBAR))
    {
        TRACE0("Failed to create position slider\n");
        return -1;
    }
    m_wndPositionSlider.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);


    // The VPC bar
    DockControlBar(&m_wndContainer);
    RecalcLayout();
    CRect rcAlreadyDockedBar;
    m_wndContainer.GetWindowRect(&rcAlreadyDockedBar);
    rcAlreadyDockedBar.OffsetRect(1, 0);
    // Followed by screens
    DockControlBar(&m_wndToolBarScreens, AFX_IDW_DOCKBAR_TOP, &rcAlreadyDockedBar);
    // Followed by palettess
    RecalcLayout();
    m_wndToolBarScreens.GetWindowRect(&rcAlreadyDockedBar);
    rcAlreadyDockedBar.OffsetRect(1, 0);
    //DockControlBar(&m_wndToolBarPalettes, AFX_IDW_DOCKBAR_TOP, &rcAlreadyDockedBar);
    // Followed by zoom
    //RecalcLayout();
    //m_wndToolBarPalettes.GetWindowRect(&rcAlreadyDockedBar);
    //rcAlreadyDockedBar.OffsetRect(1, 0);
    DockControlBar(&m_wndToolBarZoom, AFX_IDW_DOCKBAR_TOP, &rcAlreadyDockedBar);

    DockControlBar(&m_wndResizableBarPicHelper, AFX_IDW_DOCKBAR_RIGHT, &rcAlreadyDockedBar);
    

    return 0;
}


BOOL CPicChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !__super::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame diagnostics

#ifdef _DEBUG
void CPicChildFrame::AssertValid() const
{
	__super::AssertValid();
}

void CPicChildFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers
