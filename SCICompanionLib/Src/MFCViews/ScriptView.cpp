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

// SCICompilerView.cpp : implementation of the CScriptView class
//

#include "stdafx.h"

#include "ScriptDocument.h"
#include "ScriptView.h"
#include "resource.h"
#include "Vocab000.h"
#include "VocabAutoCompleteDialog.h"
#include "AppState.h"
#include "ScriptFrame.h"
#include "ClassBrowser.h"
#include "ScriptOM.h"
#include "CrystalScriptStream.h"
#include "GotoDialog.h"
#include "VisualScriptDialog.h"
#include "CompiledScript.h"
#include "Vocab99x.h"
#include "SCO.h"
#include "scii.h"
#include "SyntaxParser.h"
#include "CodeToolTips.h"
#include "CodeAutoComplete.h"
#include "InsertObject.h"
#include "ClassBrowser.h"
#include "CObjectWrap.h"
#include "ResourceEntity.h"
#include "Task.h"
#include "SyntaxContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Time for tooltip to appear
#define TOOLTIPTIMER_ID 1
#define TOOLTIPTIMER_INITIAL 500

// Time for tooltip to expire
#define TOOLTIPEXPIRE_ID 2
#define TOOLTIPEXPIRE_TIMEOUT 8000

using namespace sci;
using namespace std;

#define UWM_AUTOCOMPLETEREADY      (WM_APP + 0)
#define UWM_HOVERTIPREADY          (WM_APP + 1)

void DoToolTipParse(ScriptId scriptId, CCrystalScriptStream &stream, CScriptStreamLimiter &limiter, ToolTipResult &result)
{
    class CToolTipSyntaxParserCallback : public ISyntaxParserCallback
    {
    public:
        CToolTipSyntaxParserCallback(SyntaxContext &context, ToolTipResult &result) : _context(context), _result(result) {}

        bool Done()
        {
            _result = GetToolTipResult<SyntaxContext>(&_context);
            return false;
        }
    private:
        SyntaxContext &_context;
        ToolTipResult &_result;
    };

    Script script(scriptId);
    SyntaxContext context(stream.begin(), script, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), false, false);
    CToolTipSyntaxParserCallback callback(context, result);
    limiter.SetCallback(&callback);
    SyntaxParser_Parse(script, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), nullptr, false, &context);
}

// CScriptView
IMPLEMENT_DYNCREATE(CScriptView, CCrystalEditView)

BEGIN_MESSAGE_MAP(CScriptView, CCrystalEditView)
    ON_WM_CONTEXTMENU()
    ON_WM_ACTIVATE()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_WM_CHAR()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_COMMAND(ID_INSERTOBJECT, OnInsertObject)
    ON_COMMAND(ID_INSERTOBJECTAT, OnInsertObjectAt)
    ON_COMMAND_RANGE(ID_INSERTMETHODAT1, ID_INSERTMETHODAT18, OnInsertMethodAtRange)
    ON_COMMAND_RANGE(ID_INSERTOBJECTAT1, ID_INSERTOBJECTAT18, OnInsertObjectAtRange)
    ON_COMMAND(ID_ADDAS_NOUN, OnAddAsNoun)
    ON_COMMAND(ID_ADDAS_IMPERATIVEVERB, OnAddAsImperativeVerb)
    ON_COMMAND(ID_ADDAS_SYNONYMOF, OnAddAsSynonymOf)
    ON_COMMAND(ID_ADDAS_PREPOSITION, OnAddAsPreposition)
    ON_COMMAND(ID_ADDAS_QUALIFYINGADJECTIVE, OnAddAsQualifyingAdjective)
    ON_COMMAND(ID_ADDAS_RELATIVEPRONOUN, OnAddAsRelativePronoun)
    ON_COMMAND(ID_ADDAS_INDICATIVEVERB, OnAddAsIndicativeVerb)
    ON_COMMAND(ID_ADDAS_ADVERB, OnAddAsAdverb)
    ON_COMMAND(ID_ADDAS_ARTICLE, OnAddAsArticle)
    ON_COMMAND(ID_GOTOSCRIPT, OnGotoScriptHeader)
    ON_COMMAND(ID_MAIN_RELOADSYNTAXCOLORS, OnReloadSyntaxColors)
    ON_COMMAND(ID_EDIT_GOTO, OnGoto)
    ON_COMMAND(ID_SCRIPT_GOTODEFINITION, OnGotoDefinition)
    ON_COMMAND(ID_INTELLISENSE, OnIntellisense)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_NOUN, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_IMPERATIVEVERB, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_SYNONYMOF, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_PREPOSITION, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_QUALIFYINGADJECTIVE, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_RELATIVEPRONOUN, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_INDICATIVEVERB, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_ADVERB, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_ADDAS_ARTICLE, OnUpdateAddAs)
    ON_UPDATE_COMMAND_UI(ID_GOTOSCRIPT, OnUpdateGotoScriptHeader)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_GOTODEFINITION, OnUpdateGotoDefinition)
    // ON_COMMAND(ID_VISUALSCRIPT, OnVisualScript)
    ON_MESSAGE(UWM_AUTOCOMPLETEREADY, OnAutoCompleteReady)
    ON_MESSAGE(UWM_HOVERTIPREADY, OnHoverTipReady)
    ON_COMMAND(ID_MAIN_TOGGLEBLOCKCOMMENT, OnToggleComment)
    ON_UPDATE_COMMAND_UI(ID_MAIN_TOGGLEBLOCKCOMMENT, OnUpdateIsSCI)
    //ON_UPDATE_COMMAND_UI(ID_VISUALSCRIPT, OnUpdateAlwaysOn)
END_MESSAGE_MAP()

// CScriptView construction/destruction

CScriptView::CScriptView()
{
    _fInOnChar = FALSE;
    _pwndToolTip = nullptr;
    _ptLastMM = CPoint(0, 0);
    _ptToolTipWord = CPoint(0, 0);
    _pAutoComp = nullptr;
    _pACThread = nullptr;
    _hoverTipScheduler = nullptr;
    _pMethodTip = nullptr;
    _lastHoverTipParse = -1;
    SetViewTabs(appState->_fShowTabs);
}

void CScriptView::UpdateCaret()
{
    __super::UpdateCaret();
    if (!_fInOnChar)
    {
        // If the caret moved while we weren't in OnChar, then close the autocomplete window if
        // the cursor pos changed.
        if (_ptAC != GetCursorPos())
        {
            _ptAC = GetCursorPos();
            if (_pAutoComp)
            {
                _pAutoComp->Hide();
            }
            //_pParseContext->ResetState();
        }
    }
    // Inform that the curpos pos changed.
    GetDocument()->UpdateAllViewsAndNonViews(this, 0, &WrapHint(ScriptChangeHint::Pos));
}

void CScriptView::ScrollPosChanged()
{
    // Hide any tooltip
    if (_pMethodTip)
    {
        _pMethodTip->Hide();
    }
    if (_pwndToolTip)
    {
        _pwndToolTip->Hide();
    }
}

void CScriptView::UpdateView(CCrystalTextView *pSource, CUpdateContext *pContext, DWORD dwFlags, int nLineIndex /*= -1*/)
{
    if (_pACThread)
    {
        // This detects the case when text is inserted prior to the last autocompleted point,
        // thus invalidating our current autocomplete position and requiring us to reparse from the beginning.
        CPoint pt = _pACThread->GetCompletedPosition();
        CPoint ptRecalced(pt);
        if (pContext)
        {
            pContext->RecalcPoint(ptRecalced);
        }
        if (!pContext || (ptRecalced != pt))
        {
            _pACThread->ResetPosition();
        }
    }
    // If the document was modified, we should ignore any hover tip task result:
    _lastHoverTipParse = -1;
    __super::UpdateView(pSource, pContext, dwFlags, nLineIndex);
}

