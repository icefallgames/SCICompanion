// OutputPane.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "OutputPane.h"

using std::vector;

// OutputPane

IMPLEMENT_DYNAMIC(OutputPane, CListBox)
OutputPane::OutputPane()
{
}

OutputPane::~OutputPane()
{
}


// OutputPane message handlers

void OutputPane::ClearResults()
{
    // Remove items from us.
    SendMessage(LB_RESETCONTENT, 0, 0);
    _compileResults.clear();

    // Use default Prof-UIS font...
    // HACK: See comment in CMDITabsDialogBar::AddTab.
    SetFont(&g_PaintManager->m_FontNormal);
}

void OutputPane::AddBatch(vector<CompileResult> &compileResults)
{
    vector<CompileResult>::const_iterator resultIt = compileResults.begin();
    while (resultIt != compileResults.end())
    {
        int iIndex = (int)SendMessage(LB_ADDSTRING, 0, (LPARAM)resultIt->GetMessage().c_str());
        if ((iIndex != LB_ERR) && (iIndex != LB_ERRSPACE))
        {
            // Make a copy of the compile result passed in, and put it in our array, at the index
            // corresponding to its position in the listbox.  We'll set the ITEMDATA of the listbox items
            // later.
            _compileResults.push_back(*resultIt);
            ASSERT(_compileResults.size() == (size_t)(iIndex + 1));
        }
        ++resultIt;
    }

    // Scroll the last line into view:
    SendMessage(LB_SETCARETINDEX, compileResults.size(), FALSE);
}

void OutputPane::FinishAdd()
{
    // That's the last of the results, so now we should update our listbox items with the addresses
    // of the CCompileResults in our array.  We couldn't do this before, because we were growing our
    // array as results were coming in, and so they could be re-allocated to new memory addresses.
    vector<CompileResult>::const_iterator resultIt = _compileResults.begin();
    int i = 0;
    while (resultIt != _compileResults.end())
    {
        SendMessage(LB_SETITEMDATA, i, (LPARAM)&(*resultIt));
        ++resultIt;
        ++i;
    }
}

BEGIN_MESSAGE_MAP(OutputPane, CListBox)
    ON_WM_CREATE()
    ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClick)
    ON_COMMAND(ID_EDIT_COPY, OnCopy)
END_MESSAGE_MAP()

int OutputPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

void OutputPane::OnCopy()
{
    std::string output;
    int cItems = (int)SendMessage(LB_GETCOUNT, 0, 0);
    for (int i = 0; i < cItems; i++)
    {
        char szBuffer[1000];
        if (SendMessage(LB_GETTEXTLEN, i, 0) < ARRAYSIZE(szBuffer))
        {
            if (SendMessage(LB_GETTEXT, i, (LPARAM)szBuffer))
            {
                CompileResult *pResult = (CompileResult*)SendMessage(LB_GETITEMDATA, i, 0);
                if (pResult)
                {
                    output += pResult->GetScript().GetTitle();
                }
                output += szBuffer;
                output += "\r\n";
            }
        }
    }
    size_t cchBuffer = output.length() + 1;
    GlobalAllocGuard globalAlloc(GMEM_MOVEABLE, cchBuffer * sizeof(char));
    if (globalAlloc.Global)
    {
        GlobalLockGuard<PSTR> globalLock(globalAlloc);
        if (globalLock.Object)
        {
            StringCchCopy(globalLock.Object, cchBuffer, output.c_str());
            globalLock.Unlock();

            OpenClipboardGuard openClip(this);
            if (openClip.IsOpen())
            {
                EmptyClipboard();
                if (SetClipboardData(CF_TEXT, globalAlloc.Global))
                {
                    globalAlloc.RelinquishOwnership();
                }
            }
        }
    }
}

void OutputPane::OnDblClick()
{
    int iIndex = (int)SendMessage(LB_GETCURSEL, 0, 0);
    if (iIndex != LB_ERR)
    {
        CompileResult *pResult = (CompileResult*)SendMessage(LB_GETITEMDATA, iIndex, 0);
        if (pResult)
        {
            if (pResult->CanGotoScript())
            {
                ScriptId script = pResult->GetScript();
                appState->OpenScriptAtLine(script, pResult->GetLineNumber());
            }
            else if (pResult->GetResourceType() != ResourceType::None)
            {
                appState->OpenMostRecentResourceAt(pResult->GetResourceType(), (uint16_t)pResult->GetLineNumber(), pResult->GetColumn());
            }
        }
    }
}
