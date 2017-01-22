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

#include "stdafx.h"
#include "PropEditCtrl.h"
#include "SCIPropertyInfo.h"
#include "SCIProps.h"
#include "AppState.h"
#include "ScriptOM.h"
#include "ClassBrowser.h"

using namespace sci;

#define SCIDESC_NAME TEXT("Name of the object used in code.")
#define SCIDESC_SPECIES TEXT("Type of the object.")
#define SCIDESC_VIEW TEXT("Number of the View resource.")
#define SCIDESC_X TEXT("X coordinate of the object.")
#define SCIDESC_Y TEXT("Y coordinate of the object.")
#define SCIDESC_LOOP TEXT("View loop used for the object.")
#define SCIDESC_CEL TEXT("Loop cel used for the object.")
#define SCIDESC_Z TEXT("The height of the object above the ground.  Unlike the y coordinate, this does not affect priority.")
#define SCIDESC_PRIORITY TEXT("Determines whether an object appears in front of or behind objects.  Larger numbers are to the front.")
#define SCIDESC_IGNOREACTORS TEXT("Determines whether an object will bump into other objects or not.")
#define SCIDESC_ADDTOPIC TEXT("Lets a snapshot of the object be added permanently to the background.")
#define SCIDESC_OBSERVECONTROL TEXT("na")
#define SCIDESC_CYCLESPEED TEXT("The speed at which an object animates.  Lower numbers are faster.")
#define SCIDESC_DOORBLOCK TEXT("The control colour used to represent the line which an actor can't cross when the door is closed.")
#define SCIDESC_DOORCTRL TEXT("The control colour from where an actor and open or close a door.")
#define SCIDESC_ROOMCTRL TEXT("The control colour next to a door, from where an actor will go to a new room.")
#define SCIDESC_ENTRANCETO TEXT("The room number this door leads to.")
#define SCIDESC_MOVESPEED TEXT("The speed at which an actor moves.  Lower numbers are faster.")
#define SCIDESC_XSTEP TEXT("The amount an actor moves in the x direction in each cycle.  Together with moveSpeed, this controls the perceived speed of an actor.")
#define SCIDESC_YSTEP TEXT("The amount an actor moves in the y direction in each cycle.  Together with moveSpeed, this controls the perceived speed of an actor.")

//
// Autocomplete string enumerator for property values.
//
class CDefinesEnumString : public IEnumString
{
public:
    CDefinesEnumString(const Script *pScript, SCIClassBrowser *pBrowser)
    {
        _cRef = 1;

        // Get script defines
        const DefineVector &defines = pScript->GetDefines();
        for (DefineVector::const_iterator defineIt = defines.begin(); (defineIt != defines.end()); defineIt++)
        {
            _defines.Add((*defineIt)->GetLabel().c_str());
        }

        // Global ones
        const std::vector<sci::Script*> &headers = pBrowser->GetHeaders();
        for (size_t iHeader = 0; iHeader < headers.size(); iHeader++)
        {
            const Script *pHeader = headers[iHeader];
            const DefineVector &defines = pHeader->GetDefines();
            for (DefineVector::const_iterator defineIt = defines.begin(); (defineIt != defines.end()); defineIt++)
            {
                _defines.Add((*defineIt)->GetLabel().c_str());
            }
        }
    }
    ~CDefinesEnumString();

    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // IEnumString
    HRESULT STDMETHODCALLTYPE Next(ULONG celt, LPOLESTR *rgelt, ULONG *pceltFetched);
    HRESULT STDMETHODCALLTYPE Skip(ULONG celt);
    HRESULT STDMETHODCALLTYPE Reset();
    HRESULT STDMETHODCALLTYPE Clone(IEnumString **ppenum);

    // CDefinesEnumString
    HRESULT Init();

private:
    LONG _cRef;

    INT_PTR _iIndex;
    CStringArray _defines;
};


CDefinesEnumString::~CDefinesEnumString()
{
}

