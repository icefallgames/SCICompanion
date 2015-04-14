// BarContainerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "BarContainerDialog.h"


// CBarContainerDialog dialog

CBarContainerDialog::CBarContainerDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CBarContainerDialog::IDD, pParent)
{
}

CBarContainerDialog::~CBarContainerDialog()
{
}

void CBarContainerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBarContainerDialog, CExtResizableDialog)
    ON_WM_SIZE()
END_MESSAGE_MAP()

// Some incredibly overzealous use of functors
template<class _T>
class ResizeWindows
{
public:
    ResizeWindows(int cx, int cy) : _cx(cx), _cy(cy) {}
    void operator()(typename _T::value_type &window)
    {
        if (window.second)
        {
            window.second->SetWindowPos(NULL, 0, 0, _cx, _cy, SWP_NOMOVE | SWP_NOZORDER);
        }
    }
private:
    int _cx, _cy;
};

template<class _T>
class HideWindowsExcept
{
public:
    HideWindowsExcept(CWnd *pWnd) : _pWndException(pWnd) {}
    void operator()(typename _T::value_type &window)
    {
        if (window.second && (window.second != _pWndException))
        {
            window.second->ShowWindow(SW_HIDE);
        }
    }
private:
    CWnd *_pWndException;
};

void CBarContainerDialog::OnSize(UINT nType, int cx, int cy)
{
    // Resize all our children to be just like us.
    for_each(_childWindows.begin(), _childWindows.end(), ResizeWindows<windowmap>(cx, cy));
}

// CBarContainerDialog message handlers
void CBarContainerDialog::SelectChild(CWnd *pWnd, int iType)
{
    ASSERT((pWnd == NULL) || (pWnd->GetParent() == static_cast<CWnd*>(this)));
    _childWindows[iType] = pWnd;
    for_each(_childWindows.begin(), _childWindows.end(), HideWindowsExcept<windowmap>(pWnd));
    if (pWnd)
    {
        CRect rc;
        GetClientRect(&rc);
        pWnd->SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
        pWnd->ShowWindow(SW_SHOW);
    }
}