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
#include "AppState.h"
#include "ScriptOM.h"
#include "ClassBrowser.h"
#include "ClassBrowserInfo.h"
#include "SCO.h"
#include "SyntaxParser.h"
#include "CompiledScript.h"
#include "SCIDocs.h"
#include "CompileContext.h"
#include "ResourceContainer.h"
#include "CodeAutoComplete.h"
#include "AutoCompleteContext.h"
#include "Task.h"
#include "CCrystalTextBuffer.h"
#include "CrystalScriptStream.h"
#include "ResourceBlob.h"
#include "DependencyTracker.h"

using namespace sci;
using namespace std;
using namespace stdext;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct ReloadScriptPayload
{
    ReloadScriptPayload(SCIClassBrowser &browser, const std::string &pathIfSingleScript) : Browser(browser), ScriptPath(pathIfSingleScript) {}
    SCIClassBrowser &Browser;
    std::string ScriptPath;
};


//
// helper that looks up a pointer in a map (e.g. like you use the operator[] for, but it doesn't create
// a new entry in the map)
//
template<typename T, typename T2>
typename T::pointer lookup_ptr(const std::unordered_map<T2, T> &map, const T2 &str)
{
    typename T::pointer pRet = nullptr;
	std::unordered_map<T2, T>::const_iterator nodeIt = map.find(str);
    if (nodeIt != map.end())
    {
        pRet = nodeIt->second.get();
    }
    return pRet;
}

template<typename T, typename T2>
bool lookup_item(const std::unordered_map<T2, T> &map, const T2 &str, T &result)
{
    bool fRet = false;
	std::unordered_map<T2, T>::const_iterator nodeIt = map.find(str);
    if (nodeIt != map.end())
    {
        result = nodeIt->second;
        fRet = true;
    }
    return fRet;
}

bool IsBrowseInfoEnabled()
{
    return appState->IsBrowseInfoEnabled();
}

class DummyCompileLog : public ICompileLog
{
public:
    virtual void ReportResult(const CompileResult &result) {};
};

SCIClassBrowser::SCIClassBrowser(DependencyTracker &dependencyTracker) : _kernelNames(_kernelNamesResource.GetNames()), _invalidAutoCompleteSources(AutoCompleteSourceType::None), _dependencyTracker(dependencyTracker)
{
    _fPublicProceduresValid = false;
    _fPublicClassesValid = false;
    _fCBLocked = 0;
    _pEvents = nullptr;
    _scheduler = std::make_unique<BackgroundScheduler<ReloadScriptPayload>>();
}

SCIClassBrowser::~SCIClassBrowser()
{
    // assert we've been reset.
    assert(_scripts.empty());
    assert(_pLKGScript == nullptr);
    _scheduler->Exit();
}

void SCIClassBrowser::Lock() const
{
    _mutexClassBrowser.lock();
    ++_fCBLocked;
}

bool SCIClassBrowser::TryLock() const
{
    
    bool fRet = _mutexClassBrowser.try_lock();
    if (fRet)
    {
        ++_fCBLocked;
    }
    return fRet;
}

bool SCIClassBrowser::HasLock() const
{
    return (_fCBLocked > 0);
}

void SCIClassBrowser::Unlock() const
{
    // ref count this (and TryLock/Lock) to be more robust
    // (since we might have code that locks it twice on the same thread, which is ok)
    --_fCBLocked;
    assert(_fCBLocked >= 0);
    _mutexClassBrowser.unlock();
}

void SCIClassBrowser::SetClassBrowserEvents(IClassBrowserEvents *pEvents)
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    _pEvents = pEvents;
}

void SCIClassBrowser::OnOpenGame(SCIVersion version)
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    _version = version;
    if (IsBrowseInfoEnabled() && appState->GetResourceMap().IsGameLoaded())
    {
        ExitSchedulerAndReset();
        _scheduler->SubmitTask(
            std::make_unique<ReloadScriptPayload>(*this, ""),
            [](ITaskStatus &status, ReloadScriptPayload &payload)
        {
            if (!payload.Browser.ReLoadFromSources(status) && !status.IsAborted())
            {
                // Might not be a fan-made game... try loading from the resources themselves so
                // that we are able to provide a class hierarchy at least.
                payload.Browser.ReLoadFromCompiled(status);
            }
            return nullptr;
        }
            );
    }
}

void SCIClassBrowser::ExitSchedulerAndReset()
{
    // Abort before we go into the mutex, which can block...
    if (_scheduler)
    {
        _scheduler->Exit();
    }
    
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);

    _pLKGScript = nullptr;
    _wLKG = 1000; // out of bounds

    _scripts.clear();

    // Delete all header scripts.
    _headerMap.clear();

    // Delete all our browser infos for the classes
    _classMap.clear();

    // And the objects
    _instances.clear();

    _instanceMap.clear();

    // These are the files we look at
    fFoundRoot = false;
    strRootNames.clear();

    _fPublicProceduresValid = false;
    _fPublicClassesValid = false;

    _customHeaderMap.clear();

    // Make a new one.
    _scheduler = std::make_unique<BackgroundScheduler<ReloadScriptPayload>>();
}

void SCIClassBrowser::_AddInstanceToMap(Script& script, ClassDefinition *pClass)
{
    assert(pClass->IsInstance());

    WORD wScript = GetScriptNumberHelper(&script);
    if (wScript == InvalidResourceNumber)
    {
        appState->LogInfo(TEXT("Class browser: Invalid script number in %s."), script.GetPath());
    }

    // As always, the script owns the class
    _instanceMap[wScript].push_back(pClass);
}

