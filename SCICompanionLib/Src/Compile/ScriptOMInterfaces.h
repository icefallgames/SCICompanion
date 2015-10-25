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
	virtual const std::vector<std::unique_ptr<sci::ClassProperty>> &GetProperties() = 0;
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
