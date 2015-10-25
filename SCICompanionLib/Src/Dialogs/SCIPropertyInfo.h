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
#pragma once

namespace sci
{
	class PropertyValue;
}

typedef void(* PFNPROPERTYPOPULATOR)(CWnd *pWnd);
typedef void(* PFNPROPERTYDISPLAYTOVALUE)(const CString strDisplay, sci::PropertyValue &value);
typedef void(* PFNPROPERTYVALUETODISPLAY)(const sci::PropertyValue &value, CString &strDisplay);

IPropEditCtrl *CreatePropEditCtrl(PCTSTR pszName, SCIClassBrowser *pBrowser, CSCIPropertyBagNotify *pBag);
PCTSTR GetSCIPropertyDescription(PCTSTR pszProp);
void PopulatePropEditCtrl(PCTSTR pszName, CWnd *pWnd);
CString ValueToDisplay(PCTSTR pszName, const sci::PropertyValue &value);
void DisplayToValue(PCTSTR pszName, const CString strDisplay, sci::PropertyValue &value);

