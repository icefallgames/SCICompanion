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
#include "ClassBrowser.h"
#include "ScriptOMAll.h"
#include "StudioSyntaxParser.h"
#include "CodeToolTips.h"
#include "AppState.h"
#include <string>
#include "OutputCodeHelper.h"

//
// CodeToolTips.cpp
//
// Figures out the tooltip to be used when the user hovers over certain pieces of code.
//

using namespace sci;
using namespace std;

//
// Fills szBuf with a textual representation of the method.
//
void _GetMethodInfoHelper(PTSTR szBuf, size_t cchBuf, const FunctionBase *pMethod)
{
    const auto &sigs = pMethod->GetSignatures();
    bool first = true;
    szBuf[0] = 0;
    for (const auto &sig : sigs)
    {
        if (!first)
        {
            StringCchCat(szBuf, cchBuf, "\n");
        }
        char szTemp[512];
        StringCchPrintf(szTemp, ARRAYSIZE(szTemp), TEXT("%s("), pMethod->GetName().c_str());
        const auto &params = sig->GetParams();
        size_t requiredParamCount = sig->GetRequiredParameterCount();
        for (size_t iParam = 0; iParam < params.size(); iParam++)
        {
            if (iParam >= requiredParamCount)
            {
                StringCchCat(szTemp, ARRAYSIZE(szTemp), TEXT("["));
            }
            StringCchCat(szTemp, ARRAYSIZE(szTemp), params[iParam]->GetName().c_str());
            if (iParam >= requiredParamCount)
            {
                StringCchCat(szTemp, ARRAYSIZE(szTemp), TEXT("]"));
            }
            if (iParam < (params.size() - 1))
            {
                StringCchCat(szTemp, ARRAYSIZE(szTemp), TEXT(" ")); // add a space
            }
        }
        StringCchCat(szTemp, ARRAYSIZE(szTemp), TEXT(")"));
        StringCchCat(szBuf, cchBuf, szTemp);
        first = false;
    }
}

// Finds the class where a property is defined.
tuple<const ClassDefinition *, const sci::ClassDefinition *, const ClassProperty *> _FindClassProperty(SCIClassBrowser &browser, const sci::ClassDefinition &leafClass, const std::string &propertyName)
{
    ClassProperty *classProp = nullptr;
    ClassDefinition *classDef = nullptr;
    ClassDefinition *sourceClassDef = nullptr;
    const std::vector<ClassDefinition*> &allClasses = browser.GetAllClasses();
    std::string current = leafClass.IsInstance() ? leafClass.GetSuperClass() : leafClass.GetName();
    while (!current.empty())
    {
        auto itClass = match_name(allClasses.begin(), allClasses.end(), current);
        if (itClass != allClasses.end())
        {
            auto &classProps = (*itClass)->GetProperties();
            auto itProp = match_name(classProps.begin(), classProps.end(), propertyName);
            if (itProp != classProps.end())
            {
                if (!classDef)
                {
                    // This is the first class where it's defined.
                    classDef = *itClass;
                    classProp = (*itProp).get();
                }
                // This is the most "top" class
                sourceClassDef = *itClass;
            }
            current = (*itClass)->GetSuperClass();
        }
        else
        {
            break;
        }
    }
    return make_tuple(classDef, sourceClassDef, classProp);
}

void _GetClassInfoHelper(PTSTR szBuf, size_t cchBuf, const ClassDefinition *pClass)
{
    TCHAR szTemp[100];
    szTemp[0] = 0;
    if (!pClass->GetSuperClass().empty())
    {
        StringCchPrintf(szTemp, ARRAYSIZE(szTemp), TEXT(" of %s"), pClass->GetSuperClass().c_str());
    }
    StringCchPrintf(szBuf, cchBuf, TEXT("(class %s%s\n    (properties\n"), pClass->GetName().c_str(), szTemp);

    std::stringstream ss;
    SourceCodeWriter out(ss, appState->GetResourceMap().Helper().GetGameLanguage());
    DebugIndent indent1(out);
    DebugIndent indent2(out);
    out.pszNewLine = "\r\n";
    for (auto &prop : pClass->GetProperties())
    {
        prop->OutputSourceCode(out);
    }
    std::string propValues = ss.str();
    StringCchCat(szBuf, cchBuf, propValues.c_str());
    StringCchCat(szBuf, cchBuf, TEXT("    )\n"));

    // Prepare methods
    TCHAR szMethodPart[1000];
    StringCchPrintf(szMethodPart, ARRAYSIZE(szMethodPart), TEXT("Methods:\n"));
	for (auto &method : pClass->GetMethods())
	{
		TCHAR szMethod[100];
		StringCchCat(szMethodPart, ARRAYSIZE(szMethodPart), TEXT("    "));
		_GetMethodInfoHelper(szMethod, ARRAYSIZE(szMethod), method.get());
		StringCchCat(szMethodPart, ARRAYSIZE(szMethodPart), szMethod);
		StringCchCat(szMethodPart, ARRAYSIZE(szMethodPart), TEXT("\n"));
	}

    StringCchCat(szMethodPart, ARRAYSIZE(szMethodPart), TEXT(")"));
    StringCchCat(szBuf, cchBuf, szMethodPart);
}

