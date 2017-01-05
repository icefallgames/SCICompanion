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
// CompileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ScriptOM.h"
#include "NewCompileDialog.h"
#include "ScriptDocument.h"
#include <filesystem>
#include <regex>

using namespace std::tr2::sys;

#define UWM_STARTCOMPILE (WM_APP + 0)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CNewCompileDialog dialog

CNewCompileDialog::CNewCompileDialog(const std::unordered_set<std::string> &scriptsToRecompile, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(CNewCompileDialog::IDD, pParent), _headers(appState->GetResourceMap()), _scriptsToRecompile(scriptsToRecompile)
{
    _fResult = false;
    _fAbort = false;
    _fDone = false;
}

CNewCompileDialog::~CNewCompileDialog()
{
}

bool CNewCompileDialog::HasErrors()
{
    return _log.HasErrors();
}


LRESULT CNewCompileDialog::CompileAll(WPARAM wParam, LPARAM lParam)
{
    ShowWindow(SW_SHOW);

    m_wndProgress.SetPos(_nScript);

    int nSuccessfulResults = 0;

    // Clear out the results from previous compiles.
    _log.Clear();
    int scriptsSize = static_cast<int>(_scripts.size());
    ASSERT(_nScript < scriptsSize);

    ScriptId &scriptId = _scripts[_nScript];
    // Update the edit control with the current scripts name.
    m_wndDisplay.SetWindowText(scriptId.GetTitle().c_str());

    // Now pump some messages, so the display updates
    // Read all of the messages in this next loop, 
    // removing each message as we read it.
    MSG msg; 
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE | PM_QS_PAINT | PM_QS_INPUT)) 
    { 
        // If it is a quit message, exit.
        ASSERT(msg.message != WM_QUIT);

        //  return 1; 
        // Otherwise, dispatch the message.
        DispatchMessage(&msg); 
    } // End of PeekMessage while loop.
    if (_fAbort)
    {
        _fDone = true;
        OnCancel();
    }
    else
    {
        // Do a compile
        CompileResults results(_log);
        NewCompileScript(results, _log, _tables, _headers, scriptId);

        // The compile is done.  Post the results.
        appState->OutputAddBatch(OutputPaneType::Compile, _log.Results());
    }

    if (!_fAbort)
    {
        _nScript++;
        if (_nScript < (int)_scripts.size())
        {
            PostMessage(UWM_STARTCOMPILE, 0, 0); // Start another compile
        }
        else
        {
            // Change the text to close:
            SetDlgItemText(IDCANCEL, "Close");
            _fDone = true;
            // Actually, just close ourselves
            PostMessage(WM_CLOSE, 0, 0);
        }
    }
    return 0;
}

void CNewCompileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
    DDX_Control(pDX, IDC_EDIT, m_wndDisplay);

    // Visuals
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
}

BOOL CNewCompileDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    ShowSizeGrip(FALSE);
    try
    {
        _tables.Load(appState->GetVersion()); // REVIEW: clean up

        if (_scriptsToRecompile.empty())
        {
            // Everything
            appState->GetResourceMap().GetAllScripts(_scripts);
        }
        else
        {
            // Filtered
            std::vector<ScriptId> scriptsTemp;
            appState->GetResourceMap().GetAllScripts(scriptsTemp);
            std::copy_if(scriptsTemp.begin(), scriptsTemp.end(), std::back_inserter(_scripts),
                [&](const ScriptId &scriptId)
            {
                return _scriptsToRecompile.find(scriptId.GetTitleLower()) != _scriptsToRecompile.end();
            }
            );
        }

        if (_scripts.empty())
        {
            if (IDYES == AfxMessageBox("Error finding scripts to compile.\nDo you want to try scanning the src folder for scripts?", MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION))
            {
                path enumPath = appState->GetResourceMap().Helper().GetSrcFolder();
                std::vector<std::string> filenames;
                auto matchRSTRegex = std::regex("(\\w+)\\.sc$");
                for (auto it = directory_iterator(enumPath); it != directory_iterator(); ++it)
                {
                    const auto &file = it->path();
                    std::smatch sm;
                    std::string temp = file.filename().string();
                    if (!is_directory(file) && std::regex_search(temp, sm, matchRSTRegex) && (sm.size() > 1))
                    {
                        _scripts.push_back(ScriptId(file.string()));
                    }
                }
                if (_scripts.empty())
                {
                    AfxMessageBox("Could not find any .sc files.", MB_OK | MB_ICONERROR);
                }
            }
        }

        _nScript = 0;
        if (!_scripts.empty())
        {
            // Set the range of the progress control.
            m_wndProgress.SetRange32(0, (int)_scripts.size());
            PostMessage(UWM_STARTCOMPILE, 0, 0);
        }
        else
        {
            _fDone = true;
            // Actually, just close ourselves
            PostMessage(WM_CLOSE, 0, 0);
        }
    }
    catch (std::exception)
    {
        _fDone = true;
        // Actually, just close ourselves
        PostMessage(WM_CLOSE, 0, 0);
    }
    return fRet;
}

void CNewCompileDialog::OnDestroy()
{
    // Do some reporting.
    std::stringstream str;
    str << _nScript << " scripts compiled.";
    _log.ReportResult(str.str());
    appState->OutputAddBatch(OutputPaneType::Compile, _log.Results());

    _log.CalculateErrors();

    // Save any tables...
    _tables.Save();

    __super::OnDestroy();
}


void CNewCompileDialog::OnCancel()
{
    if (!_fDone)
    {
        // We're still doing stuff.  Signal ourself to close.
        _fAbort = TRUE;
    }
    else
    {
        __super::OnCancel();
    }
}

BEGIN_MESSAGE_MAP(CNewCompileDialog, CExtResizableDialog)
    ON_MESSAGE(UWM_STARTCOMPILE, CompileAll)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CNewCompileDialog message handlers
