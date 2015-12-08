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
// ScriptComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ScriptComboBox.h"
#include "ScriptDocument.h"
#include "ClassBrowser.h"
#include "ScriptOM.h"
#include "ScriptView.h"
#include "CObjectWrap.h"
#include "AutoCompleteSourceTypes.h"
#include "Task.h"
#include "CrystalScriptStream.h"
#include "SyntaxParser.h"

using namespace sci;
using namespace std;

// CScriptComboBox
#define UWM_RESPONSEREADY (WM_APP + 0)
#define TIMER_UPDATECOMBO 1234

struct ParsePayload
{
    ParsePayload(ScriptId script, CCrystalTextBuffer *pBuffer);

    CScriptStreamLimiter Limiter;
    CCrystalScriptStream Stream;
    ScriptId Script;
};

ParsePayload::ParsePayload(ScriptId script, CCrystalTextBuffer *pBuffer) :
    Limiter(pBuffer), Stream(&Limiter), Script(script) {}

CScriptComboBox::CScriptComboBox() : _lastTaskId(-1), _pDoc(nullptr), _fDroppedDown(false), _fIgnorePosChanged(false)
{
    // Load an image list.
    _imageList.Create(IDB_CLASSBROWSER, 16, 0, RGB(255, 0, 255));
}

CScriptComboBox::~CScriptComboBox()
{
}

void CScriptComboBox::UpdateNonView(CObject *pObject)
{
    ScriptChangeHint hint = GetHint<ScriptChangeHint>(pObject);
    if (_pDoc && (IsFlagSet(hint, ScriptChangeHint::Pos)) && !_fIgnorePosChanged)
    {
        SetTimer(TIMER_UPDATECOMBO, 500, nullptr);
    }
}

void CScriptComboBox::SetDocument(CDocument *pDoc)
{
    assert(pDoc == nullptr|| pDoc->IsKindOf(RUNTIME_CLASS(CScriptDocument)));
    _lastTaskId = -1;
    if (pDoc)
    {
        _pDoc = static_cast<CScriptDocument*>(pDoc);
        _pDoc->AddNonViewClient(this); // (assumes dupes are filtered out)

        // Don't update ourselves synchronously...
        SetTimer(TIMER_UPDATECOMBO, 500, nullptr);
    }
    else
    {
        // Clear combobox
        ResetContent();
    }
}


BEGIN_MESSAGE_MAP(CScriptComboBox, CExtComboBox)
    ON_WM_TIMER()
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
    ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
    ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropDown)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_MESSAGE(UWM_RESPONSEREADY, OnResponseReady)
END_MESSAGE_MAP()

int CScriptComboBox::OnCreate(CREATESTRUCT *createStruct)
{
    if (__super::OnCreate(createStruct) == -1)
        return -1;

    _scheduler = std::make_unique<BackgroundScheduler<ParsePayload, sci::Script>>(this->GetSafeHwnd(), UWM_RESPONSEREADY);

    return 0;
}


void CScriptComboBox::OnDestroy()
{
    if (_scheduler)
    {
        _scheduler->Exit();
    }
    __super::OnDestroy();
}

bool CScriptComboBox::_SpawnScriptTask()
{
    bool isSuccessful = false;
    if (appState->_fBrowseInfo)
    {
        // NOTE: the document could have been closed, and the CDocument deleted.
        // Ensure that the active document is still us.
        CFrameWnd *pFrame = static_cast<CFrameWnd*>(AfxGetMainWnd());
        CFrameWnd *pMDIFrame = pFrame->GetActiveFrame();
        if (pMDIFrame)
        {
            CDocument *pDocActive = pMDIFrame->GetActiveDocument();
            if (_pDoc && (pDocActive == _pDoc))
            {
                // Do a full parse (e.g. don't ask for LKG)
                _lastTaskId = _scheduler->SubmitTask(
                    make_unique<ParsePayload>(_pDoc->GetScriptId(), _pDoc->GetTextBuffer()),
                    [](ITaskStatus &status, ParsePayload &payload)
                {
                    std::unique_ptr<Script> pScript(new Script(payload.Script));
                    if (!SyntaxParser_Parse(*pScript, payload.Stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), nullptr))
                    {
                        pScript.reset(nullptr);
                    }
                    return pScript;
                }
                    );
                isSuccessful = true;
            }
        }
    }
    return isSuccessful;
}