void SCIClassBrowser::_AddToClassTree(Script& script)
{
    const ClassVector &classes = script.GetClasses();
    for (size_t i = 0; i < classes.size(); i++)
    {
        ClassDefinition *pTheClass = classes[i].get();
        SCIClassBrowserNode *pBrowserInfo;

        if ((pTheClass->GetName() == "Rm") || (pTheClass->GetName() == "Room"))
        {
            _roomClassName = pTheClass->GetName();
        }

        if (pTheClass->IsInstance())
        {
            // We only encounter instances once each... so make a new SCIClassBrowserNode for each.
			std::unique_ptr<SCIClassBrowserNode> newNode = std::make_unique<SCIClassBrowserNode>(&script);
			pBrowserInfo = newNode.get();
			newNode->SetClassDefinition(pTheClass);
			newNode->SetName(pTheClass->GetName());
			_instances.push_back(std::move(newNode));
            _AddInstanceToMap(script, pTheClass);
        }
        else
        {
            // Add an entry for ourselves, if there is not one already.
            auto nodeIt = _classMap.find(pTheClass->GetName());
            if (nodeIt != _classMap.end())
            {
                pBrowserInfo = (*nodeIt).second.get();
                if (pBrowserInfo)
                {
                    // Ensure that we don't add the same class twice (instances don't count)
                    assert(!pTheClass->IsInstance());
                    if (pBrowserInfo->GetClassDefinition() == nullptr)
                    {
                        // We're already there.  That just means a subclass of ours was added
                        // first.  Just set our pClass.
                        pBrowserInfo->SetClassDefinition(pTheClass);

                        // And fill in the script.
                        assert(pBrowserInfo->GetScript() == nullptr);
                        pBrowserInfo->SetScript(&script);
                    }
                    else
                    {
                        // SCI compiler seems to allow you to declare a class twice, even though they're automatically
                        // exported from the script.  The actual engine uses class selectors to identify superclasses, so
                        // it can handle it.  The name is just an artifact of the compiler. I'm not sure what would happen
                        // if you "included" two different scripts that had the same class, thus generating an ambiguity.
                        // I've not tried that.
                        appState->LogInfo(TEXT("Encountered second class named %s.  Not adding to tree"), pTheClass->GetName().c_str());
                        // Set this to null so we bail out of the rest of the function.
                        pBrowserInfo = nullptr;
                    }
                }
            }
            else
            {
                // This is the first time we've been encountered (didn't encounter any subclasses first)
                unique_ptr<SCIClassBrowserNode> newNode = make_unique<SCIClassBrowserNode>(&script);
                pBrowserInfo = newNode.get();
                newNode->SetClassDefinition(pTheClass);
                newNode->SetName(pTheClass->GetName());
                _classMap[pTheClass->GetName()] = move(newNode);
            }
        }

        if (pBrowserInfo)
        {
            // Now find our super class, and hook us up.
            std::string superClassName = pTheClass->GetSuperClass();
            if (superClassName.empty())
            {
                // This is the root.  Apparently we can have multiple roots though.
                fFoundRoot = true;
                strRootNames.push_back(pTheClass->GetName());
            }
            else
            {
                // Look up our superclass
                class_map::iterator nodeIt = _classMap.find(superClassName);
                if (nodeIt != _classMap.end())
                {
                    SCIClassBrowserNode *pBrowserInfoSuper = (*nodeIt).second.get();
                    // It's already there.  Just add ourselves to it.
                    pBrowserInfoSuper->AddSubClass(pBrowserInfo);
                    // And we get a reference on it.
                    pBrowserInfo->SetSuperClass(pBrowserInfoSuper);
                }
                else
                {
                    // There isn't any superclass for us yet.  Make a new one.
                    unique_ptr<SCIClassBrowserNode> pBrowserInfoSuper = make_unique<SCIClassBrowserNode>();
                    pBrowserInfoSuper->AddSubClass(pBrowserInfo); // Add ourselves to its subclasses.
                    pBrowserInfoSuper->SetName(superClassName); // The name is all we have now.
                    pBrowserInfo->SetSuperClass(pBrowserInfoSuper.get()); // So we have a way to get to it.
                    _classMap[superClassName] = move(pBrowserInfoSuper); // Add our super to the classMap.
                }
            }
        }
    }

    _invalidAutoCompleteSources = AutoCompleteSourceType::ScriptName | AutoCompleteSourceType::ClassName | AutoCompleteSourceType::Procedure;
}

bool SCIClassBrowser::ReLoadFromSources(ITaskStatus &task)
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    if (IsBrowseInfoEnabled())
    {
        // Load the kernel and selector names
        _kernelNamesResource.Load(appState->GetResourceMap().Helper());
        _selectorNames.Load(appState->GetResourceMap().Helper());

        // Add headers first, since they have defines that are needed by the other scripts.
        _AddHeaders();
        bool fRet = _CreateClassTree(task);
        _MaybeGenerateAutoCompleteTree();
        return fRet;
    }
    else
    {
        return true; // No error
    }
}

void LoadClassFromCompiled(sci::ClassDefinition *pClass, const CompiledScript &compiledScript, CompiledObject *pObject, SelectorTable *pNames, const std::unordered_map<uint16_t, std::string> &speciesToName)
{
    pClass->SetInstance(pObject->IsInstance());
    pClass->SetName(pObject->GetName());

    // We don't look up super class names from the selector table.  In fact, we can't know what
    // the superclass really is, until we have read in all scripts.
    WORD wSuperClass = pObject->GetSuperClass();

    auto it = speciesToName.find(wSuperClass);
    if (it != speciesToName.end())
    {
        pClass->SetSuperClass(it->second);
    }
    // 0xffff means no superclass, we'll leave it empty.
    // Or maybe just > the number of class names means no class.  Seems so in SQ3.

    const std::vector<WORD> &properties = pObject->GetProperties();
    const std::vector<CompiledVarValue> &propertyValues = pObject->GetPropertyValues();
    // Instances will have values, but no selector IDs - so that's ok that properties is smaller
    // than propertyValues.
    assert(properties.size() <= propertyValues.size());
    if (pClass->IsInstance())
    {
        // Add the values anyway.... we'll resolve property selectors later.
        for (size_t i = 0; i < propertyValues.size(); i++)
        {
            if (propertyValues[i].isObjectOrString)
            {
                pClass->AddProperty(make_unique<ClassProperty>(TEXT("Unknown"), compiledScript.GetStringFromOffset(propertyValues[i].value)));
            }
            else
            {
                pClass->AddProperty(make_unique<ClassProperty>(TEXT("Unknown"), propertyValues[i].value));
            }
        }
    }
    else
    {
        for (size_t i = 0; i < properties.size(); i++)
        {
            if (propertyValues[i].isObjectOrString)
            {
                pClass->AddProperty(make_unique<ClassProperty>(pNames->Lookup(properties[i]), compiledScript.GetStringFromOffset(propertyValues[i].value)));
            }
            else
            {
                pClass->AddProperty(make_unique<ClassProperty>(pNames->Lookup(properties[i]), propertyValues[i].value));
            }
        }
    }

    const std::vector<WORD> &methods = pObject->GetMethods();
    for (size_t i = 0; i < methods.size(); i++)
    {
		std::unique_ptr<sci::MethodDefinition> pMethod = std::make_unique<sci::MethodDefinition>();
        pMethod->SetName(pNames->Lookup(methods[i]));
        pMethod->SetOwnerClass(pClass);
		pClass->AddMethod(std::move(pMethod));
    }
}


//
// This creates it from a representation of a compiled script
//
void LoadScriptFromCompiled(sci::Script *pScript, CompiledScript *pCompiledScript, SelectorTable *pNames, const std::unordered_map<uint16_t, std::string> &speciesToName)
{
    BOOL fRet = TRUE;
    pScript->SetScriptNumber(pCompiledScript->GetScriptNumber());

    for (auto &object : pCompiledScript->GetObjects())
    {
        std::unique_ptr<sci::ClassDefinition> pClass = std::make_unique<ClassDefinition>();
        pClass->SetScript(pScript);
        LoadClassFromCompiled(pClass.get(), *pCompiledScript, object.get(), pNames, speciesToName);
        pScript->AddClass(std::move(pClass));
    }
}

