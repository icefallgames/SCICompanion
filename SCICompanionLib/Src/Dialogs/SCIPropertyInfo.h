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