LRESULT CScriptComboBox::OnResponseReady(WPARAM wParam, LPARAM lParam)
{
    unique_ptr<sci::Script> response = _scheduler->RetrieveResponse(_lastTaskId);
    if (response)
    {
        _script = move(response);

        CPoint pt;
        CFrameWnd *pFrame = static_cast<CFrameWnd*>(AfxGetMainWnd());
        CFrameWnd *pMDIFrame = pFrame->GetActiveFrame();
        if (pMDIFrame)
        {
            CDocument *pDocActive = pMDIFrame->GetActiveDocument();
            if (_pDoc && (pDocActive == _pDoc))
            {
                CView *pView = pMDIFrame->GetActiveView();
                if (pView && pView->IsKindOf(RUNTIME_CLASS(CScriptView)))
                {
                    CScriptView *pScriptView = static_cast<CScriptView*>(pView);
                    pt = pScriptView->GetCursorPos();
                }
            }
        }

        _OnUpdateFromScript(_script.get(), pt);
    }
    return 0;
}

void CScriptComboBox::_ParseAndUpdate(bool fForce)
{
    if (_fDroppedDown || !appState->IsScriptNavEnabled())
    {
        // Don't do it while the user is looking at the combobox
        SetTimer(TIMER_UPDATECOMBO, 1000, nullptr); // Try again in a second
    }
    else
    {
        if (!_SpawnScriptTask() && fForce)
        {
            // TODO: Clean out combo?
        }
    }
}