BOOL CScriptView::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
        if (_pAutoComp && _pAutoComp->IsWindowVisible())
        {
            if (pMsg->message != WM_CHAR) // These VK codes don't make sense to check during WM_CHAR
            {
                if ((pMsg->wParam == VK_DOWN) || (pMsg->wParam == VK_UP) ||
                    (pMsg->wParam == VK_NEXT) || (pMsg->wParam == VK_PRIOR))
                {
                    // Send it to the listbox.
                    _pAutoComp->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
                    return TRUE;
                }
                else if (pMsg->wParam == VK_RETURN)
                {
                    if (OnACDoubleClick())
                    {
                        return TRUE;
                    }
                    // But if nothing is selected, do default handling.
                }
                else if (pMsg->wParam == VK_ESCAPE)
                {
                    // Just close it.
                    _pAutoComp->Hide();
                    return TRUE;
                }
            }
        }
        if (_pMethodTip && _pMethodTip->IsWindowVisible())
        {
            if (pMsg->message != WM_CHAR) // These VK codes don't make sense to check during WM_CHAR
            {
                if (pMsg->wParam == VK_ESCAPE)
                {
                    _pMethodTip->Hide();
                }
            }
        }
        if (_pwndToolTip && _pwndToolTip->IsWindowVisible())
        {
            // If we get a char, or something else (e.g. keydown) that is one of these 4 vkeys,
            // then hide the tooltip.
            if ((pMsg->message == WM_CHAR) ||
                ((pMsg->wParam == VK_DOWN) || (pMsg->wParam == VK_UP) ||
                 (pMsg->wParam == VK_NEXT) || (pMsg->wParam == VK_PRIOR)))
            {
                _pwndToolTip->Hide();
            }
        }
	}
    return __super::PreTranslateMessage(pMsg);
}

BOOL CScriptView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return __super::PreCreateWindow(cs);
}

// CScriptView drawing

CCrystalTextBuffer *CScriptView::LocateTextBuffer()
{
    // Here, we want to connect to a particular buffer in the document
    CScriptDocument *pDoc = GetDocument();
    if (pDoc)
    {
        return pDoc->GetTextBuffer();
    }
    else
    {
        return nullptr;
    }
}


//	SCI keywords

bool _IsKeyword(const std::string &word, std::vector<std::string> &sortedVector, LPTSTR *rg_OrigList, size_t size)
{
    if (sortedVector.empty())
    {
        // First time caller...
        sortedVector.insert(sortedVector.begin(), rg_OrigList, &rg_OrigList[size]);
        sort(sortedVector.begin(), sortedVector.end());
    }
    return binary_search(sortedVector.begin(), sortedVector.end(), word);
}

std::vector<std::string> emptyList;

std::vector<std::string> topLevelKeywordsSCI =
{
    // Keep this alphabetically sorted.
    _T("class"),
    _T("define"),
    _T("include"),
    _T("instance"),
    _T("local"),
    _T("procedure"),
    _T("public"),
    _T("script#"),
    _T("string"),
    _T("synonyms"),
    _T("use"),
};

std::vector<std::string> topLevelKeywordsStudio =
{
    // Keep this alphabetically sorted.
    _T("class"),
    _T("define"),
    _T("exports"),
    _T("include"),
    _T("instance"),
    _T("local"),
    _T("procedure"),
    _T("public"),
    _T("script"),
    _T("string"),
    _T("synonyms"),
    _T("use"),
    _T("version"),
};

std::vector<std::string> topLevelKeywordCPP =
{
    // Keep this alphabetically sorted.
    _T("class"),
    _T("define"),
    _T("include"),
    _T("instance"),
    _T("private"),
    _T("public"),
    _T("synonyms"),
    _T("use"),
};
std::vector<std::string> g_TopLevelKeywordsCpp;

bool IsTopLevelKeyword(LangSyntax lang, const std::string &word)
{
    auto &list = GetTopLevelKeywords(lang);
    return binary_search(list.begin(), list.end(), word);
}

const std::vector<std::string> &GetTopLevelKeywords(LangSyntax lang)
{
    switch (lang)
    {
        case LangSyntaxCpp:
            return topLevelKeywordCPP;
        case LangSyntaxSCI:
            return topLevelKeywordsSCI;
        case LangSyntaxStudio:
            return topLevelKeywordsStudio;
    }
    return emptyList;
}

std::vector<std::string> codeLevelKeywordsSCI =
{
    // Sorted
    _T("&rest"),
    // _T("&tmp"),   // This is special
    _T("and"),
    _T("argc"),
    _T("asm"),
    _T("break"),
    _T("breakif"),
    _T("cond"),
    _T("contif"),
    _T("continue"),
    _T("else"),
    _T("enum"),
    _T("for"),
    _T("if"),
    _T("mod"),
    _T("not"),
    _T("of"),
    _T("or"),
    _T("repeat"),
    _T("return"),
    _T("scriptNumber"),
    _T("self"),
    _T("send"),
    _T("super"),
    _T("switch"),
    _T("switchto"),
    _T("while"),
};


std::vector<std::string> codeLevelKeywordsStudio =
{ 
    // Sorted
    _T("and"),
    _T("asm"),
	_T("break"),
    _T("case"),
    _T("default"),
    _T("do"),
    _T("else"),
    _T("for"),
    _T("if"),
    _T("neg"),
    _T("not"),
    _T("of"),
    _T("or"),
    _T("rest"),
    _T("return"),
    _T("scriptNumber"),
    _T("self"),
    _T("send"),
    _T("super"),
    _T("switch"),
    _T("var"),
	_T("while"),
    _T("paramTotal")
};

std::vector<std::string> codeLevelKeywordsCpp =
{
	_T("break"),
	_T("case"),
    _T("cast1234"),
    _T("default"),
    _T("do"),
    _T("else"),
    _T("false"),
    _T("for"),
    _T("if"),
    _T("null"),
    _T("res"),
    _T("rest"),
    _T("return"),
    _T("scriptNumber"),
    _T("super"),
    _T("switch"),
    _T("true"),
    _T("while"),
};

bool IsCodeLevelKeyword(LangSyntax lang, const std::string &word)
{
    auto &list = GetCodeLevelKeywords(lang);
    return binary_search(list.begin(), list.end(), word);
}

LPTSTR s_apszCPPTypeKeywordList[] =
{
	_T("var"),
	_T("int"),
	_T("uint"),
	_T("bool"),
	_T("void"),
	_T("string"),
	_T("char"),
	_T("said_string"),
	_T("selector"),
	_T("pointer"),
	_T("char"),
	_T("k_window"),
	_T("k_list"),
	_T("k_node"),
    nullptr,
};
std::vector<std::string> g_CPPTypeKeywords;
bool IsCPPTypeKeyword(const std::string &word)
{
    // -1 since it ends with nullptr.
    return _IsKeyword(word, g_CPPTypeKeywords, s_apszCPPTypeKeywordList, ARRAYSIZE(s_apszCPPTypeKeywordList) - 1);
}


LPTSTR s_apszSCIValueKeywordList[] =
{
    _T("objectFunctionArea"),
    _T("objectInfo"),
    _T("objectLocal"),
    _T("objectName"),
    _T("objectSize"),
    _T("objectSpecies"),
    _T("objectSuperclass"),
    _T("objectTotalProperties"),
    _T("objectType"),
    _T("self"),
    _T("super"),
	nullptr
};
std::vector<std::string> g_ValueKeywords;
bool IsValueKeyword(LangSyntax lang, const std::string &word)
{
    // -1 since it ends with nullptr.
    bool isKeyword = _IsKeyword(word, g_ValueKeywords, s_apszSCIValueKeywordList, ARRAYSIZE(s_apszSCIValueKeywordList) - 1);
    if (!isKeyword)
    {
        if (lang == LangSyntaxSCI)
        {
            isKeyword = (word == "argc");
        }
        else
        {
            isKeyword = (word == "paramTotal");
        }
    }
    return isKeyword;
}

std::vector<std::string> classLevelKeywordsCpp = {};
std::vector<std::string> classLevelKeywordsStudio = {  "method", "properties" };
std::vector<std::string> classLevelKeywordsSCI = { "method", "properties", "procedure" };
bool IsClassLevelKeyword(LangSyntax lang, const std::string &word)
{
    auto &list = GetClassLevelKeywords(lang);
    return binary_search(list.begin(), list.end(), word);
}