ULONG CDefinesEnumString::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG CDefinesEnumString::Release()
{
    LONG cRef = InterlockedDecrement(&_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

HRESULT CDefinesEnumString::QueryInterface(REFIID riid, void **ppv)
{
    *ppv = nullptr;
    HRESULT hr = E_NOINTERFACE;
    if (IsEqualIID(IID_IUnknown, riid))
    {
        *ppv = (IUnknown*)this;
        hr = S_OK;
    }
    else if (IsEqualIID(IID_IEnumString, riid))
    {
        *ppv = (IEnumString*)this;
        hr = S_OK;
    }

    if (SUCCEEDED(hr))
    {
        AddRef();
    }

    return hr;
}

HRESULT CDefinesEnumString::Next(ULONG celt, LPOLESTR *rgelt, ULONG *pceltFetched)
{
    HRESULT hr = S_FALSE;
    *pceltFetched = 0;
    while ((celt > 0) && SUCCEEDED(hr) && (_iIndex <= _defines.GetUpperBound()))
    {
        CString &strWord = _defines[_iIndex];
        int cch = (int)(strlen(strWord) + 1);
        rgelt[*pceltFetched] = (LPOLESTR)CoTaskMemAlloc(cch * sizeof(WCHAR));
        hr = rgelt[*pceltFetched] ? S_OK : E_OUTOFMEMORY;
        if (SUCCEEDED(hr))
        {
            MultiByteToWideChar(CP_ACP, 0, strWord, -1, rgelt[*pceltFetched], cch);
            *pceltFetched++;
            celt--;
        }
        _iIndex++;
    }
    return hr;
}

HRESULT CDefinesEnumString::Skip(ULONG celt)
{
    _iIndex += celt;
    return S_OK;
}

HRESULT CDefinesEnumString::Reset()
{
    _iIndex = 0;
    return S_OK;
}

HRESULT CDefinesEnumString_CreateInstace(REFIID riid, void **ppv, const Script *pScript, SCIClassBrowser *pBrowser)
{
    *ppv = nullptr;
    HRESULT hr = E_OUTOFMEMORY;
    CDefinesEnumString *pEnum = new CDefinesEnumString(pScript, pBrowser);
    if (pEnum)
    {
        hr = pEnum->QueryInterface(riid, ppv);
        pEnum->Release();
    }
    return hr;
}


HRESULT CDefinesEnumString::Clone(IEnumString **ppenum)
{
    return E_NOTIMPL;
}






//
// CPropEditCtrlCombo
// - Combobox for editing properties.
//
class CPropEditCtrlCombo : public CPropEditCtrl<CComboBox>
{
public:
    CPropEditCtrlCombo(BOOL fEditable = FALSE) : CPropEditCtrl<CComboBox>()
    {
        _fEditable = fEditable;
    }

protected:
    DECLARE_MESSAGE_MAP()
    // Specific to particular control
    void OnCBNKillFocus()
    {
        EndEdit(TRUE);
    }

    virtual void _GetValue(PropertyValue &value)
    {
        CString strDisplay;
        GetWindowText(strDisplay);
        DisplayToValue(_strProperty, strDisplay, value);
    }

    virtual void _PrepareControl()
    {
        __super::_PrepareControl();

        // Now look at our value, and find the matching entry and select it.
        PropertyValue value;
        if (_pBag->GetProperty(_strProperty, value))
        {
            int iIndex = FindStringExact(0, ValueToDisplay(_strProperty, value));
            ASSERT((iIndex >= 0) || _fEditable); // We should never get here in a non-editable, unless the Tx functions or populator is wrong
            if (iIndex >= 0)
            {
                SetCurSel(iIndex);
            }
        }
    }

    virtual void _PositionControl(CRect *pRect)
    {
        // Pad a bit.
        pRect->top -= 2;
        pRect->bottom += 1;
        // Adjust to look better.
        pRect->left -= 2;
        SetWindowPos(nullptr, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOZORDER | SWP_NOACTIVATE);
        CRect rectActual;
        GetWindowRect(&rectActual);
        // Reposition a bit based on actual height (e.g. for comboboxes, which determine their own height)
        int cyDiff = rectActual.Height() - pRect->Height();
        pRect->top -= cyDiff / 2;
        pRect->bottom += cyDiff / 2;
        SetWindowPos(nullptr, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOZORDER | SWP_NOACTIVATE);
    }

    virtual BOOL _CreateWindow(CWnd *pOwner)
    {
        DWORD dwDropDownStyle = _fEditable ? CBS_DROPDOWN : CBS_DROPDOWNLIST;
        return CreateEx(0, WC_COMBOBOX, TEXT("SCI inplace edit"), dwDropDownStyle | WS_CHILD, CRect(0, 0, 0, 0), pOwner, 0);
    }

    BOOL _fEditable;
};

BEGIN_MESSAGE_MAP(CPropEditCtrlCombo, CPropEditCtrl<CComboBox>)
    ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnCBNKillFocus)
END_MESSAGE_MAP()


//
// Helper class for spin buttons, used with CPropEditCtrlEditBox
//
class CPropEditCtrlEditBox; // fwd decl
class CPropEditSpinCtrl : public CSpinButtonCtrl
{
protected:
    DECLARE_MESSAGE_MAP()

    void OnUdnDeltaPos(NMHDR *pnmhdr, LRESULT *plres);
};
BEGIN_MESSAGE_MAP(CPropEditSpinCtrl, CSpinButtonCtrl)
    ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnUdnDeltaPos)
