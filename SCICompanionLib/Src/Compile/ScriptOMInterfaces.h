#pragma once

namespace sci
{
	class PropertyValue;
	class ClassProperty;
}

//
// Generic way for clients to interact with something that has a name and
// some properties.
//
class ISCIPropertyBag
{
public:
	// Implement these
	virtual bool SetProperty(PCTSTR pszName, sci::PropertyValue value) = 0;
	virtual bool GetProperty(PCTSTR pszName, sci::PropertyValue &value) = 0;
	virtual void SetBagName(PCTSTR pszName) = 0;
	virtual const std::string GetBagName() = 0;
	virtual void SetSpecies(PCTSTR pszName) = 0;
	virtual const std::string GetSpecies() const = 0;
	virtual const std::vector<sci::ClassProperty> &GetProperties() = 0;
};

class ISCIPropertyChange
{
public:
	virtual void OnPropertyChange(ISCIPropertyBag *pSource, PCTSTR pszName, sci::PropertyValue value) = 0;
	virtual void OnNameChange(ISCIPropertyBag *pSource, PCTSTR pszName) = 0;
	virtual void OnSpeciesChange(ISCIPropertyBag *pSource, PCTSTR pszSpecies) = 0;
	virtual void OnAddProperty(ISCIPropertyBag *pSource, PCTSTR pszName) = 0;
	virtual void OnRemoveProperty(ISCIPropertyBag *pSource, PCTSTR pszName) = 0;
};