bool IsSCIKeyword(LangSyntax lang, const std::string &word)
{
    return (IsValueKeyword(lang, word) || IsCodeLevelKeyword(lang, word) || IsTopLevelKeyword(lang, word) || IsClassLevelKeyword(lang, word) ||
        ((lang == LangSyntaxCpp) && IsCPPTypeKeyword(word)) ||
        ((lang == LangSyntaxSCI) && (word == "&tmp")));
}

const std::vector<std::string> &GetCodeLevelKeywords(LangSyntax lang)
{
    switch (lang)
    {
        case LangSyntaxCpp:
            return codeLevelKeywordsCpp;
        case LangSyntaxSCI:
            return codeLevelKeywordsSCI;
        case LangSyntaxStudio:
            return codeLevelKeywordsStudio;
    }
    return emptyList;
}

const std::vector<std::string> &GetClassLevelKeywords(LangSyntax lang)
{
    switch (lang)
    {
        case LangSyntaxCpp:
            return classLevelKeywordsCpp;
        case LangSyntaxSCI:
            return classLevelKeywordsSCI;
        case LangSyntaxStudio:
            return classLevelKeywordsStudio;
    }
    return emptyList;
}

static BOOL IsSCISelectorLiteral(LPCTSTR pszChars, int nLength)
{
    BOOL bRet = FALSE;
    if (pszChars[0] == '#')
    {
        bRet = TRUE;
        for (int i = 1; i < nLength; i++)
        {
            if (!isalnum(pszChars[i]))
            {
                bRet = FALSE;
                break;
            }
        }
    }
    return bRet;
}

static BOOL IsSCIKeyword(LangSyntax lang, LPCTSTR pszChars, int nLength)
{
    return IsSCIKeyword(lang, std::string(pszChars, nLength));
}

BOOL IsStudioNumber(LPCTSTR pszChars, int nLength)
{
    // Hex
    if (nLength > 1 && pszChars[0] == '$')
	{
        for (int I = 1; I < nLength; I++)
		{
			if (isdigit(pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
										(pszChars[I] >= 'a' && pszChars[I] <= 'f'))
				continue;
			return FALSE;
		}
		return TRUE;
	}
	if (!isdigit(pszChars[0]) && ((nLength <= 1) || (pszChars[0] != '-') || !isdigit(pszChars[1])))
		return FALSE;
	for (int I = 1; I < nLength; I++)
	{
		if (! isdigit(pszChars[I]) && pszChars[I] != '+' &&
			pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
			pszChars[I] != 'E')
			return FALSE;
	}
	return TRUE;
}

BOOL IsSCISelectorCall(LPCTSTR pszChars, int nLength)
{
    char ch = pszChars[nLength - 1];
    return ch == ':' || ch == '?';
}

BOOL IsSCINumber(LPCTSTR pszChars, int nLength)
{
    if (nLength > 1)
    {
        // Binary
        if (pszChars[0] == '%')
        {
            for (int i = 1; i < nLength; i++)
            {
                if ((pszChars[i] == '1') || (pszChars[i] == '0'))
                    continue;
                return FALSE;
            }
            return TRUE;
        }
        // Character literal
        else if (pszChars[0] == '`')
        {
            int lengthRequired = 2;
            if ((pszChars[1] == '^') || (pszChars[1] == '@') || (pszChars[1] == '#'))
            {
                lengthRequired++;
            }
            return (lengthRequired == nLength);
        }
    }
    return IsStudioNumber(pszChars, nLength);
}

#define DEFINE_BLOCK(pos, colorindex)	\
	ASSERT((pos) >= 0 && (pos) <= nLength);\
	if (pBuf != nullptr)\
	{\
		if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
		pBuf[nActualItems].m_nCharPos = (pos);\
		pBuf[nActualItems].m_nColorIndex = (colorindex);\
		nActualItems ++;}\
	}

#define COOKIE_COMMENT			0x0001
#define COOKIE_SELECTOR 		0x0002
#define COOKIE_EXT_COMMENT		0x0004
#define COOKIE_STRING			0x0008
#define COOKIE_CHAR				0x0010
#define COOKIE_INTERNALSTRING   0x0020

// Various states we can be in:
// % bin digit
enum class HighlightState
{
    None,
    Number,
    Hex,
    BinaryLiteral,
    CharLiteral,
    SelectorLiteral,
    RegularToken,
    RegularTokenOrNumber,
    SelectorCall,
};

