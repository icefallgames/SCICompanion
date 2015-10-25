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
//
// SCIPropertyCtrl.cpp : implementation file
// Not yet complete.
//

//
// Implements a property editor control (for editing things like the view property of an Act)
//

#include "stdafx.h"
#include "AppState.h"
#include "SCIPropertyCtrl.h"
#include "SCIProps.h"
#include "PropEditCtrl.h"
#include "SCIPropertyInfo.h"
#include "ScriptOM.h"
#include "ClassBrowser.h"

using namespace sci;

//
// Each kind of object has 3 types of properties
// - "important" properties
// - "pseudo" properties (converted into method calls)
// - open property set (excludes important props)
//
// We'll have a list of important properties for each type here.
//

const PCTSTR c_szViewProps[] = {
    SCIPROP_VIEW,
    SCIPROP_X,
    SCIPROP_Y,
    SCIPROP_LOOP,
    SCIPROP_CEL,
    SCIPROP_Z,
    SCIPROP_PRIORITY,
    SCIPROP_IGNOREACTORS,
    SCIPROP_ADDTOPIC,
};
const PCTSTR c_szPropProps[] = {
    SCIPROP_VIEW,
    SCIPROP_X,
    SCIPROP_Y,
    SCIPROP_LOOP,
    SCIPROP_CYCLESPEED,
    SCIPROP_CEL,
    SCIPROP_Z,
    SCIPROP_PRIORITY,
    SCIPROP_IGNOREACTORS,
};
const PCTSTR c_szDoorProps[] = {
    SCIPROP_VIEW,
    SCIPROP_X,
    SCIPROP_Y,
    SCIPROP_LOOP,
    SCIPROP_DOORBLOCK,
    SCIPROP_DOORCTRL,
    SCIPROP_ROOMCTRL,
    SCIPROP_CYCLESPEED,
    SCIPROP_CEL,
    SCIPROP_Z,
    SCIPROP_PRIORITY,
    SCIPROP_IGNOREACTORS,
};
const PCTSTR c_szActProps[] = {
    SCIPROP_VIEW,
    SCIPROP_X,
    SCIPROP_Y,
    SCIPROP_CYCLESPEED,
    SCIPROP_MOVESPEED,
    SCIPROP_XSTEP,
    SCIPROP_YSTEP,
    SCIPROP_LOOP,
    SCIPROP_CEL,
    SCIPROP_Z,
    SCIPROP_PRIORITY,
    SCIPROP_IGNOREACTORS,
};

//
// Table that maps object species to "important" props
//
struct
{
    PCTSTR pszSpecies;
    const PCTSTR *rgpszProps;
    size_t cProps;
}
c_rgCommonProps[] =
{
    // Ordered from most sublcassed to least (so Act doesn't match View)
    { SCISPECIES_ACT, c_szActProps, ARRAYSIZE(c_szActProps) },
    { SCISPECIES_DOOR, c_szDoorProps, ARRAYSIZE(c_szDoorProps) },
    { SCISPECIES_PROP, c_szPropProps, ARRAYSIZE(c_szPropProps) },
    { SCISPECIES_VIEW, c_szViewProps, ARRAYSIZE(c_szViewProps) },
};





#define NAME_COL 1
#define VALUE_COL 0

// CSCIPropertyCtrl

IMPLEMENT_DYNAMIC(CSCIPropertyCtrl, CListCtrl)
CSCIPropertyCtrl::CSCIPropertyCtrl()
{
    _fInLabelEdit = FALSE;
    _fMadeAutoComplete = FALSE;
    _pBag = NULL;
}

CSCIPropertyCtrl::~CSCIPropertyCtrl()
{
}

