#pragma once

#include "SCIPropertyCtrl.h"

class SCIClassBrowser; // fwd decl

class IPropEditCtrl
{
public:
    virtual void Init(PCTSTR pszName, SCIClassBrowser *pBrowser, CSCIPropertyBagNotify *pBag) = 0;
    virtual void BeginEdit(CRect *pRect, CWnd *pOwner) = 0;
};

//
// property editing control template
//
template <class T>
class CPropEditCtrl : public T, public IPropEditCtrl
{
public:
    CPropEditCtrl() : T()
    {
        _fEnded = FALSE;
    }

    void Init(PCTSTR pszName, SCIClassBrowser *pBrowser, CSCIPropertyBagNotify *pBag)
    {
        _strProperty = pszName;
        _pBrowser = pBrowser;
        _pBag = pBag;
    }

    void BeginEdit(CRect *pRect, CWnd *pOwner)
    {
        BOOL fSuccess = _CreateWindow(pOwner);
        if (fSuccess)
        {
            SetFont(pOwner->GetFont());
            _PositionControl(pRect);
            _PrepareControl();
            ShowWindow(SW_SHOWNOACTIVATE);
            SetFocus();
        }
        else
        {
            delete this;
        }
    }

    void EndEdit(BOOL fCommit)
    {
        if (!_fEnded) // Protect against re-entrancy
        {
            _fEnded = TRUE;
            if (fCommit)
            {
                sci::PropertyValue value;
                _GetValue(value);
                _pBag->SetProperty(_strProperty, value);
            }
            DestroyWindow();
        }
    }

protected:
    DECLARE_MESSAGE_MAP()
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        if (nChar == VK_ESCAPE)
        {
            // Cancel out.
            EndEdit(FALSE);
        }
        else if (nChar == VK_RETURN)
        {
            EndEdit(TRUE);
        }
        else
        {
            __super::OnKeyDown(nChar, nRepCnt, nFlags);
        }
    }

    BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
    {
        BOOL fDoDefaultHandling = TRUE;
        // We need to handle the ESC key
        if (message == WM_GETDLGCODE)
        {
            fDoDefaultHandling = FALSE;
            MSG *pMsg = (MSG*)lParam;
            if (pMsg && (pMsg->message == WM_KEYDOWN) && ((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN)))
            {
                *pResult = DLGC_WANTMESSAGE;
                return TRUE;
            }
        }
        return __super::OnWndMsg(message, wParam, lParam, pResult);
    }

    void OnNcDestroy()
    {
        __super::OnNcDestroy();
        delete this;
    }

    // Overridables
    virtual void _GetValue(sci::PropertyValue &value)
    {
    }
    virtual void _PrepareControl()
    {
        PopulatePropEditCtrl(_strProperty, this);
    }
    virtual BOOL _CreateWindow(CWnd *pOwner)
    {
        return FALSE;
    }
    virtual void _PositionControl(CRect *prc)
    {

    }

    SCIClassBrowser *_pBrowser;
    CSCIPropertyBagNotify *_pBag;
    CString _strProperty;
    BOOL _fEnded;
};

BEGIN_TEMPLATE_MESSAGE_MAP(CPropEditCtrl, T ,T)
    ON_WM_KEYDOWN()
    ON_WM_NCDESTROY()
END_MESSAGE_MAP()