DWORD CScriptView::_ParseLineSCI(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
    HighlightState state = HighlightState::None;

    int nLength = GetLineLength(nLineIndex);

    LPCTSTR pszChars = GetLineChars(nLineIndex);
    BOOL bFirstChar = TRUE;
    BOOL bRedefineBlock = TRUE;
    BOOL bDecIndex = FALSE;
    int nIdentBegin = -1;
    int I = 0;
    for (;; I++)
    {
        if (bRedefineBlock)
        {
            int nPos = I;
            if (bDecIndex)
                nPos--;
            if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
                DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
            }
            else
                if (dwCookie & (COOKIE_STRING))
                {
                    DEFINE_BLOCK(nPos, COLORINDEX_STRING);
                }
                else
                    if (dwCookie & (COOKIE_CHAR))
                    {
                        DEFINE_BLOCK(nPos, COLORINDEX_SAID);
                    }
                    else
                        if (dwCookie & COOKIE_SELECTOR)
                        {
                            DEFINE_BLOCK(nPos, COLORINDEX_SELECTORLITERAL);
                        }
                        else
                            if (dwCookie & COOKIE_INTERNALSTRING)
                            {
                                DEFINE_BLOCK(nPos, COLORINDEX_INTERNALSTRING);
                            }
                            else
                            {
                                DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
                            }
            bRedefineBlock = FALSE;
            bDecIndex = FALSE;
        }

        if (I == nLength)
            break;

        // Turning off things...
        if (dwCookie & COOKIE_COMMENT)
        {
            DEFINE_BLOCK(I, COLORINDEX_COMMENT);
            dwCookie |= COOKIE_COMMENT;
            break;
        }

        //	String constant "...."
        if (dwCookie & COOKIE_STRING)
        {
            if (pszChars[I] == '"' && (I == 0 || pszChars[I - 1] != '\\'))
            {
                dwCookie &= ~COOKIE_STRING;
                bRedefineBlock = TRUE;
            }
            continue;
        }

        // Internal string {....}
        if (dwCookie & COOKIE_INTERNALSTRING)
        {
            if (pszChars[I] == '}' && (I == 0 || pszChars[I - 1] != '\\'))
            {
                dwCookie &= ~COOKIE_INTERNALSTRING;
                bRedefineBlock = TRUE;
            }
            continue;
        }

        //	Said spec '..'
        if (dwCookie & COOKIE_CHAR)
        {
            if (pszChars[I] == '\'' && (I == 0 || pszChars[I - 1] != '\\'))
            {
                dwCookie &= ~COOKIE_CHAR;
                bRedefineBlock = TRUE;
            }
            continue;
        }

        ///////////////////////
        // Starting things
        // REVIEW: This should only be true if it's not in a string
        if (pszChars[I] == ';')
        {
            DEFINE_BLOCK(I, COLORINDEX_COMMENT);
            dwCookie |= COOKIE_COMMENT;
            break;
        }

        //	Normal text
        if (pszChars[I] == '"')
        {
            DEFINE_BLOCK(I, COLORINDEX_STRING);
            dwCookie |= COOKIE_STRING;
            continue;
        }
        if (pszChars[I] == '{')
        {
            DEFINE_BLOCK(I, COLORINDEX_INTERNALSTRING);
            dwCookie |= COOKIE_INTERNALSTRING;
            continue;
        }
        if (pszChars[I] == '\'')
        {
            DEFINE_BLOCK(I, COLORINDEX_SAID);
            dwCookie |= COOKIE_CHAR;
            continue;
        }

        if (bFirstChar)
        {
            if (!isspace(pszChars[I]))
                bFirstChar = FALSE;
        }

        if (pBuf == nullptr)
            continue;	//	We don't need to extract keywords,
        //	for faster parsing skip the rest of loop

        bool sumup = false;
        char ch = pszChars[I];
        switch (state)
        {
            case HighlightState::None:
                if (isalpha(ch) || ch == '_' || ch == '&')  // & to support &rest and &temp
                {
                    state = HighlightState::RegularToken;
                }
                else if (ch == '-') // - to support -info-, or it could be a number
                {
                    state = HighlightState::RegularTokenOrNumber;
                }
                else if (isdigit(ch))
                {
                    state = HighlightState::Number;
                }
                else if (ch == '$')
                {
                    state = HighlightState::Hex;
                }
                else if (ch == '#')
                {
                    state = HighlightState::SelectorLiteral;
                }
                else if (ch == '%')
                {
                    state = HighlightState::BinaryLiteral;
                }
                else if (ch == '`')
                {
                    state = HighlightState::CharLiteral;
                }

                if (state != HighlightState::None)
                {
                    // Beginning a token
                    if (nIdentBegin == -1)
                        nIdentBegin = I;
                }
                break;

            case HighlightState::RegularTokenOrNumber:
                if (isdigit(ch))
                {
                    state = HighlightState::Number; // Now we know...
                }
                else if (isalpha(ch))
                {
                    state = HighlightState::RegularToken; // Now we know...
                }
                else
                {
                    sumup = true;
                }
                break;

            case HighlightState::RegularToken:
                if (isalnum(ch) || ch == '_' || ch == '-' || ch == '#') // # for "script#"
                {
                    // good, keep going
                }
                else if (ch == '?' || ch == ':')
                {
                    state = HighlightState::SelectorCall;
                }
                else
                {
                    sumup = true;
                }
                break;

            case HighlightState::SelectorCall:
                sumup = true; // We're done
                break;

            case HighlightState::CharLiteral:
                sumup = (!isalnum(ch) && ch != '^' || ch != '@');

            case HighlightState::BinaryLiteral:
                sumup = (ch != '0' && ch != '1');
                break;

            case HighlightState::Hex:
                sumup = !isxdigit(ch);
                break;

            case HighlightState::Number:
                sumup = !isdigit(ch);
                break;

            case HighlightState::SelectorLiteral:
                sumup = !isalnum(ch) && ch != '_' && ch != '-';
                break;
                    
        }

        if (sumup)
        {
            if (nIdentBegin >= 0)
            {
                if (IsSCISelectorCall(pszChars + nIdentBegin, I - nIdentBegin))
                {
                    DEFINE_BLOCK(nIdentBegin, COLORINDEX_SELECTORCALL);
                }
                else if (IsSCIKeyword(LangSyntaxSCI, pszChars + nIdentBegin, I - nIdentBegin))
                {
                    DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
                }
                // REVIEW: We'll need more things, for back-quote
                else if (IsSCINumber(pszChars + nIdentBegin, I - nIdentBegin))
                {
                    DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
                }
                else if (IsSCISelectorLiteral(pszChars + nIdentBegin, I - nIdentBegin))
                {
                    DEFINE_BLOCK(nIdentBegin, COLORINDEX_SELECTORLITERAL);
                }
                bRedefineBlock = TRUE;
                bDecIndex = TRUE;
                nIdentBegin = -1;
            }
            state = HighlightState::None;
        }
    }

    if (nIdentBegin >= 0)
    {
        if (IsSCISelectorCall(pszChars + nIdentBegin, I - nIdentBegin))
        {
            DEFINE_BLOCK(nIdentBegin, COLORINDEX_SELECTORCALL);
        }
        else if (IsSCIKeyword(LangSyntaxSCI, pszChars + nIdentBegin, I - nIdentBegin))
        {
            DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
        }
        // REVIEW: We'll need more things, for back-quote
        else if (IsSCINumber(pszChars + nIdentBegin, I - nIdentBegin))
        {
            DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
        }
        else if (IsSCISelectorLiteral(pszChars + nIdentBegin, I - nIdentBegin))
        {
            DEFINE_BLOCK(nIdentBegin, COLORINDEX_SELECTORLITERAL);
        }
    }

    dwCookie &= ~COOKIE_COMMENT;

    return dwCookie;
}

DWORD CScriptView::ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
    LangSyntax lang = GetDocument()->GetScriptId().Language();
    switch (lang)
    {
        case LangSyntaxSCI:
            return _ParseLineSCI(dwCookie, nLineIndex, pBuf, nActualItems);
        case LangSyntaxStudio:
            return _ParseLineStudio(dwCookie, nLineIndex, pBuf, nActualItems);
    }
    return 0;
}

//
// SCI syntax highlighting
//
DWORD CScriptView::_ParseLineStudio(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
    int nLength = GetLineLength(nLineIndex);
    if (nLength <= 0)
        return dwCookie & COOKIE_EXT_COMMENT;

    LPCTSTR pszChars    = GetLineChars(nLineIndex);
    BOOL bFirstChar     = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
    BOOL bRedefineBlock = TRUE;
    BOOL bDecIndex  = FALSE;
    int nIdentBegin = -1;
    int I = 0;
    for (; ; I++)
    {
        if (bRedefineBlock)
        {
            int nPos = I;
            if (bDecIndex)
                nPos--;
            if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
                DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
            }
            else
            if (dwCookie & (COOKIE_STRING))
            {
                DEFINE_BLOCK(nPos, COLORINDEX_STRING);
            }
            else
            if (dwCookie & (COOKIE_CHAR))
            {
                DEFINE_BLOCK(nPos, COLORINDEX_SAID);
            }
            else
            if (dwCookie & COOKIE_SELECTOR)
            {
                DEFINE_BLOCK(nPos, COLORINDEX_SELECTORLITERAL);
            }
            else
            if (dwCookie & COOKIE_INTERNALSTRING)
            {
                DEFINE_BLOCK(nPos, COLORINDEX_INTERNALSTRING);
            }
            else
            {
                DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
            }
            bRedefineBlock = FALSE;
            bDecIndex      = FALSE;
        }

        if (I == nLength)
            break;

        if (dwCookie & COOKIE_COMMENT)
        {
            DEFINE_BLOCK(I, COLORINDEX_COMMENT);
            dwCookie |= COOKIE_COMMENT;
            break;
        }

        //	String constant "...."
        if (dwCookie & COOKIE_STRING)
        {
            if (pszChars[I] == '"' && (I == 0 || pszChars[I - 1] != '\\'))
            {
                dwCookie &= ~COOKIE_STRING;
                bRedefineBlock = TRUE;
            }
            continue;
        }

        // Internal string {....}
        if (appState->_fAllowBraceSyntax)
        {
            if (dwCookie & COOKIE_INTERNALSTRING)
            {
                if (pszChars[I] == '}' && (I == 0 || pszChars[I - 1] != '\\'))
                {
                    dwCookie &= ~COOKIE_INTERNALSTRING;
                    bRedefineBlock = TRUE;
                }
                continue;
            }
        }

        //	Said spec '..'
        if (dwCookie & COOKIE_CHAR)
        {
            if (pszChars[I] == '\'' && (I == 0 || pszChars[I - 1] != '\\'))
            {
                dwCookie &= ~COOKIE_CHAR;
                bRedefineBlock = TRUE;
            }
            continue;
        }

        //	Extended comment /*....*/
        if (dwCookie & COOKIE_EXT_COMMENT)
        {
            if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '*')
            {
                dwCookie &= ~COOKIE_EXT_COMMENT;
                bRedefineBlock = TRUE;
            }
            continue;
        }

        if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '/')
        {
            DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
            dwCookie |= COOKIE_COMMENT;
            break;
        }

        //	Normal text
        if (pszChars[I] == '"')
        {
            DEFINE_BLOCK(I, COLORINDEX_STRING);
            dwCookie |= COOKIE_STRING;
            continue;
        }
        if (appState->_fAllowBraceSyntax && pszChars[I] == '{')
        {
            DEFINE_BLOCK(I, COLORINDEX_INTERNALSTRING);
            dwCookie |= COOKIE_INTERNALSTRING;
            continue;
        }
        if (pszChars[I] == '\'')
        {
            DEFINE_BLOCK(I, COLORINDEX_SAID);
            dwCookie |= COOKIE_CHAR;
            continue;
        }
        if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
        {
            DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
            dwCookie |= COOKIE_EXT_COMMENT;
            continue;
        }

        if (bFirstChar)
        {
            if (! isspace(pszChars[I]))
                bFirstChar = FALSE;
        }

        if (pBuf == nullptr)
            continue;	//	We don't need to extract keywords,
                        //	for faster parsing skip the rest of loop

        if (isalnum(pszChars[I]) || pszChars[I] == '_' || pszChars[I] == '$' || pszChars[I] == '#')
        {
            if (nIdentBegin == -1)
                nIdentBegin = I;
        }
        else
        {
            if (nIdentBegin >= 0)
            {
                if (IsSCIKeyword(LangSyntaxStudio,  pszChars + nIdentBegin, I - nIdentBegin))
                {
                    DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
                }
                else if (IsStudioNumber(pszChars + nIdentBegin, I - nIdentBegin))
                {
                    DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
                }
                else if (IsSCISelectorLiteral(pszChars + nIdentBegin, I - nIdentBegin))
                {
                    DEFINE_BLOCK(nIdentBegin, COLORINDEX_SELECTORLITERAL);
                }
                bRedefineBlock = TRUE;
                bDecIndex = TRUE;
                nIdentBegin = -1;
            }
        }
    }

    if (nIdentBegin >= 0)
    {
        if (IsSCIKeyword(LangSyntaxStudio, pszChars + nIdentBegin, I - nIdentBegin))
        {
            DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
        }
        else
        if (IsStudioNumber(pszChars + nIdentBegin, I - nIdentBegin))
        {
            DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
        }
    }

    if (pszChars[nLength - 1] != '\\')
        dwCookie &= COOKIE_EXT_COMMENT;
    return dwCookie;
}


