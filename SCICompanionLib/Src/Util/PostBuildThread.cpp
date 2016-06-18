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
#include "PostBuildThread.h"
#include "format.h"
#include "AppState.h"
#include "MainFrm.h"

using namespace std;

std::vector<std::string> split(const std::string& value, char separator);

shared_ptr<PostBuildThread> CreatePostBuildThread(const std::string &gameFolder)
{
    shared_ptr<PostBuildThread> postBuild = make_shared<PostBuildThread>(gameFolder);
    postBuild->_Start(postBuild);
    return postBuild;
}

UINT PostBuildThread::s_PostBuildThreadWorker(void *pParam)
{
    static_cast<PostBuildThread*>(pParam)->_Main();
    return 0;
}

string GetPostBuildFilename(const string &gameFolder)
{
    return gameFolder + "\\PostRepackage.cmd";
}

void PostBuildThread::Abort()
{
    SetEvent(_hAbort.hFile);
    // Hmm... not sure about this. Who is responsible for cleanup?
}

PostBuildThread::PostBuildThread(const string &gameFolder) : _gameFolder(gameFolder), _hwndUI(nullptr) {}

void PostBuildThread::_Start(std::shared_ptr<PostBuildThread> myself)
{
    if (PathFileExists(GetPostBuildFilename(_gameFolder).c_str()))
    {
        _hwndUI = AfxGetMainWnd()->GetSafeHwnd();
        _hAbort.hFile = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        _myself = myself;

        // REVIEW: Might want to move this further up the chain.
        appState->OutputClearResults(OutputPaneType::Compile);
        appState->ShowOutputPane(OutputPaneType::Compile);

        try
        {
            std::thread ourThread = std::thread(s_PostBuildThreadWorker, this);
            ourThread.detach();   // Thread deletes itself when done.
        }
        catch (std::system_error)
        {
            _hwndUI = nullptr;
            CloseHandle(_hAbort.hFile);
            _myself = nullptr;
        }
    }
}


void _GetStuff(HANDLE handle, HWND hwndUI)
{
    DWORD cbRead;
    char buffer[1024];

    // REVIEW: I'm pretty sure thing just hangs if no data.
    if (ReadFile(handle, buffer, sizeof(buffer) - 1, &cbRead, nullptr) && (cbRead != 0))
    {
        // Got something
        buffer[cbRead] = 0; // Ensure null terminated.
        if (hwndUI)
        {
            std::vector<std::string> lines = split(buffer, '\n');
            unique_ptr<vector<CompileResult>> results = make_unique<vector<CompileResult>>();
            for (const std::string &line : lines)
            {
                results->push_back(CompileResult(line));
            }
            SendMessage(hwndUI, UWM_RESULTS, (WPARAM)OutputPaneType::Compile, reinterpret_cast<LPARAM>(results.release()));
        }
    }
}

void PostBuildThread::_Main()
{
    // Own a reference to ourselves during the lifetime of this method
    shared_ptr<PostBuildThread> myself = _myself;
    _myself.reset();

    SECURITY_ATTRIBUTES saAttr = {};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;   // Inherit handles
    saAttr.lpSecurityDescriptor = nullptr;

    ScopedHandle childOutRead;
    ScopedHandle childOutWrite;
    if (CreatePipe(&childOutRead.hFile, &childOutWrite.hFile, &saAttr, 0))
    {
        // Each this is not inherited:
        SetHandleInformation(childOutRead.hFile, HANDLE_FLAG_INHERIT, 0);

        PROCESS_INFORMATION procInfo = {};
        STARTUPINFO startInfo = {};

        startInfo.cb = sizeof(startInfo);
        startInfo.hStdError = childOutWrite.hFile;
        startInfo.hStdOutput = childOutWrite.hFile;
        startInfo.hStdInput = nullptr; // Is it ok that we're not writing this? We don't care about stdin.
        startInfo.dwFlags |= STARTF_USESTDHANDLES;

        startInfo.wShowWindow = SW_HIDE;
        startInfo.dwFlags |= STARTF_USESHOWWINDOW;

        if (CreateProcess(
            GetPostBuildFilename(_gameFolder).c_str(),
            nullptr,    // cmd line
            nullptr,    // process sec attributes,
            nullptr,    // primary thread security attributes,
            TRUE,       // inherit handles
            0,          // creation flags,
            nullptr,    // use parent's environment
            nullptr,    // use parent's current directory
            &startInfo,
            &procInfo)
            )
        {
            if (_hwndUI)
            {
                unique_ptr<vector<CompileResult>> results = make_unique<vector<CompileResult>>();
                results->push_back(CompileResult("Running " + GetPostBuildFilename(_gameFolder)));
                SendMessage(_hwndUI, UWM_RESULTS, (WPARAM)OutputPaneType::Compile, reinterpret_cast<LPARAM>(results.release()));
            }
            DWORD waitResult;
            HANDLE waitHandles[2] = { procInfo.hProcess, _hAbort.hFile };
            // Go into a loop, checking for termination of the process or "abort", and reading from its standard out.
            do
            {
                _GetStuff(childOutRead.hFile, _hwndUI);
                waitResult = WaitForMultipleObjects(ARRAYSIZE(waitHandles), waitHandles, FALSE, 0);
            } while (waitResult == WAIT_TIMEOUT);
            // Any final stuff...
            _GetStuff(childOutRead.hFile, _hwndUI);

            bool wasAborted = (waitResult == (WAIT_OBJECT_0 + 1));

            if (_hwndUI)
            {
                unique_ptr<vector<CompileResult>> results = make_unique<vector<CompileResult>>();
                results->push_back(CompileResult(wasAborted ? "Aborted" : "Completed " + GetPostBuildFilename(_gameFolder)));
                SendMessage(_hwndUI, UWM_RESULTS, (WPARAM)OutputPaneType::Compile, reinterpret_cast<LPARAM>(results.release()));
            }

            CloseHandle(procInfo.hProcess);
            CloseHandle(procInfo.hThread);
        }
    }
}
