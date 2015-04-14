#include "stdafx.h"
#include "AppState.h"
#include "ScriptOM.h"
#include "ClassBrowser.h"
#include "ClassBrowserInfo.h"

using namespace sci;

SCIClassBrowserNode::~SCIClassBrowserNode()
{
    // Don't delete all our children - they are not owned by us.
}

//
// Scan this guy, and all its superclasses, for methods.
// Put the methods in map.
//
int SCIClassBrowserNode::ComputeAllMethods(RawMethodVector &array) const
{
    int iIndexSubclassBegin = 0;
    if (_pClass)
    {
        for (auto &method : _pClass->GetMethods())
        {
            // Is this method already in the map?
            if (!matches_name(array.begin(), array.end(), method->GetName()))
            {
                // Nope.  Add it.
                array.push_back(method.get());
            }
            // else yes.  Subclasses win, so don't do anything here
        }

        iIndexSubclassBegin = (int)array.size();
        // Now go to our parent.
        if (_pSuperClass)
        {
            _pSuperClass->ComputeAllMethods(array);
        }
    }
    return iIndexSubclassBegin;
}

int SCIClassBrowserNode::ComputeAllProperties(ClassPropertyVector &array) const
{
    int iIndexSubclassBegin = 0;
    if (_pClass)
    {
        // STLCLEANUP
        const ClassPropertyVector &props = _pClass->GetProperties();
        for (size_t i = 0; i < props.size(); i++)
        {
            const ClassProperty &prop = props[i];
            bool fFound = false;
            for (auto &element : array)
            {
                if (element.GetName() == prop.GetName())
                {
                    fFound = true;
                    break;
                }
            }
            if (!fFound)
            {
                // Nope.  Add it.
                array.push_back(prop);
            }
            // else yes.  Subclasses win, so don't do anything here
        }
        iIndexSubclassBegin = (int)array.size();
        // Now go to our parent.
        if (_pSuperClass)
        {
            _pSuperClass->ComputeAllProperties(array);
        }
    }
    return iIndexSubclassBegin;
}


void SCIClassBrowserNode::RemoveSubClass(SCIClassBrowserNode* pSubClass)
{
    // We should always find it.
    // Also, assume there is only one.
    _subClasses.erase(find(_subClasses.begin(), _subClasses.end(), pSubClass));
}
