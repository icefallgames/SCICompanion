#pragma once

#include "ScriptOMInterfaces.h"

//
// CSCIPropertyCtrl
//
// Implements a listview control that can display and edit properties
// from an ISCIPropertyBag
//

class SCIClassBrowser;

namespace sci
{
	class PropertyValue;
	class ClassDefinition;
	class ClassProperty;
	class Script;
}

//
// This is a property bag that supports listeners.  In addition, it is initialized with
// another property bag, from which it takes its initial values (e.g. it copies the property
// bag) and tracks changes.
//

enum PROPERTYCHANGE_INFO
{
    PC_ADDED,
    PC_REMOVED,
    PC_CHANGED,
};

typedef std::unordered_map<std::string, sci::PropertyValue> property_map;

//
// This represents a bag of properties based on a ClassDefinition
// The assumption is that this object is kept alive as long as the class.
//
class CSCIPropertyBagNotify : public ISCIPropertyBag
{
public:
    ISCIPropertyBag *_pBagInner;
    CSCIPropertyBagNotify(SCIClassBrowser *pBrowser, sci::ClassDefinition *pClass);

    void AddListener(ISCIPropertyChange *pListener) { _listeners.Add(pListener); }

    void RemoveListener(ISCIPropertyChange *pListener)
    {
        for (INT_PTR i = 0; i < _listeners.GetSize(); i++)
        {
            if (_listeners[i] == pListener)
            {
                _listeners.RemoveAt(i);
                break;
            }
        }
    }

    // ISCIPropertyBag
    bool SetProperty(PCTSTR pszName, sci::PropertyValue value);
    bool GetProperty(PCTSTR pszName, sci::PropertyValue &value);

    void SetBagName(PCTSTR pszName)
    {
        if (_strName != pszName)
        {
            _strName = pszName;
            _fNameChanged = TRUE;
            for (INT_PTR i = 0; i < _listeners.GetSize(); i++)
            {
                _listeners[i]->OnNameChange(this, pszName);
            }
        }
    }
    const std::string GetBagName() { return _strName; }

    void SetSpecies(PCTSTR pszName)
    {
        if (_strSpecies != pszName)
        {
            _strSpecies = pszName;
            _fSpeciesChanged = TRUE;
            for (INT_PTR i = 0; i < _listeners.GetSize(); i++)
            {
                _listeners[i]->OnSpeciesChange(this, pszName);
            }
        }
    }
    const std::string GetSpecies() const { return _strSpecies; }
    const std::vector<std::unique_ptr<sci::ClassProperty>> &GetProperties()
    {
        return _propertiesEnum; // TODO, empty
    }

    const sci::ClassDefinition *GetClass() const { return _pClass; }

protected:
    sci::ClassDefinition *_pClass;
    SCIClassBrowser *_pBrowser;

private:
    CArray<ISCIPropertyChange *, ISCIPropertyChange *> _listeners;

    // Map of properties
    property_map _properties;
    // And information about when they've changed
    CMap<CString, PCTSTR, PROPERTYCHANGE_INFO, PROPERTYCHANGE_INFO> _changeInfo;
    std::string _strName;
    BOOL _fNameChanged;
    std::string _strSpecies;
    BOOL _fSpeciesChanged;

    std::vector<std::unique_ptr<sci::ClassProperty>> _propertiesEnum; // TODO
};




class CSCIPropertyCtrl : public CListCtrl, public ISCIPropertyChange
{
	DECLARE_DYNAMIC(CSCIPropertyCtrl)

public:
	CSCIPropertyCtrl();
	virtual ~CSCIPropertyCtrl();

    void SetControl(CSCIPropertyBagNotify *pBag);
    void Initialize(const sci::Script *pScript, SCIClassBrowser *pBrowser, CWnd *pDescription);

    // ISCIPropertyChange
    void OnPropertyChange(ISCIPropertyBag *pSource, PCTSTR pszName, sci::PropertyValue value);
    void OnNameChange(ISCIPropertyBag *pSource, PCTSTR pszName);
    void OnSpeciesChange(ISCIPropertyBag *pSource, PCTSTR pszSpecies) { /*TODO*/ }
    void OnAddProperty(ISCIPropertyBag *pSource, PCTSTR pszName) { /*TODO*/ }
    void OnRemoveProperty(ISCIPropertyBag *pSource, PCTSTR pszName) { /*TODO*/ }

protected:
	DECLARE_MESSAGE_MAP()
    void OnItemClick(NMHDR *pNMHDR, LRESULT *pResult);
    void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    void OnLButtonDown(UINT nFlags, CPoint point);

    void _AddProperty(PCTSTR pszProp);

    CSCIPropertyBagNotify *_pBag;
    BOOL _fInLabelEdit;
    BOOL _fMadeAutoComplete;

    const sci::Script *_pScript;
    SCIClassBrowser *_pBrowser;

    CWnd *_pDescription;

    CComboBox m_wndComboInPlace;
};




