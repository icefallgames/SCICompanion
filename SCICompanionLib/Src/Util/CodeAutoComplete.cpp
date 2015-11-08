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
#include "SCIStudioSyntaxParser.h"
#include "CodeAutoComplete.h"
#include "AppState.h"
#include "SyntaxParser.h"
#include "format.h"
using namespace sci;
using namespace std;

AutoCompleteChoice::AutoCompleteChoice() { _iIcon = AutoCompleteIconIndex::Unknown; }
AutoCompleteChoice::AutoCompleteChoice(const std::string &text, AutoCompleteIconIndex iIcon)
{
    _strLower = text;
    std::transform(_strLower.begin(), _strLower.end(), _strLower.begin(), ::tolower);
    _strText = text;
    _iIcon = iIcon;
}
AutoCompleteChoice::AutoCompleteChoice(const std::string &text, const std::string &lower, AutoCompleteIconIndex iIcon) { _strText = text; _strLower = lower, _iIcon = iIcon; }
const std::string &AutoCompleteChoice::GetText() const { return _strText; }
const std::string &AutoCompleteChoice::GetLower() const { return _strLower; }
AutoCompleteIconIndex AutoCompleteChoice::GetIcon() const { return _iIcon; }

bool operator<(const AutoCompleteChoice &one, const AutoCompleteChoice &two)
{
    return one.GetLower() < two.GetLower();
}

template<typename _TCollection, typename _TNameFunc>
void MergeResults(std::vector<AutoCompleteChoice> &existingResults, const std::string &prefixLower, AutoCompleteIconIndex icon, _TCollection &items, _TNameFunc nameFunc)
{
    std::vector<AutoCompleteChoice> newResults;
    for (auto &item : items)
    {
        std::string name = nameFunc(item);
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (0 == lower.compare(0, prefixLower.size(), prefixLower))
        {
            newResults.emplace_back(name, lower, icon);
        }
    }
    if (!newResults.empty())
    {
        std::sort(newResults.begin(), newResults.end());
        std::vector<AutoCompleteChoice> tempResults;
        tempResults.reserve(newResults.size() + existingResults.size());
        std::merge(existingResults.begin(), existingResults.end(), newResults.begin(), newResults.end(), back_inserter(tempResults));
        std::swap(tempResults, existingResults);
    }
}

void MergeResults(std::vector<AutoCompleteChoice> &existingResults, const std::string &prefix, AutoCompleteIconIndex icon, const std::vector<std::string> &items)
{
    return MergeResults(existingResults, prefix, icon, items,
        [](const std::string text) { return text; });
}

