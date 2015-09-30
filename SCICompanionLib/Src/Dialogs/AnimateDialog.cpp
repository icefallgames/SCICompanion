// AnimateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "AnimateDialog.h"
#include "Components.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"
using namespace std;

const int MaxBackground = 4;
int g_currentBackground = 0;

// CAnimateDialog dialog

CAnimateDialog::CAnimateDialog(CWnd* pParent /*=NULL*/, const ResourceEntity *resource, const PaletteComponent *palette)
    : CExtResizableDialog(CAnimateDialog::IDD, pParent), _resource(resource), _palette(palette)
{
    _nCel = 0;

    _rectDialogSize.SetRectEmpty();
    _fInitialized = false;
    _fAnimating = true;
}

//
// Called once at the beginning (or later, if the view data changes)
//
void CAnimateDialog::_AutoSize()
{
    CSize sizeMax = m_wndAnimate.GetSizeNeeded();

    CRect rect;
    m_wndAnimate.GetClientRect(&rect);
    int dx = sizeMax.cx - rect.Width();
    int dy = sizeMax.cy - rect.Height();

    if ((dx > 0) || (dy > 0))
    {
        CRect rectDialog;
        GetClientRect(&rectDialog);
        if (dx > 0)
        {
            rectDialog.right += dx;
        }
        if (dy > 0)
        {
            rectDialog.left += dy;
        }
        MoveWindow(&rectDialog);
    }
}

void CAnimateDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_ANIMATE, m_wndAnimate);
    DDX_Control(pDX, IDC_SLIDER, m_wndSlider);
    DDX_Control(pDX, IDC_BUTTONPLAY, m_wndButton);
    DDX_Control(pDX, IDC_BUTTONBG, m_wndButtonBG);
    
    m_wndAnimate.SetResource(_resource, _palette);
    m_wndAnimate.SetLoop(_nLoop);
    _UpdateBackground();

    GetClientRect(&_rectDialogSize);

    _AutoSize();

    SetWindowText(_strTitle);
    SetTimer(ANIMATE_TIMER, 200, NULL);

    _fInitialized = true;

    m_wndSlider.SetRange(0, _resource->GetComponent<RasterComponent>().Loops[_nLoop].Cels.size() - 1);
    _UpdateButton();

    DDX_Control(pDX, IDOK, m_wndOK);
}

void CAnimateDialog::_UpdateButton()
{
    // Update command ui doesn't work here, so set the text directly.
    m_wndButton.SetWindowText(_fAnimating ? TEXT("Pause") : TEXT("Play"));
}

void CAnimateDialog::OnPlay()
{
    _fAnimating = !_fAnimating;
    _UpdateButton();
}


BEGIN_MESSAGE_MAP(CAnimateDialog, CExtResizableDialog)
    ON_WM_SIZE()
    ON_WM_DRAWITEM()
    ON_WM_TIMER()
    ON_WM_HSCROLL()
    ON_COMMAND(IDC_BUTTONPLAY, OnPlay)
    ON_BN_CLICKED(IDC_BUTTONBG, &CAnimateDialog::OnBnClickedButtonbg)
END_MESSAGE_MAP()

void CAnimateDialog::OnSize(UINT nType, int cx, int cy)
{
    CRect rectDialogSize;
    GetClientRect(&rectDialogSize);

    if (_fInitialized)
    {
        int dx = rectDialogSize.Width() - _rectDialogSize.Width();
        int dy = rectDialogSize.Height() - _rectDialogSize.Height();

        if (dx || dy)
        {
            CRect rectAnimateScreen;
            m_wndAnimate.GetWindowRect(&rectAnimateScreen);
            ScreenToClient(&rectAnimateScreen);
            rectAnimateScreen.right = rectAnimateScreen.left + rectAnimateScreen.Width() + dx;
            rectAnimateScreen.bottom = rectAnimateScreen.top + rectAnimateScreen.Height() + dy;
            m_wndAnimate.MoveWindow(&rectAnimateScreen, TRUE);

            int rgid[] = { IDOK, IDC_BUTTONPLAY };
            for (int i = 0; i < ARRAYSIZE(rgid); i++)
            {
                CWnd *pOk = GetDlgItem(rgid[i]);
                if (pOk)
                {
                    CRect rectOkScreen;
                    pOk->GetWindowRect(&rectOkScreen);
                    ScreenToClient(&rectOkScreen);
                    rectOkScreen.OffsetRect(dx, dy);
                    pOk->MoveWindow(&rectOkScreen, TRUE);
                }
            }

            CWnd *pSlider = GetDlgItem(IDC_SLIDER);
            if (pSlider)
            {
                CRect rectScreen;
                pSlider->GetWindowRect(&rectScreen);
                ScreenToClient(&rectScreen);
                rectScreen.OffsetRect(0, dy);
                pSlider->SetWindowPos(NULL, rectScreen.left, rectScreen.top, rectScreen.Width() + dx, rectScreen.Height(), SWP_NOZORDER);
            }

            m_wndAnimate.Invalidate(FALSE);
        }
    }
    _rectDialogSize = rectDialogSize;

    __super::OnSize(nType, cx, cy);
}

void CAnimateDialog::SetLoop(int nLoop)
{
    _nLoop = nLoop;
}

void CAnimateDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == ANIMATE_TIMER)
    {
        const RasterComponent &raster = _resource->GetComponent<RasterComponent>();
        if ((raster.Loops.size() > 0) && _fAnimating)
        {
            int cCels = (int)raster.Loops[_nLoop].Cels.size();
            _nCel++;
            _nCel %= cCels;
            m_wndSlider.SetPos(_nCel);
            m_wndAnimate.SetCel(_nCel);
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}


void CAnimateDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd)
{
    switch (nSBCode)
    {
    case SB_THUMBTRACK:
        _nCel = nPos;
        _fAnimating = false;
        _UpdateButton();
        m_wndAnimate.SetCel(_nCel);
        break;

    case SB_LEFT:
    case SB_ENDSCROLL:
    case SB_LINELEFT:
    case SB_LINERIGHT:
    case SB_PAGELEFT:
    case SB_PAGERIGHT:
    case SB_RIGHT:
        _nCel = m_wndSlider.GetPos();
        _fAnimating = false;
        _UpdateButton();
        m_wndAnimate.SetCel(_nCel);
        break;

    case SB_THUMBPOSITION:
        // We update constantly, so no need to do anything here.
        break;
    }
}

void CAnimateDialog::OnBnClickedButtonbg()
{
    g_currentBackground++;
    g_currentBackground %= MaxBackground;
    _UpdateBackground();
}

void CAnimateDialog::_UpdateBackground()
{
    if (g_currentBackground == 0)
    {
        m_wndAnimate.SetPatterned(true);
    }
    else
    {
        m_wndAnimate.SetPatterned(false);
        COLORREF color = RGB(0, 0, 0);
        switch (g_currentBackground)
        {
            case 1:
                color = RGB(0, 0, 0);
                break;
            case 2:
                color = RGB(255, 0, 255);
                break;
            case 3:
                color = RGB(255, 255, 255);
                break;
        }
        m_wndAnimate.SetBackground(color);
    }
}