END_MESSAGE_MAP()

//
// CPropEditCtrlEditBox
// - Editbox for editing properties.
//
#define SPINNER_WIDTH 15
class CPropEditCtrlEditBox : public CPropEditCtrl<CEdit>
{
public:
    CPropEditCtrlEditBox(BOOL fSpin) : CPropEditCtrl<CEdit>()
    {
        // A little pizazz.
        _hEditBrush.CreateSolidBrush(RGB(255, 255, 192));
        //_fSpin = fSpin;
        _fSpin = fSpin;
    }


protected:
    DECLARE_MESSAGE_MAP()
    // Specific to particular control
    void OnKillFocus(CWnd *pWndNew)
    {
        __super::OnKillFocus(pWndNew);
        EndEdit(TRUE);
    }

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        if (nChar == VK_ESCAPE)
        {
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

    HBRUSH CtlColor(CDC* pDC, UINT nCtlColor) 
    {
        return (HBRUSH)_hEditBrush;
    }

    virtual void _GetValue(PropertyValue &value)
    {
        CString strDisplay;
        GetWindowText(strDisplay);
        DisplayToValue(_strProperty, strDisplay, value);
    }

    virtual void _PrepareControl()
    {
        __super::_PrepareControl();
        PropertyValue value;
        if (_pBag->GetProperty(_strProperty, value))
        {
            std::string strValue = value.ToString();
            SetWindowText(strValue.c_str());
            SetSel(0, -1);
            if (_fSpin)
            {
                m_wndSpin.SetPos(StrToInt(strValue.c_str()));
            }
        }
    }

    virtual void _PositionControl(CRect *pRect)
    {
        // Pad a bit.
        pRect->top -= 2;
        pRect->bottom += 1;
        // Adjust to look better.
        pRect->left -= 3;
        if (_fSpin)
        {
            pRect->right -= SPINNER_WIDTH;
        }
        SetWindowPos(nullptr, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOZORDER | SWP_NOACTIVATE);

        // Also set up the spinner:
        if (_fSpin)
        {
            m_wndSpin.SetWindowPos(nullptr, pRect->right, pRect->top, 10, pRect->Height(), SWP_NOZORDER);
            // Show it now, since the superclass doesn't know about it.
            m_wndSpin.ShowWindow(SW_SHOWNOACTIVATE);
        }
    }

    virtual BOOL _CreateWindow(CWnd *pOwner)
    {
        // NOTE: MFC's CDialog::PreTranslateMessage has a bug in it if you're handling VK_ESCAPE in an ES_MULTILINE editbox,
        // so don't change this to ES_MULTILINE.
        BOOL fRet = CreateEx(0, WC_EDIT, TEXT("SCI inplace edit"), WS_TABSTOP | WS_BORDER | WS_CHILD | ES_WANTRETURN, CRect(0, 0, 0, 0), pOwner, 0);
        if (fRet && _fSpin)
        {
            fRet = m_wndSpin.Create(WS_CHILD, CRect(0, 0, 0, 0), pOwner, 0);
            if (fRet)
            {
                m_wndSpin.SetBuddy(this);
            }
        }
        return fRet;
    }

public:
    static void s_SetupDefinesAutoComplete(CWnd *pWnd)
    {
        // Set up an autocomplete of all defines for this edit bot.
        CPropEditCtrlEditBox *pEdit = static_cast<CPropEditCtrlEditBox*>(pWnd);
        if (!pEdit->_fSpin)
        {
            IAutoComplete *pac;
            if (SUCCEEDED(CoCreateInstance(CLSID_AutoComplete, nullptr, CLSCTX_INPROC_SERVER, IID_IAutoComplete, (void**)&pac)))
            {
                IUnknown *pSource;
                if (SUCCEEDED(CDefinesEnumString_CreateInstace(IID_IUnknown, (void**)&pSource, pEdit->_pBag->GetClass()->GetOwnerScript(), pEdit->_pBrowser)))
                {
                    if (SUCCEEDED(pac->Init(pEdit->GetSafeHwnd(), pSource, nullptr, nullptr)))
                    {
                        IAutoComplete2 *pAC2;
                        if (SUCCEEDED(pac->QueryInterface(IID_IAutoComplete2, (void**)&pAC2)))
                        {
                            //if (appState->_fUseAutoSuggest)
                            //{
                                pAC2->SetOptions(ACO_AUTOSUGGEST);
                            //}
                            pAC2->Release();
                        }
                    }
                    pSource->Release();
                }
                pac->Release();
            }
        }
    }

    // Sets an int value, AND commits...
    void SetIntAndUpdate(int iValue)
    {
        ASSERT(_fSpin);
        TCHAR szNum[10];
        StringCchPrintf(szNum, ARRAYSIZE(szNum), TEXT("%d"), iValue);
        SetWindowText(szNum);

        // And now commit it
        PropertyValue value;
        _GetValue(value);
        _pBag->SetProperty(_strProperty, value);
    }

private:
    CBrush _hEditBrush;
    CPropEditSpinCtrl m_wndSpin;
    BOOL _fSpin;
};

BEGIN_MESSAGE_MAP(CPropEditCtrlEditBox, CPropEditCtrl<CEdit>)
    ON_WM_KILLFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void CPropEditSpinCtrl::OnUdnDeltaPos(NMHDR *pnmhdr, LRESULT *plres)
{
    *plres = 1;
    NMUPDOWN *pud = (NMUPDOWN*)pnmhdr;
    CPropEditCtrlEditBox *pEdit = static_cast<CPropEditCtrlEditBox*>(GetBuddy());
    if (pEdit)
    {
        if ((pud->iPos + pud->iDelta) >= 0)
        {
            *plres = 0; // 0 = accept
            pEdit->SetIntAndUpdate(pud->iPos + pud->iDelta);
        }
    }
}

//
// Generic helper
//
void _GeneralDisplayToValue(PCTSTR pszText, PropertyValue &value)
{
    // Set the property.  Try to set it as a number
    bool fNumber = false;
    bool fHex = false;
    WORD w;
    if (pszText[0] == TEXT('$'))
    {
        // Hex
        w = _HexToWord(pszText);
        fNumber = true;
        fHex = true;
    }
    else if (isdigit(pszText[0]) || (pszText[0] == TEXT('-')))
    {
        w = (WORD)StrToInt(pszText);
        fNumber = true;
    }

    if (fNumber)
    {
        value.SetValue(w, fHex);
    }
    else
    {
        value.SetStringValue(pszText);
    }
}


//
// Populators and transforms
//
void _PopulatePriorityCombo(CWnd *pWnd)
{
    CComboBox *pCombo = static_cast<CComboBox*>(pWnd);
    pCombo->AddString(TEXT("default"));
    for (int i = 0; i < 16; i++)
    {
        TCHAR szNum[10];
        StringCchPrintf(szNum, ARRAYSIZE(szNum), TEXT("%d"), i);
        pCombo->AddString(szNum);
    }
}

void _PriorityDisplayToValue(const CString strDisplay, PropertyValue &value)
{
    if (strDisplay == TEXT("default"))
    {
        value.SetValue((WORD)-1);
    }
    else
    {
        _GeneralDisplayToValue(strDisplay, value);
    }
}


void _PriorityValueToDisplay(const PropertyValue &value, CString &strDisplay)
{
    if (value.GetNumberValue() == (WORD)-1)
    {
        strDisplay = TEXT("default");
    }
    else
    {
        strDisplay = value.ToString().c_str();
    }
}

void _PopulateSpeciesCombo(CWnd *pWnd)
{
    CComboBox *pCombo = static_cast<CComboBox*>(pWnd);
    // We need to find all species that are subclasses of View.
	SCIClassBrowser &browser = appState->GetClassBrowser();
    std::unique_ptr<std::vector<std::string> > pArray = browser.CreateSubSpeciesArray("View");
    for (size_t i = 0; i < pArray->size(); i++)
    {
        pCombo->AddString((*pArray)[i].c_str());
    }
}

void _PopulateControlColourCombo(CWnd *pWnd)
{
    CComboBox *pCombo = static_cast<CComboBox*>(pWnd);
    pCombo->AddString(TEXT("ctlBLACK"));
    pCombo->AddString(TEXT("ctlBLUE"));
    pCombo->AddString(TEXT("ctlBROWN"));
    pCombo->AddString(TEXT("ctlCYAN"));
    pCombo->AddString(TEXT("ctlFUCHSIA"));
    pCombo->AddString(TEXT("ctlGREEN"));
    pCombo->AddString(TEXT("ctlLIME"));
    pCombo->AddString(TEXT("ctlMAROON"));
    pCombo->AddString(TEXT("ctlNAVY"));
    pCombo->AddString(TEXT("ctlPURPLE"));
    pCombo->AddString(TEXT("ctlRED"));
    pCombo->AddString(TEXT("ctlSILVER"));
    pCombo->AddString(TEXT("ctlTEAL"));
    pCombo->AddString(TEXT("ctlWHITE"));
    pCombo->AddString(TEXT("ctlYELLOW"));
}

void _PopulateBooleanCombo(CWnd *pWnd)
{
    CComboBox *pCombo = static_cast<CComboBox*>(pWnd);
    pCombo->AddString(TEXT("true"));
    pCombo->AddString(TEXT("false"));
}

// ARCHITECTURE: we're going to want to make a distinction between hard and soft property sets.
// hard would be "i want to set this to the new value"
// soft would be "i want to set this, but if the end result (e.g. TRUE vs 1) is the same, then don't bother"
// Right now, we'll be losing that distinction.
void _BooleanDisplayToValue(const CString strDisplay, PropertyValue &value)
{
    if (strDisplay == TEXT("true"))
    {
        value.SetValue(1);
    }
    else if (strDisplay == TEXT("false"))
    {
        value.SetValue(0);
    }
    else
    {
        _GeneralDisplayToValue(strDisplay, value);
    }
}


void _BooleanValueToDisplay(const PropertyValue &value, CString &strDisplay)
{
    if (value.GetNumberValue() == 0)
    {
        strDisplay = TEXT("false");
    }
    else
    {
        strDisplay = TEXT("true");
    }
}



//
// Types of property controls.
//
enum PROPEDITCTRL_TYPE
{
    PEC_COMBO,
    PEC_EDIT,
    PEC_EDITSPIN,
    PEC_EDITCOMBO,
};


typedef struct
{
    PCTSTR pszName;
    PFNPROPERTYPOPULATOR pPopulator; // adds autocomp, or items in combobox
    PFNPROPERTYDISPLAYTOVALUE pDisplayToValue;
    PFNPROPERTYVALUETODISPLAY pValueToDisplay;
    PROPEDITCTRL_TYPE editType;
    PCTSTR pszDesc;
} SCIPROPINFO;

const SCIPROPINFO c_rgPropInfo[] =
{
    { SCIPROP_NAME, nullptr, nullptr, nullptr, PEC_EDIT, SCIDESC_NAME },
    { SCIPROP_SPECIES, _PopulateSpeciesCombo, nullptr, nullptr, PEC_COMBO, SCIDESC_SPECIES },
    { SCIPROP_VIEW, CPropEditCtrlEditBox::s_SetupDefinesAutoComplete, nullptr, nullptr, PEC_EDIT, SCIDESC_VIEW },
    { SCIPROP_X, nullptr, nullptr, nullptr, PEC_EDIT, SCIDESC_X },
    { SCIPROP_Y, nullptr, nullptr, nullptr, PEC_EDIT, SCIDESC_Y },
    { SCIPROP_LOOP, nullptr, nullptr, nullptr, PEC_EDIT, SCIDESC_LOOP },
    { SCIPROP_CEL, nullptr, nullptr, nullptr, PEC_EDIT, SCIDESC_CEL },
    { SCIPROP_Z, nullptr, nullptr, nullptr, PEC_EDIT, SCIDESC_Z },
    { SCIPROP_PRIORITY, _PopulatePriorityCombo, _PriorityDisplayToValue, _PriorityValueToDisplay, PEC_COMBO, SCIDESC_PRIORITY },
    { SCIPROP_IGNOREACTORS, _PopulateBooleanCombo, _BooleanDisplayToValue, _BooleanValueToDisplay, PEC_COMBO, SCIDESC_IGNOREACTORS },
    { SCIPROP_ADDTOPIC, _PopulateBooleanCombo, _BooleanDisplayToValue, _BooleanValueToDisplay, PEC_COMBO, SCIDESC_ADDTOPIC },
    { SCIPROP_OBSERVECONTROL, _PopulateBooleanCombo, _BooleanDisplayToValue, _BooleanValueToDisplay, PEC_COMBO, SCIDESC_OBSERVECONTROL },
    { SCIPROP_CYCLESPEED, nullptr, nullptr, nullptr, PEC_EDITSPIN, SCIDESC_CYCLESPEED },
    { SCIPROP_DOORBLOCK, _PopulateControlColourCombo, nullptr, nullptr, PEC_EDITCOMBO, SCIDESC_DOORBLOCK },
    { SCIPROP_DOORCTRL, _PopulateControlColourCombo, nullptr, nullptr, PEC_EDITCOMBO, SCIDESC_DOORCTRL },
    { SCIPROP_ENTRANCETO, nullptr, nullptr, nullptr, PEC_EDIT, SCIDESC_ENTRANCETO },
    { SCIPROP_ROOMCTRL, _PopulateControlColourCombo, nullptr, nullptr, PEC_EDITCOMBO, SCIDESC_ROOMCTRL },
    { SCIPROP_MOVESPEED, nullptr, nullptr, nullptr, PEC_EDITSPIN, SCIDESC_MOVESPEED },
    { SCIPROP_XSTEP, nullptr, nullptr, nullptr, PEC_EDITSPIN, SCIDESC_XSTEP },
    { SCIPROP_YSTEP, nullptr, nullptr, nullptr, PEC_EDITSPIN, SCIDESC_YSTEP },
};

const SCIPROPINFO *_FindPropInfo(PCTSTR pszPropName)
{
    const SCIPROPINFO *ppi = nullptr;
    for (int i = 0; i < ARRAYSIZE(c_rgPropInfo); i++)
    {
        if (0 == lstrcmp(pszPropName, c_rgPropInfo[i].pszName))
        {
            ppi = &c_rgPropInfo[i];
            break;
        }
    }
    return ppi;
}

PCTSTR GetSCIPropertyDescription(PCTSTR pszProp)
{
    PCTSTR pszDesc = TEXT("");
    for (int i = 0; i < ARRAYSIZE(c_rgPropInfo); i++)
    {
        if (0 == lstrcmp(pszProp, c_rgPropInfo[i].pszName))
        {
            pszDesc = c_rgPropInfo[i].pszDesc;
            break;
        }
    }
    return pszDesc;
}


IPropEditCtrl *CreatePropEditCtrl(PCTSTR pszName, SCIClassBrowser *pBrowser, CSCIPropertyBagNotify *pBag)
{
    IPropEditCtrl *ppec = nullptr;
    for (int i = 0; i < ARRAYSIZE(c_rgPropInfo); i++)
    {
        if (0 == lstrcmp(pszName, c_rgPropInfo[i].pszName))
        {
            switch (c_rgPropInfo[i].editType)
            {
            case PEC_COMBO:
                //CPropEditCtrlCombo
                ppec = new CPropEditCtrlCombo();
                if (ppec)
                {
                    ppec->Init(pszName, pBrowser, pBag);
                }
                break;

            case PEC_EDIT:
                ppec = new CPropEditCtrlEditBox(FALSE);
                if (ppec)
                {
                    ppec->Init(pszName, pBrowser, pBag);
                }
                break;

            case PEC_EDITSPIN:
                ppec = new CPropEditCtrlEditBox(TRUE);
                if (ppec)
                {
                    ppec->Init(pszName, pBrowser, pBag);
                }
                break;

            case PEC_EDITCOMBO:
                ppec = new CPropEditCtrlCombo(TRUE);
                if (ppec)
                {
                    ppec->Init(pszName, pBrowser, pBag);
                }
                break;
            }

            break;
        }
    }
    return ppec;
}


void PopulatePropEditCtrl(PCTSTR pszName, CWnd *pWnd)
{
    IPropEditCtrl *ppec = nullptr;
    for (int i = 0; i < ARRAYSIZE(c_rgPropInfo); i++)
    {
        if (0 == lstrcmp(pszName, c_rgPropInfo[i].pszName))
        {
            if (c_rgPropInfo[i].pPopulator)
            {
                (*c_rgPropInfo[i].pPopulator)(pWnd);
            }
            break;
        }
    }
}


CString ValueToDisplay(PCTSTR pszName, const PropertyValue &value)
{
    const SCIPROPINFO *ppi = _FindPropInfo(pszName);
    if (ppi)
    {
        CString strDisplay;
        if (ppi->pValueToDisplay)
        {
            (*ppi->pValueToDisplay)(value, strDisplay);
        }
        else
        {
            strDisplay = value.ToString().c_str();
        }
        return strDisplay;
    }
    else
    {
        return TEXT("");
    }
}

void DisplayToValue(PCTSTR pszName, const CString strDisplay, PropertyValue &value)
{
    const SCIPROPINFO *ppi = _FindPropInfo(pszName);
    if (ppi)
    {
        if (ppi->pDisplayToValue)
        {
            (*ppi->pDisplayToValue)(strDisplay, value);
        }
        else
        {
            _GeneralDisplayToValue(strDisplay, value);
        }
    }
    else
    {
        value.SetStringValue("");
    }
}