// CScriptComboBox message handlers
void CScriptComboBox::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMER_UPDATECOMBO)
    {
        KillTimer(nIDEvent);
        _ParseAndUpdate();
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

int CScriptComboBox::_AddItem(const std::string &str, sci::SyntaxNode *pPos)
{
    int iIndex = InsertString(-1, str.c_str());
    SetItemData(iIndex, reinterpret_cast<DWORD_PTR>(pPos));
    return iIndex;
}

void CClassComboBox::_OnUpdateFromScript(const sci::Script *pScript, CPoint pt)
{
    SetRedraw(FALSE);
    ResetContent();
    int iSelection = -1;
    if (pScript != nullptr)
    {
        // Add all the classes, and select the one that the point is in.
		for (auto &classDef : pScript->GetClasses())
		{
			int iIndex = _AddItem(classDef->GetName(), classDef.get());
			if (classDef->GetLineNumber() <= pt.y && classDef->GetEndLineNumber() >= pt.y)
			{
				iSelection = iIndex;
			}

			// Put in the methods of this class too...
			for (auto &method : classDef->GetMethods())
			{
				std::string name = GetMethodTrackingName(classDef.get(), *method);
				int iIndex = _AddItem(name, method.get());
				if (method->GetLineNumber() <= pt.y && method->GetEndLineNumber() >= pt.y)
				{
					iSelection = iIndex; // This might override the class selection, but that's ok
				}
			}
		}

        // Add the procedures too
		for (auto &procedure : pScript->GetProcedures())
		{
			int iIndex = _AddItem(procedure->GetName(), procedure.get());
			if (procedure->GetLineNumber() <= pt.y && procedure->GetEndLineNumber() >= pt.y)
			{
				iSelection = iIndex;
			}
		}

        if (iSelection != -1)
        {
            SetCurSel(iSelection);
        }
    }
    SetRedraw(TRUE);
    Invalidate();
}

void CClassComboBox::OnSelChange()//NMHDR *pnmhdr, LRESULT *plResult)
{
    int iSelected = GetCurSel();
    if (iSelected != CB_ERR)
    {
        CPoint pt;
        sci::SyntaxNode *pPos = reinterpret_cast<sci::SyntaxNode*>(GetItemData(iSelected));

        CFrameWnd *pFrame = static_cast<CFrameWnd*>(AfxGetMainWnd());
        CFrameWnd *pMDIFrame = pFrame->GetActiveFrame();
        if (pMDIFrame)
        {
            CDocument *pDocActive = pMDIFrame->GetActiveDocument();
            if (_pDoc && (pDocActive == _pDoc))
            {
                _fIgnorePosChanged = true; // We'll get a pos changed here, so ignore it, since we're the one who caused it.
                appState->OpenScriptAtLine(_pDoc->GetScriptId(), pPos->GetLineNumber() + 1); // off by one
                _fIgnorePosChanged = false;
            }
        }
    }
}

void CClassComboBox::OnCloseUp()
{
    _fDroppedDown = false;
}

void CClassComboBox::OnDropDown()
{
    _fDroppedDown = true;
}

int _MapTypeToIndex(sci::SyntaxNode *pNode)
{
    AutoCompleteIconIndex iconIndex = AutoCompleteIconIndex::Unknown;
    switch (pNode->GetNodeType())
    {
    case sci::NodeTypeClassDefinition:
        iconIndex = AutoCompleteIconIndex::Class;
        break;
    case sci::NodeTypeFunction:
        {
            sci::FunctionBase *pFunction = static_cast<sci::FunctionBase*>(pNode);
            if (pFunction->GetOwnerClass())
            {
                iconIndex = AutoCompleteIconIndex::Selector;
            }
            else
            {
                iconIndex = AutoCompleteIconIndex::Procedure;
            }
        }
        break;
    default:
        assert(false);
    }
    return (int)iconIndex;
}

void CScriptComboBox::DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
    if (pDrawItemStruct->itemID != -1)
    {
        CDC dc;
        dc.Attach(pDrawItemStruct->hDC);    

        // Save these values to restore them when done drawing.
        COLORREF crOldTextColor = dc.GetTextColor();
        COLORREF crOldBkColor = dc.GetBkColor();
        int iMode = dc.GetBkMode();

        CString str;
        GetLBText(pDrawItemStruct->itemID, str);
        CRect rc = pDrawItemStruct->rcItem;

        sci::SyntaxNode *pNode = reinterpret_cast<sci::SyntaxNode*>(GetItemData(pDrawItemStruct->itemID));
        int iImageListIndex = _MapTypeToIndex(pNode);

        // Draw a little icon next to the guy.
        _imageList.Draw(&dc, iImageListIndex, CPoint(rc.left, rc.top), ILD_NORMAL);
        rc.left += 18;

        // Now draw the text
        if ((pDrawItemStruct->itemAction | ODA_SELECT) && (pDrawItemStruct->itemState & ODS_SELECTED))
        {
            // Show something for the selected it.
            dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
            dc.FillSolidRect(&rc, ::GetSysColor(COLOR_HIGHLIGHT));
        }
        else
        {
            dc.FillSolidRect(&rc, crOldBkColor);
        }

        rc.left += 3; // Offset a little more.
        dc.DrawText(str, &rc, DT_SINGLELINE);

        dc.SetTextColor(crOldTextColor);
        dc.SetBkColor(crOldBkColor);
        dc.SetBkMode(iMode);
        dc.Detach();
    }
}

void CScriptComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIStruct)
{
    //__super::MeasureItem(lpMIStruct);
    // CExtComboBoxBase::MeasureItem calls GetCount, which will crash on comctl32 v5, we get this
    // WM_MEASUREITEM message before WM_CREATE has finished (and the combo's listbox is not yet created)
    lpMIStruct->itemHeight = 15; // We're hard-coding this, but it makes the combobox look the best in prof-uis
}