#define REENABLE_COMPILEDSCRIPTS

//
// Generates a class browser from compiled scripts
// 
void SCIClassBrowser::ReLoadFromCompiled(ITaskStatus &task)
{
    if (!IsBrowseInfoEnabled())
    {
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 

#ifdef REENABLE_COMPILEDSCRIPTS

    // Load the kernel and selector names
    _kernelNamesResource.Load(appState->GetResourceMap().Helper());
    _selectorNames.Load(appState->GetResourceMap().Helper());

    GlobalClassTable classTable;
    if (!classTable.Load(appState->GetResourceMap().Helper()))
    {
        return;
    }

    fFoundRoot = FALSE;
    int iWorked = 0;
    int iFailed = 0;


    // We need to read in all the compiled scripts before we go making a CSCIOMScript for each.
    // This is because CSCIOMScript needs any and all superclass names, and we don't have those until all
    // classes are loaded.
    std::unordered_map<int, std::unique_ptr<CompiledScript>> compiledScriptsMap;

    if (_pEvents)
    {
        _pEvents->NotifyClassBrowserStatus(IClassBrowserEvents::InProgress, 0);
    }

    unordered_map<int, pair<unique_ptr<ResourceBlob>, unique_ptr<ResourceBlob>>> heapScriptPairs;
    auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Script | ResourceTypeFlags::Heap, ResourceEnumFlags::MostRecentOnly);
    for (auto &scriptBlob : *resourceContainer)
    {
        pair<unique_ptr<ResourceBlob>, unique_ptr<ResourceBlob>> &entry = heapScriptPairs[scriptBlob->GetNumber()];
        if (scriptBlob->GetType() == ResourceType::Script)
        {
            entry.first = move(scriptBlob);
        }
        else
        {
            entry.second = move(scriptBlob);
        }
    }

    // Now load them
    for (auto &heapScriptPair : heapScriptPairs)
    {
        int scriptNumber = heapScriptPair.first;
        pair<unique_ptr<ResourceBlob>, unique_ptr<ResourceBlob>> &entry = heapScriptPair.second;
        if (entry.first)    // It's possible we only find a heap, in error conditions.
        {
            std::unique_ptr<sci::istream> scriptStream(new sci::istream(entry.first->GetData(), entry.first->GetLength()));
            std::unique_ptr<sci::istream> heapStream;
            if (entry.second)
            {
                // Only SCI1.1+ games have separate heap resources.
                heapStream.reset(new sci::istream(entry.second->GetData(), entry.second->GetLength()));
            }
            std::unique_ptr<CompiledScript> pCompiledScript = std::make_unique<CompiledScript>(scriptNumber);
            if (pCompiledScript->Load(appState->GetResourceMap().Helper(), appState->GetVersion(), scriptNumber, *scriptStream, heapStream.get()))
            {
                compiledScriptsMap[scriptNumber] = move(pCompiledScript);
            }
        }
    }

    if (_pEvents)
    {
        _pEvents->NotifyClassBrowserStatus(IClassBrowserEvents::InProgress, 10);
    }
    int startProgress = 10;
    int endProgress = 50;

    // Now we need to prepare the array of superclass names, so we can give it to the CSCIOMScript's.
    std::unordered_map<uint16_t, std::string> speciesToName;
    const std::vector<WORD> &scriptNums = classTable.GetScriptNums();
    for (size_t i = 0; i < scriptNums.size(); i++)
    {
        WORD wScriptNum = scriptNums[i];
        auto it = compiledScriptsMap.find(wScriptNum);
        if (it != compiledScriptsMap.end())
        {
            for (auto &object : (*it).second->GetObjects())
            {
                CompiledObject *pObject = object.get();
                if (!pObject->IsInstance())
                {
                    // This is a class.  Ask for its species, and record the info
                    speciesToName[pObject->GetSpecies()] = pObject->GetName();
                }
            }
        }
        else
        {
            // This means we didn't find a script of this number
            // Oh well, they could just be leftover turds that were left in the class table!
            TCHAR szMsg[100];
            StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("class %d: couldn't find script %d!\n"), i, wScriptNum);
            OutputDebugString(szMsg);
        }
        if (_pEvents)
        {
            _pEvents->NotifyClassBrowserStatus(IClassBrowserEvents::InProgress, (int)(startProgress + (endProgress - startProgress) * i / scriptNums.size()));
        }
    }
    // Now we have our map of class # to class names.    
    
    startProgress = 50;
    endProgress = 75;
    size_t scriptCount = compiledScriptsMap.size();
    size_t soFar = 0;
    for (auto &compiledScript : compiledScriptsMap)
    {
        // There was a script of this number...
        TCHAR szScriptNum[20];
        StringCchPrintf(szScriptNum, ARRAYSIZE(szScriptNum), TEXT("script %03d"), compiledScript.first);
        std::unique_ptr<sci::Script> pScript = std::make_unique<sci::Script>(szScriptNum);
        LoadScriptFromCompiled(pScript.get(), compiledScript.second.get(), &_selectorNames, speciesToName);
		_AddToClassTree(*pScript);
		_scripts.push_back(std::move(pScript)); // Takes ownership
        soFar++;
        if (_pEvents)
        {
            _pEvents->NotifyClassBrowserStatus(IClassBrowserEvents::InProgress, (int)(startProgress + (endProgress - startProgress) * soFar / scriptCount));
        }
    }

    startProgress = 75;
    endProgress = 100;

    // Now we have one missing piece when loading compiled things.  We couldn't get property names
    // resolved for instances, since we didn't necessarily already encounter the super class which
    // mapped property index to property selector.  So do that now.
    int count = 0;
	for (auto &instanceNode : _instances)
    {
        // SCIClassBrowserNode has the class ptr being const for reasons of public
        // consumption.  It's ok for us to modify it though.
		sci::ClassDefinition *pClass = const_cast<sci::ClassDefinition*>(instanceNode->GetClassDefinition());
        std::string superClass = pClass->GetSuperClass();
        // Find the super class...
        auto nodeIt = _classMap.find(superClass);
        if (nodeIt != _classMap.end())
        {
            SCIClassBrowserNode *pNodeSuper = nodeIt->second.get();
            sci::ClassPropertyVector &classProps = const_cast<sci::ClassPropertyVector&>(pClass->GetProperties());
            const sci::ClassPropertyVector &superClassProps = pNodeSuper->GetClassDefinition()->GetProperties();
            for (size_t i = 0; i < superClassProps.size(); i++)
            {
                if (i < classProps.size())
                {
                    classProps[i]->SetName(superClassProps[i]->GetName());
                }
            }
        }
        if (_pEvents)
        {
            count++;
            _pEvents->NotifyClassBrowserStatus(IClassBrowserEvents::InProgress, (int)(startProgress + (endProgress - startProgress) * count / _instances.size()));
        }
    }

    if (_pEvents)
    {
        _pEvents->NotifyClassBrowserStatus(HasErrors() ? IClassBrowserEvents::Errors : IClassBrowserEvents::Ok, 0);
    }
