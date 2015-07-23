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
    ON_COMMAND(ID_VISUALSCRIPT, OnVisualScript)
    //ON_UPDATE_COMMAND_UI(ID_VISUALSCRIPT, OnUpdateAlwaysOn)
END_MESSAGE_MAP()

// CScriptView construction/destruction

CScriptView::CScriptView()
{
    _fInOnChar = FALSE;
    _pStream = nullptr;
    _pLimiter = nullptr;
    _pwndToolTip = nullptr;
    _ptLastMM = CPoint(0, 0);
    _ptToolTipWord = CPoint(0, 0);
    _pAutoComp = nullptr;
    _pACThread = nullptr;
    _pMethodTip = nullptr;
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
        CPoint pt = _pACThread->GetCompletedPosition();
        CPoint ptRecalced(pt);
        pContext->RecalcPoint(ptRecalced);
        if (ptRecalced != pt)
        {
            //_pACThread->ResetPosition();
        }
    }
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

CScriptView::~CScriptView()
{
    delete _pStream;
    delete _pLimiter;
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


LPTSTR s_apszTopLevelKeywordListSCI[] =
{
    _T("define"),
	_T("include"),
	_T("use"),
	_T("script"),
	_T("class"),
	_T("public"),
	_T("local"),
	_T("instance"),
	_T("procedure"),
    _T("var"),
    _T("string"),
    _T("synonyms"),
    _T("version"),
    _T("exports"),
	nullptr
};
std::vector<std::string> g_TopLevelKeywordsSCI;
bool _IsTopLevelKeywordSCI(const std::string &word)
{
    // -1 since it ends with nullptr.
    return _IsKeyword(word, g_TopLevelKeywordsSCI, s_apszTopLevelKeywordListSCI, ARRAYSIZE(s_apszTopLevelKeywordListSCI) - 1);
}

LPTSTR s_apszTopLevelKeywordListCpp[] =
{
    _T("define"),
	_T("include"),
	_T("use"),
	_T("class"),
	_T("public"),
	_T("private"),
	_T("instance"),
    _T("synonyms"),
	nullptr
};
std::vector<std::string> g_TopLevelKeywordsCpp;
bool _IsTopLevelKeywordCpp(const std::string &word)
{
    // -1 since it ends with nullptr.
    return _IsKeyword(word, g_TopLevelKeywordsCpp, s_apszTopLevelKeywordListCpp, ARRAYSIZE(s_apszTopLevelKeywordListCpp) - 1);
}

bool IsTopLevelKeyword(LangSyntax lang, const std::string &word)
{
    return (lang == LangSyntaxCpp) ? _IsTopLevelKeywordCpp(word) : _IsTopLevelKeywordSCI(word);
}

LPTSTR s_apszCodeLevelKeywordListSCI[] =
{
	_T("asm"),
	_T("break"),
	_T("send"),
	_T("case"),
	_T("switch"),
	_T("properties"),
	_T("method"),
	_T("for"),
	_T("if"),
	_T("return"),
	_T("default"),
	_T("do"),
	_T("while"),
	_T("else"),
	_T("rest"),
    _T("super"),
    _T("or"),
    _T("and"),
    _T("not"),
    _T("neg"),
    _T("of"),
    _T("scriptNumber"),
    nullptr,
};
std::vector<std::string> g_CodeLevelKeywordsSCI;
bool _IsCodeLevelKeywordSCI(const std::string &word)
{
    // -1 since it ends with nullptr.
    return _IsKeyword(word, g_CodeLevelKeywordsSCI, s_apszCodeLevelKeywordListSCI, ARRAYSIZE(s_apszCodeLevelKeywordListSCI) - 1);
}

LPTSTR s_apszCodeLevelKeywordListCpp[] =
{
	_T("break"),
	_T("case"),
	_T("switch"),
	_T("for"),
	_T("if"),
	_T("return"),
	_T("default"),
	_T("do"),
	_T("while"),
	_T("else"),
	_T("rest"),
    _T("super"),
    _T("scriptNumber"),
    _T("res"),
    _T("true"),
    _T("false"),
    _T("null"),
    _T("cast"),
    nullptr,
};
std::vector<std::string> g_CodeLevelKeywordsCpp;
bool _IsCodeLevelKeywordCpp(const std::string &word)
{
    // -1 since it ends with nullptr.
    return _IsKeyword(word, g_CodeLevelKeywordsCpp, s_apszCodeLevelKeywordListCpp, ARRAYSIZE(s_apszCodeLevelKeywordListCpp) - 1);
}

bool IsCodeLevelKeyword(LangSyntax lang, const std::string &word)
{
    return (lang == LangSyntaxCpp) ? _IsCodeLevelKeywordCpp(word) : _IsCodeLevelKeywordSCI(word);
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
    _T("paramTotal"),
    _T("self"),
    _T("super"),
	nullptr
};
std::vector<std::string> g_ValueKeywords;
bool IsValueKeyword(const std::string &word)
{
    // -1 since it ends with nullptr.
    return _IsKeyword(word, g_ValueKeywords, s_apszSCIValueKeywordList, ARRAYSIZE(s_apszSCIValueKeywordList) - 1);
}

bool IsSCIKeyword(LangSyntax lang, const std::string &word)
{
    return (IsValueKeyword(word) || IsCodeLevelKeyword(lang, word) || IsTopLevelKeyword(lang, word) ||
            ((lang == LangSyntaxCpp) && IsCPPTypeKeyword(word)));
}

static BOOL IsSCISelector(LPCTSTR pszChars, int nLength)
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

static BOOL IsSCINumber(LPCTSTR pszChars, int nLength)
{
    if (nLength > 1 && pszChars[0] == '$')
	//if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
	{
		//for (int I = 2; I < nLength; I++)
        for (int I = 1; I < nLength; I++)
		{
			if (isdigit(pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
										(pszChars[I] >= 'a' && pszChars[I] <= 'f'))
				continue;
			return FALSE;
		}
		return TRUE;
	}
	if (! isdigit(pszChars[0]))
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

//
// SCI syntax highlighting
//
DWORD CScriptView::ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
    LangSyntax lang = GetDocument()->GetScriptId().Language();
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
				DEFINE_BLOCK(nPos, COLORINDEX_PREPROCESSOR);
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
				if (IsSCIKeyword(lang,  pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
				}
				else if (IsSCINumber(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
				}
                else if (IsSCISelector(pszChars + nIdentBegin, I - nIdentBegin))
                {
	                DEFINE_BLOCK(nIdentBegin, COLORINDEX_PREPROCESSOR);
                }
				bRedefineBlock = TRUE;
				bDecIndex = TRUE;
				nIdentBegin = -1;
			}
		}
	}

	if (nIdentBegin >= 0)
	{
		if (IsSCIKeyword(lang, pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
		}
		else
		if (IsSCINumber(pszChars + nIdentBegin, I - nIdentBegin))
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
        if (fPossiblyOnWord )
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
                    BOOL fDisableMenuOption = TRUE;
                    if ((ptRight != ptLeft))
                    {
                        // This is a potential vocab string.
    	                GetText(ptLeft, ptRight, _contextMenuText);
                        if (IsValidVocabString(_contextMenuText, FALSE))
                        {
                            StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Add \"%s\" as"), (PCTSTR)_contextMenuText);
                            mii.fMask = MIIM_STRING;
                            mii.dwTypeData = szBuffer;
                            fDisableMenuOption = FALSE;
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
            ToolTipResult result = _DoToolTipParse(ptRight);
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
                _availableObjects = make_unique<AvailableObjects>();
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
                _availableMethods = make_unique<AvailableMethods>();
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
        CPoint ptWordRight;
        BOOL fInEditableArea = _ScreenToWordRight(point, ptWordRight);
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


void CScriptView::OnUpdateAddAs(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!_contextMenuText.IsEmpty());
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

    _pLimiter = new CScriptStreamLimiter(LocateTextBuffer());
    if (_pLimiter)
    {
        _pStream = new CCrystalScriptStream(_pLimiter);
    }

#ifdef SCI_AUTOCOMPLETE
    if (_pACThread)
    {
        _pACThread->InitAutoComplete(_pStream->begin(), _pLimiter);
    }
#endif
}

//
// Given a screen coordinate, finds the corresponding point in text coordinates
// that would be used for parsing a tooltip
//
BOOL CScriptView::_ScreenToWordRight(CPoint ptClient, CPoint &ptWordRight)
{
    AdjustTextPoint(ptClient);
    BOOL fRet = _ClientToTextNoMargin(ptClient, ptWordRight);
    if (fRet)
    {
        // Move to the end of the word to the right.
        ptWordRight = WordToRight(ptWordRight);
    }
    return fRet;
}




ToolTipResult CScriptView::_DoToolTipParse(CPoint pt)
{
    ToolTipResult result;
    BOOL fRet = FALSE;
    if (appState->AreHoverTipsEnabled())
    {
        CScriptStreamLimiter limiter(LocateTextBuffer());
        CCrystalScriptStream stream(&limiter);
        sci::Script script;
        SyntaxContext context(stream.begin(), script, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), false);
        limiter.Limit(LineCol(pt.y, pt.x));
        CCrystalScriptStream::const_iterator it = stream.begin();
        
        class CToolTipSyntaxParserCallback : public ISyntaxParserCallback
        {
        public:
            CToolTipSyntaxParserCallback(SyntaxContext &context, ToolTipResult &result) : _context(context), _result(result) {}

            void Done()
            {
                _result = GetToolTipResult<SyntaxContext>(&_context);
            }
        private:
            SyntaxContext &_context;
            ToolTipResult &_result;
        };

        CToolTipSyntaxParserCallback callback(context, result);
        limiter.SetCallback(&callback);

        CPrecisionTimer timer;
        timer.Start();
        g_Parser.Parse(script, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), nullptr, false, &context); // REVIEW: pass in context?
        char sz[100];
        StringCchPrintf(sz, ARRAYSIZE(sz), "Time was %d\n", (int)(timer.Stop() * 1000));
        OutputDebugString(sz);
    }
    return result;
}

void CScriptView::_BringUpToolTip(CPoint ptClient)
{
    // Kill any expire id.  We don't want an old one coming along and expiring a new tooltip
    KillTimer(TOOLTIPEXPIRE_ID);

    if (_pwndToolTip)
    {
        BOOL fInEditableArea = _ScreenToWordRight(ptClient, _ptToolTipWord);
        if (fInEditableArea)
        {
            ToolTipResult result = _DoToolTipParse(_ptToolTipWord);
            if (!result.empty())
            {
                // Place the tooltip below the line and a little to the left.
                CPoint ptTip = ClientToText(ptClient);
                // Unfortunately we can't to TextToClient unless it's a valid text pos.
                ptTip.x = ptClient.x + 10;
                ptTip.y = TextToClientLine(ptTip.y + 1);

                CRect rcScript;
                GetWindowRect(&rcScript);
                _pwndToolTip->Show(CPoint(rcScript.left + ptTip.x, rcScript.top + ptTip.y), result.strTip);

                SetTimer(TOOLTIPEXPIRE_ID, TOOLTIPEXPIRE_TIMEOUT, nullptr);
            }
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
    ASSERT(_hEventACDone);
    ASSERT(_hEventDoAC);

    if (!appState->IsCodeCompletionEnabled() && !appState->IsParamInfoEnabled())
    {
        // Nothing to see here...
        return;
    }

    // Set our current position
    CPoint pt = GetCursorPos();
    // And do the autocomplete
    
	// Wow, lots of problems, I forget what.
    //AutoCompleteResult result = _pACThread->DoAutoComplete(pt);

/*
    // Wake up the autocomplete parsecontext, after telling it where we are.
    CPoint pt = GetCursorPos();
    _pParseContext->SetCurrentPos(_pStream, _pLimiter, FALSE, LINECHAR2POS(pt.y, pt.x));
    _pParseContext->SetOptions(PARSECONTEXTOPTION_FORAUTOCOMPLETE);
    SetEvent(_hEventDoAC);
    // Wait until it's done.
    if (WAIT_OBJECT_0 == WaitForSingleObject(_hEventACDone, INFINITE))
    {
        // We have a result.
        ResetEvent(_hEventACDone); // Reset for next time.
        CAutoCompleteResult &result = _pParseContext->GetResult();

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
        }

        if (appState->IsCodeCompletionEnabled())
        {
            CAutoCompleteChoiceArray &choices = result.choices;
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
                if (result.fResultsChanged)
                {
                    _pAutoComp->ResetContent();
                    for (size_t i = 0; i < choices.size(); i++)
                    {
                        CAutoCompleteChoice &choice = choices[i];
                        int iIndex = _pAutoComp->AddString(choice.GetText().c_str());
                        if (iIndex != LB_ERR)
                        {
                            _pAutoComp->SetItemData(iIndex, choice.GetIcon());
                        }
                    }
                }
                if (!_pAutoComp->IsWindowVisible())
                {
                    // Make it visible.
                    CPoint ptClient = TextToClient(GetCursorPos());
                    ClientToScreen(&ptClient);
                    ptClient.x -= 10; // Just offset a little
                    ptClient.y += GetLineHeight() + 2; // Move it below the lien.
                    _pAutoComp->Show(ptClient);
                }
            }
            std::string &strFindText = _pParseContext->GetResult().strFindText;
            if (!strFindText.empty())
            {
                _pAutoComp->Highlight(strFindText.c_str());
            }
        }
    }
    else
    {
        // Uh oh.
    }
    */
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
            CPoint ptAutoStart(_pACThread->GetInsertPosition());
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
        }
    }
    return fSomethingWasSelected;
}


void CScriptView::AttachToAutoComp()
{
//    _pParseContext->InitializeAutoComplete(_pStream, _pLimiter, &_hEventDoAC, &_hEventACDone);
}

void CScriptView::OnVisualScript()
{
#if 0
	SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
    ClassBrowserLock lock(browser);
    lock.Lock();
    std::string fullPath = GetDocument()->GetScriptId().GetFullPath();
    const sci::Script *pScript = browser.GetLKGScript(fullPath.c_str());
    CVisualScriptDialog dialog(const_cast<sci::Script *>(pScript), &browser);
    dialog.DoModal();
#endif
}

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



//
// Puts the name of the header file under the cursor, in strHeader.
// Returns true if it's a header file.
//
BOOL CScriptView::GetHeaderFile(CPoint pt, CString &strHeader)
{
	ASSERT_VALIDTEXTPOS(pt);
    BOOL fRet = FALSE;
	int nLength = GetLineLength(pt.y);
	LPCTSTR pszChars = GetLineChars(pt.y);
    int iPos = pt.x;
    while ((iPos > 0) && (iPos < nLength))
    {
        if (! isalnum(pszChars[iPos]) && pszChars[iPos] != _T('_') && pszChars[iPos] != _T('.'))
        {
            break;
        }
        iPos --;
    }

    int iPosStart = iPos + 1;
    if (iPos > 0 && pszChars[iPos] == _T('"'))
    {
        iPos ++;
        // We're good so far.
	    while (iPos < nLength)
	    {
		    if (! isalnum(pszChars[iPos]) && pszChars[iPos] != _T('_'))
			    break;
		    iPos ++;
	    }

        if (pszChars[iPos] == '.')
        {
            iPos++;
            // Go until not alnum
            while (iPos < nLength)
            {
                if (!isalnum(pszChars[iPos]) && pszChars[iPos] != _T('_'))
                {
                    break;
                }
                iPos++;
            }

            std::string potentialHeaderName(&pszChars[iPosStart], iPos - iPosStart);
            if (IsCodeFile(potentialHeaderName))
            {
                // We have a header file.
                TCHAR szBuf[MAX_PATH];
                StringCchCopyN(szBuf, ARRAYSIZE(szBuf), &pszChars[iPosStart], iPos - iPosStart);
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