void CScriptView::OnCompile()
{
    CScriptDocument *pDocument = GetDocument();
    if (pDocument)
    {
        //pDocument->CompileScript();
    }
}

BOOL _GetMenuItem(PCTSTR pszText, CMenu *pMenu, UINT *pnID)
{
    BOOL fRet = FALSE;
    UINT nCount = pMenu->GetMenuItemCount();
    for (UINT i = 0; i < nCount; i++)
    {
        TCHAR szBuffer[MAX_PATH];
        MENUITEMINFO mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STRING;
        mii.cch = ARRAYSIZE(szBuffer);
        mii.dwTypeData = szBuffer;
        if (pMenu->GetMenuItemInfo(i, &mii, TRUE))
        {
            if (0 == lstrcmpi(pszText, mii.dwTypeData))
            {
                fRet = TRUE;
                *pnID = i;
                break;
            }
        }
    }
    return fRet;
}

BOOL _GetAddAsMenuItem(CMenu *pMenu, UINT *pnID)
{
    return _GetMenuItem(TEXT("Add as"), pMenu, pnID);
}

BOOL _GetGoToDefnMenuItem(CMenu *pMenu, UINT *pnID)
{
    return _GetMenuItem(TEXT("Go to definition"), pMenu, pnID);
}

BOOL _GetInsertObjectMenuItem(CMenu *pMenu, UINT *pnID)
{
    return _GetMenuItem(TEXT("Insert object"), pMenu, pnID);
}

BOOL _GetInsertMethodMenuItem(CMenu *pMenu, UINT *pnID)
{
    return _GetMenuItem(TEXT("Insert method"), pMenu, pnID);
}

void CScriptView::OnContextMenu(CWnd *pWnd, CPoint point)
{
    CPoint ptClient = point;
    ScreenToClient(&ptClient);
    CPoint ptText = ptClient;
    AdjustTextPoint(ptText);
    BOOL fPossiblyOnWord = _ClientToTextNoMargin(ptText, ptText);

    // Reset these strings.
    _contextMenuText = TEXT("");
    _gotoScriptText = TEXT("");
    _gotoDefinitionText = TEXT("");

    CMenu contextMenu; 
    contextMenu.LoadMenu(IDR_MENUSCRIPT); 
    CMenu* pTracker; 
    pTracker = contextMenu.GetSubMenu(0); 

    if (pTracker)
    {
        if (fPossiblyOnWord)
        {
            BOOL fIsHeaderLabel = GetHeaderFile(ptText, _gotoScriptText);
            if (!fIsHeaderLabel)
            {
                UINT nItem;
                if (_GetAddAsMenuItem(pTracker, &nItem))
                {
                    MENUITEMINFO mii = { 0 };
                    mii.cbSize = sizeof(mii);

                    // Get the text around the place where the user clicked
                    CPoint ptRight = WordToRight(ptText);
                    CPoint ptLeft = WordToLeft(ptText);
                    TCHAR szBuffer[MAX_PATH];
                    bool fDisableMenuOption = true;
                    if (appState->GetVersion().HasSaidVocab && (ptRight != ptLeft))
                    {
                        // This is a potential vocab string.
    	                GetText(ptLeft, ptRight, _contextMenuText);
                        if (IsValidVocabString(_contextMenuText, FALSE))
                        {
                            StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Add \"%s\" as"), (PCTSTR)_contextMenuText);
                            mii.fMask = MIIM_STRING;
                            mii.dwTypeData = szBuffer;
                            fDisableMenuOption = false;
                        }
                    }

                    if (fDisableMenuOption)
                    {
                        _contextMenuText = TEXT("");
                        // Disable the item.
                        mii.fMask = MIIM_STATE;
                        mii.fState = MFS_DISABLED;
                    }

                    pTracker->SetMenuItemInfo(nItem, &mii, TRUE);
                }
            }
        }

        if (fPossiblyOnWord)
        {
            // Do a parse to see if we have something interesting to show for the "goto" entry.
            CPoint ptRight = WordToRight(ptText);

            CScriptStreamLimiter limiter(LocateTextBuffer(), ptRight, 0);
            CCrystalScriptStream stream(&limiter);
            ToolTipResult result;
            DoToolTipParse(GetDocument()->GetScriptId(), stream, limiter, result);
            if (!result.empty())
            {
                _gotoDefinitionText = result.strBaseText.c_str();
                _gotoScript = result.scriptId;
                _gotoLineNumber = result.iLineNumber;
            }
        }

        // Add objects?
        UINT insertObjectIndex;
        if (_GetInsertObjectMenuItem(pTracker, &insertObjectIndex))
        {
            CMenu *subMenu = pTracker->GetSubMenu(insertObjectIndex);
            if (subMenu)
            {
                _availableObjects = make_unique<AvailableObjects>(GetDocument()->GetScriptId().Language());
                for (size_t i = 0; i < _availableObjects->GetObjects().size(); i++)
                {
                    int iIndex = 0;
                    MENUITEMINFO mii = { 0 };
                    mii.cbSize = sizeof(mii);
                    mii.fMask = MIIM_ID | MIIM_STRING;
                    mii.wID = ID_INSERTOBJECTAT1 + i;
                    std::string foo = _availableObjects->GetObjects()[i]->GetSuperClass();
                    mii.dwTypeData = const_cast<LPSTR>(foo.c_str());
                    subMenu->InsertMenuItem(ID_INSERTOBJECTAT1 + i, &mii, FALSE);
                }
            }
        }

        // Add methods?
        UINT insertMethodIndex;
        if (_GetInsertMethodMenuItem(pTracker, &insertMethodIndex))
        {
            CMenu *subMenu = pTracker->GetSubMenu(insertMethodIndex);
            if (subMenu)
            {
                subMenu->RemoveMenu(0, MF_BYPOSITION);
                _availableMethods = make_unique<AvailableMethods>(GetDocument()->GetScriptId().Language());
                for (size_t i = 0; i < _availableMethods->GetMethods().size(); i++)
                {
                    int iIndex = 0;
                    MENUITEMINFO mii = { 0 };
                    mii.cbSize = sizeof(mii);
                    mii.fMask = MIIM_ID | MIIM_STRING;
                    mii.wID = ID_INSERTMETHODAT1 + i;
                    std::string foo = _availableMethods->GetMethods()[i]->GetName();
                    mii.dwTypeData = const_cast<LPSTR>(foo.c_str());
                    subMenu->InsertMenuItem(ID_INSERTMETHODAT1 + i, &mii, FALSE);
                }
            }
        }

        pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
        point.x , point.y , AfxGetMainWnd()); 
    }

    //has always to be called (function of base class)
    __super::OnContextMenu(pWnd, point);
}