#endif
}

void SCIClassBrowser::TriggerReloadScript(const std::string &fullPath)
{
    _scheduler->SubmitTask(
        std::make_unique<ReloadScriptPayload>(*this, fullPath),
        [](ITaskStatus &status, ReloadScriptPayload &payload)
    {
        payload.Browser.ReloadScript(payload.ScriptPath);
        return nullptr;
    }
        );
}

//
// Reloads a single script into the class browser.
//
void SCIClassBrowser::ReloadScript(const std::string &fullPath)
{
    ClearErrors();
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    if (!IsBrowseInfoEnabled())
    {
        return;
    }
    if (StrRStrI(PathFindFileName(fullPath.c_str()), nullptr, TEXT(".sh")))
    {
        // It's a header file
        _AddHeader(fullPath.c_str());
        // Regenerate the defines cache
        _CacheHeaderDefines();
    }
    else
    {
        // It's a regular one.
        _AddFileName(fullPath, true);

        if (_pEvents)
        {
            _pEvents->NotifyClassBrowserStatus(HasErrors() ? IClassBrowserEvents::Errors : IClassBrowserEvents::Ok, 0);
        }
    }

    _MaybeGenerateAutoCompleteTree();
}

//
// Removes any data we have cached, that lives in pScript 
//
void SCIClassBrowser::_RemoveAllRelatedData(Script *pScript)
{
    // Remove stuff from this script's key in the instance classMap.
    instance_map::iterator instanceIt = _instanceMap.find(GetScriptNumberHelper(pScript));
    if (instanceIt != _instanceMap.end())
    {
        _instanceMap.erase(instanceIt);
    }

    // Go through our instancemap.  Go backwards, since we're deleting items as we go.
    // REVIEW PERF: could probably use a better data structure here.  All we are doing is keeping
    // track of SCIClassBrowserNode's.  We remove some, remove all, add some.  We never lookup.
    if (!_instances.empty())
    {
        size_t i = _instances.size();
        do
        {
            i--;
            auto &node = _instances[i];
			if (node->GetScript() == pScript)
            {
				assert(node->GetSubClasses().empty()); // since it's an instance
				// Remove ourself from our super's list of subclasses
                if (node->GetSuperClass()) // REVIEW REVIEW: why was this null? sq5Narrator in main.
                {
                    node->GetSuperClass()->RemoveSubClass(node.get());
                }
				// Remove this node.
				_instances.erase(_instances.begin() + i);
            }
        } while (i > 0);
    }

    // Now remove all classes defined herein.  This is a little more tricky.
    class_map::iterator nodeIt = _classMap.begin();
    while (nodeIt != _classMap.end())
    {
        SCIClassBrowserNode *pNode = (*nodeIt).second.get();
        if (pNode)
        {
            if (pNode->GetScript() == pScript)
            {
                // This is from the script we're removing.  We need to delete this node.
                // But first, remove ourselves from our superclass.
                if (pNode->GetSuperClass())
                {
                    pNode->GetSuperClass()->RemoveSubClass(pNode);
                }
                // And remove ourselves from our subclasses
                std::vector<SCIClassBrowserNode*> &subClasses = pNode->GetSubClasses();
                for (size_t i = 0; i < subClasses.size(); i++)
                {
                    SCIClassBrowserNode *pNodeSubClass = subClasses[i];
                    if (pNodeSubClass)
                    {
                        assert(pNodeSubClass->GetSuperClass() == pNode); // Make sure its us.
                        pNodeSubClass->SetSuperClass(nullptr); // Remove it
                    }
                }
                // Now we can delete ourselves.
                nodeIt = _classMap.erase(nodeIt);
            }
            else
            {
                nodeIt++;
            }
        }
    }


    // Finally, mark our caches as being invalid.  We'll recalculate them next time someone
    // asks for them.
    _fPublicProceduresValid = false;
    _fPublicClassesValid = false;
}

bool SCIClassBrowser::_AddFileName(std::string fullPath, bool fReplace)
{
    _pLKGScript = nullptr; // Clear cache.  Possible optimization: check LKG number, and if this is the same, then set _pLKGScript to this one.

    bool fRet = false;
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(fullPath.c_str()))
    {
        // "normalize" it before we use it as a key.
        std::string fullPathLower = fullPath;
        std::transform(fullPathLower.begin(), fullPathLower.end(), fullPathLower.begin(), ::tolower);

        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);
        std::unique_ptr<Script> pScript = std::make_unique<Script>(fullPath.c_str());
        if (g_Parser.Parse(*pScript, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), this))
        {
            Script *pWeakRef = pScript.get();

            bool fAdded = false;
            if (fReplace)
            {
                WORD wScriptNumber = GetScriptNumberHelper(pScript.get());
                _filenameToScriptNumber[fullPathLower] = wScriptNumber;

                assert(wScriptNumber != InvalidResourceNumber); // Do something about this.
                // Find matching script number and replace
                for (auto &script : _scripts)
                {
                    if (GetScriptNumberHelper(script.get()) == wScriptNumber)
                    {
                        _RemoveAllRelatedData(script.get());
                        fAdded = true;
                        // Replace
                        script = std::move(pScript); // Take ownership.
                    }
                }
            }
            else
            {
                WORD wScriptNumber = GetScriptNumberHelper(pScript.get());
                _filenameToScriptNumber[fullPathLower] = wScriptNumber;
            }

            _dependencyTracker.ProcessScript(*pWeakRef);

            _AddToClassTree(*pWeakRef);
            if (!fAdded)
            {
                _scripts.push_back(std::move(pScript)); // Takes ownership
            }
            fRet = true;
        }
        buffer.FreeAll();
    }
    return fRet;
}

