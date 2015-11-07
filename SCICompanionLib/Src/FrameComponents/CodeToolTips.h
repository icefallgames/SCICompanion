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

#include "ToolTipResult.h"
#include "ScriptOMAll.h"
#include "AppState.h"
#include "OutputCodeHelper.h"
#include "format.h"
#include "Kernels.h"

const key_value_pair<PCSTR, PCSTR> c_szVarToClass[] =
{
    { TEXT("gEgo"), TEXT("Ego") },
    { TEXT("gRoom"), TEXT("Rm") },
    { TEXT("gGame"), TEXT("Game") },
    { TEXT("gWindow"), TEXT("Window") },
};

using std::string;
using std::vector;
using std::unique_ptr;
using sci::ClassVector;
using sci::SendCall;
using sci::MethodVector;
using sci::DefineVector;
using sci::ClassPropertyVector;
using sci::RawClassPropertyVector;
using sci::ProcedureVector;
using sci::NodeType;
using sci::FunctionPtr;
using sci::ProcedureDefinition;
using sci::VariableDecl;
using sci::VariableDeclVector;
using sci::ClassDefinition;
using sci::ClassPtr;
using sci::NodeTypeSendCall;
using sci::FunctionBase;


void _GetMethodInfoHelper(PTSTR szBuf, size_t cchBuf, const sci::FunctionBase *pMethod);
void _GetClassInfoHelper(PTSTR szBuf, size_t cchBuf, const sci::ClassDefinition *pClass);
std::tuple<const sci::ClassDefinition *, const sci::ClassDefinition *, const sci::ClassProperty *> _FindClassProperty(SCIClassBrowser &browser, const sci::ClassDefinition &leafClass, const std::string &propertyName);

template<typename _TContext>
std::string _ClassFromObjectName(SCIClassBrowser &browser, _TContext *pContext, const std::string &strObject)
{
    std::string className;
    if (!strObject.empty())
    {
        // Get the last known good version of this script (for resolving things that reside in this script)
        const sci::Script *pGood = browser.GetLKGScript(browser.GetScriptNumberHelper(&pContext->Script()));

        if (pContext->ClassPtr)
        {
            if (strObject == "self")
            {
                className = pContext->ClassPtr->GetName();
            }
            else if (strObject == "super")
            {
                className = pContext->ClassPtr->GetSuperClass();
            }
        }
        if (className.empty())
        {
            // No luck yet... find the last known good version of this script, and try to match it to an instance.
            if (pGood)
            {
                const ClassVector &classes = pGood->GetClasses();
                for (size_t i = 0; i < classes.size(); i++)
                {
                    const ClassDefinition *pClass = classes[i].get();
                    if (pClass->IsInstance() && (strObject == pClass->GetName()))
                    {
                        // We have a match!  Get the instance's super class.
                        // REVIEW: if we had an instant way to get a instance's methods/props, by specifying
                        // a script number and the instance name, then we could avoid all this.
                        className = pClass->GetName();
                        break;
                    }
                }
            }
        }
        if (className.empty())
        {
            // Map some well know global variables to classes.
            // Ideally, they should be mapped to instances, but we really only know
            // how to deal with instances in the current script.
            className = LookupStringValue(c_szVarToClass, ARRAYSIZE(c_szVarToClass), strObject, "");
        }
        if (className.empty())
        {
            className = strObject; // A class name itself.  View:new(), etc...
        }
    }
    return className;
}

