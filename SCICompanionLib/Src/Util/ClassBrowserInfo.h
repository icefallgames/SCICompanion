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

#include "ScriptOM.h"

//
// This represents a node in the class browser tree.  It contains a reference to a
// class, the class's name, and its superclass and subclasses.
//
// In addition, it contains methods that allow you to enumerate all the methods
// on object (by following its superclasses).
// 
class SCIClassBrowserNode
{
public:
    SCIClassBrowserNode(const sci::Script *pScriptIn) { _pScript = pScriptIn; _pClass = NULL; _pSuperClass = NULL; }
    SCIClassBrowserNode() { _pScript = NULL; _pClass = NULL; _pSuperClass = NULL; }
    ~SCIClassBrowserNode();
    int ComputeAllMethods(sci::RawMethodVector &map) const;
    int ComputeAllProperties(sci::RawClassPropertyVector &map) const;
    void AddSubClass(SCIClassBrowserNode* pSubClass) { _subClasses.push_back(pSubClass); }
    void RemoveSubClass(SCIClassBrowserNode* pSubClass);

    // public accessors
    const std::string &GetName() const { return _name; }
    void SetName(const std::string &name) { _name = name; }

    const sci::ClassDefinition *GetClassDefinition() const { return _pClass; }
    void SetClassDefinition(const sci::ClassDefinition *pClass) { _pClass = pClass; }

    const std::vector<SCIClassBrowserNode*> &GetSubClasses() const { return _subClasses; }
    std::vector<SCIClassBrowserNode*> &GetSubClasses() { return _subClasses; }

    const SCIClassBrowserNode *GetSuperClass() const { return _pSuperClass; }
    SCIClassBrowserNode *GetSuperClass() { return _pSuperClass; }
    void SetSuperClass(SCIClassBrowserNode *pClass) { _pSuperClass = pClass; }

    void SetScript(const sci::Script *pScript) { _pScript = pScript; }
    const sci::Script *GetScript() const { return _pScript; }

private:
    std::string _name;
	const sci::ClassDefinition *_pClass;
    std::vector<SCIClassBrowserNode*> _subClasses;
    SCIClassBrowserNode *_pSuperClass;
    const sci::Script *_pScript;
};