void SCIClassBrowser::_MaybeGenerateAutoCompleteTree()
{
    if (_invalidAutoCompleteSources != AutoCompleteSourceType::None)
    {
        CPrecisionTimer timer;
        timer.Start();

        // REVIEW: If we find this takes too long, we can split things into different _acLists that are updated at different freqs
        std::vector<ACTreeLeaf> items;
        items.reserve(4000);
        for (auto &aDefine : _headerDefines)
        {
            // Standard defines in the system and game header files.
            items.emplace_back(AutoCompleteSourceType::Define, aDefine.first);
            //AddToTernaryList(items, aDefine.first, AutoCompleteSourceType::Define);
        }
        for (auto &aClass : _classMap)
        {
            // Updated when new classes created (rare)
            items.emplace_back(AutoCompleteSourceType::ClassName, aClass.first);
        }
        for (auto &selector : _selectorNames.GetNames())
        {
            // Updated only when new selectors are created (rare)
            items.emplace_back(AutoCompleteSourceType::Selector, selector);
        }
        for (auto &kernelNames : _kernelNamesResource.GetNames())
        {
            items.emplace_back(AutoCompleteSourceType::Kernel, kernelNames);
        }
        for (auto &publicProc : _GetPublicProcedures())
        {
            items.emplace_back(AutoCompleteSourceType::Procedure, publicProc->GetName());
        }
        for (auto &script : _scripts)
        {
            items.emplace_back(AutoCompleteSourceType::ScriptName, script->GetTitle());
        }

        // Now some global variables
        const VariableDeclVector *globals = _GetMainGlobals();
        if (globals)
        {
            for (const auto &global : *globals)
            {
                items.emplace_back(AutoCompleteSourceType::Variable, global->GetName());
            }
        }

        std::multiset<ACTreeLeaf> itemsSorted;
        std::copy(items.begin(), items.end(), std::inserter(itemsSorted, itemsSorted.begin()));
        _aclist.BuildDatabase(itemsSorted);

        _invalidAutoCompleteSources = AutoCompleteSourceType::None;
    }
}

const sci::ClassDefinition *SCIClassBrowser::LookUpClass(const std::string &className) const
{
    const sci::ClassDefinition *theClass = nullptr;
    auto it = _classMap.find(className);
    if (it != _classMap.end())
    {
        theClass = it->second->GetClassDefinition();
    }
    return theClass;
}

void SCIClassBrowser::GetAutoCompleteChoices(const std::string &prefixIn, AutoCompleteSourceType sourceTypes, std::vector<AutoCompleteChoice> &choices)
{
    choices.clear();
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    std::string prefixLower = prefixIn;
    std::transform(prefixLower.begin(), prefixLower.end(), prefixLower.begin(), ::tolower);
    _aclist.GetAutoCompleteChoices(prefixLower, sourceTypes, choices);
}

SCIClassBrowser::TimeAndHeader::TimeAndHeader() {}
SCIClassBrowser::TimeAndHeader::TimeAndHeader(FILETIME ft, std::unique_ptr<sci::Script> header) : ft(ft), header(std::move(header)) {}
SCIClassBrowser::TimeAndHeader::TimeAndHeader(TimeAndHeader &&src) : ft(ft), header(std::move(src.header)) {}
SCIClassBrowser::TimeAndHeader &SCIClassBrowser::TimeAndHeader::operator=(TimeAndHeader &&src)
{
    if (this != &src)
    {
        ft = src.ft;
        header = std::move(src.header);
    }
    return *this;
}

std::vector<std::string> globalHeaders =
{
    "game.sh", "verbs.sh", "talkers.sh", "sci.sh", "keys.sh"
};

void SCIClassBrowser::TriggerCustomIncludeCompile(std::string name)
{
    // Exclude global headers. They tend to be large, and we index the defines in them seperately.
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    if (find(globalHeaders.begin(), globalHeaders.end(), name) == globalHeaders.end())
    {
        bool needRecompile = false;

        auto lock = std::make_unique<std::lock_guard<std::recursive_mutex>>(_mutexClassBrowser);
        auto it = _customHeaderMap.find(name);
        bool alreadyExists = (it != _customHeaderMap.end());
        needRecompile = !alreadyExists;
        lock = nullptr;

        // Get time stamp
        std::string path = appState->GetResourceMap().GetIncludePath(name);
        HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            FILETIME lastWriteTime;
            BOOL result = GetFileTime(hFile, nullptr, nullptr, &lastWriteTime);
            CloseHandle(hFile);
            if (result)
            {
                if (!needRecompile)
                {
                    needRecompile = (0 != CompareFileTime(&it->second.ft, &lastWriteTime));
                }

                if (needRecompile)
                {
                    // Time-stamp is different, or we haven't yet compiled this file
                    // It's a header we have not yet encountered. Parse it.
                    ScriptId scriptId(path);

                    CCrystalTextBuffer buffer;
                    if (buffer.LoadFromFile(scriptId.GetFullPath().c_str()))
                    {
                        CScriptStreamLimiter limiter(&buffer);
                        CCrystalScriptStream stream(&limiter);
                        unique_ptr<Script> pNewHeader = std::make_unique<Script>(scriptId);
                        if (g_Parser.Parse(*pNewHeader, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), nullptr))
                        {
                            // For performance, let's pre-sort the defines.
                            std::sort(pNewHeader->GetDefines().begin(), pNewHeader->GetDefines().end(), 
                                [](std::unique_ptr<sci::Define> &one, std::unique_ptr<sci::Define> &two) { return one->GetName() < two->GetName(); }
                                );

                            std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
                            TimeAndHeader th { lastWriteTime, move(pNewHeader) };
                            _customHeaderMap[name] = move(th);
                        }
                        buffer.FreeAll();
                    }
                }
            }
        }
    }
}

sci::Script *SCIClassBrowser::GetCustomHeader(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    // Be careful. We don't want to lock the class browser while compiling.
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    assert(_fCBLocked);

    sci::Script *customHeader = nullptr;
    auto it = _customHeaderMap.find(name);
    if (it != _customHeaderMap.end())
    {
        customHeader = it->second.header.get();
    }
    return customHeader;
}

bool SCIClassBrowser::_CreateClassTree(ITaskStatus &task)
{
    ClearErrors();

    CPrecisionTimer timer;
    timer.Start();

    if (_pEvents)
    {
        _pEvents->NotifyClassBrowserStatus(IClassBrowserEvents::InProgress, 0);
    }
    std::vector<ScriptId> scripts;
    appState->GetResourceMap().GetAllScripts(scripts);
    bool fRet = false;
    std::vector<ScriptId>::iterator scriptIt = scripts.begin();
    int cItems = (int)scripts.size();
    int iItem = 0;
    bool fAborted = false;
    while ((scriptIt != scripts.end()) && !fAborted)
    {
        if (_pEvents)
        {
            _pEvents->NotifyClassBrowserStatus(IClassBrowserEvents::InProgress, 100 * iItem / cItems);
        }
        if (_AddFileName(scriptIt->GetFullPath()))
        {
            // As long as we find one script, we consider it a success and don't fallback to compiled sources.
            fRet = true;
        }
        ++scriptIt;
        ++iItem;
        fAborted = task.IsAborted();
    }

    if (_pEvents)
    {
        _pEvents->NotifyClassBrowserStatus(HasErrors() ? IClassBrowserEvents::Errors : IClassBrowserEvents::Ok, 0);
    }

    return fRet;
}
void SCIClassBrowser::_CacheHeaderDefines()
{
    _headerDefines.clear();
    for (auto &header : _headerMap)
    {
        // Suck out the defines and make them convenienty accessible in a classMap
        for (auto &theDefine : header.second->GetDefines())
        {
            _headerDefines.emplace(theDefine->GetLabel(), DefineValueCache(theDefine->GetValue(), theDefine->GetFlags()));
        }
    }

    _invalidAutoCompleteSources |= AutoCompleteSourceType::Define;
}