std::unique_ptr<AutoCompleteResult> GetAutoCompleteResult(const std::string &prefixIn, uint16_t scriptNumber, SyntaxContext &context, std::unordered_set<std::string> &parsedCustomHeaders)
{
    // Use the scriptNumber provided instead of that in the SyntaxContext's script, because we may not have reached the scriptNumber declaration in the script yet.

    std::string prefix = prefixIn;
    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

    //OutputDebugString("Prefix is ");
    //OutputDebugString(prefix.c_str());
    //OutputDebugString("\n");

    std::unique_ptr<AutoCompleteResult> result = std::make_unique<AutoCompleteResult>();
    if (!prefix.empty())
    {
        ParseAutoCompleteContext acContext = context.GetParseAutoCompleteContext();


        //OutputDebugString(fmt::format("ParseContext: {}\n", (int)acContext).c_str());

        AutoCompleteSourceType sourceTypes = AutoCompleteSourceType::None;
        switch (acContext)
        {
            case ParseAutoCompleteContext::Selector:
                sourceTypes |= AutoCompleteSourceType::Selector;
                break;

            case ParseAutoCompleteContext::ClassSelector:
                sourceTypes |= AutoCompleteSourceType::ClassSelector;
                break;

            case ParseAutoCompleteContext::TopLevelKeyword:
                sourceTypes |= AutoCompleteSourceType::TopLevelKeyword;
                break;

            case ParseAutoCompleteContext::Value:
                sourceTypes |= AutoCompleteSourceType::ClassName | AutoCompleteSourceType::Variable | AutoCompleteSourceType::Define | AutoCompleteSourceType::Kernel | AutoCompleteSourceType::Procedure | AutoCompleteSourceType::ClassSelector | AutoCompleteSourceType::Instance;
                break;

            case ParseAutoCompleteContext::LValue:
                sourceTypes |= AutoCompleteSourceType::Variable | AutoCompleteSourceType::ClassSelector;
                break;

            case ParseAutoCompleteContext::DefineValue:
                sourceTypes |= AutoCompleteSourceType::Define;
                break;

            case ParseAutoCompleteContext::SuperClass:
                sourceTypes |= AutoCompleteSourceType::ClassName;
                break;

            case ParseAutoCompleteContext::ScriptName:
                sourceTypes |= AutoCompleteSourceType::ScriptName;
                break;

            default:
                // Other things handled below
                break;
        }

        // Get things from the big global list
        SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
        if (sourceTypes != AutoCompleteSourceType::None)
        {
            browser.GetAutoCompleteChoices(prefix, sourceTypes, result->choices);
        }

        // Now get things from the local script
        // First, ensure any headers that we've encountered are parsed
        for (const string &include : context.Script().GetIncludes())
        {
            if (parsedCustomHeaders.find(include) == parsedCustomHeaders.end())
            {
                // Tell the class browser to pull this in. This could take some time, but the class browser
                // isn't locked during most of this time.
                browser.TriggerCustomIncludeCompile(include);
                parsedCustomHeaders.insert(include);
            }
        }

        // Grab variables and defines from included headers
        if (IsFlagSet(sourceTypes, AutoCompleteSourceType::Variable | AutoCompleteSourceType::Define))
        {
            ClassBrowserLock browserLock(browser);
            browserLock.Lock();
            for (const std::string &headerName : parsedCustomHeaders)
            {
                sci::Script *headerScript = browser.GetCustomHeader(headerName);
                if (headerScript)
                {
                    if (IsFlagSet(sourceTypes, AutoCompleteSourceType::Variable))
                    {
                        MergeResults(result->choices, prefix, AutoCompleteIconIndex::Variable, headerScript->GetScriptVariables(),
                            [](const std::unique_ptr<VariableDecl> &theVar)
                        {
                            return theVar->GetName();
                        }
                        );
                    }
                    if (IsFlagSet(sourceTypes, AutoCompleteSourceType::Define))
                    {
                        MergeResults(result->choices, prefix, AutoCompleteIconIndex::Define, headerScript->GetDefines(),
                            [](const std::unique_ptr<Define> &theDefine)
                        {
                            return theDefine->GetName();
                        }
                        );
                    }
                }
            }
        }

        if (IsFlagSet(sourceTypes, AutoCompleteSourceType::Instance))
        {
            // Instances in this script
            ClassBrowserLock browserLock(browser);
            browserLock.Lock();
            const Script *thisScript = browser.GetLKGScript(scriptNumber);
            if (thisScript)
            {
                std::vector<std::string> instanceNames;
                for (auto &theClass : thisScript->GetClasses())
                {
                    if (theClass->IsInstance())
                    {
                        instanceNames.push_back(theClass->GetName());
                    }

                }
                MergeResults(result->choices, prefix, AutoCompleteIconIndex::Class, instanceNames);
            }
        }

        if (IsFlagSet(sourceTypes, AutoCompleteSourceType::Variable))
        {
            ClassBrowserLock browserLock(browser);
            browserLock.Lock();
            const Script *thisScript = browser.GetLKGScript(scriptNumber);
            if (thisScript)
            {
                // Script variables
                MergeResults(result->choices, prefix, AutoCompleteIconIndex::Variable, thisScript->GetScriptVariables(),
                    [](const std::unique_ptr<VariableDecl> &theVar)
                {
                    return theVar->GetName();
                }
                );
                // Script strings
                MergeResults(result->choices, prefix, AutoCompleteIconIndex::Variable, thisScript->GetScriptStringsDeclarations(),
                    [](const std::unique_ptr<VariableDecl> &theVar)
                {
                    return theVar->GetName();
                }
                );
                if (context.FunctionPtr)
                {
                    MergeResults(result->choices, prefix, AutoCompleteIconIndex::Variable, context.FunctionPtr->GetVariables(),
                        [](const std::unique_ptr<VariableDecl> &theVar)
                    {
                        return theVar->GetName();
                    });
                    if (!context.FunctionPtr->GetSignatures().empty())
                    {
                        MergeResults(result->choices, prefix, AutoCompleteIconIndex::Variable, context.FunctionPtr->GetSignatures()[0]->GetParams(),
                            [](const std::unique_ptr<FunctionParameter> &theVar)
                        {
                            return theVar->GetName();
                        });
                    }
                }
            }
        }

        // Property selectors for the *current* class
        if (IsFlagSet(sourceTypes, AutoCompleteSourceType::ClassSelector) && context.ClassPtr)
        {
            ClassBrowserLock browserLock(browser);
            browserLock.Lock();
            std::string species = context.ClassPtr->IsInstance() ? context.ClassPtr->GetSuperClass() : context.ClassPtr->GetName();
            auto properties = browser.CreatePropertyArray(species);
            MergeResults(result->choices, prefix, AutoCompleteIconIndex::Variable, *properties,
                [](const ClassProperty *classProp)
            {
                return classProp->GetName();
            }
            );
        }

        if (IsFlagSet(sourceTypes, AutoCompleteSourceType::Define))
        {
            // Local script defines
            ClassBrowserLock browserLock(browser);
            browserLock.Lock();
            const Script *thisScript = browser.GetLKGScript(scriptNumber);
            if (thisScript)
            {
                MergeResults(result->choices, prefix, AutoCompleteIconIndex::Define, thisScript->GetDefines(),
                    [](const std::unique_ptr<Define> &theDefine)
                {
                    return theDefine->GetName();
                }
                );
            }
        }

        if (acContext == ParseAutoCompleteContext::Export)
        {
            ClassBrowserLock browserLock(browser);
            browserLock.Lock();
            const Script *thisScript = browser.GetLKGScript(scriptNumber);
            if (thisScript)
            {
                MergeResults(result->choices, prefix, AutoCompleteIconIndex::Procedure, thisScript->GetProcedures(),
                    [](const std::unique_ptr<ProcedureDefinition> &proc)
                {
                    return proc->GetName();
                }
                    );
                MergeResults(result->choices, prefix, AutoCompleteIconIndex::Procedure, thisScript->GetClasses(),
                    [](const std::unique_ptr<ClassDefinition> &proc)
                {
                    return proc->IsInstance() ? proc->GetName() : "";   // Classes can't be exports.
                }
                );
            }
        }

        LangSyntax lang = context.Script().Language();
        if (acContext == ParseAutoCompleteContext::TopLevelKeyword)
        {
            MergeResults(result->choices, prefix, AutoCompleteIconIndex::Keyword, GetTopLevelKeywords(lang));
        }
        if (acContext == ParseAutoCompleteContext::ClassLevelKeyword)
        {
            MergeResults(result->choices, prefix, AutoCompleteIconIndex::Keyword, GetClassLevelKeywords(lang));
        }
        if (acContext == ParseAutoCompleteContext::Value)
        {
            MergeResults(result->choices, prefix, AutoCompleteIconIndex::Keyword, GetCodeLevelKeywords(lang));
        }

        // Possible de-dupe
        // vec.erase(unique(vec.begin(), vec.end()), vec.end());

        result->fResultsChanged = true; // TODO
    }
    return result;
}
AutoCompleteThread2::AutoCompleteThread2() : _nextId(0), _instruction(AutoCompleteInstruction::None), _bgStatus(AutoCompleteStatus::Pending)
{
    _thread = std::thread(s_ThreadWorker, this);
}

AutoCompleteThread2::~AutoCompleteThread2()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _instruction = AutoCompleteInstruction::Abort;
    }
    _condition.notify_one();
    _thread.join();
}

void AutoCompleteThread2::InitializeForScript(CCrystalTextBuffer *buffer)
{
    _bufferUI = buffer;

    // TODO: Cancel any parsing? Or I guess it really doesn't matter. Except that if a script is closed, we want to know, so we don't send message to non-existent hwnd.
}

#define EXTRA_AC_CHARS 100

void AutoCompleteThread2::StartAutoComplete(CPoint pt, HWND hwnd, UINT message, uint16_t scriptNumber)
{
    // Take note if the background thread is potentially ready to continue parsing from where it left off.
    bool bgIsWaiting;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        bgIsWaiting = (_bgStatus == AutoCompleteStatus::WaitingForMore) || (_bgStatus == AutoCompleteStatus::Parsing);
    }

    if (bgIsWaiting && (_lastHWND == hwnd) && (pt.y == _lastPoint.y) && (pt.x > _lastPoint.x))
    {
        // Continue an existing parse.
        CString strText;
        _bufferUI->GetText(_lastPoint.y, _lastPoint.x, pt.y, pt.x, strText);

        {
            std::lock_guard<std::mutex> lock(_mutex);
            _additionalCharacters += (PCSTR)strText;

            //OutputDebugString(fmt::format("UI: Continue parse with {0}\n", (PCSTR)strText).c_str());

            _instruction = AutoCompleteInstruction::Continue;
        }
        _condition.notify_one();
    }
    else
    {
        // Now start a fresh parse
        // Make a copy of the text buffer
        std::unique_ptr<CScriptStreamLimiter> limiter = std::make_unique<CScriptStreamLimiter>(_bufferUI, pt, EXTRA_AC_CHARS);
        //limiter->Limit(LineCol(pt.y, pt.x));
        std::unique_ptr<CCrystalScriptStream> stream = std::make_unique<CCrystalScriptStream>(limiter.get());

        //OutputDebugString(fmt::format("UI: Start new parse at {0},{1}\n", pt.x, pt.y).c_str());

        // Give the work to the background thread
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _additionalCharacters.clear();
            _limiterPending = move(limiter);
            _streamPending = move(stream);
            _scriptNumberPending = scriptNumber;
            _id.hwnd = hwnd;
            _id.id = _nextId;
            _id.message = message;
            _instruction = AutoCompleteInstruction::Restart;
        }

        _condition.notify_one();
    }

    _nextId++;
    _lastHWND = hwnd;
    _lastPoint = pt;
}

void AutoCompleteThread2::ResetPosition()
{
    _lastHWND = nullptr;
    _lastPoint = CPoint();

    {
        std::lock_guard<std::mutex> lock(_mutex);

        _limiterPending.reset(nullptr);
        _streamPending.reset(nullptr);
        _id.hwnd = nullptr;
        _id.id = -1;
        _additionalCharacters = "";
        _instruction = AutoCompleteInstruction::Restart;
    }
    _condition.notify_one();
}

CPoint AutoCompleteThread2::GetCompletedPosition()
{
    return _lastPoint;
}

std::unique_ptr<AutoCompleteResult> AutoCompleteThread2::GetResult(int id)
{
    std::unique_ptr<AutoCompleteResult> result;

    {
        std::lock_guard<std::mutex> lock(_mutexResult);
        if (_resultId == id)
        {
            result = move(_result);
        }
    }

    return result;
}

UINT AutoCompleteThread2::s_ThreadWorker(void *pParam)
{
    (reinterpret_cast<AutoCompleteThread2*>(pParam))->_DoWork();
    return 0;
}

void AutoCompleteThread2::_SetResult(std::unique_ptr<AutoCompleteResult> result, AutoCompleteThread2::AutoCompleteId id)
{
    {
        std::lock_guard<std::mutex> lock(_mutexResult);
        _resultId = id.id;
        _result = move(result);
    }
    SendMessage(id.hwnd, id.message, id.id, 0);
}

void AutoCompleteThread2::_DoWork()
{
    while (_instruction != AutoCompleteInstruction::Abort)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_instruction == AutoCompleteInstruction::Abort)
        {
            // Important to ask this in the lock.
            break;
        }
        _condition.wait(lock, [&]() { return this->_instruction != AutoCompleteInstruction::None; });
        while (_instruction == AutoCompleteInstruction::Restart)
        {
            _instruction = AutoCompleteInstruction::None;

            std::unique_ptr<CScriptStreamLimiter> limiter = move(_limiterPending);
            std::unique_ptr<CCrystalScriptStream> stream = move(_streamPending);
            _bgStatus = AutoCompleteStatus::Parsing;
            if (!this->_additionalCharacters.empty())
            {
                // It's possible the UI thread saw we were parsing, and so it
                // just placed some additional characters in the buffer. But we then
                // failed to parse, so we're restarting. There's no way that we'll
                // succeed parsing though, so let's jus fail
                limiter = nullptr;
                stream = nullptr;
                _bgStatus = AutoCompleteStatus::Pending;
            }

            uint16_t scriptNumber = _scriptNumberPending;
            AutoCompleteId id = _id;
            // Unlock before we do expensive stuff
            lock.unlock();

            if (limiter)
            {
                assert(id.hwnd);
                class AutoCompleteParseCallback : public ISyntaxParserCallback
                {
                public:
                    AutoCompleteParseCallback(uint16_t scriptNumber, SyntaxContext &context, AutoCompleteThread2 &ac, CScriptStreamLimiter &limiter, AutoCompleteId id) : _context(context), _id(id), _ac(ac), _limiter(limiter), _scriptNumber(scriptNumber) {}

                    bool Done()
                    {
                        std::string word = _limiter.GetLastWord();

                        // Figure out the result
                        std::unique_ptr<AutoCompleteResult> result = GetAutoCompleteResult(word, _scriptNumber, _context, _parsedCustomHeaders);
                        result->OriginalLimit = _limiter.GetLimit();
                        result->OriginalLimit.x -= word.length();
                        result->OriginalLimit.x = max(result->OriginalLimit.x, 0);
                        _ac._SetResult(move(result), _id);

                        std::unique_lock<std::mutex> lock(_ac._mutex);
                        _ac._bgStatus = AutoCompleteStatus::WaitingForMore;
                        _ac._condition.wait(lock, [&]() { return this->_ac._instruction != AutoCompleteInstruction::None; });
                        // There is a small race condition between here....
                        bool continueParsing = (_ac._instruction == AutoCompleteInstruction::Continue);
                        // We need to set this to none now that we've received the orders:
                        _ac._instruction = AutoCompleteInstruction::None;
                        std::string additionalCharacters;
                        if (continueParsing)
                        {
                            _ac._bgStatus = AutoCompleteStatus::Parsing;
                            additionalCharacters = _ac._additionalCharacters;
                            _ac._additionalCharacters = "";
                            _limiter.Extend(additionalCharacters);

                            //OutputDebugString(fmt::format("BG: Continuing parse with {0}\n", additionalCharacters).c_str());
                        } 
                        else
                        {
                            _ac._bgStatus = AutoCompleteStatus::Pending;
                            //OutputDebugString(fmt::format("BG: Bailing out from parse\n").c_str());
                        }

                        return continueParsing;   // false -> bail
                    }
                private:
                    uint16_t _scriptNumber;
                    SyntaxContext &_context;
                    AutoCompleteId _id;
                    AutoCompleteThread2 &_ac;
                    CScriptStreamLimiter &_limiter;
                    std::unordered_set<std::string> _parsedCustomHeaders;
                };

                sci::Script script;
                CCrystalScriptStream::const_iterator it(limiter.get());
                SyntaxContext context(it, script, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), false);
#ifdef PARSE_DEBUG
                context.ParseDebug = true;
#endif

                AutoCompleteParseCallback callback(scriptNumber, context, *this, *limiter, id);
                limiter->SetCallback(&callback);

                bool result = g_Parser.ParseAC(script, it, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &context);
            }
            else
            {
                break; // Go back to waiting
            }

            lock.lock();
            _bgStatus = AutoCompleteStatus::Pending;
        }
    }
}
