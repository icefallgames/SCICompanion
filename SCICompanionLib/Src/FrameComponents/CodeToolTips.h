#pragma once

#include "ToolTipResult.h"
#include "ScriptOMAll.h"
#include "AppState.h"
#include "PMachine.h"
#include "OutputCodeHelper.h"

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
	SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
    ClassBrowserLock lock(browser);
    // REVIEW: TryLock may fail. For tooltips (which are extracted in the background), we may want to just Lock. But for the context menu
    // (which is extracted in the foreground), we probably want to TryLock. Alternatively, we could implement "Lock" with a short timeout.
    if (lock.TryLock())
    {
        // Try to figure out where we are.
        // 1. A classname in a class definition? (Detect this by seeing if name is empty, but superclass is not)
		//	REVIEW: The above comment makes no sense. I've changed it to the logic below: (prior to that, it required GetSuperClass to be empty)
        if (pContext->ClassPtr &&
            !pContext->ClassPtr->GetName().empty() &&
            !pContext->ClassPtr->GetSuperClass().empty())
        {
            std::string strText = pContext->ScratchString();
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

		if (!fFound)
		{
            // Get the last known good version of this script (for resolving things that reside in this script)
            const sci::Script *pGood = browser.GetLKGScript(browser.GetScriptNumberHelper(&pContext->Script()));

            // 2) A selector in a send statement (e.g. x in (send gEgo:x))
            std::string strText = pContext->ScratchString();
            if (!strText.empty())
            {
                bool fFound = false;
                NodeType type = pContext->GetTopKnownNode();
                if (type == NodeTypeSendParam)
                {
                    //const SendParam *pSendParam = static_cast<const SendParam*>(pContext->GetSyntaxNode(type));
                    const SendCall *pSendCall = static_cast<const SendCall*>(pContext->GetSyntaxNode(NodeTypeSendCall));
                    assert(pSendCall);
                    // Now we have the object to which we're sending... 
                    // Figure out which species/class it is.
                    std::string object = _ClassFromObjectName(browser, pContext, pSendCall->GetObject());
                    if (!object.empty())
                    {
                        unique_ptr<RawMethodVector> pMethods(browser.CreateMethodArray(object, &pContext->Script()));
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
                            unique_ptr<RawClassPropertyVector> pProperties(browser.CreatePropertyArray(object, &pContext->Script()));
                            fFound = matches_name(pProperties->begin(), pProperties->end(), strText);
                            if (fFound)
                            {
                                StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("%s::%s"), object.c_str(), strText.c_str());
                                result.strTip = szTip;
                            }
                        }
                    }
                }
                if (!fFound)
                {
                    // 3. Is it a define?

                    // Check for defines.
                    DefineVector::const_iterator defineIt;
                    // Local ones.
                    if (pGood)
                    {
                        const DefineVector &defines = pGood->GetDefines();
                        defineIt = match_name(defines.begin(), defines.end(), strText);
                        fFound = (defineIt != defines.end());
                    }
                    if (!fFound)
                    {
                        // Global ones (even those not included by this script)
                        const vector<sci::Script*> &headers = browser.GetHeaders();
                        vector<sci::Script*>::const_iterator headerIt = headers.begin();
                        for (; !fFound && (headerIt != headers.end()); ++headerIt)
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
                        StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("(define %s %s)"), strText.c_str(), ss.str().c_str());
                        result.strTip = szTip;
                        // Give location information for it.
                        result.strBaseText = (*defineIt)->GetLabel().c_str();
                        result.iLineNumber = (*defineIt)->GetLineNumber();
                        result.scriptId = ScriptId((*defineIt)->GetOwnerScript()->GetPath().c_str());
                    }
                }
                if (!fFound)
                {
                    // 4. Maybe it's a procedure
                    const ProcedureDefinition *pProc = NULL;
                    // Check public procedures
                    const RawProcedureVector &publicProcs = browser.GetPublicProcedures();
                    auto procIt = match_name(publicProcs.begin(), publicProcs.end(), strText);
                    fFound = (procIt != publicProcs.end());
					if (fFound)
					{
						pProc = *procIt;
					}
                    if (!fFound && pGood)
                    {
                        const ProcedureVector &localProcs = pGood->GetProcedures();
                        auto procIt2 = match_name(localProcs.begin(), localProcs.end(), strText);
                        fFound = (procIt2 != localProcs.end());
						if (fFound)
						{
							pProc = (*procIt2).get();
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
                if (!fFound)
                {
                    // 5. Kernel function?
                    KernelInfo *pkernel = match_name_dot(KrnlInfo, KrnlInfo + ARRAYSIZE(KrnlInfo), strText);
                    fFound = (pkernel != KrnlInfo + ARRAYSIZE(KrnlInfo));
                    if (fFound)
                    {
                        StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("%s %s(%s)"), (PCTSTR)pkernel->Ret, (PCTSTR)pkernel->Name, (PCTSTR)pkernel->Params);
                        result.strTip = szTip;
                    }
                }
                if (!fFound)
                {
                    // 6. Local method variables
                    const FunctionPtr pFunction = pContext->FunctionPtr.get();
                    if (pFunction)
                    {
                        const VariableDeclVector &tempVars = pFunction->GetVariables();
                        fFound = matches_name(tempVars.begin(), tempVars.end(), strText);
                        if (fFound)
                        {
                            StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("Temporary variable: %s"), strText.c_str());
                            result.strTip = szTip;
                        }
                        if (!fFound)
                        {
                            // TODO: needs updating due to GetParams
                            /*
                            const vector<std::string> &functionParams = pFunction->GetParams();
                            fFound = find(functionParams.begin(), functionParams.end(), strText) != functionParams.end();
                            if (fFound)
                            {
                                StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("Parameter: %s"), strText.c_str());
                                result.strTip = szTip;
                            }*/
                        }
                    }
                }
                if (!fFound)
                {
                    // 7. Script variables
                    VariableDecl *pVar = nullptr; // Used for goto information
                    if (pGood)
                    {
                        // Locals
                        const VariableDeclVector &localVars = pGood->GetScriptVariables();
                        auto varIt = match_name(localVars.begin(), localVars.end(), strText);
                        fFound = (varIt != localVars.end());
                        if (fFound)
                        {
                            pVar = (*varIt).get();
                            StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("Script variable: %s"), strText.c_str());
                            result.strTip = szTip;
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
                                StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("Global variable: %s"), strText.c_str());
                                result.strTip = szTip;
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
                if (!fFound)
                {
                    // 8. Class property
                    const ClassPtr pClass = pContext->ClassPtr.get();
                    if (pClass && !pClass->GetSuperClass().empty())
                    {
                        // Since we already know the superclass, don't pass the script in (since that relies on a
                        // successfully compiled previous script)
                        unique_ptr<RawClassPropertyVector> pProperties(browser.CreatePropertyArray(pClass->GetName(), nullptr, pClass->GetSuperClass().c_str()));
                        fFound = matches_name(pProperties->begin(), pProperties->end(), strText);
                        if (fFound)
                        {
                            StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("%s::%s"), pClass->GetSuperClass().c_str(), strText.c_str());
                            result.strTip = szTip;
                        }
                    }
                }
                if (!fFound && pGood)
                {
                    // 9. Local instance (or class, but we would have already handled that in 8)
                    // Find a class that it matches
                    const sci::ClassVector &classes = pGood->GetClasses();
                    auto classIt = match_name(classes.begin(), classes.end(), strText);
                    fFound = (classIt != classes.end());
                    if (fFound)
                    {
                        StringCchPrintf(szTip, ARRAYSIZE(szTip), TEXT("%s of %s"), strText.c_str(), (*classIt)->GetSuperClass().c_str());
                        result.strTip = szTip;
                        // "goto definition" info
                        result.iLineNumber = (*classIt)->GetLineNumber();
                        result.scriptId = ScriptId((*classIt)->GetOwnerScript()->GetPath().c_str());
                        result.strBaseText = (*classIt)->GetName();
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
    return result;
}