template<typename _TContext>
ToolTipResult GetToolTipResult(_TContext *pContext)
{
    TCHAR szTip[MAX_PATH * 4]; // Scratch buffer
    szTip[0] = 0;

    bool fFound = false;
    ToolTipResult result;
    result.OriginalText = pContext->ScratchString();
    SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();

    // Trigger any custom header compiles
    for (const string &include : pContext->Script().GetIncludes())
    {
        browser.TriggerCustomIncludeCompile(include);
    }

    ClassBrowserLock lock(browser);
    // REVIEW: TryLock may fail. For tooltips (which are extracted in the background), we may want to just Lock. But for the context menu
    // (which is extracted in the foreground), we probably want to TryLock. Alternatively, we could implement "Lock" with a short timeout.
    if (lock.TryLock())
    {
        // Grab some scripts to look at.
        std::vector<const sci::Script*> scriptsToSearch;
        // Get the last known good version of this script (for resolving things that reside in this script)
        const sci::Script *pGood = browser.GetLKGScript(browser.GetScriptNumberHelper(&pContext->Script()));
        if (pGood)
        {
            scriptsToSearch.push_back(pGood);
        }
        // And our headers...
        for (const string &include : pContext->Script().GetIncludes())
        {
            sci::Script *temp = browser.GetCustomHeader(include);
            if (temp)
            {
                scriptsToSearch.push_back(temp);
            }
        }

        std::string strText = pContext->ScratchString();

        // Try to figure out where we are.
        // 1. A classname in a class definition?
        if (pContext->ClassPtr &&
            pContext->GetParseAutoCompleteContext() == ParseAutoCompleteContext::SuperClass)
        {
            const std::vector<ClassDefinition*> &classes = browser.GetAllClasses();
            auto classIt = match_name(classes.begin(), classes.end(), strText);
            if (classIt != classes.end())
            {
                _GetClassInfoHelper(szTip, ARRAYSIZE(szTip), (*classIt));

                result.strTip = szTip;
                // Give location information for it.
                result.strBaseText = (*classIt)->GetName();
                result.iLineNumber = (*classIt)->GetLineNumber();
                result.scriptId = ScriptId((*classIt)->GetOwnerScript()->GetPath().c_str());
                fFound = true;
            }
        }

        if (!fFound && !strText.empty() && (pContext->GetParseAutoCompleteContext() == ParseAutoCompleteContext::ScriptName))
        {
            std::string filename =  appState->GetResourceMap().Helper().GetScriptFileName(strText, pContext->Script().Language());
            const sci::Script *useScript = browser.GetLKGScript(filename);
            if (useScript)
            {
                // Show the classes and exports.
                std::stringstream ss;
                ss << "    " << filename << "\n";
                ss << "Exports:" << "\n";
                for (auto &theExport : useScript->GetExports())
                {
                    if (!theExport->Name.empty())
                    {
                        ss << "    " << theExport->Slot << ": " << theExport->Name << "\n";
                    }
                }
                ss << "Classes:" << "\n";
                for (auto &theClass : useScript->GetClasses())
                {
                    if (!theClass->IsInstance())
                    {
                        ss << "    " << theClass->GetName() << "\n";
                    }
                }
                result.strTip = ss.str();
                // Give location information for it.
                result.strBaseText = strText;
                result.iLineNumber = 0;
                result.scriptId = ScriptId(filename);
                fFound = true;
            }
        }

        if (!fFound)
        {
            // 2) A selector in a send statement (e.g. x in (send gEgo:x))
            if (!strText.empty())
            {
                bool fFound = false;
                NodeType type = pContext->GetTopKnownNode();
                if (type == NodeTypeSendCall)
                {
                    const SendCall *pSendCall = static_cast<const SendCall*>(pContext->GetSyntaxNode(NodeTypeSendCall));
                    assert(pSendCall);
                    // Now we have the object to which we're sending... 
                    // Figure out which species/class it is.
                    std::string object = _ClassFromObjectName(browser, pContext, pSendCall->GetObject());
                    if (!object.empty())
                    {
                        unique_ptr<RawMethodVector> pMethods = browser.CreateMethodArray(object, &pContext->Script());
                        const RawMethodVector::const_iterator methodIt = match_name(pMethods->begin(), pMethods->end(), strText);
                        fFound = (methodIt != pMethods->end());
                        if (fFound)
                        {
                            // Make a std::string like
                            // species:method(param1 param2 param3)
                            fFound = true;
                            StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("%s::"), (*methodIt)->GetOwnerClass()->GetName().c_str());
                            TCHAR szTemp[200];
                            _GetMethodInfoHelper(szTemp, ARRAYSIZE(szTemp), (*methodIt));
                            StringCchCat(szTip, ARRAYSIZE(szTip), szTemp);
                            result.strTip = szTip;

                            // "goto definition" info
                            result.iLineNumber = (*methodIt)->GetLineNumber();
                            result.scriptId = ScriptId((*methodIt)->GetOwnerScript()->GetPath().c_str());
                            StringCchPrintf(szTemp, ARRAYSIZE(szTemp), TEXT("%s::%s"), (*methodIt)->GetOwnerClass()->GetName().c_str(), (*methodIt)->GetName().c_str());
                            result.strBaseText = szTemp;
                        }
                        else
                        {
                            // Maybe it's a property
                            auto pProperties = browser.CreatePropertyArray(object, &pContext->Script());
                            fFound = matches_name(pProperties->begin(), pProperties->end(), strText);
                            if (fFound)
                            {
                                StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("%s::%s"), object.c_str(), strText.c_str());
                                result.strTip = szTip;
                            }
                        }
                    }
                }
                // All remaining things require certain conditions.
                ParseAutoCompleteContext pacc = pContext->GetParseAutoCompleteContext();
                bool isValue = (pacc == ParseAutoCompleteContext::Value) || (pacc == ParseAutoCompleteContext::LValue);
                bool isDefineOnly = (pacc == ParseAutoCompleteContext::DefineValue);
                bool isExport = (pacc == ParseAutoCompleteContext::Export);
                if (!fFound)
                {
                    // 3. Is it a define?
                    if (isValue || isDefineOnly)
                    {
                        // Check for defines.
                        DefineVector::const_iterator defineIt;
                        // Local ones.
                        for (auto &script : scriptsToSearch)
                        {
                            const DefineVector &defines = script->GetDefines();
                            defineIt = match_name(defines.begin(), defines.end(), strText);
                            fFound = (defineIt != defines.end());
                            if (fFound)
                            {
                                break;
                            }
                        }
                        if (!fFound)
                        {
                            // Global ones (even those not included by this script)
                            const vector<sci::Script*> &headers = browser.GetHeaders();
                            auto headerIt = headers.begin();
                            for (; !fFound && (headerIt != headers.end()); ++headerIt)
                            {
                                const DefineVector &defines = (*headerIt)->GetDefines();
                                defineIt = match_name(defines.begin(), defines.end(), strText);
                                fFound = (defineIt != defines.end());
                            }
                        }
                        if (!fFound)
                        {
                            // Custom includes
                            auto headerIt = scriptsToSearch.begin();
                            for (; !fFound && (headerIt != scriptsToSearch.end()); ++headerIt)
                            {
                                const DefineVector &defines = (*headerIt)->GetDefines();
                                defineIt = match_name(defines.begin(), defines.end(), strText);
                                fFound = (defineIt != defines.end());
                            }
                        }

                        if (fFound)
                        {
                            // It was a define.
                            bool isHex = IsFlagSet((*defineIt)->GetFlags(), IntegerFlags::Hex);
                            bool isNeg = IsFlagSet((*defineIt)->GetFlags(), IntegerFlags::Negative);
                            std::stringstream ss;
                            _OutputNumber(ss, (*defineIt)->GetValue(), isHex, isNeg);
                            result.strTip = fmt::format("(define {0} {1})", strText, ss.str());
                            // Give location information for it.
                            result.strBaseText = (*defineIt)->GetLabel().c_str();
                            result.iLineNumber = (*defineIt)->GetLineNumber();
                            result.scriptId = ScriptId((*defineIt)->GetOwnerScript()->GetPath().c_str());
                        }
                    }

                    if (!fFound && (isValue || isExport))
                    {
                        // 4. Maybe it's a procedure
                        const ProcedureDefinition *pProc = nullptr;
                        // Check public procedures
                        const RawProcedureVector &publicProcs = browser.GetPublicProcedures();
                        auto procIt = match_name(publicProcs.begin(), publicProcs.end(), strText);
                        fFound = (procIt != publicProcs.end());
                        if (fFound)
                        {
                            pProc = *procIt;
                        }
                        if (!fFound)
                        {
                            for (auto &script : scriptsToSearch)
                            {
                                const ProcedureVector &localProcs = script->GetProcedures();
                                auto procIt2 = match_name(localProcs.begin(), localProcs.end(), strText);
                                fFound = (procIt2 != localProcs.end());
                                if (fFound)
                                {
                                    pProc = (*procIt2).get();
                                    break;
                                }
                            }
                        }
                        if (fFound)
                        {
                            _GetMethodInfoHelper(szTip, ARRAYSIZE(szTip), pProc);
                            result.strTip = szTip;
                            // "goto definition" info
                            result.iLineNumber = pProc->GetLineNumber();
                            result.scriptId = ScriptId(pProc->GetOwnerScript()->GetPath().c_str());
                            result.strBaseText = pProc->GetName();
                        }
                    }

                    if (!fFound && isValue)
                    {
                        // 5. Kernel function?
                        const auto &kProcs = GetKernelSignaturesScript(nullptr).GetProcedures();
                        auto itProc = match_name(kProcs.begin(), kProcs.end(), strText);
                        if (itProc != kProcs.end())
                        {
                            _GetMethodInfoHelper(szTip, ARRAYSIZE(szTip), (*itProc).get());
                            result.strTip = szTip;
                            // TODO: goto could bring us to documentation (or just open webpage)
                            fFound = true;
                        }
                    }

                    if (!fFound && isValue)
                    {
                        // 6. Local method variables
                        const FunctionPtr pFunction = pContext->FunctionPtr.get();
                        if (pFunction)
                        {
                            const VariableDeclVector &tempVars = pFunction->GetVariables();
                            auto varIt = match_name(tempVars.begin(), tempVars.end(), strText);
                            fFound = (varIt != tempVars.end());
                            if (fFound)
                            {
                                result.strTip = fmt::format("Temporary variable: {}", strText);
                                // Add some goto info
                                result.iLineNumber = (*varIt)->GetLineNumber();
                                result.scriptId = ScriptId(pFunction->GetOwnerScript()->GetPath().c_str());
                                result.strBaseText = (*varIt)->GetName().c_str();
                            }
                            if (!fFound && !pFunction->GetSignatures().empty())
                            {
                                const auto &functionParams = pFunction->GetSignatures()[0]->GetParams();
                                auto procIt2 = match_name(functionParams.begin(), functionParams.end(), strText);
                                fFound = (procIt2 != functionParams.end());
                                if (fFound)
                                {
                                    result.strTip = fmt::format("Parameter {}", strText);
                                    // Add some goto info
                                    result.iLineNumber = (*procIt2)->GetLineNumber();
                                    result.scriptId = ScriptId(pFunction->GetOwnerScript()->GetPath().c_str());
                                    result.strBaseText = (*procIt2)->GetName().c_str();
                                }
                            }
                        }
                    }
                    if (!fFound && isValue)
                    {
                        VariableDecl *pVar = nullptr; // Used for goto information

                        // 7. Script variables
                        for (auto &script : scriptsToSearch)
                        {
                            // Locals
                            const VariableDeclVector &localVars = script->GetScriptVariables();
                            auto varIt = match_name(localVars.begin(), localVars.end(), strText);
                            fFound = (varIt != localVars.end());
                            if (fFound)
                            {
                                pVar = (*varIt).get();
                                result.strTip = fmt::format("Script variable: {}", strText);
                                break;
                            }
                        }
                        if (!fFound)
                        {
                            // Globals
                            const VariableDeclVector *pVars = browser.GetMainGlobals();
                            if (pVars)
                            {
                                VariableDeclVector::const_iterator varIt = match_name(pVars->begin(), pVars->end(), strText);
                                fFound = (varIt != pVars->end());
                                if (fFound)
                                {
                                    pVar = (*varIt).get();
                                    result.strTip = fmt::format("Global variable: {}", strText);
                                }
                            }
                        }
                        if (fFound && pVar)
                        {
                            // Add some goto info
                            result.iLineNumber = pVar->GetLineNumber();
                            result.scriptId = ScriptId(pVar->GetOwnerScript()->GetPath().c_str());
                            result.strBaseText = pVar->GetName().c_str();
                        }
                    }

                    if (!fFound && isValue)
                    {
                        // 8. Class property
                        const ClassPtr pClass = pContext->ClassPtr.get();
                        if (pClass)
                        {
                            auto classAndProp = _FindClassProperty(browser, *pClass, strText);
                            if (get<0>(classAndProp))
                            {
                                result.strTip = fmt::format("{}::{}", get<1>(classAndProp)->GetName(), strText);
                                // Add some goto info
                                result.iLineNumber = get<2>(classAndProp)->GetLineNumber();
                                result.scriptId = ScriptId(get<0>(classAndProp)->GetOwnerScript()->GetPath().c_str());
                                result.strBaseText = strText;
                            }
                        }
                    }
                    if (!fFound && isValue)
                    {
                        for (auto &script : scriptsToSearch)
                        {
                            // 9. Local instance (or class, but we would have already handled that in 8)
                            // Find a class that it matches
                            const sci::ClassVector &classes = script->GetClasses();
                            auto classIt = match_name(classes.begin(), classes.end(), strText);
                            fFound = (classIt != classes.end());
                            if (fFound)
                            {
                                result.strTip = fmt::format("{} of {}", strText, (*classIt)->GetSuperClass());
                                // "goto definition" info
                                result.iLineNumber = (*classIt)->GetLineNumber();
                                result.scriptId = ScriptId((*classIt)->GetOwnerScript()->GetPath().c_str());
                                result.strBaseText = (*classIt)->GetName();
                            }
                        }
                        if (!fFound)
                        {
                            // Global class?
                            const sci::ClassDefinition *globalClass = browser.LookUpClass(strText);
                            fFound = globalClass != nullptr;
                            if (fFound)
                            {
                                result.strTip = fmt::format("{} of {}", globalClass->GetName(), globalClass->GetSuperClass());
                                // "goto definition" info
                                result.iLineNumber = globalClass->GetLineNumber();
                                result.scriptId = ScriptId(globalClass->GetOwnerScript()->GetPath().c_str());
                                result.strBaseText = globalClass->GetName();
                            }
                        }
                    }
                    if (!fFound)
                    {
                        // TODO - look up saids - could probably do this by having the
                        // various parses set a bit indicating what kind of parsing they're doing (singleq, doubleq, etc...)
                        // And checking if there is a pContext->PropertyValue, or a ComplexPropertyValue statement.
                        /*
                        PCTSTR pszWord = FindValidVocabStringFromRight(_strValueTurd.c_str());
                        if (pszWord)
                        {
                        // Part of a Said std::string?
                        Vocab000 *pVocab000 = appState->GetResourceMap().GetVocab000();
                        if (pVocab000)
                        {
                        Vocab000::WordGroup dwGroup;
                        if (pVocab000->LookupWord(pszWord, dwGroup))
                        {
                        WordClass dwClass;
                        if (pVocab000->GetGroupClass(dwGroup, &dwClass))
                        {
                        std::string strWordClass;
                        GetWordClassString(dwClass, strWordClass);
                        StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("%s: %s"), pszWord, strWordClass.c_str());
                        }
                        }
                        else
                        {
                        StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("%s - not found"), pszWord);
                        }
                        }
                        }
                        */
                    }
                }
            }
        }
    }
    return result;
}