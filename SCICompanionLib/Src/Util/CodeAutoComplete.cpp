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

bool operator<(const AutoCompleteChoice &one, const AutoCompleteChoice &two)
{
    return one.GetText() < two.GetText();
}

template<typename _TCollection, typename _TNameFunc>
void MergeResults(std::vector<AutoCompleteChoice> &existingResults, const std::string &prefix, int icon, _TCollection &items, _TNameFunc nameFunc)
{
    std::vector<AutoCompleteChoice> newResults;
    for (auto &item : items)
    {
        std::string name = nameFunc(item);
        // TODO: support case insensitive?
        if (0 == name.compare(0, prefix.size(), prefix))
        {
            newResults.emplace_back(name.c_str(), icon);
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

void MergeResults(std::vector<AutoCompleteChoice> &existingResults, const std::string &prefix, int icon, const std::vector<std::string> &items)
{
    return MergeResults(existingResults, prefix, icon, items,
        [](const std::string text) { return text; });
}

std::unique_ptr<AutoCompleteResult> GetAutoCompleteResult(const std::string &prefix, SyntaxContext *pContext)
{
    std::unique_ptr<AutoCompleteResult> result = std::make_unique<AutoCompleteResult>();
    if (pContext && !prefix.empty())
    {
        ParseAutoCompleteContext acContext = pContext->GetParseAutoCompleteContext();
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
                sourceTypes |= AutoCompleteSourceType::ClassName | AutoCompleteSourceType::Variable | AutoCompleteSourceType::Define | AutoCompleteSourceType::Kernel | AutoCompleteSourceType::Procedure | AutoCompleteSourceType::ClassSelector;
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

        SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
        if (sourceTypes != AutoCompleteSourceType::None)
        {
            browser.GetAutoCompleteChoices(prefix, sourceTypes, result->choices);
        }

        // Now get things from the local script:
        if (IsFlagSet(sourceTypes, AutoCompleteSourceType::Variable))
        {
            ClassBrowserLock browserLock(browser);
            browserLock.Lock();
            const Script *thisScript = browser.GetLKGScript(pContext->Script().GetScriptNumber());
            if (thisScript)
            {
                // Script variables
                MergeResults(result->choices, prefix, 5, thisScript->GetScriptVariables(),
                    [](const std::unique_ptr<VariableDecl> &theVar)
                {
                    return theVar->GetName();
                }
                );
                // Script strings
                MergeResults(result->choices, prefix, 5, thisScript->GetScriptStringsDeclarations(),
                    [](const std::unique_ptr<VariableDecl> &theVar)
                {
                    return theVar->GetName();
                }
                );
                if (pContext->FunctionPtr)
                {
                    MergeResults(result->choices, prefix, 5, pContext->FunctionPtr->GetVariables(),
                        [](const std::unique_ptr<VariableDecl> &theVar)
                    {
                        return theVar->GetName();
                    });
                    if (!pContext->FunctionPtr->GetSignatures().empty())
                    {
                        MergeResults(result->choices, prefix, 5, pContext->FunctionPtr->GetSignatures()[0]->GetParams(),
                            [](const std::unique_ptr<FunctionParameter> &theVar)
                        {
                            return theVar->GetName();
                        });
                    }
                }
            }
        }

        // Property selectors for the *current* class
        if (IsFlagSet(sourceTypes, AutoCompleteSourceType::ClassSelector) && pContext->ClassPtr)
        {
            ClassBrowserLock browserLock(browser);
            browserLock.Lock();
            std::string species = pContext->ClassPtr->GetSpecies();
            unique_ptr<RawClassPropertyVector> properties(browser.CreatePropertyArray(species, nullptr, nullptr));
            MergeResults(result->choices, prefix, 5, *properties,
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
            const Script *thisScript = browser.GetLKGScript(pContext->Script().GetScriptNumber());
            if (thisScript)
            {
                MergeResults(result->choices, prefix, 6, thisScript->GetDefines(),
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
            const Script *thisScript = browser.GetLKGScript(pContext->Script().GetScriptNumber());
            if (thisScript)
            {
                MergeResults(result->choices, prefix, 2, thisScript->GetProcedures(),
                    [](const std::unique_ptr<ProcedureDefinition> &proc)
                {
                    return proc->GetName();
                }
                    );
                MergeResults(result->choices, prefix, 2, thisScript->GetClasses(),
                    [](const std::unique_ptr<ClassDefinition> &proc)
                {
                    return proc->IsInstance() ? proc->GetName() : "";   // Classes can't be exports.
                }
                );
            }
        }

        LangSyntax lang = pContext->Script().Language();
        if (acContext == ParseAutoCompleteContext::TopLevelKeyword)
        {
            MergeResults(result->choices, prefix, 8, GetTopLevelKeywords(lang));
        }
        if (acContext == ParseAutoCompleteContext::ClassLevelKeyword)
        {
            MergeResults(result->choices, prefix, 8, GetClassLevelKeywords(lang));
        }
        if (acContext == ParseAutoCompleteContext::Value)
        {
            MergeResults(result->choices, prefix, 8, GetCodeLevelKeywords(lang));
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
    _pThread = AfxBeginThread(s_ThreadWorker, this, 0, 0, 0, nullptr);
}

AutoCompleteThread2::~AutoCompleteThread2()
{
    CloseHandle(_hStartWork);
    CloseHandle(_hWaitForMoreWork);

    SetEvent(_hExitThread);
    WaitForSingleObject(_pThread->m_hThread, INFINITE);

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
    OutputDebugString(fmt::format("last point is {0} {1}\n", _lastPoint.x, _lastPoint.y).c_str());

    // Take note if the background thread is potentially ready to continue parsing from where it left off.
    bool bgIsWaiting;
    {
        CGuard guard(&_cs);
        // It's ready if it hasn't begun yet (_limiterPending), or if the current parse hasn't been canceled.
        bgIsWaiting = _limiterPending || !_fCancelCurrentParse;
    }

    if (bgIsWaiting && (_lastHWND == hwnd) && (pt.y == _lastPoint.y) && (pt.x > _lastPoint.x))
    {
        OutputDebugString("UI: Continuing existing parse\n");

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
        OutputDebugString("UI: Starting new parse\n");

        // Start a new one
        // 1) If in parse, set the event getting out of it and indicate cancel.
        {
            CGuard guard(&_cs);
            _additionalCharacters = "";
            _fCancelCurrentParse = true;
            _idUpdate = _nextId;
            OutputDebugString("UI: Sending message to cancel current parse\n");

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

    OutputDebugString(fmt::format("last point is {0} {1}\n", _lastPoint.x, _lastPoint.y).c_str());
}

void AutoCompleteThread2::ResetPosition()
{
    OutputDebugString("UI: ResetPosition\n");

    _lastHWND = nullptr;
    _lastPoint = CPoint();

    OutputDebugString(fmt::format("last point is {0} {1}\n", _lastPoint.x, _lastPoint.y).c_str());

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
                    OutputDebugString(fmt::format("ACThread: Callback for {0}, top statement is {1}. Set result and block\n", word.c_str(), (int)_context.GetTopKnownNode()).c_str());
                    OutputDebugString("\n");

                    // Figure out the result
                    std::unique_ptr<AutoCompleteResult> result = GetAutoCompleteResult(word, &_context);
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
                            OutputDebugString(continueParsing ? "Continuing parsing\n" : "Abandoning and exiting parse\n");
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
            };

            sci::Script script;
            CCrystalScriptStream::const_iterator it(limiter.get());
            SyntaxContext context(it, script, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), false);
            context.ParseDebug = true; // phil temp

            AutoCompleteParseCallback callback(context, *this, *limiter, id);
            limiter->SetCallback(&callback);

            // context.ForAutoComplete = true;
            OutputDebugString("Starting parsing session\n");
            bool result = g_Parser.ParseAC(script, it, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &context);
            OutputDebugString("Ending parsing session\n");
        }
    }
    OutputDebugString("Exiting AC thread\n");
}