void CSCIPropertyCtrl::_AddProperty(PCTSTR pszProp)
{
    ASSERT(_pBag);
    int iIndex = -1;
    if ((0 == lstrcmp(SCIPROP_NAME, pszProp)) ||
        (0 == lstrcmp(SCIPROP_SPECIES, pszProp)))
    {
        BOOL fName = (0 == lstrcmp(SCIPROP_NAME, pszProp));
        // Special case Name
        LVITEM item1 = { 0 };
        item1.mask = LVIF_TEXT;
        item1.pszText = const_cast<LPSTR>(fName ? _pBag->GetBagName().c_str() : _pBag->GetSpecies().c_str());
        item1.iItem = INT_MAX; // Add to end
        item1.iSubItem = VALUE_COL;
        iIndex = InsertItem(&item1);
    }
    else
    {
        PropertyValue value;
        // Try getting the property from the bag itself.
        // If that fails, then try to get the default value from the species.
        if (_pBag->GetProperty(pszProp, value))
        {
            CString strDisplay = ValueToDisplay(pszProp, value);
            LVITEM item1 = { 0 };
            item1.mask = LVIF_TEXT;
            item1.pszText = const_cast<LPSTR>((PCTSTR)strDisplay);
            item1.iItem = INT_MAX; // Add to end
            item1.iSubItem = VALUE_COL;
            iIndex = InsertItem(&item1);
        }
    }

    if (iIndex != -1)
    {
        LVITEM item2 = { 0 };
        item2.mask = LVIF_TEXT;
        item2.pszText = const_cast<LPSTR>(pszProp);
        item2.iItem = iIndex;
        item2.iSubItem = NAME_COL;
        SetItem(&item2);
    }
}

void CSCIPropertyCtrl::SetControl(CSCIPropertyBagNotify *pBag)
{
    // Remove ourselves from our previous property bag:
    if (_pBag)
    {
        _pBag->RemoveListener(this);
    }
    // And add ourselves as a listener to the new:
    _pBag = pBag;
    _pBag->AddListener(this);

    // Clear out our control
    DeleteAllItems();
    
    // Find the most important properties for our species
    const std::string strSpecies = pBag->GetSpecies();
    const PCTSTR *rgpszProps = NULL;
    size_t cCommonProps = 0;
    for (int i = 0; i < ARRAYSIZE(c_rgCommonProps); i++)
    {
        if (_pBrowser->IsSubClassOf(strSpecies.c_str(), c_rgCommonProps[i].pszSpecies))
        {
            rgpszProps = c_rgCommonProps[i].rgpszProps;
            cCommonProps = c_rgCommonProps[i].cProps;
            break;
        }
    }

    // Add "Name" first.
    _AddProperty(SCIPROP_NAME);
    _AddProperty(SCIPROP_SPECIES);

    // Then the others
    for (size_t i = 0; i < cCommonProps; i++)
    {
        _AddProperty(rgpszProps[i]);
    }

    // TODO:
    // Then pseudo props
    // Then any others we find.

}

void CSCIPropertyCtrl::OnPropertyChange(ISCIPropertyBag *pSource, PCTSTR pszName, PropertyValue value)
{
    // REVIEW: ptr equality could be bad... maybe use a unique ID instead.
    if (pSource == _pBag)
    {
        // Find the matching item.
        for (int i = 0; i < GetItemCount(); i++)
        {
            if (GetItemText(i, 1) == pszName)
            {
                // This property changed - update
                CString strDisplay = ValueToDisplay(pszName, value);
                SetItemText(i, 0, strDisplay);
            }
        }
    }
}

void CSCIPropertyCtrl::OnNameChange(ISCIPropertyBag *pSource, PCTSTR pszName)
{
    // Name should always be the first one
    if (pSource == _pBag)
    {
        SetItemText(0, 0, pszName);
    }
}


void CSCIPropertyCtrl::Initialize(const Script *pScript, SCIClassBrowser *pBrowser, CWnd *pDescription)
{
    _pDescription = pDescription;

    // Add grid lines.
    DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
    SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LPARAM(dwExStyle));

    CRect rc;
    GetClientRect(&rc);

    // Add columns.
    // Name
    LVCOLUMN col = { 0 };
    col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
    col.iOrder = 0;
    col.iSubItem = 0;
    col.pszText = TEXT("Value");
    col.cx = rc.Width() * 3 / 5 - GetSystemMetrics(SM_CXVSCROLL); // In case the scrollbar appears.
    col.fmt = LVCFMT_RIGHT;
    InsertColumn(0, &col);

    col.iOrder = 1;
    col.iSubItem = 1;
    col.pszText = TEXT("Property");
    col.cx = rc.Width() * 2 / 5;
    InsertColumn(1, &col);

    int rgOrder[] = { 1, 0 } ;
    SetColumnOrderArray(ARRAYSIZE(rgOrder), rgOrder);

    _pScript = pScript;
    _pBrowser = pBrowser;

}


