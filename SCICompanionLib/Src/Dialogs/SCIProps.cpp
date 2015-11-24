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
#include "SCIProps.h"
#include "ClassBrowser.h"
#include "AppState.h"
#include "ScriptOMAll.h"

using namespace sci;

//
// Return true to stop, false to continue
//
typedef bool(CALLBACK *PFNFOUNDSENDCALLBACK )(PCTSTR pszMethodSelector, const SendCall *pSend, const SendParam **ppSendParam);

//
// Recursively enumerates all send calls in a segment.
//
bool _EnumSendCallsInSegments(const std::vector<std::unique_ptr<SyntaxNode>> *pSegments, PFNFOUNDSENDCALLBACK pfnSend, PCTSTR pszMethodSelector, const SendCall **ppSendCall, const SendParam **ppSendParam)
{
    bool fFound = false;
    for (size_t i = 0; !fFound && i < pSegments->size(); i++)
    {
        const SyntaxNode *pSegment = (*pSegments)[i].get();
        if (NodeTypeCodeBlock == pSegment->GetNodeType())
        {
            const CodeBlock *pSegmentList = static_cast<const CodeBlock*>((*pSegments)[i].get());
            auto &segments = pSegmentList->GetList();
            fFound = _EnumSendCallsInSegments(&segments, pfnSend, pszMethodSelector, ppSendCall, ppSendParam);
        }
        else if (NodeTypeSendCall == pSegment->GetNodeType())
        {
            const SendCall *pSendProposed = static_cast<const SendCall*>((*pSegments)[i].get());
            fFound = (*pfnSend)(pszMethodSelector, pSendProposed, ppSendParam);
            if (fFound)
            {
                ASSERT(*ppSendParam);
                *ppSendCall = pSendProposed;
            }
        }
    }
    return fFound;
}

bool CALLBACK _TestForSelfAnyMethod(PCTSTR pszMethodSelector, const SendCall *pSend, const SendParam **ppSendParam)
{
    bool fFound = false;
    //if (pSend && pSend->GetObject().GetName() == "self")
    if (pSend && pSend->GetObject() == "self")
    {
        auto &params = pSend->GetParams();

		for (auto it = params.begin(); !fFound && (it != params.end()); ++it)
		{
			fFound = (*it)->GetSelectorName() == pszMethodSelector;
			if (fFound)
			{
				*ppSendParam = it->get();
			}
		}
    }
    return fFound;
}

//
// Searches for the first instance of pszSelector in a (self: call in a class's init method
// If found, it returns the SendParam for this. 
// 
bool _FindSelfSendParamInInit(const ClassDefinition *pClass, PCTSTR pszSelector, const SendParam **ppSendParam)
{
    bool fRet = false;
	const MethodVector &methods = pClass->GetMethods();
	const MethodDefinition *pInitMethod = match_name(methods, "init");
	if (pInitMethod)
	{
		// Search for a top level codesegment that has "self" as a send object
		auto &segments = pInitMethod->GetCodeSegments();
		const SendCall *pSendCall;
		fRet = _EnumSendCallsInSegments(&segments, _TestForSelfAnyMethod, pszSelector, &pSendCall, ppSendParam);
	}

    return fRet;
}



//
// Used to obtain the first parameter for a selector in a send call
// 
bool _GetFirstSimpleValueInCodeSegmentArray(const SyntaxNodeVector &segments, PropertyValue &value)
{
    bool fRet = false;
    // Reimplement this.
/*    if (!segments.empty())
    {
        if (segments[0]->GetType() == NodeTypeValue)
        {
            value = segments[0]->GetValue();
            fRet = true;
        }
    }*/
    return fRet;
}


bool _PriorityClassToProp(const ClassDefinition *pClass, PCTSTR pszProp, PropertyValue& value)
{
    const SendParam *pSendParam;
    bool fRet = _FindSelfSendParamInInit(pClass, SCIMETHOD_SETPRI, &pSendParam);
    if (fRet)
    {
        // See what the value is.
        if (pSendParam->HasValue())
        {
            fRet = _GetFirstSimpleValueInCodeSegmentArray(pSendParam->GetSelectorParams(), value);
        }
    }
    if (!fRet)
    {
        // We didn't find anything.  The default value for priority tends to be zero, but that isn't
        // accurate.  It depends on the signal property.
        // Instead we'll use -1 as a sentinel value indicating "default priority".
        value.SetValue((WORD)-1);
        fRet = true;
    }
    return fRet;
}

bool _IgnoreActorsToProp(const ClassDefinition *pClass, PCTSTR pszProp, PropertyValue& value)
{
    const SendParam *pSendParam;
    bool fRet = _FindSelfSendParamInInit(pClass, SCIMETHOD_IGNOREACTORS, &pSendParam);
    if (fRet)
    {
        // See what the value is.
        if (pSendParam->HasValue())
        {
            fRet = _GetFirstSimpleValueInCodeSegmentArray(pSendParam->GetSelectorParams(), value);
        }
        if (!fRet)
        {
            // "ignoreActors()"
            // No params means yes, ignore actors.
            value.SetValue(1);
            fRet = true;
        }
    }
    else
    {
        // We didn't find any call to ignoreActors.  By default actors are not ignored.
        // (Actually, it would depend on bit $4000 of the signal property technically)
        value.SetValue(0);
        fRet = true;
    }
    return fRet;
}

bool _AddToPicToProp(const ClassDefinition *pClass, PCTSTR pszProp, PropertyValue& value)
{
    const SendParam *pSendParam;
    // This only depends on whether we found an addToPic call
    value.SetValue(_FindSelfSendParamInInit(pClass, SCIMETHOD_ADDTOPIC, &pSendParam) ? 1 : 0);
    return true;
}




// Descriptions of pseudo props themselves
struct
{
    PCTSTR pszPseudoProp;
    DWORD dwType;
    PFNCLASSTOPSEUDOPROP pClassToProp;    // Some way of mapping property values into SendParam
    PFNPSEUDOPROPTOCLASS pPropToClass;   // Some way of mapping CSCIOMSendParams into property values.
}
c_rgPseudoPropInfo[] =
{
    { SCIPROP_PRIORITY, NULL, _PriorityClassToProp, NULL },
    { SCIPROP_IGNOREACTORS, NULL, _IgnoreActorsToProp, NULL },
    { SCIPROP_ADDTOPIC, NULL, _AddToPicToProp, NULL },
//    { SCIPROP_OBSERVECONTROL, NULL, _ObserveControlToProp, NULL }, // Very difficult
};


BOOL IsPseudoProp(PCTSTR pszProp, PFNCLASSTOPSEUDOPROP *ppfn)
{
    BOOL fRet = FALSE;
    for (int i = 0; !fRet && i < ARRAYSIZE(c_rgPseudoPropInfo); i++)
    {
        fRet = (0 == lstrcmp(c_rgPseudoPropInfo[i].pszPseudoProp, pszProp));
        if (fRet)
        {
            *ppfn = c_rgPseudoPropInfo[i].pClassToProp;
            fRet = (*ppfn != 0); // TODO: temporary until we have functions for all
        }
    }
    return fRet;
}
