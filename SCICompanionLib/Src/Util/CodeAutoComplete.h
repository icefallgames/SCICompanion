#pragma once

class CCrystalScriptStream::const_iterator;
class CScriptStreamLimiter;
class SyntaxContext;;

class AutoCompleteChoice
{
public:
    AutoCompleteChoice() { _iIcon = 0; }
    AutoCompleteChoice(PCTSTR psz, int iIcon) { _strText = psz; _iIcon = iIcon; }
    const std::string &GetText() const { return _strText; }
    int GetIcon() const { return _iIcon; }

private:
    std::string _strText;
    int _iIcon;
};

enum ACType
{
    AC_Normal,
    AC_ShowOnFirst_Empty,
    AC_ShowOnFirst_Shown,
};


class AutoCompleteResult
{
public:
    AutoCompleteResult() { Reset(); }
    ~AutoCompleteResult() {}
    void Add(PCTSTR psz, int iIcon) { choices.push_back(AutoCompleteChoice(psz, iIcon)); }
    void AddOnFirstLetter(PCTSTR pszLetters, PCTSTR pszText, int iIcon)
    {
        fACType = pszLetters[0] ? AC_ShowOnFirst_Shown : AC_ShowOnFirst_Empty;
        Add(pszText, iIcon);
    }
    void Clear() { choices.clear(); strMethod.clear(); strParams.clear(); fACType = AC_Normal; }
    void Reset() { dwFailedPos = 0; fResultsChanged = FALSE; Clear(); fACType = AC_Normal; }
    BOOL HasMethodTip() { return !strMethod.empty(); }

    DWORD dwFailedPos;
    BOOL fResultsChanged;
    std::string strFindText;
    std::vector<AutoCompleteChoice> choices;
    ACType fACType;
    
    // method info tips
    std::string strMethod;
    std::vector<std::string> strParams;
};

class SyntaxContext; // fwd decl

AutoCompleteResult GeyAutoCompleteResult(SyntaxContext *pContext);

class AutoCompleteThread
{
public:
    // All public functions should be called by the UI thread only.
    AutoCompleteThread();
    ~AutoCompleteThread();
    void Exit();
    void AutoCompleteThread::InitAutoComplete(CCrystalScriptStream::const_iterator it, CScriptStreamLimiter *pLimit);
    AutoCompleteResult DoAutoComplete(CPoint pt);
    CPoint GetCompletedPosition();
    CPoint GetInsertPosition() { ASSERT(FALSE); return CPoint(0, 0); }
    void ResetPosition();

private:
    static UINT s_ThreadWorker(void *pParam);
    void _DoWork();
    CWinThread *_pThread;
    HANDLE _hStartWork;
    HANDLE _hEndWork;
    bool _fCancel;
    bool _fDoingWork;
    CCrystalScriptStream::const_iterator _it;
    CCrystalScriptStream::const_iterator _itOrig;
    CScriptStreamLimiter *_pLimit;            // Not owned by us.
    SyntaxContext *_pContext;
};