std::unique_ptr<sci::Script> SCIClassBrowser::_LoadScript(PCTSTR pszPath)
{
    unique_ptr<Script> pScript;
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(pszPath))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);
        std::unique_ptr<Script> pScriptT = std::make_unique<Script>(pszPath);
        if (g_Parser.Parse(*pScriptT, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), this))
        {
            pScript = move(pScriptT);
        }
        buffer.FreeAll(); // REVIEW: not exception safe.
    }
    return pScript;
}

void SCIClassBrowser::_AddHeader(PCTSTR pszHeaderPath)
{
    unique_ptr<Script> pScript = _LoadScript(pszHeaderPath);
    if (pScript)
    {
        _headerMap[pszHeaderPath] = move(pScript);
    }
}


void SCIClassBrowser::_AddHeaders()
{
    // REVIEW: ideally we'll want to include any new headers the user has made, by analyzing the
    // include statements in the scripts.  For now, we'll just hard-code 3 scripts:

    // game.sh
    TCHAR szHeaderPath[MAX_PATH];
    if (SUCCEEDED(StringCchPrintf(szHeaderPath, ARRAYSIZE(szHeaderPath), TEXT("%s\\game.sh"), appState->GetResourceMap().Helper().GetSrcFolder().c_str())))
    {
        _AddHeader(szHeaderPath);
    }
    // SCI1.1 games have Verbs.sh and Talkers.sh
    if (SUCCEEDED(StringCchPrintf(szHeaderPath, ARRAYSIZE(szHeaderPath), TEXT("%s\\Verbs.sh"), appState->GetResourceMap().Helper().GetSrcFolder().c_str())))
    {
        _AddHeader(szHeaderPath);
    }
    if (SUCCEEDED(StringCchPrintf(szHeaderPath, ARRAYSIZE(szHeaderPath), TEXT("%s\\Talkers.sh"), appState->GetResourceMap().Helper().GetSrcFolder().c_str())))
    {
        _AddHeader(szHeaderPath);
    }

    // sci.sh
    std::string includeFolder = appState->GetResourceMap().GetIncludeFolder();
    if (!includeFolder.empty())
    {
        TCHAR szHeaderPath[MAX_PATH];
        if (SUCCEEDED(StringCchPrintf(szHeaderPath, ARRAYSIZE(szHeaderPath), TEXT("%s\\sci.sh"), includeFolder.c_str())))
        {
            _AddHeader(szHeaderPath);
        }
        if (SUCCEEDED(StringCchPrintf(szHeaderPath, ARRAYSIZE(szHeaderPath), TEXT("%s\\keys.sh"), includeFolder.c_str())))
        {
            _AddHeader(szHeaderPath);
        }
    }

    // Make all the header defines accessible in a classMap.
    _CacheHeaderDefines();
}

//
// Allocates a method array, containing all the methods implemented by the
// object strObject.
//
std::unique_ptr<RawMethodVector> SCIClassBrowser::CreateMethodArray(const std::string &strObject, Script *pScript) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
	auto pMethods = std::make_unique<RawMethodVector>();
    if (IsBrowseInfoEnabled())
    {
        bool fFound = false;
        std::string strClass;
        // If a script was provided, try first looking up the object in the instance array for that script.
        if (pScript)
        {
            auto instanceIt = _instanceMap.find(GetScriptNumberHelper(pScript));
            if (instanceIt != _instanceMap.end())
            {
                // Find a match
                for (auto &theClass : instanceIt->second)
                {
                    fFound = (theClass->GetName() == strObject);
                    if (fFound)
                    {
                        // Add this instance's methods to the method array
                        add_smart_to_raw(theClass->GetMethods(), *pMethods);
                        // And use instance's super class as the class to lookup
                        strClass = theClass->GetSuperClass();
                        break;
                    }
                }
            }
        }

        if (!fFound)
        {
            strClass = strObject; // Must be a class
        }

        class_map::const_iterator nodeIt = _classMap.find(strClass);
        if (nodeIt != _classMap.end())
        {
            int iNeedThisEventually = (*nodeIt).second->ComputeAllMethods(*pMethods);
        }
    }
    return pMethods;
}

std::unique_ptr<std::vector<std::string>> SCIClassBrowser::CreatePropertyNameArray(const std::string &strObject, Script *pScript, PCTSTR pszSuper) const
{
    auto allProps = CreatePropertyArray(strObject, pScript, pszSuper);
    auto stringProps = std::make_unique<std::vector<std::string>>();
    std::transform(allProps->begin(), allProps->end(), std::back_inserter(*stringProps),
        [](sci::ClassProperty *prop) { return prop->GetName(); }
        );
    return stringProps;
}

//
// Allocates a method array, containing all the methods implemented by the
// object strObject.
//
std::unique_ptr<RawClassPropertyVector> SCIClassBrowser::CreatePropertyArray(const std::string &strObject, Script *pScript, PCTSTR pszSuper) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    auto pProperties = std::make_unique<RawClassPropertyVector>();
    if (IsBrowseInfoEnabled())
    {
        // Assume first that this is a class name (since this should be quick lookup)
        class_map::const_iterator nodeIt = _classMap.find(strObject);
        if (nodeIt != _classMap.end())
        {
            int iNeedThisEventually = nodeIt->second->ComputeAllProperties(*pProperties);
        }
        else if (pScript)
        {
            // If looking up strObject didn't work, maybe it's an instance (which isn't in the class classMap)
            auto instanceIt = _instanceMap.find(GetScriptNumberHelper(pScript));
            if (instanceIt != _instanceMap.end())
            {
                bool fFound = false;
                for (auto &theClass : instanceIt->second)
                {
                    fFound = (theClass->GetName() == strObject);
                    if (fFound)
                    {
                        // Found it.  Now look up its superclass (species)
                        nodeIt = _classMap.find(theClass->GetSuperClass());
                        if (nodeIt != _classMap.end())
                        {
                            int iNeedThisEventually = nodeIt->second->ComputeAllProperties(*pProperties);
                        }
                        break;
                    }
                }
            }
        }
        else if (pszSuper)
        {
            assert(_classMap.find(strObject) == _classMap.end());
            //nodeIt = classMap && classMap.Lookup(strObject.c_str(), pBrowserInfo))
            // Or maybe the caller already new the super 
            //int iNeedThisEventually = pBrowserInfo->ComputeAllProperties(*pProperties);
        }
    }
    return pProperties;
}

