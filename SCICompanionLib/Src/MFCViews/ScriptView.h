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
// SCICompilerView.h : interface of the CScriptView class
//
#pragma once

#include "CCrystalEditView.h"
#include "Vocab000.h"
#include "IntellisenseListBox.h"
#include "ColoredToolTip.h"
#include "ToolTipResult.h"
#include "CrystalScriptStream.h"

template<typename _TPayload, typename _TResponse>
class BackgroundScheduler;

namespace sci
{
    class Script;
}
class CMethodInfoTip;
class AutoCompleteThread2;
class CScriptDocument;
class AvailableObjects;
class AvailableMethods;

struct HoverTipPayload
{
    HoverTipPayload(ScriptId scriptId, CCrystalTextBuffer *pBuffer, CPoint ptLimit) : ScriptId(scriptId), Limiter(pBuffer, ptLimit, 0), Stream(&Limiter), Location(ptLimit) {}
    CScriptStreamLimiter Limiter;
    CPoint Location;
    CCrystalScriptStream Stream;
    ScriptId ScriptId;
};

struct HoverTipResponse
{
    ToolTipResult Result;
    CPoint Location;
};

class CScriptView : public CCrystalEditView, public IAutoCompleteClient
{
protected: // create from serialization only
	CScriptView();
	DECLARE_DYNCREATE(CScriptView)

// Attributes
public:
	CScriptDocument* GetDocument() const;
    void OnCompile();
    void HighlightLine(CPoint pt);
    int GetParenBalance(int nLineIndex);
    BOOL GetHeaderFile(CPoint pt, CString &strHeader);
    void GetSelectedText(CString &text);

    void SetAutoComplete(CIntellisenseListBox *pAutoComp, CColoredToolTip *pMethodToolTip, CColoredToolTip *pToolTip, AutoCompleteThread2 *pThread, BackgroundScheduler<HoverTipPayload, HoverTipResponse> *hoverTipScheduler)
    {
        _pAutoComp = pAutoComp;
        _pACThread = pThread;
        _pMethodTip = pMethodToolTip;
        _pwndToolTip = pToolTip;
        _hoverTipScheduler = hoverTipScheduler;
    }

// Operations
public:
// Overrides
	public:
    virtual void UpdateView(CCrystalTextView *pSource, CUpdateContext *pContext, DWORD dwFlags, int nLineIndex /*= -1*/);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual CCrystalTextBuffer *LocateTextBuffer();

    // IAutoCompleteClient
    BOOL OnACDoubleClick();

    void AttachToAutoComp();
    void OnVisualScript();

protected:

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual DWORD ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems);
    DWORD _ParseLineStudio(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems);
    DWORD _ParseLineSCI(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems);
    virtual void UpdateCaret();
    virtual void ScrollPosChanged();

// Generated message map functions
    afx_msg void OnContextMenu(CWnd *pWnd, CPoint pos);
    afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
    afx_msg void OnKillFocus(CWnd *pNewWnd);
    afx_msg void OnSetFocus(CWnd *pNewWnd);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnInsertObject();
    afx_msg void OnDestroy();
    afx_msg void OnInsertObjectAt();
    afx_msg void OnInsertObjectAtRange(UINT nID);
    afx_msg void OnInsertMethodAtRange(UINT nID);
    afx_msg void OnAddAsNoun();
    afx_msg void OnAddAsImperativeVerb();
    afx_msg void OnAddAsPreposition();
    afx_msg void OnAddAsQualifyingAdjective();
    afx_msg void OnAddAsRelativePronoun();
    afx_msg void OnAddAsIndicativeVerb();
    afx_msg void OnAddAsAdverb();
    afx_msg void OnAddAsArticle();
    afx_msg void OnAddAsSynonymOf();
    afx_msg void OnGotoScriptHeader();
    afx_msg void OnReloadSyntaxColors();
    afx_msg void OnGotoDefinition();
    afx_msg void OnIntellisense();
    afx_msg void OnGoto();
    afx_msg void OnUpdateAddAs(CCmdUI *pCmdUI);
    afx_msg void OnUpdateGotoScriptHeader(CCmdUI *pCmdUI);
    afx_msg void OnUpdateGotoDefinition(CCmdUI *pCmdUI);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()

    LRESULT OnAutoCompleteReady(WPARAM wParam, LPARAM lParam);
    LRESULT OnHoverTipReady(WPARAM wParam, LPARAM lParam);
    void _OnAddAs(WordClass dwClass);
    BOOL _ScreenToWordRight(CPoint ptClient, CPoint &ptUnder, CPoint &ptWordRight);
    void _TriggerHoverTipParse(CPoint pt);
    void _BringUpToolTip(CPoint ptClient);
    BOOL _ClientToTextNoMargin(CPoint ptClient, CPoint &ptText);
    void _OnInsertObject(bool currentPosition);

    CString _contextMenuText;
    CString _gotoScriptText;

    CIntellisenseListBox *_pAutoComp;
    CColoredToolTip *_pMethodTip;
    AutoCompleteThread2 *_pACThread; // Not owned by us
    BackgroundScheduler<HoverTipPayload, HoverTipResponse> *_hoverTipScheduler; // Not owned by us
    int _lastHoverTipParse;

    // Autocomplete
    BOOL _fInOnChar;
    CPoint _ptAC;

    CColoredToolTip *_pwndToolTip;
    CPoint _ptLastMM;
    CPoint _ptToolTipWord;
    CFont m_Font;

    // For "goto definition"
    CString _gotoDefinitionText;
    ScriptId _gotoScript;
    int _gotoLineNumber;

    CPoint _autoCompleteWordStartPosition;

    // For "Insert object"
    std::unique_ptr<AvailableObjects> _availableObjects;
    std::unique_ptr<AvailableMethods> _availableMethods;
};

#ifndef _DEBUG  // debug version in SCICompilerView.cpp
inline CScriptDocument* CScriptView::GetDocument() const
   { return reinterpret_cast<CScriptDocument*>(m_pDocument); }
#endif


