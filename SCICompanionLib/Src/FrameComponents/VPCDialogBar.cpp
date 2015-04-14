// CVPCDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "VPCDialogBar.h"
#include "PicDoc.h"
#include "CObjectWrap.h"

// CVPCDialogBar
CVPCDialogBar::CVPCDialogBar(CWnd* pParent /*=NULL*/)
	: CExtDialogFwdCmd(IDD_VPC, pParent)
{
}

CVPCDialogBar::~CVPCDialogBar()
{
}


BEGIN_MESSAGE_MAP(CVPCDialogBar, CExtDialogFwdCmd)
END_MESSAGE_MAP()

void CVPCDialogBar::SetDocument(CDocument *pDoc)
{
    _pDoc = (CPicDoc*)pDoc;
    if (_pDoc)
    {
        _pDoc->AddNonViewClient(this);

        m_wndVSet.SetDialogFactory(_pDoc);
        m_wndPSet.SetDialogFactory(_pDoc);
        m_wndCSet.SetDialogFactory(_pDoc);
        m_wndPenSet.SetDialogFactory(_pDoc);
    }
}


void CVPCDialogBar::UpdateNonView(CObject *pObject)
{
    if (_pDoc)
    {
        PicDrawManager &pdm = _pDoc->GetDrawManager();
        PicChangeHint hint = GetHint<PicChangeHint>(pObject);

        if (IsFlagSet(hint, PicChangeHint::EditPicPos | PicChangeHint::NewPic | PicChangeHint::EditPicInvalid))
        {
            // Update our buttons.
            // Get the information we need from the CPicDoc, and give it to the buttons.

            const ViewPort *pstate = pdm.GetViewPort(PicPosition::PostPlugin);
            if (pdm.IsVGA())
            {
                m_wndVSet.SetColorAndState(pdm.GetVGAPalette()[pstate->bPaletteOffset], IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Visual));
            }
            else
            {
                m_wndVSet.SetColorAndState(pstate->egaColor, IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Visual));
            }
            m_wndVToggle.SetCheck(IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Visual) != 0);
            
            EGACOLOR colorP = { pstate->bPriorityValue, pstate->bPriorityValue };
            m_wndPSet.SetColorAndState(colorP, IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Priority));
            m_wndPToggle.SetCheck(IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Priority) != 0);

            EGACOLOR colorC = { pstate->bControlValue, pstate->bControlValue };
            m_wndCSet.SetColorAndState(colorC, IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Control));
            m_wndCToggle.SetCheck(IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Control) != 0);

            // If the position changed, or we have a new pic, etc...
            // then the pen could have changed.
            hint |= PicChangeHint::PenStyle;
        }

        if (IsFlagSet(hint, PicChangeHint::PenStyle))
        {
            m_wndPenSet.SetPenStyle(_pDoc->GetPenStyle());
        }

        if (IsFlagSet(hint, PicChangeHint::Palette))
        {
            const ViewPort *pstate = pdm.GetViewPort(PicPosition::PostPlugin);
            m_wndVSet.SetColorAndState(pstate->egaColor, IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Visual));
        }
    }
}

// CVPCDialogBar message handlers

void CVPCDialogBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);

	__super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	DDX_Control(pDX, IDC_TOGGLEVISUAL, m_wndVToggle);
    DDX_Control(pDX, IDC_TOGGLEPRIORITY, m_wndPToggle);
    DDX_Control(pDX, IDC_TOGGLECONTROL, m_wndCToggle);
	DDX_Control(pDX, IDC_SETVISUAL, m_wndVSet);
    DDX_Control(pDX, IDC_SETPRIORITY, m_wndPSet);
    DDX_Control(pDX, IDC_SETCONTROL, m_wndCSet);

    DDX_Control(pDX, IDC_SETPENSTYLE, m_wndPenSet);

    // Visuals
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel);
}