std::vector<std::string> SCIClassBrowser::GetDirectSubclasses(const std::string &species)
{
    std::vector<std::string> directSubclasses;
    for (auto &aClass : _classMap)
    {
        auto classDef = aClass.second->GetClassDefinition();
        if (classDef && classDef->GetSuperClass() == species)
        {
            directSubclasses.push_back(aClass.second->GetName());
        }
    }
    return directSubclasses;
}

//
// Given a species, returns an array of all subclasses of that species (not instances, just classes),
// including the original.
// The caller must delete the std::vector<std::string>
//
std::unique_ptr<std::vector<std::string>> SCIClassBrowser::CreateSubSpeciesArray(const std::string &species)
{
    // REVIEW: it might be faster to go through all classes, and ask if they are a subclass of pszSpecies.
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    auto pArray = std::make_unique<std::vector<std::string>>();
    class_map::iterator nodeIt = _classMap.find(species);
    if (nodeIt != _classMap.end())
    {
        _AddSubclassesToArray(*pArray, nodeIt->second.get());
    }
    return pArray;
}

//
// Recursively adds subclasses to a string array
//
void SCIClassBrowser::_AddSubclassesToArray(std::vector<std::string> &pArray, SCIClassBrowserNode *pBrowserInfo)
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    // Add the name of this class.
    pArray.push_back(pBrowserInfo->GetName());
    std::vector<SCIClassBrowserNode*> &subClasses = pBrowserInfo->GetSubClasses();
    for (size_t i = 0; i < subClasses.size(); i++)
    {
        SCIClassBrowserNode *pNode = subClasses[i];
        if (!pNode->GetClassDefinition()->IsInstance())
        {
            // It's a class, not an instance, add it.
            _AddSubclassesToArray(pArray, pNode);
        }
    }
}

//
// Returns the root node in the class tree, so you can enumerate everything from there.
//
const SCIClassBrowserNode *SCIClassBrowser::GetRoot(size_t i) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    assert(_fCBLocked);
    SCIClassBrowserNode *pRootBrowserInfo = nullptr;
    if (IsBrowseInfoEnabled())
    {
        if (i < strRootNames.size())
        {
            pRootBrowserInfo = lookup_ptr(_classMap, strRootNames[i]);
        }
    }
    return pRootBrowserInfo;
}

size_t SCIClassBrowser::GetNumRoots() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    assert(_fCBLocked);
    return IsBrowseInfoEnabled() ? strRootNames.size() : 0;
}

const Script *SCIClassBrowser::GetLKGScript(std::string fullPath)
{
    std::transform(fullPath.begin(), fullPath.end(), fullPath.begin(), ::tolower);
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    assert(_fCBLocked);
    const Script *pScript = nullptr;
    if (IsBrowseInfoEnabled())
    {
        word_map::iterator filenameIt = _filenameToScriptNumber.find(fullPath);
        if (filenameIt != _filenameToScriptNumber.end())
        {
            pScript = GetLKGScript(filenameIt->second);
        }
    }
    return pScript;
}

std::string SCIClassBrowser::GetRoomClassName()
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    assert(_fCBLocked);
    return _roomClassName;
}

const Script *SCIClassBrowser::GetLKGScript(WORD wScriptNumber)
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    assert(_fCBLocked);
    const Script *pScriptLKG = nullptr;
    if (IsBrowseInfoEnabled())
    {
        if (wScriptNumber != InvalidResourceNumber)
        {
            if (_pLKGScript && (wScriptNumber == _wLKG))
            {
                // We cached this...
                pScriptLKG = _pLKGScript;
            }
            else
            {
                for (auto &script : _scripts)
                {
                    if (GetScriptNumberHelper(script.get()) == wScriptNumber)
                    {
                        pScriptLKG = script.get();
                        _pLKGScript = pScriptLKG;
                        _wLKG = wScriptNumber;
                        break;
                    }
                }
            }
        }
    }
    return pScriptLKG;
}

const VariableDeclVector *SCIClassBrowser::_GetMainGlobals() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    const VariableDeclVector *pArray = nullptr;
    if (IsBrowseInfoEnabled())
    {
        if (!_scripts.empty() && _scripts[0]) // main is 0
        {
            pArray = &_scripts[0]->GetScriptVariables();
        }
    }
    return pArray;
}

const VariableDeclVector *SCIClassBrowser::GetMainGlobals() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    assert(_fCBLocked);
    return _GetMainGlobals();
}

const std::vector<std::string> &SCIClassBrowser::GetKernelNames() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    assert(_fCBLocked);
    return _kernelNames;
}

const RawProcedureVector &SCIClassBrowser::_GetPublicProcedures()
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    if (!_fPublicProceduresValid)
    {
        _publicProcedures.clear();
        if (IsBrowseInfoEnabled())
        {
            for (auto &script : _scripts)
            {
                for (auto &proc : script->GetProcedures())
                {
                    if (proc->IsPublic())
                    {
                        this->_publicProcedures.push_back(proc.get());
                    }
                }
            }
            _fPublicProceduresValid = true;
        }
    }
    return _publicProcedures;
}

const RawProcedureVector &SCIClassBrowser::GetPublicProcedures()
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    assert(_fCBLocked);
    return _GetPublicProcedures();
}


const std::vector<ClassDefinition*> &SCIClassBrowser::GetAllClasses()
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    assert(_fCBLocked);
    if (!_fPublicClassesValid)
    {
        _allClasses.clear();
        if (IsBrowseInfoEnabled())
        {
            for (auto &script : _scripts)
            {
                for (auto &classDef : script->GetClasses())
                {
                    if (!classDef->IsInstance())
                    {
                        _allClasses.push_back(classDef.get());
                    }
                }
            }
            _fPublicClassesValid = true;
        }
    }
    return _allClasses;
}

const std::vector<sci::Script*> &SCIClassBrowser::GetHeaders()
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser);
    assert(_fCBLocked);
    // REVIEW: why are we doing this every time?
    // Regenerate the _headers array.
    _headers.clear();
    for (auto &headerPair : _headerMap)
    {
        _headers.push_back(headerPair.second.get());
    }
    return _headers;
}

const SelectorTable &SCIClassBrowser::GetSelectorNames()
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    assert(_fCBLocked);
    return _selectorNames;
}

//
// Returns true if pszClass is pszSuper, or a subclass of pszSuper
//
bool SCIClassBrowser::IsSubClassOf(PCTSTR pszClass, PCTSTR pszSuper)
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    assert(_fCBLocked);
    bool fRet = false;
    SCIClassBrowserNode *pNode = nullptr;
    int iOverflow = 100;
    std::string className = pszClass;
    while (!fRet && (pNode = lookup_ptr(_classMap, className)) && (iOverflow--)) // in case of infinite loop?
    {
        if (pNode->GetName() == pszSuper)
        {
            fRet = true;
        }
        else
        {
            pszClass = pNode->GetSuperClass() ? pNode->GetSuperClass()->GetName().c_str() : nullptr;
            if (pszClass == nullptr)
            {
                break;
            }
            className = pszClass;
        }
    }
    return fRet;
}