BEGIN_MESSAGE_MAP(CSCIPropertyCtrl, CListCtrl)
    ON_WM_CREATE()
    //ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
    ON_NOTIFY_REFLECT(NM_CLICK, OnItemClick)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CSCIPropertyCtrl::OnItemClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMITEMACTIVATE *pia = (NMITEMACTIVATE*)pNMHDR;
    CRect rect;
    // Only respond to clicks on subitem 0
    if (pia->iSubItem == VALUE_COL)
    {
        if (GetSubItemRect(pia->iItem, pia->iSubItem, LVIR_LABEL, rect))
        {
            IPropEditCtrl *ppec = CreatePropEditCtrl(GetItemText(pia->iItem, NAME_COL), _pBrowser, _pBag);
            if (ppec)
            {
                ppec->BeginEdit(&rect, this);
            }
        }
    }
}

void CSCIPropertyCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLISTVIEW *pnmlv = (NMLISTVIEW*)pNMHDR;
    if (pnmlv->uChanged & LVIF_STATE)
    {
        // State changed.
        if (pnmlv->uNewState & LVIS_SELECTED)
        {
            // This item has been selected.
            CString strPropName = GetItemText(pnmlv->iItem, NAME_COL);
            if (_pDescription)
            {
                _pDescription->SetWindowText(GetSCIPropertyDescription(strPropName));
            }
        }
    }
}


void CSCIPropertyCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    __super::OnLButtonDown(nFlags, point);

    // Hit test, to see if we were on column 0.
    LVHITTESTINFO lvhti = { 0 };
    lvhti.flags = LVHT_ONITEM;
    lvhti.pt = point;
    int nItem = SubItemHitTest(&lvhti);
    if (nItem != -1)
    {
        if (lvhti.iSubItem == VALUE_COL)
        {
            // The user clicked on the string part.  Go into edit mode.
            EditLabel(nItem);
        }
    }
}





//
// CSCIPropertyBagNotify
//
CSCIPropertyBagNotify::CSCIPropertyBagNotify(SCIClassBrowser *pBrowser, ClassDefinition *pClass)
{
    _pBrowser = pBrowser;
    _pClass = pClass;
    _strName = pClass->GetName();
    _strSpecies = pClass->GetSpecies();
    _fNameChanged = FALSE;
    _fSpeciesChanged = FALSE;
}

bool CSCIPropertyBagNotify::GetProperty(PCTSTR pszName, PropertyValue &value)
{
    property_map::const_iterator findIt = _properties.find(pszName);
    bool fRet = findIt != _properties.end();
    if (fRet)
    {
        value = findIt->second;
    }
    else
    {
        // Special case name and species
        if (0 == lstrcmp(SCIPROP_NAME, pszName))
        {
            fRet = true;
            value.SetStringValue(_strName);
        }
        else if (0 == lstrcmp(SCIPROP_SPECIES, pszName))
        {
            fRet = true;
            value.SetStringValue(_strSpecies);
        }
        else
        {
            // Ask the browser for the value from the class.  It will automatically go up
            // the chain to superclasses, if necessary.
            // Also, pseudoprops are handled by the class itself.
            fRet = _pBrowser->GetProperty(pszName, _pClass, value);
            if (fRet)
            {
                // Cache it (don't notify, since nothing changed)
                _properties[pszName] = value;
            }
        }
    }
    return fRet;
}


bool CSCIPropertyBagNotify::SetProperty(PCTSTR pszName, PropertyValue value)
{
    bool fRet = false;
    // Special case name and species
    if (0 == lstrcmp(SCIPROP_NAME, pszName))
    {
        SetBagName(value.ToString().c_str());
    }
    else if (0 == lstrcmp(SCIPROP_SPECIES, pszName))
    {
        SetSpecies(value.ToString().c_str());
    }
    else
    {
        // REVIEW: when species changes, we might need to recalculate/recache properties.
        // (easy way to do - look through all cached props, and ask for them again - NOTE: that
        //  means we have to use the species string from the propbag, and not the one from
        //  ClassDefinition when asking for props - this will require a change)
        // Only fire a change if the bag didn't have the property originally,
        // or the value is different.
        property_map::const_iterator findIt = _properties.find(pszName);
        bool fPresent = (findIt != _properties.end());
        if (!fPresent || (!(findIt->second == value)))
        {
            _properties[pszName] = value;
            _changeInfo.SetAt(pszName, fPresent ? PC_CHANGED : PC_ADDED);
            fRet = true;
            for (INT_PTR i = 0; i < _listeners.GetSize(); i++)
            {
                _listeners[i]->OnPropertyChange(this, pszName, value);
            }
        }
    }
    return fRet;
}