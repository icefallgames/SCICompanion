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

bool operator==(const AutoCompleteChoice &one, const AutoCompleteChoice &two)
{
    return one.GetText() == two.GetText() &&
        one.GetLower() == two.GetLower() &&
        one.GetIcon() == two.GetIcon();
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

std::unique_ptr<AutoCompleteResult> GetAutoCompleteResult(const std::string &prefix, SyntaxContext &context, std::unordered_set<std::string> &parsedCustomHeaders)
{
    std::unique_ptr<AutoCompleteResult> result = std::make_unique<AutoCompleteResult>();
    if (!prefix.empty())
    {
        ParseAutoCompleteContext acContext = context.GetParseAutoCompleteContext();
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
            const Script *thisScript = browser.GetLKGScript(browser.GetScriptNumberHelper(&context.Script()));
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
            const Script *thisScript = browser.GetLKGScript(browser.GetScriptNumberHelper(&context.Script()));
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
            unique_ptr<RawClassPropertyVector> properties(browser.CreatePropertyArray(species, nullptr, nullptr));
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
            const Script *thisScript = browser.GetLKGScript(browser.GetScriptNumberHelper(&context.Script()));
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
            const Script *thisScript = browser.GetLKGScript(browser.GetScriptNumberHelper(&context.Script()));
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
AutoCompleteThread2::AutoCompleteThread2() : _nextId(0)
{
    InitializeCriticalSection(&_cs);
    InitializeCriticalSection(&_csResult);
    _hStartWork = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    _hWaitForMoreWork = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    // ExitThread will be manual reset, since the background thread will wait on it multiple times:
    _hExitThread = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    _pThread = AfxBeginThread(s_ThreadWorker, this, 0, 0, CREATE_SUSPENDED, nullptr);
    if (_pThread)
    {
        _pThread->m_bAutoDelete = FALSE;
        _pThread->ResumeThread();
    }
}

AutoCompleteThread2::~AutoCompleteThread2()
{
    CloseHandle(_hStartWork);
    CloseHandle(_hWaitForMoreWork);

    SetEvent(_hExitThread);
    WaitForSingleObject(_pThread->m_hThread, INFINITE);
    delete _pThread;

    CloseHandle(_hExitThread);

    DeleteCriticalSection(&_cs);
    DeleteCriticalSection(&_csResult);
}

void AutoCompleteThread2::InitializeForScript(CCrystalTextBuffer *buffer)
{
    _bufferUI = buffer;

    // TODO: Cancel any parsing? Or I guess it really doesn't matter. Except that if a script is closed, we want to know, so we don't send message to non-existent hwnd.
}

#define EXTRA_AC_CHARS 100

void AutoCompleteThread2::StartAutoComplete(CPoint pt, HWND hwnd, UINT message)
{
    // Take note if the background thread is potentially ready to continue parsing from where it left off.
    bool bgIsWaiting;
    {
        CGuard guard(&_cs);
        // It's ready if it hasn't begun yet (_limiterPending), or if the current parse hasn't been canceled.
        bgIsWaiting = _limiterPending || !_fCancelCurrentParse;
    }

    if (bgIsWaiting && (_lastHWND == hwnd) && (pt.y == _lastPoint.y) && (pt.x > _lastPoint.x))
    {
        // Continue an existing parse.
        CString strText;
        _bufferUI->GetText(_lastPoint.y, _lastPoint.x, pt.y, pt.x, strText);

        {
            CGuard guard(&_cs);
            _additionalCharacters += (PCSTR)strText;
            SetEvent(_hWaitForMoreWork);
        }
    }
    else
    {
        // Start a new one
        // 1) If in parse, set the event getting out of it and indicate cancel.
        {
            CGuard guard(&_cs);
            _additionalCharacters = "";
            _fCancelCurrentParse = true;
            _idUpdate = _nextId;
            SetEvent(_hWaitForMoreWork);
        }

        // Now start a fresh parse
        // Make a copy of the text buffer
        std::unique_ptr<CScriptStreamLimiter> limiter = std::make_unique<CScriptStreamLimiter>(_bufferUI, pt, EXTRA_AC_CHARS);
        //limiter->Limit(LineCol(pt.y, pt.x));
        std::unique_ptr<CCrystalScriptStream> stream = std::make_unique<CCrystalScriptStream>(limiter.get());

        // Give the work to the background thread
        {
            CGuard guard(&_cs);
            _limiterPending = move(limiter);
            _streamPending = move(stream);
            _id.hwnd = hwnd;
            _id.id = _nextId;
            _id.message = message;
            ResetEvent(_hWaitForMoreWork);
            SetEvent(_hStartWork);
        }
    }

    _nextId++;
    _lastHWND = hwnd;
    _lastPoint = pt;
}

void AutoCompleteThread2::ResetPosition()
{
    _lastHWND = nullptr;
    _lastPoint = CPoint();

    // REVIEW this:
    CGuard guard(&_cs);

    _limiterPending.reset(nullptr);
    _streamPending.reset(nullptr);
    _id.hwnd = nullptr;
    _id.id = -1;

    _additionalCharacters = "";
    _fCancelCurrentParse = true;
    SetEvent(_hWaitForMoreWork);
}

CPoint AutoCompleteThread2::GetCompletedPosition()
{
    return _lastPoint;
}

std::unique_ptr<AutoCompleteResult> AutoCompleteThread2::GetResult(int id)
{
    std::unique_ptr<AutoCompleteResult> result;

    {
        CGuard guard(&_csResult);
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
        CGuard guard(&_csResult);
        _resultId = id.id;
        _result = move(result);
    }
    SendMessage(id.hwnd, id.message, id.id, 0);
}

void AutoCompleteThread2::_DoWork()
{
    HANDLE startWork[2] = { _hStartWork, _hExitThread };
    while (WAIT_OBJECT_0 == WaitForMultipleObjects(ARRAYSIZE(startWork), startWork, FALSE, INFINITE))
    {
        std::unique_ptr<CScriptStreamLimiter> limiter;
        std::unique_ptr<CCrystalScriptStream> stream;
        AutoCompleteId id;

        {
            CGuard guard(&_cs);
            limiter = move(_limiterPending);
            stream = move(_streamPending);
            _fCancelCurrentParse = false;
            id = _id;
        }

        if (limiter)
        {
            assert(id.hwnd);
            class AutoCompleteParseCallback : public ISyntaxParserCallback
            {
            public:
                AutoCompleteParseCallback(SyntaxContext &context, AutoCompleteThread2 &ac, CScriptStreamLimiter &limiter, AutoCompleteId id) : _context(context), _id(id), _ac(ac), _limiter(limiter) {}

                bool Done()
                {
                    std::string word = _limiter.GetLastWord();

                    // Figure out the result
                    std::unique_ptr<AutoCompleteResult> result = GetAutoCompleteResult(word, _context, _parsedCustomHeaders);
                    result->OriginalLimit = _limiter.GetLimit();
                    result->OriginalLimit.x -= word.length();
                    result->OriginalLimit.x = max(result->OriginalLimit.x, 0);
                    _ac._SetResult(move(result), _id);

                    bool continueParsing = false;
                    // Now wait in case we have more letters in sequence (so we don't have to reparse everything).
                    // Now we block waiting for more characters, or to be "reset".
                    HANDLE continueWork[2] = { _ac._hWaitForMoreWork, _ac._hExitThread };
                    DWORD waitResult = WaitForMultipleObjects(ARRAYSIZE(continueWork), continueWork, FALSE, INFINITE);
                    if (WAIT_OBJECT_0 == waitResult)
                    {
                        continueParsing = true;
                        std::string additionalCharacters;
                        {
                            CGuard guard(&_ac._cs);
                            continueParsing = !_ac._fCancelCurrentParse;
                            _id.id = _ac._idUpdate;
                            additionalCharacters = _ac._additionalCharacters;
                            _ac._additionalCharacters = "";
                        }
                        if (continueParsing)
                        {
                            _limiter.Extend(additionalCharacters);
                        }
                    }

                    return continueParsing;   // false -> bail
                }
            private:
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

            AutoCompleteParseCallback callback(context, *this, *limiter, id);
            limiter->SetCallback(&callback);

            // context.ForAutoComplete = true;
            bool result = g_Parser.ParseAC(script, it, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &context);
        }
    }
}