//
// Helper that attempts to get a number value for the script number.
// A script may define the scriptnumber in terms of a define.
// e.g.
// (scriptnumber MAIN_SCRIPT)
// This will attempt to resolve MAIN_SCRIPT to a number.
//
WORD SCIClassBrowser::GetScriptNumberHelper(Script *pScript) const
{
    WORD w = InvalidResourceNumber;
    if (pScript->Language() == LangSyntaxCpp)
    {
        appState->GetResourceMap().GetScriptNumber(pScript->GetScriptId(), w);
    }
    else
    {
        w = GetScriptNumberHelperConst(pScript);
        if (w != InvalidResourceNumber)
        {
            // Cache it in the script for future use.
            pScript->SetScriptNumber(w);
        }
        // This could be a header file, in which case we don't have a script number.
        // assert(w != InvalidResourceNumber);
    }
    return w;
}

WORD SCIClassBrowser::GetScriptNumberHelperConst(const Script *pScript) const
{
    WORD w = pScript->GetScriptNumber();
    if (w == InvalidResourceNumber)
    {
        const std::string &strDefine = pScript->GetScriptNumberDefine();
        if (!strDefine.empty())
        {
            // Look it up.
            PropertyValue Value;
            if (ResolveValue(pScript, strDefine, Value))
            {
                w = Value.GetNumberValue();
            }
        }
    }
    return w;
}

//
// Similar to GetPropertyValue, but doesn't resolve the value.
//
bool SCIClassBrowser::GetProperty(PCTSTR pszName, const ClassDefinition *pClass, PropertyValue &value)
{
    bool fRet = false;
    fRet = pClass->GetPropertyConst(pszName, value);
    if (!fRet)
    {
        // Ask the super
        SCIClassBrowserNode *pNode = lookup_ptr(_classMap, pClass->GetSuperClass());
        if (pNode && pNode->GetClassDefinition())
        {
            fRet = GetProperty(pszName, pNode->GetClassDefinition(), value);
        }
    }
    return fRet;    
}

//
// Gets a numeric property from a class.  Returns true if found.
// Check the superclass via GetProperty
//
bool  SCIClassBrowser::GetPropertyValue(PCTSTR pszName, const ClassDefinition *pClass, WORD *pw)
{
    *pw = 0;
    bool fRet = false;
    const Script *pScript = pClass->GetOwnerScript();
    PropertyValue value;
    fRet = pClass->GetPropertyConst(pszName, value);
    if (fRet)
    {
        // Found!
        if (value.GetStringValue().empty())
        {
            fRet = (value.GetType() == ValueType::Number);
            *pw = value.GetNumberValue();
        }
        else
        {
            PropertyValue valueResolved;
            fRet = ResolveValue(pScript, value.GetStringValue().c_str(), valueResolved);
            if (fRet)
            {
                *pw = valueResolved.GetNumberValue();
            }
        }
    }
    else
    {
        // Ask the super
        SCIClassBrowserNode *pNode = lookup_ptr(_classMap, pClass->GetSuperClass());
        if (pNode && pNode->GetClassDefinition())
        {
            fRet = GetPropertyValue(pszName, pNode->GetClassDefinition(), pw);
        }
    }
    return fRet;
}


//
// Gets a numeric property from a class.  Returns true if found.
// It does not check the superclass.
//
bool SCIClassBrowser::GetPropertyValue(PCTSTR pszName, ISCIPropertyBag *pBag, const ClassDefinition *pClass, WORD *pw)
{
    *pw = 0;
    PropertyValue value;
    bool fRet = pBag->GetProperty(pszName, value);
    if (fRet)
    {
        if (value.GetStringValue().empty())
        {
            *pw = value.GetNumberValue();
        }
        else
        {
            const Script *pScript = pClass->GetOwnerScript();
            PropertyValue valueResolved;
            fRet = ResolveValue(pScript, value.GetStringValue().c_str(), valueResolved);
            if (fRet)
            {
                *pw = valueResolved.GetNumberValue();
            }
        }        
    }
    return fRet;
}

void SCIClassBrowser::ReportResult(const CompileResult &result)
{
    std::lock_guard<std::mutex> lock(_mutexErrorReport);
    _errors.push_back(result);
}
void SCIClassBrowser::ClearErrors()
{
    std::lock_guard<std::mutex> lock(_mutexErrorReport);
    _errors.clear();
}

// Passes back a copy...
std::vector<CompileResult> SCIClassBrowser::GetErrors()
{
    std::lock_guard<std::mutex> lock(_mutexErrorReport);
    return _errors;
}

int SCIClassBrowser::HasErrors()
{
    int iHasErrors = -1; // Unknown
    // Don't want to block if we're just asking if errors are around
    if (_mutexErrorReport.try_lock())
    {
        iHasErrors = _errors.empty() ? 0 : 1;
        _mutexErrorReport.unlock();
    }
    return iHasErrors;
}


bool SCIClassBrowser::ResolveValue(const Script *pScript, const std::string &strValue, PropertyValue &Out) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    bool fFound = false;
    if (!strValue.empty())
    {
        // One special case
        if (strValue == "scriptNumber")
        {
            Out.SetValue(GetScriptNumberHelperConst(pScript));
            fFound = true;
        }
        else
        {
            const DefineVector &defines = pScript->GetDefines();
            auto it = find_if(defines.begin(), defines.end(), [&strValue](const std::unique_ptr<sci::Define> &pDefine) { return strValue == pDefine->GetLabel(); });
            if (it != defines.end())
            {
                // found it
                Out.SetValue((*it)->GetValue());
                fFound = true;
            }
            if (!fFound)
            {
                DefineValueCache dvc;
                if (lookup_item(_headerDefines, strValue, dvc))
                {
                    WORD wValue = dvc.value;
                    fFound = true;
                    Out.SetValue(wValue);
                }
            }
        }
    }
    return fFound;
}

//
// Given a value:
//      if it's a string, it will look in the current script to see if it matches and defines,
//      and then in the headers appropriate for that script. TODO: uses all headers now.
//
void SCIClassBrowser::ResolveValue(WORD wScript, const PropertyValue &In, PropertyValue &Out)
{
    std::lock_guard<std::recursive_mutex> lock(_mutexClassBrowser); 
    assert(_fCBLocked);
    const Script *pScript = GetLKGScript(wScript);
    
    // Look through this script's defines first.
    std::string strValue = In.GetStringValue();
    bool fFound = ResolveValue(pScript, strValue.c_str(), Out);
    if (!fFound)
    {
        Out = In;
    }
}

