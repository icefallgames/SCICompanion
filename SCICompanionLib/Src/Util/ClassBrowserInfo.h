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