void CScriptView::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
    // REVIEW: This is never called.
    __super::OnActivate(nState, pWndOther, bMinimized);
}

void CScriptView::OnKillFocus(CWnd *pNewWnd)
{
    if (_pAutoComp && _pMethodTip)
    {
        if (_pAutoComp->IsWindowVisible() || _pMethodTip->IsWindowVisible())
        {
            if (pNewWnd->GetSafeHwnd() != _pAutoComp->GetSafeHwnd())
            {
                // We're being de-focused, to a window other than the intellisense listbox.
                // So hide it.
                _pAutoComp->Hide();
                _pMethodTip->Hide();
            }
        }
    }
    if (_pwndToolTip)
    {
        _pwndToolTip->Hide();
    }
    __super::OnKillFocus(pNewWnd);
}

void CScriptView::OnSetFocus(CWnd *pNewWnd)
{
    appState->GiveMeAutoComplete(this);
    if (_pACThread)
    {
        _pACThread->InitializeForScript(LocateTextBuffer(), GetDocument()->GetScriptId().Language());
    }

    if (_pAutoComp && _pAutoComp->IsWindowVisible())
    {
        // We're being focused, to a window other than the intellisense listbox.
        // So hide it.
        _pAutoComp->Hide();
        _pMethodTip->Hide();
    }
    __super::OnSetFocus(pNewWnd);
}

void CScriptView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    _fInOnChar = TRUE;
    __super::OnChar(nChar, nRepCnt, nFlags);
    _fInOnChar = FALSE;
    _ptAC = GetCursorPos();
    if (nChar > 31)
    {
        OnIntellisense();
    }
}

void CScriptView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (_pMethodTip)
    {
        _pMethodTip->Hide();
    }
    KillTimer(TOOLTIPTIMER_ID);
    if (_pwndToolTip)
    {
        _pwndToolTip->Hide();
    }
    __super::OnLButtonDown(nFlags, point);
}

void CScriptView::OnRButtonDown(UINT nFlags, CPoint point)
{
    if (_pMethodTip)
    {
        _pMethodTip->Hide();
    }
    KillTimer(TOOLTIPTIMER_ID);
    if (_pwndToolTip)
    {
        _pwndToolTip->Hide();
    }
    __super::OnRButtonDown(nFlags, point);
}

void CScriptView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (_pwndToolTip)
    {
        CPoint ptWordRight, ptDummy;
        BOOL fInEditableArea = _ScreenToWordRight(point, ptDummy, ptWordRight);
        if (!fInEditableArea || (_ptToolTipWord != ptWordRight))
        {
            _pwndToolTip->Hide();
            SetTimer(TOOLTIPTIMER_ID, TOOLTIPTIMER_INITIAL, nullptr);
        }
    }
    _ptLastMM = point;
    __super::OnMouseMove(nFlags, point);
}

void CScriptView::OnTimer(UINT nIDEvent)
{
    switch (nIDEvent)
    {
    case TOOLTIPTIMER_ID:
        {
            KillTimer(TOOLTIPTIMER_ID);
            // Is mouse still over the window?  Do we still have focus?
            CPoint pt;
            if (::GetCursorPos(&pt))
            {
                ScreenToClient(&pt);
                CRect rc;
                GetClientRect(&rc);
                if (rc.PtInRect(pt))
                {
                    // Still in the window, good.
                    if (pt == _ptLastMM)
                    {
                        // At the same spot.  Try to bring up a tooltip.
                        _BringUpToolTip(pt);
                    }
                }
            }
        }
        break;

    case TOOLTIPEXPIRE_ID:
        {
            KillTimer(TOOLTIPEXPIRE_ID);
            _pwndToolTip->Hide();
        }
        break;
    }
    __super::OnTimer(nIDEvent);
}

void CScriptView::_OnAddAs(WordClass dwClass)
{
    ResourceEntity *pResource = appState->GetResourceMap().GetVocabResourceToEdit();
    if (pResource)
    {
        Vocab000 *pVocab000 = pResource->TryGetComponent<Vocab000>();
        if (pVocab000)
        {
            VocabChangeHint hint = pVocab000->AddNewWord(_contextMenuText, dwClass, TRUE);
            if (hint != VocabChangeHint::None)
            {
                appState->GetResourceMap().AppendResource(*pResource);
            }
        }
    }
}

void CScriptView::_OnInsertObject(bool currentPosition)
{
    CScriptDocument *pDoc = GetDocument();
    if (pDoc)
    {
        CInsertObject dialog(pDoc->GetScriptId().Language());
        if (IDOK == dialog.DoModal())
        {
            CString strBuffer = dialog.GetBuffer();
            if (!currentPosition)
            {
                MoveToEnd();
            }
            PasteTextAtCursorAndHightlightWord(strBuffer, c_szUnnamedObject);
        }
    }
}

void CScriptView::OnDestroy()
{
    if (_hoverTipScheduler)
    {
        _hoverTipScheduler->DeactivateHWND(this->GetSafeHwnd());
    }
    __super::OnDestroy();
}

void CScriptView::OnInsertObject()
{
    _OnInsertObject(false);
}

void CScriptView::OnInsertObjectAt()
{
    _OnInsertObject(true);
}

void CScriptView::OnInsertMethodAtRange(UINT nID)
{
    int index = nID - ID_INSERTMETHODAT1;
    if (_availableMethods)
    {
        CString strBuffer;
        const MethodDefinition *methodDef = _availableMethods->GetMethods()[index];
        _availableMethods->PrepareBuffer(methodDef, strBuffer);
        PasteTextAtCursor(strBuffer);
        _availableMethods.reset(nullptr);
    }
}

void CScriptView::OnInsertObjectAtRange(UINT nID)
{
    int index = nID - ID_INSERTOBJECTAT1;
    if (_availableObjects)
    {
        CString strBuffer;
        ClassDefinition *classDef = _availableObjects->GetObjects()[index];
        classDef->SetName(c_szUnnamedObject);
        _availableObjects->PrepareBuffer(classDef, strBuffer);

        PasteTextAtCursorAndHightlightWord(strBuffer, c_szUnnamedObject);

        _availableObjects.reset(nullptr);
    }
}

void CScriptView::OnAddAsNoun()
{
    _OnAddAs(WordClass::Noun);
}

void CScriptView::OnAddAsImperativeVerb()
{
    _OnAddAs(WordClass::ImperativeVerb);
}

void CScriptView::OnAddAsPreposition()
{
    _OnAddAs(WordClass::Proposition);
}

void CScriptView::OnAddAsQualifyingAdjective()
{
    _OnAddAs(WordClass::QualifyingAdjective);
}

void CScriptView::OnAddAsRelativePronoun()
{
    _OnAddAs(WordClass::RelativePronoun);
}

void CScriptView::OnAddAsIndicativeVerb()
{
    _OnAddAs(WordClass::IndicativeVerb);
}

void CScriptView::OnAddAsAdverb()
{
    _OnAddAs(WordClass::Adverb);
}

void CScriptView::OnAddAsArticle()
{
    _OnAddAs(WordClass::Article);
}

void CScriptView::OnAddAsSynonymOf()
{
    ASSERT(!_contextMenuText.IsEmpty());

    CVocabAutoCompleteDialog dialog;
    dialog.SetWord(_contextMenuText);
    if (IDOK == dialog.DoModal())
    {
        CString strWord = dialog.GetWord();
        CString strSynonym = dialog.GetSynonym();

        ResourceEntity *resource = appState->GetResourceMap().GetVocabResourceToEdit();
        if (resource)
        {
            Vocab000 *pVocab000 = resource->TryGetComponent<Vocab000>();
            if (pVocab000)
            {
                VocabChangeHint hint = pVocab000->AddSynonym(strWord, strSynonym);
                if (hint != VocabChangeHint::None)
                {
                    appState->GetResourceMap().AppendResource(*resource);
                }
            }
        }
    }
}

void CScriptView::OnUpdateIsSCI(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetDocument() && (GetDocument()->GetScriptId().Language() == LangSyntaxSCI));
}

void CScriptView::OnUpdateAddAs(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!_contextMenuText.IsEmpty());
}

void CScriptView::OnReloadSyntaxColors()
{
    LoadSyntaxHighlightingColors();
    Invalidate();
}

void CScriptView::OnGotoScriptHeader()
{
    ASSERT(!_gotoScriptText.IsEmpty());

    // Search in various folders for the script.
    appState->OpenScriptHeader((PCSTR)_gotoScriptText);
}

void CScriptView::OnGotoDefinition()
{
    CScriptDocument *pDoc = GetDocument();
    if (pDoc)
    {
        // Ensure the script is open (might not be if we did a "compile all")
        // +1 because line# in crystal-edit-speak starts at 1
        appState->OpenScriptAtLine(_gotoScript.GetFullPath(), _gotoLineNumber + 1);
    }
}

void CScriptView::OnInitialUpdate()
{
    __super::OnInitialUpdate();

    if (_pACThread)
    {
        _pACThread->InitializeForScript(LocateTextBuffer(), GetDocument()->GetScriptId().Language());
    }
}

//
// Given a screen coordinate, finds the corresponding point in text coordinates
// that would be used for parsing a tooltip. Also returns the point under ptClient.
//
BOOL CScriptView::_ScreenToWordRight(CPoint ptClient, CPoint &ptUnder, CPoint &ptWordRight)
{
    AdjustTextPoint(ptClient);
    BOOL fRet = _ClientToTextNoMargin(ptClient, ptUnder);
    if (fRet)
    {
        // Move to the end of the word to the right.
        ptWordRight = WordToRight(ptUnder);
    }
    return fRet;
}

void CScriptView::_TriggerHoverTipParse(CPoint pt)
{
    if (appState->AreHoverTipsEnabled() && _hoverTipScheduler)
    {
        _lastHoverTipParse = _hoverTipScheduler->SubmitTask(
            this->GetSafeHwnd(),
            UWM_HOVERTIPREADY,
            make_unique<HoverTipPayload>(GetDocument()->GetScriptId(), LocateTextBuffer(), pt),
            [](ITaskStatus &status, HoverTipPayload &payload)
        {
            std::unique_ptr<HoverTipResponse> response = std::make_unique<HoverTipResponse>();
            response->Location = payload.Location;
            DoToolTipParse(payload.ScriptId, payload.Stream, payload.Limiter, response->Result);
            return response;
        }
            );
    }
    else
    {
        _lastHoverTipParse = -1;
    }
}

void CScriptView::_BringUpToolTip(CPoint ptClient)
{
    // Kill any expire id.  We don't want an old one coming along and expiring a new tooltip
    KillTimer(TOOLTIPEXPIRE_ID);

    if (_pwndToolTip)
    {
        CPoint ptUnder, ptToolTipWord;
        BOOL fInEditableArea = _ScreenToWordRight(ptClient, ptUnder, ptToolTipWord);
        // If ptUnder == ptToolTipWord, then there's no word under the mouse.
        if (fInEditableArea && (ptToolTipWord != ptUnder))
        {
            _ptToolTipWord = ptToolTipWord;
            _TriggerHoverTipParse(_ptToolTipWord);
        }
    }
}


#define SMUDGE_FACTOR 10    // So we can hit test characters at the end of the line.

BOOL CScriptView::_ClientToTextNoMargin(CPoint ptClient, CPoint &ptText)
{
    BOOL fRet = FALSE;
    // We want to fail this operation if the client point is off the end of the line.
    ptText = ClientToText(ptClient);
    int nChars = GetLineLength(ptText.y);
    CPoint ptTest(nChars, ptText.y);
    CPoint ptClientTest = TextToClient(ptTest);
    ptClientTest.x += SMUDGE_FACTOR;
    return (ptClient.x <= ptClientTest.x);
}


void CScriptView::OnIntellisense()
{
    //assert(_hEventACDone);
    //assert(_hEventDoAC);

    if (!appState->IsCodeCompletionEnabled())
    {
        // Nothing to see here...
        return;
    }

    // Set our current position
    CPoint pt = GetCursorPos();
    // And do the autocomplete
    
    if (appState->IsCodeCompletionEnabled())
    {
        _pACThread->StartAutoComplete(pt, this->GetSafeHwnd(), UWM_AUTOCOMPLETEREADY, GetDocument()->GetScriptId().GetResourceNumber());
    }

    // Wake up the autocomplete parsecontext, after telling it where we are.
    //CPoint pt = GetCursorPos();
    //_pParseContext->SetCurrentPos(_pStream, _pLimiter, FALSE, LINECHAR2POS(pt.y, pt.x));
    //_pParseContext->SetOptions(PARSECONTEXTOPTION_FORAUTOCOMPLETE);
    // SetEvent(_hEventDoAC);
    // Wait until it's done.
    //if (WAIT_OBJECT_0 == WaitForSingleObject(_hEventACDone, INFINITE))
    {
        // We have a result.
        //  ResetEvent(_hEventACDone); // Reset for next time.
        //AutoCompleteResult &result = _pParseContext->GetResult();

        /*
        if (appState->IsParamInfoEnabled())
        {
        if (result.HasMethodTip())
        {
        // Make it visible.
        CPoint ptClient = TextToClient(GetCursorPos());
        ClientToScreen(&ptClient);
        ptClient.y -= GetLineHeight() + 2;
        _pMethodTip->Show(ptClient, result.strMethod, result.strParams, result.cParamNumber);
        }
        else
        {
        // Hide it.
        _pMethodTip->Hide();
        }
        }*/
    }
}

BOOL CScriptView::OnACDoubleClick()
{
    BOOL fSomethingWasSelected = FALSE;
    // In either case, hide the listbox
    _pAutoComp->Hide();
    if (_pAutoComp->GetCurSel() != LB_ERR)
    {
        CString strChoice;
        if (_pAutoComp->GetSelectedText(strChoice))
        {
            fSomethingWasSelected = TRUE;

            // Put this text in the document.
            // Delete what the user typed in first, if anything
            CPoint ptAutoStart = _autoCompleteWordStartPosition;
            if (ptAutoStart.x >= 0)
            {
                if (ptAutoStart != GetCursorPos())
                {
                    SetSelection(ptAutoStart, GetCursorPos());
                    // Then stick the chosen text in.
                    ReplaceSelection(strChoice);
                }
                else
                {
                    // If nothing typed, then jsut insert it.
                    PasteTextAtCursor(strChoice);
                }
                // It will be highlighted... so unhighlight it.
                SetSelection(GetCursorPos(), GetCursorPos());

                _pAutoComp->RememberChoice(strChoice);
            }
            // else oh well
        }
    }
    return fSomethingWasSelected;
}

void CScriptView::OnToggleComment()
{
    if (GetDocument() && GetDocument()->GetScriptId().Language() == LangSyntaxSCI)
    {
        // This is for SCI Script only. There are no multiline comments, so this helps with that.
        // Get the current block of code. See if the lines predominatly have comments or not.
        // Then toggle that.
        CPoint ptStart, ptEnd;
        GetSelection(ptStart, ptEnd);

        // Possibly start at the *next* line so used doesn't have to be exact when selecting text.
        if ((ptStart.y < ptEnd.y) && (ptStart.x > 0))
        {
            ptStart.y++;
            ptStart.x = 0;
        }

        int isComment = 0;
        for (int line = ptStart.y; line <= ptEnd.y; line++)
        {
            PCSTR pszChars = GetLineChars(line);
            isComment += (pszChars && pszChars[0] == ';') ? 1 : -1;
        }
        
        if (isComment >= 0)
        {
            // Uncomment
            RemoveFromLines(ptStart.y, ptEnd.y, ';');
        }
        else
        {
            // Comment
            PrependToLines(ptStart.y, ptEnd.y, ";;;");
        }
    }
}

#if 0
void CScriptView::OnVisualScript()
{
	SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
    ClassBrowserLock lock(browser);
    lock.Lock();
    std::string fullPath = GetDocument()->GetScriptId().GetFullPath();
    const sci::Script *pScript = browser.GetLKGScript(fullPath);
    CVisualScriptDialog dialog(const_cast<sci::Script *>(pScript), &browser);
    dialog.DoModal();
}
#endif

void CScriptView::OnUpdateGotoScriptHeader(CCmdUI *pCmdUI)
{
    if (_gotoScriptText.IsEmpty())
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        pCmdUI->Enable(TRUE);
        TCHAR szBuffer[MAX_PATH];
        StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Open %s"), (PCTSTR)_gotoScriptText);
        pCmdUI->SetText(szBuffer);
    }
}

void CScriptView::OnUpdateGotoDefinition(CCmdUI *pCmdUI)
{
    if (_gotoDefinitionText.IsEmpty())
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        pCmdUI->Enable(TRUE);
        TCHAR szBuffer[MAX_PATH];
        StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Go to definition: %s"), (PCTSTR)_gotoDefinitionText);
        pCmdUI->SetText(szBuffer);
    }
}

void CScriptView::HighlightLine(CPoint pt)
{
    int cChars = GetLineLength(pt.y);
    CPoint ptStart(0, pt.y);
    CPoint ptEnd(cChars, pt.y);
    SetSelection(ptStart, ptEnd);
    SetAnchor(ptStart);
}


//
// Returns the brace balance of a line.  If positive, then
// there are more ( than )
//
int CScriptView::GetParenBalance(int nLineIndex)
{
    bool fCpp = (GetDocument()->GetScriptId().Language() == LangSyntaxCpp);
    ASSERT(nLineIndex >= 0);
    LPCTSTR pszChars = GetLineChars(nLineIndex);
    int nLength = GetLineLength(nLineIndex);
    int nPos = 0;
    int nBraceCount = 0;
    BOOL fDontCountQuote = FALSE;
    int nDontCountCurly = 0;
    while (nPos < nLength)
    {
        if (!fCpp)
        {
            switch (pszChars[nPos])
            {
                // Attempt to not count ( ) that are in { } or "
                // Not quite exact, but good enough.
            case '{':
                nDontCountCurly++;
                break;
            case '}':
                nDontCountCurly--;
                break;
            case '"':
                // Not quite exact, but should be good enough.
                break;
            }
        }

        if ((nDontCountCurly == 0) && !fDontCountQuote)
        {
            if (fCpp)
            {
                switch (pszChars[nPos])
                {
                case '{':
                    nBraceCount++;
                    break;
                case '}':
                    if (nBraceCount > 0)
                    {
                        nBraceCount--;
                    }
                    break;
                }
            }
            else
            {
                switch (pszChars[nPos])
                {
                case '(':
                    nBraceCount++;
                    break;
                case ')':
                    if (nBraceCount > 0)
                    {
                        nBraceCount--;
                    }
                    break;
                }
            }
        }
        nPos++;
    }
    return nBraceCount;
}

bool _IsFileChar(char ch)
{
    return isalnum(ch) || ch == '.' || ch == '_';
}


//
// Puts the name of the header file under the cursor in strHeader.
// Returns true if it's a header file.
//
BOOL CScriptView::GetHeaderFile(CPoint pt, CString &strHeader)
{
	ASSERT_VALIDTEXTPOS(pt);
    BOOL fRet = FALSE;
	int nLength = GetLineLength(pt.y);
	LPCTSTR pszChars = GetLineChars(pt.y);
    int iEnd = pt.x;
    if ((iEnd >= 0) && (iEnd < nLength))
    {
        // Forward
        while ((iEnd < nLength) && _IsFileChar(pszChars[iEnd]))
        {
            iEnd++;
        }

        int iStart = pt.x;
        // Backward
        while ((iStart >= 0) && _IsFileChar(pszChars[iStart]))
        {
            iStart--;
        }
        iStart++;

        if (iEnd > iStart)
        {
            std::string potentialHeaderName(&pszChars[iStart], iEnd - iStart);
            if (IsCodeFile(potentialHeaderName))
            {
                // We have a header file.
                TCHAR szBuf[MAX_PATH];
                StringCchCopyN(szBuf, ARRAYSIZE(szBuf), &pszChars[iStart], iEnd - iStart);
                strHeader = szBuf;
                fRet = TRUE;
            }
        }
    }
    return fRet;
}

void CScriptView::GetSelectedText(CString &text)
{
    CPoint ptStart, ptEnd;
    GetSelection(ptStart, ptEnd);
    if (ptStart != ptEnd)
    {
        GetText(ptStart, ptEnd, text);
    }
}



void CScriptView::OnGoto()
{
    CGotoDialog dialog;
    if (IDOK == dialog.DoModal())
    {
        int nLine = dialog.GetLineNumber();
        nLine = min(GetLineCount(), nLine);
        nLine--; // 0-based, but dialog is 1 based.
        nLine = max(0, nLine);
        int nChar = min(GetLineLength(nLine), GetCursorPos().x);
        SetCursorPos(CPoint(nChar, nLine));
        EnsureVisible(CPoint(nChar, nLine));
    }
}

LRESULT CScriptView::OnHoverTipReady(WPARAM wParam, LPARAM lParam)
{
    if (_hoverTipScheduler)
    {
        std::unique_ptr<HoverTipResponse> response = _hoverTipScheduler->RetrieveResponse(_lastHoverTipParse);
        if (response)
        {
            CPoint ptClient = response->Location;
            if (response && !response->Result.empty())
            {
                // Place the tooltip below the line and a little to the left.
                CPoint ptTip = ptClient;
                ptTip.x = max(0, ptTip.x - response->Result.OriginalText.size());
                ptTip = TextToClient(ptTip);
                ptTip.y += GetLineHeight();

                CRect rcScript;
                GetWindowRect(&rcScript);
                _pwndToolTip->Show(CPoint(rcScript.left + ptTip.x, rcScript.top + ptTip.y), response->Result.strTip);

                SetTimer(TOOLTIPEXPIRE_ID, TOOLTIPEXPIRE_TIMEOUT, nullptr);
            }
        }
    }

    return 0;
}

LRESULT CScriptView::OnAutoCompleteReady(WPARAM wParam, LPARAM lParam)
{
    std::unique_ptr<AutoCompleteResult> result = _pACThread->GetResult(wParam);
    if (result)
    {
        _autoCompleteWordStartPosition = result->OriginalLimit;

        auto &choices = result->choices;
        if (choices.size() == 0)
        {
            // No results
            if (_pAutoComp->IsWindowVisible())
            {
                _pAutoComp->Hide();
            }
        }
        else
        {
            if (result->fResultsChanged)
            {
                _pAutoComp->UpdateChoices(choices);
            }

            // Make it visible (or update size of already visible)
            CPoint ptClient = TextToClient(GetCursorPos());
            ClientToScreen(&ptClient);
            ptClient.x -= 10; // Just offset a little
            ptClient.y += GetLineHeight() + 2; // Move it below the lien.
            _pAutoComp->Show(ptClient);
        }
    }
    return 0;
}


// CScriptView diagnostics

#ifdef _DEBUG
void CScriptView::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void CScriptView::Dump(CDumpContext& dc) const
{
	CCrystalEditView::Dump(dc);
}

CScriptDocument* CScriptView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CScriptDocument)));
	return static_cast<CScriptDocument*>(m_pDocument);
}
#endif //_DEBUG


// CScriptView message